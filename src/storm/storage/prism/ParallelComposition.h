#ifndef STORM_STORAGE_PRISM_PARALLELCOMPOSITION_H_
#define STORM_STORAGE_PRISM_PARALLELCOMPOSITION_H_

#include <memory>
#include <set>
#include <string>

#include "storm/storage/prism/Composition.h"

namespace storm {
namespace prism {
class ParallelComposition : public Composition {
   public:
    ParallelComposition(std::shared_ptr<Composition> const& left, std::shared_ptr<Composition> const& right);

    Composition const& getLeftSubcomposition() const;
    Composition const& getRightSubcomposition() const;

   private:
    std::shared_ptr<Composition> left;
    std::shared_ptr<Composition> right;
};
}  // namespace prism
}  // namespace storm

#endif /* STORM_STORAGE_PRISM_PARALLELCOMPOSITION_H_ */
