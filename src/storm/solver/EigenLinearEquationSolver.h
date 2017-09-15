#pragma once

#include "storm/solver/LinearEquationSolver.h"

#include "storm/utility/eigen.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class EigenLinearEquationSolverSettings {
        public:
            enum class SolutionMethod {
                SparseLU, BiCGSTAB, DGMRES, GMRES
            };
            
            enum class Preconditioner {
                Ilu, Diagonal, None
            };
            
            EigenLinearEquationSolverSettings();
            
            void setSolutionMethod(SolutionMethod const& method);
            void setPreconditioner(Preconditioner const& preconditioner);
            void setPrecision(ValueType precision);
            void setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations);
            void setNumberOfIterationsUntilRestart(uint64_t restart);
            void setForceSoundness(bool value);
            
            SolutionMethod getSolutionMethod() const;
            Preconditioner getPreconditioner() const;
            ValueType getPrecision() const;
            uint64_t getMaximalNumberOfIterations() const;
            uint64_t getNumberOfIterationsUntilRestart() const;
            bool getForceSoundness() const;
            
        private:
            bool forceSoundness;
            SolutionMethod method;
            Preconditioner preconditioner;
            double precision;
            uint64_t maximalNumberOfIterations;
            uint_fast64_t restart;
        };
        
#ifdef STORM_HAVE_CARL
        template<>
        class EigenLinearEquationSolverSettings<storm::RationalNumber> {
        public:
            EigenLinearEquationSolverSettings();
        };

        template<>
        class EigenLinearEquationSolverSettings<storm::RationalFunction> {
        public:
            EigenLinearEquationSolverSettings();
        };
#endif
        
        /*!
         * A class that uses the Eigen library to implement the LinearEquationSolver interface.
         */
        template<typename ValueType>
        class EigenLinearEquationSolver : public LinearEquationSolver<ValueType> {
        public:
            EigenLinearEquationSolver(EigenLinearEquationSolverSettings<ValueType> const& settings = EigenLinearEquationSolverSettings<ValueType>());
            EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, EigenLinearEquationSolverSettings<ValueType> const& settings = EigenLinearEquationSolverSettings<ValueType>());
            EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, EigenLinearEquationSolverSettings<ValueType> const& settings = EigenLinearEquationSolverSettings<ValueType>());
            
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& A) override;
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& A) override;
            
            virtual void multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const override;

            EigenLinearEquationSolverSettings<ValueType>& getSettings();
            EigenLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
            virtual LinearEquationSolverProblemFormat getEquationProblemFormat() const override;

        protected:
            virtual bool internalSolveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;

        private:
            virtual uint64_t getMatrixRowCount() const override;
            virtual uint64_t getMatrixColumnCount() const override;
            
            // The (eigen) matrix associated with this equation solver.
            std::unique_ptr<StormEigen::SparseMatrix<ValueType>> eigenA;

            // The settings used by the solver.
            EigenLinearEquationSolverSettings<ValueType> settings;
        };
        
        template<typename ValueType>
        class EigenLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
        public:
            using LinearEquationSolverFactory<ValueType>::create;

            virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create() const override;
            
            EigenLinearEquationSolverSettings<ValueType>& getSettings();
            EigenLinearEquationSolverSettings<ValueType> const& getSettings() const;

            virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const override;

        private:
            EigenLinearEquationSolverSettings<ValueType> settings;
        };
    }
}
