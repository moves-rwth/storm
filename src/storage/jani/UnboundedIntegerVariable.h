#pragma once

#include "src/storage/jani/Variable.h"

namespace storm {
    namespace jani {
        
        class UnboundedIntegerVariable : public Variable {
        public:
            /*!
             * Creates an unbounded integer variable.
             */
            UnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValue);
        };
        
    }
}