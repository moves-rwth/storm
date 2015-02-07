// Include generated headers.
#include "storm-config.h"

#include <tuple>
#include <sstream>

// Include other headers.
#include "src/exceptions/BaseException.h"
#include "src/utility/macros.h"
#include "src/utility/cli.h"
#include "src/utility/export.h"
#include "src/modelchecker/reachability/CollectConstraints.h"

//#include "src/modelchecker/reachability/DirectEncoding.h"
#include "src/storage/DeterministicModelBisimulationDecomposition.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "src/storage/parameters.h"
#include "src/models/Dtmc.h"

//std::tuple<storm::RationalFunction, boost::optional<storm::storage::SparseMatrix<storm::RationalFunction>>, boost::optional<std::vector<storm::RationalFunction>>, boost::optional<storm::storage::BitVector>, boost::optional<double>, boost::optional<bool>> computeReachabilityProbability(storm::models::Dtmc<storm::RationalFunction> const& dtmc, std::shared_ptr<storm::properties::prctl::PrctlFilter<double>> const& filterFormula) {
//    // The first thing we need to do is to make sure the formula is of the correct form and - if so - extract
//    // the bitvector representation of the atomic propositions.
//    
//    std::shared_ptr<storm::properties::prctl::AbstractStateFormula<double>> stateFormula = std::dynamic_pointer_cast<storm::properties::prctl::AbstractStateFormula<double>>(filterFormula->getChild());
//    std::shared_ptr<storm::properties::prctl::AbstractPathFormula<double>> pathFormula;
//    boost::optional<double> threshold;
//    boost::optional<bool> strict;
//    if (stateFormula != nullptr) {
//        std::shared_ptr<storm::properties::prctl::ProbabilisticBoundOperator<double>> probabilisticBoundFormula = std::dynamic_pointer_cast<storm::properties::prctl::ProbabilisticBoundOperator<double>>(stateFormula);
//        STORM_LOG_THROW(probabilisticBoundFormula != nullptr, storm::exceptions::InvalidPropertyException, "Illegal formula " << *filterFormula << " for parametric model checking. Note that only unbounded reachability properties are permitted.");
//        STORM_LOG_THROW(probabilisticBoundFormula->getComparisonOperator() == storm::properties::ComparisonType::LESS_EQUAL || probabilisticBoundFormula->getComparisonOperator() == storm::properties::ComparisonType::LESS, storm::exceptions::InvalidPropertyException, "Illegal formula " << *filterFormula << " for parametric model checking. Note that only unbounded reachability properties with upper probability bounds are permitted.");
//        
//        threshold = probabilisticBoundFormula->getBound();
//        strict = probabilisticBoundFormula->getComparisonOperator() == storm::properties::ComparisonType::LESS;
//        pathFormula = probabilisticBoundFormula->getChild();
//    } else {
//        pathFormula = std::dynamic_pointer_cast<storm::properties::prctl::AbstractPathFormula<double>>(filterFormula->getChild());
//    }
//    
//    STORM_LOG_THROW(pathFormula != nullptr, storm::exceptions::InvalidPropertyException, "Illegal formula " << *filterFormula << " for parametric model checking. Note that only unbounded reachability properties are permitted.");
//    
//    std::shared_ptr<storm::properties::prctl::Until<double>> untilFormula = std::dynamic_pointer_cast<storm::properties::prctl::Until<double>>(pathFormula);
//    std::shared_ptr<storm::properties::prctl::AbstractStateFormula<double>> phiStateFormula;
//    std::shared_ptr<storm::properties::prctl::AbstractStateFormula<double>> psiStateFormula;
//    if (untilFormula != nullptr) {
//        phiStateFormula = untilFormula->getLeft();
//        psiStateFormula = untilFormula->getRight();
//    } else {
//        std::shared_ptr<storm::properties::prctl::Eventually<double>> eventuallyFormula = std::dynamic_pointer_cast<storm::properties::prctl::Eventually<double>>(pathFormula);
//        STORM_LOG_THROW(eventuallyFormula != nullptr, storm::exceptions::InvalidPropertyException, "Illegal formula " << *filterFormula << " for parametric model checking. Note that only unbounded reachability properties are permitted.");
//        phiStateFormula = std::shared_ptr<storm::properties::prctl::Ap<double>>(new storm::properties::prctl::Ap<double>("true"));
//        psiStateFormula = eventuallyFormula->getChild();
//    }
//    
//    // Now we need to make sure the formulas defining the phi and psi states are just labels.
//    std::shared_ptr<storm::properties::prctl::Ap<double>> phiStateFormulaApFormula = std::dynamic_pointer_cast<storm::properties::prctl::Ap<double>>(phiStateFormula);
//    std::shared_ptr<storm::properties::prctl::Ap<double>> psiStateFormulaApFormula = std::dynamic_pointer_cast<storm::properties::prctl::Ap<double>>(psiStateFormula);
//    STORM_LOG_THROW(phiStateFormulaApFormula.get() != nullptr, storm::exceptions::InvalidPropertyException, "Illegal formula " << *phiStateFormula << " for parametric model checking. Note that only atomic propositions are admitted in that position.");
//    STORM_LOG_THROW(psiStateFormulaApFormula.get() != nullptr, storm::exceptions::InvalidPropertyException, "Illegal formula " << *psiStateFormula << " for parametric model checking. Note that only atomic propositions are admitted in that position.");
//    
//    // Now retrieve the appropriate bitvectors from the atomic propositions.
//    storm::storage::BitVector phiStates = phiStateFormulaApFormula->getAp() != "true" ? dtmc.getLabeledStates(phiStateFormulaApFormula->getAp()) : storm::storage::BitVector(dtmc.getNumberOfStates(), true);
//    storm::storage::BitVector psiStates = dtmc.getLabeledStates(psiStateFormulaApFormula->getAp());
//    
//    // Do some sanity checks to establish some required properties.
//    STORM_LOG_THROW(dtmc.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::IllegalArgumentException, "Input model is required to have exactly one initial state.");
//    
//    // Then, compute the subset of states that has a probability of 0 or 1, respectively.
//    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(dtmc, phiStates, psiStates);
//    storm::storage::BitVector statesWithProbability0 = statesWithProbability01.first;
//    storm::storage::BitVector statesWithProbability1 = statesWithProbability01.second;
//    storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
//    
//    // If the initial state is known to have either probability 0 or 1, we can directly return the result.
//    if (dtmc.getInitialStates().isDisjointFrom(maybeStates)) {
//        STORM_LOG_DEBUG("The probability of all initial states was found in a preprocessing step.");
//        return statesWithProbability0.get(*dtmc.getInitialStates().begin()) ? storm::utility::constantZero<storm::RationalFunction>() : storm::utility::constantOne<storm::RationalFunction>();
//    }
//    
//    // Determine the set of states that is reachable from the initial state without jumping over a target state.
//    storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(dtmc.getTransitionMatrix(), dtmc.getInitialStates(), maybeStates, statesWithProbability1);
//    
//    // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
//    maybeStates &= reachableStates;
//    
//    // Create a vector for the probabilities to go to a state with probability 1 in one step.
//    std::vector<storm::RationalFunction> oneStepProbabilities = dtmc.getTransitionMatrix().getConstrainedRowSumVector(maybeStates, statesWithProbability1);
//    
//    // Determine the set of initial states of the sub-DTMC.
//    storm::storage::BitVector newInitialStates = dtmc.getInitialStates() % maybeStates;
//    
//    // We then build the submatrix that only has the transitions of the maybe states.
//    storm::storage::SparseMatrix<storm::RationalFunction> submatrix = dtmc.getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
//    
//    // To be able to apply heuristics later, we now determine the distance of each state to the initial state.
//    std::vector<std::pair<storm::storage::sparse::state_type, std::size_t>> stateQueue;
//    stateQueue.reserve(submatrix.getRowCount());
//    storm::storage::BitVector statesInQueue(submatrix.getRowCount());
//    std::vector<std::size_t> distances(submatrix.getRowCount());
//    
//    storm::storage::sparse::state_type currentPosition = 0;
//    for (auto const& initialState : newInitialStates) {
//        stateQueue.emplace_back(initialState, 0);
//        statesInQueue.set(initialState);
//    }
//    
//    // Perform a BFS.
//    while (currentPosition < stateQueue.size()) {
//        std::pair<storm::storage::sparse::state_type, std::size_t> const& stateDistancePair = stateQueue[currentPosition];
//        distances[stateDistancePair.first] = stateDistancePair.second;
//        
//        for (auto const& successorEntry : submatrix.getRow(stateDistancePair.first)) {
//            if (!statesInQueue.get(successorEntry.getColumn())) {
//                stateQueue.emplace_back(successorEntry.getColumn(), stateDistancePair.second + 1);
//                statesInQueue.set(successorEntry.getColumn());
//            }
//        }
//        ++currentPosition;
//    }
//    
//    storm::modelchecker::reachability::SparseSccModelChecker<storm::RationalFunction> modelchecker;
//    
//    return std::make_tuple(modelchecker.computeReachabilityProbability(submatrix, oneStepProbabilities, submatrix.transpose(), newInitialStates, phiStates, psiStates, distances),submatrix, oneStepProbabilities, newInitialStates, threshold, strict);
//}

