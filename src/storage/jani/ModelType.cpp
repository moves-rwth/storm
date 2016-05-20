#include "src/storage/jani/ModelType.h"

namespace storm {
    namespace jani {
        
        std::ostream& operator<<(std::ostream& stream, ModelType const& type) {
            switch (type) {
                case ModelType::UNDEFINED:
                    stream << "undefined";
                    break;
                case ModelType::DTMC:
                    stream << "dtmc";
                    break;
                case ModelType::CTMC:
                    stream << "ctmc";
                    break;
                case ModelType::MDP:
                    stream << "mdp";
                    break;
                case ModelType::MA:
                    stream << "ma";
                    break;
            }
            return stream;
        }
        
    }
}
