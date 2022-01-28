//
// Created by Steffi on 18.11.21.
//
#include "storm/modelchecker/lexicographic/spotHelper/spotProduct.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/ExpressionEvaluationException.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/logic/Formulas.h"

#ifdef STORM_HAVE_SPOT
#include "spot/tl/formula.hh"
#include "spot/tl/parse.hh"
#include "spot/twaalgos/translate.hh"
#include "spot/twaalgos/hoa.hh"
#include "spot/twaalgos/totgba.hh"
#include "spot/twaalgos/product.hh"
#include "spot/twaalgos/dot.hh"
#include "storm/modelchecker/lexicographic/spotHelper/spotInternal.h"
#endif

namespace storm{
    namespace spothelper {
    template<typename SparseModelType, typename ValueType>
    std::shared_ptr<storm::automata::DeterministicAutomaton> ltl2daSpotProduct(storm::logic::MultiObjectiveFormula const& formula, CheckFormulaCallback const& formulaChecker, SparseModelType const& model, storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap& extracted, std::vector<uint>& acceptanceConditions) {
#ifdef STORM_HAVE_SPOT
        bool first = true;
        spot::twa_graph_ptr productAutomaton;
        spot::bdd_dict_ptr dict = spot::make_bdd_dict();
        uint countAccept = 0;
        // iterate over all subformulae
        for (const std::shared_ptr<const storm::logic::Formula>& subFormula : formula.getSubformulas()) {
            // get the formula in the right format (necessary?)
            storm::logic::StateFormula const& newFormula = (*subFormula).asStateFormula();
            storm::logic::ProbabilityOperatorFormula const& newFormula2 = (*subFormula).asProbabilityOperatorFormula();
            storm::logic::Formula const& newFormula3 = newFormula2.getSubformula();
            storm::logic::PathFormula const& formulaFinal = newFormula3.asPathFormula();

            // get map of state-expressions to propositions
            std::shared_ptr<storm::logic::Formula> ltlFormula1 = storm::logic::ExtractMaximalStateFormulasVisitor::extract(formulaFinal, extracted);

            // parse the formula in spot-format
            std::string prefixLtl = ltlFormula1->toPrefixString();
            spot::parsed_formula spotPrefixLtl = spot::parse_prefix_ltl(prefixLtl);
            if(!spotPrefixLtl.errors.empty()){
                std::ostringstream errorMsg;
                spotPrefixLtl.format_errors(errorMsg);
                STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Spot could not parse formula: " << prefixLtl << ": " << errorMsg.str());
            }
            spot::formula spotFormula = spotPrefixLtl.f;

            // Request a deterministic, complete automaton with state-based acceptance with parity-acceptance condition (should result in Streett)
            spot::translator trans = spot::translator(dict);
            trans.set_type(spot::postprocessor::Parity);
            trans.set_pref(spot::postprocessor::Deterministic | spot::postprocessor::SBAcc | spot::postprocessor::Complete | spot::postprocessor::Colored);
            // aut contains the Spot-automaton
            auto aut = trans.run(spotFormula);

            acceptanceConditions.push_back(countAccept);
            countAccept += aut->get_acceptance().top_conjuncts().size();
            std::ostream objOstream (std::cout.rdbuf());
            std::cout << "new Automaton\n";
            spot::print_dot(objOstream, aut, "cak");
                if (first) {
                // the first automaton does not need to be merged with the product automaton
                productAutomaton = aut;
                first = false;
                continue;
            } else {
                // create a product of the the new automaton and the already existing product automaton
                //productAutomaton = spot::product(aut,productAutomaton);
                productAutomaton = storm::spothelper::product(aut, productAutomaton);
                std::cout << "New Product Automaton\n";
                spot::print_dot(objOstream, productAutomaton, "cak");
            }
        }
        acceptanceConditions.push_back(countAccept);

        if(!(productAutomaton->get_acceptance().is_cnf())){
            // Transform the acceptance condition in disjunctive normal form and merge all the Fin-sets of each clause
            productAutomaton = to_generalized_streett(productAutomaton,true);
        }
        std::stringstream autStream;
        // Print reachable states in HOA format, implicit edges (i), state-based acceptance (s)
        spot::print_hoa(autStream, productAutomaton, "is");

        // parse the automaton into storm-format
        storm::automata::DeterministicAutomaton::ptr da = storm::automata::DeterministicAutomaton::parse(autStream);

        return da;
#else
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Storm is compiled without Spot support.");
#endif
    }

    template std::shared_ptr<storm::automata::DeterministicAutomaton> ltl2daSpotProduct<storm::models::sparse::Mdp<double>, double>(storm::logic::MultiObjectiveFormula const& formula, CheckFormulaCallback const& formulaChecker, storm::models::sparse::Mdp<double> const& model, storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap& extracted, std::vector<uint>& acceptanceConditions);
    template std::shared_ptr<storm::automata::DeterministicAutomaton> ltl2daSpotProduct<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>(storm::logic::MultiObjectiveFormula const& formula, CheckFormulaCallback const& formulaChecker, storm::models::sparse::Mdp<storm::RationalNumber> const& model, storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap& extracted, std::vector<uint>& acceptanceConditions);
    }
}
