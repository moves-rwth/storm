#ifndef STORM_LOGIC_FORMULACONTEXT_H_
#define STORM_LOGIC_FORMULACONTEXT_H_

#include <ostream>

namespace storm {
    namespace logic {

        enum class FormulaContext { Undefined, Probability, Reward, LongRunAverage, Time };
        std::ostream& operator<<(std::ostream& out, FormulaContext const& formulaContext);
    }
}

#endif /* STORM_LOGIC_FORMULACONTEXT_H_ */
