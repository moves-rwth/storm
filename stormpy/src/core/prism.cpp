#include "prism.h"

// Define python bindings
void define_prism(py::module& m) {

    // Parse prism program
    m.def("parse_prism_program", &storm::parseProgram, "Parse prism program", py::arg("path"));

    // PrismType
    py::enum_<storm::prism::Program::ModelType>(m, "PrismModelType", "Type of the prism model")
        .value("DTMC", storm::prism::Program::ModelType::DTMC)
        .value("CTMC", storm::prism::Program::ModelType::CTMC)
        .value("MDP", storm::prism::Program::ModelType::MDP)
        .value("CTMDP", storm::prism::Program::ModelType::CTMDP)
        .value("MA", storm::prism::Program::ModelType::MA)
        .value("UNDEFINED", storm::prism::Program::ModelType::UNDEFINED)
    ;
    
    // PrismProgram
    py::class_<storm::prism::Program>(m, "PrismProgram", "Prism program")
        .def_property_readonly("nr_modules", &storm::prism::Program::getNumberOfModules, "Number of modules")
        .def_property_readonly("model_type", &storm::prism::Program::getModelType, "Model type")
        .def_property_readonly("has_undefined_constants", &storm::prism::Program::hasUndefinedConstants, "Flag if program has undefined constants")
    ;
    
}
