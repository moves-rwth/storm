#pragma once

#include "storm/solver/SymbolicLinearEquationSolver.h"

namespace storm {
namespace solver {

template<storm::dd::DdType DdType, typename ValueType = double>
class SymbolicEliminationLinearEquationSolver : public SymbolicLinearEquationSolver<DdType, ValueType> {
   public:
    /*!
     * Constructs a symbolic linear equation solver.
     *
     */
    SymbolicEliminationLinearEquationSolver();

    SymbolicEliminationLinearEquationSolver(
        storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables,
        std::set<storm::expressions::Variable> const& columnMetaVariables,
        std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs);

    SymbolicEliminationLinearEquationSolver(
        storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables,
        std::set<storm::expressions::Variable> const& columnMetaVariables,
        std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs);

    virtual storm::dd::Add<DdType, ValueType> solveEquations(Environment const& env, storm::dd::Add<DdType, ValueType> const& x,
                                                             storm::dd::Add<DdType, ValueType> const& b) const override;

    virtual void setData(storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables,
                         std::set<storm::expressions::Variable> const& columnMetaVariables,
                         std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) override;

    virtual LinearEquationSolverProblemFormat getEquationProblemFormat(Environment const& env) const override;

   private:
    void createInternalData(storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables,
                            std::set<storm::expressions::Variable> const& columnMetaVariables,
                            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs);

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
    using SymbolicLinearEquationSolverFactory<DdType, ValueType>::create;

    virtual std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<DdType, ValueType>> create(Environment const& env) const override;
};

}  // namespace solver
}  // namespace storm
