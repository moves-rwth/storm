#include "model.h"

// Thin wrapper for model building
template<typename ValueType>
std::shared_ptr<storm::models::ModelBase> buildModel(storm::prism::Program const& program, std::shared_ptr<storm::logic::Formula const> const& formula) {
    return storm::buildSymbolicModel<ValueType>(program, std::vector<std::shared_ptr<storm::logic::Formula const>>(1,formula)).model;
}

// Define python bindings
void define_model(py::module& m) {
   
    // Build model
    m.def("_build_model", &buildModel<double>, "Build the model", py::arg("program"), py::arg("formula"));
    m.def("_build_parametric_model", &buildModel<storm::RationalFunction>, "Build the parametric model", py::arg("program"), py::arg("formula"));
    m.def("build_model_from_prism_program", &storm::buildSymbolicModel<double>, "Build the model", py::arg("program"), py::arg("formulas"));
    m.def("build_parametric_model_from_prism_program", &storm::buildSymbolicModel<storm::RationalFunction>, "Build the parametric model", py::arg("program"), py::arg("formulas"));

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
        .def("parametric", &storm::models::ModelBase::isParametric, "Check if model is parametric")
        .def("as_dtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<double>>, "Get model as DTMC")
        .def("as_pdtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<storm::RationalFunction>>, "Get model as pDTMC")
        .def("as_mdp", &storm::models::ModelBase::as<storm::models::sparse::Mdp<double>>, "Get model as MDP")
        .def("as_pmdp", &storm::models::ModelBase::as<storm::models::sparse::Mdp<storm::RationalFunction>>, "Get model as pMDP")
    ;

    // Models
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

}
