#include "src/storage/dd/Add.h"

namespace storm {
    namespace dd {
        
        template class Add<storm::dd::DdType::CUDD, double>;
    }
}