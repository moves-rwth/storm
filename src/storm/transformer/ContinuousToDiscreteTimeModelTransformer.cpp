#include "storm/transformer/ContinuousToDiscreteTimeModelTransformer.h"

#include <unordered_map>

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/logic/Formulas.h"
#include "storm/logic/FragmentSpecification.h"
#include "storm/logic/CloneVisitor.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace transformer {
        
        template <typename ValueType>
        void transformContinuousToDiscreteModelInPlace(std::shared_ptr<storm::models::sparse::Model<ValueType>>& markovModel, std::shared_ptr<storm::logic::Formula const>& formula) {
            boost::optional<std::string> timeRewardModelName;
            if (formula->isTimeOperatorFormula()) {
                auto const& timeOpFormula = formula->asTimeOperatorFormula();
                if (timeOpFormula.getSubformula().isReachabilityTimeFormula()) {
                    auto reachabilityRewardFormula = std::make_shared<storm::logic::EventuallyFormula>(storm::logic::CloneVisitor().clone(timeOpFormula.getSubformula().asReachabilityTimeFormula().getSubformula()), storm::logic::FormulaContext::Reward);
                    timeRewardModelName = "time";
                    // make sure that the reward model name is not already in use
                    while (markovModel->hasRewardModel(*timeRewardModelName)) *timeRewardModelName += "_";
                    formula = std::make_shared<storm::logic::RewardOperatorFormula const>(reachabilityRewardFormula, timeRewardModelName, timeOpFormula.getOperatorInformation());
                }
            }
        
            if (markovModel->isOfType(storm::models::ModelType::Ctmc)) {
                SparseCtmcToSparseDtmcTransformer<storm::models::sparse::Ctmc<ValueType>> transformer;
                if (transformer.transformationPreservesProperty(*formula)) {
                    STORM_LOG_INFO("Transforming Ctmc to embedded Dtmc...");
                    markovModel = transformer.translate(std::move(*markovModel->template as<storm::models::sparse::Ctmc<ValueType>>()), timeRewardModelName);
                }
            } else if (markovModel->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                SparseMaToSparseMdpTransformer<storm::models::sparse::MarkovAutomaton<ValueType>> transformer;
                if (transformer.transformationPreservesProperty(*formula)) {
                    STORM_LOG_INFO("Transforming Markov automaton to embedded Mdp...");
                    markovModel = transformer.translate(std::move(*markovModel->template as<storm::models::sparse::MarkovAutomaton<ValueType>>()), timeRewardModelName);
                }
            }
        }
        
        template<typename CtmcType>
        std::shared_ptr<storm::models::sparse::Dtmc<typename CtmcType::ValueType, typename CtmcType::RewardModelType>> SparseCtmcToSparseDtmcTransformer<CtmcType>::translate(CtmcType&& ctmc, boost::optional<std::string> const& timeRewardModelName) {
            // Turn the rates into probabilities by scaling each row of the transition matrix with the exit rate
            std::vector<typename CtmcType::ValueType>& exitRates = ctmc.getExitRateVector();
            storm::storage::SparseMatrix<typename CtmcType::ValueType> matrix(std::move(ctmc.getTransitionMatrix()));
            auto exitRateIt = exitRates.begin();
            for (uint_fast64_t state = 0; state < matrix.getRowCount(); ++state) {
                for (auto& entry : matrix.getRow(state)) {
                    entry.setValue(entry.getValue() / *exitRateIt);
                }
                ++exitRateIt;
            }
            STORM_LOG_ASSERT(exitRateIt == exitRates.end(), "Unexpected size of rate vector.");
            
            // Transform the reward models
            std::unordered_map<std::string, typename CtmcType::RewardModelType> rewardModels(std::move(ctmc.getRewardModels()));
            for (auto& rewardModel : rewardModels) {
                if (rewardModel.second.hasStateRewards()) {
                    storm::utility::vector::divideVectorsPointwise(rewardModel.second.getStateRewardVector(), exitRates, rewardModel.second.getStateRewardVector());
                }
            }
            
            if (timeRewardModelName) {
                // Invert the exit rate vector in place
                storm::utility::vector::applyPointwise<typename CtmcType::ValueType, typename CtmcType::ValueType>(exitRates, exitRates, [&] (typename CtmcType::ValueType const& r) -> typename CtmcType::ValueType { return storm::utility::one<typename CtmcType::ValueType>() / r; });
                typename CtmcType::RewardModelType timeRewards(std::move(exitRates));
                auto insertRes = rewardModels.insert(std::make_pair(*timeRewardModelName, std::move(timeRewards)));
                STORM_LOG_THROW(insertRes.second, storm::exceptions::InvalidArgumentException, "Could not insert auxiliary reward model " << *timeRewardModelName << " because a model with this name already exists.");
            }
            // exitRates might be invalidated at this point!!
            
            return std::make_shared<storm::models::sparse::Dtmc<typename CtmcType::ValueType, typename CtmcType::RewardModelType>>(std::move(matrix), std::move(ctmc.getStateLabeling()), std::move(rewardModels));
        }
        
        template<typename CtmcType>
        bool SparseCtmcToSparseDtmcTransformer<CtmcType>::transformationPreservesProperty(storm::logic::Formula const& formula) {
            storm::logic::FragmentSpecification fragment = storm::logic::propositional();
            fragment.setProbabilityOperatorsAllowed(true);
            fragment.setGloballyFormulasAllowed(true);
            fragment.setReachabilityProbabilityFormulasAllowed(true);
            fragment.setNextFormulasAllowed(true);
            fragment.setUntilFormulasAllowed(true);
            fragment.setRewardOperatorsAllowed(true);
            fragment.setReachabilityRewardFormulasAllowed(true);
            return formula.isInFragment(fragment);
        }
        
        template<typename MaType>
        std::shared_ptr<storm::models::sparse::Mdp<typename MaType::ValueType, typename MaType::RewardModelType>> SparseMaToSparseMdpTransformer<MaType>::translate(MaType&& ma, boost::optional<std::string> const& timeRewardModelName) {
            STORM_LOG_THROW(ma.isClosed(), storm::exceptions::InvalidArgumentException, "Transformation of MA to its underlying MDP is only possible for closed MAs");
            std::vector<typename MaType::ValueType>& exitRates = ma.getExitRates();
            
            // Markov automata already store the probability matrix
            
            // Transform the reward models
            std::unordered_map<std::string, typename MaType::RewardModelType> rewardModels(std::move(ma.getRewardModels()));
            for (auto& rewardModel : rewardModels) {
                if (rewardModel.second.hasStateRewards()) {
                    auto& stateRewards = rewardModel.second.getStateRewardVector();
                    for (auto state : ma.getMarkovianStates()) {
                        stateRewards[state] /= exitRates[state];
                    }
                }
            }
            
            if (timeRewardModelName) {
                // Invert the exit rate vector. Avoid division by zero at probabilistic states
                std::vector<typename MaType::ValueType> timeRewardVector(exitRates.size(), storm::utility::zero<typename MaType::ValueType>());
                for (auto state : ma.getMarkovianStates()) {
                    timeRewardVector[state] = storm::utility::one<typename MaType::ValueType>() / exitRates[state];
                }
                typename MaType::RewardModelType timeRewards(std::move(timeRewardVector));
                auto insertRes = rewardModels.insert(std::make_pair(*timeRewardModelName, std::move(timeRewards)));
                STORM_LOG_THROW(insertRes.second, storm::exceptions::InvalidArgumentException, "Could not insert auxiliary reward model " << *timeRewardModelName << " because a model with this name already exists.");
            }
            
            return std::make_shared<storm::models::sparse::Mdp<typename MaType::ValueType, typename MaType::RewardModelType>>(std::move(ma.getTransitionMatrix()), std::move(ma.getStateLabeling()), std::move(rewardModels));
        }
        
        template<typename MaType>
        bool SparseMaToSparseMdpTransformer<MaType>::transformationPreservesProperty(storm::logic::Formula const& formula) {
            storm::logic::FragmentSpecification fragment = storm::logic::propositional();
            fragment.setProbabilityOperatorsAllowed(true);
            fragment.setGloballyFormulasAllowed(true);
            fragment.setReachabilityProbabilityFormulasAllowed(true);
            fragment.setNextFormulasAllowed(true);
            fragment.setUntilFormulasAllowed(true);
            fragment.setRewardOperatorsAllowed(true);
            fragment.setReachabilityRewardFormulasAllowed(true);
            
            return formula.isInFragment(fragment);
        }

        template void transformContinuousToDiscreteModelInPlace<double>(std::shared_ptr<storm::models::sparse::Model<double>>& markovModel, std::shared_ptr<storm::logic::Formula const>& formula);
        template void transformContinuousToDiscreteModelInPlace<storm::RationalNumber>(std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>>& markovModel, std::shared_ptr<storm::logic::Formula const>& formula);
        template void transformContinuousToDiscreteModelInPlace<storm::RationalFunction>(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>>& markovModel, std::shared_ptr<storm::logic::Formula const>& formula);
        template class SparseCtmcToSparseDtmcTransformer<storm::models::sparse::Ctmc<double>>;
        template class SparseCtmcToSparseDtmcTransformer<storm::models::sparse::Ctmc<storm::RationalNumber>>;
        template class SparseCtmcToSparseDtmcTransformer<storm::models::sparse::Ctmc<storm::RationalFunction>>;
        template class SparseMaToSparseMdpTransformer<storm::models::sparse::MarkovAutomaton<double>>;
        template class SparseMaToSparseMdpTransformer<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
        template class SparseMaToSparseMdpTransformer<storm::models::sparse::MarkovAutomaton<storm::RationalFunction>>;
    }
}
