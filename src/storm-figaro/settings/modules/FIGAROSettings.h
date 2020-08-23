#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"


namespace storm {
    namespace settings {
        namespace modules {
            class FIGAROSettings : public ModuleSettings {
            public:
                /*!
                 * Creates a new FIGARO setting
                 */
                FIGAROSettings();
                
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
                static const std::string propertyOptionName;
                static const std::string propertyOptionShortName;
            };
        }
    }
}


