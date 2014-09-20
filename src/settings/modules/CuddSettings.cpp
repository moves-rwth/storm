#include "src/settings/modules/CuddSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string moduleName = "cudd";
            const std::string precisionOptionName = "precision";
            const std::string maximalMemoryOptionName = "maxmem";
            const std::string reorderOptionName = "reorder";
            
            CuddSettings::CuddSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager) {
                // First, we need to create all options of this module.
                std::vector<std::shared_ptr<Option>> options;
                options.push_back(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "Sets the precision used by Cudd.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision up to which to constants are considered to be different.").setDefaultValueDouble(1e-15).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                options.push_back(storm::settings::OptionBuilder(moduleName, maximalMemoryOptionName, true, "Sets the upper bound of memory available to Cudd in MB.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("mb", "The memory available to Cudd (0 means unlimited).").setDefaultValueUnsignedInteger(2048).build()).build());
                
                std::vector<std::string> reorderingTechniques;
                reorderingTechniques.push_back("none");
                reorderingTechniques.push_back("random");
                reorderingTechniques.push_back("randompivot");
                reorderingTechniques.push_back("sift");
                reorderingTechniques.push_back("siftconv");
                reorderingTechniques.push_back("ssift");
                reorderingTechniques.push_back("ssiftconv");
                reorderingTechniques.push_back("gsift");
                reorderingTechniques.push_back("gsiftconv");
                reorderingTechniques.push_back("win2");
                reorderingTechniques.push_back("win2conv");
                reorderingTechniques.push_back("win3");
                reorderingTechniques.push_back("win3conv");
                reorderingTechniques.push_back("win4");
                reorderingTechniques.push_back("win4conv");
                reorderingTechniques.push_back("annealing");
                reorderingTechniques.push_back("genetic");
                reorderingTechniques.push_back("exact");
                options.push_back(storm::settings::OptionBuilder(moduleName, reorderOptionName, true, "Sets the reordering technique used by Cudd.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("method", "Sets which technique is used by Cudd's reordering routines. Must be in {none, random, randompivot, sift, siftconv, ssift, ssiftconv, gsift, gsiftconv, win2, win2conv, win3, win3conv, win4, win4conv, annealing, genetic, exact}.").setDefaultValueString("gsift").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(reorderingTechniques)).build()).build());
                
                // Finally, register all options that we just created.
                settingsManager.registerModule(moduleName, options);
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm