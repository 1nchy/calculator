#ifndef _ASP_ARITHMETIC_OBJECT_HPP_
#define _ASP_ARITHMETIC_OBJECT_HPP_

#include <variant>
#include <functional>

#include "arithmetic_process.hpp"

#include <string>
#include <unordered_map>

namespace asp {

struct arithmetic_object;

struct arithmetic_integer_object;
struct arithmetic_float_object;
struct arithmetic_value_object;
struct arithmetic_variable_object;
struct arithmetic_operand_object;
struct arithmetic_operator_object;

typedef unsigned priority_type;

extern const std::unordered_map<operator_type, priority_type> _operator_priority;
// -1 left operand, 0 binary operand, 1 right operand
extern const std::unordered_map<operator_type, short> _operand_map;

/**
 * @brief node in arithmetic tree
 * @details
 *   arithmetic_object _obj;
 *   if (_obj.type())
*/
struct arithmetic_object {
    struct object_tag {};
    struct operand_tag {};
    struct operator_tag {};
    struct value_tag : operand_tag {};
    struct variable_tag : operator_tag {};
    struct integer_tag : value_tag {};
    struct float_tag : value_tag {};

    enum object_type {
        OT_INTEGER,
        OT_FLOAT,
        OT_VARIABLE,
        OT_OPERATOR,
    };

    typedef arithmetic_object self;

    arithmetic_object() = default;
    arithmetic_object(const self&) = delete;
    self& operator=(const self&) = delete;
    ~arithmetic_object() = default;
    virtual bool examine() const = 0;
    virtual object_type type() const = 0;

    static Integer get_value(integer_tag, const self&);
    static Float get_value(float_tag, const self&);
    static value_type get_value(variable_tag, const self&);
    static const operator_type& get_value(operator_tag, const self&);

    self* _left = nullptr; self* _right = nullptr; self* _parent = nullptr;
};


struct arithmetic_operand_object : arithmetic_object {
    typedef arithmetic_object::operand_tag object_tag;
    typedef arithmetic_operand_object self;
    arithmetic_operand_object() = default;
    arithmetic_operand_object(const self&) = delete;
    self& operator=(const self&) = delete;
    ~arithmetic_operand_object() = default;
    bool examine() const override;
};
struct arithmetic_operator_object : arithmetic_object {
    typedef arithmetic_object::operator_tag object_tag;
    typedef arithmetic_operator_object self;
    using object_type = arithmetic_object::object_type;
    arithmetic_operator_object(const operator_type& _v) : _op(_v) {}
    arithmetic_operator_object(const self&) = delete;
    self& operator=(const self&) = delete;
    ~arithmetic_operator_object() = default;
    bool examine() const override;
    static self* create(const operator_type&);
    object_type type() const override { return object_type::OT_OPERATOR; }
    const operator_type _op;
};
struct arithmetic_value_object : arithmetic_operand_object {
    typedef arithmetic_object::value_tag object_tag;
    typedef arithmetic_value_object self;
    arithmetic_value_object() = default;
    arithmetic_value_object(const self&) = delete;
    self& operator=(const self&) = delete;
    ~arithmetic_value_object() = default;
    static self* create(const value_type&);
};
struct arithmetic_variable_object : arithmetic_operand_object {
    typedef arithmetic_object::variable_tag object_tag;
    typedef arithmetic_variable_object self;
    using object_type = arithmetic_object::object_type;
    arithmetic_variable_object(const variable_type& _v) : _var(_v) {}
    arithmetic_variable_object(const self&) = delete;
    self& operator=(const self&) = delete;
    ~arithmetic_variable_object() = default;
    // void instantiable() const override {};
    static self* create(const variable_type&);
    value_type get_value() const { return 0l; }
    object_type type() const override { return object_type::OT_VARIABLE; }
    const variable_type _var;
};
struct arithmetic_integer_object : arithmetic_value_object {
    typedef arithmetic_object::integer_tag object_tag;
    typedef arithmetic_integer_object self;
    using object_type = arithmetic_object::object_type;
    arithmetic_integer_object(const Integer& _v) : _val(_v) {}
    arithmetic_integer_object(const self&) = delete;
    self& operator=(const self&) = delete;
    ~arithmetic_integer_object() = default;
    object_type type() const override { return object_type::OT_INTEGER; }
    const Integer _val;
};
struct arithmetic_float_object : arithmetic_value_object {
    typedef arithmetic_object::float_tag object_tag;
    typedef arithmetic_float_object self;
    using object_type = arithmetic_object::object_type;
    arithmetic_float_object(const Float& _v) : _val(_v) {}
    arithmetic_float_object(const self&) = delete;
    self& operator=(const self&) = delete;
    ~arithmetic_float_object() = default;
    object_type type() const override { return object_type::OT_FLOAT; }
    const Float _val;
};

};

#endif // _ASP_ARITHMETIC_OBJECT_HPP_