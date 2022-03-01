#ifndef STORM_SOLVER_NATIVELINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_NATIVELINEAREQUATIONSOLVER_H_

#include <ostream>

#include "storm/solver/LinearEquationSolver.h"

#include "storm/solver/SolverSelectionOptions.h"
#include "storm/solver/SolverStatus.h"
#include "storm/solver/helper/OptimisticValueIterationHelper.h"
#include "storm/solver/helper/SoundValueIterationHelper.h"
#include "storm/solver/multiplier/NativeMultiplier.h"

#include "storm/utility/NumberTraits.h"

namespace storm {

class Environment;

namespace solver {

/*!
 * A class that uses storm's native matrix operations to implement the LinearEquationSolver interface.
 */
template<typename ValueType>
class NativeLinearEquationSolver : public LinearEquationSolver<ValueType> {
   public:
    NativeLinearEquationSolver();
    NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A);
    NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A);

    virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& A) override;
    virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& A) override;

    virtual LinearEquationSolverProblemFormat getEquationProblemFormat(storm::Environment const& env) const override;
    virtual LinearEquationSolverRequirements getRequirements(Environment const& env) const override;

    virtual void clearCache() const override;

   protected:
    virtual bool internalSolveEquations(storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;

   private:
    struct PowerIterationResult {
        PowerIterationResult(uint64_t iterations, SolverStatus status) : iterations(iterations), status(status) {
            // Intentionally left empty.
        }

        uint64_t iterations;
        SolverStatus status;
    };

    template<typename ValueTypePrime>
    friend class NativeLinearEquationSolver;

    PowerIterationResult performPowerIteration(Environment const& env, std::vector<ValueType>*& currentX, std::vector<ValueType>*& newX,
                                               std::vector<ValueType> const& b, ValueType const& precision, bool relative, SolverGuarantee const& guarantee,
                                               uint64_t currentIterations, uint64_t maxIterations,
                                               storm::solver::MultiplicationStyle const& multiplicationStyle) const;

    void logIterations(bool converged, bool terminate, uint64_t iterations) const;

    virtual uint64_t getMatrixRowCount() const override;
    virtual uint64_t getMatrixColumnCount() const override;

    NativeLinearEquationSolverMethod getMethod(Environment const& env, bool isExactMode) const;

    virtual bool solveEquationsSOR(storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b, ValueType const& omega) const;
    virtual bool solveEquationsJacobi(storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
    virtual bool solveEquationsWalkerChae(storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
    virtual bool solveEquationsPower(storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
    virtual bool solveEquationsSoundValueIteration(storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
    virtual bool solveEquationsOptimisticValueIteration(storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
    virtual bool solveEquationsIntervalIteration(storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
    virtual bool solveEquationsRationalSearch(storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;

    template<typename RationalType, typename ImpreciseType>
    bool solveEquationsRationalSearchHelper(storm::Environment const& env, NativeLinearEquationSolver<ImpreciseType> const& impreciseSolver,
                                            storm::storage::SparseMatrix<RationalType> const& rationalA, std::vector<RationalType>& rationalX,
                                            std::vector<RationalType> const& rationalB, storm::storage::SparseMatrix<ImpreciseType> const& A,
                                            std::vector<ImpreciseType>& x, std::vector<ImpreciseType> const& b, std::vector<ImpreciseType>& tmpX) const;
    template<typename ImpreciseType>
    typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && !NumberTraits<ValueType>::IsExact, bool>::type solveEquationsRationalSearchHelper(
        storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
    template<typename ImpreciseType>
    typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && NumberTraits<ValueType>::IsExact, bool>::type solveEquationsRationalSearchHelper(
        storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
    template<typename ImpreciseType>
    typename std::enable_if<!std::is_same<ValueType, ImpreciseType>::value, bool>::type solveEquationsRationalSearchHelper(
        storm::Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
    template<typename RationalType, typename ImpreciseType>
    static bool sharpen(uint64_t precision, storm::storage::SparseMatrix<RationalType> const& A, std::vector<ImpreciseType> const& x,
                        std::vector<RationalType> const& b, std::vector<RationalType>& tmp);
    static bool isSolution(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& values, std::vector<ValueType> const& b);

    // If the solver takes posession of the matrix, we store the moved matrix in this member, so it gets deleted
    // when the solver is destructed.
    std::unique_ptr<storm::storage::SparseMatrix<ValueType>> localA;

    // A pointer to the original sparse matrix given to this solver. If the solver takes posession of the matrix
    // the pointer refers to localA.
    storm::storage::SparseMatrix<ValueType> const* A;

    // An object to dispatch all multiplication operations.
    mutable std::unique_ptr<Multiplier<ValueType>> multiplier;

    // cached auxiliary data
    mutable std::unique_ptr<std::vector<ValueType>> cachedRowVector2;  // A.getRowCount() rows
    mutable std::unique_ptr<storm::solver::helper::SoundValueIterationHelper<ValueType>> soundValueIterationHelper;
    mutable std::unique_ptr<storm::solver::helper::OptimisticValueIterationHelper<ValueType>> optimisticValueIterationHelper;

    struct JacobiDecomposition {
        JacobiDecomposition(Environment const& env, storm::storage::SparseMatrix<ValueType> const& A);

        storm::storage::SparseMatrix<ValueType> LUMatrix;
        std::vector<ValueType> DVector;
        std::unique_ptr<storm::solver::Multiplier<ValueType>> multiplier;
    };
    mutable std::unique_ptr<JacobiDecomposition> jacobiDecomposition;

    struct WalkerChaeData {
        WalkerChaeData(Environment const& env, storm::storage::SparseMatrix<ValueType> const& originalMatrix, std::vector<ValueType> const& originalB);

        void computeWalkerChaeMatrix(storm::storage::SparseMatrix<ValueType> const& originalMatrix);
        void computeNewB(std::vector<ValueType> const& originalB);
        void precomputeAuxiliaryData();

        storm::storage::SparseMatrix<ValueType> matrix;
        std::vector<ValueType> b;
        ValueType t;
        std::unique_ptr<storm::solver::Multiplier<ValueType>> multiplier;

        // Auxiliary data.
        std::vector<ValueType> columnSums;
        std::vector<ValueType> newX;
    };
    mutable std::unique_ptr<WalkerChaeData> walkerChaeData;
};

template<typename ValueType>
class NativeLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
   public:
    using LinearEquationSolverFactory<ValueType>::create;

    virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(Environment const& env) const override;

    virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const override;
};
}  // namespace solver
}  // namespace storm

#endif /* STORM_SOLVER_NATIVELINEAREQUATIONSOLVER_H_ */
