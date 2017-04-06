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
            }
            return true;
        }
        
        template<typename ValueType>
        bool StandardGameSolver<ValueType>::solveGamePolicyIteration(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            

            
            
            
           /* // Create the initial scheduler.
            std::vector<storm::storage::sparse::state_type> scheduler = this->schedulerHint ? this->schedulerHint->getChoices() : std::vector<storm::storage::sparse::state_type>(this->A.getRowGroupCount());
            
            // Get a vector for storing the right-hand side of the inner equation system.
            if(!auxiliaryRowGroupVector) {
                auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A.getRowGroupCount());
            }
            std::vector<ValueType>& subB = *auxiliaryRowGroupVector;

            // Resolve the nondeterminism according to the current scheduler.
            storm::storage::SparseMatrix<ValueType> submatrix = this->A.selectRowsFromRowGroups(scheduler, true);
            submatrix.convertToEquationSystem();
            storm::utility::vector::selectVectorValues<ValueType>(subB, scheduler, this->A.getRowGroupIndices(), b);

            // Create a solver that we will use throughout the procedure. We will modify the matrix in each iteration.
            auto solver = linearEquationSolverFactory->create(std::move(submatrix));
            if (this->lowerBound) {
                solver->setLowerBound(this->lowerBound.get());
            }
            if (this->upperBound) {
                solver->setUpperBound(this->upperBound.get());
            }
            solver->setCachingEnabled(true);
            
            Status status = Status::InProgress;
            uint64_t iterations = 0;
            do {
                // Solve the equation system for the 'DTMC'.
                // FIXME: we need to remove the 0- and 1- states to make the solution unique.
                // HOWEVER: if we start with a valid scheduler, then we will never get an illegal one, because staying
                // within illegal MECs will never strictly improve the value. Is this true?
                solver->solveEquations(x, subB);
                
                // Go through the multiplication result and see whether we can improve any of the choices.
                bool schedulerImproved = false;
                for (uint_fast64_t group = 0; group < this->A.getRowGroupCount(); ++group) {
                    for (uint_fast64_t choice = this->A.getRowGroupIndices()[group]; choice < this->A.getRowGroupIndices()[group + 1]; ++choice) {
                        // If the choice is the currently selected one, we can skip it.
                        if (choice - this->A.getRowGroupIndices()[group] == scheduler[group]) {
                            continue;
                        }
                        
                        // Create the value of the choice.
                        ValueType choiceValue = storm::utility::zero<ValueType>();
                        for (auto const& entry : this->A.getRow(choice)) {
                            choiceValue += entry.getValue() * x[entry.getColumn()];
                        }
                        choiceValue += b[choice];
                        
                        // If the value is strictly better than the solution of the inner system, we need to improve the scheduler.
                        // TODO: If the underlying solver is not precise, this might run forever (i.e. when a state has two choices where the (exact) values are equal).
                        // only changing the scheduler if the values are not equal (modulo precision) would make this unsound.
                        if (valueImproved(dir, x[group], choiceValue)) {
                            schedulerImproved = true;
                            scheduler[group] = choice - this->A.getRowGroupIndices()[group];
                        }
                    }
                }
                
                // If the scheduler did not improve, we are done.
                if (!schedulerImproved) {
                    status = Status::Converged;
                } else {
                    // Update the scheduler and the solver.
                    submatrix = this->A.selectRowsFromRowGroups(scheduler, true);
                    submatrix.convertToEquationSystem();
                    storm::utility::vector::selectVectorValues<ValueType>(subB, scheduler, this->A.getRowGroupIndices(), b);
                    solver->setMatrix(std::move(submatrix));
                }
                
                // Update environment variables.
                ++iterations;
                status = updateStatusIfNotConverged(status, x, iterations);
            } while (status == Status::InProgress);
            
            reportStatus(status, iterations);
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulerSet()) {
                this->scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(scheduler));
            }
            
            if(!this->isCachingEnabled()) {
                clearCache();
            }
            
            if(status == Status::Converged || status == Status::TerminatedEarly) {
                return true;
            } else{
                return false;
            }*/
        }
        
        template<typename ValueType>
        bool StandardGameSolver<ValueType>::valueImproved(OptimizationDirection dir, ValueType const& value1, ValueType const& value2) const {
            if (dir == OptimizationDirection::Minimize) {
                return value1 > value2;
            } else {
                return value1 < value2;
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
            
            if (!auxiliaryP2RowVector.get()) {
                auxiliaryP2RowVector = std::make_unique<std::vector<ValueType>>(player2Matrix.getRowCount());
            }
               
            if (!auxiliaryP2RowGroupVector.get()) {
                auxiliaryP2RowGroupVector = std::make_unique<std::vector<ValueType>>(player2Matrix.getRowGroupCount());
            }
             
            if (!auxiliaryP1RowGroupVector.get()) {
                auxiliaryP1RowGroupVector = std::make_unique<std::vector<ValueType>>(player1Matrix.getRowGroupCount());
            }
             
            std::vector<ValueType>& multiplyResult = *auxiliaryP2RowVector;
            std::vector<ValueType>& reducedMultiplyResult = *auxiliaryP2RowGroupVector;
             
             
            // if this->schedulerHint ...
             
             
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
                multiplyAndReduce(player1Dir, player2Dir, x, &b, *linEqSolverPlayer2Matrix, multiplyResult, reducedMultiplyResult, *auxiliaryP1RowGroupVector, &player1Choices, &player2Choices);
                this->player1Scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(player1Choices));
                this->player2Scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(player2Choices));
 
            }
            
            if(!this->isCachingEnabled()) {
                clearCache();
            }
            
            if(status == Status::Converged || status == Status::TerminatedEarly) {
                return true;
            } else{
                return false;
            }
             
             
             
             
                         /*      // Set up the environment for value iteration.
            bool converged = false;
            uint_fast64_t numberOfPlayer1States = x.size();
            std::vector<ValueType> tmpResult(numberOfPlayer1States);
            std::vector<ValueType> nondetResult(player2Matrix.getRowCount());
            std::vector<ValueType> player2Result(player2Matrix.getRowGroupCount());

            // Now perform the actual value iteration.
            uint_fast64_t iterations = 0;
            do {
                player2Matrix.multiplyWithVector(x, nondetResult);
                storm::utility::vector::addVectors(b, nondetResult, nondetResult);

                if (player2Goal == OptimizationDirection::Minimize) {
                    storm::utility::vector::reduceVectorMin(nondetResult, player2Result, player2Matrix.getRowGroupIndices());
                } else {
                    storm::utility::vector::reduceVectorMax(nondetResult, player2Result, player2Matrix.getRowGroupIndices());
                }

                for (uint_fast64_t pl1State = 0; pl1State < numberOfPlayer1States; ++pl1State) {
                    storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_rows relevantRows = player1Matrix.getRowGroup(pl1State);
                    if (relevantRows.getNumberOfEntries() > 0) {
                        storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_iterator it = relevantRows.begin();
                        storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_iterator ite = relevantRows.end();

                        // Set the first value.
                        tmpResult[pl1State] = player2Result[it->getColumn()];
                        ++it;

                        // Now iterate through the different values and pick the extremal one.
                        if (player1Goal == OptimizationDirection::Minimize) {
                            for (; it != ite; ++it) {
                                tmpResult[pl1State] = std::min(tmpResult[pl1State], player2Result[it->getColumn()]);
                            }
                        } else {
                            for (; it != ite; ++it) {
                                tmpResult[pl1State] = std::max(tmpResult[pl1State], player2Result[it->getColumn()]);
                            }
                        }
                    } else {
                        tmpResult[pl1State] = storm::utility::zero<ValueType>();
                    }
                }

                // Check if the process converged and set up the new iteration in case we are not done.
                converged = storm::utility::vector::equalModuloPrecision(x, tmpResult, this->precision, this->relative);
                std::swap(x, tmpResult);

                ++iterations;
            } while (!converged && iterations < this->maximalNumberOfIterations && !(this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(x)));
            
            STORM_LOG_WARN_COND(converged, "Iterative solver for stochastic two player games did not converge after " << iterations << " iterations.");
            
            if(this->trackScheduler){
                std::vector<uint_fast64_t> player2Choices(player2Matrix.getRowGroupCount());
                storm::utility::vector::reduceVectorMinOrMax(player2Goal, nondetResult, player2Result, player2Matrix.getRowGroupIndices(), &player2Choices);
                this->player2Scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(player2Choices));

                std::vector<uint_fast64_t> player1Choices(numberOfPlayer1States, 0);
                for (uint_fast64_t pl1State = 0; pl1State < numberOfPlayer1States; ++pl1State) {
                    storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_rows relevantRows = player1Matrix.getRowGroup(pl1State);
                    if (relevantRows.getNumberOfEntries() > 0) {
                        storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_iterator it = relevantRows.begin();
                        storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_iterator ite = relevantRows.end();
                        // Set the first value.
                        tmpResult[pl1State] = player2Result[it->getColumn()];
                        ++it;
                        storm::storage::sparse::state_type localChoice = 1;
                        // Now iterate through the different values and pick the extremal one.
                        if (player1Goal == OptimizationDirection::Minimize) {
                            for (; it != ite; ++it, ++localChoice) {
                                if(player2Result[it->getColumn()] < tmpResult[pl1State]){
                                    tmpResult[pl1State] = player2Result[it->getColumn()];
                                    player1Choices[pl1State] = localChoice;
                                }
                            }
                        } else {
                            for (; it != ite; ++it, ++localChoice) {
                                if(player2Result[it->getColumn()] > tmpResult[pl1State]){
                                    tmpResult[pl1State] = player2Result[it->getColumn()];
                                    player1Choices[pl1State] = localChoice;
                                }
                            }
                        }
                    } else {
                        STORM_LOG_ERROR("There is no choice for Player 1 at state " << pl1State << " in the stochastic two player game. This is not expected!");
                    }
                }
                this->player1Scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(player1Choices));
            }
            
             
             
             * if(!linEqSolverA) {
                linEqSolverA = linearEquationSolverFactory->create(A);
                linEqSolverA->setCachingEnabled(true);
            }
            
            if (!auxiliaryRowVector.get()) {
                auxiliaryRowVector = std::make_unique<std::vector<ValueType>>(A.getRowCount());
            }
            std::vector<ValueType>& multiplyResult = *auxiliaryRowVector;
            
            if (!auxiliaryRowGroupVector.get()) {
                auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(A.getRowGroupCount());
            }
            
            if(this->schedulerHint) {
                // Resolve the nondeterminism according to the scheduler hint
                storm::storage::SparseMatrix<ValueType> submatrix = this->A.selectRowsFromRowGroups(this->schedulerHint->getChoices(), true);
                submatrix.convertToEquationSystem();
                storm::utility::vector::selectVectorValues<ValueType>(*auxiliaryRowGroupVector, this->schedulerHint->getChoices(), this->A.getRowGroupIndices(), b);

                // Solve the resulting equation system.
                // Note that the linEqSolver might consider a slightly different interpretation of "equalModuloPrecision". Hence, we iteratively increase its precision.
                auto submatrixSolver = linearEquationSolverFactory->create(std::move(submatrix));
                submatrixSolver->setCachingEnabled(true);
                if (this->lowerBound) { submatrixSolver->setLowerBound(this->lowerBound.get()); }
                if (this->upperBound) { submatrixSolver->setUpperBound(this->upperBound.get()); }
                submatrixSolver->solveEquations(x, *auxiliaryRowGroupVector);
            }
            
            std::vector<ValueType>* newX = auxiliaryRowGroupVector.get();
            
            std::vector<ValueType>* currentX = &x;
            
            // Proceed with the iterations as long as the method did not converge or reach the maximum number of iterations.
            uint64_t iterations = 0;
            
            Status status = Status::InProgress;
            while (status == Status::InProgress) {
                // Compute x' = A*x + b.
                linEqSolverA->multiply(*currentX, &b, multiplyResult);
                
                // Reduce the vector x' by applying min/max for all non-deterministic choices.
                storm::utility::vector::reduceVectorMinOrMax(dir, multiplyResult, *newX, this->A.getRowGroupIndices());
                
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
            if (currentX == auxiliaryRowGroupVector.get()) {
                std::swap(x, *currentX);
            }
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulerSet()) {
                // Due to a custom termination condition, it may be the case that no iterations are performed. In this
                // case we need to compute x'= A*x+b once.
                if (iterations==0) {
                    linEqSolverA->multiply(x, &b, multiplyResult);
                }
                std::vector<storm::storage::sparse::state_type> choices(this->A.getRowGroupCount());
                // Reduce the multiplyResult and keep track of the choices made
                storm::utility::vector::reduceVectorMinOrMax(dir, multiplyResult, x, this->A.getRowGroupIndices(), &choices);
                this->scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(choices));
            }

            if(!this->isCachingEnabled()) {
                clearCache();
            }
            
            if(status == Status::Converged || status == Status::TerminatedEarly) {
                return true;
            } else{
                return false;
            }
             */
        }
        
        template<typename ValueType>
        void StandardGameSolver<ValueType>::repeatedMultiply(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const {
            
            if(!linEqSolverPlayer2Matrix) {
                linEqSolverPlayer2Matrix = linearEquationSolverFactory->create(player2Matrix);
                linEqSolverPlayer2Matrix->setCachingEnabled(true);
            }
            
            if (!auxiliaryP2RowVector.get()) {
                auxiliaryP2RowVector = std::make_unique<std::vector<ValueType>>(player2Matrix.getRowCount());
            }
               
            if (!auxiliaryP2RowGroupVector.get()) {
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
        void StandardGameSolver<ValueType>::multiplyAndReduce(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const* b, storm::solver::LinearEquationSolver<ValueType>& linEqSolver, std::vector<ValueType>& multiplyResult, std::vector<ValueType>& p2ReducedMultiplyResult, std::vector<ValueType>& p1ReducedMultiplyResult, std::vector<uint_fast64_t>* player1Choices, std::vector<uint_fast64_t>* player2Choices) const {
            
            linEqSolver.multiply(x, b, multiplyResult);
                
            storm::utility::vector::reduceVectorMinOrMax(player2Dir, multiplyResult, p2ReducedMultiplyResult, player2Matrix.getRowGroupIndices(), player2Choices);

            uint_fast64_t pl1State = 0;
            std::vector<uint_fast64_t>::iterator choiceIt;
            if (player1Choices) {
                choiceIt = player1Choices->begin();
            }
            for (auto& result : p1ReducedMultiplyResult) {
                storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_rows relevantRows = player1Matrix.getRowGroup(pl1State);
                STORM_LOG_ASSERT(relevantRows.getNumberOfEntries() != 0, "There is a choice of player 1 that does not lead to any player 2 choice");
                auto it = relevantRows.begin();
                auto ite = relevantRows.end();

                // Set the first value.
                result = p2ReducedMultiplyResult[it->getColumn()];
                if (player1Choices) {
                    *choiceIt = 0;
                }
                uint_fast64_t localChoice = 1;
                ++it;
                
                // Now iterate through the different values and pick the extremal one.
                if (player1Dir == OptimizationDirection::Minimize) {
                    for (; it != ite; ++it, ++localChoice) {
                        ValueType& val = p2ReducedMultiplyResult[it->getColumn()];
                        if (val < result) {
                            result = val;
                            if (player1Choices) {
                                *choiceIt = localChoice;
                            }
                        }
                    }
                } else {
                    for (; it != ite; ++it, ++localChoice) {
                        ValueType& val = p2ReducedMultiplyResult[it->getColumn()];
                        if (val > result) {
                            result = val;
                            if (player1Choices) {
                                *choiceIt = localChoice;
                            }
                        }
                    }
                }
                ++pl1State;
                if (player1Choices) {
                    ++choiceIt;
                }
            }
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