#pragma once

#include <cstdint>
#include <string>

#include "src/storage/expressions/Variable.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace jani {
        
        class BooleanVariable;
        class BoundedIntegerVariable;
        class UnboundedIntegerVariable;
        
        class Variable {
        public:
            /*!
             * Creates a new variable.
             */
            Variable(std::string const& name, storm::expressions::Variable const& variable);
            
            /*!
             * Retrieves the associated expression variable
             */
            storm::expressions::Variable const& getExpressionVariable() const;
            
            /*!
             * Retrieves the name of the variable.
             */
            std::string const& getName() const;
                        
            // Methods to determine the type of the variable.
            virtual bool isBooleanVariable() const;
            virtual bool isBoundedIntegerVariable() const;
            virtual bool isUnboundedIntegerVariable() const;
            
            // Methods to get the variable as a different type.
            BooleanVariable& asBooleanVariable();
            BooleanVariable const& asBooleanVariable() const;
            BoundedIntegerVariable& asBoundedIntegerVariable();
            BoundedIntegerVariable const& asBoundedIntegerVariable() const;
            UnboundedIntegerVariable& asUnboundedIntegerVariable();
            UnboundedIntegerVariable const& asUnboundedIntegerVariable() const;
            
        private:
            // The name of the variable.
            std::string name;
            
            // The expression variable associated with this variable.
            storm::expressions::Variable variable;
        };
        
    }
}