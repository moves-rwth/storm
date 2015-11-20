#include "src/storage/dd/sylvan/InternalSylvanAdd.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace dd {
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::operator==(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::operator!=(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::ite(InternalAdd<DdType::Sylvan, ValueType> const& thenDd, InternalAdd<DdType::Sylvan, ValueType> const& elseDd) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::operator!() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::operator||(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType>& InternalAdd<DdType::Sylvan, ValueType>::operator|=(InternalAdd<DdType::Sylvan, ValueType> const& other) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");

        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::operator+(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType>& InternalAdd<DdType::Sylvan, ValueType>::operator+=(InternalAdd<DdType::Sylvan, ValueType> const& other) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::operator*(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType>& InternalAdd<DdType::Sylvan, ValueType>::operator*=(InternalAdd<DdType::Sylvan, ValueType> const& other) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::operator-(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType>& InternalAdd<DdType::Sylvan, ValueType>::operator-=(InternalAdd<DdType::Sylvan, ValueType> const& other) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::operator/(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType>& InternalAdd<DdType::Sylvan, ValueType>::operator/=(InternalAdd<DdType::Sylvan, ValueType> const& other) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::equals(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::notEquals(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::less(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::lessOrEqual(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::greater(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::greaterOrEqual(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::pow(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::mod(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::logxy(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::floor() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::ceil() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::minimum(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::maximum(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::sumAbstract(InternalBdd<DdType::Sylvan> const& cube) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::minAbstract(InternalBdd<DdType::Sylvan> const& cube) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::maxAbstract(InternalBdd<DdType::Sylvan> const& cube) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::equalModuloPrecision(InternalAdd<DdType::Sylvan, ValueType> const& other, double precision, bool relative) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::swapVariables(std::vector<InternalAdd<DdType::Sylvan, ValueType>> const& from, std::vector<InternalAdd<DdType::Sylvan, ValueType>> const& to) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::multiplyMatrix(InternalAdd<DdType::Sylvan, ValueType> const& otherMatrix, std::vector<InternalAdd<DdType::Sylvan, ValueType>> const& summationDdVariables) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::greater(ValueType const& value) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::greaterOrEqual(ValueType const& value) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::less(ValueType const& value) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::lessOrEqual(ValueType const& value) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::notZero() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::constrain(InternalAdd<DdType::Sylvan, ValueType> const& constraint) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::restrict(InternalAdd<DdType::Sylvan, ValueType> const& constraint) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::getSupport() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::Sylvan, ValueType>::getNonZeroCount(uint_fast64_t numberOfDdVariables) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::Sylvan, ValueType>::getLeafCount() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::Sylvan, ValueType>::getNodeCount() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        ValueType InternalAdd<DdType::Sylvan, ValueType>::getMin() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        ValueType InternalAdd<DdType::Sylvan, ValueType>::getMax() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::toBdd() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::isOne() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::isZero() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::isConstant() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::Sylvan, ValueType>::getIndex() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::exportToDot(std::string const& filename, std::vector<std::string> const& ddVariableNamesAsStrings) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        AddIterator<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::begin(std::shared_ptr<DdManager<DdType::Sylvan> const> fullDdManager, std::set<storm::expressions::Variable> const& metaVariables, bool enumerateDontCareMetaVariables) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        AddIterator<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::end(std::shared_ptr<DdManager<DdType::Sylvan> const> fullDdManager, bool enumerateDontCareMetaVariables) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        Odd InternalAdd<DdType::Sylvan, ValueType>::createOdd(std::vector<uint_fast64_t> const& ddVariableIndices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::composeWithExplicitVector(storm::dd::Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, ValueType const&)> const& function) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::composeWithExplicitVector(storm::dd::Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<uint_fast64_t> const& offsets, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, ValueType const&)> const& function) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        std::vector<InternalAdd<DdType::Sylvan, ValueType>> InternalAdd<DdType::Sylvan, ValueType>::splitIntoGroups(std::vector<uint_fast64_t> const& ddGroupVariableIndices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        std::vector<std::pair<InternalAdd<DdType::Sylvan, ValueType>, InternalAdd<DdType::Sylvan, ValueType>>> InternalAdd<DdType::Sylvan, ValueType>::splitIntoGroups(InternalAdd<DdType::Sylvan, ValueType> vector, std::vector<uint_fast64_t> const& ddGroupVariableIndices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::toMatrixComponents(std::vector<uint_fast64_t> const& rowGroupIndices, std::vector<uint_fast64_t>& rowIndications, std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>& columnsAndValues, Odd const& rowOdd, Odd const& columnOdd, std::vector<uint_fast64_t> const& ddRowVariableIndices, std::vector<uint_fast64_t> const& ddColumnVariableIndices, bool writeValues) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::fromVector(InternalDdManager<DdType::Sylvan> const* ddManager, std::vector<ValueType> const& values, storm::dd::Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template class InternalAdd<DdType::Sylvan, double>;
        template class InternalAdd<DdType::Sylvan, uint_fast64_t>;
    }
}