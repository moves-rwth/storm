//
// Created by steffi on 05.11.21.
//
#include "storm/modelchecker/lexicographic/lexicographicModelChecker.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/automata/LTL2DeterministicAutomaton.h"
#include "storm/logic/Formula.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/logic/ExtractMaximalStateFormulasVisitor.h"

namespace storm {
    namespace modelchecker {
        namespace lexicographic {

        template<typename SparseModelType, typename ValueType>
        std::pair<int, int> lexicographicModelChecker<SparseModelType, ValueType>::getCompleteProductModel(const SparseModelType& model) {
            for (const std::shared_ptr<const storm::logic::Formula> subFormula : this->formula.getSubformulas()) {
                storm::logic::StateFormula const& newFormula = (*subFormula).asStateFormula();
                storm::logic::ProbabilityOperatorFormula const& newFormula2 = (*subFormula).asProbabilityOperatorFormula();
                storm::logic::Formula const& newFormula3 = newFormula2.getSubformula();
                storm::logic::PathFormula const& formula = newFormula3.asPathFormula();
                storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap extracted;
                std::shared_ptr<storm::logic::Formula> ltlFormula1 = storm::logic::ExtractMaximalStateFormulasVisitor::extract(formula, extracted);
                std::shared_ptr<storm::automata::DeterministicAutomaton> da = storm::automata::LTL2DeterministicAutomaton::ltl2daSpot(*ltlFormula1, true, true);
            }
            return std::make_pair(0,0);
        }
        template<typename SparseModelType, typename ValueType>
        std::pair<int, int> lexicographicModelChecker<SparseModelType, ValueType>::solve(int productModel, int acceptanceCondition) {
            return std::pair<int, int>(0,0);
        }
        template<typename SparseModelType, typename ValueType>
        int lexicographicModelChecker<SparseModelType, ValueType>::reachability(int bcc, int bccLexArray, int productModel) {
            return 0;
        }

        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<double>, double>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;

        }
    }
}
