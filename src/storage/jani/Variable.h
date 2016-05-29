#pragma once

#include <cstdint>
#include <string>

#include "src/storage/expressions/Variable.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace jani {
        
        class Variable {
        public:
            /*!
             * Creates a new variable.
             */
            Variable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValue);
            
            /*!
             * Retrieves the associated expression variable
             */
            storm::expressions::Variable const& getExpressionVariable() const;
            
            /*!
             * Retrieves the name of the variable.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves the initial value of the variable.
             */
            storm::expressions::Expression const& getInitialValue() const;
            
            /*!
             * Sets a new value as the initial value of the variable.
             */
            void setInitialValue(storm::expressions::Expression const& initialValue);
            
            /*!
             * Retrieves whether the variable has an initial value.
             */
            bool hasInitialValue() const;
            
        private:
            // The name of the variable.
            std::string name;
            
            // The expression variable associated with this variable.
            storm::expressions::Variable variable;
            
            // The expression defining the initial value of the variable.
            storm::expressions::Expression initialValue;
        };
        
    }
}