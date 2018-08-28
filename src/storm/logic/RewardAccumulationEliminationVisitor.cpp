#include "storm/logic/RewardAccumulationEliminationVisitor.h"
#include "storm/logic/Formulas.h"

#include "storm/utility/macros.h"

#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/symbolic/StandardRewardModel.h"

namespace storm {
    namespace logic {

        template <class RewardModelType>
        RewardAccumulationEliminationVisitor<RewardModelType>::RewardAccumulationEliminationVisitor(std::unordered_map<std::string, RewardModelType> const& rewardModels, storm::models::ModelType const& modelType) : rewardModels(rewardModels) {
            if (modelType == storm::models::ModelType::Dtmc || modelType == storm::models::ModelType::Mdp || modelType == storm::models::ModelType::S2pg) {
                isDiscreteTimeModel = true;
            } else if (modelType == storm::models::ModelType::Ctmc || modelType == storm::models::ModelType::MarkovAutomaton) {
                isDiscreteTimeModel = false;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unhandled model type " << modelType << ".");
            }
        }
        
        template <class RewardModelType>
        std::shared_ptr<Formula> RewardAccumulationEliminationVisitor<RewardModelType>::eliminateRewardAccumulations(Formula const& f) const {
            boost::any result = f.accept(*this, boost::any());
            return boost::any_cast<std::shared_ptr<Formula>>(result);
        }
        
        template <class RewardModelType>
        boost::any RewardAccumulationEliminationVisitor<RewardModelType>::visit(BoundedUntilFormula const& f, boost::any const& data) const {
            std::vector<boost::optional<TimeBound>> lowerBounds, upperBounds;
            std::vector<TimeBoundReference> timeBoundReferences;
            for (uint64_t i = 0; i < f.getDimension(); ++i) {
                if (f.hasLowerBound(i)) {
                    lowerBounds.emplace_back(TimeBound(f.isLowerBoundStrict(i), f.getLowerBound(i)));
                } else {
                    lowerBounds.emplace_back();
                }
                if (f.hasUpperBound(i)) {
                    upperBounds.emplace_back(TimeBound(f.isUpperBoundStrict(i), f.getUpperBound(i)));
                } else {
                    upperBounds.emplace_back();
                }
                storm::logic::TimeBoundReference tbr = f.getTimeBoundReference(i);
                if (tbr.hasRewardAccumulation() && canEliminate(tbr.getRewardAccumulation(), tbr.getRewardName())) {
                    // Eliminate accumulation
                    tbr = storm::logic::TimeBoundReference(tbr.getRewardName(), boost::none);
                }
                timeBoundReferences.push_back(std::move(tbr));
            }
            if (f.hasMultiDimensionalSubformulas()) {
                std::vector<std::shared_ptr<Formula const>> leftSubformulas, rightSubformulas;
                for (uint64_t i = 0; i < f.getDimension(); ++i) {
                    leftSubformulas.push_back(boost::any_cast<std::shared_ptr<Formula>>(f.getLeftSubformula(i).accept(*this, data)));
                    rightSubformulas.push_back(boost::any_cast<std::shared_ptr<Formula>>(f.getRightSubformula(i).accept(*this, data)));
                }
                return std::static_pointer_cast<Formula>(std::make_shared<BoundedUntilFormula>(leftSubformulas, rightSubformulas, lowerBounds, upperBounds, timeBoundReferences));
            } else {
                std::shared_ptr<Formula> left = boost::any_cast<std::shared_ptr<Formula>>(f.getLeftSubformula().accept(*this, data));
                std::shared_ptr<Formula> right = boost::any_cast<std::shared_ptr<Formula>>(f.getRightSubformula().accept(*this, data));
                return std::static_pointer_cast<Formula>(std::make_shared<BoundedUntilFormula>(left, right, lowerBounds, upperBounds, timeBoundReferences));
            }
        }
        
        template <class RewardModelType>
        boost::any RewardAccumulationEliminationVisitor<RewardModelType>::visit(CumulativeRewardFormula const& f, boost::any const& data) const {
            boost::optional<storm::logic::RewardAccumulation> rewAcc;
            STORM_LOG_THROW(!data.empty(), storm::exceptions::UnexpectedException, "Formula " << f << " does not seem to be a subformula of a reward operator.");
            auto rewName = boost::any_cast<boost::optional<std::string>>(data);
            if (f.hasRewardAccumulation() && !canEliminate(f.getRewardAccumulation(), rewName)) {
                rewAcc = f.getRewardAccumulation();
            }

            std::vector<TimeBound> bounds;
            std::vector<TimeBoundReference> timeBoundReferences;
            for (uint64_t i = 0; i < f.getDimension(); ++i) {
                bounds.emplace_back(TimeBound(f.isBoundStrict(i), f.getBound(i)));
                storm::logic::TimeBoundReference tbr = f.getTimeBoundReference(i);
                if (tbr.hasRewardAccumulation() && canEliminate(tbr.getRewardAccumulation(), tbr.getRewardName())) {
                    // Eliminate accumulation
                    tbr = storm::logic::TimeBoundReference(tbr.getRewardName(), boost::none);
                }
                timeBoundReferences.push_back(std::move(tbr));
            }
            return std::static_pointer_cast<Formula>(std::make_shared<CumulativeRewardFormula>(bounds, timeBoundReferences, rewAcc));
        }
        
