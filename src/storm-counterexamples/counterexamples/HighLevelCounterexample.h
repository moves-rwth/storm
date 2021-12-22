#pragma once

#include "storm-counterexamples/counterexamples/Counterexample.h"
#include "storm/storage/SymbolicModelDescription.h"

namespace storm {
namespace counterexamples {

class HighLevelCounterexample : public Counterexample {
   public:
    HighLevelCounterexample(storm::storage::SymbolicModelDescription const& model);

    void writeToStream(std::ostream& out) const override;

    bool isPrismHighLevelCounterexample() const;
    bool isJaniHighLevelCounterexample() const;

    storm::storage::SymbolicModelDescription const& getModelDescription() const;

   private:
    storm::storage::SymbolicModelDescription model;
};

}  // namespace counterexamples
}  // namespace storm
