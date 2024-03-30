#ifndef _ASP_ARITHMETIC_PROCESS_HPP_
#define _ASP_ARITHMETIC_PROCESS_HPP_

#include <variant>
#include <functional>

#include <string>
#include <unordered_map>

namespace asp {
using None = std::monostate;
using Integer = long;
using Float = double;
using String = std::string;
using operator_type = String;
using variable_type = String;
typedef std::variant<None, Integer, Float> value_type;

typedef std::function<value_type(const value_type&, const value_type&)> operator_handler;
extern const std::unordered_map<operator_type, operator_handler> _operator_handler_map;

};

#endif // _ASP_ARITHMETIC_PROCESS_HPP_