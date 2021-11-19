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
//#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"

namespace storm {

    class Environment;

    namespace modelchecker {
        namespace lexicographic {

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        class lexicographicModelChecker : public helper::SingleValueModelCheckerHelper<ValueType, storm::models::ModelRepresentation::Sparse> {
           public:
            typedef std::function<storm::storage::BitVector(storm::logic::Formula const&)> CheckFormulaCallback;
            using productModelType = typename std::conditional<Nondeterministic, storm::models::sparse::Mdp<ValueType>, storm::models::sparse::Dtmc<ValueType>>::type;

            lexicographicModelChecker(storm::logic::MultiObjectiveFormula const& formula, storm::storage::SparseMatrix<ValueType> const& transitionMatrix) : _transitionMatrix(transitionMatrix), formula(formula) {};

            std::pair<std::shared_ptr<storm::transformer::DAProduct<SparseModelType>>, std::vector<uint>> getCompleteProductModel(SparseModelType const& model, CheckFormulaCallback const& formulaChecker);

            std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>, std::vector<std::vector<bool>>> solve(std::shared_ptr<storm::transformer::DAProduct<productModelType>> productModel, std::vector<uint>& acceptanceConditions, storm::logic::MultiObjectiveFormula const& formula);

            int reachability(storm::storage::MaximalEndComponentDecomposition<ValueType> const& bcc, std::vector<std::vector<bool>> const& bccLexArray, std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> const& productModel);

            storm::storage::MaximalEndComponentDecomposition<ValueType> computeECs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions,  typename transformer::DAProduct<productModelType>::ptr product);

           private:
            storm::logic::MultiObjectiveFormula const& formula;
            storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;

            static std::map<std::string, storm::storage::BitVector> computeApSets(std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, CheckFormulaCallback const& formulaChecker);

            std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> getStreettPairs(storm::automata::AcceptanceCondition::acceptance_expr::ptr current);

            bool isAcceptingCNF(storm::storage::MaximalEndComponent const& scc, std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> const& acceptancePairs, storm::automata::AcceptanceCondition::ptr const& acceptance);
            bool isAcceptingPair(storm::storage::MaximalEndComponent const& scc, storm::automata::AcceptanceCondition::acceptance_expr::ptr const& left, storm::automata::AcceptanceCondition::acceptance_expr::ptr const& right, storm::automata::AcceptanceCondition::ptr const& acceptance);
        };

        }
    }
}
