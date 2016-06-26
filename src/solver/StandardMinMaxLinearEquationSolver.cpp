#include "src/solver/StandardMinMaxLinearEquationSolver.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/CoreSettings.h"

#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/solver/EigenLinearEquationSolver.h"
#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/EliminationLinearEquationSolver.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace solver {
        
        StandardMinMaxLinearEquationSolverSettings::StandardMinMaxLinearEquationSolverSettings() {
            auto technique = storm::settings::getModule<storm::settings::modules::CoreSettings>().getMinMaxEquationSolvingTechnique();
            switch (technique) {
                case MinMaxTechnique::ValueIteration: this->solutionMethod = SolutionMethod::ValueIteration; break;
                case MinMaxTechnique::PolicyIteration: this->solutionMethod = SolutionMethod::PolicyIteration; break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
        }
        
        void StandardMinMaxLinearEquationSolverSettings::setSolutionMethod(SolutionMethod const& solutionMethod) {
            this->solutionMethod = solutionMethod;
        }
        
        StandardMinMaxLinearEquationSolverSettings::SolutionMethod const& StandardMinMaxLinearEquationSolverSettings::getSolutionMethod() const {
            return solutionMethod;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardMinMaxLinearEquationSolverSettings const& settings) : settings(settings), linearEquationSolverFactory(std::move(linearEquationSolverFactory)), localA(nullptr), A(A) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardMinMaxLinearEquationSolverSettings const& settings) : settings(settings), linearEquationSolverFactory(std::move(linearEquationSolverFactory)), localA(std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A))), A(*localA) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::solveEquationSystem(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
            //FIXME: implement
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            //FIXME: implement
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings const& StandardMinMaxLinearEquationSolver<ValueType>::getSettings() const {
            return settings;
        }

        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings& StandardMinMaxLinearEquationSolver<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(trackScheduler), linearEquationSolverFactory(nullptr) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(trackScheduler), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType, bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(trackScheduler) {
            switch (solverType) {
                case  EquationSolverType::Gmmxx: linearEquationSolverFactory = std::make_unique<GmmxxLinearEquationSolverFactory<ValueType>>(); break;
                case  EquationSolverType::Eigen: linearEquationSolverFactory = std::make_unique<EigenLinearEquationSolverFactory<ValueType>>(); break;
                case  EquationSolverType::Native: linearEquationSolverFactory = std::make_unique<NativeLinearEquationSolverFactory<ValueType>>(); break;
                case  EquationSolverType::Elimination: linearEquationSolverFactory = std::make_unique<EliminationLinearEquationSolverFactory<ValueType>>(); break;
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> StandardMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            if (linearEquationSolverFactory) {
                return std::make_unique<StandardMinMaxLinearEquationSolver<ValueType>>(matrix, linearEquationSolverFactory->clone(), settings);
            } else {
                return std::make_unique<StandardMinMaxLinearEquationSolver<ValueType>>(matrix, std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(), settings);
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> StandardMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result;
            if (linearEquationSolverFactory) {
                result = std::make_unique<StandardMinMaxLinearEquationSolver<ValueType>>(std::move(matrix), linearEquationSolverFactory->clone(), settings);
            } else {
                result = std::make_unique<StandardMinMaxLinearEquationSolver<ValueType>>(std::move(matrix), std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(), settings);
            }
            if (this->isTrackSchedulerSet()) {
                result->setTrackScheduler(true);
            }
            return result;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings& StandardMinMaxLinearEquationSolverFactory<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings const& StandardMinMaxLinearEquationSolverFactory<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        GmmxxMinMaxLinearEquationSolverFactory<ValueType>::GmmxxMinMaxLinearEquationSolverFactory(bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Gmmxx, trackScheduler) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        EigenMinMaxLinearEquationSolverFactory<ValueType>::EigenMinMaxLinearEquationSolverFactory(bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Eigen, trackScheduler) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        NativeMinMaxLinearEquationSolverFactory<ValueType>::NativeMinMaxLinearEquationSolverFactory(bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Native, trackScheduler) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        EliminationMinMaxLinearEquationSolverFactory<ValueType>::EliminationMinMaxLinearEquationSolverFactory(bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Elimination, trackScheduler) {
            // Intentionally left empty.
        }
        
        template class StandardMinMaxLinearEquationSolver<double>;
        
        template class StandardMinMaxLinearEquationSolverFactory<double>;
        template class GmmxxMinMaxLinearEquationSolverFactory<double>;
        template class EigenMinMaxLinearEquationSolverFactory<double>;
        template class NativeMinMaxLinearEquationSolverFactory<double>;
        template class EliminationMinMaxLinearEquationSolverFactory<double>;

    }
}