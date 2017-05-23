#ifndef STORM_BUILDER_EXPLICITMODELBUILDER_H
#define	STORM_BUILDER_EXPLICITMODELBUILDER_H

#include <memory>
#include <utility>
#include <vector>
#include <deque>
#include <cstdint>
#include <boost/functional/hash.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/variant.hpp>
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/storage/prism/Program.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"
#include "storm/storage/BitVectorHashMap.h"
#include "storm/logic/Formulas.h"
#include "storm/models/sparse/StateAnnotation.h"
#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/StateLabeling.h"
#include "storm/models/sparse/ChoiceLabeling.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/sparse/ChoiceOrigins.h"
#include "storm/storage/sparse/StateStorage.h"
#include "storm/settings/SettingsManager.h"

#include "storm/utility/prism.h"

#include "storm/builder/ExplorationOrder.h"
#include "storm/builder/ExplicitModelBuilderResult.h"

#include "storm/generator/NextStateGenerator.h"
#include "storm/generator/CompressedState.h"
#include "storm/generator/VariableInformation.h"

namespace storm {
    namespace utility {
        template<typename ValueType> class ConstantsComparator;
    }
    
    namespace builder {
        
        using namespace storm::utility::prism;
        using namespace storm::generator;
        
        // Forward-declare classes.
        template <typename ValueType> struct RewardModelBuilder;
        class ChoiceInformationBuilder;
        
        template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>, typename StateType = uint32_t>
        class ExplicitModelBuilder {
        public:
            // A structure holding the individual components of a model.
            struct ModelComponents {
                ModelComponents();
                
                // The transition matrix.
                storm::storage::SparseMatrix<ValueType> transitionMatrix;
                
                // The state labeling.
                storm::models::sparse::StateLabeling stateLabeling;
                
                // The reward models associated with the model.
                std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<typename RewardModelType::ValueType>> rewardModels;
                
                // A vector that stores a labeling for each choice.
                boost::optional<storm::models::sparse::ChoiceLabeling> choiceLabeling;
                
                // A vector that stores which states are markovian.
                boost::optional<storm::storage::BitVector> markovianStates;
                
                // Stores for each state to which variable valuation it belongs (is nullptr if not generated)
                std::shared_ptr<storm::storage::sparse::StateValuations> stateValuations;
                
                // Stores for each choice from which parts of the input model description it originates (is nullptr if not generated)
                std::shared_ptr<storm::storage::sparse::ChoiceOrigins> choiceOrigins;

            };
            
            struct Options {
                /*!
                 * Creates an object representing the default building options.
                 */
                Options();
                
                // The order in which to explore the model.
                ExplorationOrder explorationOrder;
                
                // A flag that indicates whether or not to store the state information after successfully building the
                // model. If it is to be preserved, it is contained in the result obtained from <code> build </code>
                bool buildStateValuations;
            };
            
            /*!
             * Creates an explicit model builder that uses the provided generator.
             *
             * @param generator The generator to use.
             */
            ExplicitModelBuilder(std::shared_ptr<storm::generator::NextStateGenerator<ValueType, StateType>> const& generator, Options const& options = Options());

            /*!
             * Creates an explicit model builder for the given PRISM program.
             *
             * @param program The program for which to build the model.
             */
            ExplicitModelBuilder(storm::prism::Program const& program, storm::generator::NextStateGeneratorOptions const& generatorOptions = storm::generator::NextStateGeneratorOptions(), Options const& builderOptions = Options());

            /*!
             * Creates an explicit model builder for the given JANI model.
             *
             * @param model The JANI model for which to build the model.
             */
            ExplicitModelBuilder(storm::jani::Model const& model, storm::generator::NextStateGeneratorOptions const& generatorOptions = storm::generator::NextStateGeneratorOptions(), Options const& builderOptions = Options());
            
            /*!
             * Convert the program given at construction time to an abstract model. The type of the model is the one
             * specified in the program. The given reward model name selects the rewards that the model will contain.
             *
             * @return The explicit model that was given by the probabilistic program as well as additional
             *         information (if requested).
             */
            ExplicitModelBuilderResult<ValueType, RewardModelType> build();
            
        private:
            /*!
             * Retrieves the state id of the given state. If the state has not been encountered yet, it will be added to
             * the lists of all states with a new id. If the state was already known, the object that is pointed to by
             * the given state pointer is deleted and the old state id is returned. Note that the pointer should not be
             * used after invoking this method.
             *
             * @param state A pointer to a state for which to retrieve the index. This must not be used after the call.
             * @return A pair indicating whether the state was already discovered before and the state id of the state.
             */
            StateType getOrAddStateIndex(CompressedState const& state);
    
            /*!
             * Builds the transition matrix and the transition reward matrix based for the given program.
             *
             * @param transitionMatrixBuilder The builder of the transition matrix.
             * @param rewardModelBuilders The builders for the selected reward models.
             * @param choiceInformationBuilder The builder for the requested information of the choices
             * @param markovianChoices is set to a bit vector storing whether a choice is markovian (is only set if the model type requires this information).
             */
            void buildMatrices(storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, std::vector<RewardModelBuilder<typename RewardModelType::ValueType>>& rewardModelBuilders, ChoiceInformationBuilder& choiceInformationBuilder, boost::optional<storm::storage::BitVector>& markovianChoices);
            
            /*!
             * Explores the state space of the given program and returns the components of the model as a result.
             *
             * @return A structure containing the components of the resulting model.
             */
            ModelComponents buildModelComponents();
            
            /*!
             * Builds the state labeling for the given program.
             *
             * @return The state labeling of the given program.
             */
            storm::models::sparse::StateLabeling buildStateLabeling();
            
            /// The generator to use for the building process.
            std::shared_ptr<storm::generator::NextStateGenerator<ValueType, StateType>> generator;
            
            /// The options to be used for the building process.
            Options options;

            /// Internal information about the states that were explored.
            storm::storage::sparse::StateStorage<StateType> stateStorage;
            
            /// A set of states that still need to be explored.
            std::deque<CompressedState> statesToExplore;
            
            /// An optional mapping from state indices to the row groups in which they actually reside. This needs to be
            /// built in case the exploration order is not BFS.
            boost::optional<std::vector<uint_fast64_t>> stateRemapping;

        };
        
    } // namespace adapters
} // namespace storm

#endif	/* STORM_BUILDER_EXPLICITMODELBUILDER_H */
