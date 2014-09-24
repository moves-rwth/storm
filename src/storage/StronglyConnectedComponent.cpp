#include "src/storage/StronglyConnectedComponent.h"

namespace storm {
    namespace storage {
        void StronglyConnectedComponent::setIsTrivial(bool trivial) {
            this->isTrivialScc = trivial;
        }
        
        bool StronglyConnectedComponent::isTrivial() const {
            return isTrivialScc;
        }
    }
}