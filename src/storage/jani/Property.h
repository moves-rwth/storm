#pragma once

#include "src/logic/formulas.h"

namespace storm {
    namespace jani {
        
        enum class FilterType { MIN, MAX, SUM, AVG, COUNT, FORALL, EXISTS, ARGMIN, ARGMAX, VALUES };
        
        
        class PropertyExpression {
            
        };
        
        class FilterExpression : public PropertyExpression {
            std::shared_ptr<PropertyExpression> states;
            std::shared_ptr<PropertyExpression> values;
            FilterType ft;
        };
        
        class PathExpression : public PropertyExpression {
            std::shared_ptr<PropertyExpression> leftStateExpression;
            std::shared_ptr<PropertyExpression> rightStateExpression;
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
            PropertyExpression propertyExpression;
        };
    }
}

