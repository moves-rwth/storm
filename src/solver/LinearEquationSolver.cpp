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
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> LinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return create(matrix);
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            return selectSolver(matrix);
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return selectSolver(std::move(matrix));
        }
        
        template<typename ValueType>
        template<typename MatrixType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::selectSolver(MatrixType&& matrix) const {
            storm::solver::EquationSolverType equationSolver = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver();
            switch (equationSolver) {
                case storm::solver::EquationSolverType::Gmmxx: return std::make_unique<storm::solver::GmmxxLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
                case storm::solver::EquationSolverType::Native: return std::make_unique<storm::solver::NativeLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
                case storm::solver::EquationSolverType::Eigen: return std::make_unique<storm::solver::EigenLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
                case storm::solver::EquationSolverType::Elimination: return std::make_unique<storm::solver::EliminationLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
                default: return std::make_unique<storm::solver::GmmxxLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
            }
        }
        
        std::unique_ptr<storm::solver::LinearEquationSolver<storm::RationalNumber>> GeneralLinearEquationSolverFactory<storm::RationalNumber>::create(storm::storage::SparseMatrix<storm::RationalNumber> const& matrix) const {
            return selectSolver(matrix);
        }
        
        std::unique_ptr<storm::solver::LinearEquationSolver<storm::RationalNumber>> GeneralLinearEquationSolverFactory<storm::RationalNumber>::create(storm::storage::SparseMatrix<storm::RationalNumber>&& matrix) const {
            return selectSolver(std::move(matrix));
        }
        
        template<typename MatrixType>
        std::unique_ptr<storm::solver::LinearEquationSolver<storm::RationalNumber>> GeneralLinearEquationSolverFactory<storm::RationalNumber>::selectSolver(MatrixType&& matrix) const {
            storm::solver::EquationSolverType equationSolver = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver();
            switch (equationSolver) {
                case storm::solver::EquationSolverType::Elimination: return std::make_unique<storm::solver::EliminationLinearEquationSolver<storm::RationalNumber>>(matrix);
                default: return std::make_unique<storm::solver::EigenLinearEquationSolver<storm::RationalNumber>>(matrix);
            }
        }
        
        std::unique_ptr<storm::solver::LinearEquationSolver<storm::RationalFunction>> GeneralLinearEquationSolverFactory<storm::RationalFunction>::create(storm::storage::SparseMatrix<storm::RationalFunction> const& matrix) const {
            return selectSolver(matrix);
        }
        
        std::unique_ptr<storm::solver::LinearEquationSolver<storm::RationalFunction>> GeneralLinearEquationSolverFactory<storm::RationalFunction>::create(storm::storage::SparseMatrix<storm::RationalFunction>&& matrix) const {
            return selectSolver(std::move(matrix));
        }
        
        template<typename MatrixType>
        std::unique_ptr<storm::solver::LinearEquationSolver<storm::RationalFunction>> GeneralLinearEquationSolverFactory<storm::RationalFunction>::selectSolver(MatrixType&& matrix) const {
            storm::solver::EquationSolverType equationSolver = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver();
            switch (equationSolver) {
                case storm::solver::EquationSolverType::Elimination: return std::make_unique<storm::solver::EliminationLinearEquationSolver<storm::RationalFunction>>(matrix);
                default: return std::make_unique<storm::solver::EigenLinearEquationSolver<storm::RationalFunction>>(matrix);
            }
        }
        
        template class LinearEquationSolverFactory<double>;
        template class LinearEquationSolverFactory<storm::RationalNumber>;
        template class LinearEquationSolverFactory<storm::RationalFunction>;
        
        template class GeneralLinearEquationSolverFactory<double>;
        template class GeneralLinearEquationSolverFactory<storm::RationalNumber>;
        template class GeneralLinearEquationSolverFactory<storm::RationalFunction>;

    }
}