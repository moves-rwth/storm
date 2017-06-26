#pragma once

#include "storm-pars/modelchecker/region/RegionCheckEngine.h"

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for parametric model checking.
             */
            class RegionSettings : public ModuleSettings {
            public:
                
                /*!
                 * Creates a new set of parametric model checking settings.
                 */
                RegionSettings();
				
				/*!
				 * Retrieves whether region(s) were declared
				 */
				bool isRegionSet() const;
				
				/*!
				 * Retrieves the region definition string
				 */
				std::string getRegionString() const;
				
				/*!
				 * Retrieves whether region refinement is enabled
				 */
				bool isRefineSet() const;
				
				/*!
				 * Retrieves the threshold considered for iterative region refinement.
				 * The refinement converges as soon as the fraction of unknown area falls below this threshold
				 */
				double getRefinementThreshold() const;
				
				/*!
				 * Retrieves which type of region check should be performed
				 */
				storm::modelchecker::RegionCheckEngine getRegionCheckEngine() const;
				
				/*!
				 * Retrieves whether no illustration of the result should be printed.
				 */
                bool isPrintNoIllustrationSet() const;
                
                /*!
                 * Retrieves whether the full result should be printed
                 */
                bool isPrintFullResultSet() const;
                
                const static std::string moduleName;
                
            private:
				const static std::string regionOptionName;
				const static std::string regionShortOptionName;
				const static std::string refineOptionName;
				const static std::string checkEngineOptionName;
				const static std::string printNoIllustrationOptionName;
				const static std::string printFullResultOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

