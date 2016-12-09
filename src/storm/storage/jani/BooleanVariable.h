#pragma once

#include "storm/storage/jani/Variable.h"

namespace storm {
    namespace jani {
        
        class BooleanVariable : public Variable {
        public:
            
            /*!
             * Creates a Boolean variable with initial value
             */
            BooleanVariable(std::string const& name, storm::expressions::Variable const& variable);
            
            /*!
             * Creates a Boolean variable with initial value
             */
            BooleanVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValue, bool transient=false);
            
            virtual std::unique_ptr<Variable> clone() const override;
            
            virtual bool isBooleanVariable() const override;
        };
        
        /**
         * Convenience function to call the appropriate constructor and return a shared pointer to the variable.
         */
        std::shared_ptr<BooleanVariable> makeBooleanVariable(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient);
        
    }
}
