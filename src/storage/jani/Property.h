#pragma once

#include "src/logic/formulas.h"
#include "src/modelchecker/results/FilterType.h"
#include "src/logic/Formulas.h"


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
            explicit FilterExpression(std::shared_ptr<storm::logic::Formula const> formula, storm::modelchecker::FilterType ft = storm::modelchecker::FilterType::VALUES) : values(formula), ft(ft) {}
            
            
            std::shared_ptr<storm::logic::Formula const> const& getFormula() const {
                return values;
            }
            
            storm::modelchecker::FilterType getFilterType() const {
                return ft;
            }
        private:
            // For now, we assume that the states are always the initial states.
            std::shared_ptr<storm::logic::Formula const> values;
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
            
            FilterExpression const& getFilter() const;
        private:
            std::string name;
            std::string comment;
            FilterExpression filterExpression;
        };
        
        
        std::ostream& operator<<(std::ostream& os, Property const& p);
    }
}

