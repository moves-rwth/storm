#pragma once

#include "storm/automata/DeterministicAutomaton.h"
#include "storm/logic/ExtractMaximalStateFormulasVisitor.h"
#include "storm/logic/Formula.h"
#include "storm/logic/MultiObjectiveFormula.h"
#include "storm/storage/BitVector.h"

namespace storm::modelchecker::helper::lexicographic::spothelper {
typedef std::function<storm::storage::BitVector(storm::logic::Formula const&)> CheckFormulaCallback;

/**
 * Function that creates a determinitistic automaton with Streett-acceptance condition. That is done based on a multi-objective formula.
 * For each subformula, a new automaton is created and directly merged into a big product-automaton.
 * Spot is used as tool for this, however, currently it has to be an adapted version of Spot.
 * @tparam SparseModelType the model-type that will be used
 * @tparam ValueType the value (either double or Rational Number)
 * @param formula the multi-objective formula
 * @param formulaChecker
 * @param model the original model
 * @param extracted extracted atomic propositions (is empty in the beginning, and will be filled in the function)
 * @param acceptanceConditions indication which formula has which streett pairs as acceptance condition
 * @return
 */
template<typename SparseModelType, typename ValueType>
std::shared_ptr<storm::automata::DeterministicAutomaton> ltl2daSpotProduct(storm::logic::MultiObjectiveFormula const& formula,
                                                                           CheckFormulaCallback const& formulaChecker, SparseModelType const& model,
                                                                           storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap& extracted,
                                                                           std::vector<uint>& acceptanceConditions);
}  // namespace storm::modelchecker::helper::lexicographic::spothelper
