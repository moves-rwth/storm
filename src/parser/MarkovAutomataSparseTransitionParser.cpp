#include "MarkovAutomataSparseTransitionParser.h"

namespace storm {
    namespace parser {
 
        MarkovAutomataSparseTransitionParser::FirstPassResult MarkovAutomataSparseTransitionParser::performFirstPass(char* buf, SupportedLineEndingsEnum lineEndings, RewardMatrixInformationStruct* rewardMatrixInformation) {
            bool isRewardFile = rewardMatrixInformation != nullptr;
            
            MarkovAutomataSparseTransitionParser::FirstPassResult result;
            
            /*
             *	Check file header and extract number of transitions.
             */
            if (!isRewardFile) {
                // skip format hint
                buf = storm::parser::forwardToNextLine(buf, lineEndings);
            }
            
            /*
             *	Read all transitions.
             */
            int_fast64_t source, target, choice, lastchoice = -1;
            int_fast64_t lastsource = -1;
            uint_fast64_t nonzero = 0;
            double val;
            result.numberOfChoices = 0;
            result.numberOfStates = 0;
            bool isMarkovianChoice = false;
            char actionNameBuffer[20];
            char star[20];
            bool encounteredEOF = false;
            while (buf[0] != '\0') {
                /*
                 *	Read source state and choice.
                 */
                source = checked_strtol(buf, &buf);
                
                // Check if we encountered a state index that is bigger than all previously seen ones and record it if necessary.
                if (source > result.numberOfStates) {
                    result.numberOfStates = source;
                }
                
                if (isRewardFile) {
                    // FIXME: Fill this
                } else {
                    // If we have skipped some states, we need to reserve the space for the self-loop insertion in the second pass.
                    if (source > lastsource + 1) {
                        nonzero += source - lastsource - 1;
                        result.numberOfChoices += source - lastsource - 1;
                    } else if (source != lastsource) {
                        // If we have switched the source state, we need to reserve one row more.
                        ++result.numberOfChoices;
                    }
                }
                
#ifdef WINDOWS
                int length = sscanf_s(buf, "%20s\n", actionNameBuffer, 20);
#else
                int length = sscanf(buf, "%20s\n", actionNameBuffer);
#endif
                
                // If the number of arguments filled is not one, there was an error.
                if (length != 1) {
                    LOG4CPLUS_ERROR(logger, "Parsing error.");
                    throw storm::exceptions::WrongFormatException() << "Parsing error.";
                } else {
                    // If the action name was parsed successfully, we need to move by the corresponding number of characters.
                    buf += strlen(actionNameBuffer);
                }
                
                // Depending on the action name, the choice is either a probabilitic one or a markovian one.
                if (strcmp(actionNameBuffer, "!") == 0) {
                    isMarkovianChoice = true;
                } else {
                    isMarkovianChoice = false;
                }
                
                // Proceed to beginning of next line.
                switch (lineEndings) {
                    case SupportedLineEndingsEnum::SlashN:
                        buf += strcspn(buf, " \t\n");
                        break;
                    case SupportedLineEndingsEnum::SlashR:
                        buf += strcspn(buf, " \t\r");
                        break;
                    case SupportedLineEndingsEnum::SlashRN:
                        buf += strcspn(buf, " \t\r\n");
                        break;
                    default:
                    case storm::parser::SupportedLineEndingsEnum::Unsupported:
                        // This line will never be reached as the parser would have thrown already.
                        throw;
                        break;
                }
                buf = trimWhitespaces(buf);
                
                // Now that we have the source state and the information whether or not the current choice is probabilistic or Markovian, we need to read the list of successors and the probabilities/rates.
                bool hasSuccessorState = false;
                
                // At this point, we need to check whether there is an additional successor or we have reached the next choice for the same or a different state.
#ifdef WINDOWS
                length = sscanf_s(buf, "%20s\n", star, 20);
#else
                length = sscanf(buf, "%20s\n", star);
#endif

                // If the number of arguments filled is not one, there was an error.
                if (length == EOF) {
                    if (!hasSuccessorState) {
                        LOG4CPLUS_ERROR(logger, "Premature end-of-file. Expected at least one successor state for state " << source << " under action " << actionNameBuffer << ".");
                        throw storm::exceptions::WrongFormatException() << "Premature end-of-file. Expected at least one successor state for state " << source << " under action " << actionNameBuffer << ".";
                    } else {
                        // If there was at least one successor for the current choice, this is legal and we need to move on.
                        encounteredEOF = true;
                    }
                } else if (length != 1) {
                    LOG4CPLUS_ERROR(logger, "Parsing error.");
                    throw storm::exceptions::WrongFormatException() << "Parsing error.";
                } else if (strcmp(star, "*") == 0) {
                    // As we have encountered a "*", we know that there is an additional successor state for the current choice.
                    ++result.numberOfNonzeroEntries;
                    buf += strlen(star);
                } else {
                    // If it was not a "*", we have to assume that we encountered the beginning of a new choice definition. In this case, we don't move the pointer
                    // to the buffer, because we still need to read the new source state.
                }
                
                std::cout << "Got here! " << isMarkovianChoice << " // " << actionNameBuffer << " // " << strlen(actionNameBuffer) << std::endl;
                
                
                if (isRewardFile) {
                    // If we have switched the source state, we possibly need to insert the rows of the last
                    // last source state.
                    if (source != lastsource && lastsource != -1) {
                        result.numberOfChoices += lastchoice - ((*rewardMatrixInformation->nondeterministicChoiceIndices)[lastsource + 1] - (*rewardMatrixInformation->nondeterministicChoiceIndices)[lastsource] - 1);
                    }
                    
                    // If we skipped some states, we need to reserve empty rows for all their nondeterministic
                    // choices.
                    for (int_fast64_t i = lastsource + 1; i < source; ++i) {
                        result.numberOfChoices += ((*rewardMatrixInformation->nondeterministicChoiceIndices)[i + 1] - (*rewardMatrixInformation->nondeterministicChoiceIndices)[i]);
                    }
                    
                    // If we advanced to the next state, but skipped some choices, we have to reserve rows for them.
                    if (source != lastsource) {
                        result.numberOfChoices += choice + 1;
                    } else if (choice != lastchoice) {
                        result.numberOfChoices += choice - lastchoice;
                    }
                } else {
                    // If we have skipped some states, we need to reserve the space for the self-loop insertion
                    // in the second pass.
                    if (source > lastsource + 1) {
                        nonzero += source - lastsource - 1;
                        result.numberOfChoices += source - lastsource - 1;
                    } else if (source != lastsource || choice != lastchoice) {
                        // If we have switched the source state or the nondeterministic choice, we need to
                        // reserve one row more.
                        ++result.numberOfChoices;
                    }
                }
                
                // Read target and check if we encountered a state index that is bigger than all previously
                // seen.
                target = checked_strtol(buf, &buf);
                if (target > result.numberOfStates) {
                    result.numberOfStates = target;
                }
                
                // Read value and check whether it's positive.
                val = checked_strtod(buf, &buf);
                if (val < 0.0) {
                    LOG4CPLUS_ERROR(logger, "Expected a positive probability but got \"" << std::string(buf, 0, 16) << "\".");
                    throw storm::exceptions::InvalidArgumentException() << "Expected a positive probability but got \"" << std::string(buf, 0, 16) << "\".";
                }
                
                lastchoice = choice;
                lastsource = source;
                
                /*
                 *	Increase number of non-zero values.
                 */
                nonzero++;
                
                // The PRISM output format lists the name of the transition in the fourth column,
                // but omits the fourth column if it is an internal action. In either case, however, the third column
                // is followed by a space. We need to skip over that space first (instead of trimming whitespaces),
                // before we can skip to the line end, because trimming the white spaces will proceed to the next line
                // in case there is no action label in the fourth column.
                if (buf[0] == ' ') {
                    ++buf;
                }
                
                /*
                 *	Proceed to beginning of next line.
                 */
                switch (lineEndings) {
                    case SupportedLineEndingsEnum::SlashN:
                        buf += strcspn(buf, " \t\n");
                        break;
                    case SupportedLineEndingsEnum::SlashR:
                        buf += strcspn(buf, " \t\r");
                        break;
                    case SupportedLineEndingsEnum::SlashRN:
                        buf += strcspn(buf, " \t\r\n");
                        break;
                    default:
                    case storm::parser::SupportedLineEndingsEnum::Unsupported:
                        // This Line will never be reached as the Parser would have thrown already.
                        throw;
                        break;
                }
                buf = trimWhitespaces(buf);
            }
            
            if (isRewardFile) {
                // If not all rows were filled for the last state, we need to insert them.
                result.numberOfChoices += lastchoice - ((*rewardMatrixInformation->nondeterministicChoiceIndices)[lastsource + 1] - (*rewardMatrixInformation->nondeterministicChoiceIndices)[lastsource] - 1);
                
                // If we skipped some states, we need to reserve empty rows for all their nondeterministic
                // choices.
                for (uint_fast64_t i = lastsource + 1; i < rewardMatrixInformation->nondeterministicChoiceIndices->size() - 1; ++i) {
                    result.numberOfChoices += ((*rewardMatrixInformation->nondeterministicChoiceIndices)[i + 1] - (*rewardMatrixInformation->nondeterministicChoiceIndices)[i]);
                }
            }
            
            exit(-1);

            return result;
        }
        
        MarkovAutomataSparseTransitionParser::ResultType MarkovAutomataSparseTransitionParser::parseMarkovAutomataTransitions(std::string const& filename, RewardMatrixInformationStruct* rewardMatrixInformation) {
            /*
             *	Enforce locale where decimal point is '.'.
             */
            setlocale(LC_NUMERIC, "C");
            
            if (!fileExistsAndIsReadable(filename.c_str())) {
                LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
                throw storm::exceptions::WrongFormatException();
            }
            
            bool isRewardFile = rewardMatrixInformation != nullptr;
            
            /*
             *	Find out about the used line endings.
             */
            SupportedLineEndingsEnum lineEndings = findUsedLineEndings(filename, true);
            
            /*
             *	Open file.
             */
            MappedFile file(filename.c_str());
            char* buf = file.data;
            
            FirstPassResult firstPassResult = performFirstPass(buf, lineEndings, rewardMatrixInformation);
            
            return ResultType();
            
        }
        
    }
}
