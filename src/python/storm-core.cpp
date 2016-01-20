#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "../utility/storm.h"
#include "../logic/Formulas.h"

#include <type_traits>

#include "helpers.h"
#include "boostPyExtension.h"



std::shared_ptr<storm::models::ModelBase> buildModel(storm::prism::Program const& program, std::shared_ptr<storm::logic::Formula> const& formula) {
    return storm::buildSymbolicModel<storm::RationalFunction>(program, std::vector<std::shared_ptr<storm::logic::Formula>>(1,formula)).model;
}

std::unique_ptr<storm::modelchecker::CheckResult> performStateElimination(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula> const& formula) {
    std::cout << "Perform state elimination" << std::endl;
    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::verifySparseModel<storm::RationalFunction>(model, formula);
    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
    std::cout << "Result: " << *result << std::endl;
    return result;
}

void setupStormLib(std::string const& args) {
    storm::utility::setUp();
    storm::settings::SettingsManager::manager().setFromString(args);
}



BOOST_PYTHON_MODULE(_core)
{
    using namespace boost::python;
    def("setUp", setupStormLib);

    ////////////////////////////////////////////
    // Formula
    ////////////////////////////////////////////
    class_<storm::logic::Formula, std::shared_ptr<storm::logic::Formula>, boost::noncopyable>("Formula", no_init)
        .def("__str__", &storm::logic::Formula::toString);
	class_<std::vector<std::shared_ptr<storm::logic::Formula>>>("FormulaVec")
			.def(vector_indexing_suite<std::vector<std::shared_ptr<storm::logic::Formula>>, true>())
			;
    class_<storm::logic::ProbabilityOperatorFormula, std::shared_ptr<storm::logic::ProbabilityOperatorFormula>, bases<storm::logic::Formula>>("ProbabilityOperatorFormula", no_init)
        .def("__str__", &storm::logic::ProbabilityOperatorFormula::toString);

    register_ptr_to_python<std::shared_ptr<storm::logic::Formula>>();

    ////////////////////////////////////////////
    // Program
    ////////////////////////////////////////////

    defineClass<storm::prism::Program>("Program", "")
    //class_<storm::prism::Program>("Program")
        .add_property("nr_modules", &storm::prism::Program::getNumberOfModules)
    ;


    ////////////////////////////////////////////
    // Checkresult
    ////////////////////////////////////////////
    class_<storm::modelchecker::CheckResult, std::unique_ptr<storm::modelchecker::CheckResult>, boost::noncopyable>("CheckResult", no_init)
    ;


    ////////////////////////////////////////////
    // Models
    ////////////////////////////////////////////

    enum_<storm::models::ModelType>("ModelType")
            .value("dtmc", storm::models::ModelType::Dtmc)
            .value("mdp", storm::models::ModelType::Mdp)
            .value("ctmc", storm::models::ModelType::Ctmc)
            .value("ma", storm::models::ModelType::MarkovAutomaton)
            ;

    defineClass<storm::models::ModelBase, void, boost::noncopyable>("ModelBase", "")
         .add_property("nr_states", &storm::models::ModelBase::getNumberOfStates)
            .add_property("nr_transitions", &storm::models::ModelBase::getNumberOfTransitions)
            .add_property("model_type", &storm::models::ModelBase::getType)
            .def("as_dtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<double>>)
            .def("as_pdtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<storm::RationalFunction>>)
            .def("as_pmdp", &storm::models::ModelBase::as<storm::models::sparse::Mdp<storm::RationalFunction>>)

    ;

    defineClass<storm::models::sparse::Model<storm::RationalFunction>, storm::models::ModelBase, boost::noncopyable>("SparseParametricModel", "");

    defineClass<storm::models::sparse::Model<double>, storm::models::ModelBase, boost::noncopyable>("SparseModel", "");
    defineClass<storm::models::sparse::Dtmc<double>, storm::models::sparse::Model<double>, boost::noncopyable>("Dtmc", "");

    defineClass<storm::models::sparse::Dtmc<storm::RationalFunction>,  storm::models::sparse::Model<storm::RationalFunction>>("SparseParametricMc", "");
    defineClass<storm::models::sparse::Mdp<storm::RationalFunction>, storm::models::sparse::Model<storm::RationalFunction>>("SparseParametricMdp", "");


    def("parse_formulae", storm::parseFormulasForProgram);
    def("parse_program", storm::parseProgram);

    def("build_model", buildModel, return_value_policy<return_by_value>());

    def("buildodelFromPrismProgram", storm::buildSymbolicModel<double>);
    def("buildParametricModelFromPrismProgram", storm::buildSymbolicModel<storm::RationalFunction>);

    //////////////////////////////////////////////
    // Model Checking
    //////////////////////////////////////////////
    class_<storm::storage::ModelFormulasPair>("ModelProgramPair", no_init)
            .add_property("model", &storm::storage::ModelFormulasPair::model)
            .add_property("program", &storm::storage::ModelFormulasPair::formulas)
    ;


    def("perform_state_elimination", boost::python::converter::adapt_unique(performStateElimination));
}
