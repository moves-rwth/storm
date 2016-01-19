#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "../utility/storm.h"
#include "../logic/Formulas.h"


//namespace boost {
//    template<class T> T* get_pointer(std::shared_ptr<T> p) { return p.get(); }
//}

namespace boost { namespace python { namespace converter {

            template <class T>
            PyObject* shared_ptr_to_python(std::shared_ptr<T> const& x)
            {
                if (!x)
                    return python::detail::none();
                else if (shared_ptr_deleter* d = std::get_deleter<shared_ptr_deleter>(x))
                    return incref( d->owner.get() );
                else
                    return converter::registered<std::shared_ptr<T> const&>::converters.to_python(&x);
            }

    /// @brief Adapter a non-member function that returns a unique_ptr to
    ///        a python function object that returns a raw pointer but
    ///        explicitly passes ownership to Python.
    template<typename T, typename ...Args>
    object adapt_unique(std::unique_ptr<T> (*fn)(Args...))
    {
        return make_function(
            [fn](Args... args) { return fn(args...).release(); },
            return_value_policy<manage_new_object>(),
            boost::mpl::vector<T*, Args...>()
        );
    }

    /// @brief Adapter a member function that returns a unique_ptr to
    ///        a python function object that returns a raw pointer but
    ///        explicitly passes ownership to Python.
    template<typename T, typename C, typename ...Args>
    object adapt_unique(std::unique_ptr<T> (C::*fn)(Args...))
    {
        return make_function(
            [fn](C& self, Args... args) { return (self.*fn)(args...).release(); },
            python::return_value_policy<manage_new_object>(),
            boost::mpl::vector<T*, C&, Args...>()
        );
    }

        }}} // namespace boost::python::converter



std::shared_ptr<storm::models::ModelBase> buildModel(storm::prism::Program const& program, std::shared_ptr<storm::logic::Formula> const& formula) {
    storm::settings::SettingsManager::manager().setFromString("");
    return storm::buildSymbolicModel<storm::RationalFunction>(program, std::vector<std::shared_ptr<storm::logic::Formula>>(1,formula)).model;
}

void printResult(std::shared_ptr<storm::modelchecker::CheckResult> result) {
    result->writeToStream(std::cout);
}

BOOST_PYTHON_MODULE(_core)
{
    using namespace boost::python;
    def("setUp", storm::utility::setUp);

    ////////////////////////////////////////////
    // Formula
    ////////////////////////////////////////////
    class_<storm::logic::Formula, std::shared_ptr<storm::logic::Formula>, boost::noncopyable>("Formula", no_init)
        .def("toString", &storm::logic::Formula::toString);
	class_<std::vector<std::shared_ptr<storm::logic::Formula>>>("FormulaVec")
			.def(vector_indexing_suite<std::vector<std::shared_ptr<storm::logic::Formula>>, true>())
			;
    class_<storm::logic::ProbabilityOperatorFormula, std::shared_ptr<storm::logic::ProbabilityOperatorFormula>, bases<storm::logic::Formula>>("ProbabilityOperatorFormula", no_init)
        .def("toString", &storm::logic::ProbabilityOperatorFormula::toString);

    register_ptr_to_python<std::shared_ptr<storm::logic::Formula>>();

    ////////////////////////////////////////////
    // Program
    ////////////////////////////////////////////

    class_<storm::prism::Program>("Program")
        .add_property("nrModules", &storm::prism::Program::getNumberOfModules)
    ;


    ////////////////////////////////////////////
    // Checkresult
    ////////////////////////////////////////////
    class_<storm::modelchecker::CheckResult, std::unique_ptr<storm::modelchecker::CheckResult>, boost::noncopyable>("CheckResult", no_init)
    ;
    register_ptr_to_python<std::shared_ptr<storm::modelchecker::CheckResult>>();

    def("printResult", printResult);


    ////////////////////////////////////////////
    // Models
    ////////////////////////////////////////////

    enum_<storm::models::ModelType>("ModelType")
            .value("dtmc", storm::models::ModelType::Dtmc)
            .value("mdp", storm::models::ModelType::Mdp)
            .value("ctmc", storm::models::ModelType::Ctmc)
            .value("ma", storm::models::ModelType::MarkovAutomaton)
            ;

    class_<storm::models::ModelBase, std::shared_ptr<storm::models::ModelBase>, boost::noncopyable>("ModelBase", no_init)
            .add_property("nrStates", &storm::models::ModelBase::getNumberOfStates)
            .add_property("nrTransitions", &storm::models::ModelBase::getNumberOfTransitions)
            .add_property("model_type", &storm::models::ModelBase::getType)
            .def("asPdtmc", &storm::models::ModelBase::as<storm::models::sparse::Dtmc<storm::RationalFunction>>)
    ;
    class_<storm::models::sparse::Model<storm::RationalFunction>, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction> >, boost::noncopyable, bases<storm::models::ModelBase>>("SparseParametricModel", no_init);
    class_<storm::models::sparse::Model<double>, std::shared_ptr<storm::models::sparse::Model<double>>, boost::noncopyable, bases<storm::models::ModelBase>>("SparseModel", no_init);
    class_<storm::models::sparse::Dtmc<storm::RationalFunction>, std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>>, boost::noncopyable, bases<storm::models::sparse::Model<storm::RationalFunction>>>("SparseParametricMc", no_init);

    register_ptr_to_python<std::shared_ptr<storm::models::ModelBase>>();
    register_ptr_to_python<std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>>>();
    implicitly_convertible<std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>>, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>>>();
    register_ptr_to_python<std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>>>();

    def("parseFormulae", storm::parseFormulasForProgram);
    def("parseProgram", storm::parseProgram);

    def("buildModel", buildModel);

    def("buildModelFromPrismProgram", storm::buildSymbolicModel<double>);
    def("buildParametricModelFromPrismProgram", storm::buildSymbolicModel<storm::RationalFunction>);

    //////////////////////////////////////////////
    // Model Checking
    //////////////////////////////////////////////
    class_<storm::storage::ModelFormulasPair>("ModelProgramPair", no_init)
            .add_property("model", &storm::storage::ModelFormulasPair::model)
            .add_property("program", &storm::storage::ModelFormulasPair::formulas)
    ;


    def("performStateElimination", boost::python::converter::adapt_unique(storm::verifySparseModel<storm::RationalFunction>));
}
