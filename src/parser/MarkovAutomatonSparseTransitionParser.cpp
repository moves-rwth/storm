#include "MarkovAutomatonSparseTransitionParser.h"

#include "src/settings/Settings.h"

namespace storm {
    namespace parser {
 
        MarkovAutomatonSparseTransitionParser::FirstPassResult MarkovAutomatonSparseTransitionParser::firstPass(char* buf, SupportedLineEndingsEnum lineEndings) {
            MarkovAutomatonSparseTransitionParser::FirstPassResult result;

            // Skip the format hint.
            buf = storm::parser::forwardToNextLine(buf, lineEndings);
            
            // Now read the transitions.
            int_fast64_t source, target = -1;
            int_fast64_t lastsource = -1;
            bool encounteredEOF = false;
            bool stateHasMarkovianChoice = false;
            while (buf[0] != '\0' && !encounteredEOF) {
                // At the current point, the next thing to read is the source state of the next choice to come.
                source = checked_strtol(buf, &buf);
                
                // Check if we encountered a state index that is bigger than all previously seen ones and record it if necessary.
                if (source > result.highestStateIndex) {
                    result.highestStateIndex = source;
                }
                
                // If we have skipped some states, we need to reserve the space for the self-loop insertion in the second pass.
                if (source > lastsource + 1) {
                    result.numberOfNonzeroEntries += source - lastsource - 1;
                    result.numberOfChoices += source - lastsource - 1;
                } else {
                    ++result.numberOfChoices;
                }
                
                // If we have moved to the next state, we need to clear the flag that stores whether or not the source has a Markovian choice.
                if (source != lastsource) {
                    stateHasMarkovianChoice = false;
                }
                
                // Record that the current source was the last source.
                lastsource = source;
                
                char actionNameBuffer[20];
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
                bool isMarkovianChoice = false;
                if (strcmp(actionNameBuffer, "!") == 0) {
                    isMarkovianChoice = true;
                } else {
                    isMarkovianChoice = false;
                }
                
                if (isMarkovianChoice && stateHasMarkovianChoice) {
                    LOG4CPLUS_ERROR(logger, "The state " << source << " has multiple Markovian choices.");
                    throw storm::exceptions::WrongFormatException() << "The state " << source << " has multiple Markovian choices.";
                }
                
                buf = forwardToNextLine(buf, lineEndings);
                
                // Now that we have the source state and the information whether or not the current choice is probabilistic or Markovian, we need to read the list of successors and the probabilities/rates.
                bool hasSuccessorState = false;
                bool encounteredNewDistribution = false;
                
                // At this point, we need to check whether there is an additional successor or we have reached the next choice for the same or a different state.
                do {
                    // Now parse the next symbol to see whether there is another target state for the current choice
                    // or not.
                    char star[1];
#ifdef WINDOWS
                    length = sscanf_s(buf, "%1s\n", star, 1);
#else
                    length = sscanf(buf, "%1s\n", star);
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
                        // We need to record that we found at least one successor state for the current choice.
                        hasSuccessorState = true;
                        
                        // As we have encountered a "*", we know that there is an additional successor state for the current choice.
                        buf += strlen(star);
                        
                        // Now we need to read the successor state and check if we already saw a higher state index.
                        target = checked_strtol(buf, &buf);
                        if (target > result.highestStateIndex) {
                            result.highestStateIndex = target;
                        }
                                                
                        // And the corresponding probability/rate.
                        double val = checked_strtod(buf, &buf);
                        if (val <= 0.0) {
                            LOG4CPLUS_ERROR(logger, "Illegal probability/rate value " << val << ".");
                            throw storm::exceptions::WrongFormatException() << "Illegal probability/rate value " << val << ".";
                        }
                                                
                        // As we found a new successor, we need to increase the number of nonzero entries.
                        ++result.numberOfNonzeroEntries;
                        
                        buf = forwardToNextLine(buf, lineEndings);
                    } else {
                        // If it was not a "*", we have to assume that we encountered the beginning of a new choice definition. In this case, we don't move the pointer
                        // to the buffer, because we still need to read the new source state.
                        encounteredNewDistribution = true;
                    }
                } while (!encounteredEOF && !encounteredNewDistribution);
            }
            
            return result;
        }
        
