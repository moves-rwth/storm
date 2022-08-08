#pragma once

#include "storm/storage/BitVector.h"

namespace storm::dft {
namespace storage {

class DFT;

class DFTUnit {
   private:
    DFT const& mDft;
    BitVector mMembers;

   public:
    DFTUnit(DFT const& dft, BitVector const& members);
};

}  // namespace storage
}  // namespace storm::dft
