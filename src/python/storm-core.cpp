#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "../utility/storm.h"
#include "../logic/Formulas.h"


namespace boost {
    template<class T> T* get_pointer(std::shared_ptr<T> p) { return p.get(); }
}
BOOST_PYTHON_MODULE(_core)
{
    using namespace boost::python;
    def("setUp", storm::utility::setUp);
    
    class_<storm::logic::Formula, std::shared_ptr<storm::logic::Formula>, boost::noncopyable>("Formula", no_init)
        .def("toString", &storm::logic::Formula::toString);
	class_<std::vector<std::shared_ptr<storm::logic::Formula>>>("FormulaVec")
			.def(vector_indexing_suite<std::vector<std::shared_ptr<storm::logic::Formula>>, true>())
			;


	class_<storm::logic::ProbabilityOperatorFormula, std::shared_ptr<storm::logic::ProbabilityOperatorFormula>, bases<storm::logic::Formula>>("ProbabilityOperatorFormula", no_init)
        .def("toString", &storm::logic::ProbabilityOperatorFormula::toString);
    class_<storm::prism::Program>("Program")
        .def("getNumberOfModules", &storm::prism::Program::getNumberOfModules)
    ;



    def("parseFormulae", storm::parseFormulasForProgram);
    def("parseProgram", storm::parseProgram);

    def("buildAndCheck", storm::buildAndCheckSymbolicModel<double>);
    
    
    
}