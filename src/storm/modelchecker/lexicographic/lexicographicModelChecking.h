//
// Created by steffi on 04.11.21.
//
#pragma once

#include <memory>

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/logic/Formulas.h"
#include "storm/environment/Environment.h"
#include "storm/modelchecker/lexicographic/lexicographicModelCheckerHelper.h"

namespace storm {

    class Environment;

    namespace modelchecker {
        namespace lexicographic {
            typedef std::function<storm::storage::BitVector(storm::logic::Formula const&)> CheckFormulaCallback;

            /**
             * check a lexicographic LTL-formula
             */
            template<typename SparseModelType, typename ValueType>
            helper::MDPSparseModelCheckingHelperReturnType<ValueType> check(Environment const& env, SparseModelType const& model,  CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask, CheckFormulaCallback const& formulaChecker);

        }
    }
}