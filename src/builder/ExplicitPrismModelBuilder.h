#ifndef STORM_BUILDER_EXPLICITPRISMMODELBUILDER_H
#define	STORM_BUILDER_EXPLICITPRISMMODELBUILDER_H

#include <memory>
#include <utility>
#include <vector>
#include <deque>
#include <cstdint>
#include <boost/functional/hash.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include <src/models/sparse/StandardRewardModel.h>

#include "src/storage/prism/Program.h"
#include "src/storage/expressions/SimpleValuation.h"
#include "src/storage/expressions/ExpressionEvaluator.h"
#include "src/storage/BitVectorHashMap.h"
#include "src/logic/Formulas.h"
#include "src/models/sparse/StateAnnotation.h"
#include "src/models/sparse/Model.h"
#include "src/models/sparse/StateLabeling.h"
#include "src/storage/SparseMatrix.h"
#include "src/storage/sparse/StateValuations.h"
#include "src/storage/sparse/StateStorage.h"
#include "src/settings/SettingsManager.h"

#include "src/utility/prism.h"

#include "src/builder/ExplorationOrder.h"

#include "src/generator/CompressedState.h"
#include "src/generator/VariableInformation.h"

namespace storm {
    namespace utility {
        template<typename ValueType> class ConstantsComparator;
    }
    
    namespace builder {
        
        using namespace storm::utility::prism;
        using namespace storm::generator;
        
        // Forward-declare classes.
        template <typename ValueType> struct RewardModelBuilder;
        
        template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>, typename StateType = uint32_t>
        class ExplicitPrismModelBuilder {
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
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> choiceLabeling;
            };
            
            struct Options {
                /*!
                 * Creates an object representing the default building options.
                 */
                Options();
                
                /*!
                 * Copies the given set of options.
                 */
                Options(Options const& other) = default;
                
                /*! Creates an object representing the suggested building options assuming that the given formula is the
                 * only one to check. Additional formulas may be preserved by calling <code>preserveFormula</code>.
                 *
                 * @param formula The formula based on which to choose the building options.
                 */
                Options(storm::logic::Formula const& formula);

                /*! Creates an object representing the suggested building options assuming that the given formulas are
                 * the only ones to check. Additional formulas may be preserved by calling <code>preserveFormula</code>.
                 *
                 * @param formula Thes formula based on which to choose the building options.
                 */
                Options(std::vector<std::shared_ptr<const storm::logic::Formula>> const& formulas);
                
                /*!
                 * Sets the constants definitions from the given string. The string must be of the form 'X=a,Y=b,Z=c',
                 * etc. where X,Y,Z are the variable names and a,b,c are the values of the constants.
                 *
                 * @param program The program managing the constants that shall be defined. Note that the program itself
                 * is not modified whatsoever.
                 * @param constantDefinitionString The string from which to parse the constants' values.
                 */
                void addConstantDefinitionsFromString(storm::prism::Program const& program, std::string const& constantDefinitionString);
                
                /*!
                 * Changes the options in a way that ensures that the given formula can be checked on the model once it
                 * has been built.
                 *
                 * @param formula The formula that is to be ''preserved''.
                 */
                void preserveFormula(storm::logic::Formula const& formula);
                
                /*!
                 * Analyzes the given formula and sets an expression for the states states of the model that can be
                 * treated as terminal states. Note that this may interfere with checking properties different than the
                 * one provided.
                 *
                 * @param formula The formula used to (possibly) derive an expression for the terminal states of the
                 * model.
                 */
                void setTerminalStatesFromFormula(storm::logic::Formula const& formula);
                
                // The order in which to explore the model.
                ExplorationOrder explorationOrder;
                
                // A flag that indicates whether or not command labels are to be built.
                bool buildCommandLabels;
                
                // A flag that indicates whether or not all reward models are to be build.
                bool buildAllRewardModels;
                
                // A flag that indicates whether or not to store the state information after successfully building the
                // model. If it is to be preserved, it can be retrieved via the appropriate methods after a successful
                // call to <code>translateProgram</code>.
                bool buildStateValuations;
                
                // A list of reward models to be build in case not all reward models are to be build.
                std::set<std::string> rewardModelsToBuild;
                
                // An optional mapping that, if given, contains defining expressions for undefined constants.
                boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> constantDefinitions;
                
                // A flag that indicates whether all labels are to be build.
                bool buildAllLabels;
                
