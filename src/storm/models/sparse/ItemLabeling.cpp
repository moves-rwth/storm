#include "storm/models/sparse/ItemLabeling.h"

#include "storm/exceptions/OutOfRangeException.h"
#include "storm/exceptions/InvalidArgumentException.h"


namespace storm {
    namespace models {
        namespace sparse {
            ItemLabeling::ItemLabeling(uint_fast64_t itemCount) : itemCount(itemCount), nameToLabelingIndexMap(), labelings() {
                // Intentionally left empty.
            }

            bool ItemLabeling::operator==(ItemLabeling const& other) const {
                if (itemCount != other.itemCount) {
                    return false;
                }
                if (this->getNumberOfLabels() != other.getNumberOfLabels()) {
                    return false;
                }
                for (auto const& labelIndexPair : this->nameToLabelingIndexMap) {
                    if (!other.containsLabel(labelIndexPair.first)) {
                        return false;
                    }
                    if (labelings[labelIndexPair.second] != other.getItems(labelIndexPair.first)) {
                        return false;
                    }
                }
                return true;
            }

            ItemLabeling ItemLabeling::getSubLabeling(storm::storage::BitVector const& items) const {
                ItemLabeling result(items.getNumberOfSetBits());
                for (auto const& labelIndexPair : nameToLabelingIndexMap) {
                    result.addLabel(labelIndexPair.first, labelings[labelIndexPair.second] % items);
                }
                return result;
            }

            void ItemLabeling::addLabel(std::string const& label) {
                addLabel(label, storage::BitVector(itemCount));
            }

            void ItemLabeling::join(ItemLabeling const& other) {
                STORM_LOG_THROW(this->itemCount == other.itemCount, storm::exceptions::InvalidArgumentException, "The item count of the two labelings does not match: " << this->itemCount << " vs. " << other.itemCount << ".");
                for (auto const& label : other.getLabels()) {
                    if (this->containsLabel(label)) {
                        this->setItems(label, this->getItems(label) | other.getItems(label));
                    } else {
                        this->addLabel(label, other.getItems(label));
                    }
                }
            }
            
            std::set<std::string> ItemLabeling::getLabels() const {
                std::set<std::string> result;
                for (auto const& labelIndexPair : this->nameToLabelingIndexMap) {
                    result.insert(labelIndexPair.first);
                }
                return result;
            }

            std::set<std::string> ItemLabeling::getLabelsOfItem(uint64_t item) const {
                std::set<std::string> result;
                for (auto const& labelIndexPair : this->nameToLabelingIndexMap) {
                    if (this->getItemHasLabel(labelIndexPair.first, item)) {
                        result.insert(labelIndexPair.first);
                    }
                }
                return result;
            }

            void ItemLabeling::addLabel(std::string const& label, storage::BitVector const& labeling) {
                STORM_LOG_THROW(!this->containsLabel(label), storm::exceptions::InvalidArgumentException, "Label '" << label << "' already exists.");
                STORM_LOG_THROW(labeling.size() == itemCount, storm::exceptions::InvalidArgumentException, "Labeling vector has invalid size. Expected: " << itemCount << " Actual: " << labeling.size());
                nameToLabelingIndexMap.emplace(label, labelings.size());
                labelings.push_back(labeling);
            }

            void ItemLabeling::addLabel(std::string const& label, storage::BitVector&& labeling) {
                STORM_LOG_THROW(!this->containsLabel(label), storm::exceptions::InvalidArgumentException, "Label '" << label << "' already exists.");
                STORM_LOG_THROW(labeling.size() == itemCount, storm::exceptions::InvalidArgumentException, "Labeling vector has invalid size. Expected: " << itemCount << " Actual: " << labeling.size());
                nameToLabelingIndexMap.emplace(label, labelings.size());
                labelings.emplace_back(std::move(labeling));
            }

            bool ItemLabeling::containsLabel(std::string const& label) const {
                return nameToLabelingIndexMap.find(label) != nameToLabelingIndexMap.end();
            }

            void ItemLabeling::addLabelToItem(std::string const& label, uint64_t item) {
                STORM_LOG_THROW(this->containsLabel(label), storm::exceptions::OutOfRangeException, "Label '" << label << "' unknown.");
                STORM_LOG_THROW(item < itemCount, storm::exceptions::OutOfRangeException, "Item index out of range.");
                this->labelings[nameToLabelingIndexMap.at(label)].set(item, true);
            }

            bool ItemLabeling::getItemHasLabel(std::string const& label, uint64_t item) const {
                STORM_LOG_THROW(this->containsLabel(label), storm::exceptions::InvalidArgumentException, "The label '" << label << "' is invalid for the labeling of the model.");
                return this->labelings[nameToLabelingIndexMap.at(label)].get(item);
            }

            std::size_t ItemLabeling::getNumberOfLabels() const {
                return labelings.size();
            }

            storm::storage::BitVector const& ItemLabeling::getItems(std::string const& label) const {
                STORM_LOG_THROW(this->containsLabel(label), storm::exceptions::InvalidArgumentException, "The label " << label << " is invalid for the labeling of the model.");
                return this->labelings[nameToLabelingIndexMap.at(label)];
            }

            void ItemLabeling::setItems(std::string const& label, storage::BitVector const& labeling) {
                STORM_LOG_THROW(this->containsLabel(label), storm::exceptions::InvalidArgumentException, "The label " << label << " is invalid for the labeling of the model.");
                STORM_LOG_THROW(labeling.size() == itemCount, storm::exceptions::InvalidArgumentException, "Labeling vector has invalid size.");
                this->labelings[nameToLabelingIndexMap.at(label)] = labeling;
            }

            void ItemLabeling::setItems(std::string const& label, storage::BitVector&& labeling) {
                STORM_LOG_THROW(this->containsLabel(label), storm::exceptions::InvalidArgumentException, "The label " << label << " is invalid for the labeling of the model.");
                STORM_LOG_THROW(labeling.size() == itemCount, storm::exceptions::InvalidArgumentException, "Labeling vector has invalid size.");
                this->labelings[nameToLabelingIndexMap.at(label)] = labeling;
            }

            void ItemLabeling::printLabelingInformationToStream(std::ostream& out) const {
                out << "Labels: \t" << this->getNumberOfLabels() << std::endl;
                for (auto const& labelIndexPair : this->nameToLabelingIndexMap) {
                    out << "   * " << labelIndexPair.first << " -> " << this->labelings[labelIndexPair.second].getNumberOfSetBits() << " item(s)" << std::endl;
                }
            }

            void ItemLabeling::printCompleteLabelingInformationToStream(std::ostream& out) const {
                out << "Labels: \t" << this->getNumberOfLabels() << std::endl;
                for (auto label : nameToLabelingIndexMap) {
                    out << "Label '" << label.first << "': ";
                    for (auto index : this->labelings[label.second]) {
                        out << index << " ";
                    }
                    out << std::endl;
                }
            }

            std::ostream& operator<<(std::ostream& out, ItemLabeling const& labeling) {
                labeling.printLabelingInformationToStream(out);
                return out;
            }

        }
    }
}
