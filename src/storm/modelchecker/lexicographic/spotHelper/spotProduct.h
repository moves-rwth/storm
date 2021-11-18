//
// Created by steffi on 18.11.21.
//

#pragma once

#include "storm/automata/DeterministicAutomaton.h"
#include "storm/storage/BitVector.h"
#include "storm/logic/Formula.h"
#include "storm/logic/ExtractMaximalStateFormulasVisitor.h"
#include "storm/logic/MultiObjectiveFormula.h"

namespace storm {
    namespace spothelper {
        typedef std::function<storm::storage::BitVector(storm::logic::Formula const&)> CheckFormulaCallback;

        template<typename SparseModelType, typename ValueType>
        std::shared_ptr<storm::automata::DeterministicAutomaton> ltl2daSpotProduct(storm::logic::MultiObjectiveFormula const& formula, CheckFormulaCallback const& formulaChecker, SparseModelType const& model, storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap& extracted, std::vector<std::shared_ptr<storm::automata::AcceptanceCondition>>& acceptanceConditions);
    }
}