#pragma once

namespace storm {
    template<typename ValueType>
    struct NumberTraits {
        static const bool SupportsExponential = false;
    };
    
    template<>
    struct NumberTraits<double> {
        static const bool SupportsExponential = true;
    };
}
