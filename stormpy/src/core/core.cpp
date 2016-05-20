#include "core.h"

void define_core(py::module& m) {
    
    // Init
    m.def("set_up", [] (std::string const& args) {
            storm::utility::setUp();
            storm::settings::SettingsManager::manager().setFromString(args);
        }, "Initialize Storm", py::arg("arguments"));
}

void define_parse(py::module& m) {
    // Parse formulas
    m.def("parse_formulas", &storm::parseFormulasForExplicit, "Parse explicit formulas", py::arg("formula_string"));
    m.def("parse_formulas_for_prism_program", &storm::parseFormulasForProgram, "Parse formulas for prism program", py::arg("formula_string"), py::arg("prism_program"));

    // Pair <Model,Formulas>
    py::class_<storm::storage::ModelFormulasPair>(m, "ModelFormulasPair", "Pair of model and formulas")
        .def_readwrite("model", &storm::storage::ModelFormulasPair::model, "The model")
        .def_readwrite("formulas", &storm::storage::ModelFormulasPair::formulas, "The formulas")
    ;

    // Parse explicit models
    m.def("parse_explicit_model", &storm::parser::AutoParser<>::parseModel, "Parse explicit model", py::arg("transition_file"), py::arg("labeling_file"), py::arg("state_reward_file") = "", py::arg("transition_reward_file") = "", py::arg("choice_labeling_file") = "");
}
