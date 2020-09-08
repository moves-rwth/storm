#include "ExplicitFigaroModelBuilder.h"

#include <map>

#include <storm/exceptions/IllegalArgumentException.h>
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/utility/bitoperations.h"
#include "storm/utility/constants.h"
#include "storm/utility/ProgressMeasurement.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/vector.h"
#include "storm/settings/SettingsManager.h"

#include "storm-figaro/api/storm-figaro.h"



namespace storm {
    namespace figaro{
        namespace builder {


            template<typename ValueType, typename StateType>
            ExplicitFigaroModelBuilder<ValueType, StateType>::ModelComponents::ModelComponents() : transitionMatrix(), stateLabeling(), markovianStates(), exitRates(), choiceLabeling() {
                // Intentionally left empty.
            }

            template<typename ValueType, typename StateType>
            ExplicitFigaroModelBuilder<ValueType, StateType>::MatrixBuilder::MatrixBuilder(bool canHaveNondeterminism) : mappingOffset(0), stateRemapping(), currentRowGroup(0), currentRow(0), canHaveNondeterminism((canHaveNondeterminism)) {
                // Create matrix builder
                builder = storm::storage::SparseMatrixBuilder<ValueType>(0, 0, 0, false, canHaveNondeterminism, 0);
            }

            /*!
             * Constructor.
             *
             * @param figaro Figaro.
             */
            template<typename ValueType, typename StateType>
            storm::figaro::builder::ExplicitFigaroModelBuilder<ValueType, StateType>::ExplicitFigaroModelBuilder(storm::figaro::FigaroProgram & figaroprogram):
                    figaroprogram(figaroprogram),
                    variable_info(std::make_shared<storm::generator::VariableInformation> (figaroprogram, storm::figaro::api::getFigaroBooleanVariables(figaroprogram), storm::figaro::api::getFigaroIntegerVariables(figaroprogram))),
                    manager(storm::figaro::api::getFigaroExpresseionManager(figaroprogram)),
                    generator(std::make_shared<storm::figaro::generator::FigaroNextStateGenerator<ValueType, uint32_t>>(figaroprogram, *manager, *variable_info)),
                    matrixBuilder(false)//!generator.isDeterministicModel()),
//                    explorationQueue(1, 0, 0.9, false)
            {std::cout<<"Explicit Figaro Model Builder constructor called";}

            template<typename ValueType, typename StateType>
            std::shared_ptr<storm::models::sparse::Model<ValueType>> ExplicitFigaroModelBuilder<ValueType, StateType>::buildModelNoApproximation(){
                storm::builder::ExplicitModelBuilder<ValueType> builder(generator);
                std::shared_ptr<storm::models::sparse::Model<ValueType>> built_model = builder.build();
                return built_model;

            }

