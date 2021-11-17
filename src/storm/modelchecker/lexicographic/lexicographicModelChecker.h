//
// Created by steffi on 05.11.21.
//

#pragma once

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/logic/Formulas.h"
#include "storm/environment/Environment.h"
#include "storm/storage/BitVector.h"
#include "storm/automata/LTL2DeterministicAutomaton.h"

namespace storm {

    class Environment;

    namespace modelchecker {
        namespace lexicographic {

        template<typename SparseModelType, typename ValueType>
        class lexicographicModelChecker {
           public:
            typedef std::function<storm::storage::BitVector(storm::logic::Formula const&)> CheckFormulaCallback;

            lexicographicModelChecker(storm::logic::MultiObjectiveFormula const& formula) : formula(formula) {};

            std::pair<int, int> getCompleteProductModel(SparseModelType const& model, CheckFormulaCallback const& formulaChecker);

            std::pair<int, int> solve(int productModel, int acceptanceCondition);

            int reachability(int bcc, int bccLexArray, int productModel);

           private:
            storm::logic::MultiObjectiveFormula const& formula;

            static std::map<std::string, storm::storage::BitVector> computeApSets(std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, CheckFormulaCallback const& formulaChecker);

            int ltl2daSpotProduct(CheckFormulaCallback const& formulaChecker, SparseModelType const& model);
        };

        }
    }
}
