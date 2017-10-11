#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Bdd.h"

namespace storm {
    namespace solver {
        
        template<storm::dd::DdType DdType, typename ValueType = double>
        class SymbolicEquationSolver {
        public:
            SymbolicEquationSolver() = default;
            SymbolicEquationSolver(storm::dd::Bdd<DdType> const& allRows);

            void setLowerBounds(storm::dd::Add<DdType, ValueType> const& lowerBounds);
            void setLowerBound(ValueType const& lowerBound);
            void setUpperBounds(storm::dd::Add<DdType, ValueType> const& upperBounds);
            void setUpperBound(ValueType const& lowerBound);
            void setBounds(ValueType const& lowerBound, ValueType const& upperBound);
            void setBounds(storm::dd::Add<DdType, ValueType> const& lowerBounds, storm::dd::Add<DdType, ValueType> const& upperBounds);
            
            /*!
             * Retrieves a vector of lower bounds for all values (if any lower bounds are known).
             */
            storm::dd::Add<DdType, ValueType> getLowerBounds() const;
            
            /*!
             * Retrieves a vector of upper bounds for all values (if any lower bounds are known).
             */
            storm::dd::Add<DdType, ValueType> getUpperBounds() const;

        protected:
            storm::dd::DdManager<DdType>& getDdManager() const;

            void setAllRows(storm::dd::Bdd<DdType> const& allRows);
            storm::dd::Bdd<DdType> const& getAllRows() const;

            // The relevant rows to this equation solver.
            storm::dd::Bdd<DdType> allRows;

        private:
            // Lower bounds (if given).
            boost::optional<storm::dd::Add<DdType, ValueType>> lowerBounds;
            boost::optional<ValueType> lowerBound;
            
            // Upper bounds (if given).
            boost::optional<storm::dd::Add<DdType, ValueType>> upperBounds;
            boost::optional<ValueType> upperBound;
        };
        
    }
}
