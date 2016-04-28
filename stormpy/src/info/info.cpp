#include "info.h"

#include "common.h"
#include <src/utility/storm-version.h>

void define_info(py::module& m) {
    py::class_<storm::utility::StormVersion>(m, "Version")

        .def("short", &storm::utility::StormVersion::shortVersionString)
        .def("long", &storm::utility::StormVersion::longVersionString)
        .def("build_info", &storm::utility::StormVersion::buildInfo)
    ;
}
