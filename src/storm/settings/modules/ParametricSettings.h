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
				/**
				 * A type for saving the Smt2EncondingStrategy.
				 * 
				 * FULL_TRANSITION_SYSTEM: The transition system should be reduced only with very basic operations.
				 * ONLY_SCC_ENTRY_STATES: Scc elimination should be performed, but no further reduction.
				 * HIGH_INDEGREE:  State elimination but for states with a high indegree.
				 * RATIONAL_FUNCTION: The smt file should contain only the rational function.
				 */
				enum class Smt2EncodingStrategy {FULL_TRANSITION_SYSTEM, ONLY_SCC_ENTRY_STATES, HIGH_INDEGREE, RATIONAL_FUNCTION};
                
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
				 * Retrieves whether the parameter space was declared
				 */
				bool isParameterSpaceSet() const;
				
				/*!
				 * Retrieves the given parameter spcae
				 */
				std::string getParameterSpace() const;
				
				/*!
				 * Retrieves the threshold considered for iterative region refinement.
				 * The refinement converges as soon as the fraction of unknown area falls below this threshold
				 */
				double getRefinementThreshold() const;
				
				/*!
				 * Retrieves whether exact validation should be performed
				 */
				bool isExactValidationSet() const;
				
				/**
				 * Retrieves whether the encoding of the transition system should be exported to a file. 
				 * @return True iff the smt file should be encoded.
				 */
				bool exportToSmt2File() const;
				
				/**
				 * The path to a file location which should contain the smt2 encoding.
                 * @return A path to a file location.
                 */
				std::string exportSmt2Path() const;
				
				/**
				 * Retrieves which encoding strategy should be used for generating the smt2 file.
                 * @return The encoding strategy to be used.
                 */
				Smt2EncodingStrategy smt2EncodingStrategy() const;
                
                /*!
                 * Retrieves whether or not derivatives of the resulting rational function are to be generated.
                 *
                 * @return True if the derivatives are to be generated.
                 */
                bool isDerivativesSet() const;
				
                const static std::string moduleName;
                
            private:
				const static std::string encodeSmt2StrategyOptionName;
				const static std::string exportSmt2DestinationPathOptionName;
				const static std::string exportResultDestinationPathOptionName;
				const static std::string parameterSpaceOptionName;
				const static std::string refinementThresholdOptionName;
				const static std::string exactValidationOptionName;
                const static std::string derivativesOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_PARAMETRICSETTINGS_H_ */
