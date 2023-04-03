#include "storm-parsers/parser/DirectEncodingParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <iostream>
#include <regex>
#include <string>

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/exceptions/AbortException.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/settings/SettingsManager.h"

#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"

#include "storm/io/file.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/settings/SettingsManager.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/builder.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

namespace storm {
namespace parser {

template<typename ValueType, typename RewardModelType>
std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> DirectEncodingParser<ValueType, RewardModelType>::parseModel(
    std::string const& filename, DirectEncodingParserOptions const& options) {
    // Load file
    STORM_LOG_INFO("Reading from file " << filename);
    std::ifstream file;
    storm::utility::openFile(filename, file);
    std::string line;

    // Initialize
    ValueParser<ValueType> valueParser;
    bool sawType = false;
    bool sawParameters = false;
    std::unordered_map<std::string, ValueType> placeholders;
    size_t nrStates = 0;
    size_t nrChoices = 0;
    storm::models::ModelType type;
    std::vector<std::string> rewardModelNames;
    std::shared_ptr<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>> modelComponents;

    // Parse header
    while (storm::utility::getline(file, line)) {
        if (line.empty() || boost::starts_with(line, "//")) {
            continue;
        }

        if (boost::starts_with(line, "@type: ")) {
            // Parse type
            STORM_LOG_THROW(!sawType, storm::exceptions::WrongFormatException, "Type declared twice");
            type = storm::models::getModelType(line.substr(7));
            STORM_LOG_TRACE("Model type: " << type);
            STORM_LOG_THROW(type != storm::models::ModelType::S2pg, storm::exceptions::NotSupportedException,
                            "Stochastic Two Player Games in DRN format are not supported.");
            sawType = true;

        } else if (line == "@parameters") {
            // Parse parameters
            STORM_LOG_THROW(!sawParameters, storm::exceptions::WrongFormatException, "Parameters declared twice");
            storm::utility::getline(file, line);
            if (line != "") {
                std::vector<std::string> parameters;
                boost::split(parameters, line, boost::is_any_of(" "));
                for (std::string const& parameter : parameters) {
                    STORM_LOG_TRACE("New parameter: " << parameter);
                    valueParser.addParameter(parameter);
                }
            }
            sawParameters = true;

        } else if (line == "@placeholders") {
            // Parse placeholders
            while (storm::utility::getline(file, line)) {
                size_t posColon = line.find(':');
                STORM_LOG_THROW(posColon != std::string::npos, storm::exceptions::WrongFormatException, "':' not found.");
                std::string placeName = line.substr(0, posColon - 1);
                STORM_LOG_THROW(placeName.front() == '$', storm::exceptions::WrongFormatException, "Placeholder must start with dollar symbol $.");
                std::string valueStr = line.substr(posColon + 2);
                ValueType value = parseValue(valueStr, placeholders, valueParser);
                STORM_LOG_TRACE("Placeholder " << placeName << " for value " << value);
                auto ret = placeholders.insert(std::make_pair(placeName.substr(1), value));
                STORM_LOG_THROW(ret.second, storm::exceptions::WrongFormatException, "Placeholder '$" << placeName << "' was already defined before.");
                if (file.peek() == '@') {
                    // Next character is @ -> placeholder definitions ended
                    break;
                }
            }
        } else if (line == "@reward_models") {
            // Parse reward models
            STORM_LOG_THROW(rewardModelNames.empty(), storm::exceptions::WrongFormatException, "Reward model names declared twice");
            storm::utility::getline(file, line);
            boost::split(rewardModelNames, line, boost::is_any_of("\t "));
        } else if (line == "@nr_states") {
            // Parse no. of states
            STORM_LOG_THROW(nrStates == 0, storm::exceptions::WrongFormatException, "Number states declared twice");
            storm::utility::getline(file, line);
            nrStates = parseNumber<size_t>(line);
        } else if (line == "@nr_choices") {
            STORM_LOG_THROW(nrChoices == 0, storm::exceptions::WrongFormatException, "Number of actions declared twice");
            storm::utility::getline(file, line);
            nrChoices = parseNumber<size_t>(line);
        } else if (line == "@model") {
            // Parse rest of the model
            STORM_LOG_THROW(sawType, storm::exceptions::WrongFormatException, "Type has to be declared before model.");
            STORM_LOG_THROW(nrStates != 0, storm::exceptions::WrongFormatException, "No. of states has to be declared before model.");
            STORM_LOG_THROW(!options.buildChoiceLabeling || nrChoices != 0, storm::exceptions::WrongFormatException,
                            "No. of actions (@nr_choices) has to be declared before model.");
            STORM_LOG_WARN_COND(nrChoices != 0, "No. of actions has to be declared. We may continue now, but future versions might not support this.");
            // Construct model components
            modelComponents = parseStates(file, type, nrStates, nrChoices, placeholders, valueParser, rewardModelNames, options);
            break;
        } else {
            STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Could not parse line '" << line << "'.");
        }
    }
    // Done parsing
    storm::utility::closeFile(file);

    // Build model
    return storm::utility::builder::buildModelFromComponents(type, std::move(*modelComponents));
}

template<typename ValueType, typename RewardModelType>
std::shared_ptr<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>> DirectEncodingParser<ValueType, RewardModelType>::parseStates(
    std::istream& file, storm::models::ModelType type, size_t stateSize, size_t nrChoices, std::unordered_map<std::string, ValueType> const& placeholders,
    ValueParser<ValueType> const& valueParser, std::vector<std::string> const& rewardModelNames, DirectEncodingParserOptions const& options) {
    // Initialize
    auto modelComponents = std::make_shared<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>>();
    bool nonDeterministic =
        (type == storm::models::ModelType::Mdp || type == storm::models::ModelType::MarkovAutomaton || type == storm::models::ModelType::Pomdp);
    bool continuousTime = (type == storm::models::ModelType::Ctmc || type == storm::models::ModelType::MarkovAutomaton);
    storm::storage::SparseMatrixBuilder<ValueType> builder = storm::storage::SparseMatrixBuilder<ValueType>(0, 0, 0, false, nonDeterministic, 0);
    modelComponents->stateLabeling = storm::models::sparse::StateLabeling(stateSize);
    modelComponents->observabilityClasses = std::vector<uint32_t>();
    modelComponents->observabilityClasses->resize(stateSize);
    if (options.buildChoiceLabeling) {
        modelComponents->choiceLabeling = storm::models::sparse::ChoiceLabeling(nrChoices);
    }
    std::vector<std::vector<ValueType>> stateRewards;
    std::vector<std::vector<ValueType>> actionRewards;
    if (continuousTime) {
        modelComponents->exitRates = std::vector<ValueType>(stateSize);
        if (type == storm::models::ModelType::MarkovAutomaton) {
            modelComponents->markovianStates = storm::storage::BitVector(stateSize);
        }
    }
    // We parse rates for continuous time models.
    if (type == storm::models::ModelType::Ctmc) {
        modelComponents->rateTransitions = true;
    }

    // Iterate over all lines
    std::string line;
    size_t row = 0;
    size_t state = 0;
    uint64_t lineNumber = 0;
    bool firstState = true;
    bool firstActionForState = true;
    while (storm::utility::getline(file, line)) {
        lineNumber++;
        if (boost::starts_with(line, "//")) {
            continue;
        }
        if (line.empty()) {
            continue;
        }
        STORM_LOG_TRACE("Parsing line no " << lineNumber << " : " << line);
        boost::trim_left(line);
        if (boost::starts_with(line, "state ")) {
            // New state
            if (firstState) {
                firstState = false;
            } else {
                ++state;
                ++row;
            }
            firstActionForState = true;
            STORM_LOG_TRACE("New state " << state);
            STORM_LOG_THROW(state <= stateSize, storm::exceptions::WrongFormatException, "More states detected than declared (in @nr_states).");

            // Parse state id
            line = line.substr(6);  // Remove "state "
            std::string curString = line;
            size_t posEnd = line.find(" ");
            if (posEnd != std::string::npos) {
                curString = line.substr(0, posEnd);
                line = line.substr(posEnd + 1);
            } else {
                line = "";
            }
            size_t parsedId = parseNumber<size_t>(curString);
            STORM_LOG_THROW(state == parsedId, storm::exceptions::WrongFormatException,
                            "In line " << lineNumber << " state ids are not ordered and without gaps. Expected " << state << " but got " << parsedId << ".");
            if (nonDeterministic) {
                STORM_LOG_TRACE("new Row Group starts at " << row << ".");
                builder.newRowGroup(row);
                STORM_LOG_THROW(nrChoices == 0 || builder.getCurrentRowGroupCount() <= nrChoices, storm::exceptions::WrongFormatException,
                                "More actions detected than declared (in @nr_choices).");
            }

            if (continuousTime) {
                // Parse exit rate for CTMC or MA
                STORM_LOG_THROW(boost::starts_with(line, "!"), storm::exceptions::WrongFormatException, "Exit rate missing in " << lineNumber);
                line = line.substr(1);  // Remove "!"
                curString = line;
                posEnd = line.find(" ");
                if (posEnd != std::string::npos) {
                    curString = line.substr(0, posEnd);
                    line = line.substr(posEnd + 1);
                } else {
                    line = "";
                }
                ValueType exitRate = parseValue(curString, placeholders, valueParser);
                if (type == storm::models::ModelType::MarkovAutomaton && !storm::utility::isZero<ValueType>(exitRate)) {
                    modelComponents->markovianStates.get().set(state);
                }
                STORM_LOG_TRACE("Exit rate " << exitRate);
                modelComponents->exitRates.get()[state] = exitRate;
            }

            if (boost::starts_with(line, "[")) {
                // Parse rewards
                size_t posEndReward = line.find(']');
                STORM_LOG_THROW(posEndReward != std::string::npos, storm::exceptions::WrongFormatException, "] missing in line " << lineNumber << " .");
                std::string rewardsStr = line.substr(1, posEndReward - 1);
                STORM_LOG_TRACE("State rewards: " << rewardsStr);
                std::vector<std::string> rewards;
                boost::split(rewards, rewardsStr, boost::is_any_of(","));
                if (stateRewards.size() < rewards.size()) {
                    stateRewards.resize(rewards.size());
                }
                auto stateRewardsIt = stateRewards.begin();
                for (auto const& rew : rewards) {
                    auto rewardValue = parseValue(rew, placeholders, valueParser);
                    if (!storm::utility::isZero(rewardValue)) {
                        if (stateRewardsIt->empty()) {
                            stateRewardsIt->resize(stateSize, storm::utility::zero<ValueType>());
                        }
                        (*stateRewardsIt)[state] = std::move(rewardValue);
                    }
                    ++stateRewardsIt;
                }
                line = line.substr(posEndReward + 1);
            }

            if (type == storm::models::ModelType::Pomdp) {
                if (boost::starts_with(line, "{")) {
                    size_t posEndObservation = line.find("}");
                    std::string observation = line.substr(1, posEndObservation - 1);
                    STORM_LOG_TRACE("State observation " << observation);
                    modelComponents->observabilityClasses.value()[state] = std::stoi(observation);
                    line = line.substr(posEndObservation + 1);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Expected an observation for state " << state << " in line " << lineNumber);
                }
            }

            // Parse labels
            if (!line.empty()) {
                std::vector<std::string> labels;
                // Labels are separated by whitespace and can optionally be enclosed in quotation marks
                // Regex for labels with two cases:
                // * Enclosed in quotation marks: \"([^\"]+?)\"(?=(\s|$|\"))
                //   - First part matches string enclosed in quotation marks with no quotation mark inbetween (\"([^\"]+?)\")
                //   - second part is lookahead which ensures that after the matched part either whitespace, end of line or a new quotation mark follows
                //   (?=(\s|$|\"))
                // * Separated by whitespace: [^\s\"]+?(?=(\s|$))
                //   - First part matches string without whitespace and quotation marks [^\s\"]+?
                //   - Second part is again lookahead matching whitespace or end of line (?=(\s|$))
                std::regex labelRegex(R"(\"([^\"]+?)\"(?=(\s|$|\"))|([^\s\"]+?(?=(\s|$))))");

                // Iterate over matches
                auto match_begin = std::sregex_iterator(line.begin(), line.end(), labelRegex);
                auto match_end = std::sregex_iterator();
                for (std::sregex_iterator i = match_begin; i != match_end; ++i) {
                    std::smatch match = *i;
                    // Find matched group and add as label
                    if (match.length(1) > 0) {
                        labels.push_back(match.str(1));
                    } else {
                        labels.push_back(match.str(3));
                    }
                }

                for (std::string const& label : labels) {
                    if (!modelComponents->stateLabeling.containsLabel(label)) {
                        modelComponents->stateLabeling.addLabel(label);
                    }
                    modelComponents->stateLabeling.addLabelToState(label, state);
                    STORM_LOG_TRACE("New label: '" << label << "'");
                }
            }
        } else if (boost::starts_with(line, "action ")) {
            // New action
            if (firstActionForState) {
                firstActionForState = false;
            } else {
                ++row;
            }
            STORM_LOG_TRACE("New action: " << row);
            line = line.substr(7);
            std::string curString = line;
            size_t posEnd = line.find(" ");
            if (posEnd != std::string::npos) {
                curString = line.substr(0, posEnd);
                line = line.substr(posEnd + 1);
            } else {
                line = "";
            }

            // curString contains action name.
            if (options.buildChoiceLabeling) {
                if (curString != "__NOLABEL__") {
                    if (!modelComponents->choiceLabeling.value().containsLabel(curString)) {
                        modelComponents->choiceLabeling.value().addLabel(curString);
                    }
                    modelComponents->choiceLabeling.value().addLabelToChoice(curString, row);
                }
            }
            // Check for rewards
            if (boost::starts_with(line, "[")) {
                // Rewards found
                size_t posEndReward = line.find(']');
                STORM_LOG_THROW(posEndReward != std::string::npos, storm::exceptions::WrongFormatException, "] missing.");
                std::string rewardsStr = line.substr(1, posEndReward - 1);
                STORM_LOG_TRACE("Action rewards: " << rewardsStr);
                std::vector<std::string> rewards;
                boost::split(rewards, rewardsStr, boost::is_any_of(","));
                if (actionRewards.size() < rewards.size()) {
                    actionRewards.resize(rewards.size());
                }
                auto actionRewardsIt = actionRewards.begin();
                for (auto const& rew : rewards) {
                    auto rewardValue = parseValue(rew, placeholders, valueParser);
                    if (!storm::utility::isZero(rewardValue)) {
                        if (actionRewardsIt->size() <= row) {
                            actionRewardsIt->resize(std::max(row + 1, stateSize), storm::utility::zero<ValueType>());
                        }
                        (*actionRewardsIt)[row] = std::move(rewardValue);
                    }
                    ++actionRewardsIt;
                }
                line = line.substr(posEndReward + 1);
            }

        } else {
            // New transition
            size_t posColon = line.find(':');
            STORM_LOG_THROW(posColon != std::string::npos, storm::exceptions::WrongFormatException,
                            "':' not found in '" << line << "' on line " << lineNumber << ".");
            size_t target = parseNumber<size_t>(line.substr(0, posColon - 1));
            std::string valueStr = line.substr(posColon + 2);
            ValueType value = parseValue(valueStr, placeholders, valueParser);
            STORM_LOG_TRACE("Transition " << row << " -> " << target << ": " << value);
            STORM_LOG_THROW(target < stateSize, storm::exceptions::WrongFormatException,
                            "In line " << lineNumber << " target state " << target << " is greater than state size " << stateSize);
            builder.addNextValue(row, target, value);
        }

        if (storm::utility::resources::isTerminate()) {
            std::cout << "Parsed " << state << "/" << stateSize << " states before abort.\n";
            STORM_LOG_THROW(false, storm::exceptions::AbortException, "Aborted in state space exploration.");
            break;
        }

    }  // end state iteration
    STORM_LOG_TRACE("Finished parsing");

    if (nonDeterministic) {
        STORM_LOG_THROW(nrChoices == 0 || builder.getLastRow() + 1 == nrChoices, storm::exceptions::WrongFormatException,
                        "Number of actions detected (at least " << builder.getLastRow() + 1 << ") does not match number of actions declared (" << nrChoices
                                                                << ", in @nr_choices).");
    }

    // Build transition matrix
    modelComponents->transitionMatrix = builder.build(row + 1, stateSize, nonDeterministic ? stateSize : 0);
    STORM_LOG_TRACE("Built matrix");

    // Build reward models
    uint64_t numRewardModels = std::max(stateRewards.size(), actionRewards.size());
    for (uint64_t i = 0; i < numRewardModels; ++i) {
        std::string rewardModelName;
        if (rewardModelNames.size() <= i) {
            rewardModelName = "rew" + std::to_string(i);
        } else {
            rewardModelName = rewardModelNames[i];
        }
        std::optional<std::vector<ValueType>> stateRewardVector, actionRewardVector;
        if (i < stateRewards.size() && !stateRewards[i].empty()) {
            stateRewardVector = std::move(stateRewards[i]);
        }
        if (i < actionRewards.size() && !actionRewards[i].empty()) {
            actionRewards[i].resize(row + 1, storm::utility::zero<ValueType>());
            actionRewardVector = std::move(actionRewards[i]);
        }
        modelComponents->rewardModels.emplace(
            rewardModelName, storm::models::sparse::StandardRewardModel<ValueType>(std::move(stateRewardVector), std::move(actionRewardVector)));
    }
    STORM_LOG_TRACE("Built reward models");
    return modelComponents;
}

template<typename ValueType, typename RewardModelType>
ValueType DirectEncodingParser<ValueType, RewardModelType>::parseValue(std::string const& valueStr,
                                                                       std::unordered_map<std::string, ValueType> const& placeholders,
                                                                       ValueParser<ValueType> const& valueParser) {
    if (boost::starts_with(valueStr, "$")) {
        auto it = placeholders.find(valueStr.substr(1));
        STORM_LOG_THROW(it != placeholders.end(), storm::exceptions::WrongFormatException, "Placeholder " << valueStr << " unknown.");
        return it->second;
    } else {
        // Use default value parser
        return valueParser.parseValue(valueStr);
    }
}

// Template instantiations.
template class DirectEncodingParser<double>;
template class DirectEncodingParser<storm::RationalNumber>;
template class DirectEncodingParser<storm::RationalFunction>;

}  // namespace parser
}  // namespace storm
