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
                case ModelType::PTA:
                    stream << "pta";
                    break;
                case ModelType::STA:
                    stream << "sta";
                    break;
            }
            return stream;
        }

        ModelType getModelType(std::string const& input) {
            if(input == "dtmc") {
                return ModelType::DTMC;
            }
            if(input == "ctmc") {
                return ModelType::CTMC;
            }
            if(input == "mdp") {
                return ModelType::MDP;
            }
            if(input == "ma") {
                return ModelType::MA;
            }
            if(input == "pta") {
                return ModelType::PTA;
            }
            if(input == "sta") {
                return ModelType::STA;
            }
            return ModelType::UNDEFINED;
        }
        
    }
}
