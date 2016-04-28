#include "common.h"

#include "core/core.h"

PYBIND11_PLUGIN(core) {
    py::module m("core");
    define_core(m);
    return m.ptr();
}
