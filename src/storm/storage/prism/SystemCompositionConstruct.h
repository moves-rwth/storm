#ifndef STORM_STORAGE_PRISM_SYSTEMCOMPOSITIONCONSTRUCT_H_
#define STORM_STORAGE_PRISM_SYSTEMCOMPOSITIONCONSTRUCT_H_

#include <memory>
#include <string>

#include "storm/storage/prism/LocatedInformation.h"
#include "storm/utility/OsDetection.h"

#include "storm/storage/prism/Composition.h"

namespace storm {
namespace prism {
class SystemCompositionConstruct : public LocatedInformation {
   public:
    /*!
     * Creates an system composition construct with the given composition.
     *
     * @param composition A composition expression defining the system composition.
     * @param filename The filename in which the command is defined.
     * @param lineNumber The line number in which the command is defined.
     */
    SystemCompositionConstruct(std::shared_ptr<Composition> const& composition, std::string const& filename = "", uint_fast64_t lineNumber = 0);

    // Create default implementations of constructors/assignment.
    SystemCompositionConstruct() = default;
    SystemCompositionConstruct(SystemCompositionConstruct const& other) = default;
    SystemCompositionConstruct& operator=(SystemCompositionConstruct const& other) = default;
    SystemCompositionConstruct(SystemCompositionConstruct&& other) = default;
    SystemCompositionConstruct& operator=(SystemCompositionConstruct&& other) = default;

    Composition const& getSystemComposition() const;

    friend std::ostream& operator<<(std::ostream& stream, SystemCompositionConstruct const& systemCompositionConstruct);

   private:
    std::shared_ptr<Composition> composition;
};
}  // namespace prism
}  // namespace storm

#endif /* STORM_STORAGE_PRISM_SYSTEMCOMPOSITIONCONSTRUCT_H_ */
