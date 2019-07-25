#ifndef STORM_LATTICEEXTENDER_H
#define STORM_LATTICEEXTENDER_H

#include <storm/logic/Formula.h>
#include "storm/models/sparse/Dtmc.h"
#include "storm-pars/analysis/Lattice.h"
#include "storm/api/storm.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/StronglyConnectedComponent.h"



namespace storm {
    namespace analysis {


        template<typename ValueType>
        class LatticeExtender {

        public:
            /*!
             * Constructs LatticeExtender which can extend a lattice
             *
             * @param model The model for which the lattice should be extended.
             */
            LatticeExtender(std::shared_ptr<storm::models::sparse::Model<ValueType>> model);

            /*!
             * Creates a lattice based on the given formula.
             *
             * @param formulas The formulas based on which the lattice is created, only the first is used.
             * @return A triple with a pointer to the lattice and two states of which the current place in the lattice
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced but needed.
             */
            std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> toLattice(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas);

            /*!
             * Creates a lattice based on the given extremal values.
             *
             * @return A triple with a pointer to the lattice and two states of which the current place in the lattice
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced but needed.
             */
            std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> toLattice(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas, std::vector<double> minValues, std::vector<double> maxValues);


            /*!
             * Extends the lattice based on the given assumption.
             *
             * @param lattice The lattice.
             * @param assumption The assumption on states.
             * @return A triple with a pointer to the lattice and two states of which the current place in the lattice
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced but needed.
             */
            std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> extendLattice(storm::analysis::Lattice* lattice, std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption = nullptr);


        private:
            std::shared_ptr<storm::models::sparse::Model<ValueType>> model;

            std::map<uint_fast64_t, storm::storage::BitVector*> stateMap;

            std::vector<uint_fast64_t> statesSorted;

            bool acyclic;

            bool assumptionSeen;

            storm::storage::BitVector* statesToHandle;

            storm::storage::StronglyConnectedComponentDecomposition<ValueType> sccs;

            storm::storage::SparseMatrix<ValueType> matrix;

            void handleAssumption(Lattice* lattice, std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption);

            std::tuple<Lattice*, uint_fast64_t, uint_fast64_t> extendAllSuccAdded(Lattice* lattice, uint_fast64_t const & stateNumber, storm::storage::BitVector* successors);
        };
    }
}

#endif //STORM_LATTICEEXTENDER_H
