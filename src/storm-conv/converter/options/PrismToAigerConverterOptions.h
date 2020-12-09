#pragma once

#include <string>

namespace storm {
    namespace converter {
        

        struct PrismToAigerConverterOptions {
            
            PrismToAigerConverterOptions();
            
            std::string suffix;
        };
    }
}

