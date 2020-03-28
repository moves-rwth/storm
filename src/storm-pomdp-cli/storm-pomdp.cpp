#include "storm/utility/initialize.h"

#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm-pomdp-cli/settings/modules/POMDPSettings.h"
#include "storm-pomdp-cli/settings/modules/GridApproximationSettings.h"
#include "storm-pomdp-cli/settings/PomdpSettings.h"
#include "storm/analysis/GraphConditions.h"

#include "storm-cli-utilities/cli.h"
#include "storm-cli-utilities/model-handling.h"

#include "storm-pomdp/transformer/KnownProbabilityTransformer.h"
#include "storm-pomdp/transformer/ApplyFiniteSchedulerToPomdp.h"
#include "storm-pomdp/transformer/GlobalPOMDPSelfLoopEliminator.h"
#include "storm-pomdp/transformer/GlobalPomdpMecChoiceEliminator.h"
#include "storm-pomdp/transformer/PomdpMemoryUnfolder.h"
#include "storm-pomdp/transformer/BinaryPomdpTransformer.h"
#include "storm-pomdp/transformer/MakePOMDPCanonic.h"
#include "storm-pomdp/analysis/UniqueObservationStates.h"
#include "storm-pomdp/analysis/QualitativeAnalysis.h"
#include "storm-pomdp/modelchecker/ApproximatePOMDPModelchecker.h"
#include "storm-pomdp/analysis/FormulaInformation.h"
#include "storm-pomdp/analysis/MemlessStrategySearchQualitative.h"
#include "storm-pomdp/analysis/QualitativeStrategySearchNaive.h"
//
//template<typename ValueType>
//bool extractTargetAndSinkObservationSets(std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> const& pomdp, storm::logic::Formula const& subformula, std::set<uint32_t>& targetObservationSet, storm::storage::BitVector&  targetStates, storm::storage::BitVector&  badStates) {
//    //TODO refactor (use model checker to determine the states, then transform into observations).
//    //TODO rename into appropriate function name.
//    bool validFormula = false;
//    if (subformula.isEventuallyFormula()) {
//        storm::logic::EventuallyFormula const &eventuallyFormula = subformula.asEventuallyFormula();
//        storm::logic::Formula const &subformula2 = eventuallyFormula.getSubformula();
//        if (subformula2.isAtomicLabelFormula()) {
//            storm::logic::AtomicLabelFormula const &alFormula = subformula2.asAtomicLabelFormula();
//            validFormula = true;
//            std::string targetLabel = alFormula.getLabel();
//            auto labeling = pomdp->getStateLabeling();
//            for (size_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
//                if (labeling.getStateHasLabel(targetLabel, state)) {
//                    targetObservationSet.insert(pomdp->getObservation(state));
//                    targetStates.set(state);
//                }
//            }
//        } else if (subformula2.isAtomicExpressionFormula()) {
//            validFormula = true;
//            std::stringstream stream;
//            stream << subformula2.asAtomicExpressionFormula().getExpression();
//            storm::logic::AtomicLabelFormula formula3 = storm::logic::AtomicLabelFormula(stream.str());
//            std::string targetLabel = formula3.getLabel();
//            auto labeling = pomdp->getStateLabeling();
//            for (size_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
//                if (labeling.getStateHasLabel(targetLabel, state)) {
//                    targetObservationSet.insert(pomdp->getObservation(state));
//                    targetStates.set(state);
//                }
//            }
//        }
//    } else if (subformula.isUntilFormula()) {
//        storm::logic::UntilFormula const &untilFormula = subformula.asUntilFormula();
//        storm::logic::Formula const &subformula1 = untilFormula.getLeftSubformula();
//        if (subformula1.isAtomicLabelFormula()) {
//            storm::logic::AtomicLabelFormula const &alFormula = subformula1.asAtomicLabelFormula();
//            std::string targetLabel = alFormula.getLabel();
//            auto labeling = pomdp->getStateLabeling();
//            for (size_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
//                if (!labeling.getStateHasLabel(targetLabel, state)) {
//                    badStates.set(state);
//                }
//            }
//        } else if (subformula1.isAtomicExpressionFormula()) {
//            std::stringstream stream;
//            stream << subformula1.asAtomicExpressionFormula().getExpression();
//            storm::logic::AtomicLabelFormula formula3 = storm::logic::AtomicLabelFormula(stream.str());
//            std::string targetLabel = formula3.getLabel();
//            auto labeling = pomdp->getStateLabeling();
//            for (size_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
//                if (!labeling.getStateHasLabel(targetLabel, state)) {
//                    badStates.set(state);
//                }
//            }
//        } else {
//            return false;
//        }
//        storm::logic::Formula const &subformula2 = untilFormula.getRightSubformula();
//        if (subformula2.isAtomicLabelFormula()) {
//            storm::logic::AtomicLabelFormula const &alFormula = subformula2.asAtomicLabelFormula();
//            validFormula = true;
//            std::string targetLabel = alFormula.getLabel();
//            auto labeling = pomdp->getStateLabeling();
//            for (size_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
//                if (labeling.getStateHasLabel(targetLabel, state)) {
//                    targetObservationSet.insert(pomdp->getObservation(state));
//                    targetStates.set(state);
//                }
//
//            }
//        } else if (subformula2.isAtomicExpressionFormula()) {
//            validFormula = true;
//            std::stringstream stream;
//            stream << subformula2.asAtomicExpressionFormula().getExpression();
//            storm::logic::AtomicLabelFormula formula3 = storm::logic::AtomicLabelFormula(stream.str());
//            std::string targetLabel = formula3.getLabel();
//            auto labeling = pomdp->getStateLabeling();
//            for (size_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
//                if (labeling.getStateHasLabel(targetLabel, state)) {
//                    targetObservationSet.insert(pomdp->getObservation(state));
//                    targetStates.set(state);
//                }
//
//            }
//        }
//    }
//    return validFormula;
//}
//
//template<typename ValueType>
//std::set<uint32_t> extractObservations(storm::models::sparse::Pomdp<ValueType> const& pomdp, storm::storage::BitVector const& states) {
//    std::set<uint32_t> observations;
//    for(auto state : states) {
//        observations.insert(pomdp.getObservation(state));
//    }
//    return observations;
//}
//
///*!
// * Entry point for the pomdp backend.
// *
// * @param argc The argc argument of main().
// * @param argv The argv argument of main().
// * @return Return code, 0 if successfull, not 0 otherwise.
// */
//int main(const int argc, const char** argv) {
//    //try {
//        storm::utility::setUp();
//        storm::cli::printHeader("Storm-pomdp", argc, argv);
//        initializeSettings();
//
//        bool optionsCorrect = storm::cli::parseOptions(argc, argv);
//        if (!optionsCorrect) {
//            return -1;
//        }
//        storm::cli::setUrgentOptions();
//
//        auto const& coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
//        auto const& pomdpSettings = storm::settings::getModule<storm::settings::modules::POMDPSettings>();
//        auto const& ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
//        auto const &general = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
//        auto const &debug = storm::settings::getModule<storm::settings::modules::DebugSettings>();
//        auto const& pomdpQualSettings = storm::settings::getModule<storm::settings::modules::QualitativePOMDPAnalysisSettings>();
//
//        if (general.isVerboseSet()) {
//            storm::utility::setLogLevel(l3pp::LogLevel::INFO);
//        }
//        if (debug.isDebugSet()) {
//            storm::utility::setLogLevel(l3pp::LogLevel::DEBUG);
//        }
//        if (debug.isTraceSet()) {
//            storm::utility::setLogLevel(l3pp::LogLevel::TRACE);
//        }
//        if (debug.isLogfileSet()) {
//            storm::utility::initializeFileLogging();
//        }
//
//        // For several engines, no model building step is performed, but the verification is started right away.
//        storm::settings::modules::CoreSettings::Engine engine = coreSettings.getEngine();
//
//        storm::cli::SymbolicInput symbolicInput = storm::cli::parseAndPreprocessSymbolicInput();
//        // We should not export here if we are going to do some processing first.
//        auto model = storm::cli::buildPreprocessExportModelWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, double>(symbolicInput, engine);
//        STORM_LOG_THROW(model && model->getType() == storm::models::ModelType::Pomdp, storm::exceptions::WrongFormatException, "Expected a POMDP.");
//        std::shared_ptr<storm::models::sparse::Pomdp<storm::RationalNumber>> pomdp = model->template as<storm::models::sparse::Pomdp<storm::RationalNumber>>();
//        storm::transformer::MakePOMDPCanonic<storm::RationalNumber> makeCanonic(*pomdp);
//        pomdp = makeCanonic.transform();
//
//        std::shared_ptr<storm::logic::Formula const> formula;
//        if (!symbolicInput.properties.empty()) {
//            formula = symbolicInput.properties.front().getRawFormula();
//            STORM_PRINT_AND_LOG("Analyzing property '" << *formula << "'" << std::endl);
//            STORM_LOG_WARN_COND(symbolicInput.properties.size() == 1, "There is currently no support for multiple properties. All other properties will be ignored.");
//        }
//
//        if (pomdpSettings.isAnalyzeUniqueObservationsSet()) {
//            STORM_PRINT_AND_LOG("Analyzing states with unique observation ..." << std::endl);
//            storm::analysis::UniqueObservationStates<double> uniqueAnalysis(*pomdp);
//            std::cout << uniqueAnalysis.analyse() << std::endl;
//        }
//
//        if (formula) {
//            storm::logic::ProbabilityOperatorFormula const &probFormula = formula->asProbabilityOperatorFormula();
//            storm::logic::Formula const &subformula1 = probFormula.getSubformula();
//
//
//            if (formula->isProbabilityOperatorFormula()) {
//                boost::optional<storm::storage::BitVector> prob1States;
//                boost::optional<storm::storage::BitVector> prob0States;
//                if (pomdpSettings.isSelfloopReductionSet() && !storm::solver::minimize(formula->asProbabilityOperatorFormula().getOptimalityType())) {
//                    STORM_PRINT_AND_LOG("Eliminating self-loop choices ...");
//                    uint64_t oldChoiceCount = pomdp->getNumberOfChoices();
//                    storm::transformer::GlobalPOMDPSelfLoopEliminator<double> selfLoopEliminator(*pomdp);
//                    pomdp = selfLoopEliminator.transform();
//                    STORM_PRINT_AND_LOG(oldChoiceCount - pomdp->getNumberOfChoices() << " choices eliminated through self-loop elimination." << std::endl);
//=======

