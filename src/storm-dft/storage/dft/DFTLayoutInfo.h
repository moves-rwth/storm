#pragma once

namespace storm {
    namespace storage {
        struct DFTLayoutInfo {
            DFTLayoutInfo() : x(20.0), y(20.0) {
            };
            DFTLayoutInfo(double x, double y) : x(x), y(y) {};
            
            // x location
            double x;
            // y location
            double y;
        };
    }
}
