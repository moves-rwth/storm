#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-throw-by-value-catch-by-reference"
//
// Created by steffi on 05.11.21.
//
#include "storm/modelchecker/lexicographic/lexicographicModelCheckerHelper.h"
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
    lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::getCompleteProductModel(const SparseModelType& model,
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

        return std::make_pair(product, acceptanceConditions);
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    std::vector<storm::automata::AcceptanceCondition::acceptance_expr::ptr>
    lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::getStreettPairs(storm::automata::AcceptanceCondition::acceptance_expr::ptr const& current) {
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
    bool lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::isAcceptingPair(
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
    bool lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::isAcceptingStreettConditions(
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
    lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::solve(std::shared_ptr<storm::transformer::DAProduct<productModelType>> productModel,
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
        STORM_LOG_ASSERT(!acceptancePairs.empty(), "There are no accepting pairs, maybe you have a parity automaton?");
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
    lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::computeECs(automata::AcceptanceCondition const& acceptance,
                                                                                        storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                                        storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                                                        storm::storage::BitVector& allowed) {
        storm::storage::MaximalEndComponentDecomposition<ValueType> mecs(transitionMatrix, backwardTransitions, allowed);
        return std::make_pair(mecs, allowed);
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    storm::storage::BitVector lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::getGoodStates(
            storm::storage::MaximalEndComponentDecomposition<ValueType> const& bcc,
            std::vector<std::vector<bool>> const& bccLexArray,
            std::vector<uint_fast64_t> const& oldToNewStateMapping,
            uint const& condition,
            uint const numStates,
            std::vector<uint_fast64_t> const& compressedToReducedMapping,
            std::map<uint,uint_fast64_t> const& bccToStStateMapping) {
        STORM_LOG_ASSERT(!bccLexArray.empty(),"Lex-Array is empty!");
        STORM_LOG_ASSERT(condition<bccLexArray[0].size(),"Condition is not in Lex-Array!");
        std::vector<uint_fast64_t> goodStates;
        for(uint i=0; i<bcc.size(); i++) {
            std::vector<bool> const& bccLex = bccLexArray[i];
            if (bccLex[condition]) {
                uint_fast64_t bccStateOld = bccToStStateMapping.at(i);
                uint_fast64_t bccState = oldToNewStateMapping[bccStateOld];
                auto pointer = std::find(compressedToReducedMapping.begin(), compressedToReducedMapping.end(), bccState);
                if (pointer!=compressedToReducedMapping.end()) {
                    // We have to check whether the state has already been removed
                    uint_fast64_t bccStateReduced = pointer - compressedToReducedMapping.begin();
                    goodStates.push_back(bccStateReduced);
                }
            }
        }
        return {numStates, goodStates};
    }


    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    MDPSparseModelCheckingHelperReturnType<ValueType> lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::solveOneReachability(
            std::vector<uint_fast64_t>& newInitalStates,
            storm::storage::BitVector const& psiStates,
            storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
            SparseModelType const& originalMdp,
            std::vector<uint_fast64_t> const& compressedToReducedMapping,
            std::vector<uint_fast64_t> const& oldToNewStateMapping) {
        Environment env;
        // A reachability condition "F x" is transformed to "true U x"
        // phi states are all states
        // psi states are the ones from the "good bccs"
        storm::storage::BitVector phiStates(transitionMatrix.getColumnCount(), true);

        // Get initial states in the compressed model
        storm::storage::BitVector const& originalInitialStates = originalMdp.getInitialStates();

        uint pos = 0;
        while (originalInitialStates.getNextSetIndex(pos)!=originalInitialStates.size()) {
            pos = originalInitialStates.getNextSetIndex(pos)+1;
            auto pointer = std::find(compressedToReducedMapping.begin(), compressedToReducedMapping.end(), oldToNewStateMapping[pos-1]);
            if (pointer!=compressedToReducedMapping.end()) {
                newInitalStates.push_back(pointer - compressedToReducedMapping.begin());
            }
        }
        storm::storage::BitVector i(transitionMatrix.getColumnCount(), newInitalStates);

        ModelCheckerHint hint;
        storm::solver::SolveGoal<ValueType> testGoal = storm::solver::SolveGoal<ValueType>(storm::solver::OptimizationDirection::Maximize, i);
        MDPSparseModelCheckingHelperReturnType<ValueType> ret = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(
            env, storm::solver::SolveGoal<ValueType>(storm::solver::OptimizationDirection::Maximize, i), transitionMatrix,
            transitionMatrix.transpose(true), phiStates,
            psiStates, false, true, hint);
        return ret;
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    typename lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::SubsystemReturnType lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::getReducedSubsystem(
            storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
            MDPSparseModelCheckingHelperReturnType<ValueType> const& reachabilityResult,
            std::vector<uint_fast64_t> const& newInitalStates,
            storm::storage::BitVector const& goodStates
        ) {
        std::vector<std::vector<uint>> optimalActions;
        std::vector<uint_fast64_t> keptActions;
        std::vector<uint_fast64_t> const& rowGroupIndices = transitionMatrix.getRowGroupIndices();
        for (uint currentState=0; currentState<reachabilityResult.values.size(); currentState++) {
            std::vector<uint> goodActionsForState;
            uint_fast64_t bestAction;
            if (goodStates.get(currentState)) {
                uint lastAction = rowGroupIndices[currentState] + transitionMatrix.getRowGroupSize(currentState);
                uint_fast64_t count = rowGroupIndices[currentState];
                for (uint action = rowGroupIndices[currentState]; action < lastAction; action++) {
                    for (auto rowEntry : transitionMatrix.getRow(action)) {
                        if (rowEntry.getValue()==1 && rowEntry.getColumn() == currentState) {
                            // Found the self-loop
                            bestAction = count;
                        }
                    }
                    count++;
                }
                goodActionsForState.push_back(bestAction);
                keptActions.push_back(bestAction);
                optimalActions.push_back(goodActionsForState);
            } else {
                bestAction = reachabilityResult.scheduler->getChoice(currentState).getDeterministicChoice();

                ValueType bestActionValue(0);
                for (storm::storage::MatrixEntry<uint_fast64_t, ValueType> rowEntry : transitionMatrix.getRow(rowGroupIndices[currentState] + bestAction)) {
                    bestActionValue += rowEntry.getValue() * reachabilityResult.values[rowEntry.getColumn()];
                }
                uint lastAction = rowGroupIndices[currentState] + transitionMatrix.getRowGroupSize(currentState);
                for (uint action = rowGroupIndices[currentState]; action < lastAction; action++) {
                    ValueType actionValue(0);
                    for (auto rowEntry : transitionMatrix.getRow(action)) {
                        actionValue += rowEntry.getValue() * reachabilityResult.values[rowEntry.getColumn()];
                    }
                    if (actionValue == bestActionValue) {
                        goodActionsForState.push_back(action);
                        keptActions.push_back(action);
                    }
                }
                optimalActions.push_back(goodActionsForState);
            }
        }
        storm::storage::BitVector subSystemActions(transitionMatrix.getRowCount(), keptActions);
        storm::storage::BitVector subSystemStates(transitionMatrix.getRowGroupCount(),true);
        transformer::SubsystemBuilderOptions sbo;
        sbo.fixDeadlocks = true;
        storm::models::sparse::StateLabeling stateLabelling(transitionMatrix.getColumnCount());
        stateLabelling.addLabel("init");
        for (auto const& state : newInitalStates) {
            stateLabelling.addLabelToState("init",state);
        }
        storm::models::sparse::Mdp<ValueType> newModel(transitionMatrix, stateLabelling);
        SubsystemReturnType subsystemReturn = transformer::buildSubsystem(newModel, subSystemStates, subSystemActions, false, sbo);
        return subsystemReturn;
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    int lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::removeTransientSCCs(std::vector<std::vector<bool>>& bccLexArray,
                                                                                                     uint const& condition,
                                                                                                     storm::storage::MaximalEndComponentDecomposition<ValueType> const& bcc,
                                                                                                     std::vector<uint_fast64_t> const& compressedToReducedMapping,
                                                                                                     std::vector<uint_fast64_t> const& oldToNewStateMapping,
                                                                                                     MDPSparseModelCheckingHelperReturnType<ValueType> const& res){
        int count = 0;
        for(uint i=0; i<bcc.size(); i++) {
            std::vector<bool>& bccLex = bccLexArray[i];
            if (!bccLex[condition]) {
                auto firstElement = *bcc[i].begin();
                uint_fast64_t bccStateOld = firstElement.first;
                uint_fast64_t bccState = oldToNewStateMapping[bccStateOld];
                auto pointer = std::find(compressedToReducedMapping.begin(), compressedToReducedMapping.end(), bccState);
                if (pointer != compressedToReducedMapping.end()) {
                    // We have to check whether the state has already been removed
                    uint_fast64_t bccStateReduced = pointer - compressedToReducedMapping.begin();
                    if (res.values[bccStateReduced] > 0) {
                        // SCC with probability of reaching another goal greater than 0
                        bccLex = std::vector<bool>(bccLex.size(),false);
                        count++;
                    }
                }
            }
        }
        return count;
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    std::pair<storm::storage::SparseMatrix<ValueType>,std::map<uint,uint_fast64_t>> lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::addSinkStates(
            storm::storage::MaximalEndComponentDecomposition<ValueType> const& bccs,
            std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> const& productModel) {
            storm::storage::SparseMatrix<ValueType> const matrix = productModel->getProductModel().getTransitionMatrix();
            uint countNewRows = 0;
            std::map<uint_fast64_t, uint> stateToBCC;
            for (uint i=0; i<bccs.size(); i++) {
                auto bcc = bccs[i];
                countNewRows+= bcc.size() + 1;
                for (auto const& stateAndChoices : bcc) {
                    uint_fast64_t bccState = stateAndChoices.first;
                    stateToBCC[bccState] = i;
                }
            }
            storm::storage::SparseMatrixBuilder<ValueType> builder(matrix.getRowCount()+countNewRows, matrix.getColumnCount()+bccs.size(), matrix.getEntryCount()+countNewRows, false, true, matrix.getRowGroupCount());
            uint_fast64_t numRowGroups = matrix.getColumnCount()+bccs.size();
            std::map<uint, uint_fast64_t> sTstatesForBCC;
            uint_fast64_t newestRowGroup = matrix.getRowGroupCount();
            uint_fast64_t newRow = 0;
            uint_fast64_t oldRowCounting = 0;
            auto oldRowGroupIndices = matrix.getRowGroupIndices();
            for (uint_fast64_t newRowGroup = 0; newRowGroup < matrix.getColumnCount(); ++newRowGroup) {
                builder.newRowGroup(newRow);
                for (; oldRowCounting < oldRowGroupIndices[newRowGroup+1]; oldRowCounting++ ) {
                    for(auto const& entry : matrix.getRow(oldRowCounting)){
                        builder.addNextValue(newRow, entry.getColumn(), entry.getValue());
                    }
                    newRow++;
                }
                if ( stateToBCC.find(newRowGroup) != stateToBCC.end() ) {
                    if (sTstatesForBCC.find(stateToBCC[newRowGroup]) == sTstatesForBCC.end()) {
                        sTstatesForBCC[stateToBCC[newRowGroup]] = newestRowGroup;
                        newestRowGroup++;
                    }
                    builder.addNextValue(newRow, sTstatesForBCC[stateToBCC[newRowGroup]], storm::utility::one<ValueType>());
                    newRow++;
                }
            }
            for (uint_fast64_t newRowGroup = matrix.getColumnCount(); newRowGroup < numRowGroups; newRowGroup++) {
                builder.newRowGroup(newRow);
                builder.addNextValue(newRow, newRowGroup, storm::utility::one<ValueType>());
                newRow++;
            }
            storm::storage::SparseMatrix<ValueType> newMatrix = builder.build();
            return {newMatrix, sTstatesForBCC};
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    MDPSparseModelCheckingHelperReturnType<ValueType> lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::reachability(
            storm::storage::MaximalEndComponentDecomposition<ValueType> const& bcc, std::vector<std::vector<bool>> const& bccLexArray,
            std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> const& productModel, storm::storage::BitVector& allowed,
            SparseModelType const& originalMdp, ValueType& resultingProb) {
        // Eliminate all BCCs and generate one sink state instead
        // Add first new states for each BCC
        auto stStateResult = addSinkStates(bcc, productModel);
        storm::storage::SparseMatrix<ValueType> newMatrixWithNewStates = stStateResult.first;
        std::map<uint,uint_fast64_t> bccToStStateMapping = stStateResult.second;
        storm::transformer::EndComponentEliminator<ValueType> eliminator = storm::transformer::EndComponentEliminator<ValueType>();
        storm::storage::BitVector eliminationStates(newMatrixWithNewStates.getColumnCount(),true);
        auto result = eliminator.transform(newMatrixWithNewStates, bcc, eliminationStates, storm::storage::BitVector(eliminationStates.size(), false), true);

        STORM_LOG_ASSERT(!bccLexArray.empty(), "No BCCs in the model!");
        std::vector<std::vector<bool>> bccLexArrayCurrent(bccLexArray);
        MDPSparseModelCheckingHelperReturnType<ValueType> retResult(std::vector<ValueType>(bccLexArray[0].size()));
        std::vector<uint_fast64_t> compressedToReducedMapping(result.matrix.getColumnCount());
        std::iota(std::begin(compressedToReducedMapping), std::end(compressedToReducedMapping), 0);
        storm::storage::SparseMatrix<ValueType> transitionMatrix = result.matrix;

        // check reachability for each condition and restrict the model to optimal choices
        for (uint condition=0; condition<bccLexArray[0].size(); condition++) {
            storm::storage::BitVector psiStates =
                getGoodStates(bcc, bccLexArrayCurrent, result.oldToNewStateMapping, condition, transitionMatrix.getColumnCount(), compressedToReducedMapping, bccToStStateMapping);
            if (psiStates.getNumberOfSetBits()==0) {
                retResult.values[condition] = 0;
                continue;
            }
            std::vector<uint_fast64_t> newInitalStates;
            auto res = solveOneReachability(newInitalStates, psiStates, transitionMatrix, originalMdp, compressedToReducedMapping, result.oldToNewStateMapping);
            if (newInitalStates.empty()) {
                retResult.values[condition] = 0;
                continue;
            }
            retResult.values[condition] = res.values[newInitalStates[0]];
            resultingProb = res.values[newInitalStates[0]];
            //auto removed = removeTransientSCCs(bccLexArrayCurrent, condition, bcc, compressedToReducedMapping, result.oldToNewStateMapping, res);
            //STORM_PRINT("removed sccs "<<removed<<std::endl);

            auto subsystem = getReducedSubsystem(transitionMatrix, res, newInitalStates, psiStates);
            std::vector<uint_fast64_t> compressedToReducedMappingTemp;
            compressedToReducedMappingTemp.reserve(compressedToReducedMapping.size());
            for (uint_fast64_t newState : subsystem.newToOldStateIndexMapping) {
                compressedToReducedMappingTemp.push_back(compressedToReducedMapping[newState]);
            }
            compressedToReducedMapping = compressedToReducedMappingTemp;
            transitionMatrix = subsystem.model->getTransitionMatrix();
        }
        return retResult;
    }

    template<typename SparseModelType, typename ValueType, bool Nondeterministic>
    std::map<std::string, storm::storage::BitVector> lexicographicModelCheckerHelper<SparseModelType, ValueType, Nondeterministic>::computeApSets(
        std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, CheckFormulaCallback const& formulaChecker) {
        std::map<std::string, storm::storage::BitVector> apSets;
        for (auto& p : extracted) {
            STORM_LOG_DEBUG(" Computing satisfaction set for atomic proposition \"" << p.first << "\" <=> " << *p.second << "...");
            apSets[p.first] = formulaChecker(*p.second);
        }
        return apSets;
    }

    template class lexicographic::lexicographicModelCheckerHelper<storm::models::sparse::Mdp<double>, double, true>;
    template class lexicographic::lexicographicModelCheckerHelper<storm::models::sparse::Mdp<double>, double, false>;
    template class lexicographic::lexicographicModelCheckerHelper<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber, true>;
    template class lexicographic::lexicographicModelCheckerHelper<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber, false>;
    }
        }
        }
}

#pragma clang diagnostic pop