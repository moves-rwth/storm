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

                /*!
                 * Retrieves whether instead of model checking, only the wellformedness constraints should be obtained.
                 */
                bool onlyObtainConstraints() const;
				
                /*!
                 * Retrieves the samples as a comma-separated list of samples for each (relevant) variable, where the
                 * samples are colon-separated values. For example, 'N=1:2:3,K=2:4' means that N takes the values 1 to
                 * 3 and K either 2 or 4.
                 */
                std::string getSamples() const;
                
                /*!
                 * Retrieves whether the samples are graph preserving.
                 */
                bool isSamplesAreGraphPreservingSet() const;
                
                /*!
                 * Retrieves whether samples are to be computed exactly.
                 */
                bool isSampleExactSet() const;

                /*!
                 * Retrieves whether monotonicity should be used
                 */
                bool isUseMonotonicitySet() const;

//                bool isOnlyGlobalSet() const;

                const static std::string moduleName;
                
            private:
				const static std::string exportResultOptionName;
                const static std::string derivativesOptionName;
                const static std::string transformContinuousOptionName;
                const static std::string transformContinuousShortOptionName;
                const static std::string onlyWellformednessConstraintsOptionName;
                const static std::string samplesOptionName;
                const static std::string samplesGraphPreservingOptionName;
                const static std::string sampleExactOptionName;
                const static std::string useMonotonicityName;
//                const static std::string onlyGlobalName;

            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_PARAMETRICSETTINGS_H_ */
