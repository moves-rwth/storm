#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-throw-by-value-catch-by-reference"
//
// Created by steffi on 05.11.21.
//
#include "storm/modelchecker/lexicographic/lexicographicModelChecker.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/logic/Formula.h"
#include "storm/logic/ExtractMaximalStateFormulasVisitor.h"
#include "storm/automata/APSet.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/automata/DeterministicAutomaton.h"
#include "storm/modelchecker/lexicographic/spotHelper/spotProduct.h"
#include "storm/transformer/EndComponentEliminator.h"
#include "storm/transformer/SubsystemBuilder.h"
#include "storm//modelchecker/prctl/helper/SparseMdpPrctlHelper.h"
#include "storm/modelchecker/hints/ExplicitModelCheckerHint.h"
#include "storm/environment/SubEnvironment.h"

namespace storm {
    namespace modelchecker {
    namespace helper {
    namespace lexicographic {

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    std::pair<std::shared_ptr<storm::transformer::DAProduct<SparseModelType>>, std::vector<uint>>
    lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::getCompleteProductModel(const SparseModelType& model,
                                                                                                     CheckFormulaCallback const& formulaChecker) {
        storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap extracted;
        std::vector<std::shared_ptr<storm::automata::AcceptanceCondition>> acceptanceConditionsOld;
        std::vector<uint> acceptanceConditions;
        // Get the big product automton for all subformulae
        std::shared_ptr<storm::automata::DeterministicAutomaton> productAutomaton =
            spothelper::ltl2daSpotProduct<SparseModelType, ValueType>(this->formula, formulaChecker, model, extracted, acceptanceConditions);

        storm::automata::DeterministicAutomaton test = *productAutomaton;
        storm::automata::AcceptanceCondition condi = *(test.getAcceptance());
        //auto ags = *(condi.getAcceptanceExpression());
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

        std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> product =
            productBuilder.build<SparseModelType>(model.getTransitionMatrix(), statesOfInterest);

        STORM_PRINT("Product " + (Nondeterministic ? std::string("MDP-DA") : std::string("DTMC-DA")) + " has "
                    << product->getProductModel().getNumberOfStates() << " states and " << product->getProductModel().getNumberOfTransitions() << " transitions."
                    << std::endl);
        return std::make_pair(product, acceptanceConditions);
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr>
    lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::getStreettPairs(storm::automata::AcceptanceCondition::acceptance_expr::ptr const& current) {
        std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> accConds;
        if (current->isOR()) {
            accConds.push_back(current);
        } else if (current->isAND()) {
            auto leftAccConds = getStreettPairs(current->getLeft());
            accConds.reserve(accConds.size() + distance(leftAccConds.begin(), leftAccConds.end()));
            accConds.insert(accConds.end(), leftAccConds.begin(), leftAccConds.end());
            auto rightAccConds = getStreettPairs(current->getRight());
            accConds.reserve(accConds.size() + distance(rightAccConds.begin(), rightAccConds.end()));
            accConds.insert(accConds.end(), rightAccConds.begin(), rightAccConds.end());
        } else {
            STORM_LOG_THROW(true, storm::exceptions::NotImplementedException, "Finding StreettPairs - unknown type " + current->toString());
        }
        return accConds;
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    bool lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::isAcceptingPair(
        storm::storage::MaximalEndComponent const& scc, storm::automata::AcceptanceCondition::acceptance_expr::ptr const& left,
        storm::automata::AcceptanceCondition::acceptance_expr::ptr const& right, storm::automata::AcceptanceCondition::ptr const& acceptance) {
        bool accepting = false;
        STORM_LOG_THROW(left->isAtom() && right->isAtom(), storm::exceptions::NotImplementedException, "Not an Atom!");
        const cpphoafparser::AtomAcceptance& atom1 = left->getAtom();
        const storm::storage::BitVector& accSet1 = acceptance->getAcceptanceSet(atom1.getAcceptanceSet());
        const cpphoafparser::AtomAcceptance& atom2 = right->getAtom();
        const storm::storage::BitVector& accSet2 = acceptance->getAcceptanceSet(atom2.getAcceptanceSet());
        STORM_LOG_ASSERT(atom1.getType() == cpphoafparser::AtomAcceptance::TEMPORAL_FIN && atom2.getType() == cpphoafparser::AtomAcceptance::TEMPORAL_INF,
                         "Not a StreettPair!");
        if (!scc.containsAnyState(accSet1) || scc.containsAnyState(accSet2)) {
            accepting = true;
        }
        return accepting;
    }

    const storm::storage::BitVector& getStreettSet(storm::automata::AcceptanceCondition::ptr const& acceptance,
                                                   storm::automata::AcceptanceCondition::acceptance_expr::ptr const& setPointer) {
        STORM_LOG_THROW(setPointer->isAtom(), storm::exceptions::NotImplementedException, "Not an Atom!");
        const cpphoafparser::AtomAcceptance& atom = setPointer->getAtom();
        const storm::storage::BitVector& accSet = acceptance->getAcceptanceSet(atom.getAcceptanceSet());
        return accSet;
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    bool lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::isAcceptingStreettConditions(
        storm::storage::MaximalEndComponent const& scc, std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> const& acceptancePairs,
        storm::automata::AcceptanceCondition::ptr const& acceptance, productModelType model) {
        bool ret = true;
        std::vector<uint_fast64_t> states;
        for (auto const& state : scc) {
            states.push_back(state.first);
        }
        storm::storage::BitVector mecStates = storm::storage::BitVector(model.getNumberOfStates(), states);
        bool changedSomething = true;
        while (changedSomething) {
            changedSomething = false;
            auto subMecDecomposition = storm::storage::MaximalEndComponentDecomposition<ValueType>(model, mecStates);
            for (storm::storage::MaximalEndComponent const& mec : subMecDecomposition) {
                for (storm::automata::AcceptanceCondition::acceptance_expr::ptr const& streettPair : acceptancePairs) {
                    auto infSet = getStreettSet(acceptance, streettPair->getRight());
                    auto finSet = getStreettSet(acceptance, streettPair->getLeft());
                    if (mec.containsAnyState(infSet)) {
                        // streett-condition is true (INF is fulfilled)
                        continue;
                    } else {
                        // INF cannot be fulfilled
                        // check if there's a state from FIN
                        for (auto const& stateToChoice : mec) {
                            StateType state = stateToChoice.first;
                            if (finSet.get(state)) {
                                // remove the state from the set of states in this EC
                                mecStates.set(state, false);
                                model.removeIncomingTransitions(state);
                                // remove all incoming transitions to this state
                                changedSomething = true;
                            }
                        }
                    }
                }
            }
            //                for (storm::automata::AcceptanceCondition::acceptance_expr::ptr const& streettPair : acceptancePairs) {
            //                    STORM_PRINT("Evaluating pair : " << streettPair->toString() << std::endl);
            //                    auto finitePart = streettPair->getLeft();
            //                    auto infinitePart = streettPair->getRight();
            //                    bool pairIsTrue = isAcceptingPair(scc, finitePart, infinitePart, acceptance);
            //                    ret = pairIsTrue;
            //                    if (!ret) break;
            //                }
        }
        auto subMecDecomposition = storm::storage::MaximalEndComponentDecomposition<ValueType>(model, mecStates);
        if (subMecDecomposition.empty()) {
            // there are no more ECs in this set of states
            return false;
        } else {
            return true;
        }
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>, std::vector<std::vector<bool>>>
    lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::solve(std::shared_ptr<storm::transformer::DAProduct<productModelType>> productModel,
                                                                                   std::vector<uint>& acceptanceConditions,
                                                                                   storm::storage::BitVector& allowed) {
        const uint num_formulae = formula.getNumberOfSubformulas();
        std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>, storm::storage::BitVector> result =
            computeECs(*(productModel->getAcceptance()), productModel->getProductModel().getTransitionMatrix(),
                       productModel->getProductModel().getBackwardTransitions(), allowed);
        storm::storage::MaximalEndComponentDecomposition<ValueType> msccs = result.first;
        storm::storage::BitVector allowed_states = result.second;
        std::vector<std::vector<bool>> bscc_satisfaction;
        storm::automata::AcceptanceCondition::ptr acceptance = productModel->getAcceptance();
        std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> acceptancePairs = getStreettPairs(acceptance->getAcceptanceExpression());
        std::reverse(acceptancePairs.begin(), acceptancePairs.end());
        // STORM_PRINT("Streett Pairs: "<<acceptancePairs.size() << std::endl);
        for (storm::storage::MaximalEndComponent& scc : msccs) {
            std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> sprime;
            std::vector<bool> bsccAccepting;
            for (uint i = 0; i < acceptanceConditions.size() - 1; i++) {
                std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> sprimeTemp(sprime);
                std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr> sub = {acceptancePairs.begin() + acceptanceConditions[i],
                                                                                               acceptancePairs.begin() + acceptanceConditions[i + 1]};
                sprimeTemp.insert(sprimeTemp.end(), sub.begin(), sub.end());
                // TODO correct algorithm
                bool accepts = isAcceptingStreettConditions(scc, sprimeTemp, acceptance, productModel->getProductModel());
                if (accepts) {
                    bsccAccepting.push_back(true);
                    sprime.insert(sprime.end(), sub.begin(), sub.end());
                } else {
                    bsccAccepting.push_back(false);
                }
            }
            bscc_satisfaction.push_back(bsccAccepting);
        }
        return std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>&, std::vector<std::vector<bool>>&>(msccs, bscc_satisfaction);
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>, storm::storage::BitVector>
    lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::computeECs(automata::AcceptanceCondition const& acceptance,
                                                                                        storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                                        storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                                                        storm::storage::BitVector& allowed) {
        /*STORM_LOG_INFO("Computing accepting states for acceptance condition " << *acceptance.getAcceptanceExpression());
        if (acceptance.getAcceptanceExpression()->isTRUE()) {
            STORM_LOG_INFO(" TRUE -> all states accepting (assumes no deadlock in the model)");
            return storm::storage::BitVector(transitionMatrix.getRowGroupCount(), true);
        } else if (acceptance.getAcceptanceExpression()->isFALSE()) {
            STORM_LOG_INFO(" FALSE -> all states rejecting");
            return storm::storage::BitVector(transitionMatrix.getRowGroupCount(), false);
        }
        std::vector<std::vector<automata::AcceptanceCondition::acceptance_expr::ptr>> cnf = acceptance.extractFromCNF();*/
        // storm::storage::BitVector allowed(transitionMatrix.getRowGroupCount(), true);
        // storm::storage::StronglyConnectedComponentDecomposition<ValueType> bottomSccs(transitionMatrix, storage::StronglyConnectedComponentDecompositionOptions().onlyBottomSccs().dropNaiveSccs());
        storm::storage::MaximalEndComponentDecomposition<ValueType> mecs(transitionMatrix, backwardTransitions, allowed);
        return std::make_pair(mecs, allowed);
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    storm::storage::BitVector lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::getGoodStates(
        storm::storage::MaximalEndComponentDecomposition<ValueType> const& bcc,
        std::vector<std::vector<bool>> const& bccLexArray,
        std::vector<uint_fast64_t> const& oldToNewStateMapping,
        uint const& condition,
        uint const numStates) {
        STORM_LOG_ASSERT(condition<bccLexArray.size(),"Condition is not in Lex-Array!");
        std::vector<uint_fast64_t> goodStates;
        for(uint i=0; i<bcc.size(); i++) {
            std::vector<bool> const& bccLex = bccLexArray[i];
            if (bccLex[condition]) {
                auto firstElement = *bcc[i].begin();
                uint_fast64_t bccStateOld = firstElement.first;
                uint_fast64_t bccState = oldToNewStateMapping[bccStateOld];
                goodStates.push_back(bccState);
            }
        }
        return {numStates, goodStates};
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    MDPSparseModelCheckingHelperReturnType<ValueType> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::reachability(
        storm::storage::MaximalEndComponentDecomposition<ValueType> const& bcc, std::vector<std::vector<bool>> const& bccLexArray,
        std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> const& productModel, storm::storage::BitVector& allowed,
        SparseModelType const& originalMdp, ValueType& resultingProb) {
        storm::transformer::EndComponentEliminator<ValueType> eliminator = storm::transformer::EndComponentEliminator<ValueType>();
        // storm::storage::SparseMatrix<ValueType> originalMatrix;
        SparseModelType blubb = productModel->getProductModel();
        auto blaa = blubb.getTransitionMatrix();
        // storm::storage::MaximalEndComponentDecomposition<ValueType> ecs;
        // storm::storage::BitVector subsystemStates;
        // storm::storage::BitVector addSinkRowStates;
        auto result = eliminator.transform(blaa, bcc, allowed, allowed, true);
        storm::storage::BitVector choices(result.matrix.getColumnCount(), true);
        Environment env;
        // A reachability condition "F x" is transformed to "true U x"
        // phi states are all states
        // psi states are the ones from the "good bccs"
        storm::storage::BitVector phiStates(result.matrix.getColumnCount(), true);
        storm::storage::BitVector psiStates = getGoodStates(bcc, bccLexArray, result.oldToNewStateMapping, 0, result.matrix.getColumnCount());
        storm::storage::BitVector const& originalInitialStates = originalMdp.getInitialStates();
        std::vector<uint_fast64_t> newInitalStates;
        uint pos = 0;
        while (originalInitialStates.getNextSetIndex(pos)!=originalInitialStates.size()) {
            pos = originalInitialStates.getNextSetIndex(pos)+1;
            newInitalStates.push_back(result.oldToNewStateMapping[pos-1]);
        }
        storm::storage::BitVector i(result.matrix.getColumnCount(), newInitalStates);
        ExplicitModelCheckerHint<ValueType> hint = ExplicitModelCheckerHint<ValueType>();
        hint.setNoEndComponentsInMaybeStates(false);
        storm::solver::SolveGoal<ValueType> testGoal = storm::solver::SolveGoal<ValueType>(storm::solver::OptimizationDirection::Maximize, i);
        MDPSparseModelCheckingHelperReturnType<ValueType> ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(
            env, storm::solver::SolveGoal<ValueType>(storm::solver::OptimizationDirection::Maximize, i), result.matrix,
            result.matrix.transpose(true), phiStates,
            psiStates, false, true, hint);
        resultingProb = ret.values[newInitalStates[0]];

        std::vector<std::vector<uint>> optimalActions;
        std::vector<uint_fast64_t> const& rowGroupIndices = result.matrix.getRowGroupIndices();
        for (uint currentState=0; currentState<result.matrix.getColumnCount(); currentState++) {
            std::vector<uint> goodActionsForState;
            auto bestAction = ret.scheduler->getChoice(currentState).getDeterministicChoice();
            ValueType bestActionValue(0);
            for (storm::storage::MatrixEntry<uint_fast64_t,ValueType> rowEntry : result.matrix.getRow(rowGroupIndices[currentState]+bestAction)) {
                bestActionValue += rowEntry.getValue() * ret.values[rowEntry.getColumn()];
            }
            uint lastAction = rowGroupIndices[currentState] + result.matrix.getRowGroupSize(currentState);
            auto possibleActions = result.matrix.getRowGroup(currentState);
            for (uint action=rowGroupIndices[currentState]; action<lastAction; action++) {
                ValueType actionValue(0);
                for (auto rowEntry : result.matrix.getRow(action)) {
                    actionValue += rowEntry.getValue() * ret.values[rowEntry.getColumn()];
                }
                if (actionValue == bestActionValue) {
                    goodActionsForState.push_back(action);
                }
            }
            optimalActions.push_back(goodActionsForState);
        }
        return ret;
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    std::map<std::string, storm::storage::BitVector> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::computeApSets(
        std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, CheckFormulaCallback const& formulaChecker) {
        std::map<std::string, storm::storage::BitVector> apSets;
        for (auto& p : extracted) {
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
}

#pragma clang diagnostic pop