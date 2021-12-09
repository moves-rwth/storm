//
// Created by steffi on 05.11.21.
//

#pragma once
#include "storm/modelchecker/helper/SingleValueModelCheckerHelper.h"

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/logic/Formulas.h"
#include "storm/environment/Environment.h"
#include "storm/storage/BitVector.h"
#include "storm/models/ModelRepresentation.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/transformer/DAProductBuilder.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/modelchecker/prctl/helper/MDPModelCheckingHelperReturnType.h"
#include "storm/transformer/SubsystemBuilder.h"

namespace storm {

    class Environment;

    namespace modelchecker {
    namespace helper {
    namespace lexicographic {

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    class lexicographicModelCheckerHelper : public helper::SingleValueModelCheckerHelper<ValueType, storm::models::ModelRepresentation::Sparse> {
       public:
        typedef std::function<storm::storage::BitVector(storm::logic::Formula const&)> CheckFormulaCallback;
        typedef storm::transformer::SubsystemBuilderReturnType<ValueType, storm::models::sparse::StandardRewardModel<ValueType>> SubsystemReturnType;
        using StateType = typename storm::storage::sparse::state_type;
        // using productModelType = typename std::conditional<Nondeterministic, storm::models::sparse::Mdp<ValueType>, storm::models::sparse::Dtmc<ValueType>>::type;
        using productModelType = typename storm::models::sparse::Mdp<ValueType>;

        lexicographicModelCheckerHelper(storm::logic::MultiObjectiveFormula const& formula, storm::storage::SparseMatrix<ValueType> const& transitionMatrix)
            : _transitionMatrix(transitionMatrix), formula(formula){};

        std::pair<std::shared_ptr<storm::transformer::DAProduct<SparseModelType>>, std::vector<uint>> getCompleteProductModel(
            SparseModelType const& model, CheckFormulaCallback const& formulaChecker);

        std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>, std::vector<std::vector<bool>>> solve(
            std::shared_ptr<storm::transformer::DAProduct<productModelType>> productModel, std::vector<uint>& acceptanceConditions,
            storm::storage::BitVector& allowed);

        MDPSparseModelCheckingHelperReturnType<ValueType> reachability(
            storm::storage::MaximalEndComponentDecomposition<ValueType> const& bcc, std::vector<std::vector<bool>> const& bccLexArray,
            std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> const& productModel, storm::storage::BitVector& allowed,
            SparseModelType const& originalMdp, ValueType& resultingProb);

        std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>, storm::storage::BitVector> computeECs(
            automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
            storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector& allowed);

       private:
        storm::logic::MultiObjectiveFormula const& formula;
        storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;

        static std::map<std::string, storm::storage::BitVector> computeApSets(
            std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, CheckFormulaCallback const& formulaChecker);

        std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> getStreettPairs(
            storm::automata::AcceptanceCondition::acceptance_expr::ptr const& current);

        bool isAcceptingStreettConditions(storm::storage::MaximalEndComponent const& scc,
                                          std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> const& acceptancePairs,
                                          storm::automata::AcceptanceCondition::ptr const& acceptance, productModelType model);
        bool isAcceptingPair(storm::storage::MaximalEndComponent const& scc, storm::automata::AcceptanceCondition::acceptance_expr::ptr const& left,
                             storm::automata::AcceptanceCondition::acceptance_expr::ptr const& right,
                             storm::automata::AcceptanceCondition::ptr const& acceptance);

        storm::storage::BitVector getGoodStates(storm::storage::MaximalEndComponentDecomposition<ValueType> const& bcc,
                                                std::vector<std::vector<bool>> const& bccLexArray,
                                                std::vector<uint_fast64_t> const& oldToNewStateMapping,
                                                uint const& condition,
                                                uint const numStates, std::vector<uint_fast64_t> const& compressedToReducedMapping);

        MDPSparseModelCheckingHelperReturnType<ValueType> solveOneReachability(std::vector<uint_fast64_t>& newInitalStates,
                                                                                storm::storage::BitVector const& psiStates,
                                                                                storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                                SparseModelType const& originalMdp,
                                                                                std::vector<uint_fast64_t> const& compressedToReducedMapping,
                                                                                std::vector<uint_fast64_t> const& oldToNewStateMapping);

        SubsystemReturnType getReducedSubsystem(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                MDPSparseModelCheckingHelperReturnType<ValueType> const& reachabilityResult,
                                                std::vector<uint_fast64_t> const& newInitalStates,
                                                storm::storage::BitVector const& goodStates);

        int removeTransientSCCs(std::vector<std::vector<bool>>& bccLexArray,
                                uint const& condition,
                                storm::storage::MaximalEndComponentDecomposition<ValueType> const& bcc,
                                std::vector<uint_fast64_t> const& compressedToReducedMapping,
                                std::vector<uint_fast64_t> const& oldToNewStateMapping,
                                MDPSparseModelCheckingHelperReturnType<ValueType> const& res);
    };

    } // lexicographic
    } // helper
    } // modelchecker
} // storm
