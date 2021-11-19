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
#include "storm/exceptions/NotImplementedException.h"
//#include "storm/exceptions/ExpressionEvaluationException.h"
#include "storm/automata/DeterministicAutomaton.h"
#include "storm/modelchecker/lexicographic/spotHelper/spotProduct.h"
#include "storm/transformer/EndComponentEliminator.h"

namespace storm {
    namespace modelchecker {
        namespace lexicographic {

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        std::pair<std::shared_ptr<storm::transformer::DAProduct<SparseModelType>>, std::vector<uint>> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::getCompleteProductModel(const SparseModelType& model, CheckFormulaCallback const& formulaChecker) {
            storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap extracted;
            std::vector<std::shared_ptr<storm::automata::AcceptanceCondition>> acceptanceConditionsOld;
            std::vector<uint> acceptanceConditions;
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
        std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::getStreettPairs(storm::automata::AcceptanceCondition::acceptance_expr::ptr current) {
            std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> accConds;
            if (current->isOR()) {
                accConds.push_back(current);
            } else if (current->isAND()) {
                auto leftAccConds = getStreettPairs(current->getLeft());
                accConds.reserve(accConds.size() + distance(leftAccConds.begin(),leftAccConds.end()));
                accConds.insert(accConds.end(),leftAccConds.begin(),leftAccConds.end());
                auto rightAccConds = getStreettPairs(current->getRight());
                accConds.reserve(accConds.size() + distance(rightAccConds.begin(),rightAccConds.end()));
                accConds.insert(accConds.end(),rightAccConds.begin(),rightAccConds.end());
            } else {
                STORM_LOG_THROW(true, storm::exceptions::NotImplementedException, "Finding StreettPairs - unknown type " + current->toString());
            }
            return accConds;
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        bool lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::isAcceptingPair(storm::storage::MaximalEndComponent const& scc, storm::automata::AcceptanceCondition::acceptance_expr::ptr const& left, storm::automata::AcceptanceCondition::acceptance_expr::ptr const& right, storm::automata::AcceptanceCondition::ptr const& acceptance) {
            bool accepting = false;
            STORM_LOG_THROW(left->isAtom() && right->isAtom(), storm::exceptions::NotImplementedException, "Not an Atom!");
            const cpphoafparser::AtomAcceptance& atom1 = left->getAtom();
            const storm::storage::BitVector& accSet1 = acceptance->getAcceptanceSet(atom1.getAcceptanceSet());
            const cpphoafparser::AtomAcceptance& atom2 = right->getAtom();
            const storm::storage::BitVector& accSet2 = acceptance->getAcceptanceSet(atom2.getAcceptanceSet());
            STORM_LOG_ASSERT(atom1.getType() == cpphoafparser::AtomAcceptance::TEMPORAL_FIN && atom2.getType() == cpphoafparser::AtomAcceptance::TEMPORAL_INF, "Not a StreettPair!");
            if (!scc.containsAnyState(accSet1) || scc.containsAnyState(accSet2)) {
                accepting = true;
            }
            return accepting;
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        bool lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::isAcceptingCNF(storm::storage::MaximalEndComponent const& scc, std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> const& acceptancePairs, storm::automata::AcceptanceCondition::ptr const& acceptance) {
            bool ret = true;
            for (storm::automata::AcceptanceCondition::acceptance_expr::ptr const& streettPair : acceptancePairs) {
                auto finitePart = streettPair->getLeft();
                auto infinitePart = streettPair->getRight();
                bool pairIsTrue = isAcceptingPair(scc, finitePart, infinitePart, acceptance);
                ret = pairIsTrue;
                if (!ret) break;
            }
            return ret;
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>, std::vector<std::vector<bool>>> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::solve(std::shared_ptr<storm::transformer::DAProduct<productModelType>> productModel, std::vector<uint>& acceptanceConditions, storm::logic::MultiObjectiveFormula const& formula) {
            const uint num_formulae = formula.getNumberOfSubformulas();
            storm::storage::MaximalEndComponentDecomposition<ValueType> bottomSccs = computeECs(*(productModel->getAcceptance()), productModel->getProductModel().getTransitionMatrix(), productModel->getProductModel().getBackwardTransitions(), productModel);
            std::vector<std::vector<bool>> bscc_satisfaction;
            storm::automata::AcceptanceCondition::ptr acceptance = productModel->getAcceptance();
            std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> acceptancePairs = getStreettPairs(acceptance->getAcceptanceExpression());
            for (storm::storage::MaximalEndComponent& scc : bottomSccs) {
                std::vector<bool> bsccAccepting;
                for (uint i=0; i<acceptanceConditions.size()-1; i++) {
                    std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> sub = { acceptancePairs.begin()+acceptanceConditions[i],acceptancePairs.begin()+acceptanceConditions[i+1] };
                    bool accepts = isAcceptingCNF(scc, sub, acceptance);
                    if (accepts) {
                        bsccAccepting.push_back(true);
                    }
                    else {
                        bsccAccepting.push_back(false);
                    }
                }
                std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> sub = { acceptancePairs.begin() + acceptanceConditions.back(),acceptancePairs.end()};
                bool accepts = isAcceptingCNF(scc, sub, acceptance);
                if (accepts) {
                    bsccAccepting.push_back(true);
                }
                else {
                    bsccAccepting.push_back(false);
                }
                bscc_satisfaction.push_back(bsccAccepting);
            }
            return std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>&, std::vector<std::vector<bool>>&>(bottomSccs,bscc_satisfaction);
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        storm::storage::MaximalEndComponentDecomposition<ValueType> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::computeECs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions,  typename transformer::DAProduct<productModelType>::ptr product) {
            /*STORM_LOG_INFO("Computing accepting states for acceptance condition " << *acceptance.getAcceptanceExpression());
            if (acceptance.getAcceptanceExpression()->isTRUE()) {
                STORM_LOG_INFO(" TRUE -> all states accepting (assumes no deadlock in the model)");
                return storm::storage::BitVector(transitionMatrix.getRowGroupCount(), true);
            } else if (acceptance.getAcceptanceExpression()->isFALSE()) {
                STORM_LOG_INFO(" FALSE -> all states rejecting");
                return storm::storage::BitVector(transitionMatrix.getRowGroupCount(), false);
            }
            std::vector<std::vector<automata::AcceptanceCondition::acceptance_expr::ptr>> cnf = acceptance.extractFromCNF();*/
            storm::storage::BitVector allowed(transitionMatrix.getRowGroupCount(), true);
            //storm::storage::StronglyConnectedComponentDecomposition<ValueType> bottomSccs(transitionMatrix, storage::StronglyConnectedComponentDecompositionOptions().onlyBottomSccs().dropNaiveSccs());
            storm::storage::MaximalEndComponentDecomposition<ValueType> mecs(transitionMatrix, backwardTransitions, allowed);
            return mecs;
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        int lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::reachability(storm::storage::MaximalEndComponentDecomposition<ValueType> const& bcc, std::vector<std::vector<bool>> const& bccLexArray, std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> const& productModel) {
            storm::transformer::EndComponentEliminator<ValueType> eliminator = storm::transformer::EndComponentEliminator<ValueType>();
            storm::storage::SparseMatrix<ValueType> originalMatrix;
            storm::storage::MaximalEndComponentDecomposition<ValueType> ecs;
            storm::storage::BitVector subsystemStates;
            storm::storage::BitVector addSinkRowStates;
            auto result = eliminator.transform(originalMatrix, ecs, subsystemStates, addSinkRowStates, true);
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

         /*std::string exec(const char* cmd) {
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
        }*/



        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<double>, double, true>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<double>, double, false>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber, true>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber, false>;
        }
    }
}
