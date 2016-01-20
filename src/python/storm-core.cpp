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

void printResult(std::shared_ptr<storm::modelchecker::CheckResult> result) {
    result->writeToStream(std::cout);
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
    register_ptr_to_python<std::shared_ptr<storm::modelchecker::CheckResult>>();

    def("print_result", printResult);


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

    defineClass<std::vector<std::shared_ptr<storm::logic::Formula>>, void, void>("FormulaVec", "Vector of formulas")
            .def(vector_indexing_suite<std::vector<std::shared_ptr<storm::logic::Formula>>, true>())
            ;

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


    def("perform_state_elimination", boost::python::converter::adapt_unique(storm::verifySparseModel<storm::RationalFunction>));
}
