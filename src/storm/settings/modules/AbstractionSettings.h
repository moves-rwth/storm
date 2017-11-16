#pragma once

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for the abstraction procedures.
             */
            class AbstractionSettings : public ModuleSettings {
            public:
                enum class Method {
                    Games, Bisimulation
                };
                
                enum class PivotSelectionHeuristic {
                    NearestMaximalDeviation, MostProbablePath, MaxWeightedDeviation
                };
                
                enum class SplitMode {
                    All, None, NonGuard
                };
                
                enum class ReuseMode {
                    All, None, Qualitative, Quantitative
                };
                
                /*!
                 * Creates a new set of abstraction settings.
                 */
                AbstractionSettings();
                
                /*!
                 * Retrieves the selected abstraction refinement method.
                 */
                Method getAbstractionRefinementMethod() const;
                
                /*!
                 * Retrieves whether the option to use the decomposition was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isUseDecompositionSet() const;
                
                /*!
                 * Retrieves the selected split mode.
                 *
                 * @return The selected split mode.
                 */
                SplitMode getSplitMode() const;
                
                /*!
                 * Retrieves whether the option to add all guards was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isAddAllGuardsSet() const;
                
                /*!
                 * Sets the option to add all guards to the specified value.
                 *
                 * @param value The new value.
                 */
                void setAddAllGuards(bool value);
                
                /*!
                 * Retrieves whether the option to use interpolation was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isUseInterpolationSet() const;
                
                /*!
                 * Retrieves the precision that is used for detecting convergence.
                 *
                 * @return The precision to use for detecting convergence.
                 */
                double getPrecision() const;
                
                /*!
                 * Retrieves the selected heuristic to select pivot blocks.
                 *
                 * @return The selected heuristic.
                 */
                PivotSelectionHeuristic getPivotSelectionHeuristic() const;
                
                /*!
                 * Retrieves the selected reuse mode.
                 *
                 * @return The selected reuse mode.
                 */
                ReuseMode getReuseMode() const;
                
                const static std::string moduleName;
                
            private:
                const static std::string methodOptionName;
                const static std::string useDecompositionOptionName;
                const static std::string splitModeOptionName;
                const static std::string addAllGuardsOptionName;
                const static std::string useInterpolationOptionName;
                const static std::string precisionOptionName;
                const static std::string pivotHeuristicOptionName;
                const static std::string reuseResultsOptionName;
            };
            
        }
    }
}
