#pragma once

#include <boost/python.hpp>

template<typename Source, typename Target>
void shared_ptr_implicitly_convertible() {
    boost::python::implicitly_convertible<std::shared_ptr<Source>, std::shared_ptr<Target>>();
}


template<typename T>
void register_shared_ptr() {
    boost::python::register_ptr_to_python<std::shared_ptr<T>>();
}



namespace dtl{
// primary class template
    template<typename S, typename T, typename Enable = void>
    struct ImplConversionSharedPtr {
        void c() { shared_ptr_implicitly_convertible<S, T>(); }
    };

// specialized class template
    template<typename S, typename T>
    struct ImplConversionSharedPtr<S, T, typename std::enable_if<std::is_same<T, void>::value>::type> {
        void c() { }
    };

    template<typename B>
    struct bases_holder {
        typedef boost::python::bases<B> Type;
    };

    template<>
    struct bases_holder<void> {
        typedef boost::python::bases<> Type;
    };
}

template<typename C, typename B=void, typename NC=void>
boost::python::class_<C, std::shared_ptr<C>, typename dtl::bases_holder<B>::Type, NC> defineClass(char const* name, char const* docstring, typename  std::enable_if_t<std::is_default_constructible<C>::value>::type* = 0) {
    auto inst = boost::python::class_<C, std::shared_ptr<C>, typename dtl::bases_holder<B>::Type, NC>(name, docstring);
    register_shared_ptr<C>();
    dtl::ImplConversionSharedPtr<C,B>().c();
    return inst;
};

template<typename C, typename B=void, typename NC=void>
boost::python::class_<C, std::shared_ptr<C>, typename dtl::bases_holder<B>::Type, NC> defineClass(char const* name, char const* docstring) {
    auto inst = boost::python::class_<C, std::shared_ptr<C>, typename dtl::bases_holder<B>::Type, NC>(name, docstring, boost::python::no_init);
    register_shared_ptr<C>();
    dtl::ImplConversionSharedPtr<C,B>().c();
    return inst;
};