#include "storm/api/storm.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/Stopwatch.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/NumberTraits.h"

#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/NotSupportedException.h"

#include <typeinfo>

namespace storm {
    namespace pomdp {
        namespace cli {
            
            /// Perform preprocessings based on the graph structure (if requested or necessary). Return true, if some preprocessing has been done
            template<typename ValueType, storm::dd::DdType DdType>
            bool performPreprocessing(std::shared_ptr<storm::models::sparse::Pomdp<ValueType>>& pomdp, storm::pomdp::analysis::FormulaInformation& formulaInfo, storm::logic::Formula const& formula) {
                auto const& pomdpSettings = storm::settings::getModule<storm::settings::modules::POMDPSettings>();
                bool preprocessingPerformed = false;
                if (pomdpSettings.isSelfloopReductionSet()) {
                    bool apply = formulaInfo.isNonNestedReachabilityProbability() && formulaInfo.maximize();
                    apply = apply || (formulaInfo.isNonNestedExpectedRewardFormula() && formulaInfo.minimize());
                    if (apply) {
                        STORM_PRINT_AND_LOG("Eliminating self-loop choices ...");
                        uint64_t oldChoiceCount = pomdp->getNumberOfChoices();
                        storm::transformer::GlobalPOMDPSelfLoopEliminator<ValueType> selfLoopEliminator(*pomdp);
                        pomdp = selfLoopEliminator.transform();
                        STORM_PRINT_AND_LOG(oldChoiceCount - pomdp->getNumberOfChoices() << " choices eliminated through self-loop elimination." << std::endl);
                        preprocessingPerformed = true;
                    }
>>>>>>> prism-pomdp
                }
                if (pomdpSettings.isQualitativeReductionSet() && formulaInfo.isNonNestedReachabilityProbability()) {
                    storm::analysis::QualitativeAnalysis<ValueType> qualitativeAnalysis(*pomdp);
                    STORM_PRINT_AND_LOG("Computing states with probability 0 ...");
                    storm::storage::BitVector prob0States = qualitativeAnalysis.analyseProb0(formula.asProbabilityOperatorFormula());
                    std::cout << prob0States << std::endl;
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    STORM_PRINT_AND_LOG("Computing states with probability 1 ...");
                    storm::storage::BitVector  prob1States = qualitativeAnalysis.analyseProb1(formula.asProbabilityOperatorFormula());
                    std::cout << prob1States << std::endl;
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    storm::pomdp::transformer::KnownProbabilityTransformer<ValueType> kpt = storm::pomdp::transformer::KnownProbabilityTransformer<ValueType>();
                    pomdp = kpt.transform(*pomdp, prob0States, prob1States);
                    // Update formulaInfo to changes from Preprocessing
                    formulaInfo.updateTargetStates(*pomdp, std::move(prob1States));
                    formulaInfo.updateSinkStates(*pomdp, std::move(prob0States));
                    preprocessingPerformed = true;
                }
                return preprocessingPerformed;
            }
            
