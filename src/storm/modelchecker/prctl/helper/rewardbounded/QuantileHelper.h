#pragma once

#include "storm/logic/QuantileFormula.h"
#include "boost/optional.hpp"

namespace storm {
    class Environment;

    namespace storage {
        class BitVector;
    }

    namespace modelchecker {
        namespace helper {
            namespace rewardbounded {
                
                template<typename ModelType>
                class QuantileHelper {
                    typedef typename ModelType::ValueType ValueType;
                public:
                    QuantileHelper(ModelType const& model, storm::logic::QuantileFormula const& quantileFormula);

                    std::vector<std::vector<ValueType>> computeQuantile(Environment const& env) const;

                private:
                    std::vector<std::vector<ValueType>> computeTwoDimensionalQuantile(Environment& env) const;
                    bool exploreTwoDimensionalQuantile(Environment const& env, std::vector<std::pair<int64_t, typename ModelType::ValueType>> const& startEpochValues, std::vector<int64_t>& currentEpochValues, std::vector<std::vector<ValueType>>& resultPoints) const;

                    /*!
                     * Computes the limit probability, where the given dimensions approach infinity and the remaining dimensions are set to zero.
                     */
                    ValueType computeLimitValue(Environment const& env, storm::storage::BitVector const& infDimensions) const;

                    /*!
                     * Computes the limit probability, where the given dimensions approach infinity and the remaining dimensions are set to zero.
                     * The computed value is compared to the probability threshold.
                     * In sound mode, precision is iteratively increased in case of 'inconsistent' results.
                     */
                    bool checkLimitValue(Environment& env, storm::storage::BitVector const& infDimensions) const;
                    
                    /// Computes the quantile with respect to the given dimension.
                    /// boost::none is returned in case of insufficient precision.
                    boost::optional<std::pair<uint64_t, typename ModelType::ValueType>> computeQuantileForDimension(Environment const& env, uint64_t dim) const;
                    
                    /*!
                     * Gets the number of dimensions of the underlying boudned until formula
                     */
                    uint64_t getDimension() const;
                    
                    /*!
                     * Gets the dimensions that are open, i.e., for which the bound value is not fixed
                     * @return
                     */
                    storm::storage::BitVector getOpenDimensions() const;
                    
                    storm::storage::BitVector getDimensionsForVariable(storm::expressions::Variable const& var) const;
                    storm::solver::OptimizationDirection const& getOptimizationDirForDimension(uint64_t const& dim) const;
                    storm::expressions::Variable const& getVariableForDimension(uint64_t const& dim) const;

                    ModelType const& model;
                    storm::logic::QuantileFormula const& quantileFormula;
                    
                    /// Statistics
                    mutable uint64_t numCheckedEpochs;
                    mutable uint64_t numPrecisionRefinements;
                };
            }
        }
    }
}
