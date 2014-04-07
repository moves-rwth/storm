#ifndef STORM_STORAGE_PRISM_PROGRAM_H_
#define STORM_STORAGE_PRISM_PROGRAM_H_

#include <map>
#include <vector>
#include <set>
#include <boost/container/flat_set.hpp>

#include "src/storage/expressions/Expression.h"
#include "src/storage/prism/Module.h"
#include "src/storage/prism/RewardModel.h"

namespace storm {
    namespace prism {
        class Program {
        public:
            /*!
             * An enum for the different model types.
             */
            enum class ModelType {UNDEFINED, DTMC, CTMC, MDP, CTMDP, MA};
            
            /*!
             * Creates a program with the given model type, undefined constants, global variables, modules, reward
             * models, labels and initial states.
             *
             * @param modelType The type of the program.
             * @param undefinedBooleanConstants The undefined boolean constants of the program.
             * @param undefinedIntegerConstants The undefined integer constants of the program.
             * @param undefinedDoubleConstants The undefined double constants of the program.
             * @param globalBooleanVariables The global boolean variables of the program.
             * @param globalIntegerVariables The global integer variables of the program.
             * @param modules The modules of the program.
             * @param hasInitialStatesExpression A flag indicating whether the program specifies its initial states via
             * an explicit initial construct.
             * @param initialStatesExpression If the model specifies an explicit initial construct, this
             * expression defines its initial states. Otherwise it is irrelevant and may be set to an arbitrary (but
             * valid) expression, e.g. false.
             * @param rewardModels The reward models of the program.
             * @param labels The labels defined for this program.
             */
            Program(ModelType modelType, std::set<std::string> const& undefinedBooleanConstants, std::set<std::string> const& undefinedIntegerConstants, std::set<std::string> const& undefinedDoubleConstants, std::map<std::string, BooleanVariable> const& globalBooleanVariables, std::map<std::string, IntegerVariable> const& globalIntegerVariables, std::vector<storm::prism::Module> const& modules, std::map<std::string, storm::prism::RewardModel> const& rewardModels, bool hasInitialStatesExpression, storm::expressions::Expression const& initialStatesExpression, std::map<std::string, storm::expressions::Expression> const& labels);
            
            // Provide default implementations for constructors and assignments.
            Program() = default;
            Program(Program const& otherProgram) = default;
            Program& operator=(Program const& otherProgram) = default;
            Program(Program&& otherProgram) = default;
            Program& operator=(Program&& otherProgram) = default;
            
            /*!
             * Retrieves the model type of the model.
             *
             * @return The type of the model.
             */
            ModelType getModelType() const;

            /*!
             * Retrieves whether there are undefined constants of any type in the program.
             *
             * @return True iff there are undefined constants of any type in the program.
             */
            bool hasUndefinedConstants() const;
            
            /*!
             * Retrieves whether there are boolean undefined constants in the program.
             *
             * @return True iff there are boolean undefined constants in the program.
             */
            bool hasUndefinedBooleanConstants() const;

            /*!
             * Retrieves whether there are integer undefined constants in the program.
             *
             * @return True iff there are integer undefined constants in the program.
             */
            bool hasUndefinedIntegerConstants() const;
            
            /*!
             * Retrieves whether there are double undefined constants in the program.
             *
             * @return True iff there are double undefined constants in the program.
             */
            bool hasUndefinedDoubleConstants() const;
            
            /*!
             * Retrieves the undefined boolean constants of the program.
             *
             * @return The undefined boolean constants of the program.
             */
            std::set<std::string> const& getUndefinedBooleanConstants() const;
            
            /*!
             * Retrieves the undefined integer constants of the program.
             *
             * @return The undefined integer constants of the program.
             */
            std::set<std::string> const& getUndefinedIntegerConstants() const;

            /*!
             * Retrieves the undefined double constants of the program.
             *
             * @return The undefined double constants of the program.
             */
            std::set<std::string> const& getUndefinedDoubleConstants() const;
            
            /*!
             * Retrieves the global boolean variables of the program.
             *
             * @return The global boolean variables of the program.
             */
            std::map<std::string, storm::prism::BooleanVariable> const& getGlobalBooleanVariables() const;
            
            /*!
             * Retrieves a the global boolean variable with the given name.
             *
             * @param variableName The name of the global boolean variable to retrieve.
             * @return The global boolean variable with the given name.
             */
            storm::prism::BooleanVariable const& getGlobalBooleanVariable(std::string const& variableName) const;
            
            /*!
             * Retrieves the global integer variables of the program.
             *
             * @return The global integer variables of the program.
             */
            std::map<std::string, storm::prism::IntegerVariable> const& getGlobalIntegerVariables() const;

