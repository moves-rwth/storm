#include "storm/solver/TopologicalCudaMinMaxLinearEquationSolver.h"

#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/InvalidEnvironmentException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/utility/graph.h"
#include "storm/utility/vector.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm-config.h"
#include "storm/utility/macros.h"
#ifdef STORM_HAVE_CUDA
#include "cudaForStorm.h"
#endif

namespace storm {
namespace solver {

template<typename ValueType>
TopologicalCudaMinMaxLinearEquationSolver<ValueType>::TopologicalCudaMinMaxLinearEquationSolver() {
    // Get the settings object to customize solving.
    this->enableCuda = storm::settings::getModule<storm::settings::modules::CoreSettings>().isUseCudaSet();
#ifdef STORM_HAVE_CUDA
    STORM_LOG_INFO_COND(this->enableCuda, "Option CUDA was not set, but the topological value iteration solver will use it anyways.");
#endif
}

template<typename ValueType>
TopologicalCudaMinMaxLinearEquationSolver<ValueType>::TopologicalCudaMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A)
    : TopologicalCudaMinMaxLinearEquationSolver() {
    this->setMatrix(A);
}

template<typename ValueType>
void TopologicalCudaMinMaxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) {
    this->localA = nullptr;
    this->A = &matrix;
}

template<typename ValueType>
void TopologicalCudaMinMaxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& matrix) {
    this->localA = std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(matrix));
    this->A = this->localA.get();
}

template<class ValueType>
void TopologicalCudaMinMaxLinearEquationSolver<ValueType>::setSchedulerFixedForRowGroup(storm::storage::BitVector&& states) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "Setting fixed choices for scheduler not implemented for TopologicalCudaMinMaxLinearEquationSolver");
}

template<typename ValueType>
bool TopologicalCudaMinMaxLinearEquationSolver<ValueType>::internalSolveEquations(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x,
                                                                                  std::vector<ValueType> const& b) const {
    STORM_LOG_THROW(env.solver().minMax().getMethod() == MinMaxMethod::TopologicalCuda, storm::exceptions::InvalidEnvironmentException,
                    "This min max solver does not support the selected technique.");

    ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().minMax().getPrecision());
    uint64_t maxIters = env.solver().minMax().getMaximalNumberOfIterations();
    bool relative = env.solver().minMax().getMaximalNumberOfIterations();

    // For testing only
    // std::cout << "<<< Using CUDA-DOUBLE Kernels >>>\n";
    STORM_LOG_INFO("<<< Using CUDA-DOUBLE Kernels >>>");

    // Now, we need to determine the SCCs of the MDP and perform a topological sort.
    std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = this->A->getRowGroupIndices();

    // Check if the decomposition is necessary
#ifdef STORM_HAVE_CUDA
#define __USE_CUDAFORSTORM_OPT true
    size_t const gpuSizeOfCompleteSystem = basicValueIteration_mvReduce_uint64_double_calculateMemorySize(
        static_cast<size_t>(A->getRowCount()), nondeterministicChoiceIndices.size(), static_cast<size_t>(A->getEntryCount()));
    size_t const cudaFreeMemory = static_cast<size_t>(getFreeCudaMemory() * 0.95);
#else
#define __USE_CUDAFORSTORM_OPT false
    size_t const gpuSizeOfCompleteSystem = 0;
    size_t const cudaFreeMemory = 0;
