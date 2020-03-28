#include "SFTBDDChecker.h"

namespace storm {
    namespace modelchecker {

        template<storm::dd::DdType Type>
        SFTBDDChecker<Type>::SFTBDDChecker() : ddManager{std::make_shared<storm::dd::DdManager<Type>>()} {
        }

    }
}
