#include "common.h"
#include "helpers.h"

#include "info/info.h"

PYBIND11_PLUGIN(info) {
	py::module m("stormpy.info", "stormpy info handling");
	define_info(m);
	return m.ptr();
}
