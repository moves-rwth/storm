#pragma once

#include "storm-config.h"
#include "storm-pomdp/storage/PomdpMemory.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {

/*!
 * This class represents the settings for POMDP model checking.
 */
class QualitativePOMDPAnalysisSettings : public ModuleSettings {
   public:
    /*!
     * Creates a new set of POMDP settings.
     */
    QualitativePOMDPAnalysisSettings();

    uint64_t getLookahead() const;
    std::string getLookaheadType() const;
    bool isExportSATCallsSet() const;
    std::string getExportSATCallsPath() const;
    bool isOnlyDeterministicSet() const;
    bool isWinningRegionSet() const;
    bool validateIntermediateSteps() const;
    bool validateFinalResult() const;
    bool computeExpensiveStats() const;
    bool isComputeOnBeliefSupportSet() const;
    bool isPrintWinningRegionSet() const;
    bool isExportWinningRegionSet() const;
    std::string exportWinningRegionPath() const;
    bool isGraphPreprocessingAllowed() const;
    bool isMemlessSearchSet() const;
    std::string getMemlessSearchMethod() const;

    virtual ~QualitativePOMDPAnalysisSettings() = default;

    bool check() const override;
    void finalize() override;

    // The name of the module.
    static const std::string moduleName;

   private:
};

}  // namespace modules
}  // namespace settings
}  // namespace storm
