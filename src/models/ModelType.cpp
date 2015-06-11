#include "src/models/ModelType.h"

namespace storm {
    namespace models {
        std::ostream& operator<<(std::ostream& os, ModelType const& type) {
            switch (type) {
                case ModelType::Dtmc: os << "DTMC"; break;
                case ModelType::Ctmc: os << "CTMC"; break;
                case ModelType::Mdp: os << "MDP"; break;
                case ModelType::MarkovAutomaton: os << "Markov Automaton"; break;
                case ModelType::S2pg: os << "S2PG"; break;
            }
            return os;
        }
    }
}