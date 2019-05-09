#pragma once

namespace storm {
    namespace storage {
        class DFTStateGenerationInfo {
        private:
            const size_t mUsageInfoBits;
            std::map<size_t, size_t> mSpareUsageIndex; // id spare -> index first bit in state
            std::map<size_t, size_t> mSpareActivationIndex; // id spare representative -> index in state
            std::vector<size_t> mIdToStateIndex; // id -> index first bit in state
            std::map<size_t, std::vector<size_t>> mSeqRestrictionPreElements; // id -> list of restriction pre elements
            std::map<size_t, std::vector<size_t>> mSeqRestrictionPostElements; // id -> list of restriction post elements
            std::map<size_t, std::vector<size_t>> mMutexRestrictionElements; // id -> list of elments in the same mutexes
            std::vector<std::pair<size_t, std::vector<size_t>>> mSymmetries; // pair (length of symmetry group, vector indicating the starting points of the symmetry groups)

        public:

            DFTStateGenerationInfo(size_t nrElements, size_t maxSpareChildCount) :
                mUsageInfoBits(storm::utility::math::uint64_log2(maxSpareChildCount) + 1), 
                mIdToStateIndex(nrElements)
            {
                STORM_LOG_ASSERT(maxSpareChildCount < pow(2, mUsageInfoBits), "Bit length incorrect.");
            }

            size_t usageInfoBits() const {
                return mUsageInfoBits;
            }

            void addStateIndex(size_t id, size_t index) {
                STORM_LOG_ASSERT(id < mIdToStateIndex.size(), "Id invalid.");
                mIdToStateIndex[id] = index;
            }

            void setRestrictionPreElements(size_t id, std::vector<size_t> const& elems) {
                mSeqRestrictionPreElements[id] = elems;
            }
            
            void setRestrictionPostElements(size_t id, std::vector<size_t> const& elems) {
                mSeqRestrictionPostElements[id] = elems;
            }

            void setMutexElements(size_t id, std::vector<size_t> const& elems) {
                mMutexRestrictionElements[id] = elems;
            }

            std::vector<size_t> const& seqRestrictionPreElements(size_t index) const {
                STORM_LOG_ASSERT(mSeqRestrictionPreElements.count(index) > 0, "Index invalid.");
                return mSeqRestrictionPreElements.at(index);
            }
            
            std::vector<size_t> const& seqRestrictionPostElements(size_t index) const {
                STORM_LOG_ASSERT(mSeqRestrictionPostElements.count(index) > 0, "Index invalid.");
                return mSeqRestrictionPostElements.at(index);
            }

            std::vector<size_t> const& mutexRestrictionElements(size_t index) const {
                STORM_LOG_ASSERT(mMutexRestrictionElements.count(index) > 0, "Index invalid.");
                return mMutexRestrictionElements.at(index);
            }
            
            void addSpareActivationIndex(size_t id, size_t index) {
                mSpareActivationIndex[id] = index;
            }

            void addSpareUsageIndex(size_t id, size_t index) {
                mSpareUsageIndex[id] = index;
            }

            size_t getStateIndex(size_t id) const {
                STORM_LOG_ASSERT(id < mIdToStateIndex.size(), "Id invalid.");
                return mIdToStateIndex[id];
            }

            size_t getSpareUsageIndex(size_t id) const {
                STORM_LOG_ASSERT(mSpareUsageIndex.count(id) > 0, "Id invalid.");
                return mSpareUsageIndex.at(id);
            }

            size_t getSpareActivationIndex(size_t id) const {
                STORM_LOG_ASSERT(mSpareActivationIndex.count(id) > 0, "Id invalid.");
                return mSpareActivationIndex.at(id);
            }

            void addSymmetry(size_t length, std::vector<size_t>& startingIndices) {
                mSymmetries.push_back(std::make_pair(length, startingIndices));
            }
            
            /**
             * Generate more symmetries by combining two symmetries
             */
            void generateSymmetries(storm::storage::DFTIndependentSymmetries const& symmetries) {
                // Iterate over possible children
                for (size_t i = 0; i < mSymmetries.size(); ++i) {
                    size_t childStart = mSymmetries[i].second[0];
                    size_t childLength = mSymmetries[i].first;
                    // Iterate over possible parents
                    for (size_t j = i+1; j < mSymmetries.size(); ++j) {
                        size_t parentStart = mSymmetries[j].second[0];
                        size_t parentLength = mSymmetries[j].first;
                        // Check if child lies in parent
                        if (parentStart <= childStart && childStart + childLength < parentStart + parentLength) {
                            std::vector<std::vector<size_t>> newSymmetries;
                            for (size_t index = 1; index < mSymmetries[j].second.size(); ++index) {
                                // Get symmetric start by applying the bijection
                                std::vector<size_t> newStarts;
                                for (size_t symmetryStarts : mSymmetries[i].second) {
                                    newStarts.push_back(symmetryStarts + mSymmetries[j].second[index]);
                                }
                                newSymmetries.push_back(newStarts);
                            }
                            // Insert after child
                            for (size_t index = 0; index < newSymmetries.size(); ++index) {
                                mSymmetries.insert(mSymmetries.begin() + i + 1 + index, std::make_pair(childLength, newSymmetries[index]));
                            }
                            i += newSymmetries.size();
                            break;
                        }
                    }
                }
            }

            size_t getSymmetrySize() const {
                return mSymmetries.size();
            }
            
            bool hasSymmetries() const {
                return !mSymmetries.empty();
            }

            size_t getSymmetryLength(size_t pos) const {
                STORM_LOG_ASSERT(pos < mSymmetries.size(), "Pos invalid.");
                return mSymmetries[pos].first;
            }

            std::vector<size_t> const& getSymmetryIndices(size_t pos) const {
                STORM_LOG_ASSERT(pos < mSymmetries.size(), "Pos invalid.");
                return mSymmetries[pos].second;
            }

            friend std::ostream& operator<<(std::ostream& os, DFTStateGenerationInfo const& info) {
                os << "Id to state index:" << std::endl;
                for (size_t id = 0; id < info.mIdToStateIndex.size(); ++id) {
                    os << id << " -> " << info.getStateIndex(id) << std::endl;
                }
                os << "Spare usage index with usage InfoBits of size " << info.mUsageInfoBits << ":" << std::endl;
                for (auto pair : info.mSpareUsageIndex) {
                    os << pair.first << " -> " << pair.second << std::endl;
                }
                os << "Spare activation index:" << std::endl;
                for (auto pair : info.mSpareActivationIndex) {
                    os << pair.first << " -> " << pair.second << std::endl;
                }
                os << "Symmetries:" << std::endl;
                for (auto pair : info.mSymmetries) {
                    os << "Length: " << pair.first << ", starting indices: ";
                    for (size_t index : pair.second) {
                        os << index << ", ";
                    }
                    os << std::endl;
                }
                return os;
            }

        };
    }
}
