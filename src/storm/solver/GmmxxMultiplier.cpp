#include "storm/solver/GmmxxMultiplier.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/utility/constants.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        GmmxxMultiplier<ValueType>::GmmxxMultiplier(storm::storage::SparseMatrix<ValueType> const& matrix) : Multiplier<ValueType>(matrix) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::initialize() const {
            if (gmmMatrix.nrows() == 0) {
                gmmMatrix = std::move(*storm::adapters::GmmxxAdapter<ValueType>().toGmmxxSparseMatrix(this->matrix));
            }
        }
        
        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::clearCache() const {
            gmmMatrix = gmm::csr_matrix<ValueType>();
            Multiplier<ValueType>::clearCache();
        }
        
        template<typename ValueType>
        bool GmmxxMultiplier<ValueType>::parallelize(Environment const& env) const {
#ifdef STORM_HAVE_INTELTBB
            return storm::settings::getModule<storm::settings::modules::CoreSettings>().isUseIntelTbbSet();
#else
            return false;
#endif
        }
        
        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::multiply(Environment const& env, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            initialize();
            std::vector<ValueType>* target = &result;
            if (&x == &result) {
                if (this->cachedVector) {
                    this->cachedVector->resize(x.size());
                } else {
                    this->cachedVector = std::make_unique<std::vector<ValueType>>(x.size());
                }
                target = this->cachedVector.get();
            }
            if (parallelize(env)) {
                multAddParallel(x, b, *target);
            } else {
                multAdd(x, b, *target);
            }
            if (&x == &result) {
                std::swap(result, *this->cachedVector);
            }
        }
        
        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::multiplyGaussSeidel(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const* b) const {
            initialize();
            STORM_LOG_ASSERT(gmmMatrix.nr == gmmMatrix.nc, "Expecting square matrix.");
            if (b) {
                gmm::mult_add_by_row_bwd(gmmMatrix, x, *b, x, gmm::abstract_dense());
            } else {
                gmm::mult_by_row_bwd(gmmMatrix, x, x, gmm::abstract_dense());
            }
        }
        
        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::multiplyAndReduce(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices) const {
            initialize();
            std::vector<ValueType>* target = &result;
            if (&x == &result) {
                if (this->cachedVector) {
                    this->cachedVector->resize(x.size());
                } else {
                    this->cachedVector = std::make_unique<std::vector<ValueType>>(x.size());
                }
                target = this->cachedVector.get();
            }
            if (parallelize(env)) {
                multAddReduceParallel(dir, rowGroupIndices, x, b, *target, choices);
            } else {
                multAddReduceHelper(dir, rowGroupIndices, x, b, *target, choices);
            }
            if (&x == &result) {
                std::swap(result, *this->cachedVector);
            }
        }
        
        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::multiplyAndReduceGaussSeidel(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices) const {
            initialize();
            multAddReduceHelper(dir, rowGroupIndices, x, b, x, choices);
        }
        
        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::multiplyRow(uint64_t const& rowIndex, std::vector<ValueType> const& x, ValueType& value) const {
            initialize();
            value += vect_sp(gmm::mat_const_row(gmmMatrix, rowIndex), x, typename gmm::linalg_traits<gmm::csr_matrix<ValueType>>::storage_type(), typename gmm::linalg_traits<std::vector<ValueType>>::storage_type());
        }

        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::multAdd(std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            if (b) {
                gmm::mult_add(gmmMatrix, x, *b, result);
            } else {
                gmm::mult(gmmMatrix, x, result);
            }
        }

        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::multAddReduceHelper(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices) const {
            if (dir == storm::OptimizationDirection::Minimize) {
                multAddReduceHelper<storm::utility::ElementLess<ValueType>>(rowGroupIndices, x, b, result, choices);
            } else {
                multAddReduceHelper<storm::utility::ElementGreater<ValueType>>(rowGroupIndices, x, b, result, choices);
            }
        }
        
        template<typename ValueType>
        template<typename Compare>
        void GmmxxMultiplier<ValueType>::multAddReduceHelper(std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices) const {
            Compare compare;
            typedef std::vector<ValueType> VectorType;
            typedef gmm::csr_matrix<ValueType> MatrixType;
            
            typename gmm::linalg_traits<VectorType>::const_iterator add_it, add_ite;
            if (b) {
                add_it = gmm::vect_end(*b) - 1;
                add_ite = gmm::vect_begin(*b) - 1;
            }
            typename gmm::linalg_traits<VectorType>::iterator target_it = gmm::vect_end(result) - 1;
            typename gmm::linalg_traits<MatrixType>::const_row_iterator itr = mat_row_const_end(gmmMatrix) - 1;
            typename std::vector<uint64_t>::iterator choice_it;
            if (choices) {
                choice_it = choices->end() - 1;
            }

            // Variables for correctly tracking choices (only update if new choice is strictly better).
            ValueType oldSelectedChoiceValue;
            uint64_t selectedChoice;

            uint64_t currentRow = gmmMatrix.nrows() - 1;
            for (auto row_group_it = rowGroupIndices.end() - 2, row_group_ite = rowGroupIndices.begin() - 1; row_group_it != row_group_ite; --row_group_it, --choice_it, --target_it) {
                ValueType currentValue = storm::utility::zero<ValueType>();
                
                // Only multiply and reduce if the row group is not empty.
                if (*row_group_it != *(row_group_it + 1)) {
                    if (b) {
                        currentValue = *add_it;
                        --add_it;
                    }
                    
                    currentValue += vect_sp(gmm::linalg_traits<MatrixType>::row(itr), x);

                    if (choices) {
                        selectedChoice = currentRow - *row_group_it;
                        if (*choice_it == selectedChoice) {
                            oldSelectedChoiceValue = currentValue;
                        }
                    }

                    --itr;
                    --currentRow;
                    
                    for (uint64_t row = *row_group_it + 1, rowEnd = *(row_group_it + 1); row < rowEnd; ++row, --currentRow, --itr, --add_it) {
                        ValueType newValue = b ? *add_it : storm::utility::zero<ValueType>();
                        newValue += vect_sp(gmm::linalg_traits<MatrixType>::row(itr), x);
                        
                        if (choices && currentRow == *choice_it + *row_group_it) {
                            oldSelectedChoiceValue = newValue;
                        }

                        if (compare(newValue, currentValue)) {
                            currentValue = newValue;
                            if (choices) {
                                selectedChoice = currentRow - *row_group_it;
                            }
                        }
                    }
                    
                    // Finally write value to target vector.
                    *target_it = currentValue;
                    if (choices && compare(currentValue, oldSelectedChoiceValue)) {
                        *choice_it = selectedChoice;
                    }
                }
            }
        }
        
        template<>
        template<typename Compare>
        void GmmxxMultiplier<storm::RationalFunction>::multAddReduceHelper(std::vector<uint64_t> const& rowGroupIndices, std::vector<storm::RationalFunction> const& x, std::vector<storm::RationalFunction> const* b, std::vector<storm::RationalFunction>& result, std::vector<uint64_t>* choices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation not supported for this data type.");
        }
        
        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::multAddParallel(std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
#ifdef STORM_HAVE_INTELTBB
            if (b) {
                gmm::mult_add_parallel(gmmMatrix, x, *b, result);
            } else {
                gmm::mult_parallel(gmmMatrix, x, result);
            }
#else
            STORM_LOG_WARN("Storm was built without support for Intel TBB, defaulting to sequential version.");
            multAdd(x, b, result);
#endif
        }
        
#ifdef STORM_HAVE_INTELTBB
        template<typename ValueType, typename Compare>
        class TbbMultAddReduceFunctor {
        public:
            TbbMultAddReduceFunctor(std::vector<uint64_t> const& rowGroupIndices, gmm::csr_matrix<ValueType> const& matrix, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices) : dir(dir), rowGroupIndices(rowGroupIndices), matrix(matrix), x(x), b(b), result(result), choices(choices) {
                // Intentionally left empty.
            }
            
            void operator()(tbb::blocked_range<unsigned long> const& range) const {
                typedef std::vector<ValueType> VectorType;
                typedef gmm::csr_matrix<ValueType> MatrixType;

                bool min = dir == OptimizationDirection::Minimize;
                auto groupIt = rowGroupIndices.begin() + range.begin();
                auto groupIte = rowGroupIndices.begin() + range.end();
                
                auto itr = mat_row_const_begin(matrix) + *groupIt;
                typename std::vector<ValueType>::const_iterator bIt;
                if (b) {
                    bIt = b->begin() + *groupIt;
                }
                typename std::vector<uint64_t>::iterator choiceIt;
                if (choices) {
                    choiceIt = choices->begin() + range.begin();
                }
                
                auto resultIt = result.begin() + range.begin();
                
                // Variables for correctly tracking choices (only update if new choice is strictly better).
                ValueType oldSelectedChoiceValue;
                uint64_t selectedChoice;
                
                uint64_t currentRow = *groupIt;
                for (; groupIt != groupIte; ++groupIt, ++resultIt, ++choiceIt) {
                    ValueType currentValue = storm::utility::zero<ValueType>();
                    
                    // Only multiply and reduce if the row group is not empty.
                    if (*groupIt != *(groupIt + 1)) {
                        if (b) {
                            currentValue = *bIt;
                            ++bIt;
                        }
                        
                        currentValue += vect_sp(gmm::linalg_traits<MatrixType>::row(itr), x);
                        
                        if (choices) {
                            selectedChoice = currentRow - *groupIt;
                            if (*choiceIt == selectedChoice) {
                                oldSelectedChoiceValue = currentValue;
                            }
                        }
                        
                        ++itr;
                        ++currentRow;
                        
                        for (auto itre = mat_row_const_begin(matrix) + *(groupIt + 1); itr != itre; ++itr, ++bIt, ++currentRow) {
                            ValueType newValue = b ? *bIt : storm::utility::zero<ValueType>();
                            newValue += vect_sp(gmm::linalg_traits<MatrixType>::row(itr), x);
                            
                            if (compare(newValue, currentValue)) {
                                currentValue = newValue;
                                if (choices) {
                                    selectedChoice = currentRow - *groupIt;
                                }
                            }
                        }
                    }
                    
                    // Finally write value to target vector.
                    *resultIt = currentValue;
                    if (choices && compare(currentValue, oldSelectedChoiceValue)) {
                        *choiceIt = selectedChoice;
                    }
                }
            }
            
        private:
            Compare compare;
            std::vector<uint64_t> const& rowGroupIndices;
            gmm::csr_matrix<ValueType> const& matrix;
            std::vector<ValueType> const& x;
            std::vector<ValueType> const* b;
            std::vector<ValueType>& result;
            std::vector<uint64_t>* choices;
        };
