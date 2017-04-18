#pragma once

#include "storm/solver/SymbolicLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class SymbolicEliminationLinearEquationSolverSettings {
        public:
            // Intentionally left empty.
        };
        
        template<storm::dd::DdType DdType, typename ValueType = double>
        class SymbolicEliminationLinearEquationSolver : public SymbolicLinearEquationSolver<DdType, ValueType> {
        public:
            SymbolicEliminationLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs);
                        
            virtual storm::dd::Add<DdType, ValueType> solveEquations(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const override;
            
        private:
            std::vector<std::vector<storm::expressions::Variable>> oldToNewMapping;
            std::set<storm::expressions::Variable> newRowVariables;
            std::set<storm::expressions::Variable> newColumnVariables;
            std::set<storm::expressions::Variable> helperVariables;
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> newRowColumnMetaVariablePairs;
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> columnHelperMetaVariablePairs;
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> rowRowMetaVariablePairs;
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> columnColumnMetaVariablePairs;
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> oldNewMetaVariablePairs = rowRowMetaVariablePairs;
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> shiftMetaVariablePairs = newRowColumnMetaVariablePairs;
        };
        
        template<storm::dd::DdType DdType, typename ValueType>
        class SymbolicEliminationLinearEquationSolverFactory : public SymbolicLinearEquationSolverFactory<DdType, ValueType> {
        public:
            virtual std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<DdType, ValueType>> create(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const;
            
            SymbolicEliminationLinearEquationSolverSettings<ValueType>& getSettings();
            SymbolicEliminationLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
        private:
            SymbolicEliminationLinearEquationSolverSettings<ValueType> settings;
        };

    }
}
