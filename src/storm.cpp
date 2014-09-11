
/*
 *	STORM - a C++ Rebuild of MRMC
 *	
 *	STORM (Stochastic Reward Model Checker) is a model checker for discrete-time and continuous-time Markov
 *	reward models. It supports reward extensions of PCTL and CSL (PRCTL
 *	and CSRL), and allows for the automated verification of properties
 *	concerning long-run and instantaneous rewards as well as cumulative
 *	rewards.
 *	
 *  Authors: Philipp Berger
 *
 *  Description: Central part of the application containing the main() Method
 */

#include "src/utility/Initialize.h"
#include <fstream>
#include <cstdio>
#include <climits>
#include <sstream>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>




#include "storm-config.h"
#include "storm-version.h"
#include "src/models/Dtmc.h"
#include "src/models/MarkovAutomaton.h"
#include "src/storage/SparseMatrix.h"
#include "src/storage/MaximalEndComponentDecomposition.h"
#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/modelchecker/prctl/CreatePrctlModelChecker.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/solver/GurobiLpSolver.h"
#include "src/counterexamples/GenerateCounterexample.h"
#include "src/counterexamples/MILPMinimalLabelSetGenerator.h"
#include "src/counterexamples/SMTMinimalCommandSetGenerator.h"
#include "src/counterexamples/PathBasedSubsystemGenerator.h"
#include "src/parser/AutoParser.h"
#include "src/parser/MarkovAutomatonParser.h"
#include "src/parser/PrctlParser.h"
#include "src/utility/ErrorHandling.h"
#include "src/properties/Prctl.h"
#include "src/utility/vector.h"
#include "src/utility/CLI.h"


#include "src/parser/PrctlFileParser.h"
#include "src/parser/LtlFileParser.h"

#include "src/parser/PrismParser.h"
#include "src/adapters/ExplicitModelAdapter.h"
// #include "src/adapters/SymbolicModelAdapter.h"

#include "src/exceptions/InvalidSettingsException.h"




/*!
 * Checks the PRCTL formulae provided on the command line on the given model checker.
 *
 * @param modelchecker The model checker that is to be invoked on all given formulae.
 */
void checkPrctlFormulae(storm::modelchecker::prctl::AbstractModelChecker<double> const& modelchecker) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	if (s->isSet("prctl")) {
		std::string const chosenPrctlFile = s->getOptionByLongName("prctl").getArgument(0).getValueAsString();
		LOG4CPLUS_INFO(logger, "Parsing prctl file: " << chosenPrctlFile << ".");
		std::list<std::shared_ptr<storm::properties::prctl::PrctlFilter<double>>> formulaList = storm::parser::PrctlFileParser::parsePrctlFile(chosenPrctlFile);
        
        for (auto formula : formulaList) {
			std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
        	formula->check(modelchecker);
			std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
			std::cout << "Checking the formula took " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "ms." << std::endl;
        }
	}
}

/*!
 * Main entry point.
 */
