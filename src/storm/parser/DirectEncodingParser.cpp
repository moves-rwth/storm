#include "storm/parser/DirectEncodingParser.h"

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <clocale>
#include <iostream>
#include <string>

#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/settings/SettingsManager.h"

#include "storm/adapters/CarlAdapter.h"
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
            storm::models::ModelType type;

            // Iterate over all lines
            while (std::getline(file, line)) {
                STORM_LOG_TRACE("Parsing: " << line);
            }

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
                    STORM_LOG_THROW(false, storm::exceptions::FileIoException, "CTMC not supported.");
                }
                case storm::models::ModelType::Mdp:
                {
                    STORM_LOG_THROW(false, storm::exceptions::FileIoException, "MDP not supported.");
                }
                case storm::models::ModelType::MarkovAutomaton:
                {
                    STORM_LOG_THROW(false, storm::exceptions::FileIoException, "MA not supported.");
                }
                default:
                    STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Unknown/Unhandled model type " << type << " which cannot be parsed.");
            }
        }

        template class DirectEncodingParser<double>;

#ifdef STORM_HAVE_CARL
        template class DirectEncodingParser<storm::RationalFunction>;
#endif
    } // namespace parser
} // namespace storm
