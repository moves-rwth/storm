#ifndef STORM_SOLVER_ELIMINATIONLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_ELIMINATIONLINEAREQUATIONSOLVER_H_

#include "storm/solver/LinearEquationSolver.h"

#include "storm/settings/modules/EliminationSettings.h"

namespace storm {
    namespace solver {
        template<typename ValueType>
        class EliminationLinearEquationSolverSettings {
        public:
            EliminationLinearEquationSolverSettings();
            
            void setEliminationOrder(storm::settings::modules::EliminationSettings::EliminationOrder const& order);
            
            storm::settings::modules::EliminationSettings::EliminationOrder getEliminationOrder() const;

        private:
            storm::settings::modules::EliminationSettings::EliminationOrder order;
        };
        
        /*!
         * A class that uses gaussian elimination to implement the LinearEquationSolver interface.
         */
        template<typename ValueType>
        class EliminationLinearEquationSolver : public LinearEquationSolver<ValueType> {
        public:
            EliminationLinearEquationSolver(EliminationLinearEquationSolverSettings<ValueType> const& settings = EliminationLinearEquationSolverSettings<ValueType>());
            EliminationLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, EliminationLinearEquationSolverSettings<ValueType> const& settings = EliminationLinearEquationSolverSettings<ValueType>());
            EliminationLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, EliminationLinearEquationSolverSettings<ValueType> const& settings = EliminationLinearEquationSolverSettings<ValueType>());
            
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& A) override;
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& A) override;
            
            virtual bool solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;
            virtual void multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const override;

            EliminationLinearEquationSolverSettings<ValueType>& getSettings();
            EliminationLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
            virtual LinearEquationSolverProblemFormat getEquationProblemFormat() const override;
            
        private:
            void initializeSettings();
            
            virtual uint64_t getMatrixRowCount() const override;
            virtual uint64_t getMatrixColumnCount() const override;
            
            // If the solver takes posession of the matrix, we store the moved matrix in this member, so it gets deleted
            // when the solver is destructed.
            std::unique_ptr<storm::storage::SparseMatrix<ValueType>> localA;

            // A pointer to the original sparse matrix given to this solver. If the solver takes posession of the matrix
            // the pointer refers to localA.
            storm::storage::SparseMatrix<ValueType> const* A;
            
            // The settings used by the solver.
            EliminationLinearEquationSolverSettings<ValueType> settings;
        };
        
        template<typename ValueType>
        class EliminationLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
        public:
            using LinearEquationSolverFactory<ValueType>::create;
            
            virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create() const override;
            
            EliminationLinearEquationSolverSettings<ValueType>& getSettings();
            EliminationLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
            virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const override;

        private:
            EliminationLinearEquationSolverSettings<ValueType> settings;
        };
    }
}

#endif /* STORM_SOLVER_ELIMINATIONLINEAREQUATIONSOLVER_H_ */
