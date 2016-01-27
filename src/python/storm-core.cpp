#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "../utility/storm.h"
#include "../logic/Formulas.h"

#include <type_traits>

#include "helpers.h"
#include "boostPyExtension.h"

// Holds the rational function and constraints after parametric model checking
class PmcResult {
public:
    storm::RationalFunction resultFunction;
    std::unordered_set<storm::ArithConstraint<storm::RationalFunction>> constraintsWellFormed;
    std::unordered_set<storm::ArithConstraint<storm::RationalFunction>> constraintsGraphPreserving;
};

// Thin wrapper for model building
std::shared_ptr<storm::models::ModelBase> buildModel(storm::prism::Program const& program, std::shared_ptr<const storm::logic::Formula> const& formula) {
    return storm::buildSymbolicModel<storm::RationalFunction>(program, std::vector<std::shared_ptr<const storm::logic::Formula>>(1,formula)).model;
}

// Thin wrapper for parametric state elimination
std::shared_ptr<PmcResult> performStateElimination(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<const storm::logic::Formula> const& formula) {
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = storm::verifySparseModel<storm::RationalFunction>(model, formula);
    std::shared_ptr<PmcResult> result = std::make_shared<PmcResult>();
    result->resultFunction = (checkResult->asExplicitQuantitativeCheckResult<storm::RationalFunction>()[*model->getInitialStates().begin()]);
    storm::models::sparse::Dtmc<storm::RationalFunction>::ConstraintCollector constraintCollector(*(model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>()));
    result->constraintsWellFormed = constraintCollector.getWellformedConstraints();
    result->constraintsGraphPreserving = constraintCollector.getGraphPreservingConstraints();
    return result;
}

// Thin wrapper for initializing
void setupStormLib(std::string const& args) {
    storm::utility::setUp();
    storm::settings::SettingsManager::manager().setFromString(args);
}

BOOST_PYTHON_MODULE(_core)
{
    using namespace boost::python;
    def("set_up", setupStormLib);


    ////////////////////////////////////////////
    // Program
    ////////////////////////////////////////////

    defineClass<storm::prism::Program>("Program", "")
        .add_property("nr_modules", &storm::prism::Program::getNumberOfModules)
    ;


    ////////////////////////////////////////////
    // PmcResult
    ////////////////////////////////////////////
    class_<PmcResult, std::shared_ptr<PmcResult>, boost::noncopyable>("PmcResult", "Holds the results after parametric model checking")
        .add_property("result_function", &PmcResult::resultFunction)
        .add_property("constraints_well_formed", &PmcResult::constraintsWellFormed)
        .add_property("constraints_graph_preserving", &PmcResult::constraintsGraphPreserving)
    ;
    register_ptr_to_python<std::shared_ptr<PmcResult>>();


    ////////////////////////////////////////////
    // Models
    ////////////////////////////////////////////

    enum_<storm::models::ModelType>("ModelType")
        .value("DTMC", storm::models::ModelType::Dtmc)
        .value("MDP", storm::models::ModelType::Mdp)
        .value("CTMC", storm::models::ModelType::Ctmc)
        .value("MA", storm::models::ModelType::MarkovAutomaton)
    ;

    defineClass<storm::models::ModelBase, void, boost::noncopyable>("ModelBase", "")
        .add_property("nr_states", &storm::models::ModelBase::getNumberOfStates)
        .add_property("nr_transitions", &storm::models::ModelBase::getNumberOfTransitions)
        .add_property("model_type", &storm::models::ModelBase::getType)
        .add_property("parametric", &storm::models::ModelBase::isParametric)
        .def("as_dtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<double>>)
        .def("as_pdtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<storm::RationalFunction>>)
        .def("as_mdp", &storm::models::ModelBase::as<storm::models::sparse::Mdp<double>>)
        .def("as_pmdp", &storm::models::ModelBase::as<storm::models::sparse::Mdp<storm::RationalFunction>>)
    ;


    defineClass<storm::models::sparse::Model<double>, storm::models::ModelBase, boost::noncopyable>("SparseModel",
    "A probabilistic model where transitions are represented by doubles and saved in a sparse matrix");
    defineClass<storm::models::sparse::Dtmc<double>, storm::models::sparse::Model<double>, boost::noncopyable>("SparseDtmc", "");
    defineClass<storm::models::sparse::Mdp<double>, storm::models::sparse::Model<double>>("SparseMdp", "");

    defineClass<storm::models::sparse::Model<storm::RationalFunction>, storm::models::ModelBase, boost::noncopyable>("SparseParametricModel", "")
        .def("collect_probability_parameters", &storm::models::sparse::getProbabilityParameters)
    ;
    defineClass<storm::models::sparse::Dtmc<storm::RationalFunction>,  storm::models::sparse::Model<storm::RationalFunction>>("SparseParametricDtmc", "");
    defineClass<storm::models::sparse::Mdp<storm::RationalFunction>, storm::models::sparse::Model<storm::RationalFunction>>("SparseParametricMdp", "");

    defineClass<std::vector<std::shared_ptr<const storm::logic::Formula>>, void, void>("FormulaVec", "Vector of formulas")
        .def(vector_indexing_suite<std::vector<std::shared_ptr<const storm::logic::Formula>>, true>())
    ;
    
    ////////////////////////////////////////////
    // Bisimulation
    ////////////////////////////////////////////  
    enum_<storm::storage::BisimulationType>("BisimulationType")
        .value("STRONG", storm::storage::BisimulationType::Strong)
        .value("WEAK", storm::storage::BisimulationType::Weak)
    ;
    def("perform_bisimulation_parametric", static_cast<std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> (*)(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> const&, std::shared_ptr<const storm::logic::Formula> const&, storm::storage::BisimulationType)>(&storm::performBisimulationMinimization<storm::models::sparse::Model<storm::RationalFunction>>));
    
    

    def("parse_formulae", storm::parseFormulasForProgram);
    def("parse_program", storm::parseProgram);

    def("build_model", buildModel, return_value_policy<return_by_value>());

    def("build_model_from_prism_program", storm::buildSymbolicModel<double>);
    def("build_parametric_model_from_prism_program", storm::buildSymbolicModel<storm::RationalFunction>);

    //////////////////////////////////////////////
    // Model Checking
    //////////////////////////////////////////////
    class_<storm::storage::ModelFormulasPair>("ModelProgramPair", no_init)
        .add_property("model", &storm::storage::ModelFormulasPair::model)
        .add_property("program", &storm::storage::ModelFormulasPair::formulas)
    ;

    def("perform_state_elimination", performStateElimination);
}
