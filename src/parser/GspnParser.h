#ifndef STORM_GSPNPARSER_H
#define STORM_GSPNPARSER_H

#include <string>
#include "src/storage/gspn/GSPN.h"

namespace storm {
    namespace parser {
        // Parses a GSPN in xml format
        class GspnParser {
        public:
            /*!
             * Parses the given file into the GSPN storage class assuming it complies with the PNML.
             *
             * @param filename The name of the file to parse
             * @return The resulting GSPN.
             */
            static storm::gspn::GSPN parse(std::string const& filename);
        private:
        };
    }
}

#endif //STORM_GSPNPARSER_H
