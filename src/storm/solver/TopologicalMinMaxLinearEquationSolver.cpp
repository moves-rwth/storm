#include "storm/solver/TopologicalMinMaxLinearEquationSolver.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"

#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidEnvironmentException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/UnmetRequirementException.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        TopologicalMinMaxLinearEquationSolver<ValueType>::TopologicalMinMaxLinearEquationSolver() : localA(nullptr), A(nullptr) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        TopologicalMinMaxLinearEquationSolver<ValueType>::TopologicalMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : localA(nullptr), A(nullptr) {
            this->setMatrix(A);
        }

        template<typename ValueType>
        TopologicalMinMaxLinearEquationSolver<ValueType>::TopologicalMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A) : localA(nullptr), A(nullptr) {
            this->setMatrix(std::move(A));
        }
        
        template<typename ValueType>
        void TopologicalMinMaxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& A) {
            localA.reset();
            this->A = &A;
            clearCache();
        }

        template<typename ValueType>
        void TopologicalMinMaxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& A) {
            localA = std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A));
            this->A = localA.get();
            clearCache();
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
            
            //std::cout << "Solving equation system with fixpoint characterization " << std::endl;
            //std::cout << *this->A << std::endl;
            //std::cout << storm::utility::vector::toString(b) << std::endl;
            //std::cout << "Initial solution vector: " << std::endl;
            //std::cout << storm::utility::vector::toString(x) << std::endl;
            
            // For sound computations we need to increase the precision in each SCC
            bool needAdaptPrecision = env.solver().isForceSoundness();
            
            if (!this->sortedSccDecomposition || (needAdaptPrecision && !this->longestSccChainSize)) {
                STORM_LOG_TRACE("Creating SCC decomposition.");
                createSortedSccDecomposition(needAdaptPrecision);
            }
            
            //std::cout << "Sorted SCC decomposition: " << std::endl;
            //for (auto const& scc : *this->sortedSccDecomposition) {
                //std::cout << "SCC: ";
              //  for (auto const& row : scc) {
                    //std::cout << row << "  ";
               // }
                //std::cout << std::endl;
            //}
            
            // We do not need to adapt the precision if all SCCs are trivial (i.e., the system is acyclic)
            needAdaptPrecision = needAdaptPrecision && (this->sortedSccDecomposition->size() != this->A->getRowGroupCount());
            
            storm::Environment sccSolverEnvironment = getEnvironmentForUnderlyingSolver(env, needAdaptPrecision);
            
            std::cout << "Found " << this->sortedSccDecomposition->size() << "SCCs. Average size is " << static_cast<double>(this->A->getRowGroupCount()) / static_cast<double>(this->sortedSccDecomposition->size()) << "." << std::endl;
            if (this->longestSccChainSize) {
                std::cout << "Longest SCC chain size is " << this->longestSccChainSize.get() << std::endl;
            }
            
            bool returnValue = true;
            if (this->sortedSccDecomposition->size() == 1) {
                // Handle the case where there is just one large SCC
                returnValue = solveFullyConnectedEquationSystem(sccSolverEnvironment, dir, x, b);
            } else {
                if (this->isTrackSchedulerSet()) {
                    if (this->schedulerChoices) {
                        this->schedulerChoices.get().resize(x.size());
                    } else {
                        this->schedulerChoices = std::vector<uint64_t>(x.size());
                    }
                }
                storm::storage::BitVector sccRowGroupsAsBitVector(x.size(), false);
                storm::storage::BitVector sccRowsAsBitVector(b.size(), false);
                for (auto const& scc : *this->sortedSccDecomposition) {
                    if (scc.isTrivial()) {
                        returnValue = solveTrivialScc(*scc.begin(), dir, x, b) && returnValue;
                    } else {
                        sccRowGroupsAsBitVector.clear();
                        sccRowsAsBitVector.clear();
                        for (auto const& group : scc) {
                            sccRowGroupsAsBitVector.set(group, true);
                            for (uint64_t row = this->A->getRowGroupIndices()[group]; row < this->A->getRowGroupIndices()[group + 1]; ++row) {
                                sccRowsAsBitVector.set(row, true);
                            }
                        }
                        returnValue = solveScc(sccSolverEnvironment, dir, sccRowGroupsAsBitVector, sccRowsAsBitVector, x, b) && returnValue;
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
            auto sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<ValueType>(*this->A);
            
            // Get a mapping from matrix row to the corresponding scc
            STORM_LOG_THROW(sccDecomposition.size() < std::numeric_limits<uint32_t>::max(), storm::exceptions::UnexpectedException, "The number of SCCs is too large.");
            std::vector<uint32_t> sccIndices(this->A->getRowCount(), std::numeric_limits<uint32_t>::max());
            uint32_t sccIndex = 0;
            for (auto const& scc : sccDecomposition) {
                for (auto const& row : scc) {
                    sccIndices[row] = sccIndex;
                }
                ++sccIndex;
            }
            
            // Prepare the resulting set of sorted sccs
            this->sortedSccDecomposition = std::make_unique<std::vector<storm::storage::StronglyConnectedComponent>>();
            std::vector<storm::storage::StronglyConnectedComponent>& sortedSCCs = *this->sortedSccDecomposition;
            sortedSCCs.reserve(sccDecomposition.size());
            
            // Find a topological sort via DFS.
            storm::storage::BitVector unsortedSCCs(sccDecomposition.size(), true);
            std::vector<uint32_t> sccStack, chainSizes;
            if (needLongestChainSize) {
                chainSizes.resize(sccDecomposition.size(), 1u);
            }
            uint32_t longestChainSize = 0;
            uint32_t const token = std::numeric_limits<uint32_t>::max();
            std::set<uint64_t> successorSCCs;

            for (uint32_t firstUnsortedScc = 0; firstUnsortedScc < unsortedSCCs.size(); firstUnsortedScc = unsortedSCCs.getNextSetIndex(firstUnsortedScc + 1)) {
                
                sccStack.push_back(firstUnsortedScc);
                while (!sccStack.empty()) {
                    uint32_t currentSccIndex = sccStack.back();
                    if (currentSccIndex != token) {
                        // Check whether the SCC is still unprocessed
                        if (unsortedSCCs.get(currentSccIndex)) {
                            // Explore the successors of the scc.
                            storm::storage::StronglyConnectedComponent const& currentScc = sccDecomposition.getBlock(currentSccIndex);
                            // We first push a token on the stack in order to recognize later when all successors of this SCC have been explored already.
                            sccStack.push_back(token);
                            // Now add all successors that are not already sorted.
                            // Successors should only be added once, so we first prepare a set of them and add them afterwards.
                            successorSCCs.clear();
                            for (auto const& row : currentScc) {
                                for (auto const& entry : this->A->getRow(row)) {
                                    auto const& successorSCC = sccIndices[entry.getColumn()];
                                    if (successorSCC != currentSccIndex && unsortedSCCs.get(successorSCC)) {
                                        successorSCCs.insert(successorSCC);
                                    }
                                }
                            }
                            sccStack.insert(sccStack.end(), successorSCCs.begin(), successorSCCs.end());
                            
                        }
                    } else {
                        // all successors of the current scc have already been explored.
                        sccStack.pop_back(); // pop the token
                        
                        currentSccIndex = sccStack.back();
                        storm::storage::StronglyConnectedComponent& scc = sccDecomposition.getBlock(currentSccIndex);
                        
                        // Compute the longest chain size for this scc
                        if (needLongestChainSize) {
                            uint32_t& currentChainSize = chainSizes[currentSccIndex];
                            for (auto const& row : scc) {
                                for (auto const& entry : this->A->getRow(row)) {
                                    auto const& successorSCC = sccIndices[entry.getColumn()];
                                    if (successorSCC != currentSccIndex) {
                                        currentChainSize = std::max(currentChainSize, chainSizes[successorSCC] + 1);
                                    }
                                }
                            }
                            longestChainSize = std::max(longestChainSize, currentChainSize);
                        }
                        
                        unsortedSCCs.set(currentSccIndex, false);
                        sccStack.pop_back(); // pop the current scc index
                        sortedSCCs.push_back(std::move(scc));
                    }
                }
            }
            
            if (longestChainSize > 0) {
                this->longestSccChainSize = longestChainSize;
            }
        }
        
        template<typename ValueType>
        bool TopologicalMinMaxLinearEquationSolver<ValueType>::solveTrivialScc(uint64_t const& sccState, OptimizationDirection dir, std::vector<ValueType>& globalX, std::vector<ValueType> const& globalB) const {
            ValueType& xi = globalX[sccState];
            bool firstRow = true;
            uint64_t bestRow;
            
            for (uint64_t row = this->A->getRowGroupIndices()[sccState]; row < this->A->getRowGroupIndices()[sccState + 1]; ++row) {
                ValueType rowValue = globalB[sccState];
                bool hasDiagonalEntry = false;
                ValueType denominator;
                for (auto const& entry : this->A->getRow(sccState)) {
                    if (entry.getColumn() == sccState) {
                        STORM_LOG_ASSERT(!storm::utility::isOne(entry.getValue()), "Diagonal entry of fix point system has value 1.");
                        hasDiagonalEntry = true;
                        denominator = storm::utility::one<ValueType>() - entry.getValue();
                    } else {
                        rowValue += entry.getValue() * globalX[entry.getColumn()];
                    }
                }
                if (hasDiagonalEntry) {
                    rowValue /= denominator;
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
            //std::cout << "Solved trivial scc " << sccState << " with result " << globalX[sccState] << std::endl;
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
            this->sccSolver->setBoundsFromOtherSolver(*this);
            this->sccSolver->setTrackScheduler(this->isTrackSchedulerSet());
            if (this->hasInitialScheduler()) {
                auto choices = this->getInitialScheduler();
                this->sccSolver->setInitialScheduler(std::move(choices));
            }
            auto req = this->sccSolver->getRequirements(sccSolverEnvironment, dir);
            if (req.requiresUpperBounds() && this->hasUpperBound()) {
                req.clearUpperBounds();
            }
            if (req.requiresLowerBounds() && this->hasLowerBound()) {
                req.clearLowerBounds();
            }
            STORM_LOG_THROW(req.empty(), storm::exceptions::UnmetRequirementException, "Requirements of underlying solver not met.");
            
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
            this->sccSolver->setHasUniqueSolution(this->hasUniqueSolution());
            this->sccSolver->setTrackScheduler(this->isTrackSchedulerSet());
            
            // SCC Matrix
            storm::storage::SparseMatrix<ValueType> sccA = this->A->getSubmatrix(true, sccRowGroups, sccRowGroups);
            this->sccSolver->setMatrix(std::move(sccA));
            
            // x Vector
            auto sccX = storm::utility::vector::filterVector(globalX, sccRowGroups);
            
            // b Vector
            std::vector<ValueType> sccB;
            sccB.reserve(sccRows.getNumberOfSetBits());
            for (auto const& row : sccRows) {
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
            if (req.requiresUpperBounds() && this->hasUpperBound()) {
                req.clearUpperBounds();
            }
            if (req.requiresLowerBounds() && this->hasLowerBound()) {
                req.clearLowerBounds();
            }
            if (req.requiresValidInitialScheduler() && this->hasInitialScheduler()) {
                req.clearValidInitialScheduler();
            }
            STORM_LOG_THROW(req.empty(), storm::exceptions::UnmetRequirementException, "Requirements of underlying solver not met.");

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
        void TopologicalMinMaxLinearEquationSolver<ValueType>::repeatedMultiply(Environment const& env, OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const {
            
            storm::Environment sccSolverEnvironment = getEnvironmentForUnderlyingSolver(env);
            
            // Set up the SCC solver
            if (!this->sccSolver) {
                this->sccSolver = GeneralMinMaxLinearEquationSolverFactory<ValueType>().create(sccSolverEnvironment);
                this->sccSolver->setCachingEnabled(true);
            }
            this->sccSolver->setMatrix(*this->A);
            this->sccSolver->repeatedMultiply(sccSolverEnvironment, d, x, b, n);
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
        }

        template<typename ValueType>
        MinMaxLinearEquationSolverRequirements TopologicalMinMaxLinearEquationSolver<ValueType>::getRequirements(Environment const& env, boost::optional<storm::solver::OptimizationDirection> const& direction, bool const& hasInitialScheduler) const {
            // Return the requirements of the underlying solver
            return GeneralMinMaxLinearEquationSolverFactory<ValueType>().getRequirements(getEnvironmentForUnderlyingSolver(env), this->hasUniqueSolution(), direction, hasInitialScheduler);
        }
        
        template<typename ValueType>
        void TopologicalMinMaxLinearEquationSolver<ValueType>::clearCache() const {
            sortedSccDecomposition.reset();
            longestSccChainSize = boost::none;
            sccSolver.reset();
            auxiliaryRowGroupVector.reset();
            MinMaxLinearEquationSolver<ValueType>::clearCache();
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> TopologicalMinMaxLinearEquationSolverFactory<ValueType>::create(Environment const& env) const {
            return std::make_unique<storm::solver::TopologicalMinMaxLinearEquationSolver<ValueType>>();
        }
        
        // Explicitly instantiate the min max linear equation solver.
        template class TopologicalMinMaxLinearEquationSolver<double>;
        template class TopologicalMinMaxLinearEquationSolverFactory<double>;
        
#ifdef STORM_HAVE_CARL
        template class TopologicalMinMaxLinearEquationSolver<storm::RationalNumber>;
        template class TopologicalMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
#endif
    }
}
