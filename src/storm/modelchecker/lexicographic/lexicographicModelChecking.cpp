//
// Created by steffi on 04.11.21.
//
#include "storm/utility/macros.h"
#include "storm/environment/Environment.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/modelchecker/lexicographic/lexicographicModelChecking.h"
#include "storm/modelchecker/lexicographic/lexicographicModelChecker.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidEnvironmentException.h"


namespace storm {
    namespace modelchecker {
        namespace lexicographic {

            template<typename SparseModelType, typename ValueType>
            int isDone(Environment const& env, SparseModelType const& model, CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask, CheckFormulaCallback const& formulaChecker) {
                STORM_LOG_ASSERT(model.getInitialStates().getNumberOfSetBits() == 1, "Lexicographic Model checking on model with multiple initial states is not supported.");

                storm::logic::MultiObjectiveFormula const& formula = checkTask.getFormula();
                lexicographicModelChecker<SparseModelType, ValueType, true> lMC = lexicographicModelChecker<SparseModelType, ValueType, true>(formula, model.getTransitionMatrix());

                auto res = lMC.getCompleteProductModel(model, formulaChecker);
                STORM_PRINT("Got product model"<<std::endl);
                auto completeProductModel = res.first;
                auto accCond = res.second;
                std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>, std::vector<std::vector<bool>>> result = lMC.solve(completeProductModel, accCond, formula);
                STORM_PRINT("Solved ltl formula"<<std::endl);
                storm::storage::MaximalEndComponentDecomposition<ValueType> bcc = result.first;
                std::vector<std::vector<bool>> bccLexArrays = result.second;
                auto return_result = lMC.reachability(bcc, bccLexArrays, completeProductModel);
                STORM_PRINT("Lex result"<<std::endl);
                /*storm::logic::PathFormula const& pathFormula = checkTask.getFormula();

                STORM_LOG_THROW(checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");

                storm::modelchecker::helper::SparseLTLHelper<ValueType, true> helper(this->getModel().getTransitionMatrix());
                storm::modelchecker::helper::setInformationFromCheckTaskNondeterministic(helper, checkTask, this->getModel());

                auto formulaChecker = [&] (storm::logic::Formula const& formula) { return this->check(env, formula)->asExplicitQualitativeCheckResult().getTruthValuesVector(); };*/
                return return_result;
            }

            template int isDone<storm::models::sparse::Mdp<double>, double>(Environment const& env, storm::models::sparse::Mdp<double> const& model, CheckTask<storm::logic::MultiObjectiveFormula, double> const& checkTask, CheckFormulaCallback const& formulaChecker);
            template int isDone<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>(Environment const& env, storm::models::sparse::Mdp<storm::RationalNumber> const& model, CheckTask<storm::logic::MultiObjectiveFormula, storm::RationalNumber> const& checkTask, CheckFormulaCallback const& formulaChecker);
        }
    }
}
