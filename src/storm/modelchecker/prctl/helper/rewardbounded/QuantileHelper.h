#pragma once

#include "storm/logic/QuantileFormula.h"

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

                    std::vector<std::vector<ValueType>> computeQuantile(Environment const& env);

                private:
                    
                    /*!
                     * Computes the infimum over bound values in minimizing dimensions / the supremum over bound values in maximizing dimensions
                     * @param minimizingDimensions marks dimensions which should be minimized. The remaining dimensions are either not open or maximizing.
                     */
                    ValueType computeExtremalValue(Environment const& env, storm::storage::BitVector const& minimizingDimensions) const;
                    
                    
                    /// Computes the quantile with respect to the given dimension
                    std::pair<uint64_t, typename ModelType::ValueType> computeQuantileForDimension(Environment const& env, uint64_t dim) const;
                    
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

                    ModelType const& model;
                    storm::logic::QuantileFormula const& quantileFormula;
                };
            }
        }
    }
}
