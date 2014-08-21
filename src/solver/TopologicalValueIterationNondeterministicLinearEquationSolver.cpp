#include "src/solver/TopologicalValueIterationNondeterministicLinearEquationSolver.h"

#include <utility>

#include "src/settings/Settings.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"
#include "src/models/PseudoModel.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InvalidStateException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

#include "storm-config.h"
#ifdef STORM_HAVE_CUDAFORSTORM
#	include "cudaForStorm.h"
#endif

namespace storm {
    namespace solver {
        
        template<typename ValueType>
		TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>::TopologicalValueIterationNondeterministicLinearEquationSolver() {
			// Get the settings object to customize solving.
			storm::settings::Settings* settings = storm::settings::Settings::getInstance();

			// Get appropriate settings.
			this->maximalNumberOfIterations = settings->getOptionByLongName("maxiter").getArgument(0).getValueAsUnsignedInteger();
			this->precision = settings->getOptionByLongName("precision").getArgument(0).getValueAsDouble();
			this->relative = !settings->isSet("absolute");
        }
        
        template<typename ValueType>
		TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>::TopologicalValueIterationNondeterministicLinearEquationSolver(double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : NativeNondeterministicLinearEquationSolver<ValueType>(precision, maximalNumberOfIterations, relative) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
		NondeterministicLinearEquationSolver<ValueType>* TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>::clone() const {
			return new TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>(*this);
        }

		template<>
		void TopologicalValueIterationNondeterministicLinearEquationSolver<float>::solveEquationSystem(bool minimize, storm::storage::SparseMatrix<float> const& A, std::vector<float>& x, std::vector<float> const& b, std::vector<float>* multiplyResult, std::vector<float>* newX) const {
			// For testing only
			std::cout << "<<< Using CUDA-FLOAT Kernels >>>" << std::endl;
			LOG4CPLUS_INFO(logger, "<<< Using CUDA-FLOAT Kernels >>>");

			// Now, we need to determine the SCCs of the MDP and perform a topological sort.
			std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = A.getRowGroupIndices();
			storm::models::NonDeterministicMatrixBasedPseudoModel<float> pseudoModel(A, nondeterministicChoiceIndices);
			storm::storage::StronglyConnectedComponentDecomposition<float> sccDecomposition(pseudoModel, false, false);

			if (sccDecomposition.size() == 0) {
				LOG4CPLUS_ERROR(logger, "Can not solve given Equation System as the SCC Decomposition returned no SCCs.");
				throw storm::exceptions::IllegalArgumentException() << "Can not solve given Equation System as the SCC Decomposition returned no SCCs.";
			}

			storm::storage::SparseMatrix<float> stronglyConnectedComponentsDependencyGraph = pseudoModel.extractPartitionDependencyGraph(sccDecomposition);
			std::vector<uint_fast64_t> topologicalSort = storm::utility::graph::getTopologicalSort(stronglyConnectedComponentsDependencyGraph);

			// Calculate the optimal distribution of sccs
			std::vector<std::pair<bool, storm::storage::StateBlock>> optimalSccs = this->getOptimalGroupingFromTopologicalSccDecomposition(sccDecomposition, topologicalSort, A);
			LOG4CPLUS_INFO(logger, "Optimized SCC Decomposition, originally " << topologicalSort.size() << " SCCs, optimized to " << optimalSccs.size() << " SCCs.");

			std::vector<float>* currentX = nullptr;
			std::vector<float>* swap = nullptr;
			size_t currentMaxLocalIterations = 0;
			size_t localIterations = 0;
			size_t globalIterations = 0;
			bool converged = true;

			// Iterate over all SCCs of the MDP as specified by the topological sort. This guarantees that an SCC is only
			// solved after all SCCs it depends on have been solved.
			int counter = 0;

			for (auto sccIndexIt = optimalSccs.cbegin(); sccIndexIt != optimalSccs.cend() && converged; ++sccIndexIt) {
				bool const useGpu = sccIndexIt->first;
				storm::storage::StateBlock const& scc = sccIndexIt->second;

				// Generate a sub matrix
				storm::storage::BitVector subMatrixIndices(A.getColumnCount(), scc.cbegin(), scc.cend());
				storm::storage::SparseMatrix<float> sccSubmatrix = A.getSubmatrix(true, subMatrixIndices, subMatrixIndices);
				std::vector<float> sccSubB(sccSubmatrix.getRowCount());
				storm::utility::vector::selectVectorValues<float>(sccSubB, subMatrixIndices, nondeterministicChoiceIndices, b);
				std::vector<float> sccSubX(sccSubmatrix.getColumnCount());
				std::vector<float> sccSubXSwap(sccSubmatrix.getColumnCount());
				std::vector<float> sccMultiplyResult(sccSubmatrix.getRowCount());

				// Prepare the pointers for swapping in the calculation
				currentX = &sccSubX;
				swap = &sccSubXSwap;

				storm::utility::vector::selectVectorValues<float>(sccSubX, subMatrixIndices, x); // x is getCols() large, where as b and multiplyResult are getRows() (nondet. choices times states)
				std::vector<uint_fast64_t> sccSubNondeterministicChoiceIndices(sccSubmatrix.getColumnCount() + 1);
				sccSubNondeterministicChoiceIndices.at(0) = 0;

				// Pre-process all dependent states
				// Remove outgoing transitions and create the ChoiceIndices
				uint_fast64_t innerIndex = 0;
				uint_fast64_t outerIndex = 0;
				for (uint_fast64_t state : scc) {
					// Choice Indices
					sccSubNondeterministicChoiceIndices.at(outerIndex + 1) = sccSubNondeterministicChoiceIndices.at(outerIndex) + (nondeterministicChoiceIndices[state + 1] - nondeterministicChoiceIndices[state]);

					for (auto rowGroupIt = nondeterministicChoiceIndices[state]; rowGroupIt != nondeterministicChoiceIndices[state + 1]; ++rowGroupIt) {
						storm::storage::SparseMatrix<float>::const_rows row = A.getRow(rowGroupIt);
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
#ifdef STORM_HAVE_CUDAFORSTORM
					if (!resetCudaDevice()) {
						LOG4CPLUS_ERROR(logger, "Could not reset CUDA Device, can not use CUDA Equation Solver.");
						throw storm::exceptions::InvalidStateException() << "Could not reset CUDA Device, can not use CUDA Equation Solver.";
					}

					//LOG4CPLUS_INFO(logger, "Device has " << getTotalCudaMemory() << " Bytes of Memory with " << getFreeCudaMemory() << "Bytes free (" << (static_cast<double>(getFreeCudaMemory()) / static_cast<double>(getTotalCudaMemory())) * 100 << "%).");
					//LOG4CPLUS_INFO(logger, "We will allocate " << (sizeof(uint_fast64_t)* sccSubmatrix.rowIndications.size() + sizeof(uint_fast64_t)* sccSubmatrix.columnsAndValues.size() * 2 + sizeof(double)* sccSubX.size() + sizeof(double)* sccSubX.size() + sizeof(double)* sccSubB.size() + sizeof(double)* sccSubB.size() + sizeof(uint_fast64_t)* sccSubNondeterministicChoiceIndices.size()) << " Bytes.");
					//LOG4CPLUS_INFO(logger, "The CUDA Runtime Version is " << getRuntimeCudaVersion());

					bool result = false;
					localIterations = 0;
					if (minimize) {
						result = basicValueIteration_mvReduce_uint64_float_minimize(this->maximalNumberOfIterations, this->precision, this->relative, sccSubmatrix.rowIndications, sccSubmatrix.columnsAndValues, *currentX, sccSubB, sccSubNondeterministicChoiceIndices, localIterations);
					} else {
						result = basicValueIteration_mvReduce_uint64_float_maximize(this->maximalNumberOfIterations, this->precision, this->relative, sccSubmatrix.rowIndications, sccSubmatrix.columnsAndValues, *currentX, sccSubB, sccSubNondeterministicChoiceIndices, localIterations);
					}
					LOG4CPLUS_INFO(logger, "Executed " << localIterations << " of max. " << maximalNumberOfIterations << " Iterations on GPU.");

					if (!result) {
						converged = false;
						LOG4CPLUS_ERROR(logger, "An error occurred in the CUDA Plugin. Can not continue.");
						throw storm::exceptions::InvalidStateException() << "An error occurred in the CUDA Plugin. Can not continue.";
					} else {
						converged = true;
					}

					// As the "number of iterations" of the full method is the maximum of the local iterations, we need to keep
					// track of the maximum.
					if (localIterations > currentMaxLocalIterations) {
						currentMaxLocalIterations = localIterations;
					}

#else
					LOG4CPLUS_ERROR(logger, "The useGpu Flag of a SCC was set, but this version of StoRM does not support CUDA acceleration. Internal Error!");
					throw storm::exceptions::InvalidStateException() << "The useGpu Flag of a SCC was set, but this version of StoRM does not support CUDA acceleration. Internal Error!";
#endif
				} else {
					localIterations = 0;
					converged = false;
					while (!converged && localIterations < this->maximalNumberOfIterations) {
						// Compute x' = A*x + b.
						sccSubmatrix.multiplyWithVector(*currentX, sccMultiplyResult);
						storm::utility::vector::addVectorsInPlace<float>(sccMultiplyResult, sccSubB);

						//A.multiplyWithVector(scc, nondeterministicChoiceIndices, *currentX, multiplyResult);
						//storm::utility::addVectors(scc, nondeterministicChoiceIndices, multiplyResult, b);

						/*
						Versus:
						A.multiplyWithVector(*currentX, *multiplyResult);
						storm::utility::vector::addVectorsInPlace(*multiplyResult, b);
						*/

						// Reduce the vector x' by applying min/max for all non-deterministic choices.
						if (minimize) {
							storm::utility::vector::reduceVectorMin<float>(sccMultiplyResult, *swap, sccSubNondeterministicChoiceIndices);
						} else {
							storm::utility::vector::reduceVectorMax<float>(sccMultiplyResult, *swap, sccSubNondeterministicChoiceIndices);
						}

						// Determine whether the method converged.
						// TODO: It seems that the equalModuloPrecision call that compares all values should have a higher
						// running time. In fact, it is faster. This has to be investigated.
						// converged = storm::utility::equalModuloPrecision(*currentX, *newX, scc, precision, relative);
						converged = storm::utility::vector::equalModuloPrecision<float>(*currentX, *swap, this->precision, this->relative);

						// Update environment variables.
						std::swap(currentX, swap);

						++localIterations;
						++globalIterations;
					}
					LOG4CPLUS_INFO(logger, "Executed " << localIterations << " of max. " << maximalNumberOfIterations << " Iterations.");
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

			// Check if the solver converged and issue a warning otherwise.
			if (converged) {
				LOG4CPLUS_INFO(logger, "Iterative solver converged after " << currentMaxLocalIterations << " iterations.");
			} else {
				LOG4CPLUS_WARN(logger, "Iterative solver did not converged after " << currentMaxLocalIterations << " iterations.");
			}
		}
        
        template<typename ValueType>
		void TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>::solveEquationSystem(bool minimize, storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
			
#ifndef GPU_USE_DOUBLE
			TopologicalValueIterationNondeterministicLinearEquationSolver<float> tvindles(precision, maximalNumberOfIterations, relative);

			storm::storage::SparseMatrix<float> new_A = A.toValueType<float>();
			std::vector<float> new_x = storm::utility::vector::toValueType<float>(x);
			std::vector<float> const new_b = storm::utility::vector::toValueType<float>(b);

			tvindles.solveEquationSystem(minimize, new_A, new_x, new_b, nullptr, nullptr);

			for (size_t i = 0, size = new_x.size(); i < size; ++i) {
				x.at(i) = new_x.at(i);
			}
#else
			// For testing only
			std::cout << "<<< Using CUDA-DOUBLE Kernels >>>" << std::endl;
			LOG4CPLUS_INFO(logger, "<<< Using CUDA-DOUBLE Kernels >>>");

			// Now, we need to determine the SCCs of the MDP and perform a topological sort.
			std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = A.getRowGroupIndices();
			storm::models::NonDeterministicMatrixBasedPseudoModel<ValueType> pseudoModel(A, nondeterministicChoiceIndices);
			storm::storage::StronglyConnectedComponentDecomposition<ValueType> sccDecomposition(pseudoModel, false, false);

			if (sccDecomposition.size() == 0) {
				LOG4CPLUS_ERROR(logger, "Can not solve given Equation System as the SCC Decomposition returned no SCCs.");
				throw storm::exceptions::IllegalArgumentException() << "Can not solve given Equation System as the SCC Decomposition returned no SCCs.";
			}

			storm::storage::SparseMatrix<ValueType> stronglyConnectedComponentsDependencyGraph = pseudoModel.extractPartitionDependencyGraph(sccDecomposition);
			std::vector<uint_fast64_t> topologicalSort = storm::utility::graph::getTopologicalSort(stronglyConnectedComponentsDependencyGraph);

			// Calculate the optimal distribution of sccs
			std::vector<std::pair<bool, storm::storage::StateBlock>> optimalSccs = this->getOptimalGroupingFromTopologicalSccDecomposition(sccDecomposition, topologicalSort, A);
			LOG4CPLUS_INFO(logger, "Optimized SCC Decomposition, originally " << topologicalSort.size() << " SCCs, optimized to " << optimalSccs.size() << " SCCs.");

			std::vector<ValueType>* currentX = nullptr;
			std::vector<ValueType>* swap = nullptr;
			size_t currentMaxLocalIterations = 0;
			size_t localIterations = 0;
			size_t globalIterations = 0;
			bool converged = true;

			// Iterate over all SCCs of the MDP as specified by the topological sort. This guarantees that an SCC is only
			// solved after all SCCs it depends on have been solved.
			int counter = 0;

			for (auto sccIndexIt = optimalSccs.cbegin(); sccIndexIt != optimalSccs.cend() && converged; ++sccIndexIt) {
				bool const useGpu = sccIndexIt->first;
				storm::storage::StateBlock const& scc = sccIndexIt->second;

				// Generate a sub matrix
				storm::storage::BitVector subMatrixIndices(A.getColumnCount(), scc.cbegin(), scc.cend());
				storm::storage::SparseMatrix<ValueType> sccSubmatrix = A.getSubmatrix(true, subMatrixIndices, subMatrixIndices);
				std::vector<ValueType> sccSubB(sccSubmatrix.getRowCount());
				storm::utility::vector::selectVectorValues<ValueType>(sccSubB, subMatrixIndices, nondeterministicChoiceIndices, b);
				std::vector<ValueType> sccSubX(sccSubmatrix.getColumnCount());
				std::vector<ValueType> sccSubXSwap(sccSubmatrix.getColumnCount());
                std::vector<ValueType> sccMultiplyResult(sccSubmatrix.getRowCount());
                
				// Prepare the pointers for swapping in the calculation
				currentX = &sccSubX;
				swap = &sccSubXSwap;

				storm::utility::vector::selectVectorValues<ValueType>(sccSubX, subMatrixIndices, x); // x is getCols() large, where as b and multiplyResult are getRows() (nondet. choices times states)
				std::vector<uint_fast64_t> sccSubNondeterministicChoiceIndices(sccSubmatrix.getColumnCount() + 1);
				sccSubNondeterministicChoiceIndices.at(0) = 0;

				// Pre-process all dependent states
				// Remove outgoing transitions and create the ChoiceIndices
				uint_fast64_t innerIndex = 0;
                uint_fast64_t outerIndex = 0;
				for (uint_fast64_t state: scc) {
					// Choice Indices
					sccSubNondeterministicChoiceIndices.at(outerIndex + 1) = sccSubNondeterministicChoiceIndices.at(outerIndex) + (nondeterministicChoiceIndices[state + 1] - nondeterministicChoiceIndices[state]);

					for (auto rowGroupIt = nondeterministicChoiceIndices[state]; rowGroupIt != nondeterministicChoiceIndices[state + 1]; ++rowGroupIt) {
						typename storm::storage::SparseMatrix<ValueType>::const_rows row = A.getRow(rowGroupIt);
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
#ifdef STORM_HAVE_CUDAFORSTORM
					if (!resetCudaDevice()) {
						LOG4CPLUS_ERROR(logger, "Could not reset CUDA Device, can not use CUDA Equation Solver.");
						throw storm::exceptions::InvalidStateException() << "Could not reset CUDA Device, can not use CUDA Equation Solver.";
					}

					//LOG4CPLUS_INFO(logger, "Device has " << getTotalCudaMemory() << " Bytes of Memory with " << getFreeCudaMemory() << "Bytes free (" << (static_cast<double>(getFreeCudaMemory()) / static_cast<double>(getTotalCudaMemory())) * 100 << "%).");
					//LOG4CPLUS_INFO(logger, "We will allocate " << (sizeof(uint_fast64_t)* sccSubmatrix.rowIndications.size() + sizeof(uint_fast64_t)* sccSubmatrix.columnsAndValues.size() * 2 + sizeof(double)* sccSubX.size() + sizeof(double)* sccSubX.size() + sizeof(double)* sccSubB.size() + sizeof(double)* sccSubB.size() + sizeof(uint_fast64_t)* sccSubNondeterministicChoiceIndices.size()) << " Bytes.");
					//LOG4CPLUS_INFO(logger, "The CUDA Runtime Version is " << getRuntimeCudaVersion());

					bool result = false;
					localIterations = 0;
					if (minimize) {
						result = basicValueIteration_mvReduce_uint64_double_minimize(this->maximalNumberOfIterations, this->precision, this->relative, sccSubmatrix.rowIndications, sccSubmatrix.columnsAndValues, *currentX, sccSubB, sccSubNondeterministicChoiceIndices, localIterations);
					} else {
						result = basicValueIteration_mvReduce_uint64_double_maximize(this->maximalNumberOfIterations, this->precision, this->relative, sccSubmatrix.rowIndications, sccSubmatrix.columnsAndValues, *currentX, sccSubB, sccSubNondeterministicChoiceIndices, localIterations);
					}
					LOG4CPLUS_INFO(logger, "Executed " << localIterations << " of max. " << maximalNumberOfIterations << " Iterations on GPU.");

					if (!result) {
						converged = false;
						LOG4CPLUS_ERROR(logger, "An error occurred in the CUDA Plugin. Can not continue.");
						throw storm::exceptions::InvalidStateException() << "An error occurred in the CUDA Plugin. Can not continue.";
					} else {
						converged = true;
					}

					// As the "number of iterations" of the full method is the maximum of the local iterations, we need to keep
					// track of the maximum.
					if (localIterations > currentMaxLocalIterations) {
						currentMaxLocalIterations = localIterations;
					}

#else
					LOG4CPLUS_ERROR(logger, "The useGpu Flag of a SCC was set, but this version of StoRM does not support CUDA acceleration. Internal Error!");
					throw storm::exceptions::InvalidStateException() << "The useGpu Flag of a SCC was set, but this version of StoRM does not support CUDA acceleration. Internal Error!";
#endif
				} else {
					localIterations = 0;
					converged = false;
					while (!converged && localIterations < this->maximalNumberOfIterations) {
						// Compute x' = A*x + b.
						sccSubmatrix.multiplyWithVector(*currentX, sccMultiplyResult);
						storm::utility::vector::addVectorsInPlace<ValueType>(sccMultiplyResult, sccSubB);

						//A.multiplyWithVector(scc, nondeterministicChoiceIndices, *currentX, multiplyResult);
						//storm::utility::addVectors(scc, nondeterministicChoiceIndices, multiplyResult, b);

						/*
						Versus:
						A.multiplyWithVector(*currentX, *multiplyResult);
						storm::utility::vector::addVectorsInPlace(*multiplyResult, b);
						*/

						// Reduce the vector x' by applying min/max for all non-deterministic choices.
						if (minimize) {
							storm::utility::vector::reduceVectorMin<ValueType>(sccMultiplyResult, *swap, sccSubNondeterministicChoiceIndices);
						}
						else {
							storm::utility::vector::reduceVectorMax<ValueType>(sccMultiplyResult, *swap, sccSubNondeterministicChoiceIndices);
						}

						// Determine whether the method converged.
						// TODO: It seems that the equalModuloPrecision call that compares all values should have a higher
						// running time. In fact, it is faster. This has to be investigated.
						// converged = storm::utility::equalModuloPrecision(*currentX, *newX, scc, precision, relative);
						converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *swap, this->precision, this->relative);

						// Update environment variables.
						std::swap(currentX, swap);

						++localIterations;
						++globalIterations;
					}
					LOG4CPLUS_INFO(logger, "Executed " << localIterations << " of max. " << maximalNumberOfIterations << " Iterations.");
				}


				// The Result of this SCC has to be taken back into the main result vector
				innerIndex = 0;
				for (uint_fast64_t state: scc) {
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

			// Check if the solver converged and issue a warning otherwise.
			if (converged) {
				LOG4CPLUS_INFO(logger, "Iterative solver converged after " << currentMaxLocalIterations << " iterations.");
			}
			else {
				LOG4CPLUS_WARN(logger, "Iterative solver did not converged after " << currentMaxLocalIterations << " iterations.");
			}
#endif
        }

		template<typename ValueType>
		std::vector<std::pair<bool, storm::storage::StateBlock>>
			TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>::getOptimalGroupingFromTopologicalSccDecomposition(storm::storage::StronglyConnectedComponentDecomposition<ValueType> const& sccDecomposition, std::vector<uint_fast64_t> const& topologicalSort, storm::storage::SparseMatrix<ValueType> const& matrix) const {
				std::vector<std::pair<bool, storm::storage::StateBlock>> result;
#ifdef STORM_HAVE_CUDAFORSTORM
				// 95% to have a bit of padding
				size_t const cudaFreeMemory = static_cast<size_t>(getFreeCudaMemory() * 0.95);
				size_t lastResultIndex = 0;

				std::vector<uint_fast64_t> const& rowGroupIndices = matrix.getRowGroupIndices();

				size_t const gpuSizeOfCompleteSystem = basicValueIteration_mvReduce_uint64_double_calculateMemorySize(static_cast<size_t>(matrix.getRowCount()), rowGroupIndices.size(), static_cast<size_t>(matrix.getEntryCount()));
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

					size_t sccSize = basicValueIteration_mvReduce_uint64_double_calculateMemorySize(static_cast<size_t>(rowCount), scc.size(), static_cast<size_t>(entryCount));

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
								result.push_back(std::make_pair(true, storm::storage::StateBlock(boost::container::ordered_unique_range, tempGroups.cbegin(), tempGroups.cend())));
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
						
						// Copy the first group to make inplace_merge possible
						storm::storage::StateBlock const& scc_first = sccDecomposition[topologicalSort[startIndex]];
						tempGroups.insert(tempGroups.cend(), scc_first.cbegin(), scc_first.cend());

						// For set counts <= 80, Inplace Merge is faster
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
						result.push_back(std::make_pair(true, storm::storage::StateBlock(boost::container::ordered_unique_range, tempGroups.cbegin(), tempGroups.cend())));
					}
					else {
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

        // Explicitly instantiate the solver.
		template class TopologicalValueIterationNondeterministicLinearEquationSolver<double>;
    } // namespace solver
} // namespace storm
