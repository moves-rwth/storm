#include "storm/solver/LinearEquationSolver.h"

#include "storm/solver/SolverSelectionOptions.h"

#include "storm/solver/GmmxxLinearEquationSolver.h"
#include "storm/solver/NativeLinearEquationSolver.h"
#include "storm/solver/EigenLinearEquationSolver.h"
#include "storm/solver/EliminationLinearEquationSolver.h"

#include "storm/utility/vector.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        LinearEquationSolver<ValueType>::LinearEquationSolver() : cachingEnabled(false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::repeatedMultiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const {
            if (!cachedRowVector) {
                cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            // We enable caching for this. But remember how the old setting was
            bool cachingWasEnabled = isCachingEnabled();
            setCachingEnabled(true);
            
            // Set up some temporary variables so that we can just swap pointers instead of copying the result after
            // each iteration.
            std::vector<ValueType>* currentX = &x;
            std::vector<ValueType>* nextX = cachedRowVector.get();
            
            // Now perform matrix-vector multiplication as long as we meet the bound.
            for (uint_fast64_t i = 0; i < n; ++i) {
                this->multiply(*currentX, b, *nextX);
                std::swap(nextX, currentX);
            }
            
            // If we performed an odd number of repetitions, we need to swap the contents of currentVector and x,
            // because the output is supposed to be stored in the input vector x.
            if (currentX == cachedRowVector.get()) {
                std::swap(x, *currentX);
            }
            
            // restore the old caching setting
            setCachingEnabled(cachingWasEnabled);
            
            if (!isCachingEnabled()) {
                clearCache();
            }
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices) const {
            if (!cachedRowVector) {
                cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            // We enable caching for this. But remember how the old setting was
            bool cachingWasEnabled = isCachingEnabled();
            setCachingEnabled(true);

            this->multiply(x, b, *cachedRowVector);
            storm::utility::vector::reduceVectorMinOrMax(dir, *cachedRowVector, result, rowGroupIndices, choices);
            
            // restore the old caching setting
            setCachingEnabled(cachingWasEnabled);
            
            if (!isCachingEnabled()) {
                clearCache();
            }
        }
        
#ifdef STORM_HAVE_CARL
        template<>
        void LinearEquationSolver<storm::RationalFunction>::multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<storm::RationalFunction>& x, std::vector<storm::RationalFunction> const* b, std::vector<storm::RationalFunction>& result, std::vector<uint_fast64_t>* choices ) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Reducing rational function vector is not supported.");
        }
