#include "storm/storage/dd/bisimulation/PreservationInformation.h"

#include "storm/logic/Formulas.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template <storm::dd::DdType DdType, typename ValueType>
            PreservationInformation<DdType, ValueType>::PreservationInformation(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::storage::BisimulationType const& bisimulationType) : PreservationInformation(model, model.getLabels(), bisimulationType) {
                // Intentionally left empty.
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            PreservationInformation<DdType, ValueType>::PreservationInformation(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::string> const& labels, storm::storage::BisimulationType const&) {
                for (auto const& label : labels) {
                    this->addLabel(label);
                    this->addExpression(model.getExpression(label));
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            PreservationInformation<DdType, ValueType>::PreservationInformation(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::expressions::Expression> const& expressions, storm::storage::BisimulationType const&) {
                for (auto const& e : expressions) {
                    this->addExpression(e);
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            PreservationInformation<DdType, ValueType>::PreservationInformation(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType const&) {
                if (formulas.empty()) {
                    // Default to respect all labels if no formulas are given.
                    for (auto const& label : model.getLabels()) {
                        this->addLabel(label);
                        this->addExpression(model.getExpression(label));
                    }
                } else {
                    std::set<std::string> labels;
                    std::set<storm::expressions::Expression> expressions;
                    
                    for (auto const& formula : formulas) {
                        for (auto const& expressionFormula : formula->getAtomicExpressionFormulas()) {
                            this->addExpression(expressionFormula->getExpression());
                        }
                        for (auto const& labelFormula : formula->getAtomicLabelFormulas()) {
                            this->addLabel(labelFormula->getLabel());
                            std::string const& label = labelFormula->getLabel();
                            STORM_LOG_THROW(model.hasLabel(label), storm::exceptions::InvalidPropertyException, "Property refers to illegal label '" << label << "'.");
                            this->addExpression(model.getExpression(label));
                        }
                    }
                    
                    std::vector<storm::expressions::Expression> expressionVector;
                    for (auto const& expression : expressions) {
                        expressionVector.emplace_back(expression);
                    }
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PreservationInformation<DdType, ValueType>::addLabel(std::string const& label) {
                labels.insert(label);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PreservationInformation<DdType, ValueType>::addExpression(storm::expressions::Expression const& expression) {
                expressions.insert(expression);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::set<std::string> const& PreservationInformation<DdType, ValueType>::getLabels() const {
                return labels;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::set<storm::expressions::Expression> const& PreservationInformation<DdType, ValueType>::getExpressions() const {
                return expressions;
            }
            
            template class PreservationInformation<storm::dd::DdType::CUDD, double>;
            
            template class PreservationInformation<storm::dd::DdType::Sylvan, double>;
            template class PreservationInformation<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class PreservationInformation<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        }
    }
}
