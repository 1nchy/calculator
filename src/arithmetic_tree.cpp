#include "arithmetic_tree.hpp"

#include <cassert>
#include <type_traits>

#include <functional>
#include "variant_overloaded.hpp"

namespace asp {

const std::unordered_map<operator_type, priority_type>
_operator_priority = {
    {"+", 1}, {"-", 1}, {"*", 3}, {"/", 3},
    {"!", 127}, {"ln", 63}
};
const std::unordered_map<operator_type, short>
_operand_map = {
    {"+", 0}, {"-", 0}, {"*", 0}, {"/", 0},
    {"!", -1}, {"ln", 1}
};


arithmetic_tree::~arithmetic_tree() {
    if (!_head._subtree) {
        clear();
    }
}


void arithmetic_tree::append_value(const value_type& _val) {
    node* const _p = arithmetic_value_object::create(_val);
    append_operand(_p);
}
void arithmetic_tree::append_operator(const operator_type& _op) {
    node* const _p = arithmetic_operator_object::create(_op);
    assert(_head._root != nullptr);
    assert(_head._root->examine());
    const priority_type _pp = get_priority(_op);
    node* _s = _head._root;
    while (is_operator(_s)) {
        assert(_s->examine());
        const operator_type& _s_op = arithmetic_object::get_value(arithmetic_object::operator_tag(), *_s);
        const priority_type _s_p = get_priority(_s_op);
        if (_pp < _s_p) break;
        _s = _s->_right;
        assert(_s->examine());
    }
    // let %_s be the left child of %_p
    if (_s->_parent == nullptr) { // _s == _head._root;
        _p->_left = _s; _s->_parent = _p;
        _head._root = _p;
    }
    else {
        node* const _sp = _s->_parent;
        _p->_left = _s; _s->_parent = _p; _p->_parent = _sp;
        if (_sp->_left == _s) { _sp->_left = _p; }
        else if (_sp->_right == _s) { _sp->_right = _p; }
        else { assert(false); }
    }
}
void arithmetic_tree::append_variable(const variable_type& _var) {
    node* const _p = arithmetic_variable_object::create(_var);
    append_operand(_p);
}
void arithmetic_tree::append_tree(self& _tr) {
    assert(this != &_tr);
    node* const _p = _tr._head._root;
    append_operand(_p);
    _tr._head._subtree = true;
}
void arithmetic_tree::clear() {
    if (_head._subtree) return;
    std::function<void(node* const)> dfs = [&](node* const _p) {
        if (_p == nullptr) return;
        dfs(_p->_left); dfs(_p->_right);
        delete _p;
    };
    dfs(_head._root);
    _head._root = nullptr;
}


void arithmetic_tree::append_operand(node* const _p) {
    if (_head._root == nullptr) {
        _head._root = _p; return;
    }
    assert(_head._root->examine());
    node* _s = _head._root;
    while (_s != nullptr && !lack_right_operand(_s)) {
        _s = _s->_right;
        assert(is_operator(_s));
        assert(_s->examine());
    }
    assert(_s != nullptr);
    _s->_right = _p; _p->_parent = _s;
}


auto arithmetic_tree::get_priority(const operator_type& _op) const
-> priority_type {
    if (!_operator_priority.contains(_op)) return 0;
    return _operator_priority.at(_op);
};


bool arithmetic_tree::is_operator(const node* const _p) const {
    if (_p == nullptr) return false;
    return _p->type() == node::object_type::OT_OPERATOR;
}
bool arithmetic_tree::is_operand(const node* const _p) const {
    if (_p == nullptr) return false;
    return _p->type() == node::object_type::OT_INTEGER
        || _p->type() == node::object_type::OT_FLOAT
        || _p->type() == node::object_type::OT_VARIABLE;
}
bool arithmetic_tree::lack_left_operand(const node* const _p) const {
    assert(_p != nullptr);
    if (!is_operator(_p)) return false;
    const operator_type& _op = node::get_value(node::operator_tag(), *_p);
    assert(_operand_map.contains(_op));
    return _operand_map.at(_op) <= 0 && _p->_left == nullptr;
}
bool arithmetic_tree::lack_right_operand(const node* const _p) const {
    assert(_p != nullptr);
    if (!is_operator(_p)) return false;
    const operator_type& _op = node::get_value(node::operator_tag(), *_p);
    assert(_operand_map.contains(_op));
    return _operand_map.at(_op) >= 0 && _p->_right == nullptr;
}

};