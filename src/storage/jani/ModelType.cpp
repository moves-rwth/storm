#include "src/storage/jani/ModelType.h"

namespace storm {
    namespace jani {
        
        std::ostream& operator<<(std::ostream& stream, ModelType const& type) {
            switch (type) {
                case ModelType::UNDEFINED:
                    stream << "undefined";
                    break;
                case ModelType::LTS:
                    stream << "lts";
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
                case ModelType::CTMDP:
                    stream << "ctmdp";
                    break;
                case ModelType::MA:
                    stream << "ma";
                    break;
                case ModelType::TA:
                    stream << "ta";
                    break;
                case ModelType::PTA:
                    stream << "pta";
                    break;
                case ModelType::STA:
                    stream << "sta";
                    break;
                case ModelType::HA:
                    stream << "ha";
                    break;
                case ModelType::PHA:
                    stream << "pha";
                    break;
                case ModelType::SHA:
                    stream << "sha";
                    break;
            }
            return stream;
        }

        ModelType getModelType(std::string const& input) {
            if (input == "lts") {
                return ModelType::LTS;
            }
            if (input == "dtmc") {
                return ModelType::DTMC;
            }
            if (input == "ctmc") {
                return ModelType::CTMC;
            }
            if (input == "mdp") {
                return ModelType::MDP;
            }
            if (input == "ctmdp") {
                return ModelType::CTMDP;
            }
            if (input == "ma") {
                return ModelType::MA;
            }
            if (input == "ta") {
                return ModelType::TA;
            }
            if (input == "pta") {
                return ModelType::PTA;
            }
            if (input == "sta") {
                return ModelType::STA;
            }
            if (input == "ha") {
                return ModelType::HA;
            }
            if (input == "pha") {
                return ModelType::PHA;
            }
            if (input == "sha") {
                return ModelType::SHA;
            }
            return ModelType::UNDEFINED;
        }
        
    }
}
