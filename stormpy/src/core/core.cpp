#include "core.h"

#include <pybind11/stl.h>

#include "src/common.h"

#include <src/utility/storm.h>

// Thin wrapper for initializing
void setupStormLib(std::string const& args) {
    storm::utility::setUp();
    storm::settings::SettingsManager::manager().setFromString(args);
}

// Thin wrapper for model building
std::shared_ptr<storm::models::ModelBase> buildModel(storm::prism::Program const& program, std::shared_ptr<storm::logic::Formula> const& formula) {
    return storm::buildSymbolicModel<storm::RationalFunction>(program, std::vector<std::shared_ptr<const storm::logic::Formula>>(1,formula)).model;
}

// Class holding the model checking result
/*class PmcResult {
    public:
        storm::RationalFunction resultFunction;
        std::unordered_set<storm::ArithConstraint<storm::RationalFunction>> constraintsWellFormed;
        std::unordered_set<storm::ArithConstraint<storm::RationalFunction>> constraintsGraphPreserving;
};

// Thin wrapper for parametric state elimination
std::shared_ptr<PmcResult> performStateElimination(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula> const& formula) {
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = storm::verifySparseModel<storm::RationalFunction>(model, formula);
    std::shared_ptr<PmcResult> result = std::make_shared<PmcResult>();
    result->resultFunction = (checkResult->asExplicitQuantitativeCheckResult<storm::RationalFunction>()[*model->getInitialStates().begin()]);
    storm::models::sparse::Dtmc<storm::RationalFunction>::ConstraintCollector constraintCollector(*(model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>()));
    result->constraintsWellFormed = constraintCollector.getWellformedConstraints();
    result->constraintsGraphPreserving = constraintCollector.getGraphPreservingConstraints();
    return result;
}*/

