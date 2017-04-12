#include "storm/models/sparse/StateLabeling.h"

#include "storm/exceptions/OutOfRangeException.h"
#include "storm/exceptions/InvalidArgumentException.h"


namespace storm {
    namespace models {
        namespace sparse {
            StateLabeling::StateLabeling(uint_fast64_t stateCount) : stateCount(stateCount), nameToLabelingIndexMap(), labelings() {
                // Intentionally left empty.
            }
            
            bool StateLabeling::operator==(StateLabeling const& other) const {
                if (stateCount != other.stateCount) {
                    return false;
                }
                if (this->getNumberOfLabels() != other.getNumberOfLabels()) {
                    return false;
                }
                for (auto const& labelIndexPair : this->nameToLabelingIndexMap) {
                    if (!other.containsLabel(labelIndexPair.first)) {
                        return false;
                    }
                    if (labelings[labelIndexPair.second] != other.getStates(labelIndexPair.first)) {
                        return false;
                    }
                }
                return true;
            }
            
            StateLabeling StateLabeling::getSubLabeling(storm::storage::BitVector const& states) const {
                StateLabeling result(states.getNumberOfSetBits());
                for (auto const& labelIndexPair : nameToLabelingIndexMap) {
                    result.addLabel(labelIndexPair.first, labelings[labelIndexPair.second] % states);
                }
                return result;
            }
            
            void StateLabeling::addLabel(std::string const& label) {
                addLabel(label, storage::BitVector(stateCount));
            }
            
            std::set<std::string> StateLabeling::getLabels() const {
                std::set<std::string> result;
                for (auto const& labelIndexPair : this->nameToLabelingIndexMap) {
                    result.insert(labelIndexPair.first);
                }
                return result;
            }
            
            std::set<std::string> StateLabeling::getLabelsOfState(storm::storage::sparse::state_type state) const {
                std::set<std::string> result;
                for (auto const& labelIndexPair : this->nameToLabelingIndexMap) {
                    if (this->getStateHasLabel(labelIndexPair.first, state)) {
                        result.insert(labelIndexPair.first);
                    }
                }
                return result;
            }
            
            void StateLabeling::addLabel(std::string const& label, storage::BitVector const& labeling) {
                STORM_LOG_THROW(!this->containsLabel(label), storm::exceptions::InvalidArgumentException, "Label '" << label << "' already exists.");
                STORM_LOG_THROW(labeling.size() == stateCount, storm::exceptions::InvalidArgumentException, "Labeling vector has invalid size. Expected: " << stateCount << " Actual: " << labeling.size());
                nameToLabelingIndexMap.emplace(label, labelings.size());
                labelings.push_back(labeling);
            }
            
            void StateLabeling::addLabel(std::string const& label, storage::BitVector&& labeling) {
                STORM_LOG_THROW(!this->containsLabel(label), storm::exceptions::InvalidArgumentException, "Label '" << label << "' already exists.");
                STORM_LOG_THROW(labeling.size() == stateCount, storm::exceptions::InvalidArgumentException, "Labeling vector has invalid size. Expected: " << stateCount << " Actual: " << labeling.size());
                nameToLabelingIndexMap.emplace(label, labelings.size());
                labelings.emplace_back(std::move(labeling));
            }
            
            bool StateLabeling::containsLabel(std::string const& label) const {
                return nameToLabelingIndexMap.find(label) != nameToLabelingIndexMap.end();
            }
            
            void StateLabeling::addLabelToState(std::string const& label, storm::storage::sparse::state_type state) {
                STORM_LOG_THROW(this->containsLabel(label), storm::exceptions::OutOfRangeException, "Label '" << label << "' unknown.");
                STORM_LOG_THROW(state < stateCount, storm::exceptions::OutOfRangeException, "State index out of range.");
                this->labelings[nameToLabelingIndexMap.at(label)].set(state, true);
            }
            
            bool StateLabeling::getStateHasLabel(std::string const& label, storm::storage::sparse::state_type state) const {
                STORM_LOG_THROW(this->containsLabel(label), storm::exceptions::InvalidArgumentException, "The label '" << label << "' is invalid for the labeling of the model.");
                return this->labelings[nameToLabelingIndexMap.at(label)].get(state);
            }
            
            std::size_t StateLabeling::getNumberOfLabels() const {
                return labelings.size();
            }
            
            storm::storage::BitVector const& StateLabeling::getStates(std::string const& label) const {
                STORM_LOG_THROW(this->containsLabel(label), storm::exceptions::InvalidArgumentException, "The label " << label << " is invalid for the labeling of the model.");
                return this->labelings[nameToLabelingIndexMap.at(label)];
            }

            void StateLabeling::setStates(std::string const& label, storage::BitVector const& labeling) {
                STORM_LOG_THROW(this->containsLabel(label), storm::exceptions::InvalidArgumentException, "The label " << label << " is invalid for the labeling of the model.");
                STORM_LOG_THROW(labeling.size() == stateCount, storm::exceptions::InvalidArgumentException, "Labeling vector has invalid size.");
                this->labelings[nameToLabelingIndexMap.at(label)] = labeling;
            }
            
            void StateLabeling::setStates(std::string const& label, storage::BitVector&& labeling) {
                STORM_LOG_THROW(this->containsLabel(label), storm::exceptions::InvalidArgumentException, "The label " << label << " is invalid for the labeling of the model.");
                STORM_LOG_THROW(labeling.size() == stateCount, storm::exceptions::InvalidArgumentException, "Labeling vector has invalid size.");
                this->labelings[nameToLabelingIndexMap.at(label)] = labeling;
            }
            
            void StateLabeling::printLabelingInformationToStream(std::ostream& out) const {
                out << "Labels: \t" << this->getNumberOfLabels() << std::endl;
                for (auto const& labelIndexPair : this->nameToLabelingIndexMap) {
                    out << "   * " << labelIndexPair.first << " -> " << this->labelings[labelIndexPair.second].getNumberOfSetBits() << " state(s)" << std::endl;
                }
            }

            void StateLabeling::printCompleteLabelingInformationToStream(std::ostream& out) const {
                out << "Labels: \t" << this->getNumberOfLabels() << std::endl;
                for (auto label : nameToLabelingIndexMap) {
                    out << "Label '" << label.first << "': ";
                    for (auto index : this->labelings[label.second]) {
                        out << index << " ";
                    }
                    out << std::endl;
                }
            }

            std::ostream& operator<<(std::ostream& out, StateLabeling const& labeling) {
                labeling.printLabelingInformationToStream(out);
                return out;
            }

        }
    }
}