            template<typename ValueType>
            void printResult(ValueType const& lowerBound, ValueType const& upperBound) {
                if (lowerBound == upperBound) {
                    STORM_PRINT_AND_LOG(lowerBound);
                } else {
                    STORM_PRINT_AND_LOG("[" << lowerBound << ", " << upperBound << "] (width=" << ValueType(upperBound - lowerBound) << ")");
                }
                if (storm::NumberTraits<ValueType>::IsExact) {
                    STORM_PRINT_AND_LOG(" (approx. ");
                    printResult(storm::utility::convertNumber<double>(lowerBound), storm::utility::convertNumber<double>(upperBound));
                    STORM_PRINT_AND_LOG(")");
                }
            }
            
            template<typename ValueType, storm::dd::DdType DdType>
            bool performAnalysis(std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> const& pomdp, storm::pomdp::analysis::FormulaInformation const& formulaInfo, storm::logic::Formula const& formula) {
                auto const& pomdpSettings = storm::settings::getModule<storm::settings::modules::POMDPSettings>();
                bool analysisPerformed = false;
                if (pomdpSettings.isGridApproximationSet()) {
                    STORM_PRINT_AND_LOG("Applying grid approximation... ");
                    auto const& gridSettings = storm::settings::getModule<storm::settings::modules::GridApproximationSettings>();
                    typename storm::pomdp::modelchecker::ApproximatePOMDPModelchecker<ValueType>::Options options;
                    options.initialGridResolution = gridSettings.getGridResolution();
                    options.explorationThreshold = storm::utility::convertNumber<ValueType>(gridSettings.getExplorationThreshold());
                    options.doRefinement = gridSettings.isRefineSet();
                    options.refinementPrecision = storm::utility::convertNumber<ValueType>(gridSettings.getRefinementPrecision());
                    options.numericPrecision = storm::utility::convertNumber<ValueType>(gridSettings.getNumericPrecision());
                    options.cacheSubsimplices = gridSettings.isCacheSimplicesSet();
                    if (storm::NumberTraits<ValueType>::IsExact) {
                        if (gridSettings.isNumericPrecisionSetFromDefault()) {
                            STORM_LOG_WARN_COND(storm::utility::isZero(options.numericPrecision), "Setting numeric precision to zero because exact arithmethic is used.");
                            options.numericPrecision = storm::utility::zero<ValueType>();
                        } else {
                            STORM_LOG_WARN_COND(storm::utility::isZero(options.numericPrecision), "A non-zero numeric precision was set although exact arithmethic is used. Results might be inexact.");
                        }
                    }
                    storm::pomdp::modelchecker::ApproximatePOMDPModelchecker<ValueType> checker = storm::pomdp::modelchecker::ApproximatePOMDPModelchecker<ValueType>(*pomdp, options);
                    std::unique_ptr<storm::pomdp::modelchecker::POMDPCheckResult<ValueType>> result = checker.check(formula);
                    checker.printStatisticsToStream(std::cout);
                    if (result) {
                        if (storm::utility::resources::isTerminate()) {
                            STORM_PRINT_AND_LOG("\nResult till abort: ")
                        } else {
                            STORM_PRINT_AND_LOG("\nResult: ")
                        }
                        printResult(result->underApproxValue, result->overApproxValue);
                        STORM_PRINT_AND_LOG(std::endl);
                    } else {
                        STORM_PRINT_AND_LOG("\nResult: Not available." << std::endl);
                    }
                    analysisPerformed = true;
                }
                if (pomdpSettings.isMemlessSearchSet()) {
                    STORM_LOG_THROW(formulaInfo.isNonNestedReachabilityProbability(), storm::exceptions::NotSupportedException, "Qualitative memoryless scheduler search is not implemented for this property type.");

                    storm::analysis::QualitativeAnalysis<double> qualitativeAnalysis(*pomdp);
                    // After preprocessing, this might be done cheaper.
                    storm::storage::BitVector targetStates = qualitativeAnalysis.analyseProb1(formula->asProbabilityOperatorFormula());
                    storm::storage::BitVector surelyNotAlmostSurelyReachTarget = qualitativeAnalysis.analyseProbSmaller1(formula->asProbabilityOperatorFormula());
                    std::set<uint32_t> targetObservationSet = extractObservations(*pomdp, targetStates);



    //                    std::cout << std::endl;
    //                    pomdp->writeDotToStream(std::cout);
    //                    std::cout << std::endl;
    //                    std::cout << std::endl;

                    storm::expressions::ExpressionManager expressionManager;
                    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::Z3SmtSolverFactory>();

                    uint64_t lookahead = pomdpQualSettings.getLookahead();
                    if (lookahead == 0) {
                        lookahead = pomdp->getNumberOfStates();
                    }
                    storm::pomdp::MemlessSearchOptions options;

                    options.onlyDeterministicStrategies = pomdpQualSettings.isOnlyDeterministicSet();
                    uint64_t loglevel = 0;
                    // TODO a big ugly, but we have our own loglevels.
                    if(storm::utility::getLogLevel() == l3pp::LogLevel::INFO) {
                        loglevel = 1;
                    }
                    else if(storm::utility::getLogLevel() == l3pp::LogLevel::DEBUG) {
                        loglevel = 2;
                    }
                    else if(storm::utility::getLogLevel() == l3pp::LogLevel::TRACE) {
                        loglevel = 3;
                    }
                    options.setDebugLevel(loglevel);

                    if (pomdpQualSettings.isExportSATCallsSet()) {
                        options.setExportSATCalls(pomdpQualSettings.getExportSATCallsPath());
                    }

                    if (storm::utility::graph::checkIfECWithChoiceExists(pomdp->getTransitionMatrix(), pomdp->getBackwardTransitions(), ~targetStates & ~surelyNotAlmostSurelyReachTarget, storm::storage::BitVector(pomdp->getNumberOfChoices(), true))) {
                        options.lookaheadRequired = true;
                        STORM_LOG_DEBUG("Lookahead required.");
                    } else {
                        options.lookaheadRequired = false;
                        STORM_LOG_DEBUG("No lookahead required.");
                    }


                    if (pomdpSettings.getMemlessSearchMethod() == "ccd16memless") {
                        storm::pomdp::QualitativeStrategySearchNaive<double> memlessSearch(*pomdp, targetObservationSet, targetStates, surelyNotAlmostSurelyReachTarget, smtSolverFactory);
                        memlessSearch.findNewStrategyForSomeState(lookahead);
                    } else if (pomdpSettings.getMemlessSearchMethod() == "iterative") {
                        storm::pomdp::MemlessStrategySearchQualitative<double> memlessSearch(*pomdp, targetObservationSet, targetStates, surelyNotAlmostSurelyReachTarget, smtSolverFactory, options);
                        memlessSearch.findNewStrategyForSomeState(lookahead);
                        memlessSearch.finalizeStatistics();
                        memlessSearch.getStatistics().print();
                    } else {
                        STORM_LOG_ERROR("This method is not implemented.");
                    }
                    analysisPerformed = true;
                }
                if (pomdpSettings.isCheckFullyObservableSet()) {
                    STORM_PRINT_AND_LOG("Analyzing the formula on the fully observable MDP ... ");
                    auto resultPtr = storm::api::verifyWithSparseEngine<ValueType>(pomdp->template as<storm::models::sparse::Mdp<ValueType>>(), storm::api::createTask<ValueType>(formula.asSharedPointer(), true));
                    if (resultPtr) {
                        auto result = resultPtr->template asExplicitQuantitativeCheckResult<ValueType>();
                        result.filter(storm::modelchecker::ExplicitQualitativeCheckResult(pomdp->getInitialStates()));
                        if (storm::utility::resources::isTerminate()) {
                            STORM_PRINT_AND_LOG("\nResult till abort: ")
                        } else {
                            STORM_PRINT_AND_LOG("\nResult: ")
                        }
                        printResult(result.getMin(), result.getMax());
                        STORM_PRINT_AND_LOG(std::endl);
                    } else {
                        STORM_PRINT_AND_LOG("\nResult: Not available." << std::endl);
                    }
                    analysisPerformed = true;
                }
                return analysisPerformed;
            }
            
            
            template<typename ValueType, storm::dd::DdType DdType>
            bool performTransformation(std::shared_ptr<storm::models::sparse::Pomdp<ValueType>>& pomdp, storm::logic::Formula const& formula) {
                auto const& pomdpSettings = storm::settings::getModule<storm::settings::modules::POMDPSettings>();
                bool transformationPerformed = false;
                bool memoryUnfolded = false;
                if (pomdpSettings.getMemoryBound() > 1) {
                    STORM_PRINT_AND_LOG("Computing the unfolding for memory bound " << pomdpSettings.getMemoryBound() << " and memory pattern '" << storm::storage::toString(pomdpSettings.getMemoryPattern()) << "' ...");
                    storm::storage::PomdpMemory memory = storm::storage::PomdpMemoryBuilder().build(pomdpSettings.getMemoryPattern(), pomdpSettings.getMemoryBound());
                    std::cout << memory.toString() << std::endl;
                    storm::transformer::PomdpMemoryUnfolder<ValueType> memoryUnfolder(*pomdp, memory);
                    pomdp = memoryUnfolder.transform();
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    pomdp->printModelInformationToStream(std::cout);
                    transformationPerformed = true;
                    memoryUnfolded = true;
                }
        
                // From now on the pomdp is considered memoryless
        
                if (pomdpSettings.isMecReductionSet()) {
                    STORM_PRINT_AND_LOG("Eliminating mec choices ...");
                    // Note: Elimination of mec choices only preserves memoryless schedulers.
                    uint64_t oldChoiceCount = pomdp->getNumberOfChoices();
                    storm::transformer::GlobalPomdpMecChoiceEliminator<ValueType> mecChoiceEliminator(*pomdp);
                    pomdp = mecChoiceEliminator.transform(formula);
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    STORM_PRINT_AND_LOG(oldChoiceCount - pomdp->getNumberOfChoices() << " choices eliminated through MEC choice elimination." << std::endl);
                    pomdp->printModelInformationToStream(std::cout);
                    transformationPerformed = true;
                }
        
                if (pomdpSettings.isTransformBinarySet() || pomdpSettings.isTransformSimpleSet()) {
                    if (pomdpSettings.isTransformSimpleSet()) {
                        STORM_PRINT_AND_LOG("Transforming the POMDP to a simple POMDP.");
                        pomdp = storm::transformer::BinaryPomdpTransformer<ValueType>().transform(*pomdp, true);
                    } else {
                        STORM_PRINT_AND_LOG("Transforming the POMDP to a binary POMDP.");
                        pomdp = storm::transformer::BinaryPomdpTransformer<ValueType>().transform(*pomdp, false);
                    }
                    pomdp->printModelInformationToStream(std::cout);
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    transformationPerformed = true;
                }
        
                if (pomdpSettings.isExportToParametricSet()) {
                    STORM_PRINT_AND_LOG("Transforming memoryless POMDP to pMC...");
                    storm::transformer::ApplyFiniteSchedulerToPomdp<ValueType> toPMCTransformer(*pomdp);
                    std::string transformMode = pomdpSettings.getFscApplicationTypeString();
                    auto pmc = toPMCTransformer.transform(storm::transformer::parsePomdpFscApplicationMode(transformMode));
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    pmc->printModelInformationToStream(std::cout);
                    STORM_PRINT_AND_LOG("Simplifying pMC...");
                    //if (generalSettings.isBisimulationSet()) {
                    pmc = storm::api::performBisimulationMinimization<storm::RationalFunction>(pmc->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>(),{formula.asSharedPointer()}, storm::storage::BisimulationType::Strong)->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
        
                    //}
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    pmc->printModelInformationToStream(std::cout);
                    STORM_PRINT_AND_LOG("Exporting pMC...");
                    storm::analysis::ConstraintCollector<storm::RationalFunction> constraints(*pmc);
                    auto const& parameterSet = constraints.getVariables();
                    std::vector<storm::RationalFunctionVariable> parameters(parameterSet.begin(), parameterSet.end());
                    std::vector<std::string> parameterNames;
                    for (auto const& parameter : parameters) {
                        parameterNames.push_back(parameter.name());
                    }
                    storm::api::exportSparseModelAsDrn(pmc, pomdpSettings.getExportToParametricFilename(), parameterNames);
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    transformationPerformed = true;
                }
                if (transformationPerformed && !memoryUnfolded) {
                    STORM_PRINT_AND_LOG("Implicitly assumed restriction to memoryless schedulers for at least one transformation." << std::endl);
                }
                return transformationPerformed;
            }
            
