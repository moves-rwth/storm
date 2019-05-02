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
            MonotonicityChecker(std::shared_ptr<storm::models::ModelBase> model, std::vector<std::shared_ptr<storm::logic::Formula const>> formulas, bool validate);
            /*!
             * Checks for all lattices in the map if they are monotone increasing or monotone decreasing.
             *
             * @param map The map with lattices and the assumptions made to create the lattices.
             * @param matrix The transition matrix.
             * @return TODO
             */
            std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> checkMonotonicity(std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> map, storm::storage::SparseMatrix<ValueType> matrix);

            /*!
             * TODO
             * @return
             */
            std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> checkMonotonicity();

            /*!
             * TODO
             * @param lattice
             * @param matrix
             * @return
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
                    storm::solver::SmtSolver::CheckResult smtResult = storm::solver::SmtSolver::CheckResult::Unknown;

                    std::set<carl::Variable> variables = derivative.gatherVariables();


                    for (auto variable : variables) {
                        manager->declareRationalVariable(variable.name());

                    }
                    storm::expressions::Expression exprBounds = manager->boolean(true);
                    auto managervars = manager->getVariables();
                    for (auto var : managervars) {
                        exprBounds = exprBounds && manager->rational(0) < var && var < manager->rational(1);
                    }

                    auto converter = storm::expressions::RationalFunctionToExpression<ValueType>(manager);

                    storm::expressions::Expression exprToCheck1 =
                            converter.toExpression(derivative) >= manager->rational(0);
                    s.add(exprBounds);
                    s.add(exprToCheck1);
                    smtResult = s.check();
                    monIncr = smtResult == storm::solver::SmtSolver::CheckResult::Sat;

                    storm::expressions::Expression exprToCheck2 =
                            converter.toExpression(derivative) <= manager->rational(0);
                    s.reset();
                    smtResult = storm::solver::SmtSolver::CheckResult::Unknown;
                    s.add(exprBounds);
                    s.add(exprToCheck2);
                    smtResult = s.check();
                    monDecr = smtResult == storm::solver::SmtSolver::CheckResult::Sat;
                    if (monIncr && monDecr) {
                        monIncr = false;
                        monDecr = false;
                    }
                }
                assert (!(monIncr && monDecr) || derivative.isZero());

                return std::pair<bool, bool>(monIncr, monDecr);
            }

        private:
            //TODO: variabele type
            std::map<carl::Variable, std::pair<bool, bool>> analyseMonotonicity(uint_fast64_t i, storm::analysis::Lattice* lattice, storm::storage::SparseMatrix<ValueType> matrix) ;

            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> createLattice();

            std::map<carl::Variable, std::pair<bool, bool>> checkOnSamples(std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples);

            std::map<carl::Variable, std::pair<bool, bool>> checkOnSamples(std::shared_ptr<storm::models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples);

            std::unordered_map<ValueType, std::unordered_map<carl::Variable, ValueType>> derivatives;

            ValueType getDerivative(ValueType function, carl::Variable var);

            std::vector<storm::storage::ParameterRegion<ValueType>> checkAssumptionsOnRegion(std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions);

            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> extendLatticeWithAssumptions(storm::analysis::Lattice* lattice, storm::analysis::AssumptionMaker<ValueType>* assumptionMaker, uint_fast64_t val1, uint_fast64_t val2, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions);

            std::shared_ptr<storm::models::ModelBase> model;

            std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;

            bool validate;

            std::map<carl::Variable, std::pair<bool, bool>> resultCheckOnSamples;

            storm::analysis::LatticeExtender<ValueType> *extender;

            std::ofstream outfile;

            std::string filename = "results.txt";

            storm::utility::Stopwatch totalWatch;
        };
    }
}
#endif //STORM_MONOTONICITYCHECKER_H
