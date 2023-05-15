#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <vector>

namespace storm {
namespace modelchecker {
namespace helper {
namespace rewardbounded {

class EpochManager {
   public:
    typedef uint64_t Epoch;       // The number of reward steps that are "left" for each dimension
    typedef uint64_t EpochClass;  // Encodes the dimensions of an epoch that are bottom. Two epoch models within the same class have the same graph structure.

    EpochManager();
    EpochManager(uint64_t dimensionCount);

    uint64_t const& getDimensionCount() const;

    Epoch getBottomEpoch() const;
    Epoch getZeroEpoch() const;

    bool compareEpochClass(Epoch const& epoch1, Epoch const& epoch2) const;
    EpochClass getEpochClass(Epoch const& epoch) const;

    Epoch getSuccessorEpoch(Epoch const& epoch, Epoch const& step) const;
    std::vector<Epoch> getPredecessorEpochs(Epoch const& epoch, Epoch const& step) const;
    void gatherPredecessorEpochs(std::set<Epoch>& gatheredPredecessorEpochs, Epoch const& epoch, Epoch const& step) const;

    bool isZeroEpoch(Epoch const& epoch) const;
    bool isBottomEpoch(Epoch const& epoch) const;
    bool hasBottomDimension(Epoch const& epoch) const;
    bool hasBottomDimensionEpochClass(EpochClass const& epochClass) const;
    bool isPredecessorEpochClass(EpochClass const& epochClass1, EpochClass const& epochClass2) const;

    bool isValidDimensionValue(uint64_t const& value) const;

    void setBottomDimension(Epoch& epoch, uint64_t const& dimension) const;
    void setDimensionOfEpoch(Epoch& epoch, uint64_t const& dimension, uint64_t const& value) const;  // assumes that the value is valid, i.e., small enough
    void setDimensionOfEpochClass(EpochClass& epochClass, uint64_t const& dimension, bool const& setToBottom) const;

    bool isBottomDimension(Epoch const& epoch, uint64_t const& dimension) const;
    bool isBottomDimensionEpochClass(EpochClass const& epochClass, uint64_t const& dimension) const;
    uint64_t getDimensionOfEpoch(Epoch const& epoch, uint64_t const& dimension) const;  // assumes that the dimension is not bottom
    uint64_t getSumOfDimensions(Epoch const& epoch) const;                              // assumes that the dimension is not bottom

    std::string toString(Epoch const& epoch) const;

    bool epochClassZigZagOrder(Epoch const& epoch1, Epoch const& epoch2) const;
    bool epochClassOrder(EpochClass const& epochClass1, EpochClass const& epochClass2) const;

   private:
    uint64_t dimensionCount;
    uint64_t bitsPerDimension;
    uint64_t dimensionBitMask;
    uint64_t relevantBitsMask;
};
}  // namespace rewardbounded
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm
