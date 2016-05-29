#include "src/builder/DdJaniModelBuilder.h"

#include <sstream>

#include <boost/algorithm/string/join.hpp>

#include "src/logic/Formulas.h"

#include "src/utility/macros.h"
#include "src/utility/jani.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace builder {
        
        template <storm::dd::DdType Type, typename ValueType>
        DdJaniModelBuilder<Type, ValueType>::Options::Options() : buildAllRewardModels(true), rewardModelsToBuild(), constantDefinitions(), terminalStates(), negatedTerminalStates() {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        DdJaniModelBuilder<Type, ValueType>::Options::Options(storm::logic::Formula const& formula) : buildAllRewardModels(false), rewardModelsToBuild(), constantDefinitions(), terminalStates(), negatedTerminalStates() {
            this->preserveFormula(formula);
            this->setTerminalStatesFromFormula(formula);
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        DdJaniModelBuilder<Type, ValueType>::Options::Options(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) : buildAllRewardModels(false), rewardModelsToBuild(), constantDefinitions(), terminalStates(), negatedTerminalStates() {
            if (formulas.empty()) {
                this->buildAllRewardModels = true;
            } else {
                for (auto const& formula : formulas) {
                    this->preserveFormula(*formula);
                }
                if (formulas.size() == 1) {
                    this->setTerminalStatesFromFormula(*formulas.front());
                }
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        void DdJaniModelBuilder<Type, ValueType>::Options::preserveFormula(storm::logic::Formula const& formula) {
            // If we already had terminal states, we need to erase them.
            if (terminalStates) {
                terminalStates.reset();
            }
            if (negatedTerminalStates) {
                negatedTerminalStates.reset();
            }
            
            // If we are not required to build all reward models, we determine the reward models we need to build.
            if (!buildAllRewardModels) {
                std::set<std::string> referencedRewardModels = formula.getReferencedRewardModels();
                rewardModelsToBuild.insert(referencedRewardModels.begin(), referencedRewardModels.end());
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        void DdJaniModelBuilder<Type, ValueType>::Options::setTerminalStatesFromFormula(storm::logic::Formula const& formula) {
            if (formula.isAtomicExpressionFormula()) {
                terminalStates = formula.asAtomicExpressionFormula().getExpression();
            } else if (formula.isEventuallyFormula()) {
                storm::logic::Formula const& sub = formula.asEventuallyFormula().getSubformula();
                if (sub.isAtomicExpressionFormula() || sub.isAtomicLabelFormula()) {
                    this->setTerminalStatesFromFormula(sub);
                }
            } else if (formula.isUntilFormula()) {
                storm::logic::Formula const& right = formula.asUntilFormula().getRightSubformula();
                if (right.isAtomicExpressionFormula() || right.isAtomicLabelFormula()) {
                    this->setTerminalStatesFromFormula(right);
                }
                storm::logic::Formula const& left = formula.asUntilFormula().getLeftSubformula();
                if (left.isAtomicExpressionFormula()) {
                    negatedTerminalStates = left.asAtomicExpressionFormula().getExpression();
                }
            } else if (formula.isProbabilityOperatorFormula()) {
                storm::logic::Formula const& sub = formula.asProbabilityOperatorFormula().getSubformula();
                if (sub.isEventuallyFormula() || sub.isUntilFormula()) {
                    this->setTerminalStatesFromFormula(sub);
                }
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        void DdJaniModelBuilder<Type, ValueType>::Options::addConstantDefinitionsFromString(storm::jani::Model const& model, std::string const& constantDefinitionString) {
            std::map<storm::expressions::Variable, storm::expressions::Expression> newConstantDefinitions = storm::utility::jani::parseConstantDefinitionString(model, constantDefinitionString);
            
            // If there is at least one constant that is defined, and the constant definition map does not yet exist,
            // we need to create it.
            if (!constantDefinitions && !newConstantDefinitions.empty()) {
                constantDefinitions = std::map<storm::expressions::Variable, storm::expressions::Expression>();
            }
            
            // Now insert all the entries that need to be defined.
            for (auto const& entry : newConstantDefinitions) {
                constantDefinitions.get().insert(entry);
            }
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        DdJaniModelBuilder<Type, ValueType>::DdJaniModelBuilder(storm::jani::Model const& model, Options const& options) : options(options) {
            if (options.constantDefinitions) {
                this->model = model.defineUndefinedConstants(options.constantDefinitions.get());
            } else {
                this->model = model;
            }
            
            if (this->model->hasUndefinedConstants()) {
                std::vector<std::reference_wrapper<storm::jani::Constant const>> undefinedConstants = this->model->getUndefinedConstants();
                std::vector<std::string> strings;
                for (auto const& constant : undefinedConstants) {
                    std::stringstream stream;
                    stream << constant.get().getName() << " (" << constant.get().getType() << ")";
                    strings.push_back(stream.str());
                }
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Program still contains these undefined constants: " << boost::join(strings, ", ") << ".");
            }
            
            this->model = this->model->substituteConstants();
        }
        
        /*!
         * This structure represents a subcomponent of a composition.
         */
        template<storm::dd::DdType Type, typename ValueType>
        struct AutomatonDd {

        };
        
        template <storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>> DdJaniModelBuilder<Type, ValueType>::translate() {
            auto system = this->model->getSystemComposition().accept(*this, boost::any());
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        boost::any DdJaniModelBuilder<Type, ValueType>::visit(storm::jani::AutomatonComposition const& composition, boost::any const& data) {
            
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        boost::any DdJaniModelBuilder<Type, ValueType>::visit(storm::jani::RenameComposition const& composition, boost::any const& data) {
            
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        boost::any DdJaniModelBuilder<Type, ValueType>::visit(storm::jani::ParallelComposition const& composition, boost::any const& data) {
            
        }

        
        template class DdJaniModelBuilder<storm::dd::DdType::CUDD, double>;
        template class DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>;
    }
}