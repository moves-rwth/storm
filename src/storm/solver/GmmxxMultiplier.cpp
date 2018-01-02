#include "storm/solver/GmmxxMultiplier.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/utility/constants.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace solver {
        
        template<typename T>
        GmmxxMultiplier<T>::GmmxxMultiplier() : storm::utility::VectorHelper<T>() {
            // Intentionally left empty.
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAdd(gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result) const {
            if (this->parallelize()) {
                multAddParallel(matrix, x, b, result);
            } else {
                if (b) {
                    gmm::mult_add(matrix, x, *b, result);
                } else {
                    gmm::mult(matrix, x, result);
                }
            }
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAddGaussSeidelBackward(gmm::csr_matrix<T> const& matrix, std::vector<T>& x, std::vector<T> const* b) const {
            STORM_LOG_ASSERT(matrix.nr == matrix.nc, "Expecting square matrix.");
            if (b) {
                gmm::mult_add_by_row_bwd(matrix, x, *b, x, gmm::abstract_dense());
            } else {
                gmm::mult_by_row_bwd(matrix, x, x, gmm::abstract_dense());
            }
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAddReduce(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices) const {
            std::vector<T>* target = &result;
            std::unique_ptr<std::vector<T>> temporary;
            if (&x == &result) {
                STORM_LOG_WARN("Using temporary in 'multAddReduce'.");
                temporary = std::make_unique<std::vector<T>>(x.size());
                target = temporary.get();
            }
            
            if (this->parallelize()) {
                multAddReduceParallel(dir, rowGroupIndices, matrix, x, b, *target, choices);
            } else {
                multAddReduceHelper(dir, rowGroupIndices, matrix, x, b, *target, choices);
            }
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAddReduceGaussSeidel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T>& x, std::vector<T> const* b, std::vector<uint64_t>* choices) const {
            multAddReduceHelper(dir, rowGroupIndices, matrix, x, b, x, choices);
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAddReduceHelper(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices) const {
            typedef std::vector<T> VectorType;
            typedef gmm::csr_matrix<T> MatrixType;
            
            typename gmm::linalg_traits<VectorType>::const_iterator add_it, add_ite;
            if (b) {
                add_it = gmm::vect_end(*b) - 1;
                add_ite = gmm::vect_begin(*b) - 1;
            }
            typename gmm::linalg_traits<VectorType>::iterator target_it = gmm::vect_end(result) - 1;
            typename gmm::linalg_traits<MatrixType>::const_row_iterator itr = mat_row_const_end(matrix) - 1;
            typename std::vector<uint64_t>::iterator choice_it;
            if (choices) {
                choice_it = choices->end() - 1;
            }
            
            uint64_t choice;
            for (auto row_group_it = rowGroupIndices.end() - 2, row_group_ite = rowGroupIndices.begin() - 1; row_group_it != row_group_ite; --row_group_it, --choice_it, --target_it) {
                if (choices) {
                    *choice_it = 0;
                }

                T currentValue = storm::utility::zero<T>();
                
                // Only multiply and reduce if the row group is not empty.
                if (*row_group_it != *(row_group_it + 1)) {
                    if (b) {
                        currentValue = *add_it;
                        --add_it;
                    }
                    
                    currentValue += vect_sp(gmm::linalg_traits<MatrixType>::row(itr), x);
                    
                    if (choices) {
                        choice = *(row_group_it + 1) - 1 - *row_group_it;
                        *choice_it = choice;
                    }
                    
                    --itr;
                    
                    for (uint64_t row = *row_group_it + 1, rowEnd = *(row_group_it + 1); row < rowEnd; ++row, --itr, --add_it) {
                        T newValue = b ? *add_it : storm::utility::zero<T>();
                        newValue += vect_sp(gmm::linalg_traits<MatrixType>::row(itr), x);
                        
                        if (choices) {
                            --choice;
                        }
                        
                        if ((dir == OptimizationDirection::Minimize && newValue < currentValue) || (dir == OptimizationDirection::Maximize && newValue > currentValue)) {
                            currentValue = newValue;
                            if (choices) {
                                *choice_it = choice;
                            }
                        }
                    }
                } else if (choices) {
                    *choice_it = 0;
                }
                
                // Write back final value.
                *target_it = currentValue;
            }
        }
        
        template<>
        void GmmxxMultiplier<storm::RationalFunction>::multAddReduceHelper(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<storm::RationalFunction> const& matrix, std::vector<storm::RationalFunction> const& x, std::vector<storm::RationalFunction> const* b, std::vector<storm::RationalFunction>& result, std::vector<uint64_t>* choices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation not supported for this data type.");
        }
        
        template<typename T>
        void GmmxxMultiplier<T>::multAddParallel(gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result) const {
#ifdef STORM_HAVE_INTELTBB
            if (b) {
                gmm::mult_add_parallel(matrix, x, *b, result);
            } else {
                gmm::mult_parallel(matrix, x, result);
            }
#else
            STORM_LOG_WARN("Storm was built without support for Intel TBB, defaulting to sequential version.");
            multAdd(matrix, x, b, result);
#endif
        }
        
#ifdef STORM_HAVE_INTELTBB
        template<typename T>
        class TbbMultAddReduceFunctor {
        public:
            TbbMultAddReduceFunctor(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices) : dir(dir), rowGroupIndices(rowGroupIndices), matrix(matrix), x(x), b(b), result(result), choices(choices) {
                // Intentionally left empty.
            }
            
            void operator()(tbb::blocked_range<unsigned long> const& range) const {
                auto groupIt = rowGroupIndices.begin() + range.begin();
                auto groupIte = rowGroupIndices.begin() + range.end();

                auto itr = mat_row_const_begin(matrix) + *groupIt;
                typename std::vector<T>::const_iterator bIt;
                if (b) {
                    bIt = b->begin() + *groupIt;
                }
                typename std::vector<uint64_t>::iterator choiceIt;
                if (choices) {
                    choiceIt = choices->begin() + range.begin();
                }
                
                auto resultIt = result.begin() + range.begin();

                for (; groupIt != groupIte; ++groupIt, ++resultIt, ++choiceIt) {
                    if (choices) {
                        *choiceIt = 0;
                    }
                    
                    T currentValue = storm::utility::zero<T>();
                    
                    // Only multiply and reduce if the row group is not empty.
                    if (*groupIt != *(groupIt + 1)) {
                        if (b) {
                            currentValue = *bIt;
                            ++bIt;
                        }
                        
                        ++itr;
                        
                        for (auto itre = mat_row_const_begin(matrix) + *(groupIt + 1); itr != itre; ++itr) {
                            T newValue = vect_sp(gmm::linalg_traits<gmm::csr_matrix<T>>::row(itr), x, typename gmm::linalg_traits<gmm::csr_matrix<T>>::storage_type(), typename gmm::linalg_traits<std::vector<T>>::storage_type());
                            if (b) {
                                newValue += *bIt;
                                ++bIt;
                            }
                            
                            if ((dir == OptimizationDirection::Minimize && newValue < currentValue) || (dir == OptimizationDirection::Maximize && newValue > currentValue)) {
                                currentValue = newValue;
                                if (choices) {
                                    *choiceIt = std::distance(mat_row_const_begin(matrix), itr) - *groupIt;
                                }
                            }
                        }
                    }
                    
                    *resultIt = currentValue;
                }
            }
            
        private:
            storm::solver::OptimizationDirection dir;
            std::vector<uint64_t> const& rowGroupIndices;
            gmm::csr_matrix<T> const& matrix;
            std::vector<T> const& x;
            std::vector<T> const* b;
            std::vector<T>& result;
            std::vector<uint64_t>* choices;
        };
#endif
        
        template<typename T>
        void GmmxxMultiplier<T>::multAddReduceParallel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<T> const& matrix, std::vector<T> const& x, std::vector<T> const* b, std::vector<T>& result, std::vector<uint64_t>* choices) const {
#ifdef STORM_HAVE_INTELTBB
            tbb::parallel_for(tbb::blocked_range<unsigned long>(0, rowGroupIndices.size() - 1, 10), TbbMultAddReduceFunctor<T>(dir, rowGroupIndices, matrix, x, b, result, choices));
#else
            STORM_LOG_WARN("Storm was built without support for Intel TBB, defaulting to sequential version.");
            multAddReduce(dir, rowGroupIndices, matrix, x, b, result, choices);
#endif
        }
        
        template<>
        void GmmxxMultiplier<storm::RationalFunction>::multAddReduceParallel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<storm::RationalFunction> const& matrix, std::vector<storm::RationalFunction> const& x, std::vector<storm::RationalFunction> const* b, std::vector<storm::RationalFunction>& result, std::vector<uint64_t>* choices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This operation is not supported.");
        }
        
        template<typename T>
        T GmmxxMultiplier<T>::multiplyRow(gmm::csr_matrix<T> const& matrix, uint64_t const& rowIndex, std::vector<T> const& x) const {
            return vect_sp(gmm::mat_const_row(matrix, rowIndex), x, typename gmm::linalg_traits<gmm::csr_matrix<T>>::storage_type(), typename gmm::linalg_traits<std::vector<T>>::storage_type());
        }

        
        
        
        template class GmmxxMultiplier<double>;
        
#ifdef STORM_HAVE_CARL
        template class GmmxxMultiplier<storm::RationalNumber>;
        template class GmmxxMultiplier<storm::RationalFunction>;
#endif
        
    }
}
