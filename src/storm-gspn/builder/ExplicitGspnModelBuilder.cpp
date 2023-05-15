// #include "storm/builder/ExplicitGspnModelBuilder.h"
//
// #include "storm/models/sparse/StandardRewardModel.h"
//
// #include "storm/utility/macros.h"
// #include "storm/exceptions/NotImplementedException.h"
// #include "storm/storage/expressions/ExpressionManager.h"
// #include "storm-parsers/parser/FormulaParser.h"
// #include "storm/storage/expressions/ExpressionEvaluator.h"
//
//  namespace storm {
//     namespace builder {
//
//         template<typename ValueType>
//         storm::models::sparse::MarkovAutomaton<ValueType> ExplicitGspnModelBuilder<ValueType>::translateGspn(storm::gspn::GSPN const& gspn, std::string
//         const& formula) {
//             // set the given gspn and compute the limits of the net
//             this->gspn = gspn;
//             computeCapacities(gspn);
//
//             // markings maps markings to their corresponding rowgroups (indices)
//             markings = storm::storage::BitVectorHashMap<uint_fast64_t>(numberOfTotalBits, 100);
//             builder = storm::storage::SparseMatrixBuilder<double>(0, 0, 0, false, true);
//
//             // add initial marking to todo list
//             auto bitvector = gspn.getInitialMarking(numberOfBits, numberOfTotalBits)->getBitVector();
//             findOrAddBitvectorToMarkings(*bitvector);
//             currentRowIndex = 0;
//
//             // vector marking markovian states (vector contains an 1 if the state is markovian)
//             storm::storage::BitVector markovianStates(0);
//
//             // vector containing the exit rates for the markovian states
//             std::vector<ValueType> exitRates;
//
//
//             while (!todo.empty()) {
//                 // take next element from the todo list
//                 auto currentBitvector = todo.front();
//                 todo.pop_front();
//                 auto currentMarking = storm::gspn::Marking(gspn.getNumberOfPlaces(), numberOfBits, *currentBitvector);
//
//                 // increment list of states by one
//                 markovianStates.resize(markovianStates.size() + 1, 0);
//
//                 // create new row group for the current marking
//                 builder.newRowGroup(markings.getValue(*currentBitvector));
//
//                 std::cout << "work on: " << *currentBitvector << '\n';
//
//                 auto enabledImmediateTransitions = getEnabledImmediateTransition(currentMarking);
//                 if (!enabledImmediateTransitions.empty()) {
//                     markovianStates.set(currentRowIndex, 0);
//                     exitRates.push_back(0);
//
//                     auto partitions = partitonEnabledImmediateTransitions(currentMarking, enabledImmediateTransitions);
//                     addRowForPartitions(partitions, currentMarking);
//                 } else {
//
//                     auto enabledTimedTransitions = getEnabledTimedTransition(currentMarking);
//                     if (!enabledTimedTransitions.empty()) {
//                         markovianStates.set(currentRowIndex, 1);
//
//                         auto accRate = getAccumulatedRate(enabledTimedTransitions);
//                         std::cout << "\t\tacc. rate: " << accRate << '\n';
//                         exitRates.push_back(accRate);
//
//                         addRowForTimedTransitions(enabledTimedTransitions, currentMarking, accRate);
//                     } else {
//                         markovianStates.set(currentRowIndex, 1);
//                     }
//                 }
//                 ++currentRowIndex;
//             }
//
//             auto matrix = builder.build();
//
//             // create expression manager and add variables from the gspn
//             auto expressionManager = std::make_shared<storm::expressions::ExpressionManager>();
//             for (auto& place : gspn.getPlaces()) {
//                 expressionManager->declareIntegerVariable(place.getName());
//             }
//
//             // parse formula
//             storm::parser::FormulaParser formulaParser(expressionManager);
//             auto formulaPtr = formulaParser.parseSingleFormulaFromString(formula);
//             auto atomicFormulas = formulaPtr->getAtomicExpressionFormulas();
//
//             // create empty state labeling
//             storm::models::sparse::StateLabeling labeling(markings.size());
//             storm::expressions::ExpressionEvaluator<double> expressionEvaluator(*expressionManager);
//
//             std::cout << '\n';
//             std::cout << "build labeling:\n";
//             for (auto& atomicFormula : atomicFormulas) {
//                 std::cout << atomicFormula;
//                 auto label = atomicFormula->toString();
//                 labeling.addLabel(label);
//
//                 for (auto statePair : markings) {
//                     auto marking = storm::gspn::Marking(gspn.getNumberOfPlaces(), numberOfBits, statePair.first);
//                     for (auto& place : gspn.getPlaces()) {
//                         auto variable = expressionManager->getVariable(place.getName());
//                         expressionEvaluator.setIntegerValue(variable, marking.getNumberOfTokensAt(place.getID()));
//                     }
//                     bool hold = expressionEvaluator.asBool(atomicFormula->getExpression());
//                     if (hold) {
//                         labeling.addLabelToState(label, statePair.second);
//                     }
//                 }
//
//             }
//
//             //auto labeling = getStateLabeling();
//
//             return storm::models::sparse::MarkovAutomaton<double>(matrix, labeling, markovianStates, exitRates);
//         }
//
//         template<typename ValueType>
//         void ExplicitGspnModelBuilder<ValueType>::addRowForPartitions(std::vector<std::vector<std::shared_ptr<storm::gspn::ImmediateTransition<double>>>>
//         const& partitions, storm::gspn::Marking const& currentMarking) {
//             for (auto& partition : partitions) {
//                 std::cout << "\tnew partition:\n";
//                 auto accWeight = getAccumulatedWeight(partition);
//                 std::cout << "\t\tacc. weight: " << accWeight << '\n';
//
//                 std::map<uint_fast64_t , double, storm::builder::ExplicitGspnModelBuilder<ValueType>::cmpByIndex> weights;
//                 for (auto& trans : partition) {
//                     std::cout << "\t\ttransname: " << trans.getName() << '\n';
//                     auto newMarking = trans.fire(currentMarking);
//                     std::cout << "\t\t\t target marking: " << *newMarking.getBitVector() << '\n';
//
//                     findOrAddBitvectorToMarkings(*newMarking.getBitVector());
//
//                     auto it = weights.find(markings.getValue(*newMarking.getBitVector()));
//                     double currentWeight = 0;
//                     if (it != weights.end()) {
//                         currentWeight = weights.at(markings.getValue(*newMarking.getBitVector()));
//                     }
//                     currentWeight += trans.getWeight() / accWeight;
//                     weights[markings.getValue(*newMarking.getBitVector())] = currentWeight;
//                 }
//
//                 addValuesToBuilder(weights);
//             }
//         }
//
//         template<typename ValueType>
//         void ExplicitGspnModelBuilder<ValueType>::addRowForTimedTransitions(std::vector<std::shared_ptr<storm::gspn::TimedTransition<double>>> const&
//         enabledTimedTransitions, storm::gspn::Marking const& currentMarking, double const& accRate) {
//             std::map<uint_fast64_t , double, storm::builder::ExplicitGspnModelBuilder<ValueType>::cmpByIndex> rates;
//             for (auto& trans : enabledTimedTransitions) {
//                 std::cout << "\t\ttransname: " << trans.getName() << '\n';
//                 auto newMarking = trans.fire(currentMarking);
//                 std::cout << "\t\t\t target marking: " << *newMarking.getBitVector() << '\n';
//
//                 findOrAddBitvectorToMarkings(*newMarking.getBitVector());
//
//                 auto it = rates.find(markings.getValue(*newMarking.getBitVector()));
//                 double currentWeightRate = 0;
//                 if (it != rates.end()) {
//                     currentWeightRate = rates.at(markings.getValue(*newMarking.getBitVector()));
//                 }
//                 currentWeightRate += trans.getRate() / accRate;
//                 rates[markings.getValue(*newMarking.getBitVector())] = currentWeightRate;
//
//             }
//
//             addValuesToBuilder(rates);
//         }
//
//         template<typename ValueType>
//         void ExplicitGspnModelBuilder<ValueType>::addValuesToBuilder(std::map<uint_fast64_t , double,
//         storm::builder::ExplicitGspnModelBuilder<ValueType>::cmpByIndex> const& values) {
//             for (auto& it : values) {
//                 std::cout << "\t\tadd value \"" << it.second << "\" to " << getBitvector(it.first) << '\n';
//                 builder.addNextValue(currentRowIndex, it.first, it.second);
//             }
//         }
//
//         template<typename ValueType>
//         std::vector<std::vector<std::shared_ptr<storm::gspn::ImmediateTransition<double>>>>
//         ExplicitGspnModelBuilder<ValueType>::partitonEnabledImmediateTransitions(
//                 storm::gspn::Marking const& marking,
//                 std::vector<std::shared_ptr<storm::gspn::ImmediateTransition<double>>> const& enabledImmediateTransitions) {
//             decltype(partitonEnabledImmediateTransitions(marking, enabledImmediateTransitions)) result;
//
//             std::vector<std::shared_ptr<storm::gspn::ImmediateTransition<double>>> weightedTransitions;
//
//             for (auto& trans : enabledImmediateTransitions) {
//                 if (trans.noWeightAttached()) {
//                     decltype(weightedTransitions) singleton;
//                     singleton.push_back(trans);
//                     result.push_back(singleton);
//                 } else {
//                     weightedTransitions.push_back(trans);
//                 }
//             }
//
//             if (weightedTransitions.size() != 0) {
//                 result.push_back(weightedTransitions);
//             }
//
//             return result;
//         }
//
//         template<typename ValueType>
//         double ExplicitGspnModelBuilder<ValueType>::getAccumulatedWeight(std::vector<std::shared_ptr<storm::gspn::ImmediateTransition<double>>> const&
//         vector) const {
//             double result = 0;
//
//             for (auto &trans : vector) {
//                 result += trans.getWeight();
//             }
//
//             return result;
//         }
//
//         template<typename ValueType>
//         void ExplicitGspnModelBuilder<ValueType>::computeCapacities(storm::gspn::GSPN const& gspn) {
//             uint_fast64_t sum = 0;
//             for (auto& place : gspn.getPlaces()) {//TODO
//                 numberOfBits[place.getID()] = 1;
//                 sum += numberOfBits[place.getID()];
//             }
//
//             // compute next multiple of 64
//             uint_fast64_t rem = sum % 64;
//             numberOfTotalBits = sum + 64 - rem;
//         }
//
//         template<typename ValueType>
//         std::vector<std::shared_ptr<storm::gspn::TimedTransition<double>>> ExplicitGspnModelBuilder<ValueType>::getEnabledTimedTransition(
//                 storm::gspn::Marking const& marking) {
//             std::vector<std::shared_ptr<storm::gspn::TimedTransition<double>>>result;
//
//             uint_fast64_t highestSeenPriority = 0;
//
//             for (auto& trans : gspn.getTimedTransitions()) {
//                 if (trans.isEnabled(marking)) {
//                     if (trans.getPriority() > highestSeenPriority) {
//                         highestSeenPriority = trans.getPriority();
//                         result.clear();
//                     }
//                     result.push_back(trans);
//                 }
//             }
//
//             return result;
//         }
//
//         template<typename ValueType>
//         std::vector<std::shared_ptr<storm::gspn::ImmediateTransition<double>>> ExplicitGspnModelBuilder<ValueType>::getEnabledImmediateTransition(
//                 storm::gspn::Marking const& marking) {
//             std::vector<std::shared_ptr<storm::gspn::ImmediateTransition<double>>>result;
//
//             uint_fast64_t highestSeenPriority = 0;
//
//             for (auto& trans : gspn.getImmediateTransitions()) {
//                 if (trans.isEnabled(marking)) {
//                     if (trans.getPriority() > highestSeenPriority) {
//                         highestSeenPriority = trans.getPriority();
//                         result.clear();
//                     }
//                     result.push_back(trans);
//                 }
//             }
//
//             return result;
//         }
//
//         template<typename ValueType>
//         double ExplicitGspnModelBuilder<ValueType>::getAccumulatedRate(std::vector<std::shared_ptr<storm::gspn::TimedTransition<double>>> const& vector) {
//             double result = 0;
//             for (auto trans : vector) {
//                 result += trans.getRate();
//             }
//
//             return result;
//         }
//
//         template<typename ValueType>
//         storm::storage::BitVector ExplicitGspnModelBuilder<ValueType>::getBitvector(uint_fast64_t const& index) {
//             for (auto it = markings.begin(); it != markings.end(); ++it) {
//                 if (std::get<1>(*it) == index) {
//                     return std::get<0>(*it);
//                 }
//             }
//             return storm::storage::BitVector();
//         }
//
//         template<typename ValueType>
//         uint_fast64_t ExplicitGspnModelBuilder<ValueType>::findOrAddBitvectorToMarkings(storm::storage::BitVector const &bitvector) {
//             auto index = markings.findOrAdd(bitvector, nextRowGroupIndex);
//
//             if (index == nextRowGroupIndex) {
//                 // bitvector was not already in the map
//                 ++nextRowGroupIndex;
//                 // bitvector was also never in the todo list
//                 todo.push_back(std::make_shared<storm::storage::BitVector>(bitvector));
//             }
//             return index;
//         }
//
//         template<typename ValueType>
//         storm::models::sparse::StateLabeling ExplicitGspnModelBuilder<ValueType>::getStateLabeling() const {
//             storm::models::sparse::StateLabeling labeling(markings.size());
//
//             std::map<uint_fast64_t , std::string> idToName;
//             for (auto& place : gspn.getPlaces()) {
//                 idToName[place.getID()] = place.getName();
//                 labeling.addLabel(place.getName());
//             }
//
//             auto it = markings.begin();
//             for ( ; it != markings.end() ; ++it) {
//                 auto bitvector = std::get<0>(*it);
//                 storm::gspn::Marking marking(gspn.getNumberOfPlaces(), numberOfBits, bitvector);
//                 for (auto i = 0; i < marking.getNumberOfPlaces(); i++) {
//                     if (marking.getNumberOfTokensAt(i) > 0) {
//                         std::cout << i << '\n';
//                         labeling.addLabelToState(idToName.at(i), i);
//                     }
//                 }
//             }
//
//             return labeling;
//         }
//
//         template class ExplicitGspnModelBuilder<double>;
//     }
// }
