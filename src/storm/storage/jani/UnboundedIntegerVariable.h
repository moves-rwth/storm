#pragma once

#include "storm/storage/jani/Variable.h"

namespace storm {
    namespace jani {
        
        class UnboundedIntegerVariable : public Variable {
        public:
            /*!
             * Creates an unbounded integer variable without initial value.
             */
            UnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable);
            /*!
             * Creates an unbounded integer variable with initial value.
             */
            UnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const&, bool transient=false);
            
            virtual std::unique_ptr<Variable> clone() const override;
            
            virtual bool isUnboundedIntegerVariable() const override;
        };
        
        /**
        * Convenience function to call the appropriate constructor and return a shared pointer to the variable.
        */
        std::shared_ptr<UnboundedIntegerVariable> makeUnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient);
        
    }
}
