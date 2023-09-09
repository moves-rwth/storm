#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace builder {
template<typename PomdpType, typename BeliefValueType>
class BeliefMdpExplorer;
}
namespace pomdp {
namespace modelchecker {
template<typename ValueType>
struct BeliefExplorationPomdpModelCheckerOptions;
}

enum BeliefNumberType { Default, Float, Rational };
}  // namespace pomdp

namespace settings {
namespace modules {

/*!
 * This class represents the settings for POMDP model checking.
 */
class BeliefExplorationSettings : public ModuleSettings {
   public:
    /*!
     * Creates a new set of POMDP settings.
     */
    BeliefExplorationSettings();

    virtual ~BeliefExplorationSettings() = default;

    bool isCutZeroGapSet() const;
    bool isRefineSet() const;
    double getRefinePrecision() const;
    uint64_t getRefineStepLimit() const;

    uint64_t getExplorationTimeLimit() const;

    /// Discretization Resolution
    uint64_t getResolutionInit() const;
    double getResolutionFactor() const;

    /// Clipping Grid Resolution
    uint64_t getClippingGridResolution() const;

    /// The maximal number of newly expanded MDP states in a refinement step
    uint64_t getSizeThresholdInit() const;
    double getSizeThresholdFactor() const;

    /// Controls how large the gap between known lower- and upper bounds at a beliefstate needs to be in order to explore
    double getGapThresholdInit() const;
    double getGapThresholdFactor() const;

    /// Controls whether "almost optimal" choices will be considered optimal
    double getOptimalChoiceValueThresholdInit() const;
    double getOptimalChoiceValueThresholdFactor() const;

    /// Controls which observations are refined.
    double getObservationScoreThresholdInit() const;
    double getObservationScoreThresholdFactor() const;

    /// Used to determine whether two beliefs are equal
    bool isNumericPrecisionSetFromDefault() const;
    double getNumericPrecision() const;

    bool isDynamicTriangulationModeSet() const;
    bool isStaticTriangulationModeSet() const;

    /// Controls if (grid) clipping is to be used
    bool isUseClippingSet() const;

    bool isStateEliminationCutoffSet() const;

    template<typename ValueType>
    void setValuesInOptionsStruct(storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType>& options) const;

    // The name of the module.
    static const std::string moduleName;

   private:
};

}  // namespace modules
}  // namespace settings
}  // namespace storm
