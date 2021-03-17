#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm-conv/api/storm-conv.h"

#include "storm/settings/SettingsManager.h"
#include "storm-conv/settings/ConvSettings.h"
#include "storm-conv/settings/modules/ConversionGeneralSettings.h"
#include "storm-conv/settings/modules/ConversionInputSettings.h"
#include "storm-conv/settings/modules/ConversionOutputSettings.h"
#include "storm-conv/settings/modules/JaniExportSettings.h"
#include "storm-conv/settings/modules/PrismExportSettings.h"

#include "storm/api/storm.h"
#include "storm-parsers/api/storm-parsers.h"
#include "storm/utility/initialize.h"
#include "storm/utility/macros.h"
#include "storm/utility/Stopwatch.h"

#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/SymbolicModelDescription.h"


#include "storm-cli-utilities/cli.h"
#include "storm/exceptions/OptionParserException.h"

TEST(PrismToAigerTest, Simple) {
    // Defaults
    auto const& output = storm::settings::getModule<storm::settings::modules::ConversionOutputSettings>();
    auto const& input = storm::settings::getModule<storm::settings::modules::ConversionInputSettings>();
    auto const& prism = storm::settings::getModule<storm::settings::modules::PrismExportSettings>();

    // Parse the prism program
    storm::storage::SymbolicModelDescription prismProg =
      storm::api::parseProgram(STORM_TEST_RESOURCES_DIR "/conv/simple.prism", input.isPrismCompatibilityEnabled(), false);

    // Parse properties (if available)
    std::vector<storm::jani::Property> properties;
    if (input.isPropertyInputSet()) {
        boost::optional<std::set<std::string>> propertyFilter = storm::api::parsePropertyFilter(input.getPropertyInputFilter());
        properties = storm::api::parsePropertiesForSymbolicModelDescription(input.getPropertyInput(), prismProg, propertyFilter);
    }
    
    // Set constant definitions in program
    std::string constantDefinitionString = input.getConstantDefinitionString();
    auto constantDefinitions = prismProg.parseConstantDefinitions(constantDefinitionString);
    prismProg = storm::storage::SymbolicModelDescription(prismProg.asPrismProgram().defineUndefinedConstants(constantDefinitions));
    
    std::vector<storm::jani::Property> outputProperties = properties;
    
    // prism-to-aiger transformation
    std::shared_ptr<aiger> outputCircuit = storm::api::convertPrismToAiger(prismProg.asPrismProgram(), outputProperties);
}