template<typename ValueType>
void printApproximateResult(ValueType const& value) {
    // Intentionally left empty.
}

template<>
void printApproximateResult(storm::RationalFunction const& value) {
    if (value.isConstant()) {
        STORM_PRINT_AND_LOG(" (approximately " << std::setprecision(30) << carl::toDouble(value.constantPart()) << ")" << std::endl);
    }
}

template<typename ValueType>
void check() {
    // From this point on we are ready to carry out the actual computations.
    // Program Translation Time Measurement, Start
    std::chrono::high_resolution_clock::time_point programTranslationStart = std::chrono::high_resolution_clock::now();
    
    // First, we build the model using the given symbolic model description and constant definitions.
    std::string const& programFile = storm::settings::generalSettings().getSymbolicModelFilename();
    std::string const& constants = storm::settings::generalSettings().getConstantDefinitionString();
    storm::prism::Program program = storm::parser::PrismParser::parse(programFile);

    STORM_LOG_THROW(storm::settings::generalSettings().isPropertySet(), storm::exceptions::InvalidSettingsException, "Unable to perform model checking without a property.");
    std::shared_ptr<storm::logic::Formula> formula = storm::parser::FormulaParser(program.getManager().getSharedPointer()).parseFromString(storm::settings::generalSettings().getProperty());
    
    typename storm::builder::ExplicitPrismModelBuilder<ValueType>::Options options(*formula);
    options.addConstantDefinitionsFromString(program, storm::settings::generalSettings().getConstantDefinitionString());
    
    std::shared_ptr<storm::models::AbstractModel<ValueType>> model = storm::builder::ExplicitPrismModelBuilder<ValueType>::translateProgram(program, options);
    
    // Convert the transition rewards to state rewards if necessary.
    if (model->hasTransitionRewards()) {
        model->convertTransitionRewardsToStateRewards();
    }
    
    model->printModelInformationToStream(std::cout);
    
    // Program Translation Time Measurement, End
    std::chrono::high_resolution_clock::time_point programTranslationEnd = std::chrono::high_resolution_clock::now();
    std::cout << "Parsing and translating the model took " << std::chrono::duration_cast<std::chrono::milliseconds>(programTranslationEnd - programTranslationStart).count() << "ms." << std::endl << std::endl;
    
    if (model->hasTransitionRewards()) {
        model->convertTransitionRewardsToStateRewards();
    }
    
    std::shared_ptr<storm::models::Dtmc<ValueType>> dtmc = model->template as<storm::models::Dtmc<ValueType>>();
        
    storm::modelchecker::SparseDtmcEliminationModelChecker<ValueType> modelchecker(*dtmc);
    STORM_LOG_THROW(modelchecker.canHandle(*formula), storm::exceptions::InvalidPropertyException, "Model checker cannot handle the property: '" << *formula << "'.");
    
    std::cout << "Checking formula " << *formula << std::endl;
    
    bool checkRewards = formula->isRewardOperatorFormula();
    
    // Perform bisimulation minimization if requested.
    if (storm::settings::generalSettings().isBisimulationSet()) {
        typename storm::storage::DeterministicModelBisimulationDecomposition<ValueType>::Options options(*dtmc, *formula);
        options.weak = storm::settings::bisimulationSettings().isWeakBisimulationSet();
        
        storm::storage::DeterministicModelBisimulationDecomposition<ValueType> bisimulationDecomposition(*dtmc, options);
        *dtmc = std::move(*bisimulationDecomposition.getQuotient()->template as<storm::models::Dtmc<ValueType>>());

        dtmc->printModelInformationToStream(std::cout);
    }
    
    assert(dtmc);
    
    storm::modelchecker::reachability::CollectConstraints<ValueType> constraintCollector;
    constraintCollector(*dtmc);

    std::unique_ptr<storm::modelchecker::CheckResult> result = modelchecker.check(*formula);
    ValueType valueFunction = result->asExplicitQuantitativeCheckResult<ValueType>()[*model->getInitialStates().begin()];
    
    if (storm::settings::parametricSettings().exportResultToFile()) {
        storm::utility::exportParametricMcResult(valueFunction, constraintCollector);
    }

    // Report the result.
    STORM_PRINT_AND_LOG(std::endl << "Result (initial state): ");
    result->writeToStream(std::cout, model->getInitialStates());
    if (std::is_same<ValueType, storm::RationalFunction>::value) {
        printApproximateResult(valueFunction);
    }
    std::cout << std::endl;
}

