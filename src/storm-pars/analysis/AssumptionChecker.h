//
// Created by Jip Spel on 12.09.18.
//

#ifndef STORM_ASSUMPTIONCHECKER_H
#define STORM_ASSUMPTIONCHECKER_H

#include "storm/logic/Formula.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/environment/Environment.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "Lattice.h"

namespace storm {
    namespace analysis {
        template<typename ValueType>
        class AssumptionChecker {
        public:
            /*!
             * Constructs an AssumptionChecker based on the number of samples, for the given formula and model.
             *
             * @param formula The formula to check.
             * @param model The dtmc model to check the formula on.
             * @param numberOfSamples Number of sample points.
             */
            AssumptionChecker(std::shared_ptr<storm::logic::Formula const> formula, std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples);

            /*!
             * Constructs an AssumptionChecker based on the number of samples, for the given formula and model.
             *
             * @param formula The formula to check.
             * @param model The mdp model to check the formula on.
             * @param numberOfSamples Number of sample points.
             */
            AssumptionChecker(std::shared_ptr<storm::logic::Formula const> formula, std::shared_ptr<storm::models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples);

            /*!
             * Checks if the assumption holds at the sample points of the AssumptionChecker.
             *
             * @param assumption The assumption to check.
             * @return true if the assumption holds at the sample points
             */
            bool checkOnSamples(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption);

            /*!
             * Tries to validate an assumption based on the lattice and underlying transition matrix.
             *
             * @param assumption The assumption to validate.
             * @param lattice The lattice.
             * @return true if the assumption can be validated and holds, false otherwise
             */
            bool validateAssumption(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption, storm::analysis::Lattice* lattice);

            /*!
             * Looks up if assumption has been validated.
             *
             * @param assumption The assumption.
             * @return true if the assumption has been validated.
             */
            bool validated(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption);

            /*!
             * Looks up if assumption is valid. Requires the function to be validated.
             *
             * @param assumption The assumption.
             * @return true if the assumption is valid.
             */
            bool valid(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption);

        private:
            std::shared_ptr<storm::logic::Formula const> formula;

            storm::storage::SparseMatrix<ValueType> matrix;

            std::vector<std::vector<double>> results;

            std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> validatedAssumptions;

            std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> validAssumptions;

            bool validateAssumptionFunction(storm::analysis::Lattice* lattice,
                    typename storm::storage::SparseMatrix<ValueType>::iterator state1succ1,
                    typename storm::storage::SparseMatrix<ValueType>::iterator state1succ2,
                    typename storm::storage::SparseMatrix<ValueType>::iterator state2succ1,
                    typename storm::storage::SparseMatrix<ValueType>::iterator state2succ2);

            bool validateAssumptionSMTSolver(storm::analysis::Lattice* lattice,
                    typename storm::storage::SparseMatrix<ValueType>::iterator state1succ1,
                    typename storm::storage::SparseMatrix<ValueType>::iterator state1succ2,
                    typename storm::storage::SparseMatrix<ValueType>::iterator state2succ1,
                    typename storm::storage::SparseMatrix<ValueType>::iterator state2succ2);

            bool validateAssumptionSMTSolver(storm::analysis::Lattice* lattice,
                    std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption);
        };
    }
}
#endif //STORM_ASSUMPTIONCHECKER_H
