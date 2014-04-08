#ifndef STORM_STORAGE_PRISM_LOCATEDINFORMATION_H_
#define STORM_STORAGE_PRISM_LOCATEDINFORMATION_H_

#include <string>

namespace storm {
    namespace prism {
        class LocatedInformation {
        public:
            /*!
             * Constructs a located information with the given filename and line number.
             *
             * @param filename The file in which the information was found.
             * @param lineNumber The line number in which the information was found.
             */
            LocatedInformation(std::string const& filename, uint_fast64_t lineNumber);
            
            // Create default implementations of constructors/assignment.
            LocatedInformation(LocatedInformation const& other) = default;
            LocatedInformation& operator=(LocatedInformation const& other)= default;
            LocatedInformation(LocatedInformation&& other) = default;
            LocatedInformation& operator=(LocatedInformation&& other) = default;
            
            /*!
             * Retrieves the name of the file in which the information was found.
             *
             * @return The name of the file in which the information was found.
             */
            std::string const& getFilename() const;
            
            /*!
             * Retrieves the line number in which the information was found.
             *
             * @return The line number in which the information was found.
             */
            uint_fast64_t getLineNumber() const;

        private:
            // The file in which the piece of information was found.
            std::string filename;
            
            // The line in the file in which the piece of information was found.
            uint_fast64_t lineNumber;
        };
    }
}

#endif /* STORM_STORAGE_PRISM_LOCATEDINFORMATION_H_ */