/*!
 * Main entry point of the executable storm.
 */
int main(const int argc, const char** argv) {
    try {
        std::chrono::high_resolution_clock::time_point totalTimeStart = std::chrono::high_resolution_clock::now();
        storm::utility::cli::setUp();
        storm::utility::cli::printHeader(argc, argv);
        bool optionsCorrect = storm::utility::cli::parseOptions(argc, argv);
        if (!optionsCorrect) {
            return -1;
        }
        
        check<storm::RationalFunction>();
        std::chrono::high_resolution_clock::time_point totalTimeEnd = std::chrono::high_resolution_clock::now();
        std::cout << std::endl << "Total time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTimeEnd - totalTimeStart).count() << "ms." << std::endl << std::endl;
        
//    // Perform bisimulation minimization if requested.
//    if (storm::settings::generalSettings().isBisimulationSet()) {
//        storm::storage::DeterministicModelStrongBisimulationDecomposition<storm::RationalFunction> bisimulationDecomposition(*dtmc, true);
//        dtmc = bisimulationDecomposition.getQuotient()->as<storm::models::Dtmc<storm::RationalFunction>>();
//        
//        dtmc->printModelInformationToStream(std::cout);
//    }

//    storm::RationalFunction valueFunction2 = modelchecker.computeReachabilityProbability(*dtmc, filterFormula);
//    STORM_PRINT_AND_LOG(std::endl << "computed value2 " << valueFunction2 << std::endl);
//
//    storm::RationalFunction diff = storm::utility::simplify(valueFunction - valueFunction2);
//    STORM_PRINT_AND_LOG(std::endl << "difference: " << diff << std::endl);
    
        // Get variables from parameter definitions in prism program.
//        std::set<storm::Variable> parameters;
//        for(auto constant : program.getConstants())
//        {
//            if(!constant.isDefined())
//            {
//                std::cout << "got undef constant " << constant.getName() << std::endl;
//                carl::Variable p = carl::VariablePool::getInstance().findVariableWithName(constant.getName());
//                assert(p != storm::Variable::NO_VARIABLE);
//                parameters.insert(p);
//            }
//        }
    
//        STORM_LOG_ASSERT(parameters == valueFunction.gatherVariables(), "Parameters in result and program definition do not coincide.");
        
//        if(storm::settings::parametricSettings().exportResultToFile()) {
//            storm::utility::exportParametricMcResult(valueFunction, constraintCollector);
//        }
        
//        if (storm::settings::parametricSettings().exportToSmt2File() && std::get<1>(result) && std::get<2>(result) && std::get<3>(result) && std::get<4>(result) && std::get<5>(result)) {
//            storm::modelchecker::reachability::DirectEncoding dec;
//            storm::utility::exportStringStreamToFile(dec.encodeAsSmt2(std::get<1>(result).get(), std::get<2>(result).get(), parameters, std::get<3>(result).get(), carl::rationalize<storm::RationalFunction::CoeffType>(std::get<4>(result).get()), std::get<5>(result).get()), "out.smt");
//        }
    
        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cli::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM to terminate. The message of the exception is: " << exception.what());
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM to terminate. The message of this exception is: " << exception.what());
    }
}