        template <class RewardModelType>
        boost::any RewardAccumulationEliminationVisitor<RewardModelType>::visit(EventuallyFormula const& f, boost::any const& data) const {
           std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            if (f.hasRewardAccumulation()) {
                if (f.isTimePathFormula()) {
                    if (isDiscreteTimeModel && ((!f.getRewardAccumulation().isExitSet() && !f.getRewardAccumulation().isStepsSet()) || (f.getRewardAccumulation().isStepsSet() && f.getRewardAccumulation().isExitSet()))) {
                        return std::static_pointer_cast<Formula>(std::make_shared<EventuallyFormula>(subformula, f.getContext(), f.getRewardAccumulation()));
                    } else if (!isDiscreteTimeModel && (!f.getRewardAccumulation().isTimeSet() || f.getRewardAccumulation().isExitSet() || f.getRewardAccumulation().isStepsSet())) {
                        return std::static_pointer_cast<Formula>(std::make_shared<EventuallyFormula>(subformula, f.getContext(), f.getRewardAccumulation()));
                    }
                } else if (f.isRewardPathFormula()) {
                    STORM_LOG_THROW(!data.empty(), storm::exceptions::UnexpectedException, "Formula " << f << " does not seem to be a subformula of a reward operator.");
                    auto rewName = boost::any_cast<boost::optional<std::string>>(data);
                    if (!canEliminate(f.getRewardAccumulation(), rewName)) {
                        return std::static_pointer_cast<Formula>(std::make_shared<EventuallyFormula>(subformula, f.getContext(), f.getRewardAccumulation()));
                    }
                }
            }
            return std::static_pointer_cast<Formula>(std::make_shared<EventuallyFormula>(subformula, f.getContext()));
        }
        
        template <class RewardModelType>
        boost::any RewardAccumulationEliminationVisitor<RewardModelType>::visit(RewardOperatorFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, f.getOptionalRewardModelName()));
            return std::static_pointer_cast<Formula>(std::make_shared<RewardOperatorFormula>(subformula, f.getOptionalRewardModelName(), f.getOperatorInformation()));
        }
        
        template <class RewardModelType>
        boost::any RewardAccumulationEliminationVisitor<RewardModelType>::visit(TotalRewardFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(!data.empty(), storm::exceptions::UnexpectedException, "Formula " << f << " does not seem to be a subformula of a reward operator.");
            auto rewName = boost::any_cast<boost::optional<std::string>>(data);
            if (f.hasRewardAccumulation() || canEliminate(f.getRewardAccumulation(), rewName)) {
                return std::static_pointer_cast<Formula>(std::make_shared<TotalRewardFormula>());
            } else {
                return std::static_pointer_cast<Formula>(std::make_shared<TotalRewardFormula>(f.getRewardAccumulation()));
            }
        }
        
        template <class RewardModelType>
        bool RewardAccumulationEliminationVisitor<RewardModelType>::canEliminate(storm::logic::RewardAccumulation const& accumulation, boost::optional<std::string> rewardModelName) const {
            auto rewModelIt = rewardModels.end();
            if (rewardModelName.is_initialized()){
                rewModelIt = rewardModels.find(rewardModelName.get());
                STORM_LOG_THROW(rewModelIt != rewardModels.end(), storm::exceptions::InvalidPropertyException, "Unable to find reward model with name " << rewardModelName.get());
            } else if (rewardModels.size() == 1) {
                rewModelIt = rewardModels.begin();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Multiple reward models were defined but no reward model name was given for at least one property.");
            }
            RewardModelType const& rewardModel = rewModelIt->second;
            if ((rewardModel.hasStateActionRewards() || rewardModel.hasTransitionRewards()) && !accumulation.isStepsSet()) {
                return false;
            }
            if (rewardModel.hasStateRewards()) {
                if (isDiscreteTimeModel) {
                    if (!accumulation.isExitSet()) {
                        return false;
                    }
                    // accumulating over time in discrete time models has no effect, i.e., the value of accumulation.isTimeSet() does not matter here.
                } else {
                    if (accumulation.isExitSet() || !accumulation.isTimeSet()) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        template class RewardAccumulationEliminationVisitor<storm::models::sparse::StandardRewardModel<double>>;
        template class RewardAccumulationEliminationVisitor<storm::models::sparse::StandardRewardModel<storm::RationalNumber>>;
        template class RewardAccumulationEliminationVisitor<storm::models::sparse::StandardRewardModel<storm::RationalFunction>>;
        template class RewardAccumulationEliminationVisitor<storm::models::sparse::StandardRewardModel<storm::Interval>>;
        
        template class RewardAccumulationEliminationVisitor<storm::models::symbolic::StandardRewardModel<storm::dd::DdType::CUDD, double>>;
        template class RewardAccumulationEliminationVisitor<storm::models::symbolic::StandardRewardModel<storm::dd::DdType::Sylvan, double>>;
        template class RewardAccumulationEliminationVisitor<storm::models::symbolic::StandardRewardModel<storm::dd::DdType::Sylvan, storm::RationalNumber>>;
        template class RewardAccumulationEliminationVisitor<storm::models::symbolic::StandardRewardModel<storm::dd::DdType::Sylvan, storm::RationalFunction>>;

    }
}
