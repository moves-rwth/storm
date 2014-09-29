#include "src/settings/SettingsManager.h"

#include <cstring>
#include <cctype>
#include <mutex>
#include <iomanip>
#include <regex>
#include <set>
#include <boost/algorithm/string.hpp>
#include <boost/io/ios_state.hpp>

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/OptionParserException.h"

namespace storm {
    namespace settings {
        
        SettingsManager::SettingsManager() : modules(), longNameToOptions(), shortNameToOptions(), moduleOptions() {
            // Register all known settings modules.
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GeneralSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::DebugSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::CounterexampleGeneratorSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::CuddSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GmmxxEquationSolverSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::NativeEquationSolverSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GlpkSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GurobiSettings(*this)));
        }
        
        SettingsManager::~SettingsManager() {
            // Intentionally left empty.
        }
        
        SettingsManager& SettingsManager::manager() {
            static SettingsManager settingsManager;
            return settingsManager;
        }
        
        void SettingsManager::setFromCommandLine(int const argc, char const * const argv[]) {
            // We convert the arguments to a vector of strings and strip off the first element since it refers to the
            // name of the program.
            std::vector<std::string> argumentVector(argc - 1);
            for (uint_fast64_t i = 1; i < argc; ++i) {
                argumentVector[i - 1] = std::string(argv[i]);
            }
            
            this->setFromExplodedString(argumentVector);
        }
        
        void SettingsManager::setFromString(std::string const& commandLineString) {
            if (commandLineString.empty()) {
                return;
            }
            std::vector<std::string> argumentVector;
            boost::split(argumentVector, commandLineString, boost::is_any_of("\t "));
            this->setFromExplodedString(argumentVector);
        }
        
        void SettingsManager::setFromExplodedString(std::vector<std::string> const& commandLineArguments) {
            // In order to assign the parsed arguments to an option, we need to keep track of the "active" option's name.
            bool optionActive = false;
            bool activeOptionIsShortName = false;
            std::string activeOptionName = "";
            std::vector<std::string> argumentCache;
            
            // Walk through all arguments.
            for (uint_fast64_t i = 0; i < commandLineArguments.size(); ++i) {
                bool existsNextArgument = i < commandLineArguments.size() - 1;
                std::string const& currentArgument = commandLineArguments[i];
                
                // Check if the given argument is a new option or belongs to a previously given option.
                if (currentArgument.at(0) == '-') {
                    if (optionActive) {
                        // At this point we know that a new option is about to come. Hence, we need to assign the current
                        // cache content to the option that was active until now.
                        setOptionsArguments(activeOptionName, activeOptionIsShortName ? this->shortNameToOptions : this->longNameToOptions, argumentCache);
                        
                        // After the assignment, the argument cache needs to be cleared.
                        argumentCache.clear();
                    } else {
                        optionActive = true;
                    }
                    
                    if (currentArgument.at(1) == '-') {
                        // In this case, the argument has to be the long name of an option. Try to get all options that
                        // match the long name.
                        std::string optionName = currentArgument.substr(2);
                        auto optionIterator = this->longNameToOptions.find(optionName);
                        STORM_LOG_THROW(optionIterator != this->longNameToOptions.end(), storm::exceptions::OptionParserException, "Unknown option '" << optionName << "'.");
                        activeOptionIsShortName = false;
                        activeOptionName = optionName;
                    } else {
                        // In this case, the argument has to be the short name of an option. Try to get all options that
                        // match the short name.
                        std::string optionName = currentArgument.substr(1);
                        auto optionIterator = this->shortNameToOptions.find(optionName);
                        STORM_LOG_THROW(optionIterator != this->shortNameToOptions.end(), storm::exceptions::OptionParserException, "Unknown option '" << optionName << "'.");
                        activeOptionIsShortName = true;
                        activeOptionName = optionName;
                    }
                } else if (optionActive) {
                    // Add the current argument to the list of arguments for the currently active options.
                    argumentCache.push_back(currentArgument);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::OptionParserException, "Found stray argument '" << currentArgument << "' that is not preceeded by a matching option.");
                }
            }
            
            // If an option is still active at this point, we need to set it.
            if (optionActive) {
                setOptionsArguments(activeOptionName, activeOptionIsShortName ? this->shortNameToOptions : this->longNameToOptions, argumentCache);
            }
        }
        
        void SettingsManager::setFromConfigurationFile(std::string const& configFilename) {
            STORM_LOG_ASSERT(false, "Not yet implemented.");
        }
        
        void SettingsManager::printHelp(std::string const& hint) const {
            std::cout << "usage: storm [options]" << std::endl << std::endl;
            
            if (hint == "all") {
                // Find longest option name.
                uint_fast64_t maxLength = getPrintLengthOfLongestOption();
                for (auto const& moduleName : this->moduleNames) {
                    printHelpForModule(moduleName, maxLength);
                }
            } else {
                // Create a regular expression from the input hint.
                std::regex hintRegex(hint, std::regex_constants::ECMAScript | std::regex_constants::icase);
                
                // Remember which options we printed, so we don't display options twice.
                std::set<std::shared_ptr<Option>> printedOptions;
                
                // Try to match the regular expression against the known modules.
                std::vector<std::string> matchingModuleNames;
                uint_fast64_t maxLengthModules = 0;
                for (auto const& moduleName : this->moduleNames) {
                    if (std::regex_search(moduleName, hintRegex)) {
                        matchingModuleNames.push_back(moduleName);
                        maxLengthModules = std::max(maxLengthModules, getPrintLengthOfLongestOption(moduleName));
                        
                        // Add all options of this module to the list of printed options so we don't print them twice.
                        auto optionIterator = this->moduleOptions.find(moduleName);
                        printedOptions.insert(optionIterator->second.begin(), optionIterator->second.end());
                    }
                }

                // Try to match the regular expression against the known options.
                std::vector<std::shared_ptr<Option>> matchingOptions;
                uint_fast64_t maxLengthOptions = 0;
                for (auto const& optionName : this->longOptionNames) {
                    if (std::regex_search(optionName, hintRegex)) {
                        auto optionIterator = this->longNameToOptions.find(optionName);
                        for (auto const& option : optionIterator->second) {
                            // Only add the option if we have not already added it to the list of options that is going
                            // to be printed anyway.
                            if (printedOptions.find(option) == printedOptions.end()) {
                                maxLengthOptions = std::max(maxLengthOptions, option->getPrintLength());
                                matchingOptions.push_back(option);
                            }
                        }
                    }
                }
                
                // Print the matching modules.
                uint_fast64_t maxLength = std::max(maxLengthModules, maxLengthOptions);
                if (matchingModuleNames.size() > 0) {
                    std::cout << "Matching modules for hint '" << hint << "':" << std::endl;
                    for (auto const& matchingModuleName : matchingModuleNames) {
                        printHelpForModule(matchingModuleName, maxLength);
                    }
                }
                
                // Print the matching options.
                if (matchingOptions.size() > 0) {
                    std::cout << "Matching options for hint '" << hint << "':" << std::endl;
                    for (auto const& option : matchingOptions) {
                        std::cout << std::setw(maxLength) << std::left << *option << std::endl;
                    }
                }
                
                if (matchingModuleNames.empty() && matchingOptions.empty()) {
                    std::cout << "Hint '" << hint << "' did not match any modules or options." << std::endl;
                }
            }
        }
        
        void SettingsManager::printHelpForModule(std::string const& moduleName, uint_fast64_t maxLength) const {
            auto moduleIterator = moduleOptions.find(moduleName);
            STORM_LOG_THROW(moduleIterator != moduleOptions.end(), storm::exceptions::IllegalFunctionCallException, "Cannot print help for unknown module '" << moduleName << "'.");
            std::cout << "##### Module '" << moduleName << "' ";
            for (uint_fast64_t i = 0; i < std::min(maxLength, maxLength - moduleName.length() - 16); ++i) {
                std::cout << "#";
            }
            std::cout << std::endl;
            
            // Save the flags for std::cout so we can manipulate them and be sure they will be restored as soon as this
            // stream goes out of scope.
            boost::io::ios_flags_saver out(std::cout);
            
            for (auto const& option : moduleIterator->second) {
                std::cout << std::setw(maxLength) << std::left << *option << std::endl;
            }
            std::cout << std::endl;
        }
        
        uint_fast64_t SettingsManager::getPrintLengthOfLongestOption() const {
            uint_fast64_t length = 0;
            for (auto const& moduleName : this->moduleNames) {
                length = std::max(getPrintLengthOfLongestOption(moduleName), length);
            }
            return length;
        }
        
        uint_fast64_t SettingsManager::getPrintLengthOfLongestOption(std::string const& moduleName) const {
            auto moduleIterator = modules.find(moduleName);
            STORM_LOG_THROW(moduleIterator != modules.end(), storm::exceptions::IllegalFunctionCallException, "Unable to retrieve option length of unknown module '" << moduleName << "'.");
            return moduleIterator->second->getPrintLengthOfLongestOption();
        }
        
        void SettingsManager::addModule(std::unique_ptr<modules::ModuleSettings>&& moduleSettings) {
            auto moduleIterator = this->modules.find(moduleSettings->getModuleName());
            STORM_LOG_THROW(moduleIterator == this->modules.end(), storm::exceptions::IllegalFunctionCallException, "Unable to register module '" << moduleSettings->getModuleName() << "' because a module with the same name already exists.");
            
            // Take over the module settings object.
            std::string const& moduleName = moduleSettings->getModuleName();
            this->moduleNames.push_back(moduleName);
            this->modules.emplace(moduleSettings->getModuleName(), std::move(moduleSettings));
            auto iterator = this->modules.find(moduleName);
            std::unique_ptr<modules::ModuleSettings> const& settings = iterator->second;
            
            // Now register the options of the module.
            this->moduleOptions.emplace(moduleName, std::vector<std::shared_ptr<Option>>());
            for (auto const& option : settings->getOptions()) {
                this->addOption(option);
            }
        }
        
        void SettingsManager::addOption(std::shared_ptr<Option> const& option) {
            // First, we register to which module the given option belongs.
            auto moduleOptionIterator = this->moduleOptions.find(option->getModuleName());
            STORM_LOG_THROW(moduleOptionIterator != this->moduleOptions.end(), storm::exceptions::IllegalFunctionCallException, "Cannot add option for unknown module '" << option->getModuleName() << "'.");
            moduleOptionIterator->second.emplace_back(option);
            
            // Then, we add the option's name (and possibly short name) to the registered options. If a module prefix is
            // not required for this option, we have to add both versions to our mappings, the prefixed one and the
            // non-prefixed one.
            if (!option->getRequiresModulePrefix()) {
                bool isCompatible = this->isCompatible(option, option->getLongName(), this->longNameToOptions);
                STORM_LOG_THROW(isCompatible, storm::exceptions::IllegalFunctionCallException, "Unable to add option '" << option->getLongName() << "', because an option with the same name is incompatible with it.");
                addOptionToMap(option->getLongName(), option, this->longNameToOptions);
            }
            // For the prefixed name, we don't need a compatibility check, because a module is not allowed to register the same option twice.
            addOptionToMap(option->getModuleName() + ":" + option->getLongName(), option, this->longNameToOptions);
            longOptionNames.push_back(option->getModuleName() + ":" + option->getLongName());
            
            if (option->getHasShortName()) {
                if (!option->getRequiresModulePrefix()) {
                    bool isCompatible = this->isCompatible(option, option->getShortName(), this->shortNameToOptions);
                    STORM_LOG_THROW(isCompatible, storm::exceptions::IllegalFunctionCallException, "Unable to add option '" << option->getLongName() << "', because an option with the same name is incompatible with it.");
                    addOptionToMap(option->getShortName(), option, this->shortNameToOptions);
                }
                addOptionToMap(option->getModuleName() + ":" + option->getShortName(), option, this->shortNameToOptions);
            }
        }
        
        modules::ModuleSettings const& SettingsManager::getModule(std::string const& moduleName) const {
            auto moduleIterator = this->modules.find(moduleName);
            STORM_LOG_THROW(moduleIterator != this->modules.end(), storm::exceptions::IllegalFunctionCallException, "Cannot retrieve unknown module '" << moduleName << "'.");
            return *moduleIterator->second;
        }
        
        modules::ModuleSettings& SettingsManager::getModule(std::string const& moduleName) {
            auto moduleIterator = this->modules.find(moduleName);
            STORM_LOG_THROW(moduleIterator != this->modules.end(), storm::exceptions::IllegalFunctionCallException, "Cannot retrieve unknown module '" << moduleName << "'.");
            return *moduleIterator->second;
        }
        
        bool SettingsManager::isCompatible(std::shared_ptr<Option> const& option, std::string const& optionName, std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> const& optionMap) {
            auto optionIterator = optionMap.find(optionName);
            if (optionIterator != optionMap.end()) {
                for (auto const& otherOption : optionIterator->second) {
                    bool locallyCompatible = option->isCompatibleWith(*otherOption);
                    if (!locallyCompatible) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        void SettingsManager::setOptionsArguments(std::string const& optionName, std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> const& optionMap, std::vector<std::string> const& argumentCache) {
            auto optionIterator = optionMap.find(optionName);
            STORM_LOG_THROW(optionIterator != optionMap.end(), storm::exceptions::OptionParserException, "Unknown option '" << optionName << "'.");
            
            // Iterate over all options and set the arguments.
            for (auto& option : optionIterator->second) {
                option->setHasOptionBeenSet();
                STORM_LOG_THROW(argumentCache.size() <= option->getArgumentCount(), storm::exceptions::OptionParserException, "Too many arguments for option '" << optionName << "'.");
                
                // Now set the provided argument values one by one.
                for (uint_fast64_t i = 0; i < argumentCache.size(); ++i) {
                    ArgumentBase& argument = option->getArgument(i);
                    bool conversionOk = argument.setFromStringValue(argumentCache[i]);
                    STORM_LOG_THROW(conversionOk, storm::exceptions::OptionParserException, "Conversion of value of argument '" << argument.getName() << "' to its type failed.");
                }
                
                // In case there are optional arguments that were not set, we set them to their default value.
                for (uint_fast64_t i = argumentCache.size(); i < option->getArgumentCount(); ++i) {
                    ArgumentBase& argument = option->getArgument(i);
                    argument.setFromDefaultValue();
                }
            }
        }
        
        void SettingsManager::addOptionToMap(std::string const& name, std::shared_ptr<Option> const& option, std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>>& optionMap) {
            auto optionIterator = optionMap.find(name);
            if (optionIterator == optionMap.end()) {
                std::vector<std::shared_ptr<Option>> optionVector;
                optionVector.push_back(option);
                optionMap.emplace(name, optionVector);
            } else {
                optionIterator->second.push_back(option);
            }
        }
        
        SettingsManager const& manager() {
            return SettingsManager::manager();
        }
        
        SettingsManager& mutableManager() {
            return SettingsManager::manager();
        }
        
        storm::settings::modules::GeneralSettings const& generalSettings() {
            return dynamic_cast<storm::settings::modules::GeneralSettings const&>(manager().getModule(storm::settings::modules::GeneralSettings::moduleName));
        }
        
        storm::settings::modules::GeneralSettings& mutableGeneralSettings() {
            return dynamic_cast<storm::settings::modules::GeneralSettings&>(storm::settings::SettingsManager::manager().getModule(storm::settings::modules::GeneralSettings::moduleName));
        }
        
        storm::settings::modules::DebugSettings const& debugSettings()  {
            return dynamic_cast<storm::settings::modules::DebugSettings const&>(manager().getModule(storm::settings::modules::DebugSettings::moduleName));
        }
        
        storm::settings::modules::CounterexampleGeneratorSettings const& counterexampleGeneratorSettings() {
            return dynamic_cast<storm::settings::modules::CounterexampleGeneratorSettings const&>(manager().getModule(storm::settings::modules::CounterexampleGeneratorSettings::moduleName));
        }
        
        storm::settings::modules::CuddSettings const& cuddSettings() {
            return dynamic_cast<storm::settings::modules::CuddSettings const&>(manager().getModule(storm::settings::modules::CuddSettings::moduleName));
        }
        
        storm::settings::modules::GmmxxEquationSolverSettings const& gmmxxEquationSolverSettings() {
            return dynamic_cast<storm::settings::modules::GmmxxEquationSolverSettings const&>(manager().getModule(storm::settings::modules::GmmxxEquationSolverSettings::moduleName));
        }
        
        storm::settings::modules::NativeEquationSolverSettings const& nativeEquationSolverSettings() {
            return dynamic_cast<storm::settings::modules::NativeEquationSolverSettings const&>(manager().getModule(storm::settings::modules::NativeEquationSolverSettings::moduleName));
        }
        
        storm::settings::modules::GlpkSettings const& glpkSettings() {
            return dynamic_cast<storm::settings::modules::GlpkSettings const&>(manager().getModule(storm::settings::modules::GlpkSettings::moduleName));
        }
        
        storm::settings::modules::GurobiSettings const& gurobiSettings() {
            return dynamic_cast<storm::settings::modules::GurobiSettings const&>(manager().getModule(storm::settings::modules::GurobiSettings::moduleName));
        }
    }
}