#ifndef STORM_SOLVER_NATIVELINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_NATIVELINEAREQUATIONSOLVER_H_

#include <ostream>

#include "storm/solver/LinearEquationSolver.h"

#include "storm/solver/NativeMultiplier.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class NativeLinearEquationSolverSettings {
        public:
            enum class SolutionMethod {
                Jacobi, GaussSeidel, SOR, WalkerChae, Power
            };

            NativeLinearEquationSolverSettings();
            
            void setSolutionMethod(SolutionMethod const& method);
            void setPrecision(ValueType precision);
            void setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations);
            void setRelativeTerminationCriterion(bool value);
            void setOmega(ValueType omega);
            
            SolutionMethod getSolutionMethod() const;
            ValueType getPrecision() const;
            uint64_t getMaximalNumberOfIterations() const;
            uint64_t getRelativeTerminationCriterion() const;
            ValueType getOmega() const;
            
        private:
            SolutionMethod method;
            double precision;
            bool relative;
            uint_fast64_t maximalNumberOfIterations;
            ValueType omega;
        };
        
        /*!
         * A class that uses storm's native matrix operations to implement the LinearEquationSolver interface.
         */
        template<typename ValueType>
        class NativeLinearEquationSolver : public LinearEquationSolver<ValueType> {
        public:
            NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, NativeLinearEquationSolverSettings<ValueType> const& settings = NativeLinearEquationSolverSettings<ValueType>());
            NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, NativeLinearEquationSolverSettings<ValueType> const& settings = NativeLinearEquationSolverSettings<ValueType>());
            
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& A) override;
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& A) override;
            
            virtual bool solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;
            virtual void multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const override;
            virtual void multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices = nullptr) const override;
            virtual bool supportsGaussSeidelMultiplication() const override;
            virtual void multiplyGaussSeidel(std::vector<ValueType>& x, std::vector<ValueType> const* b) const override;
            virtual void multiplyAndReduceGaussSeidel(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices = nullptr) const override;
            
            void setSettings(NativeLinearEquationSolverSettings<ValueType> const& newSettings);
            NativeLinearEquationSolverSettings<ValueType> const& getSettings() const;

            virtual void clearCache() const override;

        private:
            virtual uint64_t getMatrixRowCount() const override;
            virtual uint64_t getMatrixColumnCount() const override;

            virtual bool solveEquationsSOR(std::vector<ValueType>& x, std::vector<ValueType> const& b, ValueType const& omega) const;
            virtual bool solveEquationsJacobi(std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            virtual bool solveEquationsWalkerChae(std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            virtual bool solveEquationsPower(std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            
            // If the solver takes posession of the matrix, we store the moved matrix in this member, so it gets deleted
            // when the solver is destructed.
            std::unique_ptr<storm::storage::SparseMatrix<ValueType>> localA;
            
            // A pointer to the original sparse matrix given to this solver. If the solver takes posession of the matrix
            // the pointer refers to localA.
            storm::storage::SparseMatrix<ValueType> const* A;
            
            // The settings used by the solver.
            NativeLinearEquationSolverSettings<ValueType> settings;

            // An object to dispatch all multiplication operations.
            NativeMultiplier<ValueType> multiplier;

            // cached auxiliary data
            mutable std::unique_ptr<std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>>> jacobiDecomposition;
            
            struct WalkerChaeData {
                WalkerChaeData(storm::storage::SparseMatrix<ValueType> const& originalMatrix, std::vector<ValueType> const& originalB);
                
                void computeWalkerChaeMatrix(storm::storage::SparseMatrix<ValueType> const& originalMatrix);
                void computeNewB(std::vector<ValueType> const& originalB);
                void precomputeAuxiliaryData();
                
                storm::storage::SparseMatrix<ValueType> matrix;
                std::vector<ValueType> b;
                ValueType t;

                // Auxiliary data.
                std::vector<ValueType> columnSums;
                std::vector<ValueType> newX;
            };
            mutable std::unique_ptr<WalkerChaeData> walkerChaeData;
        };
        
        template<typename ValueType>
        class NativeLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
        public:
            virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType>&& matrix) const override;
            
            NativeLinearEquationSolverSettings<ValueType>& getSettings();
            NativeLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
            virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const override;

        private:
            NativeLinearEquationSolverSettings<ValueType> settings;
        };
    }
}

#endif /* STORM_SOLVER_NATIVELINEAREQUATIONSOLVER_H_ */
