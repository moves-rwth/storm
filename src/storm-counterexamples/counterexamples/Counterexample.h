#pragma once

#include <ostream>

namespace storm {
namespace counterexamples {

class Counterexample {
   public:
    virtual ~Counterexample() = default;

    virtual void writeToStream(std::ostream& out) const = 0;
};

std::ostream& operator<<(std::ostream& out, Counterexample const& counterexample);

}  // namespace counterexamples
}  // namespace storm
