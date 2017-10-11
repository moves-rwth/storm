#include "storm/parser/DirectEncodingParser.h"

#include <iostream>
#include <string>
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

        template<typename ValueType>
        void ValueParser<ValueType>::addParameter(std::string const& parameter) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameters are not supported in this build.");
        }

        template<>
        void ValueParser<storm::RationalFunction>::addParameter(std::string const& parameter) {
            //STORM_LOG_THROW((std::is_same<ValueType, storm::RationalFunction>::value), storm::exceptions::NotSupportedException, "Parameters only allowed when using rational functions.");
            storm::expressions::Variable var = manager->declareRationalVariable(parameter);
            identifierMapping.emplace(var.getName(), var);
            parser.setIdentifierMapping(identifierMapping);
            STORM_LOG_TRACE("Added parameter: " << var.getName());
        }

        template<>
        double ValueParser<double>::parseValue(std::string const& value) const {
            return boost::lexical_cast<double>(value);
        }

        template<>
        storm::RationalFunction ValueParser<storm::RationalFunction>::parseValue(std::string const& value) const {
            storm::RationalFunction rationalFunction = evaluator.asRational(parser.parseFromString(value));
            STORM_LOG_TRACE("Parsed expression: " << rationalFunction);
            return rationalFunction;
        }

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

             std::shared_ptr<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>> modelComponents;

            // Parse header
            while(std::getline(file, line)) {
                if(line.empty() || boost::starts_with(line, "//")) {
                    continue;
                }

                if (boost::starts_with(line, "@type: ")) {
                    STORM_LOG_THROW(!sawType, storm::exceptions::WrongFormatException, "Type declared twice");
                    type = storm::models::getModelType(line.substr(7));
                    STORM_LOG_TRACE("Model type: " << type);
                    STORM_LOG_THROW(type != storm::models::ModelType::MarkovAutomaton, storm::exceptions::NotSupportedException, "Markov Automata in DRN format are not supported (unclear indication of Markovian Choices in DRN format)");
                    STORM_LOG_THROW(type != storm::models::ModelType::S2pg, storm::exceptions::NotSupportedException, "Stochastic Two Player Games in DRN format are not supported.");
                    sawType = true;
                }
                if(line == "@parameters") {
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
                }
                if(line == "@nr_states") {
                    STORM_LOG_THROW(nrStates == 0, storm::exceptions::WrongFormatException, "Number states declared twice");
                    std::getline(file, line);
                    nrStates = boost::lexical_cast<size_t>(line);

                }
                if(line == "@model") {
                    STORM_LOG_THROW(sawType, storm::exceptions::WrongFormatException, "Type has to be declared before model.");
                    STORM_LOG_THROW(sawParameters, storm::exceptions::WrongFormatException, "Parameters have to be declared before model.");
                    STORM_LOG_THROW(nrStates != 0, storm::exceptions::WrongFormatException, "Nr States has to be declared before model.");

                    // Construct model components
                    modelComponents = parseStates(file, type, nrStates, valueParser);
                    break;
                }
            }
            // Done parsing
            storm::utility::closeFile(file);

            // Build model
            return storm::utility::builder::buildModelFromComponents(type, std::move(*modelComponents));
        }

        template<typename ValueType, typename RewardModelType>
        std::shared_ptr<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>> DirectEncodingParser<ValueType, RewardModelType>::parseStates(std::istream& file, storm::models::ModelType type, size_t stateSize, ValueParser<ValueType> const& valueParser) {
            // Initialize
            auto modelComponents = std::make_shared<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>>();
            bool nonDeterministic = (type == storm::models::ModelType::Mdp || type == storm::models::ModelType::MarkovAutomaton);
            storm::storage::SparseMatrixBuilder<ValueType> builder = storm::storage::SparseMatrixBuilder<ValueType>(0, 0, 0, false, nonDeterministic, 0);
            modelComponents->stateLabeling = storm::models::sparse::StateLabeling(stateSize);
            std::vector<std::vector<ValueType>> stateRewards;
            
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
                    line = line.substr(6);
                    size_t parsedId;
                    size_t posId = line.find(" ");
                    if (posId != std::string::npos) {
                        parsedId = boost::lexical_cast<size_t>(line.substr(0, posId));

                        // Parse rewards and labels
                        line = line.substr(posId+1);
                        // Check for rewards
                        if (boost::starts_with(line, "[")) {
                            // Rewards found
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
                        // Check for labels
                        std::vector<std::string> labels;
                        boost::split(labels, line, boost::is_any_of(" "));
                        for (std::string label : labels) {
                            if (!modelComponents->stateLabeling.containsLabel(label)) {
                                modelComponents->stateLabeling.addLabel(label);
                            }
                            modelComponents->stateLabeling.addLabelToState(label, state);
                            STORM_LOG_TRACE("New label: " << label);
                        }
                    } else {
                        // Only state id given
                        parsedId = boost::lexical_cast<size_t>(line);
                    }
                    STORM_LOG_TRACE("New state " << state);
                    STORM_LOG_ASSERT(state == parsedId, "State ids do not correspond.");
                    if (nonDeterministic) {
                        STORM_LOG_TRACE("new Row Group starts at " << row << ".");
                        builder.newRowGroup(row);
                    }
                } else if (boost::starts_with(line, "\taction ")) {
                    // New action
                    if (firstAction) {
                        firstAction = false;
                    } else {
                        ++row;
                    }
                    line = line.substr(8);
                    STORM_LOG_TRACE("New action: " << row);
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
                    size_t target = boost::lexical_cast<size_t>(line.substr(2, posColon-3));
                    std::string valueStr = line.substr(posColon+2);
                    ValueType value = valueParser.parseValue(valueStr);
                    STORM_LOG_TRACE("Transition " << row << " -> " << target << ": " << value);
                    builder.addNextValue(row, target, value);
                }
            }

            STORM_LOG_TRACE("Finished parsing");
            modelComponents->transitionMatrix = builder.build(row + 1, stateSize, nonDeterministic ? stateSize : 0);
            for (uint64_t i = 0; i < stateRewards.size(); ++i) {
                modelComponents->rewardModels.emplace("rew" + std::to_string(i), storm::models::sparse::StandardRewardModel<ValueType>(std::move(stateRewards[i])));
            }
            STORM_LOG_TRACE("Built matrix");
            return modelComponents;
        }

        // Template instantiations.
        template class DirectEncodingParser<double>;

#ifdef STORM_HAVE_CARL
        template class DirectEncodingParser<storm::RationalFunction>;
#endif
    } // namespace parser
} // namespace storm
