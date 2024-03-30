#ifndef _ASP_ARITHMETIC_TREE2_HPP_
#define _ASP_ARITHMETIC_TREE2_HPP_

#include <variant>
#include <functional>

#include "arithmetic_object.hpp"
#include "arithmetic_process.hpp"

#include <string>
#include <unordered_map>

namespace asp {

struct arithmetic_tree;

typedef unsigned priority_type;
extern const std::unordered_map<operator_type, priority_type> _operator_priority;
// -1 left operand, 0 binary operand, 1 right operand
extern const std::unordered_map<operator_type, short> _operand_map;


struct arithmetic_tree {
    typedef arithmetic_tree self;
    typedef arithmetic_object node;

    arithmetic_tree() = default;
    arithmetic_tree(const self&) = delete;
    self& operator=(const self&) = delete;
    ~arithmetic_tree();

public: // operation
    void append_value(const value_type&);
    void append_operator(const operator_type&);
    void append_variable(const variable_type&);
    void append_tree(self&);
    void clear();
private: // operation
    void append_operand(node* const);
    priority_type get_priority(const operator_type&) const;
private: // examine
    bool is_operator(const node* const) const;
    bool is_operand(const node* const) const;
    bool lack_left_operand(const node* const) const;
    bool lack_right_operand(const node* const) const;


    struct node_header {
        node* _root = nullptr;
        bool _subtree = false;
    };
    node_header _head;
};

};

#endif // _ASP_ARITHMETIC_OBJECT_HPP_