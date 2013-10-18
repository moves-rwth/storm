#include "src/settings/Settings.h"

bool CounterexampleOptionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* instance) -> bool {
    std::vector<std::string> techniques;
    techniques.push_back("sat");
    techniques.push_back("milp");
	instance->addOption(storm::settings::OptionBuilder("Counterexample", "mincmd", "", "Computes a counterexample for the given symbolic model in terms of a minimal command set.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("propertyFile", "The file containing the properties for which counterexamples are to be generated.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).addArgument(storm::settings::ArgumentBuilder::createStringArgument("method", "Sets which technique is used to derive the counterexample. Must be either \"milp\" or \"sat\".").setDefaultValueString("sat").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(techniques)).build()).build());
	return true;
});