            /*!
             * Retrieves a the global integer variable with the given name.
             *
             * @param variableName The name of the global integer variable to retrieve.
             * @return The global integer variable with the given name.
             */
            storm::prism::IntegerVariable const& getGlobalIntegerVariable(std::string const& variableName) const;

            /*!
             * Retrieves the number of global boolean variables of the program.
             *
             * @return The number of global boolean variables of the program.
             */
            std::size_t getNumberOfGlobalBooleanVariables() const;
            
            /*!
             * Retrieves the number of global integer variables of the program.
             *
             * @return The number of global integer variables of the program.
             */
            std::size_t getNumberOfGlobalIntegerVariables() const;

            /*!
             * Retrieves the number of modules in the program.
             *
             * @return The number of modules in the program.
             */
            std::size_t getNumberOfModules() const;
            
            /*!
             * Retrieves the module with the given index.
             *
             * @param index The index of the module to retrieve.
             * @return The module with the given index.
             */
            storm::prism::Module const& getModule(uint_fast64_t index) const;
            
            /*!
             * Retrieves all modules of the program.
             *
             * @return All modules of the program.
             */
            std::vector<storm::prism::Module> const& getModules() const;
            
            /*!
             * Retrieves whether the program explicitly specifies an expression characterizing the initial states.
             *
             * @return True iff the program specifies an expression defining the initial states.
             */
            bool definesInitialStatesExpression() const;
            
            /*!
             * Retrieves an expression characterizing the initial states of the program.
             *
             * @return An expression characterizing the initial states.
             */
            storm::expressions::Expression getInitialStatesExpression() const;
            
            /*!
             * Retrieves the set of actions present in the program.
             *
             * @return The set of actions present in the program.
             */
            std::set<std::string> const& getActions() const;
            
            /*!
             * Retrieves the indices of all modules within this program that contain commands that are labelled with the
             * given action.
             *
             * @param action The name of the action the modules are supposed to possess.
             * @return A set of indices of all matching modules.
             */
            std::set<uint_fast64_t> const& getModuleIndicesByAction(std::string const& action) const;
            
            /*!
             * Retrieves the index of the module in which the given variable name was declared.
             *
             * @param variableName The name of the variable to search.
             * @return The index of the module in which the given variable name was declared.
             */
            uint_fast64_t getModuleIndexByVariable(std::string const& variableName) const;
            
            /*!
             * Retrieves the reward models of the program.
             *
             * @return The reward models of the program.
             */
            std::map<std::string, storm::prism::RewardModel> const& getRewardModels() const;
            
            /*!
             * Retrieves the reward model with the given name.
             *
             * @param rewardModelName The name of the reward model to return.
             * @return The reward model with the given name.
             */
            storm::prism::RewardModel const& getRewardModel(std::string const& rewardModelName) const;
            
            /*!
             * Retrieves all labels that are defined by the probabilitic program.
             *
             * @return A set of labels that are defined in the program.
             */
            std::map<std::string, storm::expressions::Expression> const& getLabels() const;
            
            /*!
             * Creates a new program that drops all commands whose indices are not in the given set.
             *
             * @param indexSet The set of indices for which to keep the commands.
             */
            Program restrictCommands(boost::container::flat_set<uint_fast64_t> const& indexSet);
            
            friend std::ostream& operator<<(std::ostream& stream, Program const& program);
            
        private:
            // The type of the model.
            ModelType modelType;
            
            // A list of undefined boolean constants of the model.
            std::set<std::string> undefinedBooleanConstants;
            
            // A list of undefined integer constants of the model.
            std::set<std::string> undefinedIntegerConstants;

            // A list of undefined double constants of the model.
            std::set<std::string> undefinedDoubleConstants;
            
            // A list of global boolean variables.
            std::map<std::string, BooleanVariable> globalBooleanVariables;
            
            // A list of global integer variables.
            std::map<std::string, IntegerVariable> globalIntegerVariables;
            
            // The modules associated with the program.
            std::vector<storm::prism::Module> modules;
            
            // The reward models associated with the program.
            std::map<std::string, storm::prism::RewardModel> rewardModels;
            
            // A flag that indicates whether the initial states of the program were given explicitly (in the form of an
            // initial construct) or implicitly (attached to the variable declarations).
            bool hasInitialStatesExpression;
            
            // The expression contained in the initial construct (if any).
            storm::expressions::Expression initialStatesExpression;
            
            // The labels that are defined for this model.
            std::map<std::string, storm::expressions::Expression> labels;
            
            // The set of actions present in this program.
            std::set<std::string> actions;
            
            // A map of actions to the set of modules containing commands labelled with this action.
            std::map<std::string, std::set<uint_fast64_t>> actionsToModuleIndexMap;
            
            // A mapping from variable names to the modules in which they were declared.
            std::map<std::string, uint_fast64_t> variableToModuleIndexMap;
        };
        
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_PROGRAM_H_ */
