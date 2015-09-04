#include <boost/python.hpp>
#include "../utility/storm-version.h"

BOOST_PYTHON_MODULE(_info)
{
    using namespace boost::python;
    class_<storm::utility::StormVersion>("Version")
            .def("short", &storm::utility::StormVersion::shortVersionString)
            .def("long", &storm::utility::StormVersion::longVersionString)
    ;
}