            template<typename ValueType, typename StateType>
            void ExplicitFigaroModelBuilder<ValueType, StateType>::buildModel(size_t iteration, double approximationThreshold, storm::builder::ApproximationHeuristic approximationHeuristic) {
                STORM_LOG_TRACE("Generating DFT state space");
                usedHeuristic = approximationHeuristic;
                std::cout<<"hello world";
                exit(8);
            }
//
//                if (approximationThreshold > 0 && !this->uniqueFailedState && false) {
//                    // Approximation requires unique failed states
//                    // TODO: lift this restriction
//                    STORM_LOG_WARN("Approximation requires unique failed state. Forcing use of unique failed state.");
//                    this->uniqueFailedState = true;
//                }
//
//                if (iteration < 1) {
//                    // Initialize
//                    switch (usedHeuristic) {
//                        case storm::builder::ApproximationHeuristic::DEPTH:
//                            explorationQueue = storm::storage::BucketPriorityQueue<ExplorationHeuristic>(dft.nrElements()+1, 0, 0.9, false);
//                            break;
//                        case storm::builder::ApproximationHeuristic::PROBABILITY:
//                            explorationQueue = storm::storage::BucketPriorityQueue<ExplorationHeuristic>(200, 0, 0.9, true);
//                            break;
//                        case storm::builder::ApproximationHeuristic::BOUNDDIFFERENCE:
//                            explorationQueue = storm::storage::BucketPriorityQueue<ExplorationHeuristic>(200, 0, 0.9, true);
//                            break;
//                        default:
//                            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Heuristic not known.");
//                    }
//                    modelComponents.markovianStates = storm::storage::BitVector(INITIAL_BITVECTOR_SIZE);
//
//                    if (this->uniqueFailedState) {
//                        // Introduce explicit fail state
//                        storm::generator::StateBehavior<ValueType, StateType> behavior = generator.createMergeFailedState([this] (DFTStatePointer const& state) {
//                            size_t failedStateId = newIndex++;
//                            STORM_LOG_ASSERT(failedStateId == 0, "Unique failed id is not 0.");
//                            matrixBuilder.stateRemapping.push_back(0);
//                            return failedStateId;
//                        } );
//
//                        matrixBuilder.setRemapping(0);
//                        STORM_LOG_ASSERT(!behavior.empty(), "Behavior is empty.");
//                        matrixBuilder.newRowGroup();
//                        setMarkovian(behavior.begin()->isMarkovian());
//
//                        // Now add self loop.
//                        // TODO: maybe use general method.
//                        STORM_LOG_ASSERT(behavior.getNumberOfChoices() == 1, "Wrong number of choices for failed state.");
//                        STORM_LOG_ASSERT(behavior.begin()->size() == 1, "Wrong number of transitions for failed state.");
//                        std::pair<StateType, ValueType> stateProbabilityPair = *(behavior.begin()->begin());
//                        STORM_LOG_ASSERT(stateProbabilityPair.first == 0, "No self loop for failed state.");
//                        STORM_LOG_ASSERT(storm::utility::isOne<ValueType>(stateProbabilityPair.second), "Probability for failed state != 1.");
//                        matrixBuilder.addTransition(stateProbabilityPair.first, stateProbabilityPair.second);
//                        matrixBuilder.finishRow();
//                    }
//
//                    // Build initial state
//                    this->stateStorage.initialStateIndices = generator.getInitialStates(std::bind(&ExplicitDFTModelBuilder::getOrAddStateIndex, this, std::placeholders::_1));
//                    STORM_LOG_ASSERT(stateStorage.initialStateIndices.size() == 1, "Only one initial state assumed.");
//                    initialStateIndex = stateStorage.initialStateIndices[0];
//                    STORM_LOG_TRACE("Initial state: " << initialStateIndex);
//
//                    // DFT may be instantly failed due to a constant failure
//                    // in this case a model only consisting of the uniqueFailedState suffices
//                    if (initialStateIndex == 0 && this->uniqueFailedState) {
//                        modelComponents.markovianStates.resize(1);
//                        modelComponents.deterministicModel = generator.isDeterministicModel();
//
//                        STORM_LOG_TRACE("Markovian states: " << modelComponents.markovianStates);
//                        STORM_LOG_DEBUG("Model has 1 state");
//                        STORM_LOG_DEBUG(
//                                "Model is " << (generator.isDeterministicModel() ? "deterministic" : "non-deterministic"));
//
//                        // Build transition matrix
//                        modelComponents.transitionMatrix = matrixBuilder.builder.build(1, 1);
//                        STORM_LOG_TRACE("Transition matrix: " << std::endl << modelComponents.transitionMatrix);
//
//                        buildLabeling();
//                        return;
//                    }
//
//                    // Initialize heuristic values for inital state
//                    STORM_LOG_ASSERT(!statesNotExplored.at(initialStateIndex).second, "Heuristic for initial state is already initialized");
//                    ExplorationHeuristicPointer heuristic;
//                    switch (usedHeuristic) {
//                        case storm::builder::ApproximationHeuristic::DEPTH:
//                            heuristic = std::make_shared<DFTExplorationHeuristicDepth<ValueType>>(initialStateIndex);
//                            break;
//                        case storm::builder::ApproximationHeuristic::PROBABILITY:
//                            heuristic = std::make_shared<DFTExplorationHeuristicProbability<ValueType>>(initialStateIndex);
//                            break;
//                        case storm::builder::ApproximationHeuristic::BOUNDDIFFERENCE:
//                            heuristic = std::make_shared<DFTExplorationHeuristicBoundDifference<ValueType>>(initialStateIndex);
//                            break;
//                        default:
//                            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Heuristic not known.");
//                    }
//                    heuristic->markExpand();
//                    statesNotExplored[initialStateIndex].second = heuristic;
//                    explorationQueue.push(heuristic);
//                } else {
//                    initializeNextIteration();
//                }
//
//                if (approximationThreshold > 0.0) {
//                    switch (usedHeuristic) {
//                        case storm::builder::ApproximationHeuristic::DEPTH:
//                            approximationThreshold = iteration + 1;
//                            break;
//                        case storm::builder::ApproximationHeuristic::PROBABILITY:
//                        case storm::builder::ApproximationHeuristic::BOUNDDIFFERENCE:
//                            approximationThreshold = std::pow(2, -(double)iteration); // Need conversion to avoid overflow when negating
//                            break;
//                        default:
//                            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Heuristic not known.");
//                    }
//                }
//
//                auto ftSettings = storm::settings::getModule<storm::settings::modules::FaultTreeSettings>();
//                if (ftSettings.isMaxDepthSet()) {
//                    STORM_LOG_ASSERT(usedHeuristic == storm::builder::ApproximationHeuristic::DEPTH, "MaxDepth requires 'depth' exploration heuristic.");
//                    approximationThreshold = ftSettings.getMaxDepth();
//                }
//
//                exploreStateSpace(approximationThreshold);
//
//                size_t stateSize = stateStorage.getNumberOfStates() + (this->uniqueFailedState ? 1 : 0);
//                modelComponents.markovianStates.resize(stateSize);
//                modelComponents.deterministicModel = generator.isDeterministicModel();
//
//                // Fix the entries in the transition matrix according to the mapping of ids to row group indices
//                STORM_LOG_ASSERT(matrixBuilder.getRemapping(initialStateIndex) == initialStateIndex, "Initial state should not be remapped.");
//                // TODO: do not consider all rows?
//                STORM_LOG_TRACE("Remap matrix: " << matrixBuilder.stateRemapping << ", offset: " << matrixBuilder.mappingOffset);
//                matrixBuilder.remap();
//
//                STORM_LOG_TRACE("State remapping: " << matrixBuilder.stateRemapping);
//                STORM_LOG_TRACE("Markovian states: " << modelComponents.markovianStates);
//                STORM_LOG_DEBUG("Model has " << stateSize << " states");
//                STORM_LOG_DEBUG("Model is " << (generator.isDeterministicModel() ? "deterministic" : "non-deterministic"));
//
//                // Build transition matrix
//                modelComponents.transitionMatrix = matrixBuilder.builder.build(stateSize, stateSize);
//                if (stateSize <= 15) {
//                    STORM_LOG_TRACE("Transition matrix: " << std::endl << modelComponents.transitionMatrix);
//                } else {
//                    STORM_LOG_TRACE("Transition matrix: too big to print");
//                }
//
//                buildLabeling();
//            }

            // Explicitly instantiate the class.
            template class ExplicitFigaroModelBuilder<double,uint32_t>;




} //builder
} //figaro
} ///storm


