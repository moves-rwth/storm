#include "storm/counterexamples/Counterexample.h"

namespace storm {
    namespace counterexamples {
        
        std::ostream& operator<<(std::ostream& out, Counterexample const& counterexample) {
            counterexample.writeToStream(out);
            return out;
        }
        
    }
}
