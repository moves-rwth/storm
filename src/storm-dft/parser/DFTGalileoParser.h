#pragma  once

#include <map>

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/parser/ExpressionParser.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/storage/dft/DFTBuilder.h"


namespace storm {
    namespace parser {

        template<typename ValueType>
        class DFTGalileoParser {
        public:

            /*!
             * Constructor.
             *
             * @param defaultInclusive Flag indicating if priority gates are inclusive by default.
             * @param binaryDependencies Flag indicating if dependencies should be converted to binary dependencies.
             */
            DFTGalileoParser(bool defaultInclusive = true, bool binaryDependencies = true) : builder(defaultInclusive, binaryDependencies) {
            }

            storm::storage::DFT<ValueType> parseDFT(std::string const& filename);
            
        private:
            void readFile(std::string const& filename);

            std::string stripQuotsFromName(std::string const& name);
            std::string parseNodeIdentifier(std::string const& name);

            storm::storage::DFTBuilder<ValueType> builder;
            bool defaultInclusive;
        };
    }
}
