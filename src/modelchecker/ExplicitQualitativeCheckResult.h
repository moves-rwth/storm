#ifndef STORM_MODELCHECKER_EXPLICITQUALITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_EXPLICITQUALITATIVECHECKRESULT_H_

#include "src/modelchecker/QualitativeCheckResult.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace modelchecker {
        class ExplicitQualitativeCheckResult : public QualitativeCheckResult {
        public:
            /*!
             * Constructs a check result with the provided truth values.
             *
             * @param truthValues The truth values of the result.
             */
            ExplicitQualitativeCheckResult(storm::storage::BitVector const& truthValues) : truthValues(truthValues) {
                // Intentionally left empty.

            }
            
            /*!
             * Constructs a check result with the provided truth values.
             *
             * @param truthValues The truth values of the result.
             */
            ExplicitQualitativeCheckResult(storm::storage::BitVector&& truthValues) : truthValues(std::move(truthValues)) {
                // Intentionally left empty.
            }
            
            ExplicitQualitativeCheckResult(ExplicitQualitativeCheckResult const& other) = default;
            ExplicitQualitativeCheckResult& operator=(ExplicitQualitativeCheckResult const& other) = default;
#ifndef WINDOWS
            ExplicitQualitativeCheckResult(ExplicitQualitativeCheckResult&& other) = default;
            ExplicitQualitativeCheckResult& operator=(ExplicitQualitativeCheckResult&& other) = default;
#endif
            
            bool operator[](uint_fast64_t index) const;
            
            virtual bool isExplicit() const override;
            virtual bool isResultForAllStates() const override;
            
            virtual bool isExplicitQualitativeCheckResult() const override;
            
            virtual CheckResult& operator&=(CheckResult const& other) override;
            virtual CheckResult& operator|=(CheckResult const& other) override;
            virtual void complement() override;

            storm::storage::BitVector const& getTruthValues() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            virtual std::ostream& writeToStream(std::ostream& out, storm::storage::BitVector const& filter) const override;

        private:
            // The values of the quantitative check result.
            storm::storage::BitVector truthValues;
        };
    }
}

#endif /* STORM_MODELCHECKER_EXPLICITQUALITATIVECHECKRESULT_H_ */