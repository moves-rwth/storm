#include "src/settings/Settings.h"

bool ExplicitModelAdapterOptionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* instance) -> bool {
	instance->addOption(storm::settings::OptionBuilder("ExplicitModelAdapter", "constants", "", "Specifies the constant replacements to use in Explicit Models").addArgument(storm::settings::ArgumentBuilder::createStringArgument("constantString", "A comma separated list of constants and their value, e.g. a=1,b=2,c=3").setDefaultValueString("").build()).build());
	return true;
});
