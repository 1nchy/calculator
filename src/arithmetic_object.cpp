#include "arithmetic_object.hpp"

#include <cassert>

#include "variant_overloaded.hpp"

namespace asp {

auto arithmetic_object::get_value(arithmetic_object::integer_tag, const arithmetic_object& _obj)
-> Integer {
    return dynamic_cast<const arithmetic_integer_object&>(_obj)._val;
}
auto arithmetic_object::get_value(arithmetic_object::float_tag, const arithmetic_object& _obj)
-> Float {
    return dynamic_cast<const arithmetic_float_object&>(_obj)._val;
}
auto arithmetic_object::get_value(arithmetic_object::variable_tag, const arithmetic_object& _obj)
-> value_type {
    return dynamic_cast<const arithmetic_variable_object&>(_obj).get_value();
}
auto arithmetic_object::get_value(arithmetic_object::operator_tag, const arithmetic_object& _obj)
-> const operator_type& {
    return dynamic_cast<const arithmetic_operator_object&>(_obj)._op;
}


auto arithmetic_operator_object::examine() const
-> bool { return this->_left != nullptr; }
auto arithmetic_operand_object::examine() const
-> bool { return this->_left == nullptr && this->_right == nullptr; }


auto arithmetic_operator_object::create(const operator_type& _op)
-> self* {
    return new arithmetic_operator_object(_op);
}
auto arithmetic_value_object::create(const value_type& _val)
-> self* {
    self* _result = nullptr;
    std::visit(overloaded {
        [&_result](const Integer& _v) { _result = new arithmetic_integer_object(_v); },
        [&_result](const Float& _v) { _result = new arithmetic_float_object(_v); },
        [&_result](const auto& _v) {},
    }, _val);
    assert(_result != nullptr);
    return _result;
}
auto arithmetic_variable_object::create(const variable_type& _var)
-> self* {
    return new arithmetic_variable_object(_var);
}

};