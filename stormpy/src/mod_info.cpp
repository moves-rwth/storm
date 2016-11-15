#include "common.h"
#include "helpers.h"

#include <src/utility/storm-version.h>

PYBIND11_PLUGIN(info) {
	py::module m("info", "Storm information");
    py::class_<storm::utility::StormVersion>(m, "Version", "Version information for Storm")
        .def_property_readonly("short", &storm::utility::StormVersion::shortVersionString, "Storm version in short representation")
        .def_property_readonly("long", &storm::utility::StormVersion::longVersionString, "Storm version in long representation")
        .def_property_readonly("build_info", &storm::utility::StormVersion::buildInfo, "Build info for Storm")
    ;
	return m.ptr();
}