            template<typename ValueType, storm::dd::DdType DdType>
            void processOptionsWithValueTypeAndDdLib(storm::cli::SymbolicInput const& symbolicInput, storm::cli::ModelProcessingInformation const& mpi) {
                auto const& pomdpSettings = storm::settings::getModule<storm::settings::modules::POMDPSettings>();
                
                auto model = storm::cli::buildPreprocessExportModelWithValueTypeAndDdlib<DdType, ValueType>(symbolicInput, mpi);
                if (!model) {
                    STORM_PRINT_AND_LOG("No input model given." << std::endl);
                    return;
                }
                STORM_LOG_THROW(model->getType() == storm::models::ModelType::Pomdp && model->isSparseModel(), storm::exceptions::WrongFormatException, "Expected a POMDP in sparse representation.");
            
                std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> pomdp = model->template as<storm::models::sparse::Pomdp<ValueType>>();
                storm::transformer::MakePOMDPCanonic<ValueType> makeCanonic(*pomdp);
                pomdp = makeCanonic.transform();

//                if (ioSettings.isExportDotSet()) {
//                    std::shared_ptr<storm::models::sparse::Model<double>> sparseModel = pomdp;
//                    storm::api::exportSparseModelAsDot(sparseModel, ioSettings.getExportDotFilename(), ioSettings.getExportDotMaxWidth());
//                }
//                if (ioSettings.isExportExplicitSet()) {
//                    std::shared_ptr<storm::models::sparse::Model<double>> sparseModel = pomdp;
//                    storm::api::exportSparseModelAsDrn(sparseModel, ioSettings.getExportExplicitFilename());
//                }
                
                std::shared_ptr<storm::logic::Formula const> formula;
                if (!symbolicInput.properties.empty()) {
                    formula = symbolicInput.properties.front().getRawFormula();
                    STORM_PRINT_AND_LOG("Analyzing property '" << *formula << "'" << std::endl);
                    STORM_LOG_WARN_COND(symbolicInput.properties.size() == 1, "There is currently no support for multiple properties. All other properties will be ignored.");
                }
            
                if (pomdpSettings.isAnalyzeUniqueObservationsSet()) {
                    STORM_PRINT_AND_LOG("Analyzing states with unique observation ..." << std::endl);
                    storm::analysis::UniqueObservationStates<ValueType> uniqueAnalysis(*pomdp);
                    std::cout << uniqueAnalysis.analyse() << std::endl;
                }
            
                if (formula) {
                    auto formulaInfo = storm::pomdp::analysis::getFormulaInformation(*pomdp, *formula);
                    STORM_LOG_THROW(!formulaInfo.isUnsupported(), storm::exceptions::InvalidPropertyException, "The formula '" << *formula << "' is not supported by storm-pomdp.");
                    
                    storm::utility::Stopwatch sw(true);
                    // Note that formulaInfo contains state-based information which potentially needs to be updated during preprocessing
                    if (performPreprocessing<ValueType, DdType>(pomdp, formulaInfo, *formula)) {
                        sw.stop();
                        STORM_PRINT_AND_LOG("Time for graph-based POMDP (pre-)processing: " << sw << "s." << std::endl);
                        pomdp->printModelInformationToStream(std::cout);
                    }
                    
                    sw.restart();
                    if (performAnalysis<ValueType, DdType>(pomdp, formulaInfo, *formula)) {
                        sw.stop();
                        STORM_PRINT_AND_LOG("Time for POMDP analysis: " << sw << "s." << std::endl);
                    }
                    
                    sw.restart();
                    if (performTransformation<ValueType, DdType>(pomdp, *formula)) {
                        sw.stop();
                        STORM_PRINT_AND_LOG("Time for POMDP transformation(s): " << sw << "s." << std::endl);
                    }
                } else {
                    STORM_LOG_WARN("Nothing to be done. Did you forget to specify a formula?");
                }
            
            }
            
