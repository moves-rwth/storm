#include "model.h"

#include "src/models/ModelBase.h"
#include "src/models/sparse/Model.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"

// Thin wrapper for getting initial states
template<typename ValueType>
std::vector<storm::storage::sparse::state_type> getInitialStates(storm::models::sparse::Model<ValueType> const& model) {
    std::vector<storm::storage::sparse::state_type> initialStates;
    for (auto entry : model.getInitialStates()) {
        initialStates.push_back(entry);
    }
    return initialStates;
}

storm::storage::SparseMatrix<double> getTransitionMatrix(storm::models::sparse::Model<double> const& model) {
    return model.getTransitionMatrix();
}

// Define python bindings
void define_model(py::module& m) {

    // ModelType
    py::enum_<storm::models::ModelType>(m, "ModelType", "Type of the model")
        .value("DTMC", storm::models::ModelType::Dtmc)
        .value("MDP", storm::models::ModelType::Mdp)
        .value("CTMC", storm::models::ModelType::Ctmc)
        .value("MA", storm::models::ModelType::MarkovAutomaton)
    ;

    // ModelBase
    py::class_<storm::models::ModelBase, std::shared_ptr<storm::models::ModelBase>>(m, "ModelBase", "Base class for all models")
        .def("nr_states", &storm::models::ModelBase::getNumberOfStates, "Get number of states")
        .def("nr_transitions", &storm::models::ModelBase::getNumberOfTransitions, "Get number of transitions")
        .def("model_type", &storm::models::ModelBase::getType, "Get model type")
        .def("supports_parameters", &storm::models::ModelBase::supportsParameters, "Check if model supports parameters")
        .def("has_parameters", &storm::models::ModelBase::hasParameters, "Check if model has parameters")
        .def("is_exact", &storm::models::ModelBase::isExact, "Check if model is exact")
        .def("as_dtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<double>>, "Get model as DTMC")
        .def("as_pdtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<storm::RationalFunction>>, "Get model as pDTMC")
        .def("as_mdp", &storm::models::ModelBase::as<storm::models::sparse::Mdp<double>>, "Get model as MDP")
        .def("as_pmdp", &storm::models::ModelBase::as<storm::models::sparse::Mdp<storm::RationalFunction>>, "Get model as pMDP")
    ;

    // Models
    py::class_<storm::models::sparse::Model<double>, std::shared_ptr<storm::models::sparse::Model<double>>>(m, "SparseModel", "A probabilistic model where transitions are represented by doubles and saved in a sparse matrix", py::base<storm::models::ModelBase>())
        .def("labels", [](storm::models::sparse::Model<double> const& model) {
                return model.getStateLabeling().getLabels();
            }, "Get labels")
        .def("labels_state", &storm::models::sparse::Model<double>::getLabelsOfState, "Get labels")
        .def("initial_states", &getInitialStates<double>, "Get initial states")
        .def("transition_matrix", &getTransitionMatrix, "Get transition matrix")
    ;    
    py::class_<storm::models::sparse::Dtmc<double>, std::shared_ptr<storm::models::sparse::Dtmc<double>>>(m, "SparseDtmc", "DTMC in sparse representation", py::base<storm::models::sparse::Model<double>>())
    ;
    py::class_<storm::models::sparse::Mdp<double>, std::shared_ptr<storm::models::sparse::Mdp<double>>>(m, "SparseMdp", "MDP in sparse representation", py::base<storm::models::sparse::Model<double>>())
    ;
    py::class_<storm::models::sparse::Model<storm::RationalFunction>, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>>>(m, "SparseParametricModel", "A probabilistic model where transitions are represented by rational functions and saved in a sparse matrix", py::base<storm::models::ModelBase>())
        .def("collect_probability_parameters", &storm::models::sparse::getProbabilityParameters, "Collect parameters")
        .def("labels", [](storm::models::sparse::Model<storm::RationalFunction> const& model) {
                return model.getStateLabeling().getLabels();
            }, "Get labels")
        .def("labels_state", &storm::models::sparse::Model<storm::RationalFunction>::getLabelsOfState, "Get labels")
        .def("initial_states", &getInitialStates<storm::RationalFunction>, "Get initial states")
    ;
    py::class_<storm::models::sparse::Dtmc<storm::RationalFunction>, std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>>>(m, "SparseParametricDtmc", "pDTMC in sparse representation", py::base<storm::models::sparse::Model<storm::RationalFunction>>())
    ;
    py::class_<storm::models::sparse::Mdp<storm::RationalFunction>, std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>>>(m, "SparseParametricMdp", "pMDP in sparse representation", py::base<storm::models::sparse::Model<storm::RationalFunction>>())
    ;

}
