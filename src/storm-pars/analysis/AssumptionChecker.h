#ifndef STORM_ASSUMPTIONCHECKER_H
#define STORM_ASSUMPTIONCHECKER_H

#include "storm/logic/Formula.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/environment/Environment.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "Order.h"
#include "storm/storage/SparseMatrix.h"


namespace storm {
    namespace analysis {
        /*!
         * Constants for status of assumption
         */
        enum AssumptionStatus {
            VALID,
            INVALID,
            UNKNOWN,
        };

        template<typename ValueType, typename ConstantType>
        class AssumptionChecker {
        public:
            typedef typename utility::parametric::VariableType<ValueType>::type VariableType;
            typedef typename utility::parametric::CoefficientType<ValueType>::type CoefficientType;
            // TODO: @Svenja, could you update the documentation of the public methods?
            /*!
             * Constructs an AssumptionChecker.
             */
            AssumptionChecker(storage::SparseMatrix<ValueType> matrix);

            /*!
             * Constructs an AssumptionChecker based on the number of samples, for the given formula and model.
             *
             * @param formula The formula to check.
             * @param model The mdp model to check the formula on.
             * @param numberOfSamples Number of sample points.
             */
            AssumptionChecker(std::shared_ptr<logic::Formula const> formula, std::shared_ptr<models::sparse::Mdp<ValueType>> model, uint_fast64_t const numberOfSamples);

            void initializeCheckingOnSamples(std::shared_ptr<logic::Formula const> formula, std::shared_ptr<models::sparse::Dtmc<ValueType>> model, storage::ParameterRegion<ValueType> region, uint_fast64_t numberOfSamples);

            void setSampleValues(std::vector<std::vector<ConstantType>> samples);

            /*!
             * Tries to validate an assumption based on the order and underlying transition matrix.
             *
             * @param assumption The assumption to validate.
             * @param order The order.
             * @return AssumptionStatus::VALID, or AssumptionStatus::UNKNOWN, or AssumptionStatus::INVALID
             */
            AssumptionStatus validateAssumption(std::shared_ptr<expressions::BinaryRelationExpression> assumption, std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region) const;

        private:
            bool useSamples;

            std::vector<std::vector<ConstantType>> samples;

            storage::SparseMatrix<ValueType> matrix;

            AssumptionStatus validateAssumptionSMTSolver(std::shared_ptr<expressions::BinaryRelationExpression> assumption, std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region) const;

            AssumptionStatus checkOnSamples(std::shared_ptr<expressions::BinaryRelationExpression> assumption) const;
        };
    }
}
#endif //STORM_ASSUMPTIONCHECKER_H
