#include "storm/counterexamples/PrismHighLevelCounterexample.h"

namespace storm {
    namespace counterexamples {
        
        PrismHighLevelCounterexample::PrismHighLevelCounterexample(storm::prism::Program const& program) : program(program) {
            // Intentionally left empty.
        }
        
        void PrismHighLevelCounterexample::writeToStream(std::ostream& out) const {
            out << "High-level counterexample (PRISM program): " << std::endl;
            out << program;
        }
        
    }
}
