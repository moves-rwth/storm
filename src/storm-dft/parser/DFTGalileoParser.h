#pragma  once

#include <map>

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/parser/ExpressionParser.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/storage/dft/DFTBuilder.h"


namespace storm {
    namespace parser {

        /*!
         * Parser for DFT in the Galileo format.
         */
        template<typename ValueType>
        class DFTGalileoParser {
        public:

            /*!
             * Parse DFT in Galileo format and build DFT.
             *
             * @param filename File.
             * @param defaultInclusive Flag indicating if priority gates are inclusive by default.
             * @param binaryDependencies Flag indicating if dependencies should be converted to binary dependencies.
             *
             * @return DFT.
             */
            static storm::storage::DFT<ValueType> parseDFT(std::string const& filename, bool defaultInclusive = true, bool binaryDependencies = true);
            
        private:
            static std::string parseName(std::string const& name);
        };
    }
}
