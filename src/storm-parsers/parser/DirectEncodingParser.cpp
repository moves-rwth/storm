#include "storm-parsers/parser/DirectEncodingParser.h"

#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Ctmc.h"

#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/settings/SettingsManager.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/utility/constants.h"
#include "storm/utility/builder.h"
#include "storm/utility/macros.h"
#include "storm/utility/file.h"


namespace storm {
    namespace parser {

        template<typename ValueType, typename RewardModelType>
        std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> DirectEncodingParser<ValueType, RewardModelType>::parseModel(std::string const& filename) {

            // Load file
            STORM_LOG_INFO("Reading from file " << filename);
            std::ifstream file;
            storm::utility::openFile(filename, file);
            std::string line;

            // Initialize
            ValueParser<ValueType> valueParser;
            bool sawType = false;
            bool sawParameters = false;
            size_t nrStates = 0;
            storm::models::ModelType type;
            std::vector<std::string> rewardModelNames;
            std::shared_ptr<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>> modelComponents;

            // Parse header
            while (std::getline(file, line)) {
                if (line.empty() || boost::starts_with(line, "//")) {
                    continue;
                }

                if (boost::starts_with(line, "@type: ")) {
                    // Parse type
                    STORM_LOG_THROW(!sawType, storm::exceptions::WrongFormatException, "Type declared twice");
                    type = storm::models::getModelType(line.substr(7));
                    STORM_LOG_TRACE("Model type: " << type);
                    STORM_LOG_THROW(type != storm::models::ModelType::S2pg, storm::exceptions::NotSupportedException, "Stochastic Two Player Games in DRN format are not supported.");
                    sawType = true;

                } else if (line == "@parameters") {
                    // Parse parameters
                    STORM_LOG_THROW(!sawParameters, storm::exceptions::WrongFormatException, "Parameters declared twice");
                    std::getline(file, line);
                    if (line != "") {
                        std::vector<std::string> parameters;
                        boost::split(parameters, line, boost::is_any_of(" "));
                        for (std::string parameter : parameters) {
                            STORM_LOG_TRACE("New parameter: " << parameter);
                            valueParser.addParameter(parameter);
                        }
                    }
                    sawParameters = true;

                } else if (line == "@reward_models") {
                    // Parse reward models
                    STORM_LOG_THROW(rewardModelNames.size() == 0, storm::exceptions::WrongFormatException, "Reward model names declared twice");
                    std::getline(file, line);
                    boost::split(rewardModelNames, line, boost::is_any_of("\t "));
                } else if (line == "@nr_states") {
                    // Parse no. of states
                    STORM_LOG_THROW(nrStates == 0, storm::exceptions::WrongFormatException, "Number states declared twice");
                    std::getline(file, line);
                    nrStates = NumberParser<size_t>::parse(line);
                } else if (line == "@model") {
                    // Parse rest of the model
                    STORM_LOG_THROW(sawType, storm::exceptions::WrongFormatException, "Type has to be declared before model.");
                    STORM_LOG_THROW(sawParameters, storm::exceptions::WrongFormatException, "Parameters have to be declared before model.");
                    STORM_LOG_THROW(nrStates != 0, storm::exceptions::WrongFormatException, "No. of states has to be declared before model.");

                    // Construct model components
                    modelComponents = parseStates(file, type, nrStates, valueParser, rewardModelNames);
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
        std::shared_ptr<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>> DirectEncodingParser<ValueType, RewardModelType>::parseStates(std::istream& file, storm::models::ModelType type, size_t stateSize, ValueParser<ValueType> const& valueParser, std::vector<std::string> const& rewardModelNames) {
            // Initialize
            auto modelComponents = std::make_shared<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>>();
            bool nonDeterministic = (type == storm::models::ModelType::Mdp || type == storm::models::ModelType::MarkovAutomaton);
            bool continousTime = (type == storm::models::ModelType::Ctmc || type == storm::models::ModelType::MarkovAutomaton);
            storm::storage::SparseMatrixBuilder<ValueType> builder = storm::storage::SparseMatrixBuilder<ValueType>(0, 0, 0, false, nonDeterministic, 0);
            modelComponents->stateLabeling = storm::models::sparse::StateLabeling(stateSize);
            std::vector<std::vector<ValueType>> stateRewards;
            if (continousTime) {
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
            bool firstState = true;
            bool firstAction = true;
            while (std::getline(file, line)) {
                STORM_LOG_TRACE("Parsing: " << line);
                if (boost::starts_with(line, "state ")) {
                    // New state
                    if (firstState) {
                        firstState = false;
                    } else {
                        ++state;
                    }
                    STORM_LOG_TRACE("New state " << state);

                    // Parse state id
                    line = line.substr(6); // Remove "state "
                    std::string curString = line;
                    size_t posEnd = line.find(" ");
                    if (posEnd != std::string::npos) {
                        curString = line.substr(0, posEnd);
                        line = line.substr(posEnd+1);
                    } else {
                        line = "";
                    }
                    size_t parsedId = NumberParser<size_t>::parse(curString);
                    STORM_LOG_ASSERT(state == parsedId, "State ids do not correspond.");
                    if (nonDeterministic) {
                        STORM_LOG_TRACE("new Row Group starts at " << row << ".");
                        builder.newRowGroup(row);
                    }

                    if (type == storm::models::ModelType::Ctmc || type == storm::models::ModelType::MarkovAutomaton) {
                        // Parse exit rate for CTMC or MA
                        STORM_LOG_THROW(boost::starts_with(line, "!"), storm::exceptions::WrongFormatException, "Exit rate missing.");
                        line = line.substr(1); //Remove "!"
                        curString = line;
                        posEnd = line.find(" ");
                        if (posEnd != std::string::npos) {
                            curString = line.substr(0, posEnd);
                            line = line.substr(posEnd+1);
                        } else {
                            line = "";
                        }
                        ValueType exitRate = valueParser.parseValue(curString);
                        if (type == storm::models::ModelType::MarkovAutomaton && !storm::utility::isZero<ValueType>(exitRate)) {
                            modelComponents->markovianStates.get().set(state);
                        }
                        STORM_LOG_TRACE("Exit rate " << exitRate);
                        modelComponents->exitRates.get()[state] = exitRate;
                    }

                    if (boost::starts_with(line, "[")) {
                        // Parse rewards
                        size_t posEndReward = line.find(']');
                        STORM_LOG_THROW(posEndReward != std::string::npos, storm::exceptions::WrongFormatException, "] missing.");
                        std::string rewardsStr = line.substr(1, posEndReward-1);
                        STORM_LOG_TRACE("State rewards: " << rewardsStr);
                        std::vector<std::string> rewards;
                        boost::split(rewards, rewardsStr, boost::is_any_of(","));
                        if (stateRewards.size() < rewards.size()) {
                            stateRewards.resize(rewards.size(), std::vector<ValueType>(stateSize, storm::utility::zero<ValueType>()));
                        }
                        auto stateRewardsIt = stateRewards.begin();
                        for (auto const& rew : rewards) {
                            (*stateRewardsIt)[state] = valueParser.parseValue(rew);
                            ++stateRewardsIt;
                        }
                        line = line.substr(posEndReward+1);
                    }

                    // Parse labels
                    if (!line.empty()) {
                        std::vector<std::string> labels;
                        boost::split(labels, line, boost::is_any_of(" "));
                        for (std::string label : labels) {
                            if (!modelComponents->stateLabeling.containsLabel(label)) {
                                modelComponents->stateLabeling.addLabel(label);
                            }
                            modelComponents->stateLabeling.addLabelToState(label, state);
                            STORM_LOG_TRACE("New label: '" << label << "'");
                        }
                    }

                } else if (boost::starts_with(line, "\taction ")) {
                    // New action
                    if (firstAction) {
                        firstAction = false;
                    } else {
                        ++row;
                    }
                    STORM_LOG_TRACE("New action: " << row);
                    line = line.substr(8); //Remove "\taction "
                    // Check for rewards
                    if (boost::starts_with(line, "[")) {
                        // Rewards found
                        size_t posEndReward = line.find(']');
                        STORM_LOG_THROW(posEndReward != std::string::npos, storm::exceptions::WrongFormatException, "] missing.");
                        std::string rewards = line.substr(1, posEndReward-1);
                        STORM_LOG_TRACE("Transition rewards: " << rewards);
                        STORM_LOG_WARN("Transition rewards [" << rewards << "] not parsed.");
                        // TODO save rewards
                        line = line.substr(posEndReward+1);
                    }
                    // TODO import choice labeling when the export works

                } else {
                    // New transition
                    size_t posColon = line.find(":");
                    STORM_LOG_ASSERT(posColon != std::string::npos, "':' not found.");
                    size_t target = NumberParser<size_t>::parse(line.substr(2, posColon-3));
                    std::string valueStr = line.substr(posColon+2);
                    ValueType value = valueParser.parseValue(valueStr);
                    STORM_LOG_TRACE("Transition " << row << " -> " << target << ": " << value);
                    builder.addNextValue(row, target, value);
                }
            }

            STORM_LOG_TRACE("Finished parsing");
            modelComponents->transitionMatrix = builder.build(row + 1, stateSize, nonDeterministic ? stateSize : 0);

            for (uint64_t i = 0; i < stateRewards.size(); ++i) {
                std::string rewardModelName;
                if (rewardModelNames.size() <= i) {
                    rewardModelName = "rew" + std::to_string(i);
                } else {
                    rewardModelName = rewardModelNames[i];
                }
                modelComponents->rewardModels.emplace(rewardModelName, storm::models::sparse::StandardRewardModel<ValueType>(std::move(stateRewards[i])));
            }
            STORM_LOG_TRACE("Built matrix");
            return modelComponents;
        }

        // Template instantiations.
        template class DirectEncodingParser<double>;
        template class DirectEncodingParser<storm::RationalFunction>;

    } // namespace parser
} // namespace storm
