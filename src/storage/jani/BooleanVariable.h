#pragma once

#include "src/storage/jani/Variable.h"

namespace storm {
    namespace jani {
        
        class BooleanVariable : public Variable {
        public:
            
            /*!
             * Creates a Boolean variable with initial value
             */
            BooleanVariable(std::string const& name, storm::expressions::Variable const& variable, bool transient=false);
            
            /*!
             * Creates a Boolean variable with initial value
             */
            BooleanVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValue, bool transient=false);
            
            virtual bool isBooleanVariable() const override;
        };
        
    }
}