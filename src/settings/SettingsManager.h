#ifndef STORM_SETTINGS_SETTINGSMANAGER_H_
#define STORM_SETTINGS_SETTINGSMANAGER_H_

#include <iostream>
#include <sstream>
#include <list>
#include <utility>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>

#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBase.h"
#include "src/settings/Argument.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/ArgumentType.h"
#include "src/settings/ArgumentTypeInferationHelper.h"
#include "src/settings/modules/ModuleSettings.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/modules/DebugSettings.h"
#include "src/settings/modules/CounterexampleGeneratorSettings.h"
#include "src/settings/modules/CuddSettings.h"
#include "src/settings/modules/GmmxxSettings.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"
#include "src/settings/modules/GlpkSettings.h"
#include "src/settings/modules/GurobiSettings.h"

#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/OptionParserException.h"

namespace storm {
    
    namespace settings {
        
        typedef std::pair<std::string, std::string> stringPair_t;
        
        class InternalOptionMemento;
        
        /*!
         *	@brief	Settings class with command line parser and type validation
         *
         *
         *	It is meant to be used as a singleton. Call
         *	@code storm::settings::Settings::getInstance() @endcode
         *	to initialize it and obtain an instance.
         *
         *	This class can be customized by other parts of the software using
         *	option modules. An option module can be anything that implements the
         *	interface specified by registerModule() and does a static initialization call to this function.
         */
        class SettingsManager {
		public:
            friend class InternalOptionMemento;
			
			/*!
			 * This parses the command line of the application and matches it to all prior registered options
			 * @throws OptionParserException
			 */
			static void parse(int const argc, char const * const argv[]);
            
			std::vector<std::shared_ptr<Option>> const& getOptions() const {
				return this->optionPointers;
			}
            
			// PUBLIC INTERFACE OF OPTIONSACCUMULATOR (now internal)
			/*!
             * Returns true IFF an option with the specified longName exists.
             */
			bool containsOptionByLongName(std::string const& longName) const {
				return this->containsLongName(longName);
			}
            
			/*!
             * Returns true IFF an option with the specified shortName exists.
             */
			bool containsOptionByShortName(std::string const& shortName) const {
				return this->containsLongName(shortName);
			}
            
			/*!
             * Returns a reference to the Option with the specified longName.
             * Throws an Exception of Type IllegalArgumentException if there is no such Option.
             */
			Option const& getOptionByLongName(std::string const& longName) const {
				return this->getByLongName(longName);
			}
            
			/*!
             * Returns a reference to the Option with the specified shortName.
             * Throws an Exception of Type IllegalArgumentException if there is no such Option.
             */
			Option const& getOptionByShortName(std::string const& shortName) const {
				return this->getByShortName(shortName);
			}
			
			/*!
			 * Adds the given option to the set of known options.
			 * Unifying with existing options is done automatically.
			 * Ownership of the Option is handed over when calling this function!
			 * Returns a reference to the settings instance
			 * @throws OptionUnificationException
			 */
			SettingsManager& addOption(Option* option);
            
			/*!
			 * Returns true iff there is an Option with the specified longName and it has been set
			 * @return bool true if the option exists and has been set
			 * @throws InvalidArgumentException
			 */
			bool isSet(std::string const& longName) const {
				return this->getByLongName(longName).getHasOptionBeenSet();
			}
            
			/*!
			 * This generated a list of all registered options and their arguments together with descriptions and defaults.
			 * @return A std::string containing the help text, delimited by \n
			 */
			std::string getHelpText() const;
            
            /*!
             * Registers a new module with the given name and options. If the module could not be successfully registered,
             * an exception is thrown.
             *
             * @param moduleName The name of the module to register.
             * @param options The set of options that the module registers.
             */
            void registerModule(std::string const& moduleName, std::vector<std::shared_ptr<Option>> const& options);
            
			/*!
			 * Retrieves the only existing instance of a settings manager.
             *
			 * @return The only existing instance of a settings manager
			 */
			static SettingsManager& manager();
            
        private:
			/*!
			 * Constructs a new manager. This constructor is private to forbid instantiation of this class. The only
             * way to create a new instance is by calling the static manager() method.
			 */
            SettingsManager();
			
			/*!
			 * This destructor is private, since we need to forbid explicit destruction of the manager.
			 */
			virtual ~SettingsManager();
 			
            // An object that provides access to the general settings.
            storm::settings::modules::GeneralSettings generalSettings;
            
            // An object that provides access to the debug settings.
            storm::settings::modules::DebugSettings debugSettings;

            // An object that provides access to the counterexample generator settings.
            storm::settings::modules::CounterexampleGeneratorSettings counterexampleGeneratorSettings;

            // An object that provides access to the CUDD settings.
            storm::settings::modules::CuddSettings cuddSettings;

            // An object that provides access to the gmm++ settings.
            storm::settings::modules::GmmxxSettings gmmxxSettings;

            // An object that provides access to the native equation solver settings.
            storm::settings::modules::NativeEquationSolverSettings nativeEquationSolverSettings;

            // An object that provides access to the glpk settings.
            storm::settings::modules::GlpkSettings glpkSettings;

