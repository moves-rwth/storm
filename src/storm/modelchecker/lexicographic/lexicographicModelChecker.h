//
// Created by steffi on 05.11.21.
//

#pragma once


#include "storm/modelchecker/results/CheckResult.h"
#include "storm/logic/Formulas.h"
#include "storm/environment/Environment.h"
#include <utility>

namespace storm {

    class Environment;

    namespace modelchecker {
        namespace lexicographic {

        template<typename SparseModelType, typename ValueType>
        class lexicographicModelChecker {
           public:

            lexicographicModelChecker(storm::logic::MultiObjectiveFormula const& formula) : formula(formula) {};

            std::pair<int, int> getCompleteProductModel(SparseModelType const& model);

            std::pair<int, int> solve(int productModel, int acceptanceCondition);

            int reachability(int bcc, int bccLexArray, int productModel);

           private:
            storm::logic::MultiObjectiveFormula const& formula;
        };

        }
    }
}
