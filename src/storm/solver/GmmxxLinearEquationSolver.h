#ifndef STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_

#include <ostream>

#include "storm/utility/gmm.h"

#include "storm/solver/GmmxxMultiplier.h"

#include "storm/solver/LinearEquationSolver.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        class GmmxxLinearEquationSolverSettings {
        public:
            // An enumeration specifying the available preconditioners.
            enum class Preconditioner {
                Ilu, Diagonal, None
            };
            
            friend std::ostream& operator<<(std::ostream& out, Preconditioner const& preconditioner) {
                switch (preconditioner) {
                    case GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu: out << "ilu"; break;
                    case GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal: out << "diagonal"; break;
                    case GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::None: out << "none"; break;
                }
                return out;
            }

            // An enumeration specifying the available solution methods.
            enum class SolutionMethod {
                Bicgstab, Qmr, Gmres
            };
            
            friend std::ostream& operator<<(std::ostream& out, SolutionMethod const& method) {
                switch (method) {
                    case GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Bicgstab: out << "BiCGSTAB"; break;
                    case GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Qmr: out << "QMR"; break;
                    case GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Gmres: out << "GMRES"; break;
                }
                return out;
            }

            GmmxxLinearEquationSolverSettings();
            
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
            // Whether or not we are forced to be sound.
            bool forceSoundness;
            
            // The method to use for solving linear equation systems.
            SolutionMethod method;
            
            // The required precision for the iterative methods.
            double precision;
            
            // The maximal number of iterations to do before iteration is aborted.
            uint_fast64_t maximalNumberOfIterations;
            
            // The preconditioner to use when solving the linear equation system.
            Preconditioner preconditioner;
            
            // A restart value that determines when restarted methods shall do so.
            uint_fast64_t restart;
        };
        
        /*!
         * A class that uses the gmm++ library to implement the LinearEquationSolver interface.
         */
        template<typename ValueType>
        class GmmxxLinearEquationSolver : public LinearEquationSolver<ValueType> {
        public:
            GmmxxLinearEquationSolver(GmmxxLinearEquationSolverSettings<ValueType> const& settings = GmmxxLinearEquationSolverSettings<ValueType>());
            GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, GmmxxLinearEquationSolverSettings<ValueType> const& settings = GmmxxLinearEquationSolverSettings<ValueType>());
            GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, GmmxxLinearEquationSolverSettings<ValueType> const& settings = GmmxxLinearEquationSolverSettings<ValueType>());
            
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& A) override;
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& A) override;
            
            virtual void multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const override;
            virtual void multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices = nullptr) const override;
            virtual bool supportsGaussSeidelMultiplication() const override;
            virtual void multiplyGaussSeidel(std::vector<ValueType>& x, std::vector<ValueType> const* b) const override;
            virtual void multiplyAndReduceGaussSeidel(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices = nullptr) const override;

            void setSettings(GmmxxLinearEquationSolverSettings<ValueType> const& newSettings);
            virtual void setPrecision(ValueType const& precision) override;

            GmmxxLinearEquationSolverSettings<ValueType> const& getSettings() const;

            virtual LinearEquationSolverProblemFormat getEquationProblemFormat() const override;
            
            virtual void clearCache() const override;

        protected:
            virtual bool internalSolveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;
            
        private:
            virtual uint64_t getMatrixRowCount() const override;
            virtual uint64_t getMatrixColumnCount() const override;

            // The matrix in gmm++ format.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmxxA;
            
            // The settings used by the solver.
            GmmxxLinearEquationSolverSettings<ValueType> settings;
            
            // A multiplier object used to dispatch the multiplication calls.
            GmmxxMultiplier<ValueType> multiplier;
            
            // cached data obtained during solving
            mutable std::unique_ptr<gmm::ilu_precond<gmm::csr_matrix<ValueType>>> iluPreconditioner;
            mutable std::unique_ptr<gmm::diagonal_precond<gmm::csr_matrix<ValueType>>> diagonalPreconditioner;
        };
        
        template<typename ValueType>
        class GmmxxLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
        public:
            using LinearEquationSolverFactory<ValueType>::create;
            
            virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create() const override;
            
            GmmxxLinearEquationSolverSettings<ValueType>& getSettings();
            GmmxxLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
            virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const override;

        private:
            GmmxxLinearEquationSolverSettings<ValueType> settings;
        };

    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_ */
