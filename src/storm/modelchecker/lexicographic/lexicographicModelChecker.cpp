//
// Created by steffi on 05.11.21.
//
#include "storm/modelchecker/lexicographic/lexicographicModelChecker.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/logic/Formula.h"
//#include "storm/modelchecker/CheckTask.h"
#include "storm/logic/ExtractMaximalStateFormulasVisitor.h"
#include "storm/automata/APSet.h"
//#include "storm/modelchecker/helper/ltl/SparseLTLHelper.h"
//#include "storm/exceptions/NotSupportedException.h"
//#include "storm/exceptions/ExpressionEvaluationException.h"
#include "storm/automata/DeterministicAutomaton.h"
#include "storm/modelchecker/lexicographic/spotHelper/spotProduct.h"


namespace storm {
    namespace modelchecker {
        namespace lexicographic {

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        std::pair<std::shared_ptr<storm::transformer::DAProduct<SparseModelType>>, std::vector<std::shared_ptr<storm::automata::AcceptanceCondition>>> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::getCompleteProductModel(const SparseModelType& model, CheckFormulaCallback const& formulaChecker) {
            storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap extracted;
            std::vector<std::shared_ptr<storm::automata::AcceptanceCondition>> acceptanceConditions;
            // Get the big product automton for all subformulae
            std::shared_ptr<storm::automata::DeterministicAutomaton> productAutomaton = spothelper::ltl2daSpotProduct<SparseModelType, ValueType>(this->formula, formulaChecker, model, extracted, acceptanceConditions);

            storm::automata::DeterministicAutomaton test = *productAutomaton;
            storm::automata::AcceptanceCondition condi = *(test.getAcceptance());
            auto ags = *(condi.getAcceptanceExpression());
            // Compute Satisfaction sets for the APs (which represent the state-subformulae
            auto apSets = computeApSets(extracted, formulaChecker);

            std::map<std::string, storm::storage::BitVector> apSatSets = computeApSets(extracted, formulaChecker);
            const storm::automata::APSet& apSet = productAutomaton->getAPSet();
            std::vector<storm::storage::BitVector> statesForAP;
            for (const std::string& ap : apSet.getAPs()) {
                auto it = apSatSets.find(ap);
                STORM_LOG_THROW(it != apSatSets.end(), storm::exceptions::InvalidOperationException,
                                "Deterministic automaton has AP " << ap << ", does not appear in formula");

                statesForAP.push_back(std::move(it->second));
            }

            storm::storage::BitVector statesOfInterest;

            if (this->hasRelevantStates()) {
                statesOfInterest = this->getRelevantStates();
            } else {
                // Product from all model states
                statesOfInterest = storm::storage::BitVector(this->_transitionMatrix.getRowGroupCount(), true);
            }

            transformer::DAProductBuilder productBuilder(*productAutomaton, statesForAP);


            std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> product = productBuilder.build<SparseModelType>(model.getTransitionMatrix(), statesOfInterest);

            STORM_PRINT("Product "+ (Nondeterministic ? std::string("MDP-DA") : std::string("DTMC-DA")) +" has " << product->getProductModel().getNumberOfStates() << " states and "
                                                                                                                      << product->getProductModel().getNumberOfTransitions() << " transitions." << std::endl);
            return std::make_pair(product,acceptanceConditions);
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        std::pair<storm::storage::StronglyConnectedComponentDecomposition<ValueType>, std::vector<std::vector<bool>>> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::solve(std::shared_ptr<storm::transformer::DAProduct<productModelType>> productModel, std::vector<std::shared_ptr<storm::automata::AcceptanceCondition>>& acceptanceConditions, storm::logic::MultiObjectiveFormula const& formula) {
            const uint num_formulae = formula.getNumberOfSubformulas();
            storm::storage::StronglyConnectedComponentDecomposition<ValueType> bottomSccs = computeECs(*(productModel->getAcceptance()), productModel->getProductModel().getTransitionMatrix(), productModel->getProductModel().getBackwardTransitions(), productModel);
            std::vector<std::vector<bool>> bscc_satisfaction;
            for (auto& scc : bottomSccs) {
                std::vector<bool> bsccAccepting;
                for (auto& acceptance : acceptanceConditions) {
                    if (acceptance->isAccepting(scc)) {
                        bsccAccepting.push_back(true);
                    }
                    else {
                        bsccAccepting.push_back(false);
                    }
                }
                bscc_satisfaction.push_back(bsccAccepting);
            }
            return std::pair<storm::storage::StronglyConnectedComponentDecomposition<ValueType>, std::vector<std::vector<bool>>>(bottomSccs,bscc_satisfaction);
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        storm::storage::StronglyConnectedComponentDecomposition<ValueType> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::computeECs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions,  typename transformer::DAProduct<productModelType>::ptr product) {
            /*STORM_LOG_INFO("Computing accepting states for acceptance condition " << *acceptance.getAcceptanceExpression());
            if (acceptance.getAcceptanceExpression()->isTRUE()) {
                STORM_LOG_INFO(" TRUE -> all states accepting (assumes no deadlock in the model)");
                return storm::storage::BitVector(transitionMatrix.getRowGroupCount(), true);
            } else if (acceptance.getAcceptanceExpression()->isFALSE()) {
                STORM_LOG_INFO(" FALSE -> all states rejecting");
                return storm::storage::BitVector(transitionMatrix.getRowGroupCount(), false);
            }
            std::vector<std::vector<automata::AcceptanceCondition::acceptance_expr::ptr>> cnf = acceptance.extractFromCNF();*/
            //storm::storage::BitVector allowed(transitionMatrix.getRowGroupCount(), true);
            storm::storage::StronglyConnectedComponentDecomposition<ValueType> bottomSccs(transitionMatrix, storage::StronglyConnectedComponentDecompositionOptions().onlyBottomSccs().dropNaiveSccs());
            //storm::storage::MaximalEndComponentDecomposition<ValueType> mecs(transitionMatrix, backwardTransitions, allowed);
            return bottomSccs;
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        int lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::reachability(storm::storage::StronglyConnectedComponentDecomposition<ValueType> bcc, std::vector<std::vector<bool>> bccLexArray, std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> productModel) {
            return 0;
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        std::map<std::string, storm::storage::BitVector> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::computeApSets(std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, CheckFormulaCallback const& formulaChecker) {
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



        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<double>, double, true>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<double>, double, false>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber, true>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber, false>;
        }
    }
}
