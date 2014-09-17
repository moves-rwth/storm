#ifndef STORM_SETTINGS_SETTINGS_H_
#define STORM_SETTINGS_SETTINGS_H_

#include <iostream>
#include <sstream>
#include <list>
#include <utility>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>


#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBase.h"
#include "src/settings/Argument.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/ArgumentType.h"
#include "src/settings/ArgumentTypeInferationHelper.h"

// Exceptions that should be catched when performing a parsing run
#include "src/exceptions/OptionParserException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

/*!
 *	@brief Contains Settings class and associated methods.
 */
namespace settings {
	class Settings;

	typedef std::function<bool (Settings*)> ModuleRegistrationFunction_t;

	typedef bool (*stringValidationFunction_t)(const std::string);
	typedef bool (*integerValidationFunction_t)(const int_fast64_t);
	typedef bool (*unsignedIntegerValidationFunction_t)(const uint_fast64_t);
	typedef bool (*doubleValidationFunction_t)(const double);
	typedef bool (*booleanValidationFunction_t)(const bool);

	typedef std::pair<std::string, std::string> stringPair_t;
	typedef std::pair<bool, std::string> fromStringAssignmentResult_t;

	class Destroyer;
	class InternalOptionMemento;

	/*!
	 *	@brief	Settings class with command line parser and type validation
	 *
	 *
	 *	It is meant to be used as a singleton. Call 
	 *	@code storm::settings::Settings::getInstance() @endcode
	 *	to initialize it and obtain an instance.
	 *
	 *	This class can be customized by other parts of the software using
	 *	option modules. An option module can be anything that implements the
	 *	interface specified by registerModule() and does a static initialization call to this function.
	 */
	class Settings {
		public:

			/*!
			 * This function handles the static initialization registration of modules and their options
			 */
			static bool registerNewModule(ModuleRegistrationFunction_t registrationFunction);
			
			/*!
			 * This parses the command line of the application and matches it to all prior registered options
			 * @throws OptionParserException
			 */
			static void parse(int const argc, char const * const argv[]);

			std::vector<std::shared_ptr<Option>> const& getOptions() const {
				return this->optionPointers;
			}

			// PUBLIC INTERFACE OF OPTIONSACCUMULATOR (now internal)
			/*!
			* Returns true IFF an option with the specified longName exists.
			*/
			bool containsOptionByLongName(std::string const& longName) const {
				return this->containsLongName(longName);
			}

			/*!
			* Returns true IFF an option with the specified shortName exists.
			*/
			bool containsOptionByShortName(std::string const& shortName) const {
				return this->containsLongName(shortName);
			}

			/*!
			* Returns a reference to the Option with the specified longName.
			* Throws an Exception of Type IllegalArgumentException if there is no such Option.
			*/
			Option const& getOptionByLongName(std::string const& longName) const {
				return this->getByLongName(longName);
			}

			/*!
			* Returns a reference to the Option with the specified shortName.
			* Throws an Exception of Type IllegalArgumentException if there is no such Option.
			*/
			Option const& getOptionByShortName(std::string const& shortName) const {
				return this->getByShortName(shortName);
			}
			
			/*!
			 * Adds the given option to the set of known options.
			 * Unifying with existing options is done automatically.
			 * Ownership of the Option is handed over when calling this function!
			 * Returns a reference to the settings instance
			 * @throws OptionUnificationException
			 */
			Settings& addOption(Option* option);

			/*!
			 * Returns true iff there is an Option with the specified longName and it has been set
			 * @return bool true if the option exists and has been set
			 * @throws InvalidArgumentException
			 */
			bool isSet(std::string const& longName) const {
				return this->getByLongName(longName).getHasOptionBeenSet();
			}

			/*!
			 * This generated a list of all registered options and their arguments together with descriptions and defaults.
			 * @return A std::string containing the help text, delimited by \n
			 */
			std::string getHelpText() const;

