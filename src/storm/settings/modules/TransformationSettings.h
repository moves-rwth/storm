#ifndef STORM_TRANSFORMATIONSETTINGS_H
#define STORM_TRANSFORMATIONSETTINGS_H

#include "storm-config.h"
#include "storm/api/transformation.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {

/*!
 * This class represents the model transformer settings
 */
class TransformationSettings : public ModuleSettings {
   public:
    /*!
     * Creates a new set of transformer settings.
     */
    TransformationSettings();

    /*!
     * Retrieves whether the option to eliminate chains of non-Markovian states was set.
     *
     * @return True if the option to eliminate chains of non-Markovian states was set.
     */
    bool isChainEliminationSet() const;

    /*!
     * Retrieves how the transformer should deal with labels when non-Markovian states are eliminated
     *
     * @return the label behavior
     */
    storm::transformer::EliminationLabelBehavior getLabelBehavior() const;

    /*!
     * Retrieves whether a DTMC/CTMC should be converted to an MDP/MA
     */
    bool isToNondeterministicModelSet() const;

    /*!
     * Retrieves whether a CTMC/MA should be converted to a DTMC/MDP
     */
    bool isToDiscreteTimeModelSet() const;

    bool check() const override;

    void finalize() override;

    // The name of the module.
    static const std::string moduleName;

   private:
    // Define the string names of the options as constants.
    static const std::string chainEliminationOptionName;
    static const std::string labelBehaviorOptionName;
    static const std::string toNondetOptionName;
    static const std::string toDiscreteTimeOptionName;
};

}  // namespace modules
}  // namespace settings
}  // namespace storm

#endif  // STORM_TRANSFORMATIONSETTINGS_H
