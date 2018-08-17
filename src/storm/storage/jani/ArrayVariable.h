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
            ArrayVariable(std::string const& name, storm::expressions::Variable const& variable, ElementType elementType);
            
            /*!
             * Creates an Array variable with initial value
             */
            ArrayVariable(std::string const& name, storm::expressions::Variable const& variable, ElementType elementType, storm::expressions::Expression const& initialValue, bool transient);
            
            /*!
             * Sets/Gets bounds to the values stored in this array
             */
            void setElementTypeBounds(storm::expressions::Expression lowerBound, storm::expressions::Expression upperBound);
            bool hasElementTypeBounds() const;
            std::pair<storm::expressions::Expression, storm::expressions::Expression> const& getElementTypeBounds() const;
            
            /*!
             * Sets/Gets the maximum size of the array
             */
            void setMaxSize(uint64_t size);
            bool hasMaxSize() const;
            uint64_t getMaxSize() const;
            
            ElementType getElementType() const;
            
            virtual std::unique_ptr<Variable> clone() const override;
            virtual bool isArrayVariable() const override;
            virtual void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) override;


        private:
            ElementType elementType;
            boost::optional<std::pair<storm::expressions::Expression, storm::expressions::Expression>> elementTypeBounds;
            boost::optional<uint64_t> maxSize;
        };
        
        
    }
}
