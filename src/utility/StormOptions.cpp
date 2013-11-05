#include "src/utility/StormOptions.h"

bool storm::utility::StormOptions::optionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* settings) -> bool {

	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "help", "h", "Shows all available options, arguments and descriptions.").build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "verbose", "v", "Be verbose.").build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "debug", "", "Be very verbose (intended for debugging).").build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "trace", "", "Be extremly verbose (intended for debugging, heavy performance impacts).").build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "logfile", "l", "If specified, the log output will also be written to this file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("logFileName", "The path and name of the file to write to.").build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "configfile", "c", "If specified, this file will be read and parsed for additional configuration settings.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("configFileName", "The path and name of the file from which to read.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "explicit", "", "Explicit parsing from transition- and labeling files.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("transitionFileName", "The path and name of the file from which to read the transitions.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).addArgument(storm::settings::ArgumentBuilder::createStringArgument("labelingFileName", "The path and name of the file from which to read the labeling.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "symbolic", "", "Parse the given symbolic model file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("symbolicFileName", "The path and name of the file from which to read the symbolic model.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "prctl", "", "Performs model checking for the PRCTL formulas given in the file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("prctlFileName", "The file from which to read the PRCTL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "csl", "", "Performs model checking for the CSL formulas given in the file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("cslFileName", "The file from which to read the CSL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "ltl", "", "Performs model checking for the LTL formulas given in the file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("ltlFileName", "The file from which to read the LTL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "counterExample", "", "Generates a counterexample for the given PRCTL formulas if not satisfied by the model").addArgument(storm::settings::ArgumentBuilder::createStringArgument("outputPath", "The path to the directory to write the generated counterexample files to.").build()).build());

	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "transitionRewards", "", "If specified, the transition rewards are read from this file and added to the explicit model. Note that this requires an explicit model.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("transitionRewardsFileName", "The file from which to read the rransition rewards.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "stateRewards", "", "If specified, the state rewards are read from this file and added to the explicit model. Note that this requires an explicit model.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("stateRewardsFileName", "The file from which to read the state rewards.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
    
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "fixDeadlocks", "", "If the model contains deadlock states, setting this option will insert self-loops for these states.").build());
    
	std::vector<std::string> matrixLibrarys;
	matrixLibrarys.push_back("gmm++");
	matrixLibrarys.push_back("native");
	settings->addOption(storm::settings::OptionBuilder("StoRM Main", "matrixLibrary", "m", "Sets which matrix library is preferred for numerical operations.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("matrixLibraryName", "The name of a matrix library. Valid values are gmm++ and native.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(matrixLibrarys)).setDefaultValueString("gmm++").build()).build());

	return true;
});
