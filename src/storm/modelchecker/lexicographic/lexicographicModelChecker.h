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

            std::pair<int, int> getCompleteProductModel(SparseModelType const& model, CheckFormulaCallback const& formulaChecker);

            std::pair<int, int> solve(int productModel, int acceptanceCondition);

            int reachability(int bcc, int bccLexArray, int productModel);

           private:
            storm::logic::MultiObjectiveFormula const& formula;
            storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;

            static std::map<std::string, storm::storage::BitVector> computeApSets(std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, CheckFormulaCallback const& formulaChecker);


        };

        }
    }
}
