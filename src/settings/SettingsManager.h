#ifndef STORM_SETTINGS_SETTINGSMANAGER_H_
#define STORM_SETTINGS_SETTINGSMANAGER_H_

#include <iostream>
#include <utility>
#include <functional>
#include <unordered_map>
#include <map>
#include <vector>
#include <memory>

namespace storm {
    namespace settings {
        namespace modules {
            class GeneralSettings;
            class DebugSettings;
            class CounterexampleGeneratorSettings;
            class CuddSettings;
            class SylvanSettings;
            class GmmxxEquationSolverSettings;
            class NativeEquationSolverSettings;
            class BisimulationSettings;
            class GlpkSettings;
            class GurobiSettings;
            class TopologicalValueIterationEquationSolverSettings;
            class ParametricSettings;
            class SparseDtmcEliminationModelCheckerSettings;
            class ModuleSettings;
        }
        class Option;
        
        /*!
         * Provides the central API for the registration of command line options and parsing the options from the
         * command line. Since this class is a singleton, the only instance is accessible via a call to the manager()
         * function.
         */
        class SettingsManager {
		public:
            
            // Explicitely delete copy constructor
            SettingsManager(SettingsManager const&) = delete;
            void operator=(SettingsManager const&) = delete;
            
            /*!
             * This function parses the given command line arguments and sets all registered options accordingly. If the
             * command line cannot be matched using the known options, an exception is thrown.
             *
             * @param argc The number of command line arguments.
             * @param argv The command line arguments.
             */
			void setFromCommandLine(int const argc, char const * const argv[]);
            
            /*!
             * This function parses the given command line arguments (represented by one big string) and sets all
             * registered options accordingly. If the command line cannot be matched using the known options, an
             * exception is thrown.
             *
             * @param commandLineString The command line arguments as one string.
             */
            void setFromString(std::string const& commandLineString);
            
            /*!
             * This function parses the given command line arguments (represented by several strings) and sets all
             * registered options accordingly. If the command line cannot be matched using the known options, an
             * exception is thrown.
             *
             * @param commandLineArguments The command line arguments.
             */
            void setFromExplodedString(std::vector<std::string> const& commandLineArguments);
            
            /*!
             * This function parses the given file and sets all registered options accordingly. If the settings cannot
             * be matched using the known options, an exception is thrown.
             */
            void setFromConfigurationFile(std::string const& configFilename);
            
            /*!
             * This function prints a help message to the standard output. Optionally, a string can be given as a hint.
             * If it is 'all', the complete help is shown. Otherwise, the string is interpreted as a regular expression
             * and matched against the known modules and options to restrict the help output.
             *
             * @param hint A regular expression to restrict the help output or "all" for the full help text.
             */
            void printHelp(std::string const& hint = "all") const;
            
            /*!
             * This function prints a help message for the specified module to the standard output.
             *
             * @param moduleName The name of the module for which to show the help.
             * @param maxLength The maximal length of an option name (necessary for proper alignment).
             */
            void printHelpForModule(std::string const& moduleName, uint_fast64_t maxLength = 30) const;
            
			/*!
			 * This function prints the version string to the command line.
			 */
			void printVersion() const;
			
            /*!
             * Retrieves the only existing instance of a settings manager.
             *
             * @return The only existing instance of a settings manager
             */
            static SettingsManager& manager();
            
            /*!
             * Adds a new module with the given name. If the module could not be successfully added, an exception is
             * thrown.
             *
             * @param moduleSettings The settings of the module to add.
             */
            void addModule(std::unique_ptr<modules::ModuleSettings>&& moduleSettings);
            
            /*!
             * Retrieves the settings of the module with the given name.
             *
             * @param moduleName The name of the module for which to retrieve the settings.
             * @return An object that provides access to the settings of the module.
             */
            modules::ModuleSettings const& getModule(std::string const& moduleName) const;

            /*!
             * Retrieves the settings of the module with the given name.
             *
             * @param moduleName The name of the module for which to retrieve the settings.
             * @return An object that provides access to the settings of the module.
             */
            modules::ModuleSettings& getModule(std::string const& moduleName);
            
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
            
            // The registered modules.
            std::vector<std::string> moduleNames;
            std::unordered_map<std::string, std::unique_ptr<modules::ModuleSettings>> modules;

