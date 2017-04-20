#include "storm/solver/StandardGameSolver.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GameSolverSettings.h"

#include "storm/solver/GmmxxLinearEquationSolver.h"
#include "storm/solver/EigenLinearEquationSolver.h"
#include "storm/solver/NativeLinearEquationSolver.h"
#include "storm/solver/EliminationLinearEquationSolver.h"

#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotImplementedException.h"
namespace storm {
    namespace solver {
        
        template<typename ValueType>
        StandardGameSolverSettings<ValueType>::StandardGameSolverSettings() {
            // Get the settings object to customize game solving.
            storm::settings::modules::GameSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::GameSolverSettings>();
            
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = storm::utility::convertNumber<ValueType>(settings.getPrecision());
            relative = settings.getConvergenceCriterion() == storm::settings::modules::GameSolverSettings::ConvergenceCriterion::Relative;
            
            auto method = settings.getGameSolvingMethod();
            switch (method) {
                case GameMethod::ValueIteration: this->solutionMethod = SolutionMethod::ValueIteration; break;
                case GameMethod::PolicyIteration: this->solutionMethod = SolutionMethod::PolicyIteration; break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
        }
        
        template<typename ValueType>
        void StandardGameSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& solutionMethod) {
            this->solutionMethod = solutionMethod;
        }
        
        template<typename ValueType>
        void StandardGameSolverSettings<ValueType>::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        void StandardGameSolverSettings<ValueType>::setRelativeTerminationCriterion(bool value) {
            this->relative = value;
        }
        
        template<typename ValueType>
        void StandardGameSolverSettings<ValueType>::setPrecision(ValueType precision) {
            this->precision = precision;
        }
        
        template<typename ValueType>
        typename StandardGameSolverSettings<ValueType>::SolutionMethod const& StandardGameSolverSettings<ValueType>::getSolutionMethod() const {
            return solutionMethod;
        }
        
        template<typename ValueType>
        uint64_t StandardGameSolverSettings<ValueType>::getMaximalNumberOfIterations() const {
            return maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        ValueType StandardGameSolverSettings<ValueType>::getPrecision() const {
            return precision;
        }
        
        template<typename ValueType>
        bool StandardGameSolverSettings<ValueType>::getRelativeTerminationCriterion() const {
            return relative;
        }
        
        template<typename ValueType>
        StandardGameSolver<ValueType>::StandardGameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardGameSolverSettings<ValueType> const& settings) : settings(settings), linearEquationSolverFactory(std::move(linearEquationSolverFactory)), localP1Matrix(nullptr), localP2Matrix(nullptr), player1Matrix(player1Matrix), player2Matrix(player2Matrix) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardGameSolver<ValueType>::StandardGameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type>&& player1Matrix, storm::storage::SparseMatrix<ValueType>&& player2Matrix, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardGameSolverSettings<ValueType> const& settings) : settings(settings), linearEquationSolverFactory(std::move(linearEquationSolverFactory)), localP1Matrix(std::make_unique<storm::storage::SparseMatrix<storm::storage::sparse::state_type>>(std::move(player1Matrix))), localP2Matrix(std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(player2Matrix))), player1Matrix(*localP1Matrix), player2Matrix(*localP2Matrix) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool StandardGameSolver<ValueType>::solveGame(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            switch (this->getSettings().getSolutionMethod()) {
                case StandardGameSolverSettings<ValueType>::SolutionMethod::ValueIteration:
                    return solveGameValueIteration(player1Dir, player2Dir, x, b);
                case StandardGameSolverSettings<ValueType>::SolutionMethod::PolicyIteration:
                    return solveGamePolicyIteration(player1Dir, player2Dir, x, b);
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "This solver does not implement the selected solution method");
            }
            return false;
        }
        
        template<typename ValueType>
        bool StandardGameSolver<ValueType>::solveGamePolicyIteration(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            
            // Create the initial choice selections.
            std::vector<storm::storage::sparse::state_type> player1Choices = this->hasSchedulerHints() ? this->player1SchedulerHint->getChoices() : std::vector<storm::storage::sparse::state_type>(this->player1Matrix.getRowGroupCount(), 0);
            std::vector<storm::storage::sparse::state_type> player2Choices = this->hasSchedulerHints() ? this->player2SchedulerHint->getChoices() : std::vector<storm::storage::sparse::state_type>(this->player2Matrix.getRowGroupCount(), 0);
            
            if(!auxiliaryP2RowGroupVector) {
                auxiliaryP2RowGroupVector = std::make_unique<std::vector<ValueType>>(this->player2Matrix.getRowGroupCount());
            }
            if(!auxiliaryP1RowGroupVector) {
                auxiliaryP1RowGroupVector = std::make_unique<std::vector<ValueType>>(this->player1Matrix.getRowGroupCount());
            }
            std::vector<ValueType>& subB = *auxiliaryP1RowGroupVector;

            // Solve the equation system induced by the two schedulers.
            storm::storage::SparseMatrix<ValueType> submatrix;
            getInducedMatrixVector(x, b, player1Choices, player2Choices, submatrix, subB);
            submatrix.convertToEquationSystem();
            auto submatrixSolver = linearEquationSolverFactory->create(std::move(submatrix));
            if (this->lowerBound) { submatrixSolver->setLowerBound(this->lowerBound.get()); }
            if (this->upperBound) { submatrixSolver->setUpperBound(this->upperBound.get()); }
            submatrixSolver->setCachingEnabled(true);
            
            Status status = Status::InProgress;
            uint64_t iterations = 0;
            do {
                // Solve the equation system for the 'DTMC'.
                // FIXME: we need to remove the 0- and 1- states to make the solution unique.
                submatrixSolver->solveEquations(x, subB);
                
                bool schedulerImproved = extractChoices(player1Dir, player2Dir, x, b, *auxiliaryP2RowGroupVector, player1Choices, player2Choices);
                
                // If the scheduler did not improve, we are done.
                if (!schedulerImproved) {
                    status = Status::Converged;
                } else {
                    // Update the solver.
                    getInducedMatrixVector(x, b, player1Choices, player2Choices, submatrix, subB);
                    submatrix.convertToEquationSystem();
                    submatrixSolver->setMatrix(std::move(submatrix));
                }
                
                // Update environment variables.
                ++iterations;
                status = updateStatusIfNotConverged(status, x, iterations);
            } while (status == Status::InProgress);
            
            reportStatus(status, iterations);
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulersSet()) {
                this->player1Scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(player1Choices));
                this->player2Scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(player2Choices));
            }
            
            if(!this->isCachingEnabled()) {
                clearCache();
            }
            
            return status == Status::Converged || status == Status::TerminatedEarly;
        }
        
        template<typename ValueType>
        bool StandardGameSolver<ValueType>::valueImproved(OptimizationDirection dir, ValueType const& value1, ValueType const& value2) const {
            if (dir == OptimizationDirection::Minimize) {
                return value2 < value1;
            } else {
                return value2 > value1;
            }
        }

        template<typename ValueType>
        ValueType StandardGameSolver<ValueType>::getPrecision() const {
            return this->getSettings().getPrecision();
        }

        template<typename ValueType>
        bool StandardGameSolver<ValueType>::getRelative() const {
            return this->getSettings().getRelativeTerminationCriterion();
        }

        template<typename ValueType>
        bool StandardGameSolver<ValueType>::solveGameValueIteration(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
                         
            if(!linEqSolverPlayer2Matrix) {
                linEqSolverPlayer2Matrix = linearEquationSolverFactory->create(player2Matrix);
                linEqSolverPlayer2Matrix->setCachingEnabled(true);
            }
            
            if (!auxiliaryP2RowVector) {
                auxiliaryP2RowVector = std::make_unique<std::vector<ValueType>>(player2Matrix.getRowCount());
            }
               
            if (!auxiliaryP2RowGroupVector) {
                auxiliaryP2RowGroupVector = std::make_unique<std::vector<ValueType>>(player2Matrix.getRowGroupCount());
            }
             
            if (!auxiliaryP1RowGroupVector) {
                auxiliaryP1RowGroupVector = std::make_unique<std::vector<ValueType>>(player1Matrix.getRowGroupCount());
            }
             
            std::vector<ValueType>& multiplyResult = *auxiliaryP2RowVector;
            std::vector<ValueType>& reducedMultiplyResult = *auxiliaryP2RowGroupVector;
             
            if (this->hasSchedulerHints()) {
                // Solve the equation system induced by the two schedulers.
                storm::storage::SparseMatrix<ValueType> submatrix;
                getInducedMatrixVector(x, b, this->player1SchedulerHint->getChoices(), this->player2SchedulerHint->getChoices(), submatrix, *auxiliaryP1RowGroupVector);
                submatrix.convertToEquationSystem();
                auto submatrixSolver = linearEquationSolverFactory->create(std::move(submatrix));
                if (this->lowerBound) { submatrixSolver->setLowerBound(this->lowerBound.get()); }
                if (this->upperBound) { submatrixSolver->setUpperBound(this->upperBound.get()); }
                submatrixSolver->solveEquations(x, *auxiliaryP1RowGroupVector);
            }
             
            std::vector<ValueType>* newX = auxiliaryP1RowGroupVector.get();
            std::vector<ValueType>* currentX = &x;
             
            // Proceed with the iterations as long as the method did not converge or reach the maximum number of iterations.
            uint64_t iterations = 0;
            
            Status status = Status::InProgress;
            while (status == Status::InProgress) {
                multiplyAndReduce(player1Dir, player2Dir, *currentX, &b, *linEqSolverPlayer2Matrix, multiplyResult, reducedMultiplyResult, *newX);

                // Determine whether the method converged.
                if (storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *newX, this->getSettings().getPrecision(), this->getSettings().getRelativeTerminationCriterion())) {
                    status = Status::Converged;
                }
                
                // Update environment variables.
                std::swap(currentX, newX);
                ++iterations;
                status = updateStatusIfNotConverged(status, *currentX, iterations);
            }
                        
            reportStatus(status, iterations);
            
            // If we performed an odd number of iterations, we need to swap the x and currentX, because the newest result
            // is currently stored in currentX, but x is the output vector.
            if (currentX == auxiliaryP1RowGroupVector.get()) {
                std::swap(x, *currentX);
            }
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulersSet()) {
                std::vector<uint_fast64_t> player1Choices(player1Matrix.getRowGroupCount(), 0);
                std::vector<uint_fast64_t> player2Choices(player2Matrix.getRowGroupCount(), 0);
                extractChoices(player1Dir, player2Dir, x, b, *auxiliaryP2RowGroupVector, player1Choices, player2Choices);
                this->player1Scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(player1Choices));
                this->player2Scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(player2Choices));
            }
            
            if(!this->isCachingEnabled()) {
                clearCache();
            }
            
            return (status == Status::Converged || status == Status::TerminatedEarly);
        }
        
        template<typename ValueType>
        void StandardGameSolver<ValueType>::repeatedMultiply(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const {
            
            if(!linEqSolverPlayer2Matrix) {
                linEqSolverPlayer2Matrix = linearEquationSolverFactory->create(player2Matrix);
                linEqSolverPlayer2Matrix->setCachingEnabled(true);
            }
            
            if (!auxiliaryP2RowVector) {
                auxiliaryP2RowVector = std::make_unique<std::vector<ValueType>>(player2Matrix.getRowCount());
            }
               
            if (!auxiliaryP2RowGroupVector) {
                auxiliaryP2RowGroupVector = std::make_unique<std::vector<ValueType>>(player2Matrix.getRowGroupCount());
            }
            std::vector<ValueType>& multiplyResult = *auxiliaryP2RowVector;
            std::vector<ValueType>& reducedMultiplyResult = *auxiliaryP2RowGroupVector;
            
            for (uint_fast64_t iteration = 0; iteration < n; ++iteration) {
                multiplyAndReduce(player1Dir, player2Dir, x, b, *linEqSolverPlayer2Matrix, multiplyResult, reducedMultiplyResult, x);
            }
            
            if(!this->isCachingEnabled()) {
                clearCache();
            }
        }
        
        template<typename ValueType>
        void StandardGameSolver<ValueType>::multiplyAndReduce(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const* b, storm::solver::LinearEquationSolver<ValueType> const& linEqSolver, std::vector<ValueType>& multiplyResult, std::vector<ValueType>& p2ReducedMultiplyResult, std::vector<ValueType>& p1ReducedMultiplyResult) const {
            
            linEqSolver.multiply(x, b, multiplyResult);
                
            storm::utility::vector::reduceVectorMinOrMax(player2Dir, multiplyResult, p2ReducedMultiplyResult, player2Matrix.getRowGroupIndices());

            uint_fast64_t pl1State = 0;
            for (auto& result : p1ReducedMultiplyResult) {
                storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_rows relevantRows = player1Matrix.getRowGroup(pl1State);
                STORM_LOG_ASSERT(relevantRows.getNumberOfEntries() != 0, "There is a choice of player 1 that does not lead to any player 2 choice");
                auto it = relevantRows.begin();
                auto ite = relevantRows.end();

                // Set the first value.
                result = p2ReducedMultiplyResult[it->getColumn()];
                ++it;
                
                // Now iterate through the different values and pick the extremal one.
                if (player1Dir == OptimizationDirection::Minimize) {
                    for (; it != ite; ++it) {
                        result = std::min(result, p2ReducedMultiplyResult[it->getColumn()]);
                    }
                } else {
                    for (; it != ite; ++it) {
                        result = std::max(result, p2ReducedMultiplyResult[it->getColumn()]);
                    }
                }
                ++pl1State;
            }
        }

        template<typename ValueType>
        bool StandardGameSolver<ValueType>::extractChoices(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType> const& x, std::vector<ValueType> const& b, std::vector<ValueType>& player2ChoiceValues, std::vector<uint_fast64_t>& player1Choices, std::vector<uint_fast64_t>& player2Choices) const {
            
            // get the choices of player 2 and the corresponding values.
            bool schedulerImproved = false;
            auto currentValueIt = player2ChoiceValues.begin();
            for (uint_fast64_t p2Group = 0; p2Group < this->player2Matrix.getRowGroupCount(); ++p2Group) {
                uint_fast64_t firstRowInGroup = this->player2Matrix.getRowGroupIndices()[p2Group];
                uint_fast64_t rowGroupSize = this->player2Matrix.getRowGroupIndices()[p2Group + 1] - firstRowInGroup;
                
                // We need to check whether the scheduler improved. Therefore, we first have to evaluate the current choice
                uint_fast64_t currentP2Choice = player2Choices[p2Group];
                *currentValueIt = storm::utility::zero<ValueType>();
                for (auto const& entry : this->player2Matrix.getRow(firstRowInGroup + currentP2Choice)) {
                    *currentValueIt += entry.getValue() * x[entry.getColumn()];
                }
                *currentValueIt += b[firstRowInGroup + currentP2Choice];
                
                // now check the other choices
                for (uint_fast64_t p2Choice = 0; p2Choice < rowGroupSize; ++p2Choice) {
                    if (p2Choice == currentP2Choice) {
                        continue;
                    }
                    ValueType choiceValue = storm::utility::zero<ValueType>();
                    for (auto const& entry : this->player2Matrix.getRow(firstRowInGroup + p2Choice)) {
                        choiceValue += entry.getValue() * x[entry.getColumn()];
                    }
                    choiceValue += b[firstRowInGroup + p2Choice];
                        
                    if (valueImproved(player2Dir, *currentValueIt, choiceValue)) {
                        schedulerImproved = true;
                        player2Choices[p2Group] = p2Choice;
                        *currentValueIt = std::move(choiceValue);
                    }
                }
            }
            
            // Now extract the choices of player 1
            for (uint_fast64_t p1Group = 0; p1Group < this->player1Matrix.getRowGroupCount(); ++p1Group) {
                uint_fast64_t firstRowInGroup = this->player1Matrix.getRowGroupIndices()[p1Group];
                uint_fast64_t rowGroupSize = this->player1Matrix.getRowGroupIndices()[p1Group + 1] - firstRowInGroup;
                uint_fast64_t currentChoice = player1Choices[p1Group];
                ValueType currentValue = player2ChoiceValues[this->player1Matrix.getRow(firstRowInGroup + currentChoice).begin()->getColumn()];
                for (uint_fast64_t p1Choice = 0; p1Choice < rowGroupSize; ++p1Choice) {
                    // If the choice is the currently selected one, we can skip it.
                    if (p1Choice == currentChoice) {
                        continue;
                    }
                    ValueType const& choiceValue = player2ChoiceValues[this->player1Matrix.getRow(firstRowInGroup + p1Choice).begin()->getColumn()];
                    if (valueImproved(player1Dir, currentValue, choiceValue)) {
                        schedulerImproved = true;
                        player1Choices[p1Group] = p1Choice;
                        currentValue = choiceValue;
                    }
                }
            }
            
            return schedulerImproved;
        }
        
        template<typename ValueType>
        void StandardGameSolver<ValueType>::getInducedMatrixVector(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<uint_fast64_t> const& player1Choices, std::vector<uint_fast64_t> const& player2Choices, storm::storage::SparseMatrix<ValueType>& inducedMatrix, std::vector<ValueType>& inducedVector) const {
            //Get the rows of the player2matrix that are selected by the schedulers
            //Note that rows can be selected more then once and in an arbitrary order.
            std::vector<storm::storage::sparse::state_type> selectedRows;
            selectedRows.reserve(player1Matrix.getRowGroupCount());
            uint_fast64_t pl1State = 0;
            for (auto const& pl1Choice : player1Choices){
                auto const& pl1Row = player1Matrix.getRow(pl1State, pl1Choice);
                STORM_LOG_ASSERT(pl1Row.getNumberOfEntries() == 1, "It is assumed that rows of player one have one entry, but this is not the case.");
                uint_fast64_t const& pl2State = pl1Row.begin()->getColumn();
                selectedRows.push_back(player2Matrix.getRowGroupIndices()[pl2State] + player2Choices[pl2State]);
                ++pl1State;
            }
            
            //Get the matrix and the vector induced by this selection. Note that we add entries at the diagonal
            inducedMatrix = player2Matrix.selectRowsFromRowIndexSequence(selectedRows, true);
            inducedVector.resize(inducedMatrix.getRowCount());
            storm::utility::vector::selectVectorValues<ValueType>(inducedVector, selectedRows, b);
        }
        
        template<typename ValueType>
        typename StandardGameSolver<ValueType>::Status StandardGameSolver<ValueType>::updateStatusIfNotConverged(Status status, std::vector<ValueType> const& x, uint64_t iterations) const {
            if (status != Status::Converged) {
                if (this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(x)) {
                    status = Status::TerminatedEarly;
                } else if (iterations >= this->getSettings().getMaximalNumberOfIterations()) {
                    status = Status::MaximalIterationsExceeded;
                }
            }
            return status;
        }
        
        template<typename ValueType>
        void StandardGameSolver<ValueType>::reportStatus(Status status, uint64_t iterations) const {
            switch (status) {
                case Status::Converged: STORM_LOG_INFO("Iterative solver converged after " << iterations << " iterations."); break;
                case Status::TerminatedEarly: STORM_LOG_INFO("Iterative solver terminated early after " << iterations << " iterations."); break;
                case Status::MaximalIterationsExceeded: STORM_LOG_WARN("Iterative solver did not converge after " << iterations << " iterations."); break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "Iterative solver terminated unexpectedly.");
            }
        }
        
        template<typename ValueType>
        StandardGameSolverSettings<ValueType> const& StandardGameSolver<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        void StandardGameSolver<ValueType>::setSettings(StandardGameSolverSettings<ValueType> const& newSettings) {
            settings = newSettings;
        }
        
        template<typename ValueType>
        void StandardGameSolver<ValueType>::clearCache() const {
            linEqSolverPlayer2Matrix.reset();
            auxiliaryP2RowVector.reset();
            auxiliaryP2RowGroupVector.reset();
            auxiliaryP1RowGroupVector.reset();
            GameSolver<ValueType>::clearCache();
        }
        
        template class StandardGameSolverSettings<double>;
        template class StandardGameSolver<double>;
        template class StandardGameSolverSettings<storm::RationalNumber>;
        template class StandardGameSolver<storm::RationalNumber>;
    }
}