#endif
    std::vector<std::pair<bool, storm::storage::StateBlock>> sccDecomposition;
    if (__USE_CUDAFORSTORM_OPT && (gpuSizeOfCompleteSystem < cudaFreeMemory)) {
        // Dummy output for SCC Times
        // std::cout << "Computing the SCC Decomposition took 0ms\n";

#ifdef STORM_HAVE_CUDA
        STORM_LOG_THROW(resetCudaDevice(), storm::exceptions::InvalidStateException, "Could not reset CUDA Device, can not use CUDA Equation Solver.");

        bool result = false;
        size_t globalIterations = 0;
        if (dir == OptimizationDirection::Minimize) {
            result = __basicValueIteration_mvReduce_minimize<uint_fast64_t, ValueType>(maxIters, precision, relative, A->rowIndications, A->columnsAndValues, x,
                                                                                       b, nondeterministicChoiceIndices, globalIterations);
        } else {
            result = __basicValueIteration_mvReduce_maximize<uint_fast64_t, ValueType>(maxIters, precision, relative, A->rowIndications, A->columnsAndValues, x,
                                                                                       b, nondeterministicChoiceIndices, globalIterations);
        }
        STORM_LOG_INFO("Executed " << globalIterations << " of max. " << maximalNumberOfIterations << " Iterations on GPU.");

        bool converged = false;
        if (!result) {
            converged = false;
            STORM_LOG_ERROR("An error occurred in the CUDA Plugin. Can not continue.");
            throw storm::exceptions::InvalidStateException() << "An error occurred in the CUDA Plugin. Can not continue.";
        } else {
            converged = true;
        }

        // Check if the solver converged and issue a warning otherwise.
        if (converged) {
            STORM_LOG_INFO("Iterative solver converged after " << globalIterations << " iterations.");
        } else {
            STORM_LOG_WARN("Iterative solver did not converged after " << globalIterations << " iterations.");
        }
#else
        STORM_LOG_ERROR("The useGpu Flag of a SCC was set, but this version of storm does not support CUDA acceleration. Internal Error!");
        throw storm::exceptions::InvalidStateException()
            << "The useGpu Flag of a SCC was set, but this version of storm does not support CUDA acceleration. Internal Error!";
#endif
    } else {
        storm::storage::StronglyConnectedComponentDecomposition<ValueType> sccDecomposition(*this->A);

        STORM_LOG_THROW(sccDecomposition.size() > 0, storm::exceptions::IllegalArgumentException,
                        "Can not solve given equation system as the SCC decomposition returned no SCCs.");

        storm::storage::SparseMatrix<ValueType> stronglyConnectedComponentsDependencyGraph = sccDecomposition.extractPartitionDependencyGraph(*this->A);
        std::vector<uint_fast64_t> topologicalSort = storm::utility::graph::getTopologicalSort(stronglyConnectedComponentsDependencyGraph);

        // Calculate the optimal distribution of sccs
        std::vector<std::pair<bool, storm::storage::StateBlock>> optimalSccs =
            this->getOptimalGroupingFromTopologicalSccDecomposition(sccDecomposition, topologicalSort, *this->A);
        STORM_LOG_INFO("Optimized SCC Decomposition, originally " << topologicalSort.size() << " SCCs, optimized to " << optimalSccs.size() << " SCCs.");

        std::vector<ValueType>* currentX = nullptr;
        std::vector<ValueType>* swap = nullptr;
        size_t currentMaxLocalIterations = 0;
        size_t localIterations = 0;
        size_t globalIterations = 0;
        bool converged = true;

        // Iterate over all SCCs of the MDP as specified by the topological sort. This guarantees that an SCC is only
        // solved after all SCCs it depends on have been solved.
        for (auto sccIndexIt = optimalSccs.cbegin(); sccIndexIt != optimalSccs.cend() && converged; ++sccIndexIt) {
            bool const useGpu = sccIndexIt->first;
            storm::storage::StateBlock const& scc = sccIndexIt->second;

            // Generate a sub matrix
            storm::storage::BitVector subMatrixIndices(this->A->getColumnCount(), scc.cbegin(), scc.cend());
            storm::storage::SparseMatrix<ValueType> sccSubmatrix = this->A->getSubmatrix(true, subMatrixIndices, subMatrixIndices);
            std::vector<ValueType> sccSubB(sccSubmatrix.getRowCount());
            storm::utility::vector::selectVectorValues<ValueType>(sccSubB, subMatrixIndices, nondeterministicChoiceIndices, b);
            std::vector<ValueType> sccSubX(sccSubmatrix.getColumnCount());
            std::vector<ValueType> sccSubXSwap(sccSubmatrix.getColumnCount());
            std::vector<ValueType> sccMultiplyResult(sccSubmatrix.getRowCount());

            // Prepare the pointers for swapping in the calculation
            currentX = &sccSubX;
            swap = &sccSubXSwap;

            storm::utility::vector::selectVectorValues<ValueType>(
                sccSubX, subMatrixIndices, x);  // x is getCols() large, where as b and multiplyResult are getRows() (nondet. choices times states)
            std::vector<uint_fast64_t> sccSubNondeterministicChoiceIndices(sccSubmatrix.getColumnCount() + 1);
            sccSubNondeterministicChoiceIndices.at(0) = 0;

            // Pre-process all dependent states
            // Remove outgoing transitions and create the ChoiceIndices
            uint_fast64_t innerIndex = 0;
            uint_fast64_t outerIndex = 0;
            for (uint_fast64_t state : scc) {
                // Choice Indices
                sccSubNondeterministicChoiceIndices.at(outerIndex + 1) =
                    sccSubNondeterministicChoiceIndices.at(outerIndex) + (nondeterministicChoiceIndices[state + 1] - nondeterministicChoiceIndices[state]);

                for (auto rowGroupIt = nondeterministicChoiceIndices[state]; rowGroupIt != nondeterministicChoiceIndices[state + 1]; ++rowGroupIt) {
                    typename storm::storage::SparseMatrix<ValueType>::const_rows row = this->A->getRow(rowGroupIt);
                    for (auto rowIt = row.begin(); rowIt != row.end(); ++rowIt) {
                        if (!subMatrixIndices.get(rowIt->getColumn())) {
                            // This is an outgoing transition of a state in the SCC to a state not included in the SCC
                            // Subtracting Pr(tau) * x_other from b fixes that
                            sccSubB.at(innerIndex) = sccSubB.at(innerIndex) + (rowIt->getValue() * x.at(rowIt->getColumn()));
                        }
                    }
                    ++innerIndex;
                }
                ++outerIndex;
            }

            // For the current SCC, we need to perform value iteration until convergence.
            if (useGpu) {
#ifdef STORM_HAVE_CUDA
                STORM_LOG_THROW(resetCudaDevice(), storm::exceptions::InvalidStateException,
                                "Could not reset CUDA Device, can not use CUDA-based equation solver.");

                // STORM_LOG_INFO("Device has " << getTotalCudaMemory() << " Bytes of Memory with " << getFreeCudaMemory() << "Bytes free (" <<
                // (static_cast<double>(getFreeCudaMemory()) / static_cast<double>(getTotalCudaMemory())) * 100 << "%)."); STORM_LOG_INFO("We will allocate " <<
                // (sizeof(uint_fast64_t)* sccSubmatrix.rowIndications.size() + sizeof(uint_fast64_t)* sccSubmatrix.columnsAndValues.size() * 2 +
                // sizeof(double)* sccSubX.size() + sizeof(double)* sccSubX.size() + sizeof(double)* sccSubB.size() + sizeof(double)* sccSubB.size() +
                // sizeof(uint_fast64_t)* sccSubNondeterministicChoiceIndices.size()) << " Bytes."); STORM_LOG_INFO("The CUDA Runtime Version is " <<
                // getRuntimeCudaVersion());

                bool result = false;
                localIterations = 0;
                if (dir == OptimizationDirection::Minimum) {
                    result = __basicValueIteration_mvReduce_minimize<uint_fast64_t, ValueType>(maxIters, precision, relative, sccSubmatrix.rowIndications,
                                                                                               sccSubmatrix.columnsAndValues, *currentX, sccSubB,
                                                                                               sccSubNondeterministicChoiceIndices, localIterations);
                } else {
                    result = __basicValueIteration_mvReduce_maximize<uint_fast64_t, ValueType>(maxIters, precision, relative, sccSubmatrix.rowIndications,
                                                                                               sccSubmatrix.columnsAndValues, *currentX, sccSubB,
                                                                                               sccSubNondeterministicChoiceIndices, localIterations);
                }
                STORM_LOG_INFO("Executed " << localIterations << " of max. " << maximalNumberOfIterations << " Iterations on GPU.");

                if (!result) {
                    converged = false;
                    STORM_LOG_ERROR("An error occurred in the CUDA Plugin. Can not continue.");
                    throw storm::exceptions::InvalidStateException() << "An error occurred in the CUDA Plugin. Can not continue.";
                } else {
                    converged = true;
                }

                // As the "number of iterations" of the full method is the maximum of the local iterations, we need to keep
                // track of the maximum.
                if (localIterations > currentMaxLocalIterations) {
                    currentMaxLocalIterations = localIterations;
                }
                globalIterations += localIterations;
#else
                STORM_LOG_ERROR("The useGpu Flag of a SCC was set, but this version of storm does not support CUDA acceleration. Internal Error!");
                throw storm::exceptions::InvalidStateException()
                    << "The useGpu Flag of a SCC was set, but this version of storm does not support CUDA acceleration. Internal Error!";
#endif
            } else {
                // std::cout << "WARNING: Using CPU based TopoSolver! (double)\n";
                STORM_LOG_INFO("Performance Warning: Using CPU based TopoSolver! (double)");
                localIterations = 0;
                converged = false;
                while (!converged && localIterations < maxIters) {
                    // Compute x' = A*x + b.
                    sccSubmatrix.multiplyWithVector(*currentX, sccMultiplyResult);
                    storm::utility::vector::addVectors<ValueType>(sccMultiplyResult, sccSubB, sccMultiplyResult);

                    // A.multiplyWithVector(scc, nondeterministicChoiceIndices, *currentX, multiplyResult);
                    // storm::utility::addVectors(scc, nondeterministicChoiceIndices, multiplyResult, b);

                    /*
                    Versus:
                    A.multiplyWithVector(*currentX, *multiplyResult);
                    storm::utility::vector::addVectorsInPlace(*multiplyResult, b);
                    */

                    // Reduce the vector x' by applying min/max for all non-deterministic choices.
                    storm::utility::vector::reduceVectorMinOrMax<ValueType>(dir, sccMultiplyResult, *swap, sccSubNondeterministicChoiceIndices);

                    // Determine whether the method converged.
                    // TODO: It seems that the equalModuloPrecision call that compares all values should have a higher
                    // running time. In fact, it is faster. This has to be investigated.
                    // converged = storm::utility::equalModuloPrecision(*currentX, *newX, scc, precision, relative);
                    converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *swap, precision, relative);

                    // Update environment variables.
                    std::swap(currentX, swap);

                    ++localIterations;
                    ++globalIterations;
                }
                STORM_LOG_INFO("Executed " << localIterations << " of max. " << maxIters << " Iterations.");
            }

            // The Result of this SCC has to be taken back into the main result vector
            innerIndex = 0;
            for (uint_fast64_t state : scc) {
                x.at(state) = currentX->at(innerIndex);
                ++innerIndex;
            }

            // Since the pointers for swapping in the calculation point to temps they should not be valid anymore
            currentX = nullptr;
            swap = nullptr;

            // As the "number of iterations" of the full method is the maximum of the local iterations, we need to keep
            // track of the maximum.
            if (localIterations > currentMaxLocalIterations) {
                currentMaxLocalIterations = localIterations;
            }
        }

        // std::cout << "Used a total of " << globalIterations << " iterations with a maximum of " << localIterations << " iterations in a single block.\n"

        // Check if the solver converged and issue a warning otherwise.
        if (converged) {
            STORM_LOG_INFO("Iterative solver converged after " << currentMaxLocalIterations << " iterations.");
        } else {
            STORM_LOG_WARN("Iterative solver did not converged after " << currentMaxLocalIterations << " iterations.");
        }

        return converged;
    }
}