            // An object that provides access to the Gurobi settings.
            storm::settings::modules::GurobiSettings gurobiSettings;
            
            // The single instance of the manager class. Since it's static, it will automatically be distructed upon termination.
 			static SettingsManager settingsManager;
            
            
            
            /*!
			 * Parser for the commdand line parameters of the program.
			 * The first entry of argv will be ignored, as it represents the program name.
			 * @throws OptionParserException
			 */
			void parseCommandLine(int const argc, char const * const argv[]);
            
			/*!
             * The map holding the information regarding registered options and their types
             */
			std::unordered_map<std::string, std::shared_ptr<Option>> options;
            
			/*!
             * The vector holding a pointer to all options
             */
			std::vector<std::shared_ptr<Option>> optionPointers;
            
			/*!
             * The map holding the information regarding registered options and their short names
             */
			std::unordered_map<std::string, std::string> shortNames;
            
			// Helper functions
			stringPair_t splitOptionString(std::string const& option);
			bool hasAssignment(std::string const& option);
			void handleAssignment(std::string const& longOptionName, std::vector<std::string> arguments);
			std::vector<std::string> argvToStringArray(int const argc, char const * const argv[]);
			std::vector<bool> scanForOptions(std::vector<std::string> const& arguments);
			bool checkArgumentSyntaxForOption(std::string const& argvString);
            
			/*!
             * Returns true IFF this contains an option with the specified longName.
             * @return bool true iff there is an option with the specified longName
             */
			bool containsLongName(std::string const& longName) const {
				return (this->options.find(longName) != this->options.end());
			}
            
			/*!
             * Returns true IFF this contains an option with the specified shortName.
             * @return bool true iff there is an option with the specified shortName
             */
			bool containsShortName(std::string const& shortName) const {
				return (this->shortNames.find(shortName) != this->shortNames.end());
			}
            
			/*!
             * Returns a reference to the Option with the specified longName.
             * Throws an Exception of Type InvalidArgumentException if there is no such Option.
             * @throws InvalidArgumentException
             */
			Option& getByLongName(std::string const& longName) const {
				auto longNameIterator = this->options.find(longName);
				if (longNameIterator == this->options.end()) {
					LOG4CPLUS_ERROR(logger, "Settings::getByLongName: This program does not contain an option named \"" << longName << "\".");
					throw storm::exceptions::IllegalArgumentException() << "This program does not contain an option named \"" << longName << "\".";
				}
				return *longNameIterator->second.get();
			}
            
			/*!
             * Returns a pointer to the Option with the specified longName.
             * Throws an Exception of Type InvalidArgumentException if there is no such Option.
             * @throws InvalidArgumentException
             */
			Option* getPtrByLongName(std::string const& longName) const {
				auto longNameIterator = this->options.find(longName);
				if (longNameIterator == this->options.end()) {
					LOG4CPLUS_ERROR(logger, "Settings::getPtrByLongName: This program does not contain an option named \"" << longName << "\".");
					throw storm::exceptions::IllegalArgumentException() << "This program does not contain an option named \"" << longName << "\".";
				}
				return longNameIterator->second.get();
			}
            
			/*!
             * Returns a reference to the Option with the specified shortName.
             * Throws an Exception of Type InvalidArgumentException if there is no such Option.
             * @throws InvalidArgumentException
             */
			Option& getByShortName(std::string const& shortName) const {
				auto shortNameIterator = this->shortNames.find(shortName);
				if (shortNameIterator == this->shortNames.end()) {
					LOG4CPLUS_ERROR(logger, "Settings::getByShortName: This program does not contain an option named \"" << shortName << "\".");
					throw storm::exceptions::IllegalArgumentException() << "This program does not contain an option named \"" << shortName << "\"";
				}
				return *(this->options.find(shortNameIterator->second)->second.get());
			}
            
			/*!
             * Returns a pointer to the Option with the specified shortName.
             * Throws an Exception of Type InvalidArgumentException if there is no such Option.
             * @throws InvalidArgumentException
             */
			Option* getPtrByShortName(std::string const& shortName) const {
				auto shortNameIterator = this->shortNames.find(shortName);
				if (shortNameIterator == this->shortNames.end()) {
					LOG4CPLUS_ERROR(logger, "Settings::getPtrByShortName: This program does not contain an option named \"" << shortName << "\".");
					throw storm::exceptions::IllegalArgumentException() << "This program does not contain an option named \"" << shortName << "\".";
				}
				return this->options.find(shortNameIterator->second)->second.get();
			}
            
			/*!
			 * Sets the Option with the specified longName
			 * This function requires the Option to have no arguments
			 * This is for TESTING only and should not be used outside of the testing code!
			 * @throws InvalidArgumentException
			 */
			void set(std::string const& longName) const {
				return this->getByLongName(longName).setHasOptionBeenSet();
			}
            
			/*!
			 * Unsets the Option with the specified longName
			 * This function requires the Option to have no arguments
			 * This is for TESTING only and should not be used outside of the testing code!
			 * @throws InvalidArgumentException
			 */
			void unset(std::string const& longName) const {
				return this->getByLongName(longName).setHasOptionBeenSet(false);
			}
        };
        
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_SETTINGSMANAGER_H_ */