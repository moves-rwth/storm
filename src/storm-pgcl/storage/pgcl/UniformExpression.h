#pragma once

#include <stdint.h>

namespace storm {
namespace pgcl {
/**
 * This class wraps a uniform distribution expression of the form
 * unif(k,l) where k <= l are both integers.
 */
class UniformExpression {
   public:
    UniformExpression() = default;
    /**
     * Constructs a uniform expression with the given beginning and
     * end.
     * @param begin The begin of the uniform distribution.
     * @param begin The end of the uniform distribution.
     */
    UniformExpression(int_fast64_t begin, int_fast64_t end);
    /**
     * Returns the begin of the uniform distribution.
     * @return The begin of the uniform distribution.
     */
    int_fast64_t getBegin() const;
    /**
     * Returns the end of the uniform distribution.
     * @return The end of the uniform distribution.
     */
    int_fast64_t getEnd() const;

   private:
    int_fast64_t begin = 0;
    int_fast64_t end = 0;
};
}  // namespace pgcl
}  // namespace storm
