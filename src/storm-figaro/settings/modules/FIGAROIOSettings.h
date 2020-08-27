#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"
#include "storm-dft/builder/DftExplorationHeuristic.h"

namespace storm {

    namespace settings {
        namespace modules {
            class FIGAROIOSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new FIGARO setting
                 */
                FIGAROIOSettings();
                
                /**
                 * Retrievew whether the figaro file option was set
                 */
                bool isfigaroFileSet() const;
                
                /**
                 * Retrieves the figaro file name
                 */
                std::string getfigaroFilename() const;
                /**
                 * Retrievew whether the xml file option was set
                 */
                bool isxmlFileSet() const;
                
                /**
                 * Retrieves the xml file name
                 */
                std::string getxmlFilename() const;
                /**
                 * Retrievew whether the result text  file option was set
                 */
                bool isrslttxtFileSet() const;
                
                /**
                 * Retrieves the result file name
                 */
                std::string getrlsttxtFilename() const;
                
                /**
                 * Whether the figaro should be  should be drawn (dot output)
                 */
                bool isToDotSet() const;
//
//                /**
//                 * returns the file name where dot output should be stored.
//                 */
                std::string getFigaroDotOutputFilename() const;
//
//                /**
//                 * Whether the figaro should be exported as drn file
//                 */
                bool isFigaroToExplicitSet() const;
//
//                /**
//                 * Destination where to write dot output of the figaro.
//                 */
                std::string getFigaroExplicitOutputFilename() const ;
//
//
//
//                /*!
//                 * Retrieves whether the property option was set.
//                 *
//                 * @return True if the property option was set.
//                 */
                bool isPropertyInputSet() const;
//
//                /*!
//                 * Retrieves the property specified with the property option.
//                 *
//                 * @return The property specified with the property option.
//                 */
                std::string getPropertyInput() const;
//
//                /*!
//                 * Retrieves the property filter.
//                 *
//                 * @return The property filter.
//                 */
                std::string getPropertyInputFilter() const;
//
                /*!
                 * Retrieves whether the option to compute an approximation is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isApproximationErrorSet() const;
                
                /*!
                 * Retrieves the relative error allowed for approximating the model checking result.
                 *
                 * @return The allowed errorbound.
                 */
                double getApproximationError() const;
                
                /*!
                 * Retrieves the heuristic used for approximation.
                 *
                 * @return The heuristic to use.
                 */
                storm::builder::ApproximationHeuristic getApproximationHeuristic() const;
                
                /*!
                 * Retrieves whether the option to set a maximal exploration depth is set.
                 *
                 * @return True iff the option was set.
                 */
                bool isMaxDepthSet() const;
                
                /*!
                 * Retrieves the maximal exploration depth.
                 *
                 * @return The maximal exploration depth.
                 */
                uint_fast64_t getMaxDepth() const;
                //
                bool check() const override;
                void finalize() override;
                
                static const std::string moduleName;
                
            private:
                static const std::string figaroFileOptionName;
                 static const std::string figaroFileOptionShortame;
                static const std::string xmlFileOptionName;
                static const std::string xmlFileOptionShortName;
                static const std::string figaroToExplicitOptionName;
                static const std::string figaroToExplicitOptionShortName;
                static const std::string figaroToDotOptionName;
                static const std::string figaroToDotOptionShortName;
                static const std::string outputtextFileOptionName;
                static const std::string outputtextFileOptionShortName;
                static const std::string propertyOptionName;
                static const std::string propertyOptionShortName;
                static const std::string approximationErrorOptionName;
                static const std::string approximationErrorOptionShortName;
                static const std::string approximationHeuristicOptionName;
                static const std::string maxDepthOptionName;
            };
        }
    }
}


