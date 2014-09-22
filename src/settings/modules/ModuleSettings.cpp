#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            ModuleSettings::ModuleSettings(storm::settings::SettingsManager& settingsManager, std::string const& moduleName) : settingsManager(settingsManager), moduleName(moduleName) {
                // Intentionally left empty.
            }
            
            bool ModuleSettings::check() const {
                return true;
            }
            
            storm::settings::SettingsManager const& ModuleSettings::getSettingsManager() const {
                return this->settingsManager;
            }
            
            void ModuleSettings::set(std::string const& name) {
                return this->getOption(name).setHasOptionBeenSet();
            }
            
            void ModuleSettings::unset(std::string const& name) {
                return this->getOption(name).setHasOptionBeenSet(false);
            }
            
            std::vector<std::shared_ptr<Option>> ModuleSettings::getOptions() const {
                std::vector<std::shared_ptr<Option>> result;
                result.reserve(this->options.size());
                
                for (auto const& option : this->options) {
                    result.push_back(option.second);
                }
                
                return result;
            }
            
            Option const& ModuleSettings::getOption(std::string const& longName) const {
                auto optionIterator = this->options.find(longName);
                LOG_THROW(optionIterator != this->options.end(), storm::exceptions::IllegalFunctionCallException, "Cannot retrieve unknown option '" << longName << "'.");
                return *optionIterator->second;
            }

            Option& ModuleSettings::getOption(std::string const& longName)  {
                auto optionIterator = this->options.find(longName);
                LOG_THROW(optionIterator != this->options.end(), storm::exceptions::IllegalFunctionCallException, "Cannot retrieve unknown option '" << longName << "'.");
                return *optionIterator->second;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm