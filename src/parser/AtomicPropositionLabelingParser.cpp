/*!
 * LabParser.cpp
 *
 *  Created on: 21.11.2012
 *      Author: Gereon Kremer
 */

#include "src/parser/AtomicPropositionLabelingParser.h"
#include "src/parser/Parser.h"

#include <cstring>
#include <string>
#include <iostream>

#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/FileIoException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
	namespace parser {

		storm::models::AtomicPropositionsLabeling AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(uint_fast64_t node_count, std::string const & filename) {

			// Open the given file.
			if (!storm::parser::fileExistsAndIsReadable(filename.c_str())) {
				LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
				throw storm::exceptions::FileIoException() << "The supplied Labeling input file \"" << filename << "\" does not exist or is not readable by this process.";
			}

			// Find out about the used line endings.
			SupportedLineEndingsEnum lineEndings = findUsedLineEndings(filename, true);

			MappedFile file(filename.c_str());
			char* buf = file.data;

			// First pass: Count the number of propositions.
			// Convert the line endings into a string containing all whitespace characters, that separate words in the file.
			char separator[5];
			storm::parser::getMatchingSeparatorString(separator, sizeof(separator), lineEndings);

			bool foundDecl = false, foundEnd = false;
			uint_fast32_t proposition_count = 0;
			size_t cnt = 0;

			// Iterate over tokens until we hit #END or the end of the file.
			while(buf[0] != '\0') {

				// Move the buffer pointer to the separator.
				buf += cnt;

				// Get the number of characters until the next separator.
				cnt = strcspn(buf, separator);
				if (cnt > 0) {

					// If the next token is #DECLARATION: Just skip it.
					// If the next token is #END: Stop the search.
					// Otherwise increase proposition_count.
					if (strncmp(buf, "#DECLARATION", cnt) == 0) {
						foundDecl = true;
						continue;
					} else if (strncmp(buf, "#END", cnt) == 0) {
						foundEnd = true;
						break;
					}
					proposition_count++;
				} else {

					// If the next character is a separator, skip it.
					buf++;
				}
			}

			// If #DECLARATION or #END have not been found, the file format is wrong.
			if (!(foundDecl && foundEnd)) {
				LOG4CPLUS_ERROR(logger, "Wrong file format in (" << filename << "). File header is corrupted.");
				if (!foundDecl) LOG4CPLUS_ERROR(logger, "\tDid not find #DECLARATION token.");
				if (!foundEnd) LOG4CPLUS_ERROR(logger, "\tDid not find #END token.");
				throw storm::exceptions::WrongFormatException();
			}


			// Create labeling object with given node and proposition count.
			storm::models::AtomicPropositionsLabeling labeling(node_count, proposition_count);

			// Second pass: Add propositions and node labels to labeling.
			// First thing to do: Reset the file pointer.
			buf = file.data;

			// Prepare a buffer for proposition names.
			char proposition[128];
			cnt = 0;

			// Parse proposition names.
			// As we already checked the file header, we know that #DECLARATION and #END are tokens in the character stream.
			while(buf[0] != '\0') {

				// Move the buffer pointer to the separator.
				buf += cnt;

				// The number of characters until the next separator.
				cnt = strcspn(buf, separator);

				if (cnt >= sizeof(proposition)) {

					// if token is longer than our buffer, the following strncpy code might get risky...
					LOG4CPLUS_ERROR(logger, "Wrong file format in (" << filename << "). Atomic proposition with length > " << (sizeof(proposition)-1) << " was found.");
					throw storm::exceptions::WrongFormatException();

				} else if (cnt > 0) {

					// If the next token is #DECLARATION: Just skip it.
					if (strncmp(buf, "#DECLARATION", cnt) == 0) continue;

					// If the next token is #END: Stop the search.
					if (strncmp(buf, "#END", cnt) == 0) break;

					// Otherwise copy the token to the buffer, append a trailing null byte and hand it to labeling.
					strncpy(proposition, buf, cnt);
					proposition[cnt] = '\0';
					labeling.addAtomicProposition(proposition);
				} else {

					// The next character is a separator, thus move one step forward.
					buf++;
				}
			}

			// At this point, the pointer buf is still pointing to our last token, i.e. to #END.
			// We want to skip it.
			buf += 4;

			uint_fast64_t node;
			cnt = 0;

			// Now parse the assignments of labels to nodes.
			while (buf[0] != '\0') {

				// Parse the node number and iterate over its labels (atomic propositions).
				// Stop at the end of the line.
				node = checked_strtol(buf, &buf);
				while ((buf[0] != '\r') && (buf[0] != '\n') && (buf[0] != '\0')) {
					cnt = strcspn(buf, separator);
					if (cnt == 0) {

						// The next character is a separator.
						// If it is a line separator, we continue with next node.
						// Otherwise, we skip it and try again.
						if (buf[0] == '\n' || buf[0] == '\r') break;
						buf++;
					} else {

						// Copy the label to the buffer, null terminate it and add it to labeling.
						strncpy(proposition, buf, cnt);
						proposition[cnt] = '\0';
						labeling.addAtomicPropositionToState(proposition, node);
						buf += cnt;
					}
				}
				buf = trimWhitespaces(buf);
			}

			return labeling;
		}

	}  // namespace parser
}  // namespace storm
