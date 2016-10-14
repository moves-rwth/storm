#pragma once

#include "src/logic/Formulas.h"

namespace storm {
    namespace jani {
        
        enum class FilterType { MIN, MAX, SUM, AVG, COUNT, FORALL, EXISTS, ARGMIN, ARGMAX, VALUES };
        
        
        
        class FilterExpression {
        public:
            explicit FilterExpression(std::shared_ptr<storm::logic::Formula const> formula) : values(formula) {}
            
            std::shared_ptr<storm::logic::Formula const> const& getFormula() const {
                return values;
            }
            
            FilterType getFilterType() const {
                return ft;
            }
        private:
            // For now, we assume that the states are always the initial states.
            
            std::shared_ptr<storm::logic::Formula const> values;
            FilterType ft = FilterType::VALUES;
        };
        
        
        
        
        class Property {
            /**
             * Constructs the property
             * @param name the name
             * @param formula the formula representation
             * @param comment An optional comment
             */
            Property(std::string const& name, std::shared_ptr<storm::logic::Formula const> const& formula, std::string const& comment = "");
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
            
            
            
        private:
            std::string name;
            std::string comment;
            FilterExpression filterExpression;
        };
    }
}

