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
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

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

            std::pair<std::shared_ptr<storm::transformer::DAProduct<SparseModelType>>, std::vector<std::shared_ptr<storm::automata::AcceptanceCondition>>> getCompleteProductModel(SparseModelType const& model, CheckFormulaCallback const& formulaChecker);

            std::pair<storm::storage::StronglyConnectedComponentDecomposition<ValueType>, std::vector<std::vector<bool>>> solve(std::shared_ptr<storm::transformer::DAProduct<productModelType>> productModel, std::vector<std::shared_ptr<storm::automata::AcceptanceCondition>>& acceptanceConditions, storm::logic::MultiObjectiveFormula const& formula);

            int reachability(storm::storage::StronglyConnectedComponentDecomposition<ValueType> bcc, std::vector<std::vector<bool>> bccLexArray, std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> productModel);

            storm::storage::StronglyConnectedComponentDecomposition<ValueType> computeECs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions,  typename transformer::DAProduct<productModelType>::ptr product);

           private:
            storm::logic::MultiObjectiveFormula const& formula;
            storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;

            static std::map<std::string, storm::storage::BitVector> computeApSets(std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, CheckFormulaCallback const& formulaChecker);


        };

        }
    }
}
