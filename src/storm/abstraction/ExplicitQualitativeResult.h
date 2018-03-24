#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/QualitativeResult.h"

namespace storm {
    namespace storage {
        class BitVector;
    }
    
    namespace abstraction {
        
        class ExplicitQualitativeResult : public QualitativeResult {
        public:
            virtual ~ExplicitQualitativeResult() = default;
            
            virtual storm::storage::BitVector const& getStates() const = 0;
        };
        
    }
}