// Define python bindings
void define_core(py::module& m) {
    m.def("set_up", &setupStormLib, "Initialize Storm");

    m.def("parse_program", &storm::parseProgram, "Parse program");
    m.def("parse_formulas", &storm::parseFormulasForProgram, "Parse formula for program");

    m.def("build_model", &buildModel, "Build the model");
    m.def("build_model_from_prism_program", &storm::buildSymbolicModel<double>, "Build the model");
    m.def("build_parametric_model_from_prism_program", &storm::buildSymbolicModel<storm::RationalFunction>, "Build the parametric model");

    //m.def("perform_state_elimination", &performStateElimination, "Perform state elimination");
    
    //m.def("perform_bisimulation_parametric", static_cast<std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> (*)(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> const&, std::shared_ptr<const storm::logic::Formula> const&, storm::storage::BisimulationType)>(&storm::performBisimulationMinimization<storm::models::sparse::Model<storm::RationalFunction>>), "Perform bisimulation");
    

    // Program
    py::enum_<storm::prism::Program::ModelType>(m, "PrismModelType", "Type of the prism model")
        .value("DTMC", storm::prism::Program::ModelType::DTMC)
        .value("CTMC", storm::prism::Program::ModelType::CTMC)
        .value("MDP", storm::prism::Program::ModelType::MDP)
        .value("CTMDP", storm::prism::Program::ModelType::CTMDP)
        .value("MA", storm::prism::Program::ModelType::MA)
        .value("UNDEFINED", storm::prism::Program::ModelType::UNDEFINED)
    ;
    
    py::class_<storm::prism::Program>(m, "Program", "Prism program")
        .def("nr_modules", &storm::prism::Program::getNumberOfModules, "Get number of modules")
        .def("model_type", &storm::prism::Program::getModelType, "Get model type")
        .def("has_undefined_constants", &storm::prism::Program::hasUndefinedConstants, "Check if program has undefined constants")
    ;
    
    py::class_<storm::storage::ModelFormulasPair>(m, "ModelProgramPair", "Pair of model and program")
        .def_readwrite("model", &storm::storage::ModelFormulasPair::model, "The model")
        .def_readwrite("formulas", &storm::storage::ModelFormulasPair::formulas, "The formulas")
    ;

    
    // Models
    py::enum_<storm::models::ModelType>(m, "ModelType", "Type of the model")
        .value("DTMC", storm::models::ModelType::Dtmc)
        .value("MDP", storm::models::ModelType::Mdp)
        .value("CTMC", storm::models::ModelType::Ctmc)
        .value("MA", storm::models::ModelType::MarkovAutomaton)
    ;

    py::class_<storm::models::ModelBase, std::shared_ptr<storm::models::ModelBase>>(m, "ModelBase", "Base class for all models")
        .def("nr_states", &storm::models::ModelBase::getNumberOfStates, "Get number of states")
        .def("nr_transitions", &storm::models::ModelBase::getNumberOfTransitions, "Get number of transitions")
        .def("model_type", &storm::models::ModelBase::getType, "Get model type")
        .def("parametric", &storm::models::ModelBase::isParametric, "Check if model is parametric")
        .def("as_dtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<double>>, "Get model as DTMC")
        .def("as_pdtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<storm::RationalFunction>>, "Get model as pDTMC")
        .def("as_mdp", &storm::models::ModelBase::as<storm::models::sparse::Mdp<double>>, "Get model as MDP")
        .def("as_pmdp", &storm::models::ModelBase::as<storm::models::sparse::Mdp<storm::RationalFunction>>, "Get model as pMDP")
    ;

    py::class_<storm::models::sparse::Model<double>, std::shared_ptr<storm::models::sparse::Model<double>>>(m, "SparseModel", "A probabilistic model where transitions are represented by doubles and saved in a sparse matrix", py::base<storm::models::ModelBase>())
    ;    

    py::class_<storm::models::sparse::Dtmc<double>, std::shared_ptr<storm::models::sparse::Dtmc<double>>>(m, "SparseDtmc", "DTMC in sparse representation", py::base<storm::models::sparse::Model<double>>())
    ;

    py::class_<storm::models::sparse::Mdp<double>, std::shared_ptr<storm::models::sparse::Mdp<double>>>(m, "SparseMdp", "MDP in sparse representation", py::base<storm::models::sparse::Model<double>>())
    ;

    py::class_<storm::models::sparse::Model<storm::RationalFunction>, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>>>(m, "SparseParametricModel", "A probabilistic model where transitions are represented by rational functions and saved in a sparse matrix", py::base<storm::models::ModelBase>())
        .def("collect_probability_parameters", &storm::models::sparse::getProbabilityParameters, "Collect parameters")
    ;

    py::class_<storm::models::sparse::Dtmc<storm::RationalFunction>, std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>>>(m, "SparseParametricDtmc", "pDTMC in sparse representation", py::base<storm::models::sparse::Model<storm::RationalFunction>>())
    ;

    py::class_<storm::models::sparse::Mdp<storm::RationalFunction>, std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>>>(m, "SparseParametricMdp", "pMDP in sparse representation", py::base<storm::models::sparse::Model<storm::RationalFunction>>())
    ;

    
    // PmcResult
    /*py::class_<PmcResult, std::shared_ptr<PmcResult>>(m, "PmcResult", "Holds the results after parametric model checking")
        .def_readwrite("result_function", &PmcResult::resultFunction, "Result as rational function")
        //.def_readwrite("constraints_well_formed", &PmcResult::constraintsWellFormed, "Constraints ensuring well-formed probabilities")
        //.def_readwrite("constraints_graph_preserving", &PmcResult::constraintsGraphPreserving, "Constraints ensuring graph preservation")
    ;*/


    // Bisimulation
    /*py::enum_<storm::storage::BisimulationType>(m, "BisimulationType", "Types of bisimulation")
        .value("STRONG", storm::storage::BisimulationType::Strong)
        .value("WEAK", storm::storage::BisimulationType::Weak)
    ;*/
    
}
