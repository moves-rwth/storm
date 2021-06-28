#include "storm/solver/TopologicalMinMaxLinearEquationSolver.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"

#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/utility/Stopwatch.h"
#include "storm/utility/ProgressMeasurement.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidEnvironmentException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/UncheckedRequirementException.h"
#include "storm/utility/SignalHandler.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        TopologicalMinMaxLinearEquationSolver<ValueType>::TopologicalMinMaxLinearEquationSolver() {
            // Intentionally left empty.
        }

        template<typename ValueType>
        TopologicalMinMaxLinearEquationSolver<ValueType>::TopologicalMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : StandardMinMaxLinearEquationSolver<ValueType>(A) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        TopologicalMinMaxLinearEquationSolver<ValueType>::TopologicalMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A) : StandardMinMaxLinearEquationSolver<ValueType>(std::move(A)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        storm::Environment TopologicalMinMaxLinearEquationSolver<ValueType>::getEnvironmentForUnderlyingSolver(storm::Environment const& env, bool adaptPrecision) const {
            storm::Environment subEnv(env);
            subEnv.solver().minMax().setMethod(env.solver().topological().getUnderlyingMinMaxMethod(), env.solver().topological().isUnderlyingMinMaxMethodSetFromDefault());
            if (adaptPrecision) {
                STORM_LOG_ASSERT(this->longestSccChainSize, "Did not compute the longest SCC chain size although it is needed.");
                storm::RationalNumber subEnvPrec = subEnv.solver().minMax().getPrecision() / storm::utility::convertNumber<storm::RationalNumber>(this->longestSccChainSize.get());
                subEnv.solver().minMax().setPrecision(subEnvPrec);
            }
            return subEnv;
        }

        template<typename ValueType>
        bool TopologicalMinMaxLinearEquationSolver<ValueType>::internalSolveEquations(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_ASSERT(x.size() == this->A->getRowGroupCount(), "Provided x-vector has invalid size.");
            STORM_LOG_ASSERT(b.size() == this->A->getRowCount(), "Provided b-vector has invalid size.");
            
            // For sound computations we need to increase the precision in each SCC
            bool needAdaptPrecision = env.solver().isForceSoundness();
            
            if (!this->sortedSccDecomposition || (needAdaptPrecision && !this->longestSccChainSize)) {
                STORM_LOG_TRACE("Creating SCC decomposition.");
                storm::utility::Stopwatch sccSw(true);
                createSortedSccDecomposition(needAdaptPrecision);
                sccSw.stop();
                STORM_LOG_INFO("SCC decomposition computed in " << sccSw << ". Found " << this->sortedSccDecomposition->size() << " SCC(s) containing a total of " << x.size() << " states. Average SCC size is " << static_cast<double>(this->A->getRowGroupCount()) / static_cast<double>(this->sortedSccDecomposition->size()) << ".");
            }
            
            // We do not need to adapt the precision if all SCCs are trivial (i.e., the system is acyclic)
            needAdaptPrecision = needAdaptPrecision && (this->sortedSccDecomposition->size() != this->A->getRowGroupCount());
            
            storm::Environment sccSolverEnvironment = getEnvironmentForUnderlyingSolver(env, needAdaptPrecision);
            
            if (this->longestSccChainSize) {
                STORM_LOG_INFO("Longest SCC chain size is " << this->longestSccChainSize.get());
            }
            
            bool returnValue = true;
            if (this->sortedSccDecomposition->size() == 1 && (!this->choiceFixedForState || this->choiceFixedForState.get().empty())) {
                // Handle the case where there is just one large SCC, as there are no fixed states, we solve it like this
                returnValue = solveFullyConnectedEquationSystem(sccSolverEnvironment, dir, x, b);
            } else {
                // Solve each SCC individually
                if (this->isTrackSchedulerSet()) {
                    if (this->schedulerChoices) {
                        this->schedulerChoices.get().resize(x.size());
                    } else {
                        this->schedulerChoices = std::vector<uint64_t>(x.size());
                    }
                }
                storm::storage::BitVector sccRowGroupsAsBitVector(x.size(), false);
                storm::storage::BitVector sccRowsAsBitVector(b.size(), false);
                uint64_t sccIndex = 0;
                storm::utility::ProgressMeasurement progress("states");
                progress.setMaxCount(x.size());
                progress.startNewMeasurement(0);
                for (auto const& scc : *this->sortedSccDecomposition) {
                    if (scc.size() == 1) {
                        returnValue = solveTrivialScc(*scc.begin(), dir, x, b) && returnValue;
                    } else {
                        STORM_LOG_TRACE("Solving SCC of size " << scc.size() << ".");
                        sccRowGroupsAsBitVector.clear();
                        sccRowsAsBitVector.clear();
                        for (auto const& group : scc) { // Group refers to state
                            sccRowGroupsAsBitVector.set(group, true);

                            if (!this->choiceFixedForState || !this->choiceFixedForState.get()[group]) {
                                for (uint64_t row = this->A->getRowGroupIndices()[group]; row < this->A->getRowGroupIndices()[group + 1]; ++row) {
                                    sccRowsAsBitVector.set(row, true);
                                }
                            } else {
                                auto row = this->A->getRowGroupIndices()[group]+this->getInitialScheduler()[group];
                                sccRowsAsBitVector.set(row, true);
                                STORM_LOG_INFO("Fixing state " << group << " to choice " << this->getInitialScheduler()[group] << ".");
                            }
                        }
                        returnValue = solveScc(sccSolverEnvironment, dir, sccRowGroupsAsBitVector, sccRowsAsBitVector, x, b) && returnValue;
                    }
                    ++sccIndex;
                    progress.updateProgress(sccIndex);
                    if (storm::utility::resources::isTerminate()) {
                        STORM_LOG_WARN("Topological solver aborted after analyzing " << sccIndex << "/" << this->sortedSccDecomposition->size() << " SCCs.");
                        break;
                    }
                }
                
                // If requested, we store the scheduler for retrieval.
                if (this->isTrackSchedulerSet()) {
                    if (!auxiliaryRowGroupVector) {
                        auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
                    }
                    this->schedulerChoices = std::vector<uint_fast64_t>(this->A->getRowGroupCount());
                    this->A->multiplyAndReduce(dir, this->A->getRowGroupIndices(), x, &b, *auxiliaryRowGroupVector.get(), &this->schedulerChoices.get());
                }
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            return returnValue;
        }
        
        template<typename ValueType>
        void TopologicalMinMaxLinearEquationSolver<ValueType>::createSortedSccDecomposition(bool needLongestChainSize) const {
            // Obtain the scc decomposition
            this->sortedSccDecomposition = std::make_unique<storm::storage::StronglyConnectedComponentDecomposition<ValueType>>(*this->A, storm::storage::StronglyConnectedComponentDecompositionOptions().forceTopologicalSort().computeSccDepths(needLongestChainSize));
            if (needLongestChainSize) {
                this->longestSccChainSize = this->sortedSccDecomposition->getMaxSccDepth() + 1;
            }
        }
        
        template<typename ValueType>
        bool TopologicalMinMaxLinearEquationSolver<ValueType>::solveTrivialScc(uint64_t const& sccState, OptimizationDirection dir, std::vector<ValueType>& globalX, std::vector<ValueType> const& globalB) const {
            ValueType& xi = globalX[sccState];
            bool firstRow = true;
            uint64_t bestRow;
            assert (!this->choiceFixedForState || !this->choiceFixedForState.get()[sccState] || (this->hasInitialScheduler() && this->A->getRowGroupSize(sccState) == 1));
            for (uint64_t row = this->A->getRowGroupIndices()[sccState]; row < this->A->getRowGroupIndices()[sccState + 1]; ++row) {
                ValueType rowValue = globalB[row];
                bool hasDiagonalEntry = false;
                ValueType denominator;
                for (auto const& entry : this->A->getRow(row)) {
                    if (entry.getColumn() == sccState) {
                        hasDiagonalEntry = true;
                        denominator = storm::utility::one<ValueType>() - entry.getValue();
                    } else {
                        rowValue += entry.getValue() * globalX[entry.getColumn()];
                    }
                }
                if (hasDiagonalEntry) {
                    STORM_LOG_WARN_COND_DEBUG(
                        storm::NumberTraits<ValueType>::IsExact || !storm::utility::isAlmostZero(denominator) || storm::utility::isZero(denominator),
                        "State " << sccState << " has a selfloop with probability '1-(" << denominator
                                 << ")'. This could be an indication for numerical issues.");
                    if (storm::utility::isZero(denominator)) {
                        // In this case we have a selfloop on this state. This can never an optimal choice:
                        // When minimizing, we are looking for the largest fixpoint (which will never be attained by this action)
                        // When maximizing, this choice reflects probability zero (non-optimal) or reward infinity (should already be handled during preprocessing).
                        continue;
                    } else {
                        rowValue /= denominator;
                    }
                }
                if (firstRow) {
                    xi = std::move(rowValue);
                    bestRow = row;
                    firstRow = false;
                } else {
                    if (minimize(dir)) {
                        if (rowValue < xi) {
                            xi = std::move(rowValue);
                            bestRow = row;
                        }
                    } else {
                        if (rowValue > xi) {
                            xi = std::move(rowValue);
                            bestRow = row;
                        }
                    }
                }
            }
            if (this->isTrackSchedulerSet()) {
                this->schedulerChoices.get()[sccState] = bestRow - this->A->getRowGroupIndices()[sccState];
            }
            STORM_LOG_THROW(!firstRow, storm::exceptions::UnexpectedException, "Empty row group in MinMax equation system.");
            return true;
        }
        
        template<typename ValueType>
        bool TopologicalMinMaxLinearEquationSolver<ValueType>::solveFullyConnectedEquationSystem(storm::Environment const& sccSolverEnvironment, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            if (!this->sccSolver) {
                this->sccSolver = GeneralMinMaxLinearEquationSolverFactory<ValueType>().create(sccSolverEnvironment);
                this->sccSolver->setCachingEnabled(true);
            }
            this->sccSolver->setMatrix(*this->A);
            this->sccSolver->setHasUniqueSolution(this->hasUniqueSolution());
            this->sccSolver->setHasNoEndComponents(this->hasNoEndComponents());
            this->sccSolver->setBoundsFromOtherSolver(*this);
            this->sccSolver->setTrackScheduler(this->isTrackSchedulerSet());
            if (this->hasInitialScheduler()) {
                auto choices = this->getInitialScheduler();
                this->sccSolver->setInitialScheduler(std::move(choices));
            }
            auto req = this->sccSolver->getRequirements(sccSolverEnvironment, dir);
            if (req.upperBounds() && this->hasUpperBound()) {
                req.clearUpperBounds();
            }
            if (req.lowerBounds() && this->hasLowerBound()) {
                req.clearLowerBounds();
            }
            if (req.validInitialScheduler() && this->hasInitialScheduler()) {
                req.clearValidInitialScheduler();
            }
            if (req.uniqueSolution() && this->hasUniqueSolution()) {
                req.clearUniqueSolution();
            }
            STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + req.getEnabledRequirementsAsString() + " not checked.");
            this->sccSolver->setRequirementsChecked(true);
            
            bool res = this->sccSolver->solveEquations(sccSolverEnvironment, dir, x, b);
            if (this->isTrackSchedulerSet()) {
                this->schedulerChoices = this->sccSolver->getSchedulerChoices();
            }
            return res;
        }
        
        template<typename ValueType>
        bool TopologicalMinMaxLinearEquationSolver<ValueType>::solveScc(storm::Environment const& sccSolverEnvironment, OptimizationDirection dir, storm::storage::BitVector const& sccRowGroups, storm::storage::BitVector const& sccRows, std::vector<ValueType>& globalX, std::vector<ValueType> const& globalB) const {

            // Set up the SCC solver
            if (!this->sccSolver) {
                this->sccSolver = GeneralMinMaxLinearEquationSolverFactory<ValueType>().create(sccSolverEnvironment);
                this->sccSolver->setCachingEnabled(true);
            }
            if (this->choiceFixedForState) {
                // convert fixed states to only fixed states of sccs
                storm::storage::BitVector choiceFixedForStateSCC(sccRowGroups.getNumberOfSetBits());
                auto j = 0;
                for (auto i : sccRowGroups) {
                    choiceFixedForStateSCC.set(j, this->choiceFixedForState.get()[i]);
                    j++;
                }
                assert (j = sccRowGroups.getNumberOfSetBits());
                this->sccSolver->setChoiceFixedForStates(std::move(choiceFixedForStateSCC));
            }
            this->sccSolver->setHasUniqueSolution(this->hasUniqueSolution());
            this->sccSolver->setHasNoEndComponents(this->hasNoEndComponents());
            this->sccSolver->setTrackScheduler(this->isTrackSchedulerSet());
            
            // SCC Matrix
            storm::storage::SparseMatrix<ValueType> sccA;
            if (this->choiceFixedForState) {
                sccA = this->A->getSubmatrix(false, sccRows, sccRowGroups);
            } else {
                sccA = this->A->getSubmatrix(true, sccRowGroups, sccRowGroups);

            }

            this->sccSolver->setMatrix(std::move(sccA));
            
            // x Vector
            auto sccX = storm::utility::vector::filterVector(globalX, sccRowGroups);
            
            // b Vector
            std::vector<ValueType> sccB;
            sccB.reserve(sccRows.getNumberOfSetBits());
            for (auto row : sccRows) {
                ValueType bi = globalB[row];
                for (auto const& entry : this->A->getRow(row)) {
                    if (!sccRowGroups.get(entry.getColumn())) {
                        bi += entry.getValue() * globalX[entry.getColumn()];
                    }
                }
                sccB.push_back(std::move(bi));
            }
            
            // initial scheduler
            if (this->hasInitialScheduler()) {
                auto sccInitChoices = storm::utility::vector::filterVector(this->getInitialScheduler(), sccRowGroups);
                this->sccSolver->setInitialScheduler(std::move(sccInitChoices));
                if (this->choiceFixedForState) {
                    this->sccSolver->setFixedChoicesToFirst();
                }
            }
            
            // lower/upper bounds
            if (this->hasLowerBound(storm::solver::AbstractEquationSolver<ValueType>::BoundType::Global)) {
                this->sccSolver->setLowerBound(this->getLowerBound());
            } else if (this->hasLowerBound(storm::solver::AbstractEquationSolver<ValueType>::BoundType::Local)) {
                this->sccSolver->setLowerBounds(storm::utility::vector::filterVector(this->getLowerBounds(), sccRowGroups));
            }
            if (this->hasUpperBound(storm::solver::AbstractEquationSolver<ValueType>::BoundType::Global)) {
                this->sccSolver->setUpperBound(this->getUpperBound());
            } else if (this->hasUpperBound(storm::solver::AbstractEquationSolver<ValueType>::BoundType::Local)) {
                this->sccSolver->setUpperBounds(storm::utility::vector::filterVector(this->getUpperBounds(), sccRowGroups));
            }
            
            // Requirements
            auto req = this->sccSolver->getRequirements(sccSolverEnvironment, dir);
            if (req.upperBounds() && this->hasUpperBound()) {
                req.clearUpperBounds();
            }
            if (req.lowerBounds() && this->hasLowerBound()) {
                req.clearLowerBounds();
            }
            if (req.validInitialScheduler() && this->hasInitialScheduler()) {
                req.clearValidInitialScheduler();
            }
            if (req.uniqueSolution() && this->hasUniqueSolution()) {
                req.clearUniqueSolution();
            }
            STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + req.getEnabledRequirementsAsString() + " not checked.");
            this->sccSolver->setRequirementsChecked(true);

            // Invoke scc solver
            bool res = this->sccSolver->solveEquations(sccSolverEnvironment, dir, sccX, sccB);
            //std::cout << "rhs is " << storm::utility::vector::toString(sccB) << std::endl;
            //std::cout << "x is " << storm::utility::vector::toString(sccX) << std::endl;
            
            // Set Scheduler choices
            if (this->isTrackSchedulerSet()) {
                storm::utility::vector::setVectorValues(this->schedulerChoices.get(), sccRowGroups, this->sccSolver->getSchedulerChoices());
            }
            
            // Set solution
            storm::utility::vector::setVectorValues(globalX, sccRowGroups, sccX);
            
            return res;
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverRequirements TopologicalMinMaxLinearEquationSolver<ValueType>::getRequirements(Environment const& env, boost::optional<storm::solver::OptimizationDirection> const& direction, bool const& hasInitialScheduler) const {
            // Return the requirements of the underlying solver
            return GeneralMinMaxLinearEquationSolverFactory<ValueType>().getRequirements(getEnvironmentForUnderlyingSolver(env), this->hasUniqueSolution(), this->hasNoEndComponents(), direction, hasInitialScheduler, this->isTrackSchedulerSet());
        }
        
        template<typename ValueType>
        void TopologicalMinMaxLinearEquationSolver<ValueType>::clearCache() const {
            sortedSccDecomposition.reset();
            longestSccChainSize = boost::none;
            sccSolver.reset();
            auxiliaryRowGroupVector.reset();
            StandardMinMaxLinearEquationSolver<ValueType>::clearCache();
        }
        
        // Explicitly instantiate the min max linear equation solver.
        template class TopologicalMinMaxLinearEquationSolver<double>;
        
#ifdef STORM_HAVE_CARL
        template class TopologicalMinMaxLinearEquationSolver<storm::RationalNumber>;
#endif
    }
}
