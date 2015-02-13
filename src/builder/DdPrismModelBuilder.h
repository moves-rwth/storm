#ifndef STORM_BUILDER_DDPRISMMODELBUILDER_H_
#define STORM_BUILDER_DDPRISMMODELBUILDER_H_

#include <map>
#include <boost/optional.hpp>

#include "src/logic/Formulas.h"
#include "src/storage/prism/Program.h"
#include "src/adapters/DdExpressionAdapter.h"

namespace storm {
    namespace adapters {
        
        template <storm::dd::DdType Type>
        class DdPrismModelBuilder {
        public:
            struct Options {
                /*!
                 * Creates an object representing the default building options.
                 */
                Options();
                
                /*! Creates an object representing the suggested building options assuming that the given formula is the
                 * only one to check.
                 *
                 * @param formula The formula based on which to choose the building options.
                 */
                Options(storm::logic::Formula const& formula);
                
                /*!
                 * Sets the constants definitions from the given string. The string must be of the form 'X=a,Y=b,Z=c',
                 * etc. where X,Y,Z are the variable names and a,b,c are the values of the constants.
                 *
                 * @param program The program managing the constants that shall be defined. Note that the program itself
                 * is not modified whatsoever.
                 * @param constantDefinitionString The string from which to parse the constants' values.
                 */
                void addConstantDefinitionsFromString(storm::prism::Program const& program, std::string const& constantDefinitionString);
                
                // A flag that indicates whether or not a reward model is to be built.
                bool buildRewards;
                
                // An optional string, that, if given, indicates which of the reward models is to be built.
                boost::optional<std::string> rewardModelName;
                
                // An optional mapping that, if given, contains defining expressions for undefined constants.
                boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> constantDefinitions;
                
                // An optional set of labels that, if given, restricts the labels that are built.
                boost::optional<std::set<std::string>> labelsToBuild;
                
                // An optional set of expressions for which labels need to be built.
                boost::optional<std::vector<storm::expressions::Expression>> expressionLabels;
            };
            
            /*!
             * Translates the given program into a model that stores the transition relation as a decision diagram.
             *
             * @param program The program to translate.
             */
            static void translateProgram(storm::prism::Program const& program, Options const& options = Options());
            
        private:
            // This structure can store the decision diagrams representing a particular action.
            struct ActionDecisionDiagram {
                ActionDecisionDiagram(storm::dd::DdManager<Type> const& manager) : guardDd(manager.getZero()), transitionsDd(manager.getZero()), numberOfUsedNondeterminismVariables(0) {
                    // Intentionally left empty.
                }
                
                // The guard of the action.
                storm::dd::Dd<Type> guardDd;
                
                // The actual transitions (source and target states).
                storm::dd::Dd<Type> transitionsDd;
                
                // The number of variables that are used to encode the nondeterminism.
                uint_fast64_t numberOfUsedNondeterminismVariables;
            };
            
            // This structure holds all decision diagrams related to a module.
            struct ModuleDecisionDiagram {
                ModuleDecisionDiagram(storm::dd::DdManager<Type> const& manager) : independantAction(manager), synchronizingActionToDecisionDiagramMap(), identity(manager.getZero()) {
                    // Intentionally left empty.
                }
                
                // The decision diagram for the independant action.
                ActionDecisionDiagram independantAction;
                
                // A mapping from synchronizing action indices to the decision diagram.
                std::map<uint_fast64_t, ActionDecisionDiagram> synchronizingActionToDecisionDiagramMap;
                
                // A decision diagram that represents the identity of this module.
                storm::dd::Dd<Type> identity;
            };
            
            /*!
             * Structure to store all information required to generate the model from the program.
             */
            class GenerationInformation {
                GenerationInformation(storm::prism::Program const& program) : program(program), manager(std::make_shared<storm::dd::DdManager<Type>>()), rowMetaVariables(), columnMetaVariables(), metaVariablePairs(), numberOfNondetVariables(0), variableToIdentityMap(), moduleToIdentityMap(), allSynchronizingActionIndices(), labelToStateDdMap() {
                    // Initializes variables and identity DDs.
                    createMetaVariables();
                    createIdentities();
                }
                
