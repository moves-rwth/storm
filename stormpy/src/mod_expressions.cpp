#include "common.h"
#include "helpers.h"
#include "storm/storage/expressions/ExpressionManager.h"

PYBIND11_PLUGIN(expressions) {
	py::module m("expressions", "Storm expressions");

    py::class_<std::shared_ptr<storm::expressions::ExpressionManager>>(m, "ExpressionManager", "Manages variables for expressions")
        ;
    
    py::class_<storm::expressions::Expression>(m, "Expression", "Holds an expression")
    .def("__str__", &storm::expressions::Expression::toString)
    .def_property_readonly("contains_variables", &storm::expressions::Expression::containsVariables)
    .def_property_readonly("has_boolean_type", &storm::expressions::Expression::hasBooleanType)
    .def_property_readonly("has_integer_type", &storm::expressions::Expression::hasIntegerType)
    .def_property_readonly("has_rational_type", &storm::expressions::Expression::hasRationalType)
    
        ;

	return m.ptr();
}
