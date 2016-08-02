#include "core.h"

void define_core(py::module& m) {
    
    // Init
    m.def("set_up", [] (std::string const& args) {
            storm::utility::setUp();
            storm::settings::SettingsManager::manager().setFromString(args);
        }, "Initialize Storm", py::arg("arguments"));

    // Parse formulas
    m.def("parse_formulas", &storm::parseFormulasForExplicit, "Parse explicit formulas", py::arg("formula_string"));
    m.def("parse_formulas_for_program", &storm::parseFormulasForProgram, "Parse formulas for program", py::arg("formula_string"), py::arg("program"));

    // Pair <Model,Formulas>
    py::class_<storm::storage::ModelFormulasPair>(m, "ModelFormulasPair", "Pair of model and formulas")
        .def_readwrite("model", &storm::storage::ModelFormulasPair::model, "The model")
        .def_readwrite("formulas", &storm::storage::ModelFormulasPair::formulas, "The formulas")
    ;
}
