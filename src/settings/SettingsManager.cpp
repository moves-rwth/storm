#include "src/settings/SettingsManager.h"

#include <cstring>
#include <cctype>
#include <mutex>
#include <boost/algorithm/string.hpp>

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
                    // At this point we know that a new option is about to come. Hence, we need to assign the current
                    // cache content to the option that was active until now.
                    setOptionsArguments(activeOptionName, activeOptionIsShortName ? this->longNameToOptions : this->shortNameToOptions, argumentCache);
                    
                    if (currentArgument.at(1) == '-') {
                        // In this case, the argument has to be the long name of an option. Try to get all options that
                        // match the long name.
                        std::string optionName = currentArgument.substr(2);
                        auto optionIterator = this->longNameToOptions.find(optionName);
                        LOG_THROW(optionIterator != this->longNameToOptions.end(), storm::exceptions::OptionParserException, "Unknown option '" << optionName << "'.");
                        activeOptionIsShortName = false;
                        activeOptionName = optionName;
                    } else {
                        // In this case, the argument has to be the short name of an option. Try to get all options that
                        // match the short name.
                        std::string optionName = currentArgument.substr(1);
                        auto optionIterator = this->shortNameToOptions.find(optionName);
                        LOG_THROW(optionIterator != this->shortNameToOptions.end(), storm::exceptions::OptionParserException, "Unknown option '" << optionName << "'.");
                        activeOptionIsShortName = true;
                        activeOptionName = optionName;
                    }
                } else if (optionActive) {
                    // Add the current argument to the list of arguments for the currently active options.
                    argumentCache.push_back(currentArgument);
                } else {
                    LOG_THROW(false, storm::exceptions::OptionParserException, "Found stray argument '" << currentArgument << "' that is not preceeded by a matching option.");
                }
            }
        }
        
        void SettingsManager::setFromConfigurationFile(std::string const& configFilename) {
            LOG_ASSERT(false, "Not yet implemented");
        }
                
        void SettingsManager::printHelp(std::string const& moduleName) const {
            LOG_ASSERT(false, "Not yet implemented");
        }
        
        void SettingsManager::addModule(std::unique_ptr<modules::ModuleSettings>&& moduleSettings) {
            auto moduleIterator = this->modules.find(moduleSettings->getModuleName());
            LOG_THROW(moduleIterator == this->modules.end(), storm::exceptions::IllegalFunctionCallException, "Unable to register module '" << moduleSettings->getModuleName() << "' because a module with the same name already exists.");
            this->modules.emplace(moduleSettings->getModuleName(), std::move(moduleSettings));
            
            for (auto const& option : moduleSettings->getOptions()) {
                this->addOption(option);
            }
        }

        void SettingsManager::addOption(std::shared_ptr<Option> const& option) {
            // First, we register to which module the given option belongs.
            auto moduleOptionIterator = this->moduleOptions.find(option->getModuleName());
            LOG_THROW(moduleOptionIterator != this->moduleOptions.end(), storm::exceptions::IllegalFunctionCallException, "Cannot add option for unknown module '" << option->getModuleName() << "'.");
            moduleOptionIterator->second.emplace_back(option);
            
            // Then, we add the option's name (and possibly short name) to the registered options. If a module prefix is
            // not required for this option, we have to add both versions to our mappings, the prefixed one and the
            // non-prefixed one.
            if (!option->getRequiresModulePrefix()) {
                bool isCompatible = this->isCompatible(option, option->getLongName(), this->longNameToOptions);
                LOG_THROW(isCompatible, storm::exceptions::IllegalFunctionCallException, "Unable to add option '" << option->getLongName() << "', because an option with the same name is incompatible with it.");
                this->longNameToOptions.emplace(option->getLongName(), option);
            }
            // For the prefixed name, we don't need a compatibility check, because a module is not allowed to register the same option twice.
            this->longNameToOptions.emplace(option->getModuleName() + ":" + option->getLongName(), option);
            
            if (option->getHasShortName()) {
                if (!option->getRequiresModulePrefix()) {
                    this->shortNameToOptions.emplace(option->getShortName(), option);
                    bool isCompatible = this->isCompatible(option, option->getShortName(), this->shortNameToOptions);
                    LOG_THROW(isCompatible, storm::exceptions::IllegalFunctionCallException, "Unable to add option '" << option->getLongName() << "', because an option with the same name is incompatible with it.");
                }
                this->shortNameToOptions.emplace(option->getModuleName() + ":" + option->getShortName(), option);
            }
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
            LOG_THROW(optionIterator != optionMap.end(), storm::exceptions::OptionParserException, "Unknown option '" << optionName << "'.");
            
            // Iterate over all options and set the arguments.
            for (auto& option : optionIterator->second) {
                option->setHasOptionBeenSet();
                LOG_THROW(option->getArgumentCount() <= argumentCache.size(), storm::exceptions::OptionParserException, "Unknown option '" << optionName << "'.");
                
                // Now set the provided argument values one by one.
                for (uint_fast64_t i = 0; i < argumentCache.size(); ++i) {
                    ArgumentBase& argument = option->getArgument(i);
                    argument.setFromStringValue(argumentCache[i]);
                }
                
                // In case there are optional arguments that were not set, we set them to their default value.
                for (uint_fast64_t i = argumentCache.size(); i < option->getArgumentCount(); ++i) {
                    ArgumentBase& argument = option->getArgument(i);
                    argument.setFromDefaultValue();
                }
            }
        }
        
    }
}