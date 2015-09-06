#include "src/settings/modules/RegionSettings.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"
#include "exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string RegionSettings::moduleName = "region";
            const std::string RegionSettings::regionfileOptionName = "regionfile";
            const std::string RegionSettings::regionsOptionName = "regions";
            const std::string RegionSettings::approxmodeOptionName = "approxmode";
            const std::string RegionSettings::samplemodeOptionName = "samplemode";
            const std::string RegionSettings::smtmodeOptionName = "smtmode";
            
            RegionSettings::RegionSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName), modesModified(false) {
                this->addOption(storm::settings::OptionBuilder(moduleName, regionfileOptionName, true, "Specifies the regions via a file. Format: 0.3<=p<=0.4,0.2<=q<=0.5; 0.6<=p<=0.7,0.8<=q<=0.9")
                            .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The file from which to read the regions.")
                                .addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, regionsOptionName, true, "Specifies the regions via command line. Format: '0.3<=p<=0.4,0.2<=q<=0.5; 0.6<=p<=0.7,0.8<=q<=0.9'")
                            .addArgument(storm::settings::ArgumentBuilder::createStringArgument("regions", "The considered regions.").build()).build());
                std::vector<std::string> approxModes = {"off", "testfirst", "guessallsat", "guessallviolated"};
                this->addOption(storm::settings::OptionBuilder(moduleName, approxmodeOptionName, true, "Sets whether approximation should be done and whether lower or upper bounds are computed first.")
                            .addArgument(storm::settings::ArgumentBuilder::createStringArgument("mode", "The mode, (off, testfirst (default), guessallsat, guessallviolated). E.g. guessallsat will first try to prove ALLSAT")
                                .addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(approxModes)).setDefaultValueString("testfirst").build()).build());
                std::vector<std::string> sampleModes = {"off", "instantiate", "evaluate"};
                this->addOption(storm::settings::OptionBuilder(moduleName, samplemodeOptionName, true, "Sets whether sampling should be done and whether to instantiate a model or compute+evaluate a function.")
                            .addArgument(storm::settings::ArgumentBuilder::createStringArgument("mode", "The mode, (off, instantiate (default), evaluate)")
                                .addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(sampleModes)).setDefaultValueString("instantiate").build()).build());
                std::vector<std::string> smtModes = {"off", "function", "model"};
                this->addOption(storm::settings::OptionBuilder(moduleName, smtmodeOptionName, true, "Sets whether SMT solving should be done and whether to encode it via a function or the model.")
                            .addArgument(storm::settings::ArgumentBuilder::createStringArgument("mode", "The mode, (off, function (default), model)")
                                .addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(smtModes)).setDefaultValueString("off").build()).build());
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
            
            RegionSettings::ApproxMode RegionSettings::getApproxMode() const {
                if(this->modesModified) {
                    return this->approxMode;
                }
                std::string modeString= this->getOption(approxmodeOptionName).getArgumentByName("mode").getValueAsString();
                if(modeString=="off"){
                    return ApproxMode::OFF;
                }
                if(modeString=="guessallsat"){
                    return ApproxMode::GUESSALLSAT;
                }
                if(modeString=="guessallviolated"){
                    return ApproxMode::GUESSALLVIOLATED;
                }
                if(modeString=="testfirst"){
                    return ApproxMode::TESTFIRST;
                }
                //if we reach this point, something went wrong
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The approx mode '" << modeString << "' is not valid");
                return ApproxMode::OFF;
            }

            bool RegionSettings::doApprox() const {
                return getApproxMode()!=ApproxMode::OFF;
            }
            
            RegionSettings::SampleMode RegionSettings::getSampleMode() const {
                if (this->modesModified){
                    return this->sampleMode;
                }
                std::string modeString= this->getOption(samplemodeOptionName).getArgumentByName("mode").getValueAsString();
                if(modeString=="off"){
                    return SampleMode::OFF;
                }
                if(modeString=="instantiate"){
                    return SampleMode::INSTANTIATE;
                }
                if(modeString=="evaluate"){
                    return SampleMode::EVALUATE;
                }
                //if we reach this point, something went wrong
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The sample mode '" << modeString << "' is not valid");
                return SampleMode::OFF;
            }

            bool RegionSettings::doSample() const {
                return getSampleMode()!=SampleMode::OFF;
            }
            
            RegionSettings::SmtMode RegionSettings::getSmtMode() const {
                if(this->modesModified){
                    return this->smtMode;
                }
                std::string modeString= this->getOption(smtmodeOptionName).getArgumentByName("mode").getValueAsString();
                if(modeString=="off"){
                    return SmtMode::OFF;
                }
                if(modeString=="function"){
                    return SmtMode::FUNCTION;
                }
                if(modeString=="model"){
                    return SmtMode::MODEL;
                }
                //if we reach this point, something went wrong
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The smt mode '" << modeString << "' is not valid");
                return SmtMode::OFF;
            }
            
            bool RegionSettings::doSmt() const {
                return getSmtMode()!=SmtMode::OFF;
            }
            
            void RegionSettings::modifyModes(ApproxMode const& approxMode, SampleMode const& sampleMode, SmtMode const& smtMode) {
                this->approxMode = approxMode;
                this->sampleMode = sampleMode;
                this->smtMode = smtMode;
                this->modesModified=true;
            }
            
            void RegionSettings::resetModes() {
                this->modesModified=false;
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