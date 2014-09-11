
#ifndef STORM_COUNTEREXAMPLES_GENERATECOUNTEREXAMPLE_H_
#define STORM_COUNTEREXAMPLES_GENERATECOUNTEREXAMPLE_H_



/*!
 * Handles the counterexample generation control.
 *
 * @param parser An AutoParser to get the model from.
 */
 void generateCounterExample(std::shared_ptr<storm::models::AbstractModel<double>> model) {
	LOG4CPLUS_INFO(logger, "Starting counterexample generation.");
	LOG4CPLUS_INFO(logger, "Testing inputs...");

	storm::settings::Settings* s  = storm::settings::Settings::getInstance();

	// First test output directory.
	std::string outPath = s->getOptionByLongName("counterExample").getArgument(0).getValueAsString();
	if(outPath.back() != '/' && outPath.back() != '\\') {
		LOG4CPLUS_ERROR(logger, "The output path is not valid.");
		return;
	}
	std::ofstream testFile(outPath + "test.dot");
	if(testFile.fail()) {
		LOG4CPLUS_ERROR(logger, "The output path is not valid.");
		return;
	}
	testFile.close();
	std::remove((outPath + "test.dot").c_str());

 	// Differentiate between model types.
	if(model->getType() != storm::models::DTMC) {
		LOG4CPLUS_ERROR(logger, "Counterexample generation for the selected model type is not supported.");
		return;
	}

	// Get the Dtmc back from the AbstractModel
	// Note that the ownership of the object referenced by dtmc lies at the main function.
	// Thus, it must not be deleted.
	storm::models::Dtmc<double> dtmc = *(model->as<storm::models::Dtmc<double>>());
	LOG4CPLUS_INFO(logger, "Model is a DTMC.");

	// Get specified PRCTL formulas.
	if(!s->isSet("prctl")) {
		LOG4CPLUS_ERROR(logger, "No PRCTL formula file specified.");
		return;
	}

	std::string const chosenPrctlFile = s->getOptionByLongName("prctl").getArgument(0).getValueAsString();
	LOG4CPLUS_INFO(logger, "Parsing prctl file: " << chosenPrctlFile << ".");
	std::list<std::shared_ptr<storm::properties::prctl::PrctlFilter<double>>> formulaList = storm::parser::PrctlFileParser::parsePrctlFile(chosenPrctlFile);

	// Test for each formula if a counterexample can be generated for it.
	if(formulaList.size() == 0) {
		LOG4CPLUS_ERROR(logger, "No PRCTL formula found.");
		return;
	}

	// Get prctl file name without the filetype
	uint_fast64_t first = 0;
	if(chosenPrctlFile.find('/') != std::string::npos) {
		first = chosenPrctlFile.find_last_of('/') + 1;
	} else if(chosenPrctlFile.find('\\') != std::string::npos) {
		first = chosenPrctlFile.find_last_of('\\') + 1;
	}

	uint_fast64_t length;
	if(chosenPrctlFile.find_last_of('.') != std::string::npos && chosenPrctlFile.find_last_of('.') >= first) {
		length = chosenPrctlFile.find_last_of('.') - first;
	} else {
		length = chosenPrctlFile.length() - first;
	}

	std::string outFileName = chosenPrctlFile.substr(first, length);

	// Test formulas and do generation
	uint_fast64_t fIndex = 0;
	for (auto formula : formulaList) {

		// First check if it is a formula type for which a counterexample can be generated.
		if (std::dynamic_pointer_cast<storm::properties::prctl::AbstractStateFormula<double>>(formula->getChild()).get() == nullptr) {
			LOG4CPLUS_ERROR(logger, "Unexpected kind of formula. Expected a state formula.");
			continue;
		}

		std::shared_ptr<storm::properties::prctl::AbstractStateFormula<double>> stateForm = std::static_pointer_cast<storm::properties::prctl::AbstractStateFormula<double>>(formula->getChild());

		// Do some output
		std::cout << "Generating counterexample for formula " << fIndex << ":" << std::endl;
		LOG4CPLUS_INFO(logger, "Generating counterexample for formula " + std::to_string(fIndex) + ": ");
		std::cout << "\t" << formula->toString() << "\n" << std::endl;
		LOG4CPLUS_INFO(logger, formula->toString());

		// Now check if the model does not satisfy the formula.
		// That is if there is at least one initial state of the model that does not.

		// Also raise the logger threshold for the log file, so that the model check infos aren't logged (useless and there are lots of them)
		// Lower it again after the model check.
		logger.getAppender("mainFileAppender")->setThreshold(log4cplus::WARN_LOG_LEVEL);
		storm::storage::BitVector result = stateForm->check(*createPrctlModelChecker(dtmc));
		logger.getAppender("mainFileAppender")->setThreshold(log4cplus::INFO_LOG_LEVEL);

		if((result & dtmc.getInitialStates()).getNumberOfSetBits() == dtmc.getInitialStates().getNumberOfSetBits()) {
			std::cout << "Formula is satisfied. Can not generate counterexample.\n\n" << std::endl;
			LOG4CPLUS_INFO(logger, "Formula is satisfied. Can not generate counterexample.");
			continue;
		}

		// Generate counterexample
		storm::models::Dtmc<double> counterExample = storm::counterexamples::PathBasedSubsystemGenerator<double>::computeCriticalSubsystem(dtmc, stateForm);

		LOG4CPLUS_INFO(logger, "Found counterexample.");

		// Output counterexample
		// Do standard output
		std::cout << "Found counterexample with following properties: " << std::endl;
		counterExample.printModelInformationToStream(std::cout);
		std::cout << "For full Dtmc see " << outFileName << "_" << fIndex << ".dot at given output path.\n\n" << std::endl;

		// Write the .dot file
		std::ofstream outFile(outPath + outFileName + "_" + std::to_string(fIndex) + ".dot");
		if(outFile.good()) {
			counterExample.writeDotToStream(outFile, true, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, true);
			outFile.close();
		}

		fIndex++;
	}
 }
 
#endif