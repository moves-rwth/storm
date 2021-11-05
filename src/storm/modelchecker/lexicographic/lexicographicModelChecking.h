//
// Created by steffi on 04.11.21.
//
#pragma once

#include <memory>

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/AbstractModelChecker.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/environment/Environment.h"

namespace storm {

    class Environment;

    namespace modelchecker {
        namespace lexicographic {


            //template<typename SparseModelType>
            //std::unique_ptr<CheckResult> performLexicographicModelChecking(Environment const& env, SparseModelType const& model, storm::logic::MultiObjectiveFormula const& formula);
            template<typename SparseModelType, typename ValueType>
            int isDone(Environment const& env, SparseModelType const& model,  CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask);

            template<typename SparseModelType>
            int getCompleteProductModel(SparseModelType const& model, storm::logic::MultiObjectiveFormula const& formula);
            // instead of formula  CheckTask<storm::logic::PathFormula, ValueType> const& checkTask
        }
    }
}