template<typename ValueType>
std::vector<std::pair<bool, storm::storage::StateBlock>>
TopologicalCudaMinMaxLinearEquationSolver<ValueType>::getOptimalGroupingFromTopologicalSccDecomposition(
    storm::storage::StronglyConnectedComponentDecomposition<ValueType> const& sccDecomposition, std::vector<uint_fast64_t> const& topologicalSort,
    storm::storage::SparseMatrix<ValueType> const& matrix) const {
    (void)matrix;

    std::vector<std::pair<bool, storm::storage::StateBlock>> result;

#ifdef STORM_HAVE_CUDA
    // 95% to have a bit of padding
    size_t const cudaFreeMemory = static_cast<size_t>(getFreeCudaMemory() * 0.95);
    size_t lastResultIndex = 0;

    std::vector<uint_fast64_t> const& rowGroupIndices = matrix.getRowGroupIndices();

    size_t const gpuSizeOfCompleteSystem = basicValueIteration_mvReduce_uint64_double_calculateMemorySize(
        static_cast<size_t>(matrix.getRowCount()), rowGroupIndices.size(), static_cast<size_t>(matrix.getEntryCount()));
    size_t const gpuSizePerRowGroup = std::max(static_cast<size_t>(gpuSizeOfCompleteSystem / rowGroupIndices.size()), static_cast<size_t>(1));
    size_t const maxRowGroupsPerMemory = cudaFreeMemory / gpuSizePerRowGroup;

    size_t currentSize = 0;
    size_t neededReserveSize = 0;
    size_t startIndex = 0;
    for (size_t i = 0; i < topologicalSort.size(); ++i) {
        storm::storage::StateBlock const& scc = sccDecomposition[topologicalSort[i]];
        size_t const currentSccSize = scc.size();

        uint_fast64_t rowCount = 0;
        uint_fast64_t entryCount = 0;

        for (auto sccIt = scc.cbegin(); sccIt != scc.cend(); ++sccIt) {
            rowCount += matrix.getRowGroupSize(*sccIt);
            entryCount += matrix.getRowGroupEntryCount(*sccIt);
        }

        size_t sccSize =
            basicValueIteration_mvReduce_uint64_double_calculateMemorySize(static_cast<size_t>(rowCount), scc.size(), static_cast<size_t>(entryCount));

        if ((currentSize + sccSize) <= cudaFreeMemory) {
            // There is enough space left in the current group
            neededReserveSize += currentSccSize;
            currentSize += sccSize;
        } else {
            // This would make the last open group to big for the GPU

            if (startIndex < i) {
                if ((startIndex + 1) < i) {
                    // More than one component
                    std::vector<uint_fast64_t> tempGroups;
                    tempGroups.reserve(neededReserveSize);

                    // Copy the first group to make inplace_merge possible
                    storm::storage::StateBlock const& scc_first = sccDecomposition[topologicalSort[startIndex]];
                    tempGroups.insert(tempGroups.cend(), scc_first.cbegin(), scc_first.cend());

                    if (((startIndex + 1) + 80) >= i) {
                        size_t lastSize = 0;
                        for (size_t j = startIndex + 1; j < topologicalSort.size(); ++j) {
                            storm::storage::StateBlock const& scc = sccDecomposition[topologicalSort[j]];
                            lastSize = tempGroups.size();
                            tempGroups.insert(tempGroups.cend(), scc.cbegin(), scc.cend());
                            std::vector<uint_fast64_t>::iterator middleIterator = tempGroups.begin();
                            std::advance(middleIterator, lastSize);
                            std::inplace_merge(tempGroups.begin(), middleIterator, tempGroups.end());
                        }
                    } else {
                        // Use std::sort
                        for (size_t j = startIndex + 1; j < i; ++j) {
                            storm::storage::StateBlock const& scc = sccDecomposition[topologicalSort[j]];
                            tempGroups.insert(tempGroups.cend(), scc.cbegin(), scc.cend());
                        }
                        std::sort(tempGroups.begin(), tempGroups.end());
                    }
                    result.push_back(std::make_pair(true, storm::storage::StateBlock(tempGroups.cbegin(), tempGroups.cend())));
                } else {
                    // Only one group, copy construct.
                    result.push_back(std::make_pair(true, storm::storage::StateBlock(std::move(sccDecomposition[topologicalSort[startIndex]]))));
                }
                ++lastResultIndex;
            }

            if (sccSize <= cudaFreeMemory) {
                currentSize = sccSize;
                neededReserveSize = currentSccSize;
                startIndex = i;
            } else {
                // This group is too big to fit into the CUDA Memory by itself
                result.push_back(std::make_pair(false, storm::storage::StateBlock(std::move(sccDecomposition[topologicalSort[i]]))));
                ++lastResultIndex;

                currentSize = 0;
                neededReserveSize = 0;
                startIndex = i + 1;
            }
        }
    }

    size_t const topologicalSortSize = topologicalSort.size();
    if (startIndex < topologicalSortSize) {
        if ((startIndex + 1) < topologicalSortSize) {
            // More than one component
            std::vector<uint_fast64_t> tempGroups;
            tempGroups.reserve(neededReserveSize);

            // Copy the first group to make inplace_merge possible.
            storm::storage::StateBlock const& scc_first = sccDecomposition[topologicalSort[startIndex]];
            tempGroups.insert(tempGroups.cend(), scc_first.cbegin(), scc_first.cend());

            // For set counts <= 80, in-place merge is faster.
            if (((startIndex + 1) + 80) >= topologicalSortSize) {
                size_t lastSize = 0;
                for (size_t j = startIndex + 1; j < topologicalSort.size(); ++j) {
                    storm::storage::StateBlock const& scc = sccDecomposition[topologicalSort[j]];
                    lastSize = tempGroups.size();
                    tempGroups.insert(tempGroups.cend(), scc.cbegin(), scc.cend());
                    std::vector<uint_fast64_t>::iterator middleIterator = tempGroups.begin();
                    std::advance(middleIterator, lastSize);
                    std::inplace_merge(tempGroups.begin(), middleIterator, tempGroups.end());
                }
            } else {
                // Use std::sort
                for (size_t j = startIndex + 1; j < topologicalSort.size(); ++j) {
                    storm::storage::StateBlock const& scc = sccDecomposition[topologicalSort[j]];
                    tempGroups.insert(tempGroups.cend(), scc.cbegin(), scc.cend());
                }
                std::sort(tempGroups.begin(), tempGroups.end());
            }
            result.push_back(std::make_pair(true, storm::storage::StateBlock(tempGroups.cbegin(), tempGroups.cend())));
        } else {
            // Only one group, copy construct.
            result.push_back(std::make_pair(true, storm::storage::StateBlock(std::move(sccDecomposition[topologicalSort[startIndex]]))));
        }
        ++lastResultIndex;
    }
#else
    for (auto sccIndexIt = topologicalSort.cbegin(); sccIndexIt != topologicalSort.cend(); ++sccIndexIt) {
        storm::storage::StateBlock const& scc = sccDecomposition[*sccIndexIt];
        result.push_back(std::make_pair(false, scc));
    }
#endif
    return result;
}

template<typename ValueType>
TopologicalCudaMinMaxLinearEquationSolverFactory<ValueType>::TopologicalCudaMinMaxLinearEquationSolverFactory(bool trackScheduler) {
    // Intentionally left empty.
}

template<typename ValueType>
std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> TopologicalCudaMinMaxLinearEquationSolverFactory<ValueType>::create(Environment const& env) const {
    STORM_LOG_THROW(env.solver().minMax().getMethod() == MinMaxMethod::Topological, storm::exceptions::InvalidEnvironmentException,
                    "This min max solver does not support the selected technique.");
    return std::make_unique<TopologicalCudaMinMaxLinearEquationSolver<ValueType>>();
}

// Explicitly instantiate the solver.
template class TopologicalCudaMinMaxLinearEquationSolver<double>;

template class TopologicalCudaMinMaxLinearEquationSolverFactory<double>;
}  // namespace solver
}  // namespace storm
