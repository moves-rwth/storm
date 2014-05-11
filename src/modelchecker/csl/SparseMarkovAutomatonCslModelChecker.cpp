#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"

bool SparseMarkovAutomatonCslModelCheckerOptionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* instance) -> bool {
    
	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "digiprecision", "", "Precision used for iterative solving of linear equation systems").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("precision value", "Precision").setDefaultValueDouble(1e-4).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
    
	return true;
});