                // An optional set of labels that, if given, restricts the labels that are built.
                boost::optional<std::set<std::string>> labelsToBuild;
                
                // An optional set of expressions for which labels need to be built.
                boost::optional<std::vector<storm::expressions::Expression>> expressionLabels;
                
                // An optional expression or label that characterizes (a subset of) the terminal states of the model. If
                // this is set, the outgoing transitions of these states are replaced with a self-loop.
                boost::optional<boost::variant<storm::expressions::Expression, std::string>> terminalStates;
                
                // An optional expression or label whose negation characterizes (a subset of) the terminal states of the
                // model. If this is set, the outgoing transitions of these states are replaced with a self-loop.
                boost::optional<boost::variant<storm::expressions::Expression, std::string>> negatedTerminalStates;
            };
            
            /*!
             * Creates a builder for the given program.
             *
             * @param program The program to build.
             */
            ExplicitPrismModelBuilder(storm::prism::Program const& program, Options const& options = Options());
            
            /*!
             * Convert the program given at construction time to an abstract model. The type of the model is the one
             * specified in the program. The given reward model name selects the rewards that the model will contain.
             *
             * @param program The program to translate.
             * @param constantDefinitionString A string that contains a comma-separated definition of all undefined
             * constants in the model.
             * @param rewardModel The reward model that is to be built.
             * @return The explicit model that was given by the probabilistic program.
             */
            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> translate();
            
            /*!
             * If requested in the options, information about the variable valuations in the reachable states can be
             * retrieved via this function.
             *
             * @return A structure that stores information about all reachable states.
             */
            storm::storage::sparse::StateValuations const& getStateValuations() const;
            
            /*!
             * Retrieves the program that was actually translated (i.e. including constant substitutions etc.).
             *
             * @return The translated program.
             */
            storm::prism::Program const& getTranslatedProgram() const;
            
        private:
            storm::expressions::SimpleValuation unpackStateIntoValuation(storm::storage::BitVector const& currentState);
            
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
             * @param program The program for which to build the matrices.
             * @param variableInformation A structure containing information about the variables in the program.
             * @param transitionRewards A list of transition rewards that are to be considered in the transition reward
             * matrix.
             * @param deterministicModel A flag indicating whether the model is supposed to be deterministic or not.
             * @param transitionMatrix A reference to an initialized matrix which is filled with all transitions by this
             * function.
             * @param rewardModelBuilders A vector of reward model builders that is used to build the vector of selected
             * reward models.
             * @param terminalExpression If given, terminal states are not explored further.
             * @return A tuple containing a vector with all rows at which the nondeterministic choices of each state begin
             * and a vector containing the labels associated with each choice.
             */
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> buildMatrices(std::vector<std::reference_wrapper<storm::prism::RewardModel const>> const& selectedRewardModels, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, std::vector<RewardModelBuilder<typename RewardModelType::ValueType>>& rewardModelBuilders, boost::optional<storm::expressions::Expression> const& terminalExpression);
            
            /*!
             * Explores the state space of the given program and returns the components of the model as a result.
             *
             * @param program The program whose state space to explore.
             * @param selectedRewardModels The reward models that are to be considered.
             * @param options A set of options used to customize the building process.
             * @return A structure containing the components of the resulting model.
             */
            ModelComponents buildModelComponents(std::vector<std::reference_wrapper<storm::prism::RewardModel const>> const& selectedRewardModels);
            
            /*!
             * Builds the state labeling for the given program.
             *
             * @return The state labeling of the given program.
             */
            storm::models::sparse::StateLabeling buildStateLabeling();
            
            // The program to translate. The necessary transformations are performed upon construction of the builder.
            storm::prism::Program program;
            
            // The options to be used for translating the program.
            Options options;
            
            // The variable information.
            VariableInformation variableInformation;
            
            // Internal information about the states that were explored.
            storm::storage::sparse::StateStorage<StateType> stateStorage;
            
            // This member holds information about reachable states that can be retrieved from the outside after a
            // successful build.
            boost::optional<storm::storage::sparse::StateValuations> stateValuations;
            
            // A set of states that still need to be explored.
            std::deque<CompressedState> statesToExplore;
            
            // An optional mapping from state indices to the row groups in which they actually reside. This needs to be
            // built in case the exploration order is not BFS.
            boost::optional<std::vector<uint_fast64_t>> stateRemapping;

        };
        
    } // namespace adapters
} // namespace storm

#endif	/* STORM_BUILDER_EXPLICITPRISMMODELBUILDER_H */
