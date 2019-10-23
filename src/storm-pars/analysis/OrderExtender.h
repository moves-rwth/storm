#ifndef STORM_LATTICEEXTENDER_H
#define STORM_LATTICEEXTENDER_H

#include <storm/logic/Formula.h>
#include "storm/models/sparse/Dtmc.h"
#include "storm-pars/analysis/Order.h"
#include "storm/api/storm.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/StronglyConnectedComponent.h"



namespace storm {
    namespace analysis {


        template<typename ValueType>
        class OrderExtender {

        public:
            /*!
             * Constructs OrderExtender which can extend an order
             *
             * @param model The model for which the order should be extended.
             */
            OrderExtender(std::shared_ptr<storm::models::sparse::Model<ValueType>> model);

            /*!
             * Creates an order based on the given formula.
             *
             * @param formulas The formulas based on which the order is created, only the first is used.
             * @return A triple with a pointer to the order and two states of which the current place in the order
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced but needed.
             */
            std::tuple<storm::analysis::Order*, uint_fast64_t, uint_fast64_t> toOrder(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas);

            /*!
             * Creates an order based on the given extremal values.
             *
             * @return A triple with a pointer to the order and two states of which the current place in the order
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced but needed.
             */
            std::tuple<storm::analysis::Order*, uint_fast64_t, uint_fast64_t> toOrder(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas, std::vector<double> minValues, std::vector<double> maxValues);


            /*!
             * Extends the order based on the given assumption.
             *
             * @param order The order.
             * @param assumption The assumption on states.
             * @return A triple with a pointer to the order and two states of which the current place in the order
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced but needed.
             */
            std::tuple<storm::analysis::Order*, uint_fast64_t, uint_fast64_t> extendOrder(storm::analysis::Order* order, std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption = nullptr);


        private:
            std::shared_ptr<storm::models::sparse::Model<ValueType>> model;

            std::map<uint_fast64_t, storm::storage::BitVector*> stateMap;

            bool acyclic;

            bool assumptionSeen;

            storm::storage::StronglyConnectedComponentDecomposition<ValueType> sccs;

            storm::storage::SparseMatrix<ValueType> matrix;

            void handleAssumption(Order* order, std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption);

            std::tuple<Order*, uint_fast64_t, uint_fast64_t> extendAllSuccAdded(Order* order, uint_fast64_t const & stateNumber, storm::storage::BitVector* successors);

            std::tuple<bool, uint_fast64_t, uint_fast64_t> allSuccAdded(Order* order, uint_fast64_t stateNumber);
        };
    }
}

#endif //STORM_ORDEREXTENDER_H
