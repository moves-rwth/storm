#include "src/settings/modules/DFTSettings.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/SettingMemento.h"
#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"

#include "src/exceptions/InvalidSettingsException.h"


namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string DFTSettings::moduleName = "dft";
            const std::string DFTSettings::dftFileOptionName = "dftfile";
            const std::string DFTSettings::dftFileOptionShortName = "dft";
            const std::string DFTSettings::symmetryReductionOptionName = "symmetryreduction";
            const std::string DFTSettings::symmetryReductionOptionShortName = "symred";
            const std::string DFTSettings::modularisationOptionName = "modularisation";
            const std::string DFTSettings::disableDCOptionName = "disabledc";
            const std::string DFTSettings::propExpectedTimeOptionName = "expectedtime";
            const std::string DFTSettings::propExpectedTimeOptionShortName = "mttf";
            const std::string DFTSettings::propProbabilityOptionName = "probability";
            const std::string DFTSettings::propTimeBoundOptionName = "timebound";
            const std::string DFTSettings::minValueOptionName = "min";
            const std::string DFTSettings::maxValueOptionName = "max";
#ifdef STORM_HAVE_Z3
            const std::string DFTSettings::solveWithSmtOptionName = "smt";
#endif
            
            DFTSettings::DFTSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, dftFileOptionName, false, "Parses the model given in the Galileo format.").setShortName(dftFileOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file from which to read the DFT model.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, symmetryReductionOptionName, false, "Exploit symmetric structure of model.").setShortName(symmetryReductionOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, modularisationOptionName, false, "Use modularisation (not applicable for expected time).").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, disableDCOptionName, false, "Disable Dont Care propagation.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, propExpectedTimeOptionName, false, "Compute expected time of system failure.").setShortName(propExpectedTimeOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, propProbabilityOptionName, false, "Compute probability of system failure.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, propTimeBoundOptionName, false, "Compute probability of system failure up to given timebound.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("time", "The timebound to use.").addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleGreaterValidatorExcluding(0.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, minValueOptionName, false, "Compute minimal value in case of non-determinism.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maxValueOptionName, false, "Compute maximal value in case of non-determinism.").build());
#ifdef STORM_HAVE_Z3
                this->addOption(storm::settings::OptionBuilder(moduleName, solveWithSmtOptionName, true, "Solve the DFT with SMT.").build());
#endif
            }
            
            bool DFTSettings::isDftFileSet() const {
                return this->getOption(dftFileOptionName).getHasOptionBeenSet();
            }
            
            std::string DFTSettings::getDftFilename() const {
                return this->getOption(dftFileOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool DFTSettings::useSymmetryReduction() const {
                return this->getOption(symmetryReductionOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::useModularisation() const {
                return this->getOption(modularisationOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::isDisableDC() const {
                return this->getOption(disableDCOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::usePropExpectedTime() const {
                return this->getOption(propExpectedTimeOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::usePropProbability() const {
                return this->getOption(propProbabilityOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::usePropTimebound() const {
                return this->getOption(propTimeBoundOptionName).getHasOptionBeenSet();
            }
            
            double DFTSettings::getPropTimebound() const {
                return this->getOption(propTimeBoundOptionName).getArgumentByName("time").getValueAsDouble();
            }
            
            bool DFTSettings::isComputeMinimalValue() const {
                return this->getOption(minValueOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::isComputeMaximalValue() const {
                return this->getOption(maxValueOptionName).getHasOptionBeenSet();
            }
            
#ifdef STORM_HAVE_Z3
            bool DFTSettings::solveWithSMT() const {
                return this->getOption(solveWithSmtOptionName).getHasOptionBeenSet();
            }
#endif
            
            void DFTSettings::finalize() {
                
            }

            bool DFTSettings::check() const {
                // Ensure that only one property is given.
                if (usePropExpectedTime()) {
                    STORM_LOG_THROW(!usePropProbability() && !usePropTimebound(), storm::exceptions::InvalidSettingsException, "More than one property given.");
                } else if (usePropProbability()) {
                    STORM_LOG_THROW(!usePropTimebound(), storm::exceptions::InvalidSettingsException, "More than one property given.");
                }
                
                // Ensure that at most one of min or max is set
                STORM_LOG_THROW(!isComputeMinimalValue() || !isComputeMaximalValue(), storm::exceptions::InvalidSettingsException, "Min and max can not both be set.");
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm