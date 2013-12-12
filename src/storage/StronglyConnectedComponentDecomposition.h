#ifndef STORM_STORAGE_STRONGLYCONNECTEDCOMPONENTDECOMPOSITION_H_
#define STORM_STORAGE_STRONGLYCONNECTEDCOMPONENTDECOMPOSITION_H_

#include "src/storage/Decomposition.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace models {
        // Forward declare the abstract model class.
        template <typename ValueType> class AbstractModel;
    }
    
    namespace storage {
        
        /*!
         * This class represents the decomposition of a graph-like structure into its strongly connected components.
         */
        template <typename ValueType>
        class StronglyConnectedComponentDecomposition : public Decomposition<StateBlock> {
        public:
            /*
             * Creates an empty SCC decomposition.
             */
            StronglyConnectedComponentDecomposition();
            
            /*
             * Creates an SCC decomposition of the given model.
             *
             * @param model The model to decompose into SCCs.
             * @param dropNaiveSccs A flag that indicates whether trivial SCCs (i.e. SCCs consisting of just one state
             * without a self-loop) are to be kept in the decomposition.
             * @param onlyBottomSccs If set to true, only bottom SCCs, i.e. SCCs in which all states have no way of
             * leaving the SCC), are kept.
             */
            StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, bool dropNaiveSccs = false, bool onlyBottomSccs = false);
            
            /*
             * Creates an SCC decomposition of the given block in the given model.
             *
             * @param model The model whose block to decompose.
             * @param block The block to decompose into SCCs.
             * @param dropNaiveSccs A flag that indicates whether trivial SCCs (i.e. SCCs consisting of just one state
             * without a self-loop) are to be kept in the decomposition.
             * @param onlyBottomSccs If set to true, only bottom SCCs, i.e. SCCs in which all states have no way of
             * leaving the SCC), are kept.
             */
            StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, StateBlock const& block, bool dropNaiveSccs = false, bool onlyBottomSccs = false);
            
            /*
             * Creates an SCC decomposition of the given subsystem in the given model.
             *
             * @param model The model that contains the block.
             * @param subsystem A bit vector indicating which subsystem to consider for the decomposition into SCCs.
             * @param dropNaiveSccs A flag that indicates whether trivial SCCs (i.e. SCCs consisting of just one state
             * without a self-loop) are to be kept in the decomposition.
             * @param onlyBottomSccs If set to true, only bottom SCCs, i.e. SCCs in which all states have no way of
             * leaving the SCC), are kept.
             */
            StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs = false, bool onlyBottomSccs = false);
            
            /*!
             * Creates an SCC decomposition by copying the given SCC decomposition.
             *
             * @oaram other The SCC decomposition to copy.
             */
            StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition const& other);
            
            /*!
             * Assigns the contents of the given SCC decomposition to the current one by copying its contents.
             *
             * @oaram other The SCC decomposition from which to copy-assign.
             */
            StronglyConnectedComponentDecomposition& operator=(StronglyConnectedComponentDecomposition const& other);
            
            /*!
             * Creates an SCC decomposition by moving the given SCC decomposition.
             *
             * @oaram other The SCC decomposition to move.
             */
            StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition&& other);
            
            /*!
             * Assigns the contents of the given SCC decomposition to the current one by moving its contents.
             *
             * @oaram other The SCC decomposition from which to copy-assign.
             */
            StronglyConnectedComponentDecomposition& operator=(StronglyConnectedComponentDecomposition&& other);
            
        private:
            /*!
             * Performs the SCC decomposition of the given model. As a side-effect this fills the vector of
             * blocks of the decomposition.
             *
             * @param model The model to decompose into SCCs.
             * @param dropNaiveSccs A flag that indicates whether trivial SCCs (i.e. SCCs consisting of just one state
             * without a self-loop) are to be kept in the decomposition.
             * @param onlyBottomSccs If set to true, only bottom SCCs, i.e. SCCs in which all states have no way of
             * leaving the SCC), are kept.
             */
            void performSccDecomposition(storm::models::AbstractModel<ValueType> const& model, bool dropNaiveSccs, bool onlyBottomSccs);
            
            /*
             * Performs the SCC decomposition of the given block in the given model. As a side-effect this fills
             * the vector of blocks of the decomposition.
             *
             * @param model The model that contains the block.
             * @param subsystem A bit vector indicating which subsystem to consider for the decomposition into SCCs.
             * @param dropNaiveSccs A flag that indicates whether trivial SCCs (i.e. SCCs consisting of just one state
             * without a self-loop) are to be kept in the decomposition.
             * @param onlyBottomSccs If set to true, only bottom SCCs, i.e. SCCs in which all states have no way of
             * leaving the SCC), are kept.
             */
            void performSccDecomposition(storm::models::AbstractModel<ValueType> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs);
            
            /*!
             * A helper function that performs the SCC decomposition given all auxiliary data structures. As a
             * side-effect this fills the vector of blocks of the decomposition.
             *
             * @param model The model to decompose into SCCs.
             * @param startState The starting state for the search of Tarjan's algorithm.
             * @param subsystem The subsystem to search.
             * @param currentIndex The next free index that can be assigned to states.
             * @param stateIndices A vector storing the index for each state.
             * @param lowlinks A vector storing the lowlink for each state.
             * @param tarjanStack A vector that is to be used as the stack in Tarjan's algorithm.
             * @param tarjanStackStates A bit vector indicating which states are currently in the stack.
             * @param visitedStates A bit vector containing all states that have already been visited.
             * @param dropNaiveSccs A flag that indicates whether trivial SCCs (i.e. SCCs consisting of just one state
             * without a self-loop) are to be kept in the decomposition.
             * @param onlyBottomSccs If set to true, only bottom SCCs, i.e. SCCs in which all states have no way of
             * leaving the SCC), are kept.
             */
            void performSccDecompositionHelper(storm::models::AbstractModel<ValueType> const& model, uint_fast64_t startState, storm::storage::BitVector const& subsystem, uint_fast64_t& currentIndex, std::vector<uint_fast64_t>& stateIndices, std::vector<uint_fast64_t>& lowlinks, std::vector<uint_fast64_t>& tarjanStack, storm::storage::BitVector& tarjanStackStates, storm::storage::BitVector& visitedStates, bool dropNaiveSccs, bool onlyBottomSccs);
        };
    }
}

#endif /* STORM_STORAGE_STRONGLYCONNECTEDCOMPONENTDECOMPOSITION_H_ */
