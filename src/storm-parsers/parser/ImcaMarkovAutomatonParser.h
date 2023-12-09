#pragma once

#include "storm/models/sparse/MarkovAutomaton.h"

namespace storm {
namespace parser {

template<typename ValueType = double>
class ImcaMarkovAutomatonParser {
   public:
    /*!
     * Parses the given file under the assumption that it contains a Markov automaton specified in the imca format.
     *
     * @param filename The name of the file to parse.
     * @return The obtained Markov automaton
     */
    static std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> parseImcaFile(std::string const& filename);
};

}  // namespace parser
}  // namespace storm
