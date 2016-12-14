#pragma once

namespace storm {
    namespace storage {
        struct DFTLayoutInfo {
            DFTLayoutInfo() {};
            DFTLayoutInfo(double x, double y) : x(x), y(y) {};
            
            // x location
            double x = 0.0;
            // y location
            double y = 0.0;
        };
    }
}
