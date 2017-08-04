#ifndef STORM_SETTINGS_MODULES_PARAMETRICSETTINGS_H_
#define STORM_SETTINGS_MODULES_PARAMETRICSETTINGS_H_

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for parametric model checking.
             */
            class ParametricSettings : public ModuleSettings {
            public:
                
                /*!
                 * Creates a new set of parametric model checking settings.
                 */
                ParametricSettings();
				
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
				
               /*!
                 * Retrieves whether or not derivatives of the resulting rational function are to be generated.
                 *
                 * @return True if the derivatives are to be generated.
                 */
                bool isDerivativesSet() const;
                
                /*!
                 * Retrieves whether Continuous time models should be transformed to discrete time models
                 */
                bool transformContinuousModel() const;

                /*
                 * Retrieves whether instead of model checking, only the wellformedness constraints should be obtained.
                 */
                bool onlyObtainConstraints() const;
				
                const static std::string moduleName;
                
            private:
				const static std::string exportResultOptionName;
                const static std::string derivativesOptionName;
                const static std::string transformContinuousOptionName;
                const static std::string transformContinuousShortOptionName;
                const static std::string onlyWellformednessConstraintsOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_PARAMETRICSETTINGS_H_ */
