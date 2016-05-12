#include "bisimulation.h"

// Thin wrapper for bisimulation
template<typename ValueType>
std::shared_ptr<storm::models::sparse::Model<ValueType>> performBisimulation(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::shared_ptr<storm::logic::Formula const> const& formula, storm::storage::BisimulationType bisimulationType) {
    return storm::performBisimulationMinimization<storm::models::sparse::Model<ValueType>>(model, formula, bisimulationType);
}

// Define python bindings
void define_bisimulation(py::module& m) {

    // Bisimulation
    m.def("perform_bisimulation", &performBisimulation<double>, "Perform bisimulation", py::arg("program"), py::arg("formula"), py::arg("bisimulation_type"));
    m.def("perform_parametric_bisimulation", &performBisimulation<storm::RationalFunction>, "Perform bisimulation on parametric model", py::arg("program"), py::arg("formula"), py::arg("bisimulation_type"));

    // BisimulationType 
    py::enum_<storm::storage::BisimulationType>(m, "BisimulationType", "Types of bisimulation")
        .value("STRONG", storm::storage::BisimulationType::Strong)
        .value("WEAK", storm::storage::BisimulationType::Weak)
    ;

}
