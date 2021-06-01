#pragma once

#include "DFTBE.h"

#include <map>

namespace storm {
    namespace storage {

        /*!
         * BE where the failure distribution is defined by samples.
         * A sample defines the unreliability at a time point (i.e. the cumulative distribution function F(x)).
         */
        template<typename ValueType>
        class BESamples : public DFTBE<ValueType> {

        public:
            /*!
             * Constructor.
             * @param id Id.
             * @param name Name.
             * @param activeSamples Samples defining unreliability in active state for certain time points.
             */
            BESamples(size_t id, std::string const& name, std::map<ValueType, ValueType> activeSamples) :
                    DFTBE<ValueType>(id, name), mActiveSamples(activeSamples) {
                STORM_LOG_ASSERT(activeSamples.size() > 0, "At least one sample should be given.");
                STORM_LOG_ASSERT(this->canFail(), "At least one sample should have a non-zero probability.");
            }

            BEType beType() const override {
                return BEType::SAMPLES;
            }

            /*!
             * Return samples defining unreliability in active state.
             * @return Samples for active state.
             */
            std::map<ValueType, ValueType> const& activeSamples() const {
                return mActiveSamples;
            }

            ValueType getUnreliability(ValueType time) const override;

            bool canFail() const override {
                // At least one sample is not zero
                for (auto const& sample : mActiveSamples) {
                    if (!storm::utility::isZero(sample.second)) {
                        return true;
                    }
                }
                return true;
            }

            bool isTypeEqualTo(DFTElement<ValueType> const& other) const override {
                if (!DFTBE<ValueType>::isTypeEqualTo(other)) {
                    return false;
                }

                auto& otherBE = static_cast<BESamples<ValueType> const&>(other);                
                return mActiveSamples.size() == otherBE.activeSamples().size() && std::equal(mActiveSamples.begin(), mActiveSamples.end(), otherBE.activeSamples().begin());
            }

            std::string toString() const override {
                std::stringstream stream;
                stream << "{" << this->name() << "} BE samples(" << this->activeSamples().size() << " samples)";
                return stream.str();
            }

        private:
             std::map<ValueType, ValueType> mActiveSamples;
        };

    }
}
