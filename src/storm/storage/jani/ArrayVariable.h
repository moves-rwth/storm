#pragma once

#include "storm/storage/jani/Variable.h"

namespace storm {
    namespace jani {
        
        class ArrayVariable : public Variable {
        public:
            
            enum class ElementType {Bool, Int, Real};
            
            /*!
             * Creates an Array variable
             */
            ArrayVariable(std::string const& name, storm::expressions::Variable const& variable, ElementType const& elementType);
            
            /*!
             * Creates an Array variable with initial value
             */
            ArrayVariable(std::string const& name, storm::expressions::Variable const& variable, ElementType const& elementType, storm::expressions::Expression const& initValue, bool transient);
            
            /*!
             * Sets the lower bound to the values stored in this array
             */
            void setLowerElementTypeBound(storm::expressions::Expression const& lowerBound);
            
             /*!
             * Sets the upper bound to the values stored in this array
             */
            void setUpperElementTypeBound(storm::expressions::Expression const& upperBound);
           
            /*!
             * Returns true if there is either an upper bound or a lower bound
             */
            bool hasElementTypeBound() const;
            
            /*!
             * Returns true if there is an upper element type bound
             */
            bool hasUpperElementTypeBound() const;
            
            /*!
             * Returns true if there is a lower element type bound
             */
            bool hasLowerElementTypeBound() const;
            
            /*!
             * Returns the upper element type bound. The returned expression might not be initialized if there is no such bound.
             */
            storm::expressions::Expression const& getUpperElementTypeBound() const;
            
            /*!
             * Returns the lower element type bound. The returned expression might not be initialized if there is no such bound.
             */
            storm::expressions::Expression const& getLowerElementTypeBound() const;

            std::pair<storm::expressions::Expression, storm::expressions::Expression> const& getElementTypeBounds() const;
            
            
            ElementType getElementType() const;
            
            virtual std::unique_ptr<Variable> clone() const override;
            virtual bool isArrayVariable() const override;
            virtual void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) override;


        private:
            ElementType elementType;
            storm::expressions::Expression lowerElementTypeBound, upperElementTypeBound;
        };
        
        
    }
}
