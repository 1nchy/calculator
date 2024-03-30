#include "arithmetic_tree.hpp"

int main(void) {
    // asp::calculator_frontend::singleton().activate();
    // asp::calculator_frontend::singleton().deactivate();
    asp::arithmetic_tree _atr;
    // asp::arithmetic_tree::value_type _val;
    // asp::arithmetic_tree::operator_type _op;
    // _val = 3l; _atr.append_value(_val);
    _atr.append_operator("ln");
    _atr.append_value(3l);
    _atr.append_operator("+");
    _atr.append_value(4l);
    _atr.append_operator("!");
    _atr.append_operator("*");
    _atr.append_value(5.5);
    _atr.clear();
    return 0;
}