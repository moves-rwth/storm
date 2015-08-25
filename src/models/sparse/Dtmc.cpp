#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/adapters/CarlAdapter.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/utility/constants.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType, typename RewardModelType>
            Dtmc<ValueType, RewardModelType>::Dtmc(storm::storage::SparseMatrix<ValueType> const& probabilityMatrix,
                 storm::models::sparse::StateLabeling const& stateLabeling,
                 std::unordered_map<std::string, RewardModelType> const& rewardModels,
                 boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : DeterministicModel<ValueType>(storm::models::ModelType::Dtmc, probabilityMatrix, stateLabeling, rewardModels, optionalChoiceLabeling) {
                STORM_LOG_THROW(probabilityMatrix.isProbabilistic(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
            }
            
            template <typename ValueType, typename RewardModelType>
            Dtmc<ValueType, RewardModelType>::Dtmc(storm::storage::SparseMatrix<ValueType>&& probabilityMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                std::unordered_map<std::string, RewardModelType>&& rewardModels, boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : DeterministicModel<ValueType>(storm::models::ModelType::Dtmc, std::move(probabilityMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)) {
                STORM_LOG_THROW(probabilityMatrix.isProbabilistic(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
            }
            
            template <typename ValueType, typename RewardModelType>
            Dtmc<ValueType> Dtmc<ValueType, RewardModelType>::getSubDtmc(storm::storage::BitVector const& states) const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This function is not yet implemented.");
                // FIXME: repair this
                //
                //
                //		// Is there any state in the subsystem?
                //		if(subSysStates.getNumberOfSetBits() == 0) {
                //			LOG4CPLUS_ERROR(logger, "No states in subsystem!");
                //			return storm::models::Dtmc<ValueType>(storm::storage::SparseMatrix<ValueType>(),
                //					  	  	  	  	  	  storm::models::sparse::StateLabeling(this->getStateLabeling(), subSysStates),
                //					  	  	  	  	  	  boost::optional<std::vector<ValueType>>(),
                //					  	  	  	  	  	  boost::optional<storm::storage::SparseMatrix<ValueType>>(),
                //					  	  	  	  	  	  boost::optional<std::vector<LabelSet>>());
                //		}
                //
                //		// Does the vector have the right size?
                //		if(subSysStates.size() != this->getNumberOfStates()) {
                //			LOG4CPLUS_INFO(logger, "BitVector has wrong size. Resizing it...");
                //			subSysStates.resize(this->getNumberOfStates());
                //		}
                //
                //		// Test if it is a proper subsystem of this Dtmc, i.e. if there is at least one state to be left out.
                //		if(subSysStates.getNumberOfSetBits() == subSysStates.size()) {
                //			LOG4CPLUS_INFO(logger, "All states are kept. This is no proper subsystem.");
                //			return storm::models::Dtmc<ValueType>(*this);
                //		}
                //
                //		// 1. Get all necessary information from the old transition matrix
                //		storm::storage::SparseMatrix<ValueType> const & origMat = this->getTransitionMatrix();
                //
                //		// Iterate over all rows. Count the number of all transitions from the old system to be
                //		// transfered to the new one. Also build a mapping from the state number of the old system
                //		// to the state number of the new system.
                //		uint_fast64_t subSysTransitionCount = 0;
                //		uint_fast64_t newRow = 0;
                //		std::vector<uint_fast64_t> stateMapping;
                //		for(uint_fast64_t row = 0; row < origMat.getRowCount(); ++row) {
                //			if(subSysStates.get(row)){
                //				for(auto const& entry : origMat.getRow(row)) {
                //					if(subSysStates.get(entry.getColumn())) {
                //						subSysTransitionCount++;
                //					}
                //				}
                //				stateMapping.push_back(newRow);
                //				newRow++;
                //			} else {
                //				stateMapping.push_back((uint_fast64_t) -1);
                //			}
                //		}
                //
                //		// 2. Construct transition matrix
                //
                //		// Take all states indicated by the vector as well as one additional state s_b as target of
                //		// all transitions that target a state that is not kept.
                //		uint_fast64_t const newStateCount = subSysStates.getNumberOfSetBits() + 1;
                //
                //		// The number of transitions of the new Dtmc is the number of transitions transfered
                //		// from the old one plus one transition for each state to s_b.
                //		storm::storage::SparseMatrixBuilder<ValueType> newMatBuilder(newStateCount,newStateCount,subSysTransitionCount + newStateCount);
                //
                //		// Now fill the matrix.
                //		newRow = 0;
                //        T rest = storm::utility::zero<ValueType>();
                //		for(uint_fast64_t row = 0; row < origMat.getRowCount(); ++row) {
                //			if(subSysStates.get(row)){
                //				// Transfer transitions
                //				for(auto& entry : origMat.getRow(row)) {
                //					if(subSysStates.get(entry.getColumn())) {
                //						newMatBuilder.addNextValue(newRow, stateMapping[entry.getColumn()], entry.getValue());
                //					} else {
                //						rest += entry.getValue();
                //					}
                //				}
                //
                //				// Insert the transition taking care of the remaining outgoing probability.
                //				newMatBuilder.addNextValue(newRow, newStateCount - 1, rest);
                //				rest = storm::utility::zero<ValueType>();
                //
                //				newRow++;
                //			}
                //		}
                //
                //		// Insert last transition: self loop on s_b
                //		newMatBuilder.addNextValue(newStateCount - 1, newStateCount - 1, storm::utility::one<ValueType>());
                //
                //		// 3. Take care of the labeling.
                //		storm::models::sparse::StateLabeling newLabeling = storm::models::sparse::StateLabeling(this->getStateLabeling(), subSysStates);
                //		newLabeling.addState();
                //		if(!newLabeling.containsLabel("s_b")) {
                //			newLabeling.addLabel("s_b");
                //		}
                //		newLabeling.addLabelToState("s_b", newStateCount - 1);
                //
                //		// 4. Handle the optionals
                //
                //		boost::optional<std::vector<ValueType>> newStateRewards;
                //		if(this->hasStateRewards()) {
                //
                //			// Get the rewards and move the needed values to the front.
                //			std::vector<ValueType> newRewards(this->getStateRewardVector());
                //			storm::utility::vector::selectVectorValues(newRewards, subSysStates, newRewards);
                //
                //			// Throw away all values after the last state and set the reward for s_b to 0.
                //			newRewards.resize(newStateCount);
                //			newRewards[newStateCount - 1] = (T) 0;
                //
                //			newStateRewards = newRewards;
                //		}
                //
                //		boost::optional<storm::storage::SparseMatrix<ValueType>> newTransitionRewards;
                //		if(this->hasTransitionRewards()) {
                //
                //			storm::storage::SparseMatrixBuilder<ValueType> newTransRewardsBuilder(newStateCount, subSysTransitionCount + newStateCount);
                //
                //			// Copy the rewards for the kept states
                //			newRow = 0;
                //			for(uint_fast64_t row = 0; row < this->getTransitionRewardMatrix().getRowCount(); ++row) {
                //				if(subSysStates.get(row)){
                //					// Transfer transition rewards
                //					for(auto& entry : this->getTransitionRewardMatrix().getRow(row)) {
                //						if(subSysStates.get(entry.getColumn())) {
                //							newTransRewardsBuilder.addNextValue(newRow, stateMapping[entry.getColumn()], entry.getValue());
                //						}
                //					}
                //
                //					// Insert the reward (e.g. 0) for the transition taking care of the remaining outgoing probability.
                //					newTransRewardsBuilder.addNextValue(newRow, newStateCount - 1, storm::utility::zero<ValueType>());
                //
                //					newRow++;
                //				}
                //			}
                //
                //			newTransitionRewards = newTransRewardsBuilder.build();
                //		}
                //
                //		boost::optional<std::vector<LabelSet>> newChoiceLabels;
                //		if(this->hasChoiceLabeling()) {
                //
                //			// Get the choice label sets and move the needed values to the front.
                //			std::vector<LabelSet> newChoice(this->getChoiceLabeling());
                //			storm::utility::vector::selectVectorValues(newChoice, subSysStates, this->getChoiceLabeling());
                //
                //			// Throw away all values after the last state and set the choice label set for s_b as empty.
                //			newChoice.resize(newStateCount);
                //			newChoice[newStateCount - 1] = LabelSet();
                //
                //			newChoiceLabels = newChoice;
                //		}
                //
                //		// 5. Make Dtmc from its parts and return it
                //		return storm::models::Dtmc<ValueType>(newMatBuilder.build(), newLabeling, newStateRewards, std::move(newTransitionRewards), newChoiceLabels);
            }
            
#ifdef STORM_HAVE_CARL
            template <typename ValueType, typename RewardModelType>
            Dtmc<ValueType, RewardModelType>::ConstraintCollector::ConstraintCollector(Dtmc<ValueType> const& dtmc) {
                process(dtmc);
            }
            
            template <typename ValueType, typename RewardModelType>
            std::unordered_set<storm::ArithConstraint<ValueType>> const& Dtmc<ValueType, RewardModelType>::ConstraintCollector::getWellformedConstraints() const {
                return this->wellformedConstraintSet;
            }
            
            template <typename ValueType, typename RewardModelType>
            std::unordered_set<storm::ArithConstraint<ValueType>> const& Dtmc<ValueType, RewardModelType>::ConstraintCollector::getGraphPreservingConstraints() const {
                return this->graphPreservingConstraintSet;
            }
            
            template <typename ValueType, typename RewardModelType>
            void Dtmc<ValueType, RewardModelType>::ConstraintCollector::process(storm::models::sparse::Dtmc<ValueType> const& dtmc) {
                for(uint_fast64_t state = 0; state < dtmc.getNumberOfStates(); ++state) {
                    ValueType sum = storm::utility::zero<ValueType>();
                    for (auto const& transition : dtmc.getRows(state)) {
                        sum += transition.getValue();
                        if (!storm::utility::isConstant(transition.getValue())) {
                            wellformedConstraintSet.emplace(transition.getValue() - 1, storm::CompareRelation::LEQ);
                            wellformedConstraintSet.emplace(transition.getValue(), storm::CompareRelation::GEQ);
                            graphPreservingConstraintSet.emplace(transition.getValue(), storm::CompareRelation::GREATER);
                        }
                    }
                    STORM_LOG_ASSERT(!storm::utility::isConstant(sum) || storm::utility::isOne(sum), "If the sum is a constant, it must be equal to 1.");
                    if(!storm::utility::isConstant(sum)) {
                        wellformedConstraintSet.emplace(sum - 1, storm::CompareRelation::EQ);
                    }
                    
                }
            }
            
            template <typename ValueType, typename RewardModelType>
            void Dtmc<ValueType, RewardModelType>::ConstraintCollector::operator()(storm::models::sparse::Dtmc<ValueType> const& dtmc) {
                process(dtmc);
            }
#endif
            
            template class Dtmc<double>;
            template class Dtmc<float>;

#ifdef STORM_HAVE_CARL
            template class Dtmc<storm::RationalFunction>;
#endif

        } // namespace sparse
    } // namespace models
} // namespace storm
