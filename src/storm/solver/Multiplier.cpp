#include "storm/solver/Multiplier.h"

#include "storm-config.h"

#include "storm/storage/SparseMatrix.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/utility/macros.h"
#include "storm/solver/SolverSelectionOptions.h"
#include "storm/solver/NativeMultiplier.h"
#include "storm/environment/solver/MultiplierEnvironment.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        Multiplier<ValueType>::Multiplier(storm::storage::SparseMatrix<ValueType> const& matrix) : matrix(matrix), allowGaussSeidelMultiplications(false) {
            // Intentionally left empty.
        }
    
        template<typename ValueType>
        bool Multiplier<ValueType>::getAllowGaussSeidelMultiplications() const {
            return allowGaussSeidelMultiplications;
        }
    
        template<typename ValueType>
        void Multiplier<ValueType>::setAllowGaussSeidelMultiplications(bool value) {
            allowGaussSeidelMultiplications = value;
        }
        
        template<typename ValueType>
        void Multiplier<ValueType>::clearCache() const {
            cachedVector.reset();
        }
    
        template<typename ValueType>
        void Multiplier<ValueType>::repeatedMultiply(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint64_t n) const {
            for (uint64_t i = 0; i < n; ++i) {
                multiply(env, x, b, x);
            }
        }
    
        template<typename ValueType>
        void Multiplier<ValueType>::repeatedMultiplyAndReduce(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint64_t n) const {
            for (uint64_t i = 0; i < n; ++i) {
                multiplyAndReduce(env, dir, rowGroupIndices, x, b, x);
            }
        }
    
        template<typename ValueType>
        std::unique_ptr<Multiplier<ValueType>> MultiplierFactory<ValueType>::create(Environment const& env, storm::storage::SparseMatrix<ValueType> const& matrix) {
            switch (env.solver().multiplier().getType()) {
                case MultiplierType::Gmmxx:
                    //return std::make_unique<GmmxxMultiplier<ValueType>>(matrix);
                STORM_PRINT_AND_LOG("gmm mult not yet supported");
                case MultiplierType::Native:
                    return std::make_unique<NativeMultiplier<ValueType>>(matrix);
            }
        }

        
        template class Multiplier<double>;
        template class MultiplierFactory<double>;
        
#ifdef STORM_HAVE_CARL
        template class Multiplier<storm::RationalNumber>;
        template class MultiplierFactory<storm::RationalNumber>;
        template class Multiplier<storm::RationalFunction>;
        template class MultiplierFactory<storm::RationalFunction>;
#endif
        
    }
}
