#include "src/storage/dd/sylvan/InternalSylvanBdd.h"

#include "src/storage/dd/sylvan/InternalSylvanAdd.h"
#include "src/storage/dd/sylvan/SylvanAddIterator.h"
#

#include "src/storage/BitVector.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace dd {
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::fromVector(InternalDdManager<DdType::Sylvan> const* ddManager, std::vector<ValueType> const& values, Odd const& odd, std::vector<uint_fast64_t> const& sortedDdVariableIndices, std::function<bool (ValueType const&)> const& filter) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        bool InternalBdd<DdType::Sylvan>::operator==(InternalBdd<DdType::Sylvan> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        bool InternalBdd<DdType::Sylvan>::operator!=(InternalBdd<DdType::Sylvan> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::ite(InternalBdd<DdType::Sylvan> const& thenDd, InternalBdd<DdType::Sylvan> const& elseDd) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::operator||(InternalBdd<DdType::Sylvan> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan>& InternalBdd<DdType::Sylvan>::operator|=(InternalBdd<DdType::Sylvan> const& other) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::operator&&(InternalBdd<DdType::Sylvan> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan>& InternalBdd<DdType::Sylvan>::operator&=(InternalBdd<DdType::Sylvan> const& other) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::iff(InternalBdd<DdType::Sylvan> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::exclusiveOr(InternalBdd<DdType::Sylvan> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::implies(InternalBdd<DdType::Sylvan> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::operator!() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan>& InternalBdd<DdType::Sylvan>::complement() {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::existsAbstract(InternalBdd<DdType::Sylvan> const& cube) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::universalAbstract(InternalBdd<DdType::Sylvan> const& cube) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::andExists(InternalBdd<DdType::Sylvan> const& other, InternalBdd<DdType::Sylvan> const& cube) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::constrain(InternalBdd<DdType::Sylvan> const& constraint) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::restrict(InternalBdd<DdType::Sylvan> const& constraint) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::swapVariables(std::vector<InternalBdd<DdType::Sylvan>> const& from, std::vector<InternalBdd<DdType::Sylvan>> const& to) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::getSupport() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        uint_fast64_t InternalBdd<DdType::Sylvan>::getNonZeroCount(uint_fast64_t numberOfDdVariables) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        uint_fast64_t InternalBdd<DdType::Sylvan>::getLeafCount() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        uint_fast64_t InternalBdd<DdType::Sylvan>::getNodeCount() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        bool InternalBdd<DdType::Sylvan>::isOne() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        bool InternalBdd<DdType::Sylvan>::isZero() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        uint_fast64_t InternalBdd<DdType::Sylvan>::getIndex() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        void InternalBdd<DdType::Sylvan>::exportToDot(std::string const& filename, std::vector<std::string> const& ddVariableNamesAsStrings) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalBdd<DdType::Sylvan>::toAdd() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        storm::storage::BitVector InternalBdd<DdType::Sylvan>::toVector(storm::dd::Odd const& rowOdd, std::vector<uint_fast64_t> const& ddVariableIndices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        Odd InternalBdd<DdType::Sylvan>::createOdd(std::vector<uint_fast64_t> const& ddVariableIndices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        void InternalBdd<DdType::Sylvan>::filterExplicitVector(Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType> const& sourceValues, std::vector<ValueType>& targetValues) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::fromVector(InternalDdManager<DdType::Sylvan> const* ddManager, std::vector<double> const& values, Odd const& odd, std::vector<uint_fast64_t> const& sortedDdVariableIndices, std::function<bool (double const&)> const& filter);
        template InternalBdd<DdType::Sylvan> InternalBdd<DdType::Sylvan>::fromVector(InternalDdManager<DdType::Sylvan> const* ddManager, std::vector<uint_fast64_t> const& values, Odd const& odd, std::vector<uint_fast64_t> const& sortedDdVariableIndices, std::function<bool (uint_fast64_t const&)> const& filter);
        
        template InternalAdd<DdType::Sylvan, double> InternalBdd<DdType::Sylvan>::toAdd() const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalBdd<DdType::Sylvan>::toAdd() const;
                
        template void InternalBdd<DdType::Sylvan>::filterExplicitVector(Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<double> const& sourceValues, std::vector<double>& targetValues) const;
        template void InternalBdd<DdType::Sylvan>::filterExplicitVector(Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<uint_fast64_t> const& sourceValues, std::vector<uint_fast64_t>& targetValues) const;
    }
}