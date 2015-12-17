#ifndef DFTGALILEOPARSER_H
#define	DFTGALILEOPARSER_H

#include "../storage/dft/DFT.h"
#include "../storage/dft/DFTBuilder.h"

#include <map>

namespace storm {
    namespace parser {

        template<typename ValueType>
        class DFTGalileoParser {
            storm::storage::DFTBuilder<ValueType> mBuilder;
        public:
            storm::storage::DFT parseDFT(std::string const& filename);
            
        private:
            bool readFile(std::string const& filename);

            std::string stripQuotsFromName(std::string const& name);
        };
}
}

#endif	/* DFTGALILEOPARSER_H */

