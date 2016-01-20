#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "helpers.h"
#include "boostPyExtension.h"

#include "../storage/expressions/ExpressionManager.h"


BOOST_PYTHON_MODULE(_expressions)
{
    using namespace boost::python;

    defineClass<storm::expressions::ExpressionManager, void, boost::noncopyable>("ExpressionManager",
    "Manages variables for expressions");
    defineClass<storm::expressions::Expression, void, boost::noncopyable>("Expression",
    "");

}
