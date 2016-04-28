#include "common.h"

#include "logic/formulas.h"

PYBIND11_PLUGIN(logic) {
    py::module m("stormpy.logic", "Logic module for Storm");
    define_formulas(m);
    return m.ptr();
}
