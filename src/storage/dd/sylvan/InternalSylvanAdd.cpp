#include "src/storage/dd/sylvan/InternalSylvanAdd.h"

#include <iostream>

#include "src/storage/dd/sylvan/InternalSylvanDdManager.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace dd {
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType>::InternalAdd(InternalDdManager<DdType::Sylvan> const* ddManager, sylvan::Mtbdd const& sylvanMtbdd) : ddManager(ddManager), sylvanMtbdd(sylvanMtbdd) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::operator==(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return this->sylvanMtbdd == other.sylvanMtbdd;
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::operator!=(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return this->sylvanMtbdd != other.sylvanMtbdd;
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::ite(InternalAdd<DdType::Sylvan, ValueType> const& thenDd, InternalAdd<DdType::Sylvan, ValueType> const& elseDd) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, sylvan::Mtbdd(static_cast<MTBDD>(this->sylvanMtbdd.NotZero().GetBDD())).Ite(thenDd.sylvanMtbdd, elseDd.sylvanMtbdd));
        }
                
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::operator+(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Plus(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType>& InternalAdd<DdType::Sylvan, ValueType>::operator+=(InternalAdd<DdType::Sylvan, ValueType> const& other) {
            this->sylvanMtbdd = this->sylvanMtbdd.Plus(other.sylvanMtbdd);
            return *this;
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::operator*(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Times(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType>& InternalAdd<DdType::Sylvan, ValueType>::operator*=(InternalAdd<DdType::Sylvan, ValueType> const& other) {
            this->sylvanMtbdd = this->sylvanMtbdd.Times(other.sylvanMtbdd);
            return *this;
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::operator-(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager,  this->sylvanMtbdd.Minus(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType>& InternalAdd<DdType::Sylvan, ValueType>::operator-=(InternalAdd<DdType::Sylvan, ValueType> const& other) {
            this->sylvanMtbdd = this->sylvanMtbdd.Minus(other.sylvanMtbdd);
            return *this;
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::operator/(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager,  this->sylvanMtbdd.Divide(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType>& InternalAdd<DdType::Sylvan, ValueType>::operator/=(InternalAdd<DdType::Sylvan, ValueType> const& other) {
            this->sylvanMtbdd = this->sylvanMtbdd.Divide(other.sylvanMtbdd);
            return *this;
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::equals(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalBdd<DdType::Sylvan>(ddManager,  this->sylvanMtbdd.Equals(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::notEquals(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return !this->equals(other);
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::less(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalBdd<DdType::Sylvan>(ddManager, this->sylvanMtbdd.Less(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::lessOrEqual(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalBdd<DdType::Sylvan>(ddManager, this->sylvanMtbdd.LessOrEqual(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::greater(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return !this->lessOrEqual(other);
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::greaterOrEqual(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return !this->less(other);
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::pow(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Pow(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::mod(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Mod(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::logxy(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Logxy(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::floor() const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Floor());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::ceil() const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Ceil());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::minimum(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Min(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::maximum(InternalAdd<DdType::Sylvan, ValueType> const& other) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Max(other.sylvanMtbdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::sumAbstract(InternalBdd<DdType::Sylvan> const& cube) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.AbstractPlus(static_cast<MTBDD>(cube.sylvanBdd.GetBDD())));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::minAbstract(InternalBdd<DdType::Sylvan> const& cube) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.AbstractMin(static_cast<MTBDD>(cube.sylvanBdd.GetBDD())));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::maxAbstract(InternalBdd<DdType::Sylvan> const& cube) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.AbstractMax(static_cast<MTBDD>(cube.sylvanBdd.GetBDD())));
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::equalModuloPrecision(InternalAdd<DdType::Sylvan, ValueType> const& other, double precision, bool relative) const {
            if (relative) {
                return this->sylvanMtbdd.EqualNormRel(other.sylvanMtbdd, precision);
            } else {
                return this->sylvanMtbdd.EqualNorm(other.sylvanMtbdd, precision);
            }
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::swapVariables(std::vector<InternalBdd<DdType::Sylvan>> const& from, std::vector<InternalBdd<DdType::Sylvan>> const& to) const {
            std::vector<sylvan::Mtbdd> fromMtbdd;
            std::vector<sylvan::Mtbdd> toMtbdd;
            for (auto it1 = from.begin(), ite1 = from.end(), it2 = to.begin(); it1 != ite1; ++it1, ++it2) {
                fromMtbdd.push_back(it1->getSylvanBdd());
                toMtbdd.push_back(it2->getSylvanBdd());
            }
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Permute(fromMtbdd, toMtbdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::multiplyMatrix(InternalAdd<DdType::Sylvan, ValueType> const& otherMatrix, std::vector<InternalBdd<DdType::Sylvan>> const& summationDdVariables) const {
            InternalBdd<DdType::Sylvan> summationVariables = ddManager->getBddOne();
            for (auto const& ddVariable : summationDdVariables) {
                summationVariables &= ddVariable;
            }
            
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.AndExists(otherMatrix.sylvanMtbdd, summationVariables.getSylvanBdd()));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::greater(ValueType const& value) const {
            return InternalBdd<DdType::Sylvan>(ddManager, this->sylvanMtbdd.BddStrictThreshold(value));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::greaterOrEqual(ValueType const& value) const {
            return InternalBdd<DdType::Sylvan>(ddManager, this->sylvanMtbdd.BddThreshold(value));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::less(ValueType const& value) const {
            return !this->greaterOrEqual(value);
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::lessOrEqual(ValueType const& value) const {
            return !this->greater(value);
        }
        
        template<typename ValueType>
        InternalBdd<DdType::Sylvan> InternalAdd<DdType::Sylvan, ValueType>::notZero() const {
            return InternalBdd<DdType::Sylvan>(ddManager, this->sylvanMtbdd.NotZero());
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
            return InternalBdd<DdType::Sylvan>(ddManager, sylvan::Bdd(static_cast<BDD>(this->sylvanMtbdd.Support().GetMTBDD())));
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::Sylvan, ValueType>::getNonZeroCount(uint_fast64_t numberOfDdVariables) const {
            if (numberOfDdVariables == 0) {
                return 0;
            }
            return static_cast<uint_fast64_t>(this->sylvanMtbdd.NonZeroCount(numberOfDdVariables));
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::Sylvan, ValueType>::getLeafCount() const {
            return static_cast<uint_fast64_t>(this->sylvanMtbdd.CountLeaves());
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::Sylvan, ValueType>::getNodeCount() const {
            return static_cast<uint_fast64_t>(this->sylvanMtbdd.NodeCount());
        }
        
        template<typename ValueType>
        ValueType InternalAdd<DdType::Sylvan, ValueType>::getMin() const {
            return static_cast<ValueType>(this->sylvanMtbdd.getDoubleMin());
        }
        
        template<typename ValueType>
        ValueType InternalAdd<DdType::Sylvan, ValueType>::getMax() const {
            return static_cast<ValueType>(this->sylvanMtbdd.getDoubleMax());
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::isOne() const {
            return *this == ddManager->getAddOne<ValueType>();
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::isZero() const {
            return *this == ddManager->getAddZero<ValueType>();
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::Sylvan, ValueType>::isConstant() const {
            return this->sylvanMtbdd.isTerminal();
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::Sylvan, ValueType>::getIndex() const {
            return static_cast<uint_fast64_t>(this->sylvanMtbdd.TopVar());
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::exportToDot(std::string const& filename, std::vector<std::string> const& ddVariableNamesAsStrings) const {
            // Open the file, dump the DD and close it again.
            FILE* filePointer = fopen(filename.c_str() , "w");
            mtbdd_fprintdot(filePointer, this->sylvanMtbdd.GetMTBDD(), nullptr);
            fclose(filePointer);
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
        
        template<typename ValueType>
        sylvan::Mtbdd InternalAdd<DdType::Sylvan, ValueType>::getSylvanMtbdd() const {
            return sylvanMtbdd;
        }
        
        template class InternalAdd<DdType::Sylvan, double>;
        template class InternalAdd<DdType::Sylvan, uint_fast64_t>;
    }
}