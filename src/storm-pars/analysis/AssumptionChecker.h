#ifndef STORM_ASSUMPTIONCHECKER_H
#define STORM_ASSUMPTIONCHECKER_H

#include "storm/logic/Formula.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/environment/Environment.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "Order.h"

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
            /*!
             * Constructs an AssumptionChecker based on the number of samples, for the given formula and model.
             *
             * @param formula The formula to check.
             * @param model The dtmc model to check the formula on.
             * @param numberOfSamples Number of sample points.
             */
            AssumptionChecker(std::shared_ptr<logic::Formula const> formula, std::shared_ptr<models::sparse::Dtmc<ValueType>> model, storage::ParameterRegion<ValueType> region, uint_fast64_t numberOfSamples);

            /*!
             * Constructs an AssumptionChecker based on the number of samples, for the given formula and model.
             *
             * @param formula The formula to check.
             * @param model The mdp model to check the formula on.
             * @param numberOfSamples Number of sample points.
             */
            AssumptionChecker(std::shared_ptr<logic::Formula const> formula, std::shared_ptr<models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples);

            /*!
             * Checks if the assumption holds at the sample points of the AssumptionChecker.
             *
             * @param assumption The assumption to check.
             * @return AssumptionStatus::UNKNOWN or AssumptionStatus::INVALID
             */
            AssumptionStatus checkOnSamples(std::shared_ptr<expressions::BinaryRelationExpression> assumption);

            /*!
             * Tries to validate an assumption based on the order and underlying transition matrix.
             *
             * @param assumption The assumption to validate.
             * @param order The order.
             * @return AssumptionStatus::VALID, or AssumptionStatus::UNKNOWN, or AssumptionStatus::INVALID
             */
            AssumptionStatus validateAssumption(std::shared_ptr<expressions::BinaryRelationExpression> assumption, Order* order);

            /*!
             * Tries to validate an assumption based on the order, and SMT solving techniques
             *
             * @param assumption The assumption to validate.
             * @param order The order.
             * @return AssumptionStatus::VALID, or AssumptionStatus::UNKNOWN, or AssumptionStatus::INVALID
             */
            AssumptionStatus validateAssumptionSMTSolver(std::shared_ptr<expressions::BinaryRelationExpression> assumption, Order* order);

        private:
            std::shared_ptr<logic::Formula const> formula;

            storage::SparseMatrix<ValueType> matrix;

            std::vector<std::vector<ConstantType>> samples;

            storage::ParameterRegion<ValueType> region;
        };
    }
}
#endif //STORM_ASSUMPTIONCHECKER_H
