#include "storm/solver/AbstractEquationSolver.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/UnmetRequirementException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        AbstractEquationSolver<ValueType>::AbstractEquationSolver() {
            auto const& generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            showProgressFlag = generalSettings.isVerboseSet();
            showProgressDelay = generalSettings.getShowProgressDelay();
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::setTerminationCondition(std::unique_ptr<TerminationCondition<ValueType>> terminationCondition) {
            this->terminationCondition = std::move(terminationCondition);
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::resetTerminationCondition() {
            this->terminationCondition = nullptr;
        }
        
        template<typename ValueType>
        bool AbstractEquationSolver<ValueType>::hasCustomTerminationCondition() const {
            return static_cast<bool>(this->terminationCondition);
        }
        
        template<typename ValueType>
        TerminationCondition<ValueType> const& AbstractEquationSolver<ValueType>::getTerminationCondition() const {
            return *terminationCondition;
        }
        
        template<typename ValueType>
        std::unique_ptr<TerminationCondition<ValueType>> const& AbstractEquationSolver<ValueType>::getTerminationConditionPointer() const {
            return terminationCondition;
        }
        
        template<typename ValueType>
        bool AbstractEquationSolver<ValueType>::terminateNow(std::vector<ValueType> const& values, SolverGuarantee const& guarantee) const {
            if (!this->hasCustomTerminationCondition()) {
                return false;
            }
            
            return this->getTerminationCondition().terminateNow(values, guarantee);
        }
        
        template<typename ValueType>
        bool AbstractEquationSolver<ValueType>::hasRelevantValues() const {
            return static_cast<bool>(relevantValues);
        }
        
        template<typename ValueType>
        storm::storage::BitVector const& AbstractEquationSolver<ValueType>::getRelevantValues()const {
            return relevantValues.get();
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::setRelevantValues(storm::storage::BitVector&& relevantValues) {
            this->relevantValues = std::move(relevantValues);
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::clearRelevantValues() {
            relevantValues = boost::none;
        }
        
        template<typename ValueType>
        bool AbstractEquationSolver<ValueType>::hasLowerBound(BoundType const& type) const {
            if (type == BoundType::Any) {
                return static_cast<bool>(lowerBound) || static_cast<bool>(lowerBounds);
            } else if (type == BoundType::Global) {
                return static_cast<bool>(lowerBound);
            } else if (type == BoundType::Local) {
                return static_cast<bool>(lowerBounds);
            }
            return false;
        }
        
        template<typename ValueType>
        bool AbstractEquationSolver<ValueType>::hasUpperBound(BoundType const& type) const {
            if (type == BoundType::Any) {
                return static_cast<bool>(upperBound) || static_cast<bool>(upperBounds);
            } else if (type == BoundType::Global) {
                return static_cast<bool>(upperBound);
            } else if (type == BoundType::Local) {
                return static_cast<bool>(upperBounds);
            }
            return false;
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::setLowerBound(ValueType const& value) {
            lowerBound = value;
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::setUpperBound(ValueType const& value) {
            upperBound = value;
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::setBounds(ValueType const& lower, ValueType const& upper) {
            setLowerBound(lower);
            setUpperBound(upper);
        }
        
        template<typename ValueType>
        ValueType const& AbstractEquationSolver<ValueType>::getLowerBound() const {
            return lowerBound.get();
        }
        
        template<typename ValueType>
        ValueType const& AbstractEquationSolver<ValueType>::getUpperBound() const {
            return upperBound.get();
        }
        
        template<typename ValueType>
        std::vector<ValueType> const& AbstractEquationSolver<ValueType>::getLowerBounds() const {
            return lowerBounds.get();
        }
        
        template<typename ValueType>
        std::vector<ValueType> const& AbstractEquationSolver<ValueType>::getUpperBounds() const {
            return upperBounds.get();
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::setLowerBounds(std::vector<ValueType> const& values) {
            lowerBounds = values;
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::setUpperBounds(std::vector<ValueType> const& values) {
            upperBounds = values;
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::setUpperBounds(std::vector<ValueType>&& values) {
            upperBounds = std::move(values);
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::setBounds(std::vector<ValueType> const& lower, std::vector<ValueType> const& upper) {
            setLowerBounds(lower);
            setUpperBounds(upper);
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::clearBounds() {
            lowerBound = boost::none;
            upperBound = boost::none;
            lowerBounds = boost::none;
            upperBounds = boost::none;
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::createLowerBoundsVector(std::vector<ValueType>& lowerBoundsVector) const {
            if (this->hasLowerBound(BoundType::Local)) {
                lowerBoundsVector = this->getLowerBounds();
            } else {
                ValueType lowerBound = this->hasLowerBound(BoundType::Global) ? this->getLowerBound() : storm::utility::zero<ValueType>();
                for (auto& e : lowerBoundsVector) {
                    e = lowerBound;
                }
            }
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::createUpperBoundsVector(std::vector<ValueType>& upperBoundsVector) const {
            STORM_LOG_ASSERT(this->hasUpperBound(), "Expecting upper bound(s).");
            if (this->hasUpperBound(BoundType::Global)) {
                upperBoundsVector.assign(upperBoundsVector.size(), this->getUpperBound());
            } else {
                upperBoundsVector.assign(this->getUpperBounds().begin(), this->getUpperBounds().end());
            }
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::createUpperBoundsVector(std::unique_ptr<std::vector<ValueType>>& upperBoundsVector, uint64_t length) const {
            STORM_LOG_ASSERT(this->hasUpperBound(), "Expecting upper bound(s).");
            if (!upperBoundsVector) {
                if (this->hasUpperBound(BoundType::Local)) {
                    STORM_LOG_ASSERT(length == this->getUpperBounds().size(), "Mismatching sizes.");
                    upperBoundsVector = std::make_unique<std::vector<ValueType>>(this->getUpperBounds());
                } else {
                    upperBoundsVector = std::make_unique<std::vector<ValueType>>(length, this->getUpperBound());
                }
            } else {
                createUpperBoundsVector(*upperBoundsVector);
            }
        }
        
        template<typename ValueType>
        bool AbstractEquationSolver<ValueType>::isShowProgressSet() const {
            return showProgressFlag;
        }
        
        template<typename ValueType>
        uint64_t AbstractEquationSolver<ValueType>::getShowProgressDelay() const {
            return showProgressDelay;
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::startMeasureProgress(uint64_t startingIteration) const {
            timeOfStart = std::chrono::high_resolution_clock::now();
            timeOfLastMessage = timeOfStart;
            iterationOfLastMessage = startingIteration;
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::showProgressIterative(uint64_t iteration, boost::optional<uint64_t> const& bound) const {
            if (this->isShowProgressSet()) {
                auto now = std::chrono::high_resolution_clock::now();
                auto durationSinceLastMessage = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(now - timeOfLastMessage).count());
                if (durationSinceLastMessage >= this->getShowProgressDelay()) {
                    uint64_t numberOfIterationsSinceLastMessage = iteration - iterationOfLastMessage;
                    STORM_LOG_INFO("Completed " << iteration << " iterations "
                                   << (bound ? "(out of " + std::to_string(bound.get()) + ") " : "")
                                   << "in " << std::chrono::duration_cast<std::chrono::seconds>(now - timeOfStart).count() << "s (currently " << (static_cast<double>(numberOfIterationsSinceLastMessage) / durationSinceLastMessage) << " per second)."
                                   );
                    timeOfLastMessage = std::chrono::high_resolution_clock::now();
                    iterationOfLastMessage = iteration;
                }
            }
        }
        
        template<typename ValueType>
        void AbstractEquationSolver<ValueType>::setPrecision(ValueType const& precision) {
            STORM_LOG_DEBUG("Setting solver precision for a solver that does not support precisions.");
        }

        
        
        template class AbstractEquationSolver<double>;
        template class AbstractEquationSolver<float>;

#ifdef STORM_HAVE_CARL
        template class AbstractEquationSolver<storm::RationalNumber>;
        template class AbstractEquationSolver<storm::RationalFunction>;
#endif
        
    }
}