            private:
                /*!
                 * Creates the required meta variables.
                 */
                void createMetaVariables();
                
                /*!
                 * Creates identity DDs for all variables and all modules.
                 */
                void createIdentities();
                
                // The program that is currently translated.
                storm::prism::Program const& program;
                
                // The manager used to build the decision diagrams.
                std::shared_ptr<storm::dd::DdManager<Type>> manager;

                // The meta variables for the row encoding.
                std::vector<storm::expressions::Variable> rowMetaVariables;
                
                // The meta variables for the column encoding.
                std::vector<storm::expressions::Variable> columnMetaVariables;

                // Pairs of meta variables (row, column).
                std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> metaVariablePairs;

                // Number of variables used to encode the nondeterminism.
                uint_fast64_t numberOfNondetVariables;
                
                // DDs representing the identity for each variable.
                std::map<storm::expressions::Variable, storm::dd::Dd<Type>> variableToIdentityMap;
                
                // DDs representing the identity for each module (index).
                std::map<uint_fast64_t, storm::dd::Dd<Type>> moduleToIdentityMap;
                
                // All synchronizing actions
                std::set<uint_fast64_t> allSynchronizingActionIndices;
                
                // DDs representing the identity matrix for each module
                std::unordered_map<std::string, storm::dd::Dd<Type>> labelToStateDdMap;
            };
            
