/*
 * common.h
 *
 *  Created on: 15 Apr 2016
 *      Author: hbruintjes
 */

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <tuple>

namespace py = pybind11;

#if PY_MAJOR_VERSION >= 3
#define PY_DIV "__truediv__"
#define PY_RDIV "__rtruediv__"
#else
#define PY_DIV "__div__"
#define PY_RDIV "__rdiv__"
#endif

namespace pybind11 {
namespace detail {
/**
 * Dummy type caster for handle, so functions can return pybind11 handles directly
 */
template <> class type_caster<handle> {
public:
    bool load(handle src, bool) {
        value = handle(src).inc_ref();
        return true;
    }

    static handle cast(handle src, return_value_policy policy, handle parent) {
        switch(policy) {
            case return_value_policy::automatic:
            case return_value_policy::copy:
            case return_value_policy::take_ownership:
                return handle(src);
            case return_value_policy::reference:
                return handle(src).inc_ref();
            case return_value_policy::reference_internal:
                parent.inc_ref();
                return handle(src);
        }
        return handle(src);
    }
    PYBIND11_TYPE_CASTER(handle, _("handle"));
};
/*
template <typename TupleType, typename ... Keys> struct tuple_caster {
    typedef TupleType type;
    template<typename Type>
    using type_conv = type_caster<typename intrinsic_type<Type>::type>;

    bool load(handle src, bool convert) {
        pybind11::tuple tup(src, true);
        if (!tup.check())
            return false;

        return loadItem<0, Keys...>(tup, convert);
    }

    static handle cast(const type &src, return_value_policy policy, handle parent) {
        pybind11::tuple tup(sizeof...(Keys));
        if (!castItem<0, Keys...>(tup, src, policy, parent)) {
            return handle();
        }

        return tup.release();
    }

private:
    template<int N, typename Type, typename ... Types>
    bool loadItem(pybind11::tuple& tup, bool convert) {
        type_conv<Type> conv;
        if (!conv.load(tup[N], convert))
                return false;
        std::get<N>(value) = static_cast<Type>(conv);
        return loadItem<N+1, Types...>(tup, convert);
    }

    template<int N, typename Type>
    bool loadItem(pybind11::tuple& tup, bool convert) {
        type_conv<Type> conv;
        if (!conv.load(tup[N], convert))
                return false;
        std::get<N>(value) = static_cast<Type>(conv);
        return true;
    }

    template<int N, typename Type, typename ... Types>
    static bool castItem(pybind11::tuple& tup, const type &src, return_value_policy policy, handle parent) {
        auto obj = type_conv<Type>::cast(std::get<N>(src), policy, parent);
        object value_ = object(obj, false);
        if (!obj) {
            return false;
        }
        return castItem<N+1, Types...>(tup, src, policy, parent);
    }

    template<int N, typename Type>
    static bool castItem(pybind11::tuple& tu, const type &src, return_value_policy policy, handle parent) {
        auto obj = type_conv<Type>::cast(std::get<N>(src), policy, parent);
        object value_ = object(obj, false);
        if (!obj) {
            return false;
        }
        return true;
    }
};

template <typename ... Types> struct type_caster<std::tuple<Types...>>
 : tuple_caster<std::tuple<Types...>, Types...> { };

template <typename Type1, typename Type2> struct type_caster<std::pair<Type1, Type2>>
 : tuple_caster<std::pair<Type1, Type2>, Type1, Type2> { };
*/

}
}
