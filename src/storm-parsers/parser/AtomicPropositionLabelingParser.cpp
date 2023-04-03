/*!
 * LabParser.cpp
 *
 *  Created on: 21.11.2012
 *      Author: Gereon Kremer
 */

#include "storm-parsers/parser/AtomicPropositionLabelingParser.h"

#include <cstring>
#include <iostream>
#include <string>
#include "storm/utility/macros.h"

#include "storm-parsers/parser/MappedFile.h"
#include "storm-parsers/util/cstring.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/WrongFormatException.h"

namespace storm {
namespace parser {

using namespace storm::utility::cstring;

storm::models::sparse::StateLabeling AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(uint_fast64_t stateCount, std::string const& filename) {
    // Open the given file.
    MappedFile file(filename.c_str());
    char const* buf = file.getData();

    // First pass: Count the number of propositions.
    bool foundDecl = false, foundEnd = false;
    uint_fast32_t proposition_count = 0;
    size_t cnt = 0;

    // Iterate over tokens until we hit #END or the end of the file.
    while (buf[0] != '\0') {
        // Move the buffer to the beginning of the next word.
        buf += cnt;
        buf = trimWhitespaces(buf);

        // Get the number of characters until the next separator.
        cnt = skipWord(buf) - buf;
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
        }
    }

    // If #DECLARATION or #END have not been found, the file format is wrong.
    if (!(foundDecl && foundEnd)) {
        STORM_LOG_ERROR("Error while parsing " << filename << ": File header is corrupted (#DECLARATION or #END missing - case sensitive).");
        if (!foundDecl)
            STORM_LOG_ERROR("\tDid not find #DECLARATION token.");
        if (!foundEnd)
            STORM_LOG_ERROR("\tDid not find #END token.");
        throw storm::exceptions::WrongFormatException()
            << "Error while parsing " << filename << ": File header is corrupted (#DECLARATION or #END missing - case sensitive).";
    }

    // Create labeling object with given node and proposition count.
    storm::models::sparse::StateLabeling labeling(stateCount);

    // Second pass: Add propositions and node labels to labeling.
    // First thing to do: Reset the file pointer.
    buf = file.getData();

    // Prepare a buffer for proposition names.
    char proposition[128];
    cnt = 0;

    // Parse proposition names.
    // As we already checked the file header, we know that #DECLARATION and #END are tokens in the character stream.
    while (buf[0] != '\0') {
        // Move the buffer to the beginning of the next word.
        buf += cnt;
        buf = trimWhitespaces(buf);

        // Get the number of characters until the next separator.
        cnt = skipWord(buf) - buf;

        if (cnt >= sizeof(proposition)) {
            // if token is longer than our buffer, the following strncpy code might get risky...
            STORM_LOG_ERROR("Error while parsing " << filename << ": Atomic proposition with length > " << (sizeof(proposition) - 1) << " was found.");
            throw storm::exceptions::WrongFormatException()
                << "Error while parsing " << filename << ": Atomic proposition with length > " << (sizeof(proposition) - 1) << " was found.";

        } else if (cnt > 0) {
            // If the next token is #DECLARATION: Just skip it.
            if (strncmp(buf, "#DECLARATION", cnt) == 0)
                continue;

            // If the next token is #END: Stop the search.
            if (strncmp(buf, "#END", cnt) == 0)
                break;

            // Otherwise copy the token to the buffer, append a trailing null byte and hand it to labeling.
            strncpy(proposition, buf, cnt);
            proposition[cnt] = '\0';
            labeling.addLabel(proposition);
        }
    }

    // At this point, the pointer buf is still pointing to our last token, i.e. to #END.
    // We want to skip it.
    buf += 4;

    // Now eliminate remaining whitespaces such as empty lines and start parsing.
    buf = trimWhitespaces(buf);

    uint_fast64_t state = 0;
    uint_fast64_t lastState = (uint_fast64_t)-1;
    uint_fast64_t const startIndexComparison = lastState;
    cnt = 0;

    // Now parse the assignments of labels to nodes.
    while (buf[0] != '\0') {
        // Parse the state number and iterate over its labels (atomic propositions).
        // Stop at the end of the line.
        state = checked_strtol(buf, &buf);

        // If the state has already been read or skipped once there might be a problem with the file (doubled lines, or blocks).
        if (state <= lastState && lastState != startIndexComparison) {
            STORM_LOG_ERROR("Error while parsing " << filename << ": State " << state << " was found but has already been read or skipped previously.");
            throw storm::exceptions::WrongFormatException()
                << "Error while parsing " << filename << ": State " << state << " was found but has already been read or skipped previously.";
        }

        while ((buf[0] != '\r') && (buf[0] != '\n') && (buf[0] != '\0')) {
            cnt = skipWord(buf) - buf;
            if (cnt == 0) {
                // The next character is a separator.
                // If it is a line separator, we continue with next node.
                // Otherwise, we skip it and try again.
                if (buf[0] == '\n' || buf[0] == '\r')
                    break;
                buf++;
            } else {
                // Copy the label to the buffer, null terminate it and add it to labeling.
                strncpy(proposition, buf, cnt);
                proposition[cnt] = '\0';

                // Has the label been declared in the header?
                if (!labeling.containsLabel(proposition)) {
                    STORM_LOG_ERROR("Error while parsing " << filename << ": Atomic proposition" << proposition << " was found but not declared.");
                    throw storm::exceptions::WrongFormatException()
                        << "Error while parsing " << filename << ": Atomic proposition" << proposition << " was found but not declared.";
                }
                labeling.addLabelToState(proposition, state);
                buf += cnt;
            }
        }
        buf = trimWhitespaces(buf);
        lastState = state;
    }

    return labeling;
}

}  // namespace parser
}  // namespace storm
