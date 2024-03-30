#ifndef _ASP_BACKEND_HPP_
#define _ASP_BACKEND_HPP_

#include "virtual_backend.hpp"

#include <map>
#include <unordered_map>

#include "arithmetic_tree.hpp"

#include "trie_tree/trie_tree.hpp"

namespace asp {

class calculator_backend;

class calculator_backend : public backend_interface {
private:
    calculator_backend(int _in = STDIN_FILENO, int _out = STDOUT_FILENO, int _err = STDERR_FILENO);
public:
    static calculator_backend& singleton(int _in = STDIN_FILENO, int _out = STDOUT_FILENO, int _err = STDERR_FILENO);
    virtual ~calculator_backend();
    /**
     * @brief parse command line
     * @return -1 error, 0 normal, 1 need futher input
    */
    int parse(const std::string&) override;
    bool compile() override;
    bool execute() override;
    const std::string build_information() override;
    const std::string build_tab_next(const std::string&) override;
    const std::vector<std::string> build_tab_list(const std::string&) override;
    const std::string& prev_history() override;
    const std::string& next_history() override;
    void append_history(const std::string&) override;
public: // signal
    void sigchld_handler(int);
    void sigtstp_handler(int);
    void sigint_handler(int);
    void sigpipe_handler(int);
    void sigttin_handler(int);
    void sigttou_handler(int);
private: // job controller
    void clear();
private: // command parsing
    enum parse_status {
        parsing,
        eof,
        backslash,
        parentheses,    // ()
        brackets,       // []
        braces,         // {}
    };
    bool end_of_line(parse_status _s = parse_status::parsing);
    bool compile_word(std::string&) const;
    parse_status _parse_status = parse_status::parsing;
    const trie_tree _parse_symbol_dict = {};
    std::string _word;
private: // tab
    enum tab_type : short { // bitmap
        none = 0x0,
        operation = 0x1 << 0,
        number = 0x1 << 1,
        variable = 0x1 << 2,
    };
    bool env_symbol(const std::string&) const;
    #define tab_type_check(_type, _t) ((tab_type::_type) & (_t))
    const trie_tree _tab_symbol_dict = {
        ">", ">>", "<", /*"<<",*/ ";", "|", "||", "&", "&&", "$",
    };
    std::string _word_2bc;
    short _word2bc_type = tab_type::none;
    trie_tree _operator_dict;
    trie_tree _number_dict;
    trie_tree _variable_dict;
    void parse_tab(const std::string& _line);
    std::string build_operator_tab_next(const std::string&);
    std::string build_number_tab_next(const std::string&);
    std::string build_variable_tab_next(const std::string&);
    std::vector<std::string> build_operator_tab_list(const std::string&);
    std::vector<std::string> build_number_tab_list(const std::string&);
    std::vector<std::string> build_variable_tab_list(const std::string&);
    void fetch_operator_dict();
    void fetch_number_dict();
    void fetch_variable_dict();
private: // variables
    // std::unordered_map<std::string, std::string> _variable_map;
    // // ~~trie_tree _var_dict~~;
    // void init_variable_map();
    // void add_variable(const std::string&, const std::string&);
    // void del_variable(const std::string&);
    // size_t variable_length(const std::string&, size_t) const;
    // std::pair<bool, const std::string&> variable(const std::string&) const;
private: // history
    void load_history();
    std::vector<std::string> _history;
    typename decltype(_history)::const_iterator _history_iterator;
};

};

#endif // _ASP_BACKEND_HPP_