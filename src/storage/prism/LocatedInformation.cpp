#include "src/storage/prism/LocatedInformation.h"

namespace storm {
    namespace prism {
        LocatedInformation::LocatedInformation(std::string const& filename, uint_fast64_t lineNumber) : filename(filename), lineNumber(lineNumber) {
            // Intentionally left empty.
        }

        std::string const& LocatedInformation::getFilename() const {
            return this->filename;
        }
        
        uint_fast64_t LocatedInformation::getLineNumber() const {
            return this->lineNumber;
        }
    }
}