#ifndef STORM_UTILITY_SOLVER_H_
#define STORM_UTILITY_SOLVER_H_

#include <iostream>

#include <set>
#include <vector>
#include <memory>

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/storage/sparse/StateType.h"
#include "storm/storage/dd/DdType.h"
#include "storm/solver/SolverSelectionOptions.h"

namespace storm {
    namespace solver {
        template<storm::dd::DdType T, typename ValueType>
        class SymbolicGameSolver;
        
        template<storm::dd::DdType T, typename V>
        class SymbolicLinearEquationSolver;
        
        template<storm::dd::DdType T, typename V>
        class SymbolicMinMaxLinearEquationSolver;
        
        template<typename V>
        class LinearEquationSolver;
        
        template<typename V>
        class MinMaxLinearEquationSolver;
        
        template<typename ValueType>
        class LpSolver;
        
        class SmtSolver;
    }

    namespace storage {
        template<typename V> class SparseMatrix;
    }
    
    namespace dd {
        template<storm::dd::DdType Type, typename ValueType>
        class Add;
        
        template<storm::dd::DdType Type>
        class Bdd;
    }
    
    namespace expressions {
        class Variable;
        class ExpressionManager;
    }
    
    namespace utility {
        namespace solver {
            
            template<typename ValueType>
            class LpSolverFactory {
            public:
                virtual ~LpSolverFactory() = default;

                /*!
                 * Creates a new linear equation solver instance with the given name.
                 *
                 * @param name The name of the LP solver.
                 * @return A pointer to the newly created solver.
                 */
                virtual std::unique_ptr<storm::solver::LpSolver<ValueType>> create(std::string const& name) const;
                virtual std::unique_ptr<storm::solver::LpSolver<ValueType>> create(std::string const& name, storm::solver::LpSolverTypeSelection solvType) const;
                
                virtual std::unique_ptr<LpSolverFactory<ValueType>> clone() const;

            };
            
            template<typename ValueType>
            class GlpkLpSolverFactory : public LpSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::LpSolver<ValueType>> create(std::string const& name) const override;
                virtual std::unique_ptr<LpSolverFactory<ValueType>> clone() const override;
            };
            
            template<typename ValueType>
            class GurobiLpSolverFactory : public LpSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::LpSolver<ValueType>> create(std::string const& name) const override;
                virtual std::unique_ptr<LpSolverFactory<ValueType>> clone() const override;
            };

            template<typename ValueType>
            class Z3LpSolverFactory : public LpSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::LpSolver<ValueType>> create(std::string const& name) const override;
                virtual std::unique_ptr<LpSolverFactory<ValueType>> clone() const override;
            };
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LpSolver<ValueType>> getLpSolver(std::string const& name, storm::solver::LpSolverTypeSelection solvType = storm::solver::LpSolverTypeSelection::FROMSETTINGS) ;

            class SmtSolverFactory {
            public:
                virtual ~SmtSolverFactory() = default;

                /*!
                 * Creates a new SMT solver instance.
                 *
                 * @param manager The expression manager responsible for the expressions that will be given to the SMT
                 * solver.
                 * @return A pointer to the newly created solver.
                 */
                virtual std::unique_ptr<storm::solver::SmtSolver> create(storm::expressions::ExpressionManager& manager) const;
            };
            
            class Z3SmtSolverFactory : public SmtSolverFactory {
            public:
                virtual std::unique_ptr<storm::solver::SmtSolver> create(storm::expressions::ExpressionManager& manager) const;
            };
            
            class MathsatSmtSolverFactory : public SmtSolverFactory {
            public:
                virtual std::unique_ptr<storm::solver::SmtSolver> create(storm::expressions::ExpressionManager& manager) const;
            };
            
            std::unique_ptr<storm::solver::SmtSolver> getSmtSolver(storm::expressions::ExpressionManager& manager);
        }
    }
}

#endif /* STORM_UTILITY_SOLVER_H_ */
