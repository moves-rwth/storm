#pragma once

#include <memory>
#include <string>

#include "storm/storage/BoostTypes.h"
#include "storm/storage/sparse/ChoiceOrigins.h"

namespace storm {
namespace jani {
class Model;
}

namespace storage {
namespace sparse {

/*!
 * This class represents for each choice the origin in the jani specification
 * // TODO complete this
 */
class JaniChoiceOrigins : public ChoiceOrigins {
   public:
    typedef storm::storage::FlatSet<uint_fast64_t> EdgeIndexSet;

    /*!
     * Creates a new representation of the choice indices to their origin in the Jani specification
     */
    JaniChoiceOrigins(std::shared_ptr<storm::jani::Model const> const& janiModel, std::vector<uint_fast64_t> const& indexToIdentifierMapping,
                      std::vector<EdgeIndexSet> const& identifierToEdgeIndexSetMapping);

    virtual ~JaniChoiceOrigins() = default;

    virtual bool isJaniChoiceOrigins() const override;

    /*
     * Returns the number of identifiers that are used by this object.
     * This can be used to, e.g., loop over all identifiers.
     */
    virtual uint_fast64_t getNumberOfIdentifiers() const override;

    /*!
     * Retrieves the associated JANI model.
     */
    storm::jani::Model const& getModel() const;

    /*
     * Returns the set of edges that induced the choice with the given index.
     * The edges set is represented by a set of indices that encode an automaton index and its local edge index.
     */
    EdgeIndexSet const& getEdgeIndexSet(uint_fast64_t choiceIndex) const;

    std::size_t hash() const override;

   private:
    /*
     * Returns a copy of this object where the mapping of choice indices to origin identifiers is replaced by the given one.
     */
    virtual std::shared_ptr<ChoiceOrigins> cloneWithNewIndexToIdentifierMapping(std::vector<uint_fast64_t>&& indexToIdentifierMapping) const override;

    /*
     * Computes the identifier infos (i.e., human readable strings representing the choice origins).
     */
    virtual void computeIdentifierInfos() const override;

    /*
     * Computes the identifier infos as json (i.e., a machine readable representation of the choice origins).
     */
    virtual void computeIdentifierJson() const override;

    std::shared_ptr<storm::jani::Model const> model;
    std::vector<EdgeIndexSet> identifierToEdgeIndexSet;
};
}  // namespace sparse
}  // namespace storage
}  // namespace storm
