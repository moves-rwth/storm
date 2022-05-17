#pragma once

#include "DFTBE.h"

namespace storm::dft {
namespace storage {
namespace elements {

/*!
 * BE which is either constant failed or constant failsafe.
 * The BE is either always failed (from the beginning) or can never fail (failsafe).
 */
template<typename ValueType>
class BEConst : public DFTBE<ValueType> {
   public:
    /*!
     * Constructor.
     * @param id Id.
     * @param name Name.
     * @param failed True iff the const BE is failed, otherwise it is failsafe.
     */
    BEConst(size_t id, std::string const& name, bool failed) : DFTBE<ValueType>(id, name), mFailed(failed) {
        // Intentionally empty
    }

    storm::dft::storage::elements::BEType beType() const override {
        return storm::dft::storage::elements::BEType::CONSTANT;
    }

    /*!
     * Return whether the BE has failed.
     * @return True iff the BE is const failed.
     */
    bool failed() const {
        return mFailed;
    }

    bool canFail() const override {
        return this->failed();
    }

    ValueType getUnreliability(ValueType time) const override;

    bool isTypeEqualTo(DFTElement<ValueType> const& other) const override {
        if (!DFTBE<ValueType>::isTypeEqualTo(other)) {
            return false;
        }
        auto& otherBE = static_cast<BEConst<ValueType> const&>(other);
        return this->failed() == otherBE.failed();
    }

    std::string toString() const override {
        std::stringstream stream;
        stream << "{" << this->name() << "} BE(const " << (this->failed() ? "failed" : "failsafe") << ")";
        return stream.str();
    }

   private:
    bool mFailed;
};

}  // namespace elements
}  // namespace storage
}  // namespace storm::dft
