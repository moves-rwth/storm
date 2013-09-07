#include "GmmxxLinearEquationSolver.h"

#include <vector>
#include <string>

bool optionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* instance) -> bool {

	std::vector<std::string> methods;
	methods.push_back("bicgstab");
	methods.push_back("qmr");
	methods.push_back("lscg");
	methods.push_back("gmres");
	methods.push_back("jacobi");

	std::vector<std::string> preconditioner;
	preconditioner.push_back("ilu");
	preconditioner.push_back("diagonal");
	preconditioner.push_back("ildlt");
	preconditioner.push_back("none");

	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "leMethod", "", "The Method used in Linear Equation Solving. Available are: bicgstab, qmr, lscg, gmres, jacobi").addArgument(storm::settings::ArgumentBuilder::createStringArgument("leMethodName", "The Name of the Method to use").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("gmres").build()).build());
	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "preconditioner", "", "The Preconditioning Technique used in Linear Equation Solving. Available are: ilu, diagonal, ildlt, none").addArgument(storm::settings::ArgumentBuilder::createStringArgument("preconditionerName", "The Name of the Preconditioning Method").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(preconditioner)).setDefaultValueString("ilu").build()).build());
	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "maxIterations", "", "Maximum number of Iterations to perform while solving a linear equation system").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("iterationCount", "Max. Iteration Count").setDefaultValueUnsignedInteger(10000).build()).build());
	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "precision", "", "Precision used for iterative solving of linear equation systems").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("precisionValue", "Precision").setDefaultValueDouble(1e-6).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1000000.0)).build()).build());
	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "relative", "", "Whether the relative or the absolute error is considered for deciding convergence via a given precision").addArgument(storm::settings::ArgumentBuilder::createBooleanArgument("useRelative", "relative or absolute comparison").setDefaultValueBoolean(true).build()).build());

	return true;
});