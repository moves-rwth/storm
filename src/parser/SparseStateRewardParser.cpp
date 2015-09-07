#include <iostream>
#include "src/parser/SparseStateRewardParser.h"

#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/FileIoException.h"
#include "src/utility/cstring.h"
#include "src/parser/MappedFile.h"

#include "src/adapters/CarlAdapter.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
    namespace parser {

        using namespace storm::utility::cstring;

        template<typename ValueType>
        std::vector<ValueType> SparseStateRewardParser<ValueType>::parseSparseStateReward(uint_fast64_t stateCount, std::string const& filename) {
            // Open file.
            if (!MappedFile::fileExistsAndIsReadable(filename.c_str())) {
                LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
                throw storm::exceptions::FileIoException() << "Error while parsing " << filename << ": File does not exist or is not readable.";
            }

            MappedFile file(filename.c_str());
            char const* buf = file.getData();

            // Create state reward vector with given state count.
            std::vector<ValueType> stateRewards(stateCount);

            // Now parse state reward assignments.
            uint_fast64_t state = 0;
            uint_fast64_t lastState = (uint_fast64_t) - 1;
            uint_fast64_t const startIndexComparison = lastState;
            double reward;

            // Iterate over states.
            while (buf[0] != '\0') {

                // Parse state.
                state = checked_strtol(buf, &buf);

                // If the state has already been read or skipped once there might be a problem with the file (doubled lines, or blocks).
                // Note: The value -1 shows that lastState has not yet been set, i.e. this is the first run of the loop (state index (2^64)-1 is a really bad starting index).
                if (state <= lastState && lastState != startIndexComparison) {
                    LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": State " << state << " was found but has already been read or skipped previously.");
                    throw storm::exceptions::WrongFormatException() << "Error while parsing " << filename << ": State " << state << " was found but has already been read or skipped previously.";
                }

                if (stateCount <= state) {
                    LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": Found reward for a state of an invalid index \"" << state << "\". The model has only " << stateCount << " states.");
                    throw storm::exceptions::OutOfRangeException() << "Error while parsing " << filename << ": Found reward for a state of an invalid index \"" << state << "\"";
                }

                // Parse reward value.
                reward = checked_strtod(buf, &buf);

                if (reward < 0.0) {
                    LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": Expected positive reward value but got \"" << reward << "\".");
                    throw storm::exceptions::WrongFormatException() << "Error while parsing " << filename << ": State reward file specifies illegal reward value.";
                }

                stateRewards[state] = reward;

                buf = trimWhitespaces(buf);
                lastState = state;
            }
            return stateRewards;
        }
        
        template class SparseStateRewardParser<double>;

#ifdef STORM_HAVE_CARL
        template class SparseStateRewardParser<storm::Interval>;
#endif

    } // namespace parser
} // namespace storm
