#include "src/storage/dd/cudd/InternalCuddAdd.h"

#include "src/storage/dd/cudd/CuddOdd.h"

#include "src/storage/dd/cudd/InternalCuddDdManager.h"
#include "src/storage/dd/cudd/InternalCuddBdd.h"

#include "src/storage/SparseMatrix.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"

namespace storm {
    namespace dd {
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType>::InternalAdd(InternalDdManager<DdType::CUDD> const* ddManager, ADD cuddAdd) : ddManager(ddManager), cuddAdd(cuddAdd) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::CUDD, ValueType>::operator==(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return this->getCuddAdd() == other.getCuddAdd();
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::CUDD, ValueType>::operator!=(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return !(*this == other);
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::ite(InternalAdd<DdType::CUDD, ValueType> const& thenDd, InternalAdd<DdType::CUDD, ValueType> const& elseDd) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Ite(thenDd.getCuddAdd(), elseDd.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::operator!() const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, ~this->getCuddAdd());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::operator||(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd() | other.getCuddAdd());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType>& InternalAdd<DdType::CUDD, ValueType>::operator|=(InternalAdd<DdType::CUDD, ValueType> const& other) {
            this->cuddAdd = this->getCuddAdd() | other.getCuddAdd();
            return *this;
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::operator+(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd() + other.getCuddAdd());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType>& InternalAdd<DdType::CUDD, ValueType>::operator+=(InternalAdd<DdType::CUDD, ValueType> const& other) {
            this->cuddAdd = this->getCuddAdd() + other.getCuddAdd();
            return *this;
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::operator*(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd() * other.getCuddAdd());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType>& InternalAdd<DdType::CUDD, ValueType>::operator*=(InternalAdd<DdType::CUDD, ValueType> const& other) {
            this->cuddAdd = this->getCuddAdd() * other.getCuddAdd();
            return *this;
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::operator-(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd() - other.getCuddAdd());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType>& InternalAdd<DdType::CUDD, ValueType>::operator-=(InternalAdd<DdType::CUDD, ValueType> const& other) {
            this->cuddAdd = this->getCuddAdd() - other.getCuddAdd();
            return *this;
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::operator/(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Divide(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType>& InternalAdd<DdType::CUDD, ValueType>::operator/=(InternalAdd<DdType::CUDD, ValueType> const& other) {
            this->cuddAdd = this->getCuddAdd().Divide(other.getCuddAdd());
            return *this;
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::equals(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Equals(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::notEquals(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().NotEquals(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::less(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().LessThan(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::lessOrEqual(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().LessThanOrEqual(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::greater(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().GreaterThan(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::greaterOrEqual(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().GreaterThanOrEqual(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::pow(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Pow(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::mod(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Mod(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::logxy(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().LogXY(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::floor() const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Floor());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::ceil() const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Ceil());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::minimum(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Minimum(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::maximum(InternalAdd<DdType::CUDD, ValueType> const& other) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Maximum(other.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::sumAbstract(InternalBdd<DdType::CUDD> const& cube) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().ExistAbstract(cube.toAdd<ValueType>().getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::minAbstract(InternalBdd<DdType::CUDD> const& cube) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().MinAbstract(cube.toAdd<ValueType>().getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::maxAbstract(InternalBdd<DdType::CUDD> const& cube) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().MaxAbstract(cube.toAdd<ValueType>().getCuddAdd()));
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::CUDD, ValueType>::equalModuloPrecision(InternalAdd<DdType::CUDD, ValueType> const& other, double precision, bool relative) const {
            if (relative) {
                return this->getCuddAdd().EqualSupNormRel(other.getCuddAdd(), precision);
            } else {
                return this->getCuddAdd().EqualSupNorm(other.getCuddAdd(), precision);
            }
        };
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::swapVariables(std::vector<InternalAdd<DdType::CUDD, ValueType>> const& from, std::vector<InternalAdd<DdType::CUDD, ValueType>> const& to) const {
            std::vector<ADD> fromAdd;
            std::vector<ADD> toAdd;
            STORM_LOG_ASSERT(fromAdd.size() == toAdd.size(), "Sizes of vectors do not match.");
            for (auto it1 = from.begin(), ite1 = from.end(), it2 = to.begin(); it1 != ite1; ++it1, ++it2) {
                fromAdd.push_back(it1->getCuddAdd());
                toAdd.push_back(it2->getCuddAdd());
            }
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().SwapVariables(fromAdd, toAdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::multiplyMatrix(InternalAdd<DdType::CUDD, ValueType> const& otherMatrix, std::vector<InternalAdd<DdType::CUDD, ValueType>> const& summationDdVariables) const {
            // Create the CUDD summation variables.
            std::vector<ADD> summationAdds;
            for (auto const& ddVariable : summationDdVariables) {
                summationAdds.push_back(ddVariable.getCuddAdd());
            }
            
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().MatrixMultiply(otherMatrix.getCuddAdd(), summationAdds));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::CUDD> InternalAdd<DdType::CUDD, ValueType>::greater(ValueType const& value) const {
            return InternalBdd<DdType::CUDD>(ddManager, this->getCuddAdd().BddStrictThreshold(value));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::CUDD> InternalAdd<DdType::CUDD, ValueType>::greaterOrEqual(ValueType const& value) const {
            return InternalBdd<DdType::CUDD>(ddManager, this->getCuddAdd().BddThreshold(value));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::CUDD> InternalAdd<DdType::CUDD, ValueType>::less(ValueType const& value) const {
            return InternalBdd<DdType::CUDD>(ddManager, ~this->getCuddAdd().BddThreshold(value));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::CUDD> InternalAdd<DdType::CUDD, ValueType>::lessOrEqual(ValueType const& value) const {
            return InternalBdd<DdType::CUDD>(ddManager, ~this->getCuddAdd().BddStrictThreshold(value));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::CUDD> InternalAdd<DdType::CUDD, ValueType>::notZero() const {
            return this->toBdd();
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::constrain(InternalAdd<DdType::CUDD, ValueType> const& constraint) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Constrain(constraint.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::restrict(InternalAdd<DdType::CUDD, ValueType> const& constraint) const {
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, this->getCuddAdd().Restrict(constraint.getCuddAdd()));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::CUDD> InternalAdd<DdType::CUDD, ValueType>::getSupport() const {
            return InternalBdd<DdType::CUDD>(ddManager, this->getCuddAdd().Support());
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::CUDD, ValueType>::getNonZeroCount(uint_fast64_t numberOfDdVariables) const {
            return static_cast<uint_fast64_t>(this->getCuddAdd().CountMinterm(static_cast<int>(numberOfDdVariables)));
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::CUDD, ValueType>::getLeafCount() const {
            return static_cast<uint_fast64_t>(this->getCuddAdd().CountLeaves());
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::CUDD, ValueType>::getNodeCount() const {
            return static_cast<uint_fast64_t>(this->getCuddAdd().nodeCount());
        }
        
        template<typename ValueType>
        ValueType InternalAdd<DdType::CUDD, ValueType>::getMin() const {
            ADD constantMinAdd = this->getCuddAdd().FindMin();
            return static_cast<double>(Cudd_V(constantMinAdd.getNode()));
        }
        
        template<typename ValueType>
        ValueType InternalAdd<DdType::CUDD, ValueType>::getMax() const {
            ADD constantMaxAdd = this->getCuddAdd().FindMax();
            return static_cast<double>(Cudd_V(constantMaxAdd.getNode()));
        }
        
        template<typename ValueType>
        InternalBdd<DdType::CUDD> InternalAdd<DdType::CUDD, ValueType>::toBdd() const {
            return InternalBdd<DdType::CUDD>(ddManager, this->getCuddAdd().BddPattern());
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::CUDD, ValueType>::isOne() const {
            return this->getCuddAdd().IsOne();
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::CUDD, ValueType>::isZero() const {
            return this->getCuddAdd().IsZero();
        }
        
        template<typename ValueType>
        bool InternalAdd<DdType::CUDD, ValueType>::isConstant() const {
            return Cudd_IsConstant(this->getCuddAdd().getNode());
        }
        
        template<typename ValueType>
        uint_fast64_t InternalAdd<DdType::CUDD, ValueType>::getIndex() const {
            return static_cast<uint_fast64_t>(this->getCuddAdd().NodeReadIndex());
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::CUDD, ValueType>::exportToDot(std::string const& filename, std::vector<std::string> const& ddVariableNamesAsStrings) const {
            // Build the name input of the DD.
            std::vector<char*> ddNames;
            std::string ddName("f");
            ddNames.push_back(new char[ddName.size() + 1]);
            std::copy(ddName.c_str(), ddName.c_str() + 2, ddNames.back());
            
            // Now build the variables names.
            std::vector<char*> ddVariableNames;
            for (auto const& element : ddVariableNamesAsStrings) {
                ddVariableNames.push_back(new char[element.size() + 1]);
                std::copy(element.c_str(), element.c_str() + element.size() + 1, ddVariableNames.back());
            }
            
            // Open the file, dump the DD and close it again.
            FILE* filePointer = fopen(filename.c_str() , "w");
            std::vector<ADD> cuddAddVector = { this->getCuddAdd() };
            ddManager->getCuddManager().DumpDot(cuddAddVector, &ddVariableNames[0], &ddNames[0], filePointer);
            fclose(filePointer);
            
            // Finally, delete the names.
            for (char* element : ddNames) {
                delete[] element;
            }
            for (char* element : ddVariableNames) {
                delete[] element;
            }
        }
        
        template<typename ValueType>
        AddIterator<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::begin(std::shared_ptr<DdManager<DdType::CUDD> const> fullDdManager, std::set<storm::expressions::Variable> const& metaVariables, bool enumerateDontCareMetaVariables) const {
            int* cube;
            double value;
            DdGen* generator = this->getCuddAdd().FirstCube(&cube, &value);
            return AddIterator<DdType::CUDD, ValueType>(fullDdManager, generator, cube, value, (Cudd_IsGenEmpty(generator) != 0), &metaVariables, enumerateDontCareMetaVariables);
        }
        
        template<typename ValueType>
        AddIterator<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::end(std::shared_ptr<DdManager<DdType::CUDD> const> fullDdManager, bool enumerateDontCareMetaVariables) const {
            return AddIterator<DdType::CUDD, ValueType>(fullDdManager, nullptr, nullptr, 0, true, nullptr, enumerateDontCareMetaVariables);
        }
        
        template<typename ValueType>
        ADD InternalAdd<DdType::CUDD, ValueType>::getCuddAdd() const {
            return this->cuddAdd;
        }
        
        template<typename ValueType>
        DdNode* InternalAdd<DdType::CUDD, ValueType>::getCuddDdNode() const {
            return this->getCuddAdd().getNode();
        }

        template<typename ValueType>
        void InternalAdd<DdType::CUDD, ValueType>::composeWithExplicitVector(storm::dd::Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, ValueType const&)> const& function) const {
            composeWithExplicitVectorRec(this->getCuddDdNode(), nullptr, 0, ddVariableIndices.size(), 0, odd, ddVariableIndices, targetVector, function);
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::CUDD, ValueType>::composeWithExplicitVector(storm::dd::Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<uint_fast64_t> const& offsets, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, ValueType const&)> const& function) const {
            composeWithExplicitVectorRec(this->getCuddDdNode(), &offsets, 0, ddVariableIndices.size(), 0, odd, ddVariableIndices, targetVector, function);
        }

        template<typename ValueType>
        void InternalAdd<DdType::CUDD, ValueType>::composeWithExplicitVectorRec(DdNode const* dd, std::vector<uint_fast64_t> const* offsets, uint_fast64_t currentLevel, uint_fast64_t maxLevel, uint_fast64_t currentOffset, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, ValueType const&)> const& function) const {
            // For the empty DD, we do not need to add any entries.
            if (dd == Cudd_ReadZero(ddManager->getCuddManager().getManager())) {
                return;
            }
            
            // If we are at the maximal level, the value to be set is stored as a constant in the DD.
            if (currentLevel == maxLevel) {
                ValueType& targetValue = targetVector[offsets != nullptr ? (*offsets)[currentOffset] : currentOffset];
                targetValue = function(targetValue, Cudd_V(dd));
            } else if (ddVariableIndices[currentLevel] < dd->index) {
                // If we skipped a level, we need to enumerate the explicit entries for the case in which the bit is set
                // and for the one in which it is not set.
                composeWithExplicitVectorRec(dd, offsets, currentLevel + 1, maxLevel, currentOffset, odd.getElseSuccessor(), ddVariableIndices, targetVector, function);
                composeWithExplicitVectorRec(dd, offsets, currentLevel + 1, maxLevel, currentOffset + odd.getElseOffset(), odd.getThenSuccessor(), ddVariableIndices, targetVector, function);
            } else {
                // Otherwise, we simply recursively call the function for both (different) cases.
                composeWithExplicitVectorRec(Cudd_E(dd), offsets, currentLevel + 1, maxLevel, currentOffset, odd.getElseSuccessor(), ddVariableIndices, targetVector, function);
                composeWithExplicitVectorRec(Cudd_T(dd), offsets, currentLevel + 1, maxLevel, currentOffset + odd.getElseOffset(), odd.getThenSuccessor(), ddVariableIndices, targetVector, function);
            }
        }
        
        template<typename ValueType>
        std::vector<InternalAdd<DdType::CUDD, ValueType>> InternalAdd<DdType::CUDD, ValueType>::splitIntoGroups(std::vector<uint_fast64_t> const& ddGroupVariableIndices) const {
            std::vector<InternalAdd<DdType::CUDD, ValueType>> result;
            splitIntoGroupsRec(this->getCuddDdNode(), result, ddGroupVariableIndices, 0, ddGroupVariableIndices.size());
            return result;
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::CUDD, ValueType>::splitIntoGroupsRec(DdNode* dd, std::vector<InternalAdd<DdType::CUDD, ValueType>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel) const {
            // For the empty DD, we do not need to create a group.
            if (dd == Cudd_ReadZero(ddManager->getCuddManager().getManager())) {
                return;
            }
            
            if (currentLevel == maxLevel) {
                groups.push_back(InternalAdd<DdType::CUDD, ValueType>(ddManager, ADD(ddManager->getCuddManager(), dd)));
            } else if (ddGroupVariableIndices[currentLevel] < dd->index) {
                splitIntoGroupsRec(dd, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                splitIntoGroupsRec(dd, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
            } else {
                splitIntoGroupsRec(Cudd_E(dd), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                splitIntoGroupsRec(Cudd_T(dd), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
            }
        }
        
        template<typename ValueType>
        std::vector<std::pair<InternalAdd<DdType::CUDD, ValueType>, InternalAdd<DdType::CUDD, ValueType>>> InternalAdd<DdType::CUDD, ValueType>::splitIntoGroups(InternalAdd<DdType::CUDD, ValueType> vector, std::vector<uint_fast64_t> const& ddGroupVariableIndices) const {
            std::vector<std::pair<InternalAdd<DdType::CUDD, ValueType>, InternalAdd<DdType::CUDD, ValueType>>> result;
            splitIntoGroupsRec(this->getCuddDdNode(), vector.getCuddDdNode(), result, ddGroupVariableIndices, 0, ddGroupVariableIndices.size());
            return result;
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::CUDD, ValueType>::splitIntoGroupsRec(DdNode* dd1, DdNode* dd2, std::vector<std::pair<InternalAdd<DdType::CUDD, ValueType>, InternalAdd<DdType::CUDD, ValueType>>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel) const {
            // For the empty DD, we do not need to create a group.
            if (dd1 == Cudd_ReadZero(ddManager->getCuddManager().getManager()) && dd2 == Cudd_ReadZero(ddManager->getCuddManager().getManager())) {
                return;
            }
            
            if (currentLevel == maxLevel) {
                groups.push_back(std::make_pair(InternalAdd<DdType::CUDD, ValueType>(ddManager, ADD(ddManager->getCuddManager(), dd1)), InternalAdd<DdType::CUDD, ValueType>(ddManager, ADD(ddManager->getCuddManager(), dd2))));
            } else if (ddGroupVariableIndices[currentLevel] < dd1->index) {
                if (ddGroupVariableIndices[currentLevel] < dd2->index) {
                    splitIntoGroupsRec(dd1, dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                    splitIntoGroupsRec(dd1, dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                } else {
                    splitIntoGroupsRec(dd1, Cudd_T(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                    splitIntoGroupsRec(dd1, Cudd_E(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                }
            } else if (ddGroupVariableIndices[currentLevel] < dd2->index) {
                splitIntoGroupsRec(Cudd_T(dd1), dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                splitIntoGroupsRec(Cudd_E(dd1), dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
            } else {
                splitIntoGroupsRec(Cudd_T(dd1), Cudd_T(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                splitIntoGroupsRec(Cudd_E(dd1), Cudd_E(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
            }
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::CUDD, ValueType>::toMatrixComponents(std::vector<uint_fast64_t> const& rowGroupIndices, std::vector<uint_fast64_t>& rowIndications, std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>& columnsAndValues, Odd<DdType::CUDD> const& rowOdd, Odd<DdType::CUDD> const& columnOdd, std::vector<uint_fast64_t> const& ddRowVariableIndices, std::vector<uint_fast64_t> const& ddColumnVariableIndices, bool writeValues) const {
            return toMatrixComponentsRec(this->getCuddDdNode(), rowGroupIndices, rowIndications, columnsAndValues, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, writeValues);
        }

        template<typename ValueType>
        void InternalAdd<DdType::CUDD, ValueType>::toMatrixComponentsRec(DdNode const* dd, std::vector<uint_fast64_t> const& rowGroupOffsets, std::vector<uint_fast64_t>& rowIndications, std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>& columnsAndValues, Odd<DdType::CUDD> const& rowOdd, Odd<DdType::CUDD> const& columnOdd, uint_fast64_t currentRowLevel, uint_fast64_t currentColumnLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset, uint_fast64_t currentColumnOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices, std::vector<uint_fast64_t> const& ddColumnVariableIndices, bool generateValues) const {
            // For the empty DD, we do not need to add any entries.
            if (dd == Cudd_ReadZero(ddManager->getCuddManager().getManager())) {
                return;
            }
            
            // If we are at the maximal level, the value to be set is stored as a constant in the DD.
            if (currentRowLevel + currentColumnLevel == maxLevel) {
                if (generateValues) {
                    columnsAndValues[rowIndications[rowGroupOffsets[currentRowOffset]]] = storm::storage::MatrixEntry<uint_fast64_t, ValueType>(currentColumnOffset, Cudd_V(dd));
                }
                ++rowIndications[rowGroupOffsets[currentRowOffset]];
            } else {
                DdNode const* elseElse;
                DdNode const* elseThen;
                DdNode const* thenElse;
                DdNode const* thenThen;
                
                if (ddColumnVariableIndices[currentColumnLevel] < dd->index) {
                    elseElse = elseThen = thenElse = thenThen = dd;
                } else if (ddRowVariableIndices[currentColumnLevel] < dd->index) {
                    elseElse = thenElse = Cudd_E(dd);
                    elseThen = thenThen = Cudd_T(dd);
                } else {
                    DdNode const* elseNode = Cudd_E(dd);
                    if (ddColumnVariableIndices[currentColumnLevel] < elseNode->index) {
                        elseElse = elseThen = elseNode;
                    } else {
                        elseElse = Cudd_E(elseNode);
                        elseThen = Cudd_T(elseNode);
                    }
                    
                    DdNode const* thenNode = Cudd_T(dd);
                    if (ddColumnVariableIndices[currentColumnLevel] < thenNode->index) {
                        thenElse = thenThen = thenNode;
                    } else {
                        thenElse = Cudd_E(thenNode);
                        thenThen = Cudd_T(thenNode);
                    }
                }
                
                // Visit else-else.
                toMatrixComponentsRec(elseElse, rowGroupOffsets, rowIndications, columnsAndValues, rowOdd.getElseSuccessor(), columnOdd.getElseSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset, currentColumnOffset, ddRowVariableIndices, ddColumnVariableIndices, generateValues);
                // Visit else-then.
                toMatrixComponentsRec(elseThen, rowGroupOffsets, rowIndications, columnsAndValues, rowOdd.getElseSuccessor(), columnOdd.getThenSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset, currentColumnOffset + columnOdd.getElseOffset(), ddRowVariableIndices, ddColumnVariableIndices, generateValues);
                // Visit then-else.
                toMatrixComponentsRec(thenElse, rowGroupOffsets, rowIndications, columnsAndValues, rowOdd.getThenSuccessor(), columnOdd.getElseSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset + rowOdd.getElseOffset(), currentColumnOffset, ddRowVariableIndices, ddColumnVariableIndices, generateValues);
                // Visit then-then.
                toMatrixComponentsRec(thenThen, rowGroupOffsets, rowIndications, columnsAndValues, rowOdd.getThenSuccessor(), columnOdd.getThenSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset + rowOdd.getElseOffset(), currentColumnOffset + columnOdd.getElseOffset(), ddRowVariableIndices, ddColumnVariableIndices, generateValues);
            }
        }
        

//        template<typename ValueType>
//        storm::storage::SparseMatrix<ValueType> InternalAdd<DdType::CUDD, ValueType>::toMatrix(uint_fast64_t numberOfDdVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, std::vector<uint_fast64_t> const& ddRowVariableIndices, storm::dd::Odd<DdType::CUDD> const& columnOdd, std::vector<uint_fast64_t> const& ddColumnVariableIndices) const {
//            // Prepare the vectors that represent the matrix.
//            std::vector<uint_fast64_t> rowIndications(rowOdd.getTotalOffset() + 1);
//            std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> columnsAndValues(this->getNonZeroCount(numberOfDdVariables));
//            
//            // Create a trivial row grouping.
//            std::vector<uint_fast64_t> trivialRowGroupIndices(rowIndications.size());
//            uint_fast64_t i = 0;
//            for (auto& entry : trivialRowGroupIndices) {
//                entry = i;
//                ++i;
//            }
//            
//            // Use the toMatrixRec function to compute the number of elements in each row. Using the flag, we prevent
//            // it from actually generating the entries in the entry vector.
//            toMatrixRec(this->getCuddDdNode(), rowIndications, columnsAndValues, trivialRowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, false);
//            
//            // TODO: counting might be faster by just summing over the primed variables and then using the ODD to convert
//            // the resulting (DD) vector to an explicit vector.
//            
//            // Now that we computed the number of entries in each row, compute the corresponding offsets in the entry vector.
//            uint_fast64_t tmp = 0;
//            uint_fast64_t tmp2 = 0;
//            for (uint_fast64_t i = 1; i < rowIndications.size(); ++i) {
//                tmp2 = rowIndications[i];
//                rowIndications[i] = rowIndications[i - 1] + tmp;
//                std::swap(tmp, tmp2);
//            }
//            rowIndications[0] = 0;
//            
//            // Now actually fill the entry vector.
//            toMatrixRec(this->getCuddDdNode(), rowIndications, columnsAndValues, trivialRowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, true);
//            
//            // Since the last call to toMatrixRec modified the rowIndications, we need to restore the correct values.
//            for (uint_fast64_t i = rowIndications.size() - 1; i > 0; --i) {
//                rowIndications[i] = rowIndications[i - 1];
//            }
//            rowIndications[0] = 0;
//            
//            // Construct matrix and return result.
//            return storm::storage::SparseMatrix<double>(columnOdd.getTotalOffset(), std::move(rowIndications), std::move(columnsAndValues), std::move(trivialRowGroupIndices), false);
//        }
//        
//        template<typename ValueType>
//        storm::storage::SparseMatrix<ValueType> InternalAdd<DdType::CUDD, ValueType>::toMatrix(std::vector<uint_fast64_t> const& ddGroupVariableIndices, InternalBdd<DdType::CUDD> const& groupVariableCube, storm::dd::Odd<DdType::CUDD> const& rowOdd, std::vector<uint_fast64_t> const& ddRowVariableIndices, storm::dd::Odd<DdType::CUDD> const& columnOdd, std::vector<uint_fast64_t> const& ddColumnVariableIndices, InternalBdd<DdType::CUDD> const& columnVariableCube) const {
//            // Start by computing the offsets (in terms of rows) for each row group.
//            InternalAdd<DdType::CUDD, uint_fast64_t> stateToNumberOfChoices = this->notZero().existsAbstract(columnVariableCube).template toAdd<uint_fast64_t>().sumAbstract(groupVariableCube);
//            std::vector<ValueType> rowGroupIndices = stateToNumberOfChoices.toVector(rowOdd);
//            rowGroupIndices.resize(rowGroupIndices.size() + 1);
//            uint_fast64_t tmp = 0;
//            uint_fast64_t tmp2 = 0;
//            for (uint_fast64_t i = 1; i < rowGroupIndices.size(); ++i) {
//                tmp2 = rowGroupIndices[i];
//                rowGroupIndices[i] = rowGroupIndices[i - 1] + tmp;
//                std::swap(tmp, tmp2);
//            }
//            rowGroupIndices[0] = 0;
//            
//            // Next, we split the matrix into one for each group. This only works if the group variables are at the very
//            // top.
//            std::vector<InternalAdd<DdType::CUDD, ValueType>> groups;
//            splitGroupsRec(this->getCuddDdNode(), groups, ddGroupVariableIndices, 0, ddGroupVariableIndices.size());
//            
//            // Create the actual storage for the non-zero entries.
//            std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> columnsAndValues(this->getNonZeroCount());
//            
//            // Now compute the indices at which the individual rows start.
//            std::vector<uint_fast64_t> rowIndications(rowGroupIndices.back() + 1);
//            
//            std::vector<InternalAdd<DdType::CUDD, ValueType>> statesWithGroupEnabled(groups.size());
//            InternalAdd<DdType::CUDD, ValueType> stateToRowGroupCount = this->getDdManager()->getAddZero();
//            for (uint_fast64_t i = 0; i < groups.size(); ++i) {
//                auto const& dd = groups[i];
//                
//                toMatrixRec(dd.getCuddDdNode(), rowIndications, columnsAndValues, rowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, false);
//                
//                statesWithGroupEnabled[i] = dd.notZero().existsAbstract(columnVariableCube).toAdd();
//                stateToRowGroupCount += statesWithGroupEnabled[i];
//                statesWithGroupEnabled[i].addToVector(rowOdd, ddRowVariableIndices, rowGroupIndices);
//            }
//            
//            // Since we modified the rowGroupIndices, we need to restore the correct values.
//            std::function<uint_fast64_t (uint_fast64_t const&, uint_fast64_t const&)> fct = [] (uint_fast64_t const& a, double const& b) -> uint_fast64_t { return a - static_cast<uint_fast64_t>(b); };
//            composeVectorRec(stateToRowGroupCount.getCuddDdNode(), 0, ddRowVariableIndices.size(), 0, rowOdd, ddRowVariableIndices, rowGroupIndices, fct);
//            
//            // Now that we computed the number of entries in each row, compute the corresponding offsets in the entry vector.
//            tmp = 0;
//            tmp2 = 0;
//            for (uint_fast64_t i = 1; i < rowIndications.size(); ++i) {
//                tmp2 = rowIndications[i];
//                rowIndications[i] = rowIndications[i - 1] + tmp;
//                std::swap(tmp, tmp2);
//            }
//            rowIndications[0] = 0;
//            
//            // Now actually fill the entry vector.
//            for (uint_fast64_t i = 0; i < groups.size(); ++i) {
//                auto const& dd = groups[i];
//                
//                toMatrixRec(dd.getCuddDdNode(), rowIndications, columnsAndValues, rowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, true);
//                
//                statesWithGroupEnabled[i].addToVector(rowOdd, ddRowVariableIndices, rowGroupIndices);
//            }
//            
//            // Since we modified the rowGroupIndices, we need to restore the correct values.
//            composeVectorRec(stateToRowGroupCount.getCuddDdNode(), 0, ddRowVariableIndices.size(), 0, rowOdd, ddRowVariableIndices, rowGroupIndices, fct);
//            
//            // Since the last call to toMatrixRec modified the rowIndications, we need to restore the correct values.
//            for (uint_fast64_t i = rowIndications.size() - 1; i > 0; --i) {
//                rowIndications[i] = rowIndications[i - 1];
//            }
//            rowIndications[0] = 0;
//            
//            return storm::storage::SparseMatrix<ValueType>(columnOdd.getTotalOffset(), std::move(rowIndications), std::move(columnsAndValues), std::move(rowGroupIndices), true);
//        }
//        
//        template<typename ValueType>
//        std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> InternalAdd<DdType::CUDD, ValueType>::toMatrixVector(InternalAdd<DdType::CUDD, ValueType> const& vector, std::vector<uint_fast64_t> const& ddGroupVariableIndices, std::vector<uint_fast64_t>&& rowGroupIndices, storm::dd::Odd<DdType::CUDD> const& rowOdd, std::vector<uint_fast64_t> const& ddRowVariableIndices, storm::dd::Odd<DdType::CUDD> const& columnOdd, std::vector<uint_fast64_t> const& ddColumnVariableIndices, InternalBdd<DdType::CUDD> const& columnVariableCube) const {
//            // Transform the row group sizes to the actual row group indices.
//            rowGroupIndices.resize(rowGroupIndices.size() + 1);
//            uint_fast64_t tmp = 0;
//            uint_fast64_t tmp2 = 0;
//            for (uint_fast64_t i = 1; i < rowGroupIndices.size(); ++i) {
//                tmp2 = rowGroupIndices[i];
//                rowGroupIndices[i] = rowGroupIndices[i - 1] + tmp;
//                std::swap(tmp, tmp2);
//            }
//            rowGroupIndices[0] = 0;
//            
//            // Create the explicit vector we need to fill later.
//            std::vector<double> explicitVector(rowGroupIndices.back());
//            
//            // Next, we split the matrix into one for each group. This only works if the group variables are at the very top.
//            std::vector<std::pair<InternalAdd<DdType::CUDD, ValueType>, InternalAdd<DdType::CUDD, ValueType>>> groups;
//            splitGroupsRec(this->getCuddDdNode(), vector.getCuddDdNode(), groups, ddGroupVariableIndices, 0, ddGroupVariableIndices.size());
//            
//            // Create the actual storage for the non-zero entries.
//            std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> columnsAndValues(this->getNonZeroCount());
//            
//            // Now compute the indices at which the individual rows start.
//            std::vector<uint_fast64_t> rowIndications(rowGroupIndices.back() + 1);
//            
//            std::vector<storm::dd::InternalAdd<DdType::CUDD, ValueType>> statesWithGroupEnabled(groups.size());
//            storm::dd::InternalAdd<storm::dd::DdType::CUDD, ValueType> stateToRowGroupCount = this->getDdManager()->getAddZero();
//            for (uint_fast64_t i = 0; i < groups.size(); ++i) {
//                std::pair<storm::dd::InternalAdd<DdType::CUDD, ValueType>, storm::dd::InternalAdd<DdType::CUDD, ValueType>> ddPair = groups[i];
//                
//                toMatrixRec(ddPair.first.getCuddDdNode(), rowIndications, columnsAndValues, rowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, false);
//                toVectorRec(ddPair.second.getCuddDdNode(), explicitVector, rowGroupIndices, rowOdd, 0, ddRowVariableIndices.size(), 0, ddRowVariableIndices);
//                
//                statesWithGroupEnabled[i] = (ddPair.first.notZero().existsAbstract(columnVariableCube) || ddPair.second.notZero()).toAdd();
//                stateToRowGroupCount += statesWithGroupEnabled[i];
//                statesWithGroupEnabled[i].addToVector(rowOdd, ddRowVariableIndices, rowGroupIndices);
//            }
//            
//            // Since we modified the rowGroupIndices, we need to restore the correct values.
//            std::function<uint_fast64_t (uint_fast64_t const&, double const&)> fct = [] (uint_fast64_t const& a, double const& b) -> uint_fast64_t { return a - static_cast<uint_fast64_t>(b); };
//            composeVectorRec(stateToRowGroupCount.getCuddDdNode(), 0, ddRowVariableIndices.size(), 0, rowOdd, ddRowVariableIndices, rowGroupIndices, fct);
//            
//            // Now that we computed the number of entries in each row, compute the corresponding offsets in the entry vector.
//            tmp = 0;
//            tmp2 = 0;
//            for (uint_fast64_t i = 1; i < rowIndications.size(); ++i) {
//                tmp2 = rowIndications[i];
//                rowIndications[i] = rowIndications[i - 1] + tmp;
//                std::swap(tmp, tmp2);
//            }
//            rowIndications[0] = 0;
//            
//            // Now actually fill the entry vector.
//            for (uint_fast64_t i = 0; i < groups.size(); ++i) {
//                auto const& dd = groups[i].first;
//                
//                toMatrixRec(dd.getCuddDdNode(), rowIndications, columnsAndValues, rowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, true);
//                
//                statesWithGroupEnabled[i].addToVector(rowOdd, ddRowVariableIndices, rowGroupIndices);
//            }
//            
//            // Since we modified the rowGroupIndices, we need to restore the correct values.
//            composeVectorRec(stateToRowGroupCount.getCuddDdNode(), 0, ddRowVariableIndices.size(), 0, rowOdd, ddRowVariableIndices, rowGroupIndices, fct);
//            
//            // Since the last call to toMatrixRec modified the rowIndications, we need to restore the correct values.
//            for (uint_fast64_t i = rowIndications.size() - 1; i > 0; --i) {
//                rowIndications[i] = rowIndications[i - 1];
//            }
//            rowIndications[0] = 0;
//            
//            return std::make_pair(storm::storage::SparseMatrix<ValueType>(columnOdd.getTotalOffset(), std::move(rowIndications), std::move(columnsAndValues), std::move(rowGroupIndices), true), std::move(explicitVector));
//        }
//        
//        template<typename ValueType>
//        void InternalAdd<DdType::CUDD, ValueType>::splitGroupsRec(DdNode* dd1, DdNode* dd2, std::vector<std::pair<InternalAdd<DdType::CUDD, ValueType>, InternalAdd<DdType::CUDD, ValueType>>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel) const {
//            // For the empty DD, we do not need to create a group.
//            if (dd1 == Cudd_ReadZero(ddManager->getCuddManager().getManager()) && dd2 == Cudd_ReadZero(ddManager->getCuddManager().getManager())) {
//                return;
//            }
//            
//            if (currentLevel == maxLevel) {
//                groups.push_back(std::make_pair(InternalAdd<DdType::CUDD, ValueType>(ddManager, ADD(ddManager->getCuddManager(), dd1)),
//                                                InternalAdd<DdType::CUDD, ValueType>(ddManager, ADD(ddManager->getCuddManager(), dd2))));
//            } else if (ddGroupVariableIndices[currentLevel] < dd1->index) {
//                if (ddGroupVariableIndices[currentLevel] < dd2->index) {
//                    splitGroupsRec(dd1, dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
//                    splitGroupsRec(dd1, dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
//                } else {
//                    splitGroupsRec(dd1, Cudd_T(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
//                    splitGroupsRec(dd1, Cudd_E(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
//                }
//            } else if (ddGroupVariableIndices[currentLevel] < dd2->index) {
//                splitGroupsRec(Cudd_T(dd1), dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
//                splitGroupsRec(Cudd_E(dd1), dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
//            } else {
//                splitGroupsRec(Cudd_T(dd1), Cudd_T(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
//                splitGroupsRec(Cudd_E(dd1), Cudd_E(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
//            }
//        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalAdd<DdType::CUDD, ValueType>::fromVector(InternalDdManager<DdType::CUDD> const* ddManager, std::vector<ValueType> const& values, storm::dd::Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices) {
            uint_fast64_t offset = 0;
            return InternalAdd<DdType::CUDD, ValueType>(ddManager, ADD(ddManager->getCuddManager(), fromVectorRec(ddManager->getCuddManager().getManager(), offset, 0, ddVariableIndices.size(), values, odd, ddVariableIndices)));
        }
        
        template<typename ValueType>
        DdNode* InternalAdd<DdType::CUDD, ValueType>::fromVectorRec(::DdManager* manager, uint_fast64_t& currentOffset, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<ValueType> const& values, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices) {
            if (currentLevel == maxLevel) {
                // If we are in a terminal node of the ODD, we need to check whether the then-offset of the ODD is one
                // (meaning the encoding is a valid one) or zero (meaning the encoding is not valid). Consequently, we
                // need to copy the next value of the vector iff the then-offset is greater than zero.
                if (odd.getThenOffset() > 0) {
                    return Cudd_addConst(manager, values[currentOffset++]);
                } else {
                    return Cudd_ReadZero(manager);
                }
            } else {
                // If the total offset is zero, we can just return the constant zero DD.
                if (odd.getThenOffset() + odd.getElseOffset() == 0) {
                    return Cudd_ReadZero(manager);
                }
                
                // Determine the new else-successor.
                DdNode* elseSuccessor = nullptr;
                if (odd.getElseOffset() > 0) {
                    elseSuccessor = fromVectorRec(manager, currentOffset, currentLevel + 1, maxLevel, values, odd.getElseSuccessor(), ddVariableIndices);
                } else {
                    elseSuccessor = Cudd_ReadZero(manager);
                }
                Cudd_Ref(elseSuccessor);
                
                // Determine the new then-successor.
                DdNode* thenSuccessor = nullptr;
                if (odd.getThenOffset() > 0) {
                    thenSuccessor = fromVectorRec(manager, currentOffset, currentLevel + 1, maxLevel, values, odd.getThenSuccessor(), ddVariableIndices);
                } else {
                    thenSuccessor = Cudd_ReadZero(manager);
                }
                Cudd_Ref(thenSuccessor);
                
                // Create a node representing ITE(currentVar, thenSuccessor, elseSuccessor);
                DdNode* result = Cudd_addIthVar(manager, static_cast<int>(ddVariableIndices[currentLevel]));
                Cudd_Ref(result);
                DdNode* newResult = Cudd_addIte(manager, result, thenSuccessor, elseSuccessor);
                Cudd_Ref(newResult);
                
                // Dispose of the intermediate results
                Cudd_RecursiveDeref(manager, result);
                Cudd_RecursiveDeref(manager, thenSuccessor);
                Cudd_RecursiveDeref(manager, elseSuccessor);
                
                // Before returning, we remove the protection imposed by the previous call to Cudd_Ref.
                Cudd_Deref(newResult);
                
                return newResult;
            }
        }
        
        template class InternalAdd<DdType::CUDD, double>;
        template class InternalAdd<DdType::CUDD, uint_fast64_t>;
    }
}