        MarkovAutomatonSparseTransitionParser::ResultType MarkovAutomatonSparseTransitionParser::secondPass(char* buf, SupportedLineEndingsEnum lineEndings, FirstPassResult const& firstPassResult) {
            ResultType result(firstPassResult);
            
            bool fixDeadlocks = storm::settings::Settings::getInstance()->isSet("fixDeadlocks");
            
            // Skip the format hint.
            buf = storm::parser::forwardToNextLine(buf, lineEndings);
            
            // Now read the transitions.
            int_fast64_t source, target = -1;
            int_fast64_t lastsource = -1;
            bool encounteredEOF = false;
            uint_fast64_t currentChoice = 0;
            while (buf[0] != '\0' && !encounteredEOF) {
                // At the current point, the next thing to read is the source state of the next choice to come.
                source = checked_strtol(buf, &buf);
                
                // If we have skipped some states, we need to insert self-loops if requested.
                if (source > lastsource + 1) {
                    if (fixDeadlocks) {
                        for (uint_fast64_t index = lastsource + 1; index < source; ++source) {
                            result.nondeterministicChoiceIndices[index] = currentChoice;
                            result.transitionMatrix.addNextValue(currentChoice, index, 1);
                            ++currentChoice;
                        }
                    } else {
                        LOG4CPLUS_ERROR(logger, "Found deadlock states (e.g. " << lastsource + 1 << ") during parsing. Please fix them or set the appropriate flag.");
                        throw storm::exceptions::WrongFormatException() << "Found deadlock states (e.g. " << lastsource + 1 << ") during parsing. Please fix them or set the appropriate flag.";
                    }
                }
                
                if (source != lastsource) {
                    // If we skipped to a new state we need to record the beginning of the choices in the nondeterministic choice indices.
                    result.nondeterministicChoiceIndices[source] = currentChoice;
                }
                
                // Record that the current source was the last source.
                lastsource = source;
                
                char actionNameBuffer[20];
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
                bool isMarkovianChoice = false;
                if (strcmp(actionNameBuffer, "!") == 0) {
                    isMarkovianChoice = true;
                    
                    // Mark the current state as a Markovian one.
                    result.markovianStates.set(source, true);
                } else {
                    isMarkovianChoice = false;
                }
                
                buf = forwardToNextLine(buf, lineEndings);
                
                // Now that we have the source state and the information whether or not the current choice is probabilistic or Markovian, we need to read the list of successors and the probabilities/rates.
                bool hasSuccessorState = false;
                bool encounteredNewDistribution = false;
                
                // At this point, we need to check whether there is an additional successor or we have reached the next choice for the same or a different state.
                do {
                    // Now parse the next symbol to see whether there is another target state for the current choice
                    // or not.
                    char star[1];
#ifdef WINDOWS
                    length = sscanf_s(buf, "%1s\n", star, 1);
#else
                    length = sscanf(buf, "%1s\n", star);
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
                        // We need to record that we found at least one successor state for the current choice.
                        hasSuccessorState = true;
                        
                        // As we have encountered a "*", we know that there is an additional successor state for the current choice.
                        buf += strlen(star);
                        
                        // Now we need to read the successor state and check if we already saw a higher state index.
                        target = checked_strtol(buf, &buf);
                        
                        // And the corresponding probability/rate.
                        double val = checked_strtod(buf, &buf);

                        // Record the value as well as the exit rate in case of a Markovian choice.
                        result.transitionMatrix.addNextValue(currentChoice, target, val);
                        if (isMarkovianChoice) {
                            result.exitRates[source] += val;
                        }
                        
                        buf = forwardToNextLine(buf, lineEndings);
                    } else {
                        // If it was not a "*", we have to assume that we encountered the beginning of a new choice definition. In this case, we don't move the pointer
                        // to the buffer, because we still need to read the new source state.
                        encounteredNewDistribution = true;
                    }
                } while (!encounteredEOF && !encounteredNewDistribution);
                
                ++currentChoice;
            }
            
            // As we have added all entries at this point, we need to finalize the matrix.
            result.transitionMatrix.finalize();
            
            // Put a sentinel element at the end.
            result.nondeterministicChoiceIndices[firstPassResult.highestStateIndex + 1] = currentChoice;
            
            return result;
        }
        
        MarkovAutomatonSparseTransitionParser::ResultType MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(std::string const& filename) {
            // Set the locale to correctly recognize floating point numbers.
            setlocale(LC_NUMERIC, "C");
            
            if (!fileExistsAndIsReadable(filename.c_str())) {
                LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
                throw storm::exceptions::WrongFormatException() << "Error while parsing " << filename << ": File does not exist or is not readable.";
            }
            
            // Determine used line endings.
            SupportedLineEndingsEnum lineEndings = findUsedLineEndings(filename, true);
            
            // Open file and prepare pointer to buffer.
            MappedFile file(filename.c_str());
            char* buf = file.data;
            
            return secondPass(buf, lineEndings, firstPass(buf, lineEndings));
        }
        
    } // namespace parser
} // namespace storm
