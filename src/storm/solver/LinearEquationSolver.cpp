#include "storm/solver/LinearEquationSolver.h"

#include "storm/solver/SolverSelectionOptions.h"

#include "storm/solver/GmmxxLinearEquationSolver.h"
#include "storm/solver/NativeLinearEquationSolver.h"
#include "storm/solver/EigenLinearEquationSolver.h"
#include "storm/solver/EliminationLinearEquationSolver.h"

#include "storm/utility/vector.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnmetRequirementException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        LinearEquationSolver<ValueType>::LinearEquationSolver() : cachingEnabled(false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            return this->internalSolveEquations(x, b);
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
            vectorHelper.reduceVector(dir, *cachedRowVector, result, rowGroupIndices, choices);
            
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
        LinearEquationSolverRequirements LinearEquationSolver<ValueType>::getRequirements() const {
            return LinearEquationSolverRequirements();
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
        bool LinearEquationSolver<ValueType>::hasLowerBound(BoundType const& type) const {
            if (type == BoundType::Any) {
                return static_cast<bool>(lowerBound) || static_cast<bool>(lowerBounds);
            } else if (type == BoundType::Global) {
                return static_cast<bool>(lowerBound);
            } else if (type == BoundType::Local) {
                return static_cast<bool>(lowerBounds);
            }
            return false;
        }
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::hasUpperBound(BoundType const& type) const {
            if (type == BoundType::Any) {
                return static_cast<bool>(upperBound) || static_cast<bool>(upperBounds);
            } else if (type == BoundType::Global) {
                return static_cast<bool>(upperBound);
            } else if (type == BoundType::Local) {
                return static_cast<bool>(upperBounds);
            }
            return false;
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
        ValueType const& LinearEquationSolver<ValueType>::getLowerBound() const {
            return lowerBound.get();
        }
        
        template<typename ValueType>
        ValueType const& LinearEquationSolver<ValueType>::getUpperBound() const {
            return upperBound.get();
        }
        
        template<typename ValueType>
        std::vector<ValueType> const& LinearEquationSolver<ValueType>::getLowerBounds() const {
            return lowerBounds.get();
        }
        
        template<typename ValueType>
        std::vector<ValueType> const& LinearEquationSolver<ValueType>::getUpperBounds() const {
            return upperBounds.get();
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::setLowerBounds(std::vector<ValueType> const& values) {
            lowerBounds = values;
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::setUpperBounds(std::vector<ValueType> const& values) {
            upperBounds = values;
        }

        template<typename ValueType>
        void LinearEquationSolver<ValueType>::setUpperBounds(std::vector<ValueType>&& values) {
            upperBounds = std::move(values);
        }

        template<typename ValueType>
        void LinearEquationSolver<ValueType>::setBounds(std::vector<ValueType> const& lower, std::vector<ValueType> const& upper) {
            setLowerBounds(lower);
            setUpperBounds(upper);
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::createLowerBoundsVector(std::vector<ValueType>& lowerBoundsVector) const {
            if (this->hasLowerBound(BoundType::Local)) {
                lowerBoundsVector = this->getLowerBounds();
            } else {
                STORM_LOG_THROW(this->hasLowerBound(BoundType::Global), storm::exceptions::UnmetRequirementException, "Cannot create lower bounds vector without lower bound.");
                for (auto& e : lowerBoundsVector) {
                    e = this->getLowerBound();
                }
            }
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::createUpperBoundsVector(std::unique_ptr<std::vector<ValueType>>& upperBoundsVector) const {
            if (!upperBoundsVector) {
                if (this->hasUpperBound(BoundType::Local)) {
                    upperBoundsVector = std::make_unique<std::vector<ValueType>>(this->getUpperBounds());
                } else {
                    upperBoundsVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount(), this->getUpperBound());
                }
            } else {
                if (this->hasUpperBound(BoundType::Local)) {
                    for (auto& e : *upperBoundsVector) {
                        e = this->getUpperBound();
                    }
                } else {
                    auto upperBoundsIt = this->getUpperBounds().begin();
                    for (auto& e : *upperBoundsVector) {
                        e = *upperBoundsIt;
                        ++upperBoundsIt;
                    }
                }
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolver<ValueType>> LinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            std::unique_ptr<LinearEquationSolver<ValueType>> solver = this->create();
            solver->setMatrix(matrix);
            return solver;
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolver<ValueType>> LinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            std::unique_ptr<LinearEquationSolver<ValueType>> solver = this->create();
            solver->setMatrix(std::move(matrix));
            return solver;
        }
        
        template<typename ValueType>
        LinearEquationSolverProblemFormat LinearEquationSolverFactory<ValueType>::getEquationProblemFormat() const {
            return this->create()->getEquationProblemFormat();
        }
        
        template<typename ValueType>
        LinearEquationSolverRequirements LinearEquationSolverFactory<ValueType>::getRequirements() const {
            return this->create()->getRequirements();
        }
        
        template<typename ValueType>
        GeneralLinearEquationSolverFactory<ValueType>::GeneralLinearEquationSolverFactory() {
            auto const& coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            auto const& generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            
            EquationSolverType actualEquationSolver = coreSettings.getEquationSolver();
            if (generalSettings.isSoundSet()) {
                if (coreSettings.isEquationSolverSetFromDefaultValue()) {
                    STORM_LOG_WARN_COND(actualEquationSolver == EquationSolverType::Native, "Switching to native equation solver to guarantee soundness. To select other solvers, please explicitly specify a solver.");
                } else {
                    STORM_LOG_WARN_COND(actualEquationSolver == EquationSolverType::Native, "Switching to native equation solver from explicitly selected solver '" << storm::solver::toString(actualEquationSolver) << "' to guarantee soundness.");
                }
                actualEquationSolver = EquationSolverType::Native;
            }
            
            setEquationSolverType(actualEquationSolver);
        }
        
        template<typename ValueType>
        GeneralLinearEquationSolverFactory<ValueType>::GeneralLinearEquationSolverFactory(EquationSolverType const& equationSolver) {
            setEquationSolverType(equationSolver);
        }
        
        template<typename ValueType>
        void GeneralLinearEquationSolverFactory<ValueType>::setEquationSolverType(EquationSolverType const& equationSolver) {
            this->equationSolver = equationSolver;
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolver<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::create() const {
            switch (equationSolver) {
                case EquationSolverType::Gmmxx: return std::make_unique<GmmxxLinearEquationSolver<ValueType>>();
                case EquationSolverType::Native: return std::make_unique<NativeLinearEquationSolver<ValueType>>();
                case EquationSolverType::Eigen: return std::make_unique<EigenLinearEquationSolver<ValueType>>();
                case EquationSolverType::Elimination: return std::make_unique<EliminationLinearEquationSolver<ValueType>>();
                default: return std::make_unique<GmmxxLinearEquationSolver<ValueType>>();
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(*this);
        }
        
#ifdef STORM_HAVE_CARL
        std::unique_ptr<LinearEquationSolver<storm::RationalNumber>> GeneralLinearEquationSolverFactory<storm::RationalNumber>::create() const {
            EquationSolverType equationSolver = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver();
            switch (equationSolver) {
                case EquationSolverType::Elimination: return std::make_unique<EliminationLinearEquationSolver<storm::RationalNumber>>();
                default: return std::make_unique<EigenLinearEquationSolver<storm::RationalNumber>>();
            }
        }
        
        std::unique_ptr<LinearEquationSolverFactory<storm::RationalNumber>> GeneralLinearEquationSolverFactory<storm::RationalNumber>::clone() const {
            return std::make_unique<GeneralLinearEquationSolverFactory<storm::RationalNumber>>(*this);
        }
        
        std::unique_ptr<LinearEquationSolver<storm::RationalFunction>> GeneralLinearEquationSolverFactory<storm::RationalFunction>::create() const {
            EquationSolverType equationSolver = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver();
            switch (equationSolver) {
                case EquationSolverType::Elimination: return std::make_unique<EliminationLinearEquationSolver<storm::RationalFunction>>();
                default: return std::make_unique<EigenLinearEquationSolver<storm::RationalFunction>>();
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
