/*
 * AutoParser.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: Manuel S. Weiand
 */

#include "src/parser/AutoParser.h"

#include "src/parser/MappedFile.h"

#include "src/parser/DeterministicModelParser.h"
#include "src/parser/NondeterministicModelParser.h"
#include "src/parser/MarkovAutomatonParser.h"
#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"

#include "src/utility/cstring.h"
#include "src/utility/OsDetection.h"

namespace storm {
	namespace parser {

		using namespace storm::utility::cstring;

		std::shared_ptr<storm::models::AbstractModel<double>> AutoParser::parseModel(std::string const & transitionsFilename,
																					 std::string const & labelingFilename,
																					 std::string const & stateRewardFilename,
																					 std::string const & transitionRewardFilename) {

			// Find and parse the model type hint.
			storm::models::ModelType type = AutoParser::analyzeHint(transitionsFilename);

			// In case the hint string is unknown or could not be found, throw an exception.
			if (type == storm::models::Unknown) {
				LOG4CPLUS_ERROR(logger, "Could not determine file type of " << transitionsFilename << ".");
				LOG4CPLUS_ERROR(logger, "The first line of the file should contain a format hint. Please fix your file and try again.");
				throw storm::exceptions::WrongFormatException() << "Could not determine type of file " << transitionsFilename;
			} else {
				LOG4CPLUS_INFO(logger, "Model type seems to be " << type);
			}

			// Do the actual parsing.
			std::shared_ptr<storm::models::AbstractModel<double>> model;
			switch (type) {
				case storm::models::DTMC: {
					model.reset(new storm::models::Dtmc<double>(std::move(DeterministicModelParser::parseDtmc(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename))));
					break;
				}
				case storm::models::CTMC: {
					model.reset(new storm::models::Ctmc<double>(std::move(DeterministicModelParser::parseCtmc(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename))));
					break;
				}
				case storm::models::MDP: {
					model.reset(new storm::models::Mdp<double>(std::move(NondeterministicModelParser::parseMdp(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename))));
					break;
				}
				case storm::models::CTMDP: {
					model.reset(new storm::models::Ctmdp<double>(std::move(NondeterministicModelParser::parseCtmdp(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename))));
					break;
				}
				case storm::models::MA: {
					model.reset(new storm::models::MarkovAutomaton<double>(storm::parser::MarkovAutomatonParser::parseMarkovAutomaton(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename)));
					break;
				}
				default:
					LOG4CPLUS_WARN(logger, "Unknown/Unhandled Model Type which cannot be parsed.");  // Unknown
			}

			return model;
		}

		storm::models::ModelType AutoParser::analyzeHint(std::string const & filename) {
			storm::models::ModelType hintType = storm::models::Unknown;

			// Open the file.
			MappedFile file(filename.c_str());
            
			STORM_LOG_THROW(file.getDataSize() >= STORM_PARSER_AUTOPARSER_HINT_LENGTH, storm::exceptions::WrongFormatException, "File too short to be readable.");
			char const* fileData = file.getData();
            
			char filehintBuffer[STORM_PARSER_AUTOPARSER_HINT_LENGTH + 1];
			memcpy(filehintBuffer, fileData, STORM_PARSER_AUTOPARSER_HINT_LENGTH);
			filehintBuffer[STORM_PARSER_AUTOPARSER_HINT_LENGTH] = 0;

			// Find and read in the hint.
			std::string formatString = "%" + std::to_string(STORM_PARSER_AUTOPARSER_HINT_LENGTH) + "s";
			char hint[5];
		#ifdef WINDOWS
			sscanf_s(filehintBuffer, formatString.c_str(), hint, STORM_PARSER_AUTOPARSER_HINT_LENGTH + 1);
		#else
			int ret = sscanf(filehintBuffer, formatString.c_str(), hint);
		#endif
			for (char* c = hint; *c != '\0'; c++) *c = toupper(*c);
            
			// Check if the hint value is known and store the appropriate enum value.
			if (strcmp(hint, "DTMC") == 0) hintType = storm::models::DTMC;
			else if (strcmp(hint, "CTMC") == 0) hintType = storm::models::CTMC;
			else if (strcmp(hint, "MDP") == 0) hintType = storm::models::MDP;
			else if (strcmp(hint, "CTMDP") == 0) hintType = storm::models::CTMDP;
			else if (strcmp(hint, "MA") == 0) hintType = storm::models::MA;

			return hintType;
		}

	} // namespace parser
} // namespace storm
