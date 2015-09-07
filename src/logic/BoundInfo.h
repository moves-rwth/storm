
#ifndef BOUNDINFO_H
#define	BOUNDINFO_H

#include "ComparisonType.h"


namespace storm {
    namespace logic {
        template<typename BT> 
        struct BoundInfo {
            BT bound;
            ComparisonType boundType;
        };
    }
}

#endif	/* BOUNDINFO_H */

