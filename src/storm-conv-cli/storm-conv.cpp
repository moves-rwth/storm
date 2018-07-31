
#include "storm-conv/api/storm-conv.h"

#include "storm/settings/SettingsManager.h"
#include "storm-conv/settings/ConvSettings.h"
#include "storm-conv/settings/modules/ConversionGeneralSettings.h"
#include "storm-conv/settings/modules/ConversionInputSettings.h"
#include "storm-conv/settings/modules/ConversionOutputSettings.h"

#include "storm/api/storm.h"
#include "storm-parsers/api/storm-parsers.h"
#include "storm/utility/initialize.h"
#include "storm/utility/macros.h"

#include "storm/storage/SymbolicModelDescription.h"


#include "storm-cli-utilities/cli.h"
#include "storm/exceptions/OptionParserException.h"

namespace storm {
    namespace conv {
        
        void setUrgentOptions() {
            
            // Set the correct log level
            if (storm::settings::getModule<storm::settings::modules::ConversionOutputSettings>().isStdOutOutputEnabled()) {
                storm::utility::setLogLevel(l3pp::LogLevel::OFF);
            } else {
                auto const& general = storm::settings::getModule<storm::settings::modules::ConversionGeneralSettings>();
                if (general.isVerboseSet()) {
                    storm::utility::setLogLevel(l3pp::LogLevel::INFO);
                }
                if (general.isDebugOutputSet()) {
                    storm::utility::setLogLevel(l3pp::LogLevel::DEBUG);
                }
                if (general.isTraceOutputSet()) {
                    storm::utility::setLogLevel(l3pp::LogLevel::TRACE);
                }
            }
        }
        
        void processPrismInputJaniOutput(storm::prism::Program const& prismProg, std::vector<storm::jani::Property> const& properties) {
            auto const& output = storm::settings::getModule<storm::settings::modules::ConversionOutputSettings>();
            auto const& input = storm::settings::getModule<storm::settings::modules::ConversionInputSettings>();
            auto const& jani = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();
            
            storm::converter::PrismToJaniConverterOptions options;
            options.allVariablesGlobal = jani.isGlobalVarsSet();
            options.suffix = "";
            options.janiOptions = storm::converter::JaniConversionOptions(jani);
            auto janiModelProperties = storm::api::convertPrismToJani(prismProg, properties, options);
            
            std::string outputFilename = "";
            if (output.isJaniOutputFilenameSet()) {
                outputFilename = output.getJaniOutputFilename();
            } else if (input.isPrismInputSet() && !output.isStdOutOutputEnabled()) {
                std::string suffix = "";
                if (input.isConstantsSet()) {
                    suffix = input.getConstantDefinitionString();
                    std::replace(suffix.begin(), suffix.end(), ',', '_');
                }
                suffix = suffix + ".jani";
                outputFilename = input.getPrismInputFilename() + suffix;
            }
            
            if (outputFilename != "") {
                storm::api::exportJaniToFile(janiModelProperties.first, janiModelProperties.second, outputFilename);
            }
            
            if (output.isStdOutOutputEnabled()) {
                storm::api::printJaniToStream(janiModelProperties.first, janiModelProperties.second, std::cout);
            }
        }
        
        void processPrismInput() {
            auto const& input = storm::settings::getModule<storm::settings::modules::ConversionInputSettings>();

            // Parse the prism program
            storm::storage::SymbolicModelDescription prismProg = storm::api::parseProgram(input.getPrismInputFilename(), input.isPrismCompatibilityEnabled());

            // Parse properties (if available)
            std::vector<storm::jani::Property> properties;
            if (input.isPropertyInputSet()) {
                boost::optional<std::set<std::string>> propertyFilter = storm::api::parsePropertyFilter(input.getPropertyInputFilter());
                properties = storm::api::parsePropertiesForSymbolicModelDescription(input.getPropertyInput(), prismProg, propertyFilter);
            }
            
            // Substitute constant definitions in program and properties.
            std::string constantDefinitionString = input.getConstantDefinitionString();
            auto constantDefinitions = prismProg.parseConstantDefinitions(constantDefinitionString);
            prismProg = prismProg.preprocess(constantDefinitions);
            if (!properties.empty()) {
                properties = storm::api::substituteConstantsInProperties(properties, constantDefinitions);
            }
            
            // Branch on the type of output
            auto const& output = storm::settings::getModule<storm::settings::modules::ConversionOutputSettings>();
            if (output.isJaniOutputSet()) {
                processPrismInputJaniOutput(prismProg.asPrismProgram(), properties);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "There is either no outputformat specified or the provided combination of input and output format is not compatible.");
            }
        }
        
        void processOptions() {
            // Start by setting some urgent options (log levels, etc.)
            setUrgentOptions();
            
            // Branch on the type of input
            auto const& input = storm::settings::getModule<storm::settings::modules::ConversionInputSettings>();
            if (input.isPrismInputSet()) {
                processPrismInput();
            }
        }
    }
}

bool parseOptions(const int argc, const char* argv[]) {
    try {
        storm::settings::mutableManager().setFromCommandLine(argc, argv);
    } catch (storm::exceptions::OptionParserException& e) {
        storm::settings::manager().printHelp();
        throw e;
        return false;
    }
    
    auto const& general = storm::settings::getModule<storm::settings::modules::ConversionGeneralSettings>();
    
    // Set options from config file (if given)
    if (general.isConfigSet()) {
        storm::settings::mutableManager().setFromConfigurationFile(general.getConfigFilename());
    }

    bool result = true;
    if (general.isHelpSet()) {
        storm::settings::manager().printHelp(general.getHelpModuleName());
        result = false;
    }
    
    if (general.isVersionSet()) {
        storm::cli::printVersion("storm-conv");
        result = false;;
    }

    return result;
}

/*!
 * Main entry point of the executable storm-conv.
 */
int main(const int argc, const char** argv) {

    try {
        storm::utility::setUp();
        
        // Print header info only if output to sdtout is disabled
        bool outputToStdOut = false;
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--" + storm::settings::modules::ConversionOutputSettings::stdoutOptionName) {
                outputToStdOut = true;
            }
        }
        if (outputToStdOut) {
            storm::utility::setLogLevel(l3pp::LogLevel::OFF);
        } else {
            storm::cli::printHeader("Storm-conv", argc, argv);
        }
        
        storm::settings::initializeConvSettings("Storm-conv", "storm-conv");
        if (!parseOptions(argc, argv)) {
            return -1;
        }

        storm::conv::processOptions();

        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm-conv to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm-conv to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
