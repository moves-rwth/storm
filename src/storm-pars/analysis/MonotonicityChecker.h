#ifndef STORM_MONOTONICITYCHECKER_H
#define STORM_MONOTONICITYCHECKER_H

#include <map>
#include <boost/container/flat_map.hpp>
#include "Order.h"
#include "LocalMonotonicityResult.h"
#include "MonotonicityResult.h"
#include "storm-pars/storage/ParameterRegion.h"


#include "storm/solver/Z3SmtSolver.h"

#include "storm/storage/SparseMatrix.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/RationalFunctionToExpression.h"

#include "storm/utility/constants.h"
#include "storm/utility/solver.h"

namespace storm {
    namespace analysis {

        template <typename ValueType>
        class MonotonicityChecker {

        public:
            typedef typename utility::parametric::VariableType<ValueType>::type VariableType;
            typedef typename utility::parametric::CoefficientType<ValueType>::type CoefficientType;
            typedef typename MonotonicityResult<VariableType>::Monotonicity Monotonicity;
            typedef typename storage::ParameterRegion<ValueType> Region;

            /*!
             * Constructs a new MonotonicityChecker object.
             *
             * @param matrix The Matrix of the model.
             */
            MonotonicityChecker(storage::SparseMatrix<ValueType> matrix);

            /*!
             * Checks if a derivative >=0 or/and <=0.
             *
             * @param derivative The derivative you want to check.
             * @param reg The region of the parameters.
             * @return Pair of bools, >= 0 and <= 0.
             */
            static std::pair<bool, bool> checkDerivative(ValueType derivative, storage::ParameterRegion<ValueType> reg) {
                bool monIncr = false;
                bool monDecr = false;

                if (derivative.isZero()) {
                    monIncr = true;
                    monDecr = true;
                } else if (derivative.isConstant()) {
                    monIncr = derivative.constantPart() >= 0;
                    monDecr = derivative.constantPart() <= 0;
                } else {
                    std::shared_ptr<utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<utility::solver::MathsatSmtSolverFactory>();
                    std::shared_ptr<expressions::ExpressionManager> manager(new expressions::ExpressionManager());
                    solver::Z3SmtSolver s(*manager);
                    std::set<VariableType> variables = derivative.gatherVariables();

                    expressions::Expression exprBounds = manager->boolean(true);
                    for (auto variable : variables) {
                        auto managerVariable = manager->declareRationalVariable(variable.name());
                        auto lb = utility::convertNumber<RationalNumber>(reg.getLowerBoundary(variable));
                        auto ub = utility::convertNumber<RationalNumber>(reg.getUpperBoundary(variable));
                        exprBounds = exprBounds && manager->rational(lb) < managerVariable && managerVariable < manager->rational(ub);
                    }

                    auto converter = expressions::RationalFunctionToExpression<ValueType>(manager);

                    // < 0, so not monotone increasing. If this is unsat, then it should be monotone increasing.
                    expressions::Expression exprToCheck = converter.toExpression(derivative) < manager->rational(0);
                    s.add(exprBounds);
                    s.add(exprToCheck);
                    monIncr = s.check() == solver::SmtSolver::CheckResult::Unsat;

                    // > 0, so not monotone decreasing. If this is unsat it should be monotone decreasing.
                    exprToCheck = converter.toExpression(derivative) > manager->rational(0);
                    s.reset();
                    s.add(exprBounds);
                    s.add(exprToCheck);
                    monDecr = s.check() == solver::SmtSolver::CheckResult::Unsat;
                }
                assert (!(monIncr && monDecr) || derivative.isZero());

                return std::pair<bool, bool>(monIncr, monDecr);
            }

            /*!
             * Checks for local monotonicity at the given state.
             *
             * @param order The order on which the monotonicity should be checked.
             * @param state The considerd state.
             * @param var The variable in which we check for monotonicity.
             * @param region The region on which we check the monotonicity.
             * @return Incr, Decr, Constant, Unknown or Not
             */
            Monotonicity checkLocalMonotonicity(std::shared_ptr<Order> const & order, uint_fast64_t state, VariableType const& var, storage::ParameterRegion<ValueType> const& region);

        private:
            Monotonicity checkTransitionMonRes(ValueType function, VariableType param, Region region);

            ValueType& getDerivative(ValueType function, VariableType var);

            storage::SparseMatrix<ValueType> matrix;

            boost::container::flat_map<ValueType, boost::container::flat_map<VariableType, ValueType>> derivatives;
        };
    }
}
#endif //STORM_MONOTONICITYCHECKER_H
