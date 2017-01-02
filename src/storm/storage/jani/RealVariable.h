#pragma once

#include "storm/storage/jani/Variable.h"

namespace storm {
    namespace jani {
        
        class RealVariable : public Variable {
        public:
            /*!
             * Creates a real variable without initial value.
             */
            RealVariable(std::string const& name, storm::expressions::Variable const& variable);
            
            /*!
             * Creates a real variable with initial value.
             */
            RealVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, bool transient=false);
            
            virtual std::unique_ptr<Variable> clone() const override;
            
            virtual bool isRealVariable() const override;
        };

        
    }
}