            template <storm::dd::DdType DdType>
            void processOptionsWithDdLib(storm::cli::SymbolicInput const& symbolicInput, storm::cli::ModelProcessingInformation const& mpi) {
                STORM_LOG_ERROR_COND(mpi.buildValueType == mpi.verificationValueType, "Build value type differs from verification value type. Will ignore Verification value type.");
                switch (mpi.buildValueType) {
                    case storm::cli::ModelProcessingInformation::ValueType::FinitePrecision:
                        processOptionsWithValueTypeAndDdLib<double, DdType>(symbolicInput, mpi);
                        break;
                    case storm::cli::ModelProcessingInformation::ValueType::Exact:
                        STORM_LOG_THROW(DdType == storm::dd::DdType::Sylvan, storm::exceptions::UnexpectedException, "Exact arithmetic is only supported with Dd library Sylvan.");
                        processOptionsWithValueTypeAndDdLib<storm::RationalNumber, storm::dd::DdType::Sylvan>(symbolicInput, mpi);
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected ValueType for model building.");
                }
            }
            
            void processOptions() {
                auto symbolicInput = storm::cli::parseSymbolicInput();
                storm::cli::ModelProcessingInformation mpi;
                std::tie(symbolicInput, mpi) = storm::cli::preprocessSymbolicInput(symbolicInput);
                switch (mpi.ddType) {
                    case storm::dd::DdType::CUDD:
                        processOptionsWithDdLib<storm::dd::DdType::CUDD>(symbolicInput, mpi);
                        break;
                    case storm::dd::DdType::Sylvan:
                        processOptionsWithDdLib<storm::dd::DdType::Sylvan>(symbolicInput, mpi);
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected Dd Type.");
                }
            }
        }
    }
}

/*!
 * Entry point for the pomdp backend.
 *
 * @param argc The argc argument of main().
 * @param argv The argv argument of main().
 * @return Return code, 0 if successfull, not 0 otherwise.
 */
int main(const int argc, const char** argv) {
    //try {
        storm::utility::setUp();
        storm::cli::printHeader("Storm-pomdp", argc, argv);
        storm::settings::initializePomdpSettings("Storm-POMDP", "storm-pomdp");

        bool optionsCorrect = storm::cli::parseOptions(argc, argv);
        if (!optionsCorrect) {
            return -1;
        }
        storm::cli::setUrgentOptions();

        // Invoke storm-pomdp with obtained settings
        storm::pomdp::cli::processOptions();

        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        return 0;
    // } catch (storm::exceptions::BaseException const &exception) {
    //    STORM_LOG_ERROR("An exception caused Storm-pomdp to terminate. The message of the exception is: " << exception.what());
    //    return 1;
    //} catch (std::exception const &exception) {
    //    STORM_LOG_ERROR("An unexpected exception occurred and caused Storm-pomdp to terminate. The message of this exception is: " << exception.what());
    //    return 2;
    //}
}
