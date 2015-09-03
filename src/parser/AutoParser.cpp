#include "src/parser/AutoParser.h"

#include "src/models/sparse/StandardRewardModel.h"

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

        std::shared_ptr<storm::models::sparse::Model<double>> AutoParser::parseModel(std::string const& transitionsFilename,
                std::string const& labelingFilename,
                std::string const& stateRewardFilename,
                std::string const& transitionRewardFilename,
                std::string const& choiceLabelingFilename) {

            // Find and parse the model type hint.
            storm::models::ModelType type = AutoParser::analyzeHint(transitionsFilename);

            // Do the actual parsing.
            std::shared_ptr<storm::models::sparse::Model<double>> model;
            switch (type) {
                case storm::models::ModelType::Dtmc:
                {
                    model.reset(new storm::models::sparse::Dtmc<double>(std::move(DeterministicModelParser::parseDtmc(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename))));
                    break;
                }
                case storm::models::ModelType::Ctmc:
                {
                    model.reset(new storm::models::sparse::Ctmc<double>(std::move(DeterministicModelParser::parseCtmc(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename))));
                    break;
                }
                case storm::models::ModelType::Mdp:
                {
                    model.reset(new storm::models::sparse::Mdp<double>(std::move(NondeterministicModelParser::parseMdp(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename, choiceLabelingFilename))));
                    break;
                }
                case storm::models::ModelType::MarkovAutomaton:
                {
                    model.reset(new storm::models::sparse::MarkovAutomaton<double>(storm::parser::MarkovAutomatonParser::parseMarkovAutomaton(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename)));
                    break;
                }
                default:
                    LOG4CPLUS_WARN(logger, "Unknown/Unhandled Model Type which cannot be parsed."); // Unknown
            }

            return model;
        }

        storm::models::ModelType AutoParser::analyzeHint(std::string const & filename) {
            storm::models::ModelType hintType = storm::models::ModelType::Dtmc;

            // Open the file.
            MappedFile file(filename.c_str());

            STORM_LOG_THROW(file.getDataSize() >= STORM_PARSER_AUTOPARSER_HINT_LENGTH, storm::exceptions::WrongFormatException, "File too short to be readable.");
            char const* fileData = file.getData();

            char filehintBuffer[STORM_PARSER_AUTOPARSER_HINT_LENGTH + 1];
            memcpy(filehintBuffer, fileData, STORM_PARSER_AUTOPARSER_HINT_LENGTH);
            filehintBuffer[STORM_PARSER_AUTOPARSER_HINT_LENGTH] = 0;

            // Find and read in the hint.
            std::string formatString = "%" + std::to_string(STORM_PARSER_AUTOPARSER_HINT_LENGTH) + "s";
            char hint[STORM_PARSER_AUTOPARSER_HINT_LENGTH + 1];
#ifdef WINDOWS
            sscanf_s(filehintBuffer, formatString.c_str(), hint, STORM_PARSER_AUTOPARSER_HINT_LENGTH + 1);
#else
            sscanf(filehintBuffer, formatString.c_str(), hint);
#endif
            for (char* c = hint; *c != '\0'; c++) *c = toupper(*c);

            // Check if the hint value is known and store the appropriate enum value.
            if (strcmp(hint, "DTMC") == 0) hintType = storm::models::ModelType::Dtmc;
            else if (strcmp(hint, "CTMC") == 0) hintType = storm::models::ModelType::Ctmc;
            else if (strcmp(hint, "MDP") == 0) hintType = storm::models::ModelType::Mdp;
            else if (strcmp(hint, "MA") == 0) hintType = storm::models::ModelType::MarkovAutomaton;
            else STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Unable to find model hint in explicit input.");

            return hintType;
        }

    } // namespace parser
} // namespace storm
