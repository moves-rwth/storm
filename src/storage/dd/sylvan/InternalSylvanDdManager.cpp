#include "src/storage/dd/sylvan/InternalSylvanDdManager.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace dd {
        
        InternalDdManager<DdType::Sylvan>::InternalDdManager() {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalDdManager<DdType::Sylvan>::getBddOne() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalDdManager<DdType::Sylvan>::getAddOne() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalDdManager<DdType::Sylvan>::getBddZero() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalDdManager<DdType::Sylvan>::getAddZero() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalDdManager<DdType::Sylvan>::getConstant(ValueType const& value) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        std::pair<InternalBdd<DdType::Sylvan>, InternalBdd<DdType::Sylvan>> InternalDdManager<DdType::Sylvan>::createNewDdVariablePair() {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        void InternalDdManager<DdType::Sylvan>::allowDynamicReordering(bool value) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        bool InternalDdManager<DdType::Sylvan>::isDynamicReorderingAllowed() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        void InternalDdManager<DdType::Sylvan>::triggerReordering() {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
                
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddOne() const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddOne() const;
        
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddZero() const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddZero() const;
        
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getConstant(double const& value) const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getConstant(uint_fast64_t const& value) const;
    }
}