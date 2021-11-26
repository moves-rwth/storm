//
// Created by steffi on 18.11.21.
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
#include "spot/twaalgos/dot.hh"
#include "spot/twaalgos/product.hh"
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
        for (const std::shared_ptr<const storm::logic::Formula> subFormula : formula.getSubformulas()) {
            // get the formula in the right format (necessary?)
            storm::logic::StateFormula const& newFormula = (*subFormula).asStateFormula();
            storm::logic::ProbabilityOperatorFormula const& newFormula2 = (*subFormula).asProbabilityOperatorFormula();
            storm::logic::Formula const& newFormula3 = newFormula2.getSubformula();
            storm::logic::PathFormula const& formula = newFormula3.asPathFormula();

            // get map of state-expressions to propositions
            std::shared_ptr<storm::logic::Formula> ltlFormula1 = storm::logic::ExtractMaximalStateFormulasVisitor::extract(formula, extracted);
            STORM_PRINT("Extracted: ");
            for (auto i : extracted) {
                std::string ap = i.first;
                STORM_PRINT(ap << "=" << i.second->toString() <<",");
            }
            STORM_PRINT(std::endl);

            // get automaton
            //std::shared_ptr<storm::automata::DeterministicAutomaton> dap = storm::automata::LTL2DeterministicAutomaton::ltl2daSpot(*ltlFormula1, true, true);
            std::string prefixLtl = ltlFormula1->toPrefixString();
            spot::parsed_formula spotPrefixLtl = spot::parse_prefix_ltl(prefixLtl);
            if(!spotPrefixLtl.errors.empty()){
                std::ostringstream errorMsg;
                spotPrefixLtl.format_errors(errorMsg);
                STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Spot could not parse formula: " << prefixLtl << ": " << errorMsg.str());
            }
            spot::formula spotFormula = spotPrefixLtl.f;

            // Request a deterministic, complete automaton with state-based acceptance
            spot::translator trans = spot::translator(dict);
            trans.set_type(spot::postprocessor::Generic);
            trans.set_pref(spot::postprocessor::Deterministic | spot::postprocessor::SBAcc | spot::postprocessor::Complete);
            STORM_PRINT("1 - Construct deterministic automaton for "<< spotFormula << std::endl);
            // aut contains the Spot-automaton
            auto aut = trans.run(spotFormula);
            STORM_PRINT("2 - DNF info " << aut->get_acceptance() << " " << aut->get_acceptance().is_dnf() << std::endl);

            if(!(aut->get_acceptance().is_dnf())){
                STORM_PRINT("3 - Convert acceptance condition "<< aut->get_acceptance() << " into DNF..." << std::endl);
                // Transform the acceptance condition in disjunctive normal form and merge all the Fin-sets of each clause
                aut = to_generalized_streett(aut,true);
            }
            aut = spot::dnf_to_streett(aut);
            std::ostream objOstream (std::cout.rdbuf());
            STORM_PRINT("Automaton for " << prefixLtl << " ");
            spot::print_dot(objOstream, aut, "cak");

            acceptanceConditions.push_back(countAccept);
            countAccept += aut->get_acceptance().top_conjuncts().size();
            STORM_PRINT("4 - The deterministic automaton has acceptance condition:  "<< aut->get_acceptance() << std::endl);
            std::stringstream autStreamTemp;
            // Print reachable states in HOA format, implicit edges (i), state-based acceptance (s)
            spot::print_hoa(autStreamTemp, aut, "is");
            storm::automata::DeterministicAutomaton::ptr temp = storm::automata::DeterministicAutomaton::parse(autStreamTemp);
            //acceptanceConditions.push_back(temp->getAcceptance());
            if (first) {
                productAutomaton = aut;
                first = false;

                auto bla = aut->get_dict();
                std::cout << bla << std::endl;
                continue;
            } else {
                STORM_PRINT("product Automaton before ");
                std::ostream objOstream (std::cout.rdbuf());
                spot::print_dot(objOstream, productAutomaton, "cak");
                productAutomaton = spot::product(aut,productAutomaton);
                auto bla = productAutomaton->get_acceptance();
                auto blubb = bla.top_conjuncts();
                STORM_PRINT("conjuncts " << blubb.size() << std::endl);
            }
        }
        acceptanceConditions.push_back(countAccept);
        if(!(productAutomaton->get_acceptance().is_cnf())){
            STORM_PRINT("Convert acceptance condition "<< productAutomaton->get_acceptance() << " into CNF..." << std::endl);
            // Transform the acceptance condition in disjunctive normal form and merge all the Fin-sets of each clause
            productAutomaton = to_generalized_streett(productAutomaton,true);
        }
        std::stringstream autStream;
        // Print reachable states in HOA format, implicit edges (i), state-based acceptance (s)
        spot::print_hoa(autStream, productAutomaton, "is");
        std::ostream objOstream (std::cout.rdbuf());
        spot::print_dot(objOstream, productAutomaton, "cak");

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
