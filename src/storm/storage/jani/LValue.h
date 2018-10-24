#pragma once

#include "storm/storage/jani/ArrayVariable.h"
#include "storm/storage/expressions/Expressions.h"

namespace storm {
    namespace jani {
        
        class LValue {
        public:
            explicit LValue(storm::jani::Variable const& variable);
            LValue(LValue const& array, storm::expressions::Expression const& index);
            
            LValue(LValue const&) = default;
            bool operator==(LValue const& other) const;

            bool isVariable() const;
            storm::jani::Variable const& getVariable() const;
            
            bool isArrayAccess() const;
            storm::jani::ArrayVariable const& getArray() const;
            storm::expressions::Expression const& getArrayIndex() const;
            void setArrayIndex(storm::expressions::Expression const& newIndex);
            
            bool isTransient() const;
            bool operator< (LValue const& other) const;
            
            LValue changeAssignmentVariables(std::map<Variable const*, std::reference_wrapper<Variable const>> const& remapping) const;
            
            friend std::ostream& operator<<(std::ostream& stream, LValue const& lvalue);

        private:
            
            // The variable being assigned.
            storm::jani::Variable const* variable;
            
            
            // In case of an array access LValue, this is the accessed index of the array.
            storm::expressions::Expression arrayIndex;
        };
    }
}