			/*!
			 * This function is the Singleton interface for the Settings class
			 * @return Settings* A Pointer to the singleton instance of Settings 
			 */
			static Settings* getInstance();
			friend class Destroyer;
			friend class InternalOptionMemento;
		private:
			/*!
			 *	@brief	Private constructor.
			 *
			 *	This constructor is private, as noone should be able to create
			 *	an instance manually, one should always use the
			 *	newInstance() method.
			 */
			Settings() {
				this->addOption(storm::settings::OptionBuilder("StoRM Main", "help", "h", "Shows all available options, arguments and descriptions.").build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "verbose", "v", "Be verbose.").build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "debug", "", "Be very verbose (intended for debugging).").build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "trace", "", "Be extremly verbose (intended for debugging, heavy performance impacts).").build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "logfile", "l", "If specified, the log output will also be written to this file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("logFileName", "The path and name of the file to write to.").build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "exportdot", "", "If specified, the loaded model will be written to the specified file in the dot format.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("dotFileName", "The file to export the model to.").build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "configfile", "c", "If specified, this file will be read and parsed for additional configuration settings.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("configFileName", "The path and name of the file from which to read.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "explicit", "", "Explicit parsing from transition- and labeling files.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("transitionFileName", "The path and name of the file from which to read the transitions.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).addArgument(storm::settings::ArgumentBuilder::createStringArgument("labelingFileName", "The path and name of the file from which to read the labeling.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "symbolic", "", "Parse the given symbolic model file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("symbolicFileName", "The path and name of the file from which to read the symbolic model.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "prctl", "", "Performs model checking for the PRCTL formulas given in the file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("prctlFileName", "The file from which to read the PRCTL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "csl", "", "Performs model checking for the CSL formulas given in the file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("cslFileName", "The file from which to read the CSL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "ltl", "", "Performs model checking for the LTL formulas given in the file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("ltlFileName", "The file from which to read the LTL formulas.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "counterExample", "", "Generates a counterexample for the given PRCTL formulas if not satisfied by the model").addArgument(storm::settings::ArgumentBuilder::createStringArgument("outputPath", "The path to the directory to write the generated counterexample files to.").build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "transitionRewards", "", "If specified, the transition rewards are read from this file and added to the explicit model. Note that this requires an explicit model.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("transitionRewardsFileName", "The file from which to read the transition rewards.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "stateRewards", "", "If specified, the state rewards are read from this file and added to the explicit model. Note that this requires an explicit model.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("stateRewardsFileName", "The file from which to read the state rewards.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "fixDeadlocks", "", "If the model contains deadlock states, setting this option will insert self-loops for these states.").build());
                
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "timeout", "t", "If specified, computation will abort after the given number of seconds.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("seconds", "The number of seconds after which to timeout.").setDefaultValueUnsignedInteger(0).build()).build());
                
                std::vector<std::string> linearEquationSolver;
                linearEquationSolver.push_back("gmm++");
                linearEquationSolver.push_back("native");
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "linsolver", "", "Sets which solver is preferred for solving systems of linear equations.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the solver to prefer. Available are: gmm++ and native.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(linearEquationSolver)).setDefaultValueString("gmm++").build()).build());
                
                std::vector<std::string> nondeterministicLinearEquationSolver;
                nondeterministicLinearEquationSolver.push_back("gmm++");
                nondeterministicLinearEquationSolver.push_back("native");
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "ndsolver", "", "Sets which solver is preferred for solving systems of linear equations arising from nondeterministic systems.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the solver to prefer. Available are: gmm++ and native.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(nondeterministicLinearEquationSolver)).setDefaultValueString("native").build()).build());
                
                std::vector<std::string> lpSolvers;
                lpSolvers.push_back("gurobi");
                lpSolvers.push_back("glpk");
                this->addOption(storm::settings::OptionBuilder("StoRM Main", "lpsolver", "", "Sets which LP solver is preferred.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("LP solver name", "The name of an available LP solver. Valid values are gurobi and glpk.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(lpSolvers)).setDefaultValueString("glpk").build()).build());
                
                this->addOption(storm::settings::OptionBuilder("ExplicitModelAdapter", "constants", "", "Specifies the constant replacements to use in Explicit Models").addArgument(storm::settings::ArgumentBuilder::createStringArgument("constantString", "A comma separated list of constants and their value, e.g. a=1,b=2,c=3").setDefaultValueString("").build()).build());
                
                std::vector<std::string> techniques;
                techniques.push_back("sat");
                techniques.push_back("milp");
                this->addOption(storm::settings::OptionBuilder("Counterexample", "mincmd", "", "Computes a counterexample for the given symbolic model in terms of a minimal command set.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("propertyFile", "The file containing the properties for which counterexamples are to be generated.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).addArgument(storm::settings::ArgumentBuilder::createStringArgument("method", "Sets which technique is used to derive the counterexample. Must be either \"milp\" or \"sat\".").setDefaultValueString("sat").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(techniques)).build()).build());
                this->addOption(storm::settings::OptionBuilder("Counterexample", "stats", "s", "Sets whether to display statistics for certain functionalities.").build());
                this->addOption(storm::settings::OptionBuilder("Counterexample", "encreach", "", "Sets whether to encode reachability for SAT-based minimal command counterexample generation.").build());
                this->addOption(storm::settings::OptionBuilder("Counterexample", "schedcuts", "", "Sets whether to add the scheduler cuts for MILP-based minimal command counterexample generation.").build());
                
                this->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "digiprecision", "", "Precision used for iterative solving of linear equation systems").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("precision value", "Precision").setDefaultValueDouble(1e-4).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("GlpkLpSolver", "glpkoutput", "", "If set, the glpk output will be printed to the command line.").build());
                
                this->addOption(storm::settings::OptionBuilder("GurobiLpSolver", "glpkinttol", "", "Sets glpk's precision for integer variables.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                // Offer all available methods as a command line option.
                std::vector<std::string> methods;
                methods.push_back("bicgstab");
                methods.push_back("qmr");
                methods.push_back("gmres");
                methods.push_back("jacobi");
                this->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "gmmlin", "", "The method to be used for solving linear equation systems with the gmm++ engine. Available are: bicgstab, qmr, gmres, jacobi.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("gmres").build()).build());
                
                // Register available preconditioners.
                std::vector<std::string> preconditioner;
                preconditioner.push_back("ilu");
                preconditioner.push_back("diagonal");
                preconditioner.push_back("ildlt");
                preconditioner.push_back("none");
                this->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "gmmpre", "", "The preconditioning technique used for solving linear equation systems with the gmm++ engine. Available are: ilu, diagonal, none.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the preconditioning method.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(preconditioner)).setDefaultValueString("ilu").build()).build());
                
                this->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "gmmrestart", "", "The number of iteration until restarted methods are actually restarted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of iterations.").setDefaultValueUnsignedInteger(50).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "maxiter", "i", "The maximal number of iterations to perform before iterative solving is aborted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "precision", "", "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "absolute", "", "Whether the relative or the absolute error is considered for deciding convergence.").build());
                
                this->addOption(storm::settings::OptionBuilder("GmmxxNondeterminsticLinearEquationSolver", "maxiter", "i", "The maximal number of iterations to perform before iterative solving is aborted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("GmmxxNondeterminsticLinearEquationSolver", "precision", "", "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-6).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("GmmxxNondeterminsticLinearEquationSolver", "absolute", "", "Whether the relative or the absolute error is considered for deciding convergence.").build());
                
                this->addOption(storm::settings::OptionBuilder("GurobiLpSolver", "gurobithreads", "", "The number of threads that may be used by Gurobi.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of threads.").setDefaultValueUnsignedInteger(1).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("GurobiLpSolver", "gurobioutput", "", "If set, the Gurobi output will be printed to the command line.").build());
                
                this->addOption(storm::settings::OptionBuilder("GurobiLpSolver", "gurobiinttol", "", "Sets Gurobi's precision for integer variables.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                // Offer all available methods as a command line option.
                methods.clear();
                methods.push_back("jacobi");
                this->addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "nativelin", "", "The method to be used for solving linear equation systems with the native engine. Available are: jacobi.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("jacobi").build()).build());
                
                this->addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "maxiter", "i", "The maximal number of iterations to perform before iterative solving is aborted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "precision", "", "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "absolute", "", "Whether the relative or the absolute error is considered for deciding convergence.").build());
                
                this->addOption(storm::settings::OptionBuilder("NativeNondeterminsticLinearEquationSolver", "maxiter", "i", "The maximal number of iterations to perform before iterative solving is aborted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("NativeNondeterminsticLinearEquationSolver", "precision", "", "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("NativeNondeterminsticLinearEquationSolver", "absolute", "", "Whether the relative or the absolute error is considered for deciding convergence.").build());
                
                // Set up options for precision and maximal memory available to Cudd.
                this->addOption(storm::settings::OptionBuilder("Cudd", "cuddprec", "", "Sets the precision used by Cudd.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision up to which to constants are considered to be different.").setDefaultValueDouble(1e-15).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder("Cudd", "cuddmaxmem", "", "Sets the upper bound of memory available to Cudd in MB.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("mb", "The memory available to Cudd (0 means unlimited).").setDefaultValueUnsignedInteger(2048).build()).build());
                
                // Set up option for reordering.
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
                this->addOption(storm::settings::OptionBuilder("Cudd", "reorder", "", "Sets the reordering technique used by Cudd.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("method", "Sets which technique is used by Cudd's reordering routines. Must be in {\"none\", \"random\", \"randompivot\", \"sift\", \"siftconv\", \"ssift\", \"ssiftconv\", \"gsift\", \"gsiftconv\", \"win2\", \"win2conv\", \"win3\", \"win3conv\", \"win4\", \"win4conv\", \"annealing\", \"genetic\", \"exact\"}.").setDefaultValueString("gsift").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(reorderingTechniques)).build()).build());
			}
			
			/*!
			 *	@brief	Private destructor.
			 *
			 *	This destructor should be private, as noone should be able to destroy a singleton.
			 *	The object is automatically destroyed when the program terminates by the destroyer.
			 */
			virtual ~Settings() {
				//
			}

			/*!
			 * Parser for the commdand line parameters of the program.
			 * The first entry of argv will be ignored, as it represents the program name.
			 * @throws OptionParserException
			 */
			void parseCommandLine(int const argc, char const * const argv[]);

			/*!
			* The map holding the information regarding registered options and their types
			*/
			std::unordered_map<std::string, std::shared_ptr<Option>> options;

			/*!
			* The vector holding a pointer to all options
			*/
			std::vector<std::shared_ptr<Option>> optionPointers;

			/*!
			* The map holding the information regarding registered options and their short names
			*/
			std::unordered_map<std::string, std::string> shortNames;
 			
 			/*!
 			 *	@brief	Destroyer object.
 			 */
 			static Destroyer destroyer;

			// Helper functions
			stringPair_t splitOptionString(std::string const& option);
			bool hasAssignment(std::string const& option);
			void handleAssignment(std::string const& longOptionName, std::vector<std::string> arguments);
			std::vector<std::string> argvToStringArray(int const argc, char const * const argv[]);
			std::vector<bool> scanForOptions(std::vector<std::string> const& arguments);
			bool checkArgumentSyntaxForOption(std::string const& argvString);

			/*!
			* Returns true IFF this contains an option with the specified longName.
			* @return bool true iff there is an option with the specified longName
			*/
			bool containsLongName(std::string const& longName) const {
				return (this->options.find(storm::utility::StringHelper::stringToLower(longName)) != this->options.end());
			}

			/*!
			* Returns true IFF this contains an option with the specified shortName.
			* @return bool true iff there is an option with the specified shortName
			*/
			bool containsShortName(std::string const& shortName) const {
				return (this->shortNames.find(storm::utility::StringHelper::stringToLower(shortName)) != this->shortNames.end());
			}

			/*!
			* Returns a reference to the Option with the specified longName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			* @throws InvalidArgumentException
			*/
			Option& getByLongName(std::string const& longName) const {
				auto longNameIterator = this->options.find(storm::utility::StringHelper::stringToLower(longName));
				if (longNameIterator == this->options.end()) {
					LOG4CPLUS_ERROR(logger, "Settings::getByLongName: This program does not contain an option named \"" << longName << "\".");
					throw storm::exceptions::IllegalArgumentException() << "This program does not contain an option named \"" << longName << "\".";
				}
				return *longNameIterator->second.get();
			}

			/*!
			* Returns a pointer to the Option with the specified longName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			* @throws InvalidArgumentException
			*/
			Option* getPtrByLongName(std::string const& longName) const {
				auto longNameIterator = this->options.find(storm::utility::StringHelper::stringToLower(longName));
				if (longNameIterator == this->options.end()) {
					LOG4CPLUS_ERROR(logger, "Settings::getPtrByLongName: This program does not contain an option named \"" << longName << "\".");
					throw storm::exceptions::IllegalArgumentException() << "This program does not contain an option named \"" << longName << "\".";
				}
				return longNameIterator->second.get();
			}

			/*!
			* Returns a reference to the Option with the specified shortName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			* @throws InvalidArgumentException
			*/
			Option& getByShortName(std::string const& shortName) const {
				auto shortNameIterator = this->shortNames.find(storm::utility::StringHelper::stringToLower(shortName));
				if (shortNameIterator == this->shortNames.end()) {
					LOG4CPLUS_ERROR(logger, "Settings::getByShortName: This program does not contain an option named \"" << shortName << "\".");
					throw storm::exceptions::IllegalArgumentException() << "This program does not contain an option named \"" << shortName << "\"";
				}
				return *(this->options.find(shortNameIterator->second)->second.get());
			}

			/*!
			* Returns a pointer to the Option with the specified shortName.
			* Throws an Exception of Type InvalidArgumentException if there is no such Option.
			* @throws InvalidArgumentException
			*/
			Option* getPtrByShortName(std::string const& shortName) const {
				auto shortNameIterator = this->shortNames.find(storm::utility::StringHelper::stringToLower(shortName));
				if (shortNameIterator == this->shortNames.end()) {
					LOG4CPLUS_ERROR(logger, "Settings::getPtrByShortName: This program does not contain an option named \"" << shortName << "\".");
					throw storm::exceptions::IllegalArgumentException() << "This program does not contain an option named \"" << shortName << "\".";
				}
				return this->options.find(shortNameIterator->second)->second.get();
			}

			/*!
			 * Sets the Option with the specified longName
			 * This function requires the Option to have no arguments
			 * This is for TESTING only and should not be used outside of the testing code!
			 * @throws InvalidArgumentException
			 */
			void set(std::string const& longName) const {
				return this->getByLongName(longName).setHasOptionBeenSet();
			}

			/*!
			 * Unsets the Option with the specified longName
			 * This function requires the Option to have no arguments
			 * This is for TESTING only and should not be used outside of the testing code!
			 * @throws InvalidArgumentException
			 */
			void unset(std::string const& longName) const {
				return this->getByLongName(longName).setHasOptionBeenSet(false);
			}
	};

	/*!
	 *	@brief	Destroyer class for singleton object of Settings.
	 *
	 *	The sole purpose of this class is to clean up the singleton object
	 *	instance of Settings. The Settings class has a static member of this
	 *	Destroyer type that gets cleaned up when the program terminates. In
	 *	it's destructor, this object will remove the Settings instance.
	 */
	class Destroyer {
		public:
			Destroyer(): settingsInstance(nullptr) {
				this->settingsInstance = storm::settings::Settings::getInstance();
			}

			/*!
			 *	@brief	Destructor.
			 *
			 *	Free Settings::inst.
			 */
			virtual ~Destroyer() {
				if (this->settingsInstance != nullptr) {
					//LOG4CPLUS_DEBUG(logger, "Destroyer::~Destroyer: Destroying Settings Instance...");
					// The C++11 Method of Singleton deletes its instance on its own
					//delete this->settingsInstance;
					this->settingsInstance = nullptr;
				}
			}
		private:
			storm::settings::Settings* settingsInstance;
	};
	




} // namespace settings
} // namespace storm

#endif // 