#include "src/storage/dd/sylvan/SylvanAddIterator.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace dd {
        template<typename ValueType>
        AddIterator<DdType::Sylvan, ValueType>::AddIterator() {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        AddIterator<DdType::Sylvan, ValueType>& AddIterator<DdType::Sylvan, ValueType>::operator++() {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        bool AddIterator<DdType::Sylvan, ValueType>::operator==(AddIterator<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        bool AddIterator<DdType::Sylvan, ValueType>::operator!=(AddIterator<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        std::pair<storm::expressions::SimpleValuation, ValueType> AddIterator<DdType::Sylvan, ValueType>::operator*() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template class AddIterator<DdType::Sylvan, double>;
        template class AddIterator<DdType::Sylvan, uint_fast64_t>;
    }
}