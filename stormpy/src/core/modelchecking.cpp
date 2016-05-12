#include "modelchecking.h"

// Class holding the model checking result
class PmcResult {
    public:
        storm::RationalFunction resultFunction;
        std::unordered_set<storm::ArithConstraint<storm::RationalFunction>> constraintsWellFormed;
        std::unordered_set<storm::ArithConstraint<storm::RationalFunction>> constraintsGraphPreserving;

        std::string toString() {
            std::stringstream stream;
            stream << resultFunction << std::endl;
            stream << "Well formed constraints:" << std::endl;
            for (auto constraint : constraintsWellFormed) {
                stream << constraint << std::endl;
            }
            stream << "Graph preserving constraints:" << std::endl;
            for (auto constraint : constraintsGraphPreserving) {
                stream << constraint << std::endl;
            }
            return stream.str();
        }
};

// Thin wrapper for parametric state elimination
std::shared_ptr<PmcResult> performStateElimination(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula const> const& formula) {
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = storm::verifySparseModel<storm::RationalFunction>(model, formula);
    std::shared_ptr<PmcResult> result = std::make_shared<PmcResult>();
    result->resultFunction = checkResult->asExplicitQuantitativeCheckResult<storm::RationalFunction>()[*model->getInitialStates().begin()];
    storm::models::sparse::Dtmc<storm::RationalFunction>::ConstraintCollector constraintCollector(*(model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>()));
    result->constraintsWellFormed = constraintCollector.getWellformedConstraints();
    result->constraintsGraphPreserving = constraintCollector.getGraphPreservingConstraints();
    return result;
}

// Define python bindings
void define_modelchecking(py::module& m) {

    // Model checking
    m.def("perform_state_elimination", &performStateElimination, "Perform state elimination", py::arg("model"), py::arg("formula"));

    // PmcResult
    py::class_<PmcResult, std::shared_ptr<PmcResult>>(m, "PmcResult", "Holds the results after parametric model checking")
        .def("__str__", &PmcResult::toString)
        .def_readonly("result_function", &PmcResult::resultFunction, "Result as rational function")
        .def_readonly("constraints_well_formed", &PmcResult::constraintsWellFormed, "Constraints ensuring well-formed probabilities")
        .def_readonly("constraints_graph_preserving", &PmcResult::constraintsGraphPreserving, "Constraints ensuring graph preservation")
    ;

}
