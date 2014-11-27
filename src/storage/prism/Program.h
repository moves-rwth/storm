#ifndef STORM_STORAGE_PRISM_PROGRAM_H_
#define STORM_STORAGE_PRISM_PROGRAM_H_

#include <map>
#include <vector>
#include <set>
#include <boost/container/flat_set.hpp>

#include "src/storage/expressions/Expression.h"
#include "src/storage/prism/Constant.h"
#include "src/storage/prism/Formula.h"
#include "src/storage/prism/Label.h"
#include "src/storage/prism/Module.h"
#include "src/storage/prism/RewardModel.h"
#include "src/storage/prism/InitialConstruct.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace prism {
        class Program : public LocatedInformation {
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
             * @param constants The constants of the program.
             * @param globalBooleanVariables The global boolean variables of the program.
             * @param globalIntegerVariables The global integer variables of the program.
             * @param formulas The formulas defined in the program.
             * @param modules The modules of the program.
             * @param fixInitialConstruct A flag that indicates whether the given initial construct is to be ignored and
             * replaced by a new one created from the initial values of the variables.
             * @param initialConstruct The initial construct of the program. If the initial construct specifies "false"
             * as the initial condition, the default values of the variables are used to construct a legal initial
             * condition.
             * @param rewardModels The reward models of the program.
             * @param labels The labels defined for this program.
             * @param filename The filename in which the program is defined.
             * @param lineNumber The line number in which the program is defined.
             * @param checkValidity If set to true, the program is checked for validity.
             */
            Program(ModelType modelType, std::vector<Constant> const& constants, std::vector<BooleanVariable> const& globalBooleanVariables, std::vector<IntegerVariable> const& globalIntegerVariables, std::vector<Formula> const& formulas, std::vector<Module> const& modules, std::vector<RewardModel> const& rewardModels, bool fixInitialConstruct, storm::prism::InitialConstruct const& initialConstruct, std::vector<Label> const& labels, std::string const& filename = "", uint_fast64_t lineNumber = 0, bool checkValidity = true);
            
            // Provide default implementations for constructors and assignments.
            Program() = default;
            Program(Program const& other) = default;
            Program& operator=(Program const& other) = default;
#ifndef WINDOWS
            Program(Program&& other) = default;
            Program& operator=(Program&& other) = default;
#endif
            
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
             * Retrieves whether the given constant exists in the program.
             *
             * @param constantName The name of the constant to search.
             * @return True iff the constant exists in the program.
             */
            bool hasConstant(std::string const& constantName) const;
            
            /*!
             * Retrieves the constant with the given name if it exists.
             *
             * @param constantName The name of the constant to retrieve.
             * @return The constant with the given name if it exists.
             */
            Constant const& getConstant(std::string const& constantName) const;
            
            /*!
             * Retrieves all constants defined in the program.
             *
             * @return The constants defined in the program.
             */
            std::vector<Constant> const& getConstants() const;
            
            /*!
             * Retrieves the number of all constants defined in the program.
             *
             * @return The number of constants defined in the program.
             */
            std::size_t getNumberOfConstants() const;
            
            /*!
             * Retrieves the global boolean variables of the program.
             *
             * @return The global boolean variables of the program.
             */
            std::vector<BooleanVariable> const& getGlobalBooleanVariables() const;
            
            /*!
             * Retrieves a the global boolean variable with the given name.
             *
             * @param variableName The name of the global boolean variable to retrieve.
             * @return The global boolean variable with the given name.
             */
            BooleanVariable const& getGlobalBooleanVariable(std::string const& variableName) const;
            
            /*!
             * Retrieves the global integer variables of the program.
             *
             * @return The global integer variables of the program.
             */
            std::vector<IntegerVariable> const& getGlobalIntegerVariables() const;

            /*!
             * Retrieves a the global integer variable with the given name.
             *
             * @param variableName The name of the global integer variable to retrieve.
             * @return The global integer variable with the given name.
             */
            IntegerVariable const& getGlobalIntegerVariable(std::string const& variableName) const;

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
             * Retrieves the formulas defined in the program.
             *
             * @return The formulas defined in the program.
             */
            std::vector<Formula> const& getFormulas() const;
            
            /*!
             * Retrieves the number of formulas in the program.
             *
             * @return The number of formulas in the program.
             */
            std::size_t getNumberOfFormulas() const;
            
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
            Module const& getModule(uint_fast64_t index) const;

            /*!
             * Retrieves the module with the given name.
             *
             * @param moduleName The name of the module to retrieve.
             * @return The module with the given name.
             */
            Module const& getModule(std::string const& moduleName) const;
            
            /*!
             * Retrieves all modules of the program.
             *
             * @return All modules of the program.
             */
            std::vector<Module> const& getModules() const;
            
            /*!
             * Retrieves the initial construct of the program.
             *
             * @return The initial construct of the program.
             */
            storm::prism::InitialConstruct const& getInitialConstruct() const;
            
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
             * Retrieves whether the program has reward models.
             *
             * @return True iff the program has at least one reward model.
             */
            bool hasRewardModel() const;
            
            /*!
             * Retrieves the reward models of the program.
             *
             * @return The reward models of the program.
             */
            std::vector<RewardModel> const& getRewardModels() const;
            
            /*!
             * Retrieves the number of reward models in the program.
             *
             * @return The number of reward models in the program.
             */
            std::size_t getNumberOfRewardModels() const;
            
            /*!
             * Retrieves the reward model with the given name.
             *
             * @param rewardModelName The name of the reward model to return.
             * @return The reward model with the given name.
             */
            RewardModel const& getRewardModel(std::string const& rewardModelName) const;
            
            /*!
             * Retrieves the reward model with the given index.
             *
             * @param index The index of the reward model to return.
             * @return The reward model with the given index.
             */
            RewardModel const& getRewardModel(uint_fast64_t index) const;
            
            /*!
             * Retrieves all labels that are defined by the probabilitic program.
             *
             * @return A set of labels that are defined in the program.
             */
            std::vector<Label> const& getLabels() const;
            
            /*!
             * Retrieves the number of labels in the program.
             *
             * @return The number of labels in the program.
             */
            std::size_t getNumberOfLabels() const;

            /*!
             * Creates a new program that drops all commands whose indices are not in the given set.
             *
             * @param indexSet The set of indices for which to keep the commands.
             */
            Program restrictCommands(boost::container::flat_set<uint_fast64_t> const& indexSet) const;
            
            /*!
             * Defines the undefined constants according to the given map and returns the resulting program.
             *
             * @param constantDefinitions A mapping from undefined constant names to the expressions they are supposed
             * to be replaced with.
             * @return The program after all undefined constants in the given map have been replaced with their
             * definitions.
             */
            Program defineUndefinedConstants(std::map<std::string, storm::expressions::Expression> const& constantDefinitions) const;
            
            /*!
             * Substitutes all constants appearing in the expressions of the program by their defining expressions. For
             * this to work, all constants need to be defined prior to calling this.
             *
             * @return The resulting program that only contains expressions over variables of the program.
             */
            Program substituteConstants() const;
            
            /*!
             * Checks the validity of the program. If the program is not valid, an exception is thrown with a message
             * that indicates the source of the problem.
             */
            void checkValidity() const;
            
            friend std::ostream& operator<<(std::ostream& stream, Program const& program);
            
        private:
            // Creates the internal mappings.
            void createMappings();
            
            // The type of the model.
            ModelType modelType;
            
            // The undefined constants of the program.
            std::vector<Constant> constants;
            
            // A mapping from constant names to their corresponding indices.
            std::map<std::string, uint_fast64_t> constantToIndexMap;

            // The global boolean variables.
            std::vector<BooleanVariable> globalBooleanVariables;
            
            // A mapping from global boolean variable names to their corresponding indices.
            std::map<std::string, uint_fast64_t> globalBooleanVariableToIndexMap;
            
            // The global integer variables.
            std::vector<IntegerVariable> globalIntegerVariables;
            
            // A mapping from global integer variable names to their corresponding indices.
            std::map<std::string, uint_fast64_t> globalIntegerVariableToIndexMap;
            
            // The formulas defined in the program.
            std::vector<Formula> formulas;
            
            // A mapping of formula names to their corresponding indices.
            std::map<std::string, uint_fast64_t> formulaToIndexMap;
            
            // The modules associated with the program.
            std::vector<Module> modules;
            
            // A mapping of module names to their indices.
            std::map<std::string, uint_fast64_t> moduleToIndexMap;
            
            // The reward models associated with the program.
            std::vector<RewardModel> rewardModels;
            
            // A mapping of reward models to their indices.
            std::map<std::string, uint_fast64_t> rewardModelToIndexMap;
            
            // The initial construct of the program.
            storm::prism::InitialConstruct initialConstruct;
            
            // The labels that are defined for this model.
            std::vector<Label> labels;
            
            // A mapping from label names to their corresponding indices.
            std::map<std::string, uint_fast64_t> labelToIndexMap;
            
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
