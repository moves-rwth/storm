#include "common.h"

#include "logic/formulae.h"

PYBIND11_PLUGIN(logic) {
    py::module m("stormpy.logic", "Logic module for Storm");
    define_formulae(m);
    return m.ptr();
}
