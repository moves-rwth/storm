#ifndef STORM_GENERATOR_COMPRESSEDSTATE_H_
#define STORM_GENERATOR_COMPRESSEDSTATE_H_

#include "src/storage/BitVector.h"

namespace storm {
    namespace expressions {
        template<typename ValueType> class ExpressionEvaluator;
    }
    
    namespace generator {
        
        typedef storm::storage::BitVector CompressedState;

        class VariableInformation;
        
        /*!
         * Unpacks the compressed state into the evaluator.
         *
         * @param state The state to unpack.
         * @param variableInformation The information about how the variables are packed with the state.
         * @param evaluator The evaluator into which to load the state.
         */
        template<typename ValueType>
        static void unpackStateIntoEvaluator(CompressedState const& state, VariableInformation const& variableInformation, storm::expressions::ExpressionEvaluator<ValueType>& evaluator);
    }
}

#endif /* STORM_GENERATOR_COMPRESSEDSTATE_H_ */