            // Mappings from all known option names to the options that match it. All options for one option name need
            // to be compatible in the sense that calling isCompatible(...) pairwise on all options must always return true.
            std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> longNameToOptions;
            std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> shortNameToOptions;
            
            // A mapping of module names to the corresponding options.
            std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> moduleOptions;
            
            // A list of long option names to keep the order in which they were registered. This is, for example, used
            // to match the regular expression given to the help option against the option names.
            std::vector<std::string> longOptionNames;
            
            /*!
             * Adds the given option to the known options.
             *
             * @param option The option to add.
             */
            void addOption(std::shared_ptr<Option> const& option);
            
            /*!
             * Sets the arguments of the given option from the provided strings.
             *
             * @param optionName The name of the option. This is only used for error output.
             * @param option The option for which to set the arguments.
             * @param argumentCache The arguments of the option as string values.
             */
            static void setOptionArguments(std::string const& optionName, std::shared_ptr<Option> option, std::vector<std::string> const& argumentCache);
            
            /*!
             * Sets the arguments of the options matching the given name from the provided strings.
             *
             * @param optionName The name of the options for which to set the arguments.
             * @param optionMap The mapping from option names to options.
             * @param argumentCache The arguments of the option as string values.
             */
            static void setOptionsArguments(std::string const& optionName, std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> const& optionMap, std::vector<std::string> const& argumentCache);
            
            /*!
             * Checks whether the given option is compatible with all options with the given name in the given mapping.
             */
            static bool isCompatible(std::shared_ptr<Option> const& option, std::string const& optionName, std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> const& optionMap);
            
            /*!
             * Inserts the given option to the options with the given name in the given map.
             *
             * @param name The name of the option.
             * @param option The option to add.
             * @param optionMap The map into which the option is to be inserted.
             */
            static void addOptionToMap(std::string const& name, std::shared_ptr<Option> const& option, std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>>& optionMap);
            
            /*!
             * Prepares some modules for further changes.
             * Checks all modules for consistency by calling their respective check method.
             */
            void finalizeAllModules();
            
            /*!
             * Retrieves the (print) length of the longest option of all modules.
             *
             * @return The length of the longest option.
             */
            uint_fast64_t getPrintLengthOfLongestOption() const;
            
            /*!
             * Retrieves the (print) length of the longest option in the given module, so we can align the options.
             *
             * @param moduleName The module name for which to retrieve the length of the longest option.
             * @return The length of the longest option name.
             */
            uint_fast64_t getPrintLengthOfLongestOption(std::string const& moduleName) const;
            
            /*!
             * Parses the given file and stores the settings in the returned map.
             *
             * @param filename The name of the file that is to be scanned for settings.
             * @return A mapping of option names to the argument values (represented as strings).
             */
            std::map<std::string, std::vector<std::string>> parseConfigFile(std::string const& filename) const;
        };
        
        /*!
         * Retrieves the settings manager.
         *
         * @return The only settings manager.
         */
        SettingsManager const& manager();

        /*!
         * Retrieves the settings manager.
         *
         * @return The only settings manager.
         */
        SettingsManager& mutableManager();
        
        /*!
         * Add new module to use for the settings. The new module is given as a template argument.
         */
        template<typename SettingsType>
        void addModule() {
            static_assert(std::is_base_of<storm::settings::modules::ModuleSettings, SettingsType>::value, "Template argument must be derived from ModuleSettings");
            mutableManager().addModule(std::unique_ptr<modules::ModuleSettings>(new SettingsType()));
        }
        
        /*!
         * Get module. The type of the module is given as a template argument.
         *
         * @return The module.
         */
        template<typename SettingsType>
        SettingsType getModule() {
            static_assert(std::is_base_of<storm::settings::modules::ModuleSettings, SettingsType>::value, "Template argument must be derived from ModuleSettings");
            return dynamic_cast<SettingsType const&>(manager().getModule(SettingsType::moduleName));
        }
        
        /*!
         * Retrieves the general settings in a mutable form. This is only meant to be used for debug purposes or very
         * rare cases where it is necessary.
         *
         * @return An object that allows accessing and modifying the general settings.
         */
        storm::settings::modules::GeneralSettings& mutableGeneralSettings();
        
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_SETTINGSMANAGER_H_ */