#include "src/settings/modules/RegionSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string RegionSettings::moduleName = "region";
            const std::string RegionSettings::regionfileOptionName = "regionfile";
            const std::string RegionSettings::regionsOptionName = "regions";
            
            RegionSettings::RegionSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, regionfileOptionName, true, "Specifies the regions via a file. Format: 0.3<=p<=0.4,0.2<=q<=0.5; 0.6<=p<=0.7,0.8<=q<=0.9")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The file from which to read the regions.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, regionsOptionName, true, "Specifies the regions via command line. Format: '0.3<=p<=0.4,0.2<=q<=0.5; 0.6<=p<=0.7,0.8<=q<=0.9'")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("regions", "The considered regions.").build()).build());
            }
            
           
            
            bool RegionSettings::isRegionFileSet() const{
                return this->getOption(regionfileOptionName).getHasOptionBeenSet();
            }
            
            std::string RegionSettings::getRegionFilePath() const{
                return this->getOption(regionfileOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool RegionSettings::isRegionsSet() const{
                return this->getOption(regionsOptionName).getHasOptionBeenSet();
            }
            
            std::string RegionSettings::getRegionsFromCmdLine() const{
                return this->getOption(regionsOptionName).getArgumentByName("regions").getValueAsString();
            }
            
            bool RegionSettings::check() const{
                if(isRegionsSet() && isRegionFileSet()){
                    STORM_LOG_ERROR("Regions specified twice: via command line AND via file.");
                    return false;
                }
                return true;
            }
            
            

        } // namespace modules
    } // namespace settings
} // namespace storm