#include "storm/modelchecker/multiobjective/constraintbased/SparseCbAchievabilityQuery.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/utility/solver.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/MultiObjectiveSettings.h"
#include "storm/storage/expressions/Expressions.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template <class SparseModelType>
            SparseCbAchievabilityQuery<SparseModelType>::SparseCbAchievabilityQuery(SparseMultiObjectivePreprocessorReturnType<SparseModelType>& preprocessorResult) : SparseCbQuery<SparseModelType>(preprocessorResult) {
                STORM_LOG_ASSERT(preprocessorResult.queryType==SparseMultiObjectivePreprocessorReturnType<SparseModelType>::QueryType::Achievability, "Invalid query Type");
                solver = storm::utility::solver::SmtSolverFactory().create(*this->expressionManager);
            }

            template <class SparseModelType>
            std::unique_ptr<CheckResult> SparseCbAchievabilityQuery<SparseModelType>::check() {
               
                bool result = this->checkAchievability();
                
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(this->originalModel.getInitialStates().getNextSetIndex(0), result));
                
            }
            
            template <class SparseModelType>
            bool SparseCbAchievabilityQuery<SparseModelType>::checkAchievability() {
                
                //this->preprocessedModel.writeDotToStream(std::cout);
                
                initializeConstraintSystem();
                solver->push();
                addObjectiveConstraints();
                solver->push();
                
                storm::solver::SmtSolver::CheckResult result = solver->check();
                switch(result) {
                    case storm::solver::SmtSolver::CheckResult::Sat:
                        // std::cout << std::endl << "Satisfying assignment: " << std::endl << solver->getModelAsValuation().toString(true) << std::endl;
                        return true;
                    case storm::solver::SmtSolver::CheckResult::Unsat:
                        // std::cout << std::endl << "Unsatisfiability core: {" << std::endl;
                        // for (auto const& expr : solver->getUnsatCore()) {
                        //    std::cout << "\t " << expr << std::endl;
                        // }
                        // std::cout << "}" << std::endl;
                        return false;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "SMT solver yielded an unexpected result");
                }
                
                return false;
            }
            
            template <class SparseModelType>
            void SparseCbAchievabilityQuery<SparseModelType>::initializeConstraintSystem() {
                uint_fast64_t numStates = this->preprocessedModel.getNumberOfStates();
                uint_fast64_t numChoices = this->preprocessedModel.getNumberOfChoices();
                uint_fast64_t numBottomStates = this->possibleBottomStates.getNumberOfSetBits();
                
                storm::expressions::Expression zero = this->expressionManager->rational(storm::utility::zero<ValueType>());
                storm::expressions::Expression one = this->expressionManager->rational(storm::utility::one<ValueType>());
                
                // Declare the variables for the choices and bottom states
                expectedChoiceVariables.reserve(numChoices);
                for (uint_fast64_t choice = 0; choice < numChoices; ++choice) {
                    expectedChoiceVariables.push_back(this->expressionManager->declareRationalVariable("y" + std::to_string(choice)));
                }
                bottomStateVariables.reserve(numBottomStates);
                for (uint_fast64_t bottomState = 0; bottomState < numBottomStates; ++bottomState) {
                    bottomStateVariables.push_back(this->expressionManager->declareRationalVariable("z" + std::to_string(bottomState)));
                }
                
                // assert that the values are greater zero and that the bottom state values sum up to one
                for (auto& var : expectedChoiceVariables) {
                    solver->add(var.getExpression() >= zero);
                }
                storm::expressions::Expression bottomStateSum = zero;
                for (auto& var : bottomStateVariables) {
                    solver->add(var.getExpression() >= zero);
                    bottomStateSum = bottomStateSum + var.getExpression();
                }
                solver->add(bottomStateSum == one);
                
                // assert that the "incoming" value of each state equals the "outgoing" value
                storm::storage::SparseMatrix<ValueType> backwardsTransitions = this->preprocessedModel.getTransitionMatrix().transpose();
                auto bottomStateVariableIt = bottomStateVariables.begin();
                for (uint_fast64_t state = 0; state < numStates; ++state) {
                    // get the "incomming" value
                    storm::expressions::Expression value = this->preprocessedModel.getInitialStates().get(state) ? one : zero;
                    for (auto const& backwardsEntry : backwardsTransitions.getRow(state)) {
                        value = value + (this->expressionManager->rational(backwardsEntry.getValue()) * expectedChoiceVariables[backwardsEntry.getColumn()].getExpression());
                    }
                    
                    // subtract the "outgoing" value
                    for (uint_fast64_t choice = this->preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]; choice < this->preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state + 1]; ++choice) {
                        value = value - expectedChoiceVariables[choice];
                    }
                    if (this->possibleBottomStates.get(state)) {
                        value = value - (*bottomStateVariableIt);
                        ++bottomStateVariableIt;
                    }
                    solver->add(value == zero);
                }
                assert(bottomStateVariableIt == bottomStateVariables.end());
                
            }
            
            template <class SparseModelType>
            void SparseCbAchievabilityQuery<SparseModelType>::addObjectiveConstraints() {
                storm::expressions::Expression zero = this->expressionManager->rational(storm::utility::zero<ValueType>());
                for (Objective<ValueType> const& obj : this->objectives) {
                    if (obj.rewardModelName) {
                        STORM_LOG_THROW(obj.bound, storm::exceptions::InvalidOperationException, "Invoked achievability query but no bound was specified for at least one objective.");
                        STORM_LOG_THROW(!obj.lowerTimeBound && !obj.upperTimeBound, storm::exceptions::NotSupportedException, "Constraint based method currently does not support step bounds");
                        std::vector<ValueType> rewards = getActionBasedExpectedRewards(*obj.rewardModelName);
                        storm::expressions::Expression objValue = zero;
                        for (uint_fast64_t choice = 0; choice < rewards.size(); ++choice) {
                            if (!storm::utility::isZero(rewards[choice])) {
                                objValue = objValue + (this->expressionManager->rational(rewards[choice]) * expectedChoiceVariables[choice].getExpression());
                            }
                        }
                        // We need to actually evaluate the threshold as rational number. Otherwise a threshold like '<=16/9' might be considered as 1 due to integer division
                        STORM_LOG_THROW(!obj.bound->threshold.containsVariables(), storm::exceptions::InvalidOperationException, "The threshold for one objective still contains undefined variables");
                        storm::expressions::Expression threshold = this->expressionManager->rational(obj.bound->threshold.evaluateAsRational());
                        switch (obj.bound->comparisonType) {
                            case storm::logic::ComparisonType::Greater:
                                solver->add( objValue > threshold);
                                break;
                            case storm::logic::ComparisonType::GreaterEqual:
                                solver->add( objValue >= threshold);
                                break;
                            case storm::logic::ComparisonType::Less:
                                solver->add( objValue < threshold);
                                break;
                            case storm::logic::ComparisonType::LessEqual:
                                solver->add( objValue <= threshold);
                                break;
                            default:
                                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "One or more objectives have an invalid comparison type");
                        }
                    }
                }
            }
            
            template <>
            std::vector<double> SparseCbAchievabilityQuery<storm::models::sparse::Mdp<double>>::getActionBasedExpectedRewards(std::string const& rewardModelName) const {
                return this->preprocessedModel.getRewardModel(rewardModelName).getTotalRewardVector(this->preprocessedModel.getTransitionMatrix());
            }
            
            template <>
            std::vector<double> SparseCbAchievabilityQuery<storm::models::sparse::MarkovAutomaton<double>>::getActionBasedExpectedRewards(std::string const& rewardModelName) const {
                auto const& rewModel = this->preprocessedModel.getRewardModel(rewardModelName);
                STORM_LOG_ASSERT(!rewModel.hasTransitionRewards(), "Preprocessed Reward model has transition rewards which is not expected.");
                std::vector<double> result = rewModel.hasStateActionRewards() ? rewModel.getStateActionRewardVector() : std::vector<double>(this->preprocessedModel.getNumberOfChoices(), storm::utility::zero<ValueType>());
                if(rewModel.hasStateRewards()) {
                    // Note that state rewards are earned over time and thus play no role for probabilistic states
                    for(auto markovianState : this->preprocessedModel.getMarkovianStates()) {
                        result[this->preprocessedModel.getTransitionMatrix().getRowGroupIndices()[markovianState]] += rewModel.getStateReward(markovianState) / this->preprocessedModel.getExitRate(markovianState);
                    }
                }
                return result;
            }

            template <>
            std::vector<storm::RationalNumber> SparseCbAchievabilityQuery<storm::models::sparse::Mdp<storm::RationalNumber>>::getActionBasedExpectedRewards(std::string const& rewardModelName) const {
                return this->preprocessedModel.getRewardModel(rewardModelName).getTotalRewardVector(this->preprocessedModel.getTransitionMatrix());
            }
            

#ifdef STORM_HAVE_CARL
            template class SparseCbAchievabilityQuery<storm::models::sparse::Mdp<double>>;
            template class SparseCbAchievabilityQuery<storm::models::sparse::MarkovAutomaton<double>>;
            
            template class SparseCbAchievabilityQuery<storm::models::sparse::Mdp<storm::RationalNumber>>;
         //   template class SparseCbAchievabilityQuery<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
#endif
        }
    }
}
