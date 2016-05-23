#include "common.h"

#include "core/core.h"
#include "core/modelchecking.h"
#include "core/bisimulation.h"
#include "core/prism.h"

PYBIND11_PLUGIN(core) {
    py::module m("core");
    define_core(m);
    define_parse(m);
    define_build(m);
    define_modelchecking(m);
    define_bisimulation(m);
    define_prism(m);
    return m.ptr();
}
