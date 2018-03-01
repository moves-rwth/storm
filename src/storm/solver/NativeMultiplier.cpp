#include <storm/exceptions/InvalidTypeException.h>
#include "storm/solver/NativeMultiplier.h"

#include "storm-config.h"

#include "storm/environment/solver/MultiplierEnvironment.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/storage/SparseMatrix.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType, typename IndexType>
        NativeMultiplier<ValueType, IndexType>::NativeMultiplier(storm::storage::SparseMatrix<ValueType> const& matrix) : Multiplier<ValueType>(matrix), numRows(0) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::initialize() const {
            if (numRows == 0) {
                numRows = this->matrix.getRowCount();
                entries.clear();
                columns.clear();
                rowIndications.clear();
                entries.reserve(this->matrix.getNonzeroEntryCount());
                columns.reserve(this->matrix.getColumnCount());
                rowIndications.reserve(numRows + 1);
                
                rowIndications.push_back(0);
                for (IndexType r = 0; r < numRows; ++r) {
                    for (auto const& entry : this->matrix.getRow(r)) {
                        entries.push_back(entry.getValue());
                        columns.push_back(entry.getColumn());
                    }
                    rowIndications.push_back(entries.size());
                }
            }
        }
        
        template<typename ValueType, typename IndexType>
        bool NativeMultiplier<ValueType, IndexType>::parallelize(Environment const& env) const {
#ifdef STORM_HAVE_INTELTBB
            return storm::settings::getModule<storm::settings::modules::CoreSettings>().isUseIntelTbbSet();
#else
            return false;
#endif
        }
        
        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::multiply(Environment const& env, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
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
        
        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::multiplyGaussSeidel(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const* b) const {
            initialize();
            // multiply the rows in backwards order
            IndexType r = numRows;
            if (b) {
                std::vector<ValueType> const& bRef = *b;
                while (r > 0) {
                    --r;
                    ValueType xr = bRef[r];
                    this->multiplyRow(r, x, xr);
                    x[r] = std::move(xr);
                }
            } else {
                while (r > 0) {
                    --r;
                    ValueType xr = storm::utility::zero<ValueType>();
                    this->multiplyRow(r, x, xr);
                    x[r] = std::move(xr);
                }
            }
        }
        
        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::multiplyAndReduce(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices) const {
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
                multAddReduce(dir, rowGroupIndices, x, b, *target, choices);
            }
            if (&x == &result) {
                std::swap(result, *this->cachedVector);
            }
        }
        
        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::multiplyAndReduceGaussSeidel(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices) const {
            initialize();
            assert(rowGroupIndices.size() - 1 == x.size());
            // multiply the rowgroups in backwards order
            IndexType g = x.size();
            if (choices) {
                while (g > 0) {
                    --g;
                    x[g] = multiplyAndReduceRowGroup(dir, rowGroupIndices[g], rowGroupIndices[g + 1], x, b, &((*choices)[g]));
                }
            } else {
                while (g > 0) {
                    --g;
                    x[g] = multiplyAndReduceRowGroup(dir, rowGroupIndices[g], rowGroupIndices[g + 1], x, b);
                }
            }
        }
        
        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::multiplyRow(uint64_t const& rowIndex, std::vector<ValueType> const& x, ValueType& value) const {
            initialize();
            assert(rowIndex < numRows);
            IndexType const& rowStart = rowIndications[rowIndex];
            IndexType const& rowEnd = rowIndications[rowIndex + 1];
            for (IndexType e = rowStart; e < rowEnd; ++e) {
                value += entries[e] * x[columns[e]];
            }
        }
        
        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::multiplyRow2(uint64_t const& rowIndex, std::vector<ValueType> const& x1, ValueType& val1, std::vector<ValueType> const& x2, ValueType& val2) const {
            initialize();
            assert(rowIndex < numRows);
            IndexType const& rowStart = rowIndications[rowIndex];
            IndexType const& rowEnd = rowIndications[rowIndex + 1];
            for (IndexType e = rowStart; e < rowEnd; ++e) {
                val1 += entries[e] * x1[columns[e]];
                val2 += entries[e] * x2[columns[e]];
            }
        }
        
        template<typename ValueType, typename IndexType>
        ValueType NativeMultiplier<ValueType, IndexType>::multiplyAndReduceRowGroup(OptimizationDirection const& dir, IndexType const& groupStart, IndexType const& groupEnd, std::vector<ValueType> const& x, std::vector<ValueType> const* b, uint_fast64_t* choice) const {
            // Compute value for first row
            ValueType result = b ? (*b)[groupStart] : storm::utility::zero<ValueType>();
            multiplyRow(groupStart, x, result);
            if (choice) {
                *choice = 0;
                // Compute the value for the remaining rows. Keep track of the optimal choice
                for (IndexType r = groupStart + 1; r < groupEnd; ++r) {
                    ValueType rowVal = b ? (*b)[r] : storm::utility::zero<ValueType>();
                    multiplyRow(r, x, rowVal);
                    if (dir == OptimizationDirection::Minimize) {
                        if (rowVal < result) {
                            result = std::move(rowVal);
                            *choice = r - groupStart;
                        }
                    } else {
                        if (rowVal > result) {
                            result = std::move(rowVal);
                            *choice = r - groupStart;
                        }
                    }
                }
            } else {
                // Compute the value for the remaining rows
                for (IndexType r = groupStart + 1; r < groupEnd; ++r) {
                    ValueType rowVal = b ? (*b)[r] : storm::utility::zero<ValueType>();
                    multiplyRow(r, x, rowVal);
                    if (dir == OptimizationDirection::Minimize) {
                        if (rowVal < result) {
                            result = std::move(rowVal);
                        }
                    } else {
                        if (rowVal > result) {
                            result = std::move(rowVal);
                        }
                    }
                }
            }
            return result;
        }
        
        template<>
        storm::RationalFunction NativeMultiplier<storm::RationalFunction, uint32_t>::multiplyAndReduceRowGroup(OptimizationDirection const&, uint32_t const&, uint32_t const&, std::vector<storm::RationalFunction> const&, std::vector<storm::RationalFunction> const*, uint_fast64_t*) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "This operation is not supported with the given value type.");
            return storm::utility::zero<storm::RationalFunction>();
        }

        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::multAdd(std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            // multiply the rows sequentially (in forward order)
            if (b) {
                std::vector<ValueType> const& bRef = *b;
                for (IndexType r = 0; r < numRows; ++r) {
                    ValueType xr = bRef[r];
                    this->multiplyRow(r, x, xr);
                    result[r] = std::move(xr);
                }
            } else {
                for (IndexType r = 0; r < numRows; ++r) {
                    ValueType xr = storm::utility::zero<ValueType>();
                    this->multiplyRow(r, x, xr);
                    result[r] = std::move(xr);
                }
            }
        }
        
        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::multAddReduce(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices) const {
            assert(rowGroupIndices.size() - 1 == x.size());
            // multiply the rowgroups in forward order
            if (choices) {
                for (IndexType g = 0, groupsEnd = x.size(); g < groupsEnd; ++g) {
                    result[g] = multiplyAndReduceRowGroup(dir, rowGroupIndices[g], rowGroupIndices[g + 1], x, b, &((*choices)[g]));
                }
            } else {
                for (IndexType g = 0, groupsEnd = x.size(); g < groupsEnd; ++g) {
                    result[g] = multiplyAndReduceRowGroup(dir, rowGroupIndices[g], rowGroupIndices[g + 1], x, b);
                }
            }
        }
        
        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::multAddParallel(std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
#ifdef STORM_HAVE_INTELTBB
            this->matrix.multiplyWithVectorParallel(x, result, b);
#else
            STORM_LOG_WARN("Storm was built without support for Intel TBB, defaulting to sequential version.");
            multAdd(x, b, result);
#endif
        }
                
        template<typename ValueType, typename IndexType>
        void NativeMultiplier<ValueType, IndexType>::multAddReduceParallel(storm::solver::OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType> const& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint64_t>* choices) const {
#ifdef STORM_HAVE_INTELTBB
            this->matrix.multiplyAndReduceParallel(dir, rowGroupIndices, x, b, result, choices);
#else
            STORM_LOG_WARN("Storm was built without support for Intel TBB, defaulting to sequential version.");
            multAddReduce(dir, rowGroupIndices, x, b, result, choices);
#endif
        }

        template class NativeMultiplier<double, uint32_t>;
#ifdef STORM_HAVE_CARL
        template class NativeMultiplier<storm::RationalNumber, uint32_t>;
        template class NativeMultiplier<storm::RationalFunction, uint32_t>;
#endif
        
    }
}
