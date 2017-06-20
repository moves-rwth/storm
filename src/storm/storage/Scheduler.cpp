#include <storm/utility/vector.h>
#include "storm/storage/Scheduler.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace storage {
        
        template <typename ValueType>
        Scheduler<ValueType>::Scheduler(uint_fast64_t numberOfModelStates, boost::optional<storm::storage::MemoryStructure> const& memoryStructure) : memoryStructure(memoryStructure) {
            uint_fast64_t numOfMemoryStates = memoryStructure ? memoryStructure->getNumberOfStates() : 1;
            schedulerChoices = std::vector<std::vector<SchedulerChoice<ValueType>>>(numOfMemoryStates, std::vector<SchedulerChoice<ValueType>>(numberOfModelStates));
            numOfUndefinedChoices = numOfMemoryStates * numberOfModelStates;
            numOfDeterministicChoices = 0;
        }
        
        template <typename ValueType>
        Scheduler<ValueType>::Scheduler(uint_fast64_t numberOfModelStates, boost::optional<storm::storage::MemoryStructure>&& memoryStructure) : memoryStructure(std::move(memoryStructure)) {
            uint_fast64_t numOfMemoryStates = memoryStructure ? memoryStructure->getNumberOfStates() : 1;
            schedulerChoices = std::vector<std::vector<SchedulerChoice<ValueType>>>(numOfMemoryStates, std::vector<SchedulerChoice<ValueType>>(numberOfModelStates));
            numOfUndefinedChoices = numOfMemoryStates * numberOfModelStates;
            numOfDeterministicChoices = 0;
        }
        
        template <typename ValueType>
        void Scheduler<ValueType>::setChoice(SchedulerChoice<ValueType> const& choice, uint_fast64_t modelState, uint_fast64_t memoryState) {
            STORM_LOG_ASSERT(memoryState < getNumberOfMemoryStates(), "Illegal memory state index");
            STORM_LOG_ASSERT(modelState < schedulerChoices[memoryState].size(), "Illegal model state index");
            auto& schedulerChoice = schedulerChoices[memoryState][modelState];
            if (schedulerChoice.isDefined()) {
                if (!choice.isDefined()) {
                    ++numOfUndefinedChoices;
                }
            } else {
                if (choice.isDefined()) {
                   assert(numOfUndefinedChoices > 0);
                    --numOfUndefinedChoices;
                }
            }
            if (schedulerChoice.isDeterministic()) {
                if (!choice.isDeterministic()) {
                    assert(numOfDeterministicChoices > 0);
                    --numOfDeterministicChoices;
                }
            } else {
                if (choice.isDeterministic()) {
                    ++numOfDeterministicChoices;
                }
            }
            
            schedulerChoice = choice;
        }

        template <typename ValueType>
        void Scheduler<ValueType>::clearChoice(uint_fast64_t modelState, uint_fast64_t memoryState) {
            STORM_LOG_ASSERT(memoryState < getNumberOfMemoryStates(), "Illegal memory state index");
            STORM_LOG_ASSERT(modelState < schedulerChoices[memoryState].size(), "Illegal model state index");
            setChoice(SchedulerChoice<ValueType>(), modelState, memoryState);
        }
 
        template <typename ValueType>
        SchedulerChoice<ValueType> const& Scheduler<ValueType>::getChoice(uint_fast64_t modelState, uint_fast64_t memoryState) const {
            STORM_LOG_ASSERT(memoryState < getNumberOfMemoryStates(), "Illegal memory state index");
            STORM_LOG_ASSERT(modelState < schedulerChoices[memoryState].size(), "Illegal model state index");
            return schedulerChoices[memoryState][modelState];
        }
        
        template <typename ValueType>
        bool Scheduler<ValueType>::isPartialScheduler() const {
            return numOfUndefinedChoices != 0;
        }
        
        template <typename ValueType>
        bool Scheduler<ValueType>::isDeterministicScheduler() const {
            return numOfDeterministicChoices == (schedulerChoices.size() * schedulerChoices.begin()->size()) - numOfUndefinedChoices;
        }
        
        template <typename ValueType>
        bool Scheduler<ValueType>::isMemorylessScheduler() const {
            return getNumberOfMemoryStates() == 1;
        }

        template <typename ValueType>
        uint_fast64_t Scheduler<ValueType>::getNumberOfMemoryStates() const {
            return memoryStructure ? memoryStructure->getNumberOfStates() : 1;
        }

        template <typename ValueType>
        void Scheduler<ValueType>::printToStream(std::ostream& out, std::shared_ptr<storm::models::sparse::Model<ValueType>> model, bool skipUniqueChoices) const {
            out << "___________________________________________________________________" << std::endl;
            out << (isPartialScheduler() ? "Partially" : "Fully") << " defined ";
            out << (isMemorylessScheduler() ? "memoryless " : "");
            out << (isDeterministicScheduler() ? "deterministic" : "randomized") << " scheduler";
            if (!isMemorylessScheduler()) {
                out << " with " << getNumberOfMemoryStates() << " memory states";
            }
            out << "." << std::endl;
            out << "___________________________________________________________________" << std::endl;
            out << "Choices on " << schedulerChoices.front().size() << " model states:" << std::endl;
            STORM_LOG_WARN_COND(!(skipUniqueChoices && model == nullptr), "Can not skip unique choices if the model is not given.");
            out << "    model state:    " << "    " << (isMemorylessScheduler() ? "" : " memory:     ") << "choice(s)" << std::endl;
                for (uint_fast64_t state = 0; state < schedulerChoices.front().size(); ++state) {
                    // Check whether the state is skipped
                    if (skipUniqueChoices && model != nullptr && model->getTransitionMatrix().getRowGroupSize(state) == 1) {
                        continue;
                    }
                    if (model != nullptr && model->hasStateValuations()) {
                        out << std::setw(20) << model->getStateValuations().getStateInfo(state);
                    } else {
                        out << std::setw(20) << state;
                    }
                    out << "    ";
                    bool firstMemoryState = true;
                    for (uint_fast64_t memoryState = 0; memoryState < getNumberOfMemoryStates(); ++memoryState) {
                        if (firstMemoryState) {
                            firstMemoryState = false;
                        } else {
                            out << std::setw(20) << "";
                            out << "    ";
                        }
                        if (!isMemorylessScheduler()) {
                            out << "m" << std::setw(8) << memoryState;
                        }
                        
                        SchedulerChoice<ValueType> const& choice = schedulerChoices[memoryState][state];
                        if (choice.isDefined()) {
                            if (choice.isDeterministic()) {
                                if (model != nullptr && model->hasChoiceOrigins()) {
                                    out << model->getChoiceOrigins()->getChoiceInfo(model->getTransitionMatrix().getRowGroupIndices()[state] + choice.getDeterministicChoice());
                                } else {
                                    out << choice.getDeterministicChoice();
                                }
                            } else {
                                bool firstChoice = true;
                                for (auto const& choiceProbPair : choice.getChoiceAsDistribution()) {
                                    if (firstChoice) {
                                        firstChoice = false;
                                    } else {
                                        out << "   +    ";
                                    }
                                    out << choiceProbPair.second << ": (";
                                    if (model != nullptr && model->hasChoiceOrigins()) {
                                        out << model->getChoiceOrigins()->getChoiceInfo(model->getTransitionMatrix().getRowGroupIndices()[state] + choiceProbPair.first);
                                    } else {
                                        out << choiceProbPair.first;
                                    }
                                    out << ")";
                                }
                            }
                        } else {
                            out << "undefined.";
                        }
                        out << std::endl;
                    }
            }
            out << "___________________________________________________________________" << std::endl;
        }

        template class Scheduler<double>;
        template class Scheduler<float>;
        template class Scheduler<storm::RationalNumber>;
        template class Scheduler<storm::RationalFunction>;
        
    }
}