#pragma once

#include "storm/counterexamples/Counterexample.h"

#include "storm/storage/prism/Program.h"

namespace storm {
    namespace counterexamples {
        
        class PrismHighLevelCounterexample : public Counterexample {
        public:
            PrismHighLevelCounterexample(storm::prism::Program const& program);

            void writeToStream(std::ostream& out) const override;
            
        private:
            storm::prism::Program program;
        };
        
    }
}
