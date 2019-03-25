#pragma once

#include "storm/storage/jani/Variable.h"

namespace storm {
    namespace jani {
        
        class ClockVariable : public Variable {
        public:
            
            /*!
             * Creates a clock variable
             */
            ClockVariable(std::string const& name, storm::expressions::Variable const& variable);
            
            /*!
             * Creates a clock variable with initial value
             */
            ClockVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValue, bool transient=false);
            
            virtual std::unique_ptr<Variable> clone() const override;
            
            virtual bool isClockVariable() const override;
        };
        
        /**
         * Convenience function to call the appropriate constructor and return a shared pointer to the variable.
         */
        std::shared_ptr<ClockVariable> makeClockVariable(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient);
        
    }
}