//#include <memory>
//#include <stdint.h>
//
//#include "stormParametric.h"
//#include "adapters/ExplicitModelAdapter.h"
//#include "utility/graph.h"
//#include "modelchecker/reachability/DirectEncoding.h"
//#include "storage/BitVector.h"
//#include "storage/DeterministicTransition.h"
//
//using storm::storage::StateId;
//
//namespace storm
//{
//
//
//void ParametricStormEntryPoint::createModel()
//{
//    mModel = storm::adapters::ExplicitModelAdapter<RationalFunction>::translateProgram(mProgram, mConstants);
//    mModel->printModelInformationToStream(std::cout);
//}
//
//std::string ParametricStormEntryPoint::reachabilityToSmt2(std::string const& label)
//{
//
//    storm::storage::BitVector phiStates(mModel->getNumberOfStates(), true);
//    storm::storage::BitVector initStates = mModel->getInitialStates();
//    storm::storage::BitVector targetStates = mModel->getLabeledStates(label);
//
//    std::shared_ptr<models::Dtmc<RationalFunction>> dtmc = mModel->as<models::Dtmc<RationalFunction>>();
//    // 1. make target states absorbing.
//    dtmc->makeAbsorbing(targetStates);
//    // 2. throw away anything which does not add to the reachability probability.
//    // 2a. remove non productive states
//    storm::storage::BitVector productiveStates = utility::graph::performProbGreater0(*dtmc, dtmc->getBackwardTransitions(), phiStates, targetStates);
//    // 2b. calculate set of states wich
//    storm::storage::BitVector almostSurelyReachingTargetStates = ~utility::graph::performProbGreater0(*dtmc, dtmc->getBackwardTransitions(), phiStates, ~productiveStates);
//    // 2c. Make such states also target states.
//    dtmc->makeAbsorbing(almostSurelyReachingTargetStates);
//    // 2d. throw away non reachable states
//    storm::storage::BitVector reachableStates = utility::graph::performProbGreater0(*dtmc, dtmc->getTransitionMatrix(), phiStates, initStates);
//    storm::storage::BitVector bv = productiveStates & reachableStates;
//    dtmc->getStateLabeling().addAtomicProposition("__targets__", targetStates | almostSurelyReachingTargetStates);
//    models::Dtmc<RationalFunction> subdtmc = dtmc->getSubDtmc(bv);
//
//    phiStates = storm::storage::BitVector(subdtmc.getNumberOfStates(), true);
//    initStates = subdtmc.getInitialStates();
//    targetStates = subdtmc.getLabeledStates("__targets__");
//    storm::storage::BitVector deadlockStates(phiStates);
//    deadlockStates.set(subdtmc.getNumberOfStates()-1,false);
//
//    // Search for states with only one non-deadlock successor.
//    std::map<StateId, storage::DeterministicTransition<RationalFunction>> chainedStates;
//    StateId nrStates = subdtmc.getNumberOfStates();
//    StateId deadlockState = nrStates - 1;
//    for(StateId source = 0; source < nrStates - 1; ++source)
//    {
//        if(targetStates[source])
//        {
//            continue;
//        }
//        storage::DeterministicTransition<RationalFunction> productiveTransition(nrStates);
//        for(auto const& transition : subdtmc.getRows(source))
//        {
//            if(productiveTransition.targetState() == nrStates)
//            {
//                // first transition.
//                productiveTransition = transition;
//            }
//            else
//            {
//                // second transition
//                if(transition.first != deadlockState)
//                {
//                    productiveTransition.targetState() = nrStates;
//                    break;
//                }
//            }
//        }
//        if(productiveTransition.targetState() != nrStates)
//        {
//            chainedStates.emplace(source, productiveTransition);
//        }
//    }
//    storage::BitVector eliminatedStates(nrStates, false);
//    for(auto & chainedState : chainedStates)
//    {
//        assert(chainedState.first != chainedState.second.targetState());
//        auto it = chainedStates.find(chainedState.second.targetState());
//        if(it != chainedStates.end())
//        {
//            //std::cout << "----------------------------" << std::endl;
//            //std::cout << chainedState.first << " -- " << chainedState.second.probability() << " --> " << chainedState.second.targetState() << std::endl;
//            //std::cout << it->first << " -- " << it->second.probability() << " --> " << it->second.targetState() << std::endl;
//            chainedState.second.targetState() = it->second.targetState();
//            chainedState.second.probability() *= it->second.probability();
//            //std::cout << chainedState.first << " -- " << chainedState.second.probability() << " --> " << chainedState.second.targetState() << std::endl;
//            //std::cout << "----------------------------" << std::endl;
//            chainedStates.erase(it);
//            eliminatedStates.set(it->first, true);
//        }
//    }
//
//
//    for(auto chainedState : chainedStates)
//    {
//        if(!eliminatedStates[chainedState.first])
//        {
//            std::cout << chainedState.first << " -- " << chainedState.second.probability() << " --> " << chainedState.second.targetState() << std::endl;
//        }
//    }
//
//    storage::StronglyConnectedComponentDecomposition<RationalFunction> sccs(subdtmc);
//    std::cout << sccs << std::endl;
//
//    modelchecker::reachability::DirectEncoding dec;
//    std::vector<carl::Variable> parameters;

//    return dec.encodeAsSmt2(subdtmc, parameters, subdtmc.getLabeledStates("init"), subdtmc.getLabeledStates("__targets__"), mpq_class(1,2));
//
//}
//
//
//void storm_parametric(const std::string& constants, const storm::prism::Program& program)
//{
//    ParametricStormEntryPoint entry(constants, program);
//    entry.createModel();
//    storm::settings::Settings* s = storm::settings::Settings::getInstance();
//    if(s->isSet("reachability"))
//    {
//        std::ofstream fstream("test.smt2");
//        fstream << entry.reachabilityToSmt2(s->getOptionByLongName("reachability").getArgument(0).getValueAsString());
//        fstream.close();
//    }
//
//
//}
//
//}
