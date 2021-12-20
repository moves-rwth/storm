#pragma once

#include "storm/storage/expressions/Expression.h"

namespace storm {
namespace logic {

class TimeBound {
   public:
    TimeBound(bool strict, storm::expressions::Expression const& bound);
    TimeBound(TimeBound const& other) = default;

    storm::expressions::Expression const& getBound() const;
    bool isStrict() const;

   private:
    bool strict;
    storm::expressions::Expression bound;
};

}  // namespace logic
}  // namespace storm
