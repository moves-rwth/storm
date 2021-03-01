#pragma once

#include "storm/storage/jani/Variable.h"
#include "storm/storage/expressions/Expressions.h"

namespace storm {
    namespace jani {
        
        class LValue {
        public:
            explicit LValue(storm::jani::Variable const& variable);
            LValue(LValue const& array, std::vector<storm::expressions::Expression> const&, std::vector<size_t> const& sizes);
            LValue(LValue const& array, storm::expressions::Expression const&, size_t size);

            LValue(LValue const&) = default;
            bool operator==(LValue const& other) const;

            bool isVariable() const;
            storm::jani::Variable const& getVariable() const;
            
            bool isArrayAccess() const;
            storm::jani::Variable const& getArray() const;
            std::vector<storm::expressions::Expression> const& getArrayIndexVector() const;
            storm::expressions::Expression const& getArrayIndex() const;
            std::vector<size_t> const& getSizes() const;
            size_t const& getSizeAt(int i) const;
            size_t const getTotalSize() const;
            bool arrayIndexContainsVariable() const;
            void setArrayIndex(std::vector<storm::expressions::Expression> const& newIndex);
            
            bool isTransient() const;
            bool operator< (LValue const& other) const;
            
            LValue changeAssignmentVariables(std::map<Variable const*, std::reference_wrapper<Variable const>> const& remapping) const;
            
            friend std::ostream& operator<<(std::ostream& stream, LValue const& lvalue);

        private:
            
            // The variable being assigned.
            storm::jani::Variable const* variable;


            // In case of an array access LValue, this is the accessed index of the array.
            boost::optional<storm::expressions::Expression> arrayIndex;
            boost::optional<std::vector<storm::expressions::Expression>> arrayIndexVector;
            std::vector<size_t> sizes;
        };
    }
}