#include <memory>
#include <vector>
#include "storm/adapters/RationalFunctionForward.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/io/file.h"
#include "storm/utility/OptionalRef.h"
#include "storm/utility/macros.h"

namespace storm {
namespace analysis {
template<typename V>
class ConstraintCollector;
}

namespace api {
void exportParametricResultToFile(std::optional<storm::RationalFunction>,
                                  storm::OptionalRef<storm::analysis::ConstraintCollector<storm::RationalFunction> const> const&, std::string const&);
}  // namespace api
}  // namespace storm
