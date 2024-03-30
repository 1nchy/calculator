#include "backend.hpp"

#include <sys/wait.h>

#include <chrono>
#include <thread>

#include <unistd.h>
#include <cassert>

#include "signal_stack/signal_stack.hpp"
#include "output/output.hpp"

extern char** environ;

namespace asp {

static const std::string _empty_string;
static signal_stack _signal_stack;
static calculator_backend* _instance_pointer = nullptr;
static void shell_sigchld_handler(int _sig) {
    assert(_instance_pointer != nullptr);
    _instance_pointer->sigchld_handler(_sig);
}
static void shell_sigtstp_handler(int _sig) {
    assert(_instance_pointer != nullptr);
    _instance_pointer->sigtstp_handler(_sig);
}
static void shell_sigint_handler(int _sig) {
    assert(_instance_pointer != nullptr);
    _instance_pointer->sigint_handler(_sig);
}
static void shell_sigpipe_handler(int _sig) {
    assert(_instance_pointer != nullptr);
}
static void shell_sigttin_handler(int _sig) {
    assert(_instance_pointer != nullptr);
    _instance_pointer->sigttin_handler(_sig);
}
static void shell_sigttou_handler(int _sig) {
    assert(_instance_pointer != nullptr);
    _instance_pointer->sigttou_handler(_sig);
}


calculator_backend::calculator_backend(int _in, int _out, int _err)
: backend_interface(_in, _out, _err) {
    output::set_output_level(output::fatal);
    output_debug("calculator_backend::constructor\n");
    _instance_pointer = this;

    dup2(_in, STDIN_FILENO);
    dup2(_out, STDOUT_FILENO);
    dup2(_err, STDERR_FILENO);

    _signal_stack.build(SIGCHLD, shell_sigchld_handler);
    _signal_stack.build(SIGTSTP, shell_sigtstp_handler);
    _signal_stack.build(SIGINT, shell_sigint_handler);
    _signal_stack.build(SIGTTIN, SIG_IGN);
    _signal_stack.build(SIGTTOU, SIG_IGN);

    clear();
}
calculator_backend& calculator_backend::singleton(int _in, int _out, int _err) {
    static calculator_backend _instance(_in, _out, _err);
    return _instance;
}
calculator_backend::~calculator_backend() {
    output_debug("calculator_backend::destructor\n");
}
int calculator_backend::parse(const std::string& _line) {
    // if (_line.empty()) return 0;
    size_t _i = 0;
    while (_parse_status != parse_status::eof) {
        // handle parsing status
    if (_parse_status == parse_status::backslash) {
        if (_i == _line.size()) {
            _parse_status = parse_status::backslash;
            return 1;
        }
        _parse_status = parse_status::parsing;
        _word.push_back(_line[_i]); ++_i;
    }
    else if (_parse_status == parse_status::parentheses) {}
    else if (_parse_status == parse_status::brackets) {}
    else if (_parse_status == parse_status::braces) {}
    
        // skip blank between words
    while (_i != _line.size() && _line[_i] == ' ') { ++_i; }
    if (_i == _line.size()) {
        if (!end_of_line()) {
            return 1;
        }
        continue;
    }
        // build one word in a cycle at least
    for (; _i < _line.size(); ++_i) {
        const auto& _c = _line[_i];
        if (_c == '\\') {
            _word.push_back(_c); ++_i;
            if (_i == _line.size() || _line[_i] == '\n' || _line[_i] == '\r') {
                _parse_status = parse_status::backslash;
                return 1;
            }
            _word.push_back(_line[_i]);
        }
        // else if (_c == '(') {}
        // else if (_c == '[') {}
        // else if (_c == '{') {}
        else if (_c == ' ') {
            // _commands.build_word(_word); _word.clear();
            break;
        }
        else if (_c == '\n' || _c == '\r') {
            if (!end_of_line()) return 1;
            break;
        }
        else { // character that should be read
    // size_t _l = _parse_symbol_dict.longest_match(_line.cbegin() + _i, _line.cend());
    // if (_l == 0) { // normal character
    //     _word.push_back(_c);
    //     continue;
    // }
    //     // special character
    // _commands.build_word(_word); _word.clear();
    // _relation = _line.substr(_i, _l); _i += _l;
    // if (pipe_symbol(_relation)) {
    //     _commands.build_instruction();
    //     _commands.need_further_input();
    //     break;
    // }
    // else if (join_symbol(_relation)) {
    //     _commands.build_instruction();
    //     _commands.build_command(_relation);
    //     _commands.need_further_input();
    //     break;
    // }
    // else if (redirect_symbol(_relation)) {
    //     _commands.prepare_redirection(_relation);
    //     _commands.need_further_input();
    //     break;
    // }
    // else if (background_symbol(_relation)) {
    //     _relation.clear();
    //     _commands.build_instruction();
    //     _commands.build_command(_relation, true);
    //     break;
    // }
    }}}
    return 0;
}
bool calculator_backend::compile() {
    if (_parse_status != parse_status::eof) {
        clear(); return false;
    }
    // for (auto& _cmd : _commands._parsed_command) {
    //     for (auto& _ins : _cmd._instructions) {
    //         for (auto& _w : _ins) {
    //             if (!compile_word(_w)) return false;
    //         }
    //     }
    // }
    return true;
}
bool calculator_backend::execute() {
    // for (size_t _i = 0; _i < _commands.size(); ++_i) {
    //     execute_command(_commands._parsed_command[_i]);
    // }
    clear();
    return true;
}
const std::string calculator_backend::build_information() {
    std::string _info;
    _info.push_back('[');
    _info.push_back(']');
    return _info;
}
const std::string calculator_backend::build_tab_next(const std::string& _line) {
    parse_tab(_line);
    trie_tree _tab_next_dict;
    if (tab_type_check(operation, _word2bc_type)) {
        const auto& _operator_tab_next = build_operator_tab_next(_word_2bc);
        if (_operator_tab_next.empty()) return "";
        _tab_next_dict.add(_operator_tab_next);
    }
    if (tab_type_check(number, _word2bc_type)) {
        const auto& _number_tab_next = build_number_tab_next(_word_2bc);
        if (_number_tab_next.empty()) return "";
        _tab_next_dict.add(_number_tab_next);
    }
    return _tab_next_dict.next("");
}
const std::vector<std::string> calculator_backend::build_tab_list(const std::string& _line) {
    parse_tab(_line);
    // tab classification
    std::vector<std::string> _tab_list;
    if (tab_type_check(operation, _word2bc_type)) {
        const auto& _operator_tab_list = build_operator_tab_list(_word_2bc);
        _tab_list.insert(_tab_list.end(), _operator_tab_list.cbegin(), _operator_tab_list.cend());
    }
    if (tab_type_check(number, _word2bc_type)) {
        const auto& _number_tab_list = build_number_tab_list(_word_2bc);
        _tab_list.insert(_tab_list.end(), _number_tab_list.cbegin(), _number_tab_list.cend());
    }
    return _tab_list;
}
const std::string& calculator_backend::prev_history() {
    if (_history.empty()) {
        return _empty_string;
    }
    if (_history_iterator != _history.cbegin()) {
        --_history_iterator;
    }
    return *_history_iterator;
}
const std::string& calculator_backend::next_history() {
    if (_history.empty() || _history_iterator == _history.cend() || _history_iterator == _history.cend() - 1) {
        _history_iterator = _history.cend();
        return _empty_string;
    }
    ++_history_iterator;
    return *_history_iterator;
}
void calculator_backend::append_history(const std::string& _s) {
    if (_s.empty()) return;
    _history.push_back(_s);
    _history_iterator = _history.cend();
}


void calculator_backend::sigchld_handler(int) {}
void calculator_backend::sigtstp_handler(int) {}
void calculator_backend::sigint_handler(int) {}
void calculator_backend::sigpipe_handler(int) {}
void calculator_backend::sigttin_handler(int) {
    output_debug("receive sigttin\n");
}
void calculator_backend::sigttou_handler(int) {
    output_debug("receive sigttou\n");
}


void calculator_backend::clear() {
    // _commands.clear();
    _parse_status = parse_status::parsing;
    _word.clear();
    _history_iterator = _history.cend();
}


bool calculator_backend::end_of_line(parse_status _s) {
    // _commands.build_word(_word); _word.clear();
    // if (_commands.further_input()) {
    //     // _line_parsed = false;
    //     _parse_status = _s;
    //     return false;
    // }
    // else {
    //     _commands.build_instruction();
    //     _commands.build_command(_relation);
    //     // _line_parsed = true;
    //     _parse_status = parse_status::eof;
    //     return true;
    // }
    return true;
}
bool calculator_backend::compile_word(std::string& _s) const {
    // supposed that %_s meets grammer rule
    for (size_t _i = 0; _i < _s.size();) {
        const auto _c = _s[_i];
        if (_c == '\\') {
            _s.erase(_s.cbegin() + _i);
        }
        else if (_c == '\'') {
            _s.erase(_s.cbegin() + _i);
            while (_i != _s.size() && _s[_i] != '\'') ++_i;
            if (_i != _s.size()) { // _s[_i] == '\''
                _s.erase(_s.cbegin() + _i);
            }
        }
        // else if (_c == '\"') {}
        // else if (_c == '`') {}
        else if (_c == '$') {
            // expand env variable
            // const size_t _l = variable_length(_s, _i + 1);
            // if (_l != 0) {
            //     const auto& _ev = variable(_s.substr(_i + 1, _l));
            //     if (_ev.first) {
            //         _s.erase(_s.cbegin() + _i, _s.cbegin() + _i + 1 + _l);
            //         _s.insert(_i, _ev.second);
            //         _i += _ev.second.size();
            //         continue;
            //     }
            // }
        }
        ++_i;
    }
    return true;
}


bool calculator_backend::env_symbol(const std::string& _r) const {
    return _r == "$";
}
void calculator_backend::parse_tab(const std::string& _line) {
    _word_2bc.clear(); _word2bc_type = tab_type::none;
    if (_line.empty()) {
        _word2bc_type = tab_type::none; return;
    }
    auto _tab_parse_status = parse_status::parsing; size_t _i = 0; 
    // tab parse
    while (_tab_parse_status != parse_status::eof) {
        // skip blank between words
        while (_i != _line.size() && _line[_i] == ' ') { ++_i; }
        if (_i == _line.size()) break;
        // process one word in a cycle at least
        for (; _i != _line.size(); ++_i) {
            const auto& _c = _line[_i];
            if (_c == '\\') {
                ++_i;
                if (_i == _line.size()) {
                    _tab_parse_status = parse_status::eof; break;
                }
                _word_2bc.push_back(_line[_i]);
            }
            // else if (_c == '(') {}
            // else if (_c == '[') {}
            // else if (_c == '{') {}
            else if (_c == ' ') {
                _word_2bc.clear(); _word2bc_type = tab_type::none; break;
            }
            else if (_c == '$') {
                _word_2bc.clear(); _word2bc_type = tab_type::variable;
            }
            else {
                // size_t _l = _tab_symbol_dict.longest_match(_line.cbegin() + _i, _line.cend());
                // if (_l == 0) { // normal character
                //     _word_2bc.push_back(_c);
                //     continue;
                // }
                // _word_2bc.clear(); _word2bc_type = tab_type::file;
                // const std::string _symbol = _line.substr(_i, _l); _i += _l;
                // if (pipe_symbol(_symbol)) {
                //     _word2bc_type = tab_type::program;
                // }
                // else if (join_symbol(_symbol)) {
                //     _word2bc_type = tab_type::program;
                // }
                // else if (redirect_symbol(_symbol)) {
                //     _word2bc_type = tab_type::file;
                // }
                // else if (env_symbol(_symbol)) {
                //     _word2bc_type = tab_type::env;
                // }
            }
        }
    }
}
std::string calculator_backend::build_operator_tab_next(const std::string& _s) {
    return _operator_dict.next(_s);
}
std::string calculator_backend::build_number_tab_next(const std::string& _s) {
    return _number_dict.next(_s);
}
std::string calculator_backend::build_variable_tab_next(const std::string& _s) {
    return _variable_dict.next(_s);
}
std::vector<std::string> calculator_backend::build_operator_tab_list(const std::string& _s) {
    return _operator_dict.tab(_s);
}
std::vector<std::string> calculator_backend::build_number_tab_list(const std::string& _s) {
    return _number_dict.tab(_s);
}
std::vector<std::string> calculator_backend::build_variable_tab_list(const std::string& _s) {
    return _variable_dict.tab(_s);
}
void calculator_backend::fetch_operator_dict() {}
void calculator_backend::fetch_number_dict() {}
void calculator_backend::fetch_variable_dict() {}


void calculator_backend::load_history() {
    // todo
    _history_iterator = _history.cend();
}
};