#endif
        
        template<typename ValueType>
        void GmmxxMultiplier<ValueType>::multAddReduceParallel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices) const {
#ifdef STORM_HAVE_INTELTBB
            if (dir == storm::OptimizationDirection::Minimize) {
                tbb::parallel_for(tbb::blocked_range<unsigned long>(0, rowGroupIndices.size() - 1, 100), TbbMultAddReduceFunctor<ValueType, storm::utility::ElementLess<ValueType>>(rowGroupIndices, this->gmmMatrix, x, b, result, choices));
            } else {
                tbb::parallel_for(tbb::blocked_range<unsigned long>(0, rowGroupIndices.size() - 1, 100), TbbMultAddReduceFunctor<ValueType storm::utility::ElementGreater<ValueType>>(rowGroupIndices, this->gmmMatrix, x, b, result, choices));
            }
#else
            STORM_LOG_WARN("Storm was built without support for Intel TBB, defaulting to sequential version.");
            multAddReduceHelper(dir, rowGroupIndices, x, b, result, choices);
#endif
        }
        
        template<>
        void GmmxxMultiplier<storm::RationalFunction>::multAddReduceParallel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<storm::RationalFunction> const& x, std::vector<storm::RationalFunction> const* b, std::vector<storm::RationalFunction>& result, std::vector<uint64_t>* choices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This operation is not supported.");
        }
        
        template class GmmxxMultiplier<double>;
        
#ifdef STORM_HAVE_CARL
        template class GmmxxMultiplier<storm::RationalNumber>;
        template class GmmxxMultiplier<storm::RationalFunction>;
#endif
        
    }
}
