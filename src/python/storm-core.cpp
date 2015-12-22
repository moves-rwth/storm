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

    ////////////////////////////////////////////
    // Formula
    ////////////////////////////////////////////
    class_<storm::logic::Formula, std::shared_ptr<storm::logic::Formula>, boost::noncopyable>("Formula", no_init)
        .def("toString", &storm::logic::Formula::toString);
	class_<std::vector<std::shared_ptr<storm::logic::Formula>>>("FormulaVec")
			.def(vector_indexing_suite<std::vector<std::shared_ptr<storm::logic::Formula>>, true>())
			;
    class_<storm::logic::ProbabilityOperatorFormula, std::shared_ptr<storm::logic::ProbabilityOperatorFormula>, bases<storm::logic::Formula>>("ProbabilityOperatorFormula", no_init)
        .def("toString", &storm::logic::ProbabilityOperatorFormula::toString);


    ////////////////////////////////////////////
    // Program
    ////////////////////////////////////////////

    class_<storm::prism::Program>("Program")
        .add_property("nrModules", &storm::prism::Program::getNumberOfModules)
    ;


    ////////////////////////////////////////////
    // Checkresult
    ////////////////////////////////////////////
    class_<std::unique_ptr<storm::modelchecker::CheckResult>, boost::noncopyable>("CheckResult", no_init);


    ////////////////////////////////////////////
    // Models
    ////////////////////////////////////////////
    class_<storm::models::ModelBase, std::shared_ptr<storm::models::ModelBase>, boost::noncopyable>("ModelBase", no_init)
            .add_property("nrStates", &storm::models::ModelBase::getNumberOfStates)
            .add_property("nrTransitions", &storm::models::ModelBase::getNumberOfTransitions);
    class_<storm::models::sparse::Model<double>, std::shared_ptr<storm::models::sparse::Model<double>>, boost::noncopyable, bases<storm::models::ModelBase>>("SparseModel", no_init);

    def("parseFormulae", storm::parseFormulasForProgram);
    def("parseProgram", storm::parseProgram);

    def("buildModelFromPrismProgram", storm::buildSymbolicModel<double>);


    def("exportMatrixFromModel", storm::exportMatrixToFile)

    
}