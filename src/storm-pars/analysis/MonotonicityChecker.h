#ifndef STORM_MONOTONICITYCHECKER_H
#define STORM_MONOTONICITYCHECKER_H

#include <map>
#include "Order.h"
#include "OrderExtender.h"
#include "AssumptionMaker.h"

#include "storm/logic/Formula.h"

#include "storm/models/ModelBase.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"

#include "storm/solver/Z3SmtSolver.h"

#include "storm/storage/SparseMatrix.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/RationalFunctionToExpression.h"

#include "storm/utility/constants.h"

#include "storm-pars/api/region.h"
#include "MonotonicityResult.h"

// TODO: Use monotonicityResult instead of pair of bools
// TODO: Update monotonicity while creating the order

namespace storm {
    namespace analysis {

        template <typename ValueType, typename ConstantType>
        class MonotonicityChecker {

        public:
            typedef typename utility::parametric::VariableType<ValueType>::type VariableType;
            typedef typename utility::parametric::CoefficientType<ValueType>::type CoefficientType;
            /*!
             * Constructor of MonotonicityChecker
             * @param model the model considered
             * @param formula the formula considered
             * @param regions the regions to consider
             * @param numberOfSamples number of samples taken for monotonicity checking, default 0,
             *          if 0 then no check on samples is executed
             * @param precision precision on which the samples are compared
             */
            MonotonicityChecker(std::shared_ptr<models::ModelBase> model, std::vector<std::shared_ptr<logic::Formula const>> formulas, std::vector<storage::ParameterRegion<ValueType>> regions, uint_fast64_t numberOfSamples=0, double const& precision=0.000001, bool dotOutput = false);

            /*!
             * Checks for given min/maxValues monotonicity
             * Will not make any assumptions
             * @param minValues lower bound on probabilities for the states
             * @param maxValues upper bound on probabilities for the states
             * @return a map with for each parameter the monotonicity result
             */
            MonotonicityResult<VariableType> checkMonotonicity(std::vector<ConstantType> minValues, std::vector<ConstantType> maxValues);

            /*!
             * Checks if monotonicity can be found in this order. Unordered states are not checked
             */
            bool somewhereMonotonicity(analysis::Order* order) ;

            /*!
             * Checks if a derivative >=0 or/and <=0
             * @param derivative The derivative you want to check
             * @return pair of bools, >= 0 and <= 0
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
             * Builds Reachability Orders for the given model and simultaneously uses them to check for Monotonicity
             */
            std::map<analysis::Order*, std::pair<std::shared_ptr<MonotonicityResult<VariableType>>, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>>> checkMonotonicityInBuild(std::ostream& outfile, std::string dotOutfileName = "dotOutput");


        private:
            std::map<analysis::Order*, std::map<VariableType, std::pair<bool, bool>>> checkMonotonicity(std::ostream& outfile, std::map<analysis::Order*, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>> map);

            std::map<VariableType, std::pair<bool, bool>> analyseMonotonicity(uint_fast64_t i, Order* order) ;

            MonotonicityResult<VariableType> analyseMonotonicity(Order* order) ;

            void createOrder();

            Order* createOrder(std::vector<ConstantType> minValues, std::vector<ConstantType> maxValues);

            std::map<VariableType, std::pair<bool, bool>> checkMonotonicityOnSamples(std::shared_ptr<models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples);

            std::map<VariableType, std::pair<bool, bool>> checkMonotonicityOnSamples(std::shared_ptr<models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples);

            std::unordered_map<ValueType, std::unordered_map<VariableType, ValueType>> derivatives;

            ValueType getDerivative(ValueType function, VariableType var);

            void extendOrderWithAssumptions(Order* order, AssumptionMaker<ValueType, ConstantType>* assumptionMaker, uint_fast64_t val1, uint_fast64_t val2, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>> assumptions, std::shared_ptr<MonotonicityResult<VariableType>> monRes);

            std::shared_ptr<models::ModelBase> model;

            std::vector<std::shared_ptr<logic::Formula const>> formulas;

            bool dotOutput;

            bool checkSamples;

            std::map<VariableType, std::pair<bool, bool>> resultCheckOnSamples;

            std::map<analysis::Order*, std::pair<std::shared_ptr<MonotonicityResult<VariableType>>, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>>> monResults;

            OrderExtender<ValueType, ConstantType> *extender;

            ConstantType precision;

            storage::ParameterRegion<ValueType> region;

            storage::SparseMatrix<ValueType> matrix;

            void checkParOnStateMonRes(uint_fast64_t s, const std::vector<uint_fast64_t>& succ, VariableType param, std::shared_ptr<MonotonicityResult<VariableType>> monResult);

            typename MonotonicityResult<VariableType>::Monotonicity checkTransitionMonRes(uint_fast64_t from, uint_fast64_t to, VariableType param);

            std::pair<std::vector<double>, std::vector<double>> getMinMaxValues();

            ValueType getMatrixEntry(uint_fast64_t rowIndex, uint_fast64_t columnIndex);
        };
    }
}
#endif //STORM_MONOTONICITYCHECKER_H
