//
// Created by Jip Spel on 03.09.18.
//

#ifndef STORM_MONOTONICITYCHECKER_H
#define STORM_MONOTONICITYCHECKER_H

#include <map>
#include "Lattice.h"
#include "LatticeExtender.h"
#include "AssumptionMaker.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/storage/expressions/RationalFunctionToExpression.h"
#include "storm/utility/constants.h"
#include "carl/core/Variable.h"
#include "storm/models/ModelBase.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/logic/Formula.h"
#include "storm/storage/SparseMatrix.h"
#include "storm-pars/api/region.h"
#include "storm/solver/Z3SmtSolver.h"


namespace storm {
    namespace analysis {

        template <typename ValueType>
        class MonotonicityChecker {

        public:
            /*!
             * Constructor of MonotonicityChecker
             * @param model the model considered
             * @param formula the formula considered
             * @param validate whether or not assumptions are to be validated
             * @param numberOfSamples number of samples taken for monotonicity checking, default 0,
             *          if 0then no check on samples is executed
             */
            MonotonicityChecker(std::shared_ptr<storm::models::ModelBase> model, std::vector<std::shared_ptr<storm::logic::Formula const>> formulas, bool validate, uint_fast64_t numberOfSamples=0);

            /*!
             * Checks for model and formula as provided in constructor for monotonicity
             */
            std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> checkMonotonicity();

            /*!
             * Checks if monotonicity can be found in this lattice. Unordered states are not checked
             */
            bool somewhereMonotonicity(storm::analysis::Lattice* lattice) ;

            /*!
             * Checks if a derivative >=0 or/and <=0
             * @param derivative The derivative you want to check
             * @return pair of bools, >= 0 and <= 0
             */
            static std::pair<bool, bool> checkDerivative(ValueType derivative) {
                bool monIncr = false;
                bool monDecr = false;

                if (derivative.isZero()) {
                    monIncr = true;
                    monDecr = true;
                } else if (derivative.isConstant()) {
                    monIncr = derivative.constantPart() >= 0;
                    monDecr = derivative.constantPart() <= 0;
                } else {

                    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
                    std::shared_ptr<storm::expressions::ExpressionManager> manager(
                            new storm::expressions::ExpressionManager());

                    storm::solver::Z3SmtSolver s(*manager);

                    std::set<carl::Variable> variables = derivative.gatherVariables();


                    for (auto variable : variables) {
                        manager->declareRationalVariable(variable.name());

                    }
                    storm::expressions::Expression exprBounds = manager->boolean(true);
                    auto managervars = manager->getVariables();
                    for (auto var : managervars) {
                        exprBounds = exprBounds && manager->rational(0) < var && var < manager->rational(1);
                    }
                    assert (s.check() == storm::solver::SmtSolver::CheckResult::Sat);

                    auto converter = storm::expressions::RationalFunctionToExpression<ValueType>(manager);

                    // < 0 so not monotone increasing
                    storm::expressions::Expression exprToCheck =
                            converter.toExpression(derivative) < manager->rational(0);
                    s.add(exprBounds);
                    s.add(exprToCheck);
                    // If it is unsatisfiable then it should be monotone increasing
                    monIncr = s.check() == storm::solver::SmtSolver::CheckResult::Unsat;

                    // > 0 so not monotone decreasing
                    exprToCheck =
                            converter.toExpression(derivative) > manager->rational(0);

                    s.reset();
                    s.add(exprBounds);
                    assert (s.check() == storm::solver::SmtSolver::CheckResult::Sat);
                    s.add(exprToCheck);
                    monDecr = s.check() == storm::solver::SmtSolver::CheckResult::Unsat;
                }
                assert (!(monIncr && monDecr) || derivative.isZero());

                return std::pair<bool, bool>(monIncr, monDecr);
            }

        private:
            std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> checkMonotonicity(std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> map, storm::storage::SparseMatrix<ValueType> matrix);

            //TODO: variabele type
            std::map<carl::Variable, std::pair<bool, bool>> analyseMonotonicity(uint_fast64_t i, Lattice* lattice, storm::storage::SparseMatrix<ValueType> matrix) ;

            std::map<Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> createLattice();

            std::map<carl::Variable, std::pair<bool, bool>> checkOnSamples(std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples);

            std::map<carl::Variable, std::pair<bool, bool>> checkOnSamples(std::shared_ptr<storm::models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples);

            std::unordered_map<ValueType, std::unordered_map<carl::Variable, ValueType>> derivatives;

            ValueType getDerivative(ValueType function, carl::Variable var);

            std::map<Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> extendLatticeWithAssumptions(Lattice* lattice, AssumptionMaker<ValueType>* assumptionMaker, uint_fast64_t val1, uint_fast64_t val2, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions);

            std::shared_ptr<storm::models::ModelBase> model;

            std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;

            bool validate;

            bool checkSamples;

            std::map<carl::Variable, std::pair<bool, bool>> resultCheckOnSamples;

            LatticeExtender<ValueType> *extender;

            std::ofstream outfile;

            std::string filename = "monotonicity.txt";
        };
    }
}
#endif //STORM_MONOTONICITYCHECKER_H
