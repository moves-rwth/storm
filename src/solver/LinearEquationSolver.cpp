#include "src/solver/LinearEquationSolver.h"

#include "src/solver/SolverSelectionOptions.h"

#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/EigenLinearEquationSolver.h"
#include "src/solver/EliminationLinearEquationSolver.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/CoreSettings.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        LinearEquationSolver<ValueType>::LinearEquationSolver() : auxiliaryRepeatedMultiplyStorage(nullptr) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::repeatedMultiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const {
            bool allocatedAuxStorage = !this->hasAuxStorage(LinearEquationSolverOperation::MultiplyRepeatedly);
            if (allocatedAuxStorage) {
                auxiliaryRepeatedMultiplyStorage = std::make_unique<std::vector<ValueType>>(x.size());
            }
            
            // Set up some temporary variables so that we can just swap pointers instead of copying the result after
            // each iteration.
            std::vector<ValueType>* currentX = &x;
            std::vector<ValueType>* nextX = auxiliaryRepeatedMultiplyStorage.get();
            
            // Now perform matrix-vector multiplication as long as we meet the bound.
            for (uint_fast64_t i = 0; i < n; ++i) {
                this->multiply(*currentX, b, *nextX);
                std::swap(nextX, currentX);
            }
            
            // If we performed an odd number of repetitions, we need to swap the contents of currentVector and x,
            // because the output is supposed to be stored in the input vector x.
            if (currentX == auxiliaryRepeatedMultiplyStorage.get()) {
                std::swap(x, *currentX);
            }

            // If we allocated auxiliary storage, we need to dispose of it now.
            if (allocatedAuxStorage) {
                auxiliaryRepeatedMultiplyStorage.reset();
            }
        }
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::allocateAuxStorage(LinearEquationSolverOperation operation) {
            if (this->hasAuxStorage(operation)) {
                return false;
            }
            
            auxiliaryRepeatedMultiplyStorage = std::make_unique<std::vector<ValueType>>(this->getMatrixColumnCount());
            return true;
        }
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::deallocateAuxStorage(LinearEquationSolverOperation operation) {
            if (operation == LinearEquationSolverOperation::MultiplyRepeatedly) {
                bool result = auxiliaryRepeatedMultiplyStorage != nullptr;
                if (result) {
                    auxiliaryRepeatedMultiplyStorage.reset();
                }
                return result;
            }
            return false;
        }
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::reallocateAuxStorage(LinearEquationSolverOperation operation) {
            bool result = false;
            if (operation == LinearEquationSolverOperation::MultiplyRepeatedly) {
                if (auxiliaryRepeatedMultiplyStorage != nullptr) {
                    result = auxiliaryRepeatedMultiplyStorage->size() != this->getMatrixColumnCount();
                    auxiliaryRepeatedMultiplyStorage->resize(this->getMatrixColumnCount());
                }
            }
            return result;
        }
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::hasAuxStorage(LinearEquationSolverOperation operation) const {
            if (operation == LinearEquationSolverOperation::MultiplyRepeatedly) {
                return auxiliaryRepeatedMultiplyStorage != nullptr;
            }
            return false;
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

        template class LinearEquationSolver<double>;
        template class LinearEquationSolver<storm::RationalNumber>;
        template class LinearEquationSolver<storm::RationalFunction>;

        template class LinearEquationSolverFactory<double>;
        template class LinearEquationSolverFactory<storm::RationalNumber>;
        template class LinearEquationSolverFactory<storm::RationalFunction>;
        
        template class GeneralLinearEquationSolverFactory<double>;
        template class GeneralLinearEquationSolverFactory<storm::RationalNumber>;
        template class GeneralLinearEquationSolverFactory<storm::RationalFunction>;

    }
}