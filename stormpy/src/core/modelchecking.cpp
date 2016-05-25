#include "modelchecking.h"

// Class holding the model checking result
class PmcResult {
    public:
        storm::RationalFunction resultFunction;
        std::unordered_set<storm::ArithConstraint<storm::RationalFunction>> constraintsWellFormed;
        std::unordered_set<storm::ArithConstraint<storm::RationalFunction>> constraintsGraphPreserving;

        storm::RationalFunction getResultFunction() const {
            return resultFunction;
        }

        std::unordered_set<storm::ArithConstraint<storm::RationalFunction>>  getConstraintsWellFormed() const {
            return constraintsWellFormed;
        }

        std::unordered_set<storm::ArithConstraint<storm::RationalFunction>>  getConstraintsGraphPreserving() const {
            return constraintsGraphPreserving;
        }

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

// Thin wrapper for model checking
double modelChecking(std::shared_ptr<storm::models::sparse::Model<double>> model, std::shared_ptr<storm::logic::Formula const> const& formula) {
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = storm::verifySparseModel<double>(model, formula);
    return checkResult->asExplicitQuantitativeCheckResult<double>()[*model->getInitialStates().begin()];
}

// Thin wrapper for parametric model checking
std::shared_ptr<PmcResult> parametricModelChecking(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula const> const& formula) {
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
    m.def("_model_checking", &modelChecking, "Perform model checking", py::arg("model"), py::arg("formula"));
    m.def("_parametric_model_checking", &parametricModelChecking, "Perform parametric model checking", py::arg("model"), py::arg("formula"));

    // PmcResult
    py::class_<PmcResult, std::shared_ptr<PmcResult>>(m, "PmcResult", "Holds the results after parametric model checking")
        .def("__str__", &PmcResult::toString)
        .def("result_function", &PmcResult::getResultFunction, "Result as rational function")
        .def("constraints_well_formed", &PmcResult::getConstraintsWellFormed, "Constraints ensuring well-formed probabilities")
        .def("constraints_graph_preserving", &PmcResult::getConstraintsGraphPreserving, "Constraints ensuring graph preservation")
    ;

}
