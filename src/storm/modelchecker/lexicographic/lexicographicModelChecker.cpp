//
// Created by steffi on 05.11.21.
//
#include "storm/modelchecker/lexicographic/lexicographicModelChecker.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/logic/Formula.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/logic/ExtractMaximalStateFormulasVisitor.h"
#include "storm/automata/APSet.h"
//#include "storm/modelchecker/helper/ltl/SparseLTLHelper.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/ExpressionEvaluationException.h"
#include "storm/automata/DeterministicAutomaton.h"
#include "storm/transformer/DAProductBuilder.h"

#include "spot/tl/formula.hh"
#include "spot/tl/parse.hh"
#include "spot/twaalgos/translate.hh"
#include "spot/twaalgos/hoa.hh"
#include "spot/twaalgos/totgba.hh"
#include "spot/twaalgos/dot.hh"
#include <spot/parseaut/public.hh>
#include <spot/twaalgos/postproc.hh>
#include "spot/twaalgos/product.hh"
//#include "storm/modelchecker/lexicographic/product.h"

namespace storm {
    namespace modelchecker {
        namespace lexicographic {

        template<typename SparseModelType, typename ValueType>
        std::pair<int, int> lexicographicModelChecker<SparseModelType, ValueType>::getCompleteProductModel(const SparseModelType& model, CheckFormulaCallback const& formulaChecker) {
            //storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap extractedTotal;
            //storm::logic::PathFormula const& blablubb = this->formula.asPathFormula();
            //std::shared_ptr<storm::logic::Formula> ltlFormulaTest = storm::logic::ExtractMaximalStateFormulasVisitor::extract(this->formula, extractedTotal);
            //std::cout << ltlFormulaTest->toPrefixString() << std::endl;
            auto produktAutomaton = ltl2daSpotProduct(formulaChecker, model);
            //transformer::DAProductBuilder productBuilder(produktAutomaton, statesForAP);
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

        template<typename SparseModelType, typename ValueType>
        std::map<std::string, storm::storage::BitVector> lexicographicModelChecker<SparseModelType, ValueType>::computeApSets(std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, CheckFormulaCallback const& formulaChecker) {
            std::map<std::string, storm::storage::BitVector> apSets;
            for (auto& p: extracted) {
                STORM_LOG_DEBUG(" Computing satisfaction set for atomic proposition \"" << p.first << "\" <=> " << *p.second << "...");
                apSets[p.first] = formulaChecker(*p.second);
            }
            return apSets;
        }

        std::string exec(const char* cmd) {
            std::array<char, 128> buffer{};
            std::string result;
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }
            return result;
        }

        template<typename SparseModelType, typename ValueType>
        int lexicographicModelChecker<SparseModelType, ValueType>::ltl2daSpotProduct(CheckFormulaCallback const& formulaChecker, SparseModelType const& model) {
            bool first = true;
            spot::twa_graph_ptr productAutomaton;
            spot::bdd_dict_ptr dict = spot::make_bdd_dict();
            storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap extracted;
            std::vector<std::string> acceptanceConditions;
            for (const std::shared_ptr<const storm::logic::Formula> subFormula : this->formula.getSubformulas()) {
                // get the formula in the right format (necessary?)
                storm::logic::StateFormula const& newFormula = (*subFormula).asStateFormula();
                storm::logic::ProbabilityOperatorFormula const& newFormula2 = (*subFormula).asProbabilityOperatorFormula();
                storm::logic::Formula const& newFormula3 = newFormula2.getSubformula();
                storm::logic::PathFormula const& formula = newFormula3.asPathFormula();

                // get map of state-expressions to propositions
                std::shared_ptr<storm::logic::Formula> ltlFormula1 = storm::logic::ExtractMaximalStateFormulasVisitor::extract(formula, extracted);

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
                //acceptanceConditions.push_back(aut->get_acceptance());

                auto bla = aut->get_acceptance();

                STORM_PRINT("4 - The deterministic automaton has acceptance condition:  "<< aut->get_acceptance() << std::endl);
                if (first) {
                    productAutomaton = aut;
                    first = false;

                    auto bla = aut->get_dict();
                    std::cout << bla << std::endl;
                    continue;
                } else {
                    std::ostream objOstream (std::cout.rdbuf());
                    spot::print_dot(objOstream, productAutomaton, "cak");
                    productAutomaton = spot::product(aut,productAutomaton);
                }
                continue;
                //productAutomaton = spotStorm::product(productAutomaton, aut);
                std::ofstream logger;
                logger.open("Aut1.hoa");
                // Print reachable states in HOA format, implicit edges (i), state-based acceptance (s)
                spot::print_hoa(logger, productAutomaton, "is");
                logger.close();
                logger.open("Aut2.hoa");
                spot::print_hoa(logger, aut, "is");
                logger.close();
                std::string cmd = "/home/steffi/Documents/so_test/spot_src/bin/autfilt Aut1.hoa --product=Aut2.hoa";
                const char *run_cmd = cmd.c_str();
                const std::string &result = exec(run_cmd);
                logger.open("Product.hoa");
                logger << result;
                logger.close();
                spot::parsed_aut_ptr pa = spot::parse_aut("Product.hoa", spot::make_bdd_dict());
                if (pa->format_errors(std::cerr))
                    return 1;
                if (pa->aborted)
                {
                    std::cerr << "--ABORT-- read\n";
                    return 1;
                }
                std::ostream objOstream (std::cout.rdbuf());
                spot::print_dot(objOstream, pa->aut, "cak");
                productAutomaton = pa->aut;
            }
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

            //storm::automata::DeterministicAutomaton da = *dap;


            std::map<std::string, storm::storage::BitVector> apSatSets = computeApSets(extracted, formulaChecker);
            const storm::automata::APSet& apSet = da->getAPSet();
            std::vector<storm::storage::BitVector> statesForAP;
            for (const std::string& ap : apSet.getAPs()) {
                auto it = apSatSets.find(ap);
                STORM_LOG_THROW(it != apSatSets.end(), storm::exceptions::InvalidOperationException,
                                "Deterministic automaton has AP " << ap << ", does not appear in formula");

                statesForAP.push_back(std::move(it->second));
            }

            storm::storage::BitVector statesOfInterest;

            /*if (this->hasRelevantStates()) {
                statesOfInterest = this->getRelevantStates();
            } else {
                // Product from all model states
                statesOfInterest = storm::storage::BitVector(this->_transitionMatrix.getRowGroupCount(), true);
            }

            transformer::DAProductBuilder productBuilder(*da, statesForAP);*/


            //auto product = productBuilder.build<productModelType>(model.getTransitionMatrix(), statesOfInterest);

            return 0;

        }
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<double>, double>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;

        }
    }
}