int main(const int argc, const char* argv[]) {
    // Register a signal handler to catch signals and display a backtrace.
	installSignalHandler();
    
    // Print an information header.
	printHeader(argc, argv);

    // Initialize the logging engine and perform other initalizations.
	initializeLogger();
	setUp();

	try {
		LOG4CPLUS_INFO(logger, "StoRM was invoked.");

		// Parse options.
		if (!parseOptions(argc, argv)) {
			// If parsing failed or the option to see the usage was set, program execution stops here.
			return 0;
		}
        
        // If requested by the user, we install a timeout signal to abort computation.
		storm::settings::Settings* s = storm::settings::Settings::getInstance();
        uint_fast64_t timeout = s->getOptionByLongName("timeout").getArgument(0).getValueAsUnsignedInteger();
        if (timeout != 0) {
			stormSetAlarm(timeout);
        }
        
		// Execution Time measurement, start
		std::chrono::high_resolution_clock::time_point executionStart = std::chrono::high_resolution_clock::now();

		// Now, the settings are received and the specified model is parsed. The actual actions taken depend on whether
        // the model was provided in explicit or symbolic format.
		if (s->isSet("explicit")) {
			std::string const chosenTransitionSystemFile = s->getOptionByLongName("explicit").getArgument(0).getValueAsString();
			std::string const chosenLabelingFile = s->getOptionByLongName("explicit").getArgument(1).getValueAsString();
            
			std::string chosenStateRewardsFile = "";
			if (s->isSet("stateRewards")) {
				chosenStateRewardsFile = s->getOptionByLongName("stateRewards").getArgument(0).getValueAsString();
			}
			std::string chosenTransitionRewardsFile = "";
			if (s->isSet("transitionRewards")) {
				chosenTransitionRewardsFile = s->getOptionByLongName("transitionRewards").getArgument(0).getValueAsString();
			}

			std::shared_ptr<storm::models::AbstractModel<double>> model = storm::parser::AutoParser::parseModel(chosenTransitionSystemFile, chosenLabelingFile, chosenStateRewardsFile, chosenTransitionRewardsFile);

			// Model Parsing Time Measurement, End
			std::chrono::high_resolution_clock::time_point parsingEnd = std::chrono::high_resolution_clock::now();
			std::cout << "Parsing the given model took " << std::chrono::duration_cast<std::chrono::milliseconds>(parsingEnd - executionStart).count() << " milliseconds." << std::endl;

            if (s->isSet("exportdot")) {
                std::ofstream outputFileStream;
                outputFileStream.open(s->getOptionByLongName("exportdot").getArgument(0).getValueAsString(), std::ofstream::out);
                model->writeDotToStream(outputFileStream);
                outputFileStream.close();
            }
            
			// Should there be a counterexample generated in case the formula is not satisfied?
			if(s->isSet("counterexample")) {
				// Counterexample Time Measurement, Start
				std::chrono::high_resolution_clock::time_point counterexampleStart = std::chrono::high_resolution_clock::now();

				generateCounterExample(model);
			
				// Counterexample Time Measurement, End
				std::chrono::high_resolution_clock::time_point counterexampleEnd = std::chrono::high_resolution_clock::now();
				std::cout << "Generating the counterexample took " << std::chrono::duration_cast<std::chrono::milliseconds>(counterexampleEnd - counterexampleStart).count() << " milliseconds." << std::endl;
			} else {
				// Depending on the model type, the appropriate model checking procedure is chosen.
				storm::modelchecker::prctl::AbstractModelChecker<double>* modelchecker = nullptr;
				model->printModelInformationToStream(std::cout);
                
				// Modelchecking Time Measurement, Start
				std::chrono::high_resolution_clock::time_point modelcheckingStart = std::chrono::high_resolution_clock::now();

				switch (model->getType()) {
				case storm::models::DTMC:
					LOG4CPLUS_INFO(logger, "Model is a DTMC.");
					modelchecker = createPrctlModelChecker(*model->as<storm::models::Dtmc<double>>());
					checkPrctlFormulae(*modelchecker);
					break;
				case storm::models::MDP:
					LOG4CPLUS_INFO(logger, "Model is an MDP.");
					modelchecker = createPrctlModelChecker(*model->as<storm::models::Mdp<double>>());
					checkPrctlFormulae(*modelchecker);
					break;
				case storm::models::CTMC:
					LOG4CPLUS_INFO(logger, "Model is a CTMC.");
					LOG4CPLUS_ERROR(logger, "The selected model type is not supported.");
					break;
				case storm::models::CTMDP:
					LOG4CPLUS_INFO(logger, "Model is a CTMDP.");
					LOG4CPLUS_ERROR(logger, "The selected model type is not supported.");
					break;
                case storm::models::MA: {
                    LOG4CPLUS_INFO(logger, "Model is a Markov automaton.");
                    storm::models::MarkovAutomaton<double> markovAutomaton = *model->as<storm::models::MarkovAutomaton<double>>();
                    markovAutomaton.close();
                    storm::modelchecker::csl::SparseMarkovAutomatonCslModelChecker<double> mc(markovAutomaton);
//                    std::cout << mc.checkExpectedTime(true, markovAutomaton->getLabeledStates("goal")) << std::endl;
//                    std::cout << mc.checkExpectedTime(false, markovAutomaton->getLabeledStates("goal")) << std::endl;
                    std::cout << mc.checkLongRunAverage(true, markovAutomaton.getLabeledStates("goal")) << std::endl;
                    std::cout << mc.checkLongRunAverage(false, markovAutomaton.getLabeledStates("goal")) << std::endl;
//                    std::cout << mc.checkTimeBoundedEventually(true, markovAutomaton->getLabeledStates("goal"), 0, 1) << std::endl;
//                    std::cout << mc.checkTimeBoundedEventually(true, markovAutomaton->getLabeledStates("goal"), 1, 2) << std::endl;
                    break;
                }
				case storm::models::Unknown:
				default:
					LOG4CPLUS_ERROR(logger, "The model type could not be determined correctly.");
					break;
				}

				if (modelchecker != nullptr) {
					delete modelchecker;
				}

				// Modelchecking Time Measurement, End
				std::chrono::high_resolution_clock::time_point modelcheckingEnd = std::chrono::high_resolution_clock::now();
				std::cout << "Running the ModelChecker took " << std::chrono::duration_cast<std::chrono::milliseconds>(modelcheckingEnd - modelcheckingStart).count() << " milliseconds." << std::endl;
			}
		} else if (s->isSet("symbolic")) {
			// Program Translation Time Measurement, Start
			std::chrono::high_resolution_clock::time_point programTranslationStart = std::chrono::high_resolution_clock::now();

            // First, we build the model using the given symbolic model description and constant definitions.
            std::string const& programFile = s->getOptionByLongName("symbolic").getArgument(0).getValueAsString();
            std::string const& constants = s->getOptionByLongName("constants").getArgument(0).getValueAsString();
            storm::prism::Program program = storm::parser::PrismParser::parse(programFile);
            std::shared_ptr<storm::models::AbstractModel<double>> model = storm::adapters::ExplicitModelAdapter<double>::translateProgram(program, constants);
            model->printModelInformationToStream(std::cout);
            
			// Program Translation Time Measurement, End
			std::chrono::high_resolution_clock::time_point programTranslationEnd = std::chrono::high_resolution_clock::now();
			std::cout << "Parsing and translating the Symbolic Input took " << std::chrono::duration_cast<std::chrono::milliseconds>(programTranslationEnd - programTranslationStart).count() << " milliseconds." << std::endl;

            if (s->isSet("mincmd")) {
                if (model->getType() != storm::models::MDP) {
                    LOG4CPLUS_ERROR(logger, "Minimal command counterexample generation is only supported for models of type MDP.");
                    throw storm::exceptions::InternalTypeErrorException() << "Minimal command counterexample generation is only supported for models of type MDP.";
                }
                
                std::shared_ptr<storm::models::Mdp<double>> mdp = model->as<storm::models::Mdp<double>>();
                
                // Determine whether we are required to use the MILP-version or the SAT-version.
                bool useMILP = s->getOptionByLongName("mincmd").getArgumentByName("method").getValueAsString() == "milp";
                
				// MinCMD Time Measurement, Start
				std::chrono::high_resolution_clock::time_point minCmdStart = std::chrono::high_resolution_clock::now();

                // Now parse the property file and receive the list of parsed formulas.
                std::string const& propertyFile = s->getOptionByLongName("mincmd").getArgumentByName("propertyFile").getValueAsString();
                std::list<std::shared_ptr<storm::properties::prctl::PrctlFilter<double>>> formulaList = storm::parser::PrctlFileParser::parsePrctlFile(propertyFile);

                // Now generate the counterexamples for each formula.
                for (auto formulaPtr : formulaList) {
                    if (useMILP) {
                        storm::counterexamples::MILPMinimalLabelSetGenerator<double>::computeCounterexample(program, *mdp, formulaPtr->getChild());
                    } else {
                        // storm::counterexamples::SMTMinimalCommandSetGenerator<double>::computeCounterexample(program, constants, *mdp, formulaPtr->getChild());
                    }
                }

				// MinCMD Time Measurement, End
				std::chrono::high_resolution_clock::time_point minCmdEnd = std::chrono::high_resolution_clock::now();
				std::cout << "Minimal command Counterexample generation took " << std::chrono::duration_cast<std::chrono::milliseconds>(minCmdEnd - minCmdStart).count() << " milliseconds." << std::endl;
            } else if (s->isSet("prctl")) {
				// Depending on the model type, the appropriate model checking procedure is chosen.
				storm::modelchecker::prctl::AbstractModelChecker<double>* modelchecker = nullptr;
                
				// Modelchecking Time Measurement, Start
				std::chrono::high_resolution_clock::time_point modelcheckingStart = std::chrono::high_resolution_clock::now();

				switch (model->getType()) {
                    case storm::models::DTMC:
                        LOG4CPLUS_INFO(logger, "Model is a DTMC.");
                        modelchecker = createPrctlModelChecker(*model->as<storm::models::Dtmc<double>>());
                        checkPrctlFormulae(*modelchecker);
                        break;
                    case storm::models::MDP:
                        LOG4CPLUS_INFO(logger, "Model is an MDP.");
                        modelchecker = createPrctlModelChecker(*model->as<storm::models::Mdp<double>>());
                        checkPrctlFormulae(*modelchecker);
                        break;
                    case storm::models::CTMC:
                        LOG4CPLUS_INFO(logger, "Model is a CTMC.");
                        LOG4CPLUS_ERROR(logger, "The selected model type is not supported.");
                        break;
                    case storm::models::CTMDP:
                        LOG4CPLUS_INFO(logger, "Model is a CTMDP.");
                        LOG4CPLUS_ERROR(logger, "The selected model type is not supported.");
                        break;
                    case storm::models::MA:
                        LOG4CPLUS_INFO(logger, "Model is a Markov automaton.");
                        break;
                    case storm::models::Unknown:
                    default:
                        LOG4CPLUS_ERROR(logger, "The model type could not be determined correctly.");
                        break;
				}
                
				if (modelchecker != nullptr) {
					delete modelchecker;
				}

				// Modelchecking Time Measurement, End
				std::chrono::high_resolution_clock::time_point modelcheckingEnd = std::chrono::high_resolution_clock::now();
				std::cout << "Running the PRCTL ModelChecker took " << std::chrono::duration_cast<std::chrono::milliseconds>(modelcheckingEnd - modelcheckingStart).count() << " milliseconds." << std::endl;
            }
        }

		// Execution Time Measurement, End
		std::chrono::high_resolution_clock::time_point executionEnd = std::chrono::high_resolution_clock::now();
		std::cout << "Complete execution took " << std::chrono::duration_cast<std::chrono::milliseconds>(executionEnd - executionStart).count() << " milliseconds." << std::endl;
        
        // Perform clean-up and terminate.
		cleanUp();
        printUsage();
		LOG4CPLUS_INFO(logger, "StoRM terminating.");
		return 0;
	} catch (std::exception& e) {
		LOG4CPLUS_FATAL(logger, "An exception was thrown. Terminating.");
		LOG4CPLUS_FATAL(logger, "\t" << e.what());
	}
	return 1;
}