        private:
            
//            /*!
//             * Creates system DD (full parallel)
//             *
//             * @return A DD representing the whole system
//             */
//            static storm::dd::Dd<Type> createSystemDecisionDiagramm(GenerationInformation & generationInfo);
//            
//            /*!
//             * Combines all DDs to one DD (including independent/synchronized actions)
//             *
//             * @param systemDds DDs representing the whole system
//             * @return System DDs with combined independent/synchronized actions
//             */
//            static SystemComponentDecisionDiagram<Type> combineSystem(GenerationInformation const & generationInfo, SystemComponentDecisionDiagram<Type> systemDds);
//            
//            /*!
//             * Combines 2 modules with/without synchronizing actions
//             *
//             * @param synchronizing Synchronizing actions or not
//             * @param module1 First module
//             * @param module1 Second module
//             * @param identity1 Identity matrix of first module
//             * @param identity2 Identity matrix of second module
//             * @return A module DD representing the combination of both modules
//             */
//            static ModuleDecisionDiagram<Type> combineModules(GenerationInformation const & generationInfo, bool synchronizing, ModuleDecisionDiagram<Type> moduleDd1, ModuleDecisionDiagram<Type> moduleDd2, storm::dd::Dd<Type> const& identityDd1, storm::dd::Dd<Type> const& identityDd2);
//            
//            /*!
//             * Combines 2 modules and solves non-determinism (MDP)
//             *
//             * @param module1 First module
//             * @param module1 Second module
//             * @return A module DD representing the combination of both modules
//             */
//            static ModuleDecisionDiagram<Type> combineModulesMDP(GenerationInformation const & generationInfo, ModuleDecisionDiagram<Type> moduleDd1, ModuleDecisionDiagram<Type> moduleDd2);
//            
//            /*!
//             * Creates a system component DD (module)
//             *
//             * @param module Module
//             * @param usedNondetVariablesVector Number of used nondet. variables
//             * @return A system component DD storing all required module information
//             */
//            static SystemComponentDecisionDiagram<Type> createSystemComponentDecisionDiagramm(GenerationInformation const & generationInfo, storm::prism::Module const & module, std::vector<uint_fast64_t> usedNondetVariablesVector);
//            
//            /*!
//             * Combines commands (with guards) to a module DD (DTMC)
//             *
//             * @param numberOfCommands Number of commands in the current module
//             * @param commandDds DDs containing all updates for each command
//             * @param guardDds Guard DDs for each command
//             * @return A DD representing the module.
//             */
//            static ModuleDecisionDiagram<Type> combineCommandsDTMC(std::shared_ptr<storm::dd::DdManager<Type>> const & manager, uint_fast64_t numberOfCommands, std::vector<storm::dd::Dd<Type>> const& commandDds, std::vector<storm::dd::Dd<Type>> const& guardDds);
//            
//            /*!
//             * Combines commands (with guards) to a module DD (MDP)
//             *
//             * @param numberOfCommands Number of commands in the current module
//             * @param commandDds DDs containing all updates for each command
//             * @param guardDds Guard DDs for each command
//             * @param usedNondetVariables Number of used nondet. variables
//             * @return A DD representing the module.
//             */
//            static ModuleDecisionDiagram<Type> combineCommandsMDP(std::shared_ptr<storm::dd::DdManager<Type>> const & manager, uint_fast64_t numberOfCommands, std::vector<storm::dd::Dd<Type>> const& commandDds, std::vector<storm::dd::Dd<Type>> const& guardDds, uint_fast64_t usedNondetVariables);
//            
//            /*!
//             * Creates a module DD
//             *
//             * @param module Module
//             * @param synchronizingAction Name of the synchronizing action ("" = no synchronization)
//             * @param usedNondetVariables Number of used nondet. variables
//             * @return A DD representing the module (with/without synchronized actions).
//             */
//            static ModuleDecisionDiagram<Type> createModuleDecisionDiagramm(GenerationInformation const & generationInfo, storm::prism::Module const& module, std::string const& synchronizingAction, uint_fast64_t usedNondetVariables);
//            
//            /*!
//             * Creates a command DD
//             *
//             * @param module Current module
//             * @param guard Command guard
//             * @param update Command expression
//             * @return A DD representing the command.
//             */
//            static storm::dd::Dd<Type> createCommandDecisionDiagramm(GenerationInformation const & generationInfo, storm::prism::Module const& module, storm::dd::Dd<Type> const& guard, storm::prism::Command const& command);
//            
//            /*!
//             * Creates an update DD
//             *
//             * @param module Current module
//             * @param guard Command guard
//             * @param update Update expression
//             * @return A DD representing the update.
//             */
//            static storm::dd::Dd<Type> createUpdateDecisionDiagramm(GenerationInformation const & generationInfo, storm::prism::Module const& module, storm::dd::Dd<Type> const& guard, storm::prism::Update const& update);
//            
//            /*!
//             * Creates initial state DD
//             *
//             * @return A DD representing the initial state.
//             */
//            static storm::dd::Dd<Type> getInitialStateDecisionDiagram(GenerationInformation const & generationInfo);
//            
//            /*!
//             * Calculates the reachable states of the given transition matrix
//             *
//             * @param systemDd The transition matrix DD
//             * @param initialStateDd All initial states
//             * @return A DD representing all reachable states
//             */
//            static storm::dd::Dd<Type> performReachability(GenerationInformation & generationInfo, storm::dd::Dd<Type> const& systemDd, storm::dd::Dd<Type> const& initialStateDd);
//            
//            /*!
//             * Adds a self-loop to deadlock states
//             *
//             * @param systemDd The given DD
//             * @param reachableStatesDd DD representing all reachable states
//             * @return A DD with fixed deadlocks.
//             */
//            static storm::dd::Dd<Type> findDeadlocks(GenerationInformation const & generationInfo, storm::dd::Dd<Type> systemDd, storm::dd::Dd<Type> const& reachableStatesDd);
//            
//            /*!
//             * Computes state and transition rewards
//             *
//             * @param systemDds System DDs
//             */
//            static std::pair<std::vector<storm::dd::Dd<Type>>, std::vector<storm::dd::Dd<Type>>> computeRewards(GenerationInformation const & generationInfo, SystemComponentDecisionDiagram<Type> const& systemDds);
        };
        
    } // namespace adapters
} // namespace storm

#endif /* STORM_BUILDER_DDPRISMMODELBUILDER_H_ */
