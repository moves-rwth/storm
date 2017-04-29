#ifndef STORM_MODELS_SPARSE_DTMC_H_
#define STORM_MODELS_SPARSE_DTMC_H_
#include <unordered_set>

#include "storm/models/sparse/DeterministicModel.h"
#include "storm/utility/OsDetection.h"
#include "storm/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            /*!
             * This class represents a discrete-time Markov chain.
             */
            template<class ValueType, typename RewardModelType = StandardRewardModel<ValueType>>
            class Dtmc : public DeterministicModel<ValueType, RewardModelType> {
            public:
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                Dtmc(storm::storage::SparseMatrix<ValueType> const& probabilityMatrix,
                     storm::models::sparse::StateLabeling const& stateLabeling,
                     std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                     boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
                /*!
                 * Constructs a model by moving the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                Dtmc(storm::storage::SparseMatrix<ValueType>&& probabilityMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                     std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                     boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
                Dtmc(Dtmc<ValueType, RewardModelType> const& dtmc) = default;
                Dtmc& operator=(Dtmc<ValueType, RewardModelType> const& dtmc) = default;
                
                Dtmc(Dtmc<ValueType, RewardModelType>&& dtmc) = default;
                Dtmc& operator=(Dtmc<ValueType, RewardModelType>&& dtmc) = default;

                
#ifdef STORM_HAVE_CARL
                class ConstraintCollector {
                private:
                    // A set of constraints that says that the DTMC actually has valid probability distributions in all states.
                    std::unordered_set<storm::ArithConstraint<ValueType>> wellformedConstraintSet;
                    
                    // A set of constraints that makes sure that the underlying graph of the model does not change depending
                    // on the parameter values.
                    std::unordered_set<storm::ArithConstraint<ValueType>> graphPreservingConstraintSet;
                    
                public:
                    /*!
                     * Constructs the a constraint collector for the given DTMC. The constraints are built and ready for
                     * retrieval after the construction.
                     *
                     * @param dtmc The DTMC for which to create the constraints.
                     */
                    ConstraintCollector(storm::models::sparse::Dtmc<ValueType> const& dtmc);
                    
                    /*!
                     * Returns the set of wellformed-ness constraints.
                     *
                     * @return The set of wellformed-ness constraints.
                     */
                    std::unordered_set<storm::ArithConstraint<ValueType>> const&  getWellformedConstraints() const;
                    
                    /*!
                     * Returns the set of graph-preserving constraints.
                     *
                     * @return The set of graph-preserving constraints.
                     */
                    std::unordered_set<storm::ArithConstraint<ValueType>> const&  getGraphPreservingConstraints() const;
                    
                    /*!
                     * Constructs the constraints for the given DTMC.
                     *
                     * @param dtmc The DTMC for which to create the constraints.
                     */
                    void process(storm::models::sparse::Dtmc<ValueType> const& dtmc);
                    
                    /*!
                     * Constructs the constraints for the given DTMC by calling the process method.
                     *
                     * @param dtmc The DTMC for which to create the constraints.
                     */
                    void operator()(storm::models::sparse::Dtmc<ValueType> const& dtmc);
                    
                };
#endif
            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_DTMC_H_ */
