
#include <boost/python.hpp>
#include "../utility/storm.h"

BOOST_PYTHON_MODULE(_core)
{
    using namespace boost::python;
    def("setUp", storm::utility::setUp);
    
    class_<storm::prism::Program>("Program")
        .def("getNumberOfModules", &storm::prism::Program::getNumberOfModules)
    ;
    
    def("parseProgram", storm::parseProgram);
    
}