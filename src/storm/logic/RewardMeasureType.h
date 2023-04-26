#ifndef STORM_LOGIC_REWARDMEASURETYPE_H_
#define STORM_LOGIC_REWARDMEASURETYPE_H_

#include <iosfwd>

namespace storm {
namespace logic {

enum class RewardMeasureType { Expectation, Variance };

std::ostream& operator<<(std::ostream& out, RewardMeasureType const& type);

}  // namespace logic
}  // namespace storm

#endif /* STORM_LOGIC_REWARDMEASURETYPE_H_ */
