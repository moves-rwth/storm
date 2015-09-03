#include <boost/python.hpp>
#include "../utility/storm-version.h"

BOOST_PYTHON_MODULE(libstormpy)
{
    using namespace boost::python;
    class_<storm::utility::StormVersion>("Version")
            .def("short", &storm::utility::StormVersion::shortVersionString)
    ;
}