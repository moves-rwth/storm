#include "storm/storage/bisimulation/DeterministicBlockData.h"

#include <iostream>

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/utility/macros.h"

namespace storm {
    namespace storage {
        namespace bisimulation {
            
            DeterministicBlockData::DeterministicBlockData() : DeterministicBlockData(0, 0) {
                // Intentionally left empty.
            }
            
            DeterministicBlockData::DeterministicBlockData(uint_fast64_t marker1, uint_fast64_t marker2) : valMarker1(marker1), valMarker2(marker2), splitterFlag(false), needsRefinementFlag(false), absorbingFlag(false), valRepresentativeState() {
                // Intentionally left empty.
            }
            
            uint_fast64_t DeterministicBlockData::marker1() const {
                return valMarker1;
            }
            
            void DeterministicBlockData::setMarker1(uint_fast64_t newMarker1) {
                valMarker1 = newMarker1;
            }
            
            void DeterministicBlockData::incrementMarker1() {
                ++valMarker1;
            }
            
            void DeterministicBlockData::decrementMarker1() {
                --valMarker1;
            }
            
            uint_fast64_t DeterministicBlockData::marker2() const {
                return valMarker2;
            }
            
            void DeterministicBlockData::setMarker2(uint_fast64_t newMarker2) {
                valMarker2 = newMarker2;
            }
            
            void DeterministicBlockData::incrementMarker2() {
                ++valMarker2;
            }
            
            void DeterministicBlockData::decrementMarker2() {
                --valMarker2;
            }
            
            bool DeterministicBlockData::resetMarkers(Block<DeterministicBlockData> const& block) {
                bool result = block.getBeginIndex() != this->valMarker1 || block.getBeginIndex() != this->valMarker2;
                this->valMarker1 = this->valMarker2 = block.getBeginIndex();
                return result;
            }
            
            bool DeterministicBlockData::splitter() const {
                return this->splitterFlag;
            }
            
            void DeterministicBlockData::setSplitter(bool value) {
                this->splitterFlag = value;
            }
                        
            void DeterministicBlockData::setAbsorbing(bool absorbing) {
                this->absorbingFlag = absorbing;
            }
            
            bool DeterministicBlockData::absorbing() const {
                return this->absorbingFlag;
            }
            
            void DeterministicBlockData::setRepresentativeState(storm::storage::sparse::state_type representativeState) {
                this->valRepresentativeState = representativeState;
            }
            
            bool DeterministicBlockData::hasRepresentativeState() const {
                return static_cast<bool>(valRepresentativeState);
            }
            
            storm::storage::sparse::state_type DeterministicBlockData::representativeState() const {
                STORM_LOG_THROW(valRepresentativeState, storm::exceptions::InvalidOperationException, "Unable to retrieve representative state for block.");
                return valRepresentativeState.get();
            }
            
            bool DeterministicBlockData::needsRefinement() const {
                return needsRefinementFlag;
            }
            
            void DeterministicBlockData::setNeedsRefinement(bool value) {
                needsRefinementFlag = value;
            }

            std::ostream& operator<<(std::ostream& out, DeterministicBlockData const& data) {
                out << "m1: " << data.marker1() << ", m2: " << data.marker2();
                return out;
            }
        }
    }
}
