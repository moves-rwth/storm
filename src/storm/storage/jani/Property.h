#pragma once

#include "storm/modelchecker/results/FilterType.h"
#include "storm/logic/Formulas.h"

namespace storm {
    namespace jani {
        
        /**
         *  Property intervals as per Jani Specification. 
         *  Currently mainly used to help parsing.
         */
        struct PropertyInterval {
            storm::expressions::Expression lowerBound;
            bool lowerBoundStrict = false;
            storm::expressions::Expression upperBound;
            bool upperBoundStrict = false;
            
            bool hasLowerBound() {
                return lowerBound.isInitialized();
            }
            
            bool hasUpperBound() {
                return upperBound.isInitialized();
            }
        };
        
        
        class FilterExpression {
        public:
            explicit FilterExpression(std::shared_ptr<storm::logic::Formula const> formula, storm::modelchecker::FilterType ft = storm::modelchecker::FilterType::VALUES) : formula(formula), ft(ft) {}
            
            
            std::shared_ptr<storm::logic::Formula const> const& getFormula() const {
                return formula;
            }
            
            storm::modelchecker::FilterType getFilterType() const {
                return ft;
            }
            
            FilterExpression substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
                return FilterExpression(formula->substitute(substitution), ft);
            }
            
            FilterExpression substituteLabels(std::map<std::string, std::string> const& labelSubstitution) const {
                return FilterExpression(formula->substitute(labelSubstitution), ft);
            }

        private:
            // For now, we assume that the states are always the initial states.
            std::shared_ptr<storm::logic::Formula const> formula;
            storm::modelchecker::FilterType ft;
        };
        
        
        std::ostream& operator<<(std::ostream& os, FilterExpression const& fe);
        
        
        
        class Property {
        public:
            /**
             * Constructs the property
             * @param name the name
             * @param formula the formula representation
             * @param comment An optional comment
             */
            Property(std::string const& name, std::shared_ptr<storm::logic::Formula const> const& formula, std::string const& comment = "");
            
            /**
             * Constructs the property
             * @param name the name
             * @param formula the formula representation
             * @param comment An optional comment
             */
            Property(std::string const& name, FilterExpression const& fe, std::string const& comment = "");
            
            /**
             * Get the provided name
             * @return the name
             */
            std::string const& getName() const;
            
            /**
             * Get the provided comment, if any
             * @return the comment
             */
            std::string const& getComment() const;
            
            Property substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const;
            Property substituteLabels(std::map<std::string, std::string> const& labelSubstitution) const;
            
            FilterExpression const& getFilter() const;
            
            std::shared_ptr<storm::logic::Formula const> getRawFormula() const;
        private:
            std::string name;
            std::string comment;
            FilterExpression filterExpression;
        };
        
        
        std::ostream& operator<<(std::ostream& os, Property const& p);
    }
}

