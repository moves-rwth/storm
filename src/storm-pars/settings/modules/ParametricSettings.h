#pragma once
#include "storm/settings/modules/ModuleSettings.h"
#include "storm-pars/utility/ParametricMode.h"

namespace storm::settings::modules {
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
                 * Retrieves whether Continuous time models should be transformed to discrete time models
                 */
                bool transformContinuousModel() const;

                /*!
                 * Retrieves whether monotonicity should be used as preprocessing
                 */
                bool isUseMonotonicitySet() const;

                /*!
                 * Has the operation mode (feasibility, verification, etc) been set?
                 */
                bool hasOperationModeBeenSet() const;

                /*!
                 * In what operation mode should storm-pars run
                 */
                pars::utility::ParametricMode getOperationMode() const;

                /*! 
                 * Retrieves whether time-travelling should be enabled.
                 */
				bool isTimeTravellingEnabled() const;

                /*! 
                 * Retrieves whether time-travelling should be enabled.
                 */
				bool isLinearToSimpleEnabled() const;

                const static std::string moduleName;

            };
}
