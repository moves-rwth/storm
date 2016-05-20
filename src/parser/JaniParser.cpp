#include "JaniParser.h"
#include "src/storage/jani/Model.h"
#include "src/exceptions/FileIoException.h"

#include <iostream>
#include <fstream>

namespace storm {
    namespace parser {
        void JaniParser::readFile(std::string const &path) {
            std::ifstream file;
            file.exceptions ( std::ifstream::failbit );
            try {
                file.open(path);
            }
            catch (std::ifstream::failure e) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Exception during file opening on " << path << ".");
                return;
            }
            file.exceptions( std::ifstream::goodbit );

            parsedStructure << file;
            file.close();

        }
    }
}