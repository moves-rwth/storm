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
        
    }
}
