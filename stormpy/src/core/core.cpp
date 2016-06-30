#include "core.h"

void define_core(py::module& m) {
    // Init
    m.def("set_up", [](std::string const& args) {
            storm::utility::setUp();
            storm::settings::initializeAll("StoRM-Py", "stormpy");
            storm::settings::SettingsManager::manager().setFromString(args);
        }, "Initialize Storm", py::arg("arguments"));
}

void define_parse(py::module& m) {
    // Parse formulas
    m.def("parse_formulas", &storm::parseFormulasForExplicit, "Parse explicit formulas", py::arg("formula_string"));
    m.def("parse_formulas_for_prism_program", &storm::parseFormulasForProgram, "Parse formulas for prism program", py::arg("formula_string"), py::arg("prism_program"));

    // Pair <Model,Formulas>
    py::class_<storm::storage::ModelFormulasPair>(m, "ModelFormulasPair", "Pair of model and formulas")
        .def("model", [](storm::storage::ModelFormulasPair const& pair) {
                return pair.model;
            }, "The model")
        .def("formulas", [](storm::storage::ModelFormulasPair const& pair) {
                return pair.formulas;
            }, "The formulas")
    ;

    // Parse explicit models
    m.def("parse_explicit_model", &storm::parser::AutoParser<>::parseModel, "Parse explicit model", py::arg("transition_file"), py::arg("labeling_file"), py::arg("state_reward_file") = "", py::arg("transition_reward_file") = "", py::arg("choice_labeling_file") = "");
}

// Thin wrapper for model building
template<typename ValueType>
std::shared_ptr<storm::models::ModelBase> buildModel(storm::prism::Program const& program, std::shared_ptr<storm::logic::Formula const> const& formula) {
    return storm::buildSymbolicModel<ValueType>(program, std::vector<std::shared_ptr<storm::logic::Formula const>>(1,formula));
}

void define_build(py::module& m) {
    // Build model
    m.def("_build_model", &buildModel<double>, "Build the model", py::arg("program"), py::arg("formula"));
    m.def("_build_parametric_model", &buildModel<storm::RationalFunction>, "Build the parametric model", py::arg("program"), py::arg("formula"));
    m.def("build_model_from_prism_program", &storm::buildSymbolicModel<double>, "Build the model", py::arg("program"), py::arg("formulas"));
    m.def("build_parametric_model_from_prism_program", &storm::buildSymbolicModel<storm::RationalFunction>, "Build the parametric model", py::arg("program"), py::arg("formulas"));
    
}
