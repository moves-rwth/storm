#pragma once

#include <boost/optional.hpp>
#include <memory>
#include <string>

#include "storm/environment/Environment.h"
#include "storm/environment/SubEnvironment.h"

namespace storm {

enum class SteadyStateDistributionAlgorithm { Automatic, EquationSystem, ExpectedVisitingTimes, Classic };

// Forward declare subenvironments
class MultiObjectiveModelCheckerEnvironment;

class ModelCheckerEnvironment {
   public:
    ModelCheckerEnvironment();
    ~ModelCheckerEnvironment();

    MultiObjectiveModelCheckerEnvironment& multi();
    MultiObjectiveModelCheckerEnvironment const& multi() const;

    SteadyStateDistributionAlgorithm getSteadyStateDistributionAlgorithm() const;
    void setSteadyStateDistributionAlgorithm(SteadyStateDistributionAlgorithm value);

    bool isLtl2daToolSet() const;
    std::string const& getLtl2daTool() const;
    void setLtl2daTool(std::string const& value);
    void unsetLtl2daTool();

   private:
    SubEnvironment<MultiObjectiveModelCheckerEnvironment> multiObjectiveModelCheckerEnvironment;
    boost::optional<std::string> ltl2daTool;
    SteadyStateDistributionAlgorithm steadyStateDistributionAlgorithm;
};
}  // namespace storm
