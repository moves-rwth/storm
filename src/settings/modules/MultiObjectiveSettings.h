#ifndef STORM_SETTINGS_MODULES_MULTIOBJECTIVESETTINGS_H_
#define STORM_SETTINGS_MODULES_MULTIOBJECTIVESETTINGS_H_

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for multi-objective model checking.
             */
            class MultiObjectiveSettings : public ModuleSettings {
            public:
                
                /*!
                 * Creates a new set of multi-objective model checking settings.
                 */
                MultiObjectiveSettings();
				
				/**
				 * Retrieves whether the model checking result should be exported to a file.
                 * @return  True iff the result should be exported to a file.
                 */
				bool exportResultToFile() const;
				
				/**
				 * The path to a file location which should contain the model checking result.
                 * @return A path to a file location.
                 */
				std::string exportResultPath() const;
				
				/**
				 * Retrieves the desired precision for numerical- and pareto queries.
                 * @return the desired precision for numerical- and pareto queries.
				 */
				double getPrecision() const;
				
                /*!
                 * Retrieves whether or not a threshold for the number of performed iterations is given.
                 *
                 * @return True if a threshold for the number of performed iterations is given.
                 */
                bool isMaxIterationsSet() const;
                
                
                /*!
                 * Retrieves The maximum number of iterations that should be performed (if given).
                 *
                 * @return the maximum number of iterations that should be performed (if given).
                 */
                uint_fast64_t getMaxIterations() const;
                
                
                const static std::string moduleName;
                
            private:
				const static std::string exportResultFileOptionName;
				const static std::string precisionOptionName;
				const static std::string maxIterationsOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_MULTIOBJECTIVESETTINGS_H_ */
