#pragma once
#include <boost/python.hpp>


namespace boost { namespace python { namespace converter {

    template <class T>
    PyObject* shared_ptr_to_python(std::shared_ptr<T> const& x)
    {
        if (!x)
            return python::detail::none();
        else if (shared_ptr_deleter* d = std::get_deleter<shared_ptr_deleter>(x))
            return incref( d->owner.get() );
        else
            return converter::registered<std::shared_ptr<T> const&>::converters.to_python(&x);
    }

    /// @brief Adapter a non-member function that returns a unique_ptr to
    ///        a python function object that returns a raw pointer but
    ///        explicitly passes ownership to Python.
    template<typename T, typename ...Args>
    object adapt_unique(std::unique_ptr<T> (*fn)(Args...))
    {
        return make_function(
            [fn](Args... args) { return fn(args...).release(); },
            return_value_policy<manage_new_object>(),
            boost::mpl::vector<T*, Args...>()
        );
    }

    /// @brief Adapter a member function that returns a unique_ptr to
    ///        a python function object that returns a raw pointer but
    ///        explicitly passes ownership to Python.
    template<typename T, typename C, typename ...Args>
    object adapt_unique(std::unique_ptr<T> (C::*fn)(Args...))
    {
        return make_function(
            [fn](C& self, Args... args) { return (self.*fn)(args...).release(); },
            python::return_value_policy<manage_new_object>(),
            boost::mpl::vector<T*, C&, Args...>()
        );
    }

}}} // namespace boost::python::converter
