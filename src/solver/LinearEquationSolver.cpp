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
        void LinearEquationSolver<ValueType>::performMatrixVectorMultiplication(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            
            // Set up some temporary variables so that we can just swap pointers instead of copying the result after
            // each iteration.
            std::vector<ValueType>* currentX = &x;
            
            bool multiplyResultProvided = true;
            std::vector<ValueType>* nextX = multiplyResult;
            if (nextX == nullptr) {
                nextX = new std::vector<ValueType>(x.size());
                multiplyResultProvided = false;
            }
            std::vector<ValueType> const* copyX = nextX;
            
            // Now perform matrix-vector multiplication as long as we meet the bound.
            for (uint_fast64_t i = 0; i < n; ++i) {
                this->performMatrixVectorMultiplication(*currentX, *nextX, b);
                std::swap(nextX, currentX);
            }
            
            // If we performed an odd number of repetitions, we need to swap the contents of currentVector and x,
            // because the output is supposed to be stored in the input vector x.
            if (currentX == copyX) {
                std::swap(x, *currentX);
            }
            
            // If the vector for the temporary multiplication result was not provided, we need to delete it.
            if (!multiplyResultProvided) {
                delete copyX;
            }
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