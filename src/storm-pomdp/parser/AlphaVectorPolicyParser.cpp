#include "AlphaVectorPolicyParser.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <string>

#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/OutOfRangeException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/io/file.h"

#include "storm-parsers/util/cstring.h"

#include "storm/utility/macros.h"

namespace storm{
namespace pomdp{
namespace parser {
    template<typename ValueType>
    storm::pomdp::storage::AlphaVectorPolicy<ValueType> AlphaVectorPolicyParser<ValueType>::parseAlphaVectorPolicy(std::string const& filename){
        return parse(filename);
    }

    template<typename ValueType>
    storm::pomdp::storage::AlphaVectorPolicy<ValueType> AlphaVectorPolicyParser<ValueType>::parse(const std::string& filename){
        std::vector<std::string> actions;
        storm::storage::SparseMatrixBuilder<ValueType> alphaMatrixBuilder;
        setlocale(LC_NUMERIC, "C");

        // Open file.
        std::ifstream file;
        storm::utility::openFile(filename, file);

        std::string line;

        uint64_t lineNo = 0;
        uint64_t matrixRow = 0;

        uint64_t nrRows = 0;
        uint64_t nrColumns = 0;
        try {
            while (storm::utility::getline(file, line)) {
                ++lineNo;
                if(lineNo == 1){
                    size_t openBracket = line.find('(');
                    std::string cleanedStr;
                    if (openBracket != std::string::npos) {
                        // Remove brackets
                        size_t closingBracket = line.find(')', openBracket + 1);
                        STORM_LOG_THROW(closingBracket != std::string::npos, storm::exceptions::WrongFormatException,
                                        "No closing bracket found in " << line);
                        cleanedStr = line.substr(openBracket + 1, closingBracket - 1);
                    } else {
                        cleanedStr = line;
                    }
                    std::vector<std::string> tokens;
                    boost::split(tokens, cleanedStr, boost::is_any_of(","), boost::token_compress_on);
                    nrColumns = std::stoi(tokens[0]);
                    nrRows = std::stoi(tokens[1]);
                    alphaMatrixBuilder = storm::storage::SparseMatrixBuilder<ValueType>(nrRows, nrColumns);
                } else {
                    boost::trim(line);
                    if (line.empty()) {
                        // Empty line
                        continue;
                    }
                    // Split line into tokens w.r.t. white space
                    std::vector<std::string> tokens;
                    boost::split(tokens, line, boost::is_any_of(";"), boost::token_compress_on);
                    STORM_LOG_ERROR_COND(tokens.size() == 2, "Found too many tokens in line " << lineNo << "\n");
                    size_t openBracket = tokens[0].find('[');
                    std::string actionName;
                    if (openBracket != std::string::npos) {
                        // Remove brackets
                        size_t closingBracket = tokens[0].find(']', openBracket + 1);
                        STORM_LOG_THROW(closingBracket != std::string::npos, storm::exceptions::WrongFormatException,
                                        "No closing bracket found in " << tokens[0]);
                        actionName = tokens[0].substr(openBracket + 1, closingBracket - 1);
                    } else {
                        actionName = tokens[0];
                    }
                    actions.push_back(actionName);

                    openBracket = tokens[1].find('[');
                    std::string sparseList;
                    if (openBracket != std::string::npos) {
                        // Remove brackets
                        size_t closingBracket = tokens[1].find(']', openBracket + 1);
                        STORM_LOG_THROW(closingBracket != std::string::npos, storm::exceptions::WrongFormatException,
                                        "No closing bracket found in " << tokens[1]);
                        sparseList = tokens[1].substr(openBracket + 1, closingBracket - 1);
                    } else {
                        sparseList = tokens[1];
                    }

                    std::vector<std::string> values;
                    boost::split(values, sparseList, boost::is_any_of(","), boost::token_compress_on);
                    for (auto const& token : values) {
                        size_t colon_pos = token.find(':');
                        uint64_t state = std::stoi(token.substr(0, colon_pos));
                        double value = std::stod(token.substr(colon_pos + 1));
                        alphaMatrixBuilder.addNextValue(matrixRow, state, storm::utility::convertNumber<ValueType>(value));
                    }
                    ++matrixRow;
                }
            }
        } catch (storm::exceptions::BaseException const& exception) {
            STORM_LOG_THROW(false, storm::exceptions::FileIoException, "A parsing exception occurred in line " << lineNo << ": " << exception.what());
        }

        storm::utility::closeFile(file);
        auto mat = alphaMatrixBuilder.build();
        return storm::pomdp::storage::AlphaVectorPolicy<ValueType>({mat, actions});
    }
    template class AlphaVectorPolicyParser<double>;
    template class AlphaVectorPolicyParser<storm::RationalNumber>;
}
}
}