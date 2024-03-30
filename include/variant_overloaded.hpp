#ifndef _ASP_VARIANT_OVERLOADED_HPP_
#define _ASP_VARIANT_OVERLOADED_HPP_

namespace asp {

template <typename... _Args> struct overloaded : _Args... {
    overloaded(_Args&&... _args) : _Args(_args)... {}
    using _Args::operator()...;
};
// template <typename... _Args> overloaded(_Args...) -> overloaded<_Args...>;

};

#endif // _ASP_VARIANT_OVERLOADED_HPP_