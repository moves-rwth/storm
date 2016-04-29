#include "core.h"

#include "src/common.h"

#include <src/utility/storm.h>

// Thin wrapper for initializing
void setupStormLib(std::string const& args) {
    storm::utility::setUp();
    storm::settings::SettingsManager::manager().setFromString(args);
}

void define_core(py::module& m) {
    
    m.def("set_up", &setupStormLib, "Initialize Storm");

    m.def("parse_formulae", storm::parseFormulasForProgram, "Parse formula for program");
    m.def("parse_program", storm::parseProgram, "Parse program");

    //m.def("build_model", buildModel, return_value_policy<return_by_value>());

    //m.def("build_model_from_prism_program", storm::buildSymbolicModel<double>);
    //m.def("build_parametric_model_from_prism_program", storm::buildSymbolicModel<storm::RationalFunction>);


}
