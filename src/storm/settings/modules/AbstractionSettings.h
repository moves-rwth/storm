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
                enum class PivotSelectionHeuristic {
                    NearestMaximalDeviation, MostProbablePath, MaxWeightedDeviation
                };
                
                enum class InvalidBlockDetectionStrategy {
                    None, Command, Global
                };
                
                /*!
                 * Creates a new set of abstraction settings.
                 */
                AbstractionSettings();
                
                /*!
                 * Retrieves whether the option to add all guards was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isAddAllGuardsSet() const;

                /*!
                 * Retrieves whether the option to split predicates to atoms was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isSplitPredicatesSet() const;

                /*!
                 * Retrieves whether the option to split the initially added guards to atoms was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isSplitInitialGuardsSet() const;

                /*!
                 * Retrieves whether the option to split guards derived later to atoms was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isSplitGuardsSet() const;

                /*!
                 * Retrieves whether the option to split all predicates to atoms was set.
                 *
                 * @return True iff the option was set.
                 */
                bool isSplitAllSet() const;
                
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
                 * Retrieves the strategy to use for invalid block detection.
                 *
                 * @return The strategy to use
                 */
                InvalidBlockDetectionStrategy getInvalidBlockDetectionStrategy() const;
                
                /*!
                 * Retrieves whether the option to reuse the qualitative results.
                 *
                 * @param True iff the option was set.
                 */
                bool isReuseQualitativeResultsSet() const;

                const static std::string moduleName;
                
            private:
                const static std::string addAllGuardsOptionName;
                const static std::string splitPredicatesOptionName;
                const static std::string splitInitialGuardsOptionName;
                const static std::string splitGuardsOptionName;
                const static std::string useInterpolationOptionName;
                const static std::string splitInterpolantsOptionName;
                const static std::string splitAllOptionName;
                const static std::string precisionOptionName;
                const static std::string pivotHeuristicOptionName;
                const static std::string invalidBlockStrategyOptionName;
                const static std::string reuseQualitativeResultsOptionName;
            };
            
        }
    }
}
