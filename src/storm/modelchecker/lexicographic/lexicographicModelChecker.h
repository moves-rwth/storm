//
// Created by steffi on 05.11.21.
//

#pragma once


#include "storm/modelchecker/results/CheckResult.h"
#include "storm/logic/Formulas.h"
#include "storm/environment/Environment.h"

namespace storm {

    class Environment;

    namespace modelchecker {
        namespace lexicographic {

        template<typename SparseModelType>
        class lexicographicModelChecker {
           public:

            lexicographicModelChecker() = default;

            int getCompleteProductModel(SparseModelType const& model, storm::logic::MultiObjectiveFormula const& formula);

           private:
            void foo();
            void foo2();
        };

        }
    }
}
