#include <vector>
#include <cstdint>

namespace storm {
    namespace utility {
        namespace numerical {

            template<typename ValueType>
            struct FoxGlynnResult {
                FoxGlynnResult();

                uint64_t left;
                uint64_t right;
                ValueType totalWeight;
                std::vector<ValueType> weights;
            };
            
            template<typename ValueType>
            FoxGlynnResult<ValueType> foxGlynn(ValueType lambda, ValueType epsilon);
                
        }
    }
}
