#ifndef DFTGALILEOPARSER_H
#define	DFTGALILEOPARSER_H

#include "../storage/dft/DFT.h"
#include "../storage/dft/DFTBuilder.h"

#include <map>

namespace storm {
    namespace parser {
        class DFTGalileoParser {
            storm::storage::DFTBuilder mBuilder;
        public:
            storm::storage::DFT parseDFT(std::string const& filename);
            
        private:
            bool readFile(std::string const& filename);
            
    
    };
}
}

#endif	/* DFTGALILEOPARSER_H */

