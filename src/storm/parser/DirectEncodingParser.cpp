#include "storm/parser/DirectEncodingParser.h"

#include <iostream>
#include <string>

#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Ctmc.h"

#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/settings/SettingsManager.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/utility/constants.h"
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

            // Parse header
            std::getline(file, line);
            STORM_LOG_THROW(line == "// Exported by storm", storm::exceptions::WrongFormatException, "Expected header information.");
            std::getline(file, line);
            STORM_LOG_THROW(boost::starts_with(line, "// Original model type: "), storm::exceptions::WrongFormatException, "Expected header information.");
            // Parse model type
            std::getline(file, line);
            STORM_LOG_THROW(boost::starts_with(line, "@type: "), storm::exceptions::WrongFormatException, "Expected model type.");
            storm::models::ModelType type = storm::models::getModelType(line.substr(7));
            STORM_LOG_TRACE("Model type: " << type);
            // Parse parameters
            std::getline(file, line);
            STORM_LOG_THROW(line == "@parameters", storm::exceptions::WrongFormatException, "Expected parameter declaration.");
            std::getline(file, line);
            if (line != "") {
                std::vector<std::string> parameters;
                boost::split(parameters, line, boost::is_any_of(" "));
                for (std::string parameter : parameters) {
                    STORM_LOG_TRACE("New parameter: " << parameter);
                    valueParser.addParameter(parameter);
                }
            }

            // Parse no. states
            std::getline(file, line);
            STORM_LOG_THROW(line == "@nr_states", storm::exceptions::WrongFormatException, "Expected number of states.");
            std::getline(file, line);
            size_t nrStates = boost::lexical_cast<size_t>(line);
            STORM_LOG_TRACE("Model type: " << type);

            std::getline(file, line);
            STORM_LOG_THROW(line == "@model", storm::exceptions::WrongFormatException, "Expected model declaration.");

            // Construct transition matrix
            std::shared_ptr<ModelComponents> modelComponents = parseStates(file, type, nrStates, valueParser);

            // Done parsing
            storm::utility::closeFile(file);

            // Build model
            switch (type) {
                case storm::models::ModelType::Dtmc:
                {
                    STORM_LOG_THROW(false, storm::exceptions::FileIoException, "DTMC not supported.");
                }
                case storm::models::ModelType::Ctmc:
                {
                    return std::make_shared<storm::models::sparse::Ctmc<ValueType, RewardModelType>>(std::move(modelComponents->transitionMatrix), std::move(modelComponents->stateLabeling), std::move(modelComponents->rewardModels), std::move(modelComponents->choiceLabeling));
                }
                case storm::models::ModelType::Mdp:
                {
                    STORM_LOG_THROW(false, storm::exceptions::FileIoException, "MDP not supported.");
                }
                case storm::models::ModelType::MarkovAutomaton:
                {
                    return std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType>>(std::move(modelComponents->transitionMatrix), std::move(modelComponents->stateLabeling), std::move(modelComponents->markovianStates), std::move(modelComponents->exitRates));
                }
                default:
                    STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Unknown/Unhandled model type " << type << " which cannot be parsed.");
            }
        }

        template<typename ValueType, typename RewardModelType>
        std::shared_ptr<typename DirectEncodingParser<ValueType, RewardModelType>::ModelComponents> DirectEncodingParser<ValueType, RewardModelType>::parseStates(std::istream& file, storm::models::ModelType type, size_t stateSize, ValueParser<ValueType> const& valueParser) {
            // Initialize
            std::shared_ptr<ModelComponents> modelComponents = std::make_shared<ModelComponents>();
            modelComponents->nonDeterministic = (type == storm::models::ModelType::Mdp || type == storm::models::ModelType::MarkovAutomaton);
            storm::storage::SparseMatrixBuilder<ValueType> builder = storm::storage::SparseMatrixBuilder<ValueType>(0, 0, 0, false, modelComponents->nonDeterministic, 0);
            modelComponents->stateLabeling = storm::models::sparse::StateLabeling(stateSize);

            // Iterate over all lines
            std::string line;
            size_t row = 0;
            size_t state = 0;
            bool first = true;
            while (std::getline(file, line)) {
                STORM_LOG_TRACE("Parsing: " << line);
                if (boost::starts_with(line, "state ")) {
                    // New state
                    if (first) {
                        first = false;
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
                            std::string rewards = line.substr(1, posEndReward-1);
                            STORM_LOG_TRACE("State rewards: " << rewards);
                            // TODO import rewards
                            STORM_LOG_WARN("Rewards were not imported");
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
                    if (modelComponents->nonDeterministic) {
                        builder.newRowGroup(row);
                    }
                } else if (boost::starts_with(line, "\taction ")) {
                    // New action
                    if (first) {
                        first = false;
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
                    STORM_LOG_TRACE("Transition " << state << " -> " << target << ": " << value);
                    builder.addNextValue(state, target, value);
                }
            }

            STORM_LOG_TRACE("Finished parsing");
            modelComponents->transitionMatrix = builder.build(stateSize, stateSize);
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