#endif
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::supportsGaussSeidelMultiplication() const {
            return false;
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::multiplyGaussSeidel(std::vector<ValueType>& x, std::vector<ValueType> const* b) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support the function 'multiplyGaussSeidel'.");
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::multiplyAndReduceGaussSeidel(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support the function 'multiplyAndReduceGaussSeidel'.");
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::setCachingEnabled(bool value) const {
            if(cachingEnabled && !value) {
                // caching will be turned off. Hence we clear the cache at this point
                clearCache();
            }
            cachingEnabled = value;
        }
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::isCachingEnabled() const {
            return cachingEnabled;
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::clearCache() const {
            cachedRowVector.reset();
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::setLowerBound(ValueType const& value) {
            lowerBound = value;
        }

        template<typename ValueType>
        void LinearEquationSolver<ValueType>::setUpperBound(ValueType const& value) {
            upperBound = value;
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::setBounds(ValueType const& lower, ValueType const& upper) {
            setLowerBound(lower);
            setUpperBound(upper);
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolver<ValueType>> LinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return create(matrix);
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolver<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            return selectSolver(matrix);
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolver<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return selectSolver(std::move(matrix));
        }
        
        template<typename ValueType>
        template<typename MatrixType>
        std::unique_ptr<LinearEquationSolver<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::selectSolver(MatrixType&& matrix) const {
            EquationSolverType equationSolver = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver();
            switch (equationSolver) {
                case EquationSolverType::Gmmxx: return std::make_unique<GmmxxLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
                case EquationSolverType::Native: return std::make_unique<NativeLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
                case EquationSolverType::Eigen: return std::make_unique<EigenLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
                case EquationSolverType::Elimination: return std::make_unique<EliminationLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
                default: return std::make_unique<GmmxxLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(*this);
        }
        
#ifdef STORM_HAVE_CARL
        std::unique_ptr<LinearEquationSolver<storm::RationalNumber>> GeneralLinearEquationSolverFactory<storm::RationalNumber>::create(storm::storage::SparseMatrix<storm::RationalNumber> const& matrix) const {
            return selectSolver(matrix);
        }
        
        std::unique_ptr<LinearEquationSolver<storm::RationalNumber>> GeneralLinearEquationSolverFactory<storm::RationalNumber>::create(storm::storage::SparseMatrix<storm::RationalNumber>&& matrix) const {
            return selectSolver(std::move(matrix));
        }
        
        template<typename MatrixType>
        std::unique_ptr<LinearEquationSolver<storm::RationalNumber>> GeneralLinearEquationSolverFactory<storm::RationalNumber>::selectSolver(MatrixType&& matrix) const {
            EquationSolverType equationSolver = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver();
            switch (equationSolver) {
                case EquationSolverType::Elimination: return std::make_unique<EliminationLinearEquationSolver<storm::RationalNumber>>(std::forward<MatrixType>(matrix));
                default: return std::make_unique<EigenLinearEquationSolver<storm::RationalNumber>>(std::forward<MatrixType>(matrix));
            }
        }
        
        std::unique_ptr<LinearEquationSolverFactory<storm::RationalNumber>> GeneralLinearEquationSolverFactory<storm::RationalNumber>::clone() const {
            return std::make_unique<GeneralLinearEquationSolverFactory<storm::RationalNumber>>(*this);
        }
        
        std::unique_ptr<LinearEquationSolver<storm::RationalFunction>> GeneralLinearEquationSolverFactory<storm::RationalFunction>::create(storm::storage::SparseMatrix<storm::RationalFunction> const& matrix) const {
            return selectSolver(matrix);
        }
        
        std::unique_ptr<LinearEquationSolver<storm::RationalFunction>> GeneralLinearEquationSolverFactory<storm::RationalFunction>::create(storm::storage::SparseMatrix<storm::RationalFunction>&& matrix) const {
            return selectSolver(std::move(matrix));
        }
        
        template<typename MatrixType>
        std::unique_ptr<LinearEquationSolver<storm::RationalFunction>> GeneralLinearEquationSolverFactory<storm::RationalFunction>::selectSolver(MatrixType&& matrix) const {
            EquationSolverType equationSolver = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver();
            switch (equationSolver) {
                case EquationSolverType::Elimination: return std::make_unique<EliminationLinearEquationSolver<storm::RationalFunction>>(std::forward<MatrixType>(matrix));
                default: return std::make_unique<EigenLinearEquationSolver<storm::RationalFunction>>(std::forward<MatrixType>(matrix));
            }
        }
        
        std::unique_ptr<LinearEquationSolverFactory<storm::RationalFunction>> GeneralLinearEquationSolverFactory<storm::RationalFunction>::clone() const {
            return std::make_unique<GeneralLinearEquationSolverFactory<storm::RationalFunction>>(*this);
        }
#endif

        template class LinearEquationSolver<double>;
        template class LinearEquationSolverFactory<double>;
        template class GeneralLinearEquationSolverFactory<double>;

#ifdef STORM_HAVE_CARL
        template class LinearEquationSolver<storm::RationalNumber>;
        template class LinearEquationSolver<storm::RationalFunction>;

        template class LinearEquationSolverFactory<storm::RationalNumber>;
        template class LinearEquationSolverFactory<storm::RationalFunction>;
        
        template class GeneralLinearEquationSolverFactory<storm::RationalNumber>;
        template class GeneralLinearEquationSolverFactory<storm::RationalFunction>;
#endif

    }
}
