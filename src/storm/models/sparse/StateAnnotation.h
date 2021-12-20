#ifndef STORM_MODELS_SPARSE_STATEANNOTATION_H_
#define STORM_MODELS_SPARSE_STATEANNOTATION_H_

#include "storm/storage/sparse/StateType.h"

namespace storm {
namespace models {
namespace sparse {

class StateAnnotation {
   public:
    virtual ~StateAnnotation() = default;

    virtual std::string getStateInfo(storm::storage::sparse::state_type const& state) const = 0;
};

}  // namespace sparse
}  // namespace models
}  // namespace storm

#endif /* STORM_MODELS_SPARSE_STATEANNOTATION_H_ */
