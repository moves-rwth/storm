#pragma once

#include <boost/optional.hpp>

#include "src/storage/dd/DdType.h"

#include "src/logic/Formula.h"

#include "src/storage/jani/Model.h"

namespace storm {
    namespace models {
        namespace symbolic {
            template <storm::dd::DdType Type, typename ValueType>
            class Model;
        }
    }
    
    namespace builder {
        
        template <storm::dd::DdType Type, typename ValueType = double>
        class DdJaniModelBuilder {
        public:            
            struct Options {
                /*!
                 * Creates an object representing the default building options.
                 */
                Options();
                
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
                Options(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas);
                
                /*!
                 * Sets the constants definitions from the given string. The string must be of the form 'X=a,Y=b,Z=c',
                 * etc. where X,Y,Z are the variable names and a,b,c are the values of the constants.
                 *
                 * @param program The program managing the constants that shall be defined. Note that the program itself
                 * is not modified whatsoever.
                 * @param constantDefinitionString The string from which to parse the constants' values.
                 */
                void addConstantDefinitionsFromString(storm::jani::Model const& model, std::string const& constantDefinitionString);
                
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
                
                // A flag that indicates whether or not all reward models are to be build.
                bool buildAllRewardModels;
                
                // A list of reward models to be build in case not all reward models are to be build.
                std::set<std::string> rewardModelsToBuild;
                
                // An optional mapping that, if given, contains defining expressions for undefined constants.
                boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> constantDefinitions;
                
                // An optional expression or label that (a subset of) characterizes the terminal states of the model.
                // If this is set, the outgoing transitions of these states are replaced with a self-loop.
                boost::optional<storm::expressions::Expression> terminalStates;
                
                // An optional expression or label whose negation characterizes (a subset of) the terminal states of the
                // model. If this is set, the outgoing transitions of these states are replaced with a self-loop.
                boost::optional<storm::expressions::Expression> negatedTerminalStates;
            };
            
            /*!
             * Creates a builder for the given model that uses the given options.
             */
            DdJaniModelBuilder(storm::jani::Model const& model, Options const& options = Options());
            
            /*!
             * Translates the given program into a symbolic model (i.e. one that stores the transition relation as a
             * decision diagram).
             *
             * @param model The model to translate.
             * @return A pointer to the resulting model.
             */
            std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>> translate();
            
            /*!
             * Retrieves the model that was actually translated (i.e. including constant substitutions etc.). Note
             * that this function may only be called after a succesful translation.
             *
             * @return The translated model.
             */
            storm::jani::Model const& getTranslatedModel() const;
            
        private:
            /// The model to translate.
            boost::optional<storm::jani::Model> model;
            
            /// The options to use for building the model.
            Options options;
        };
        
    }
}