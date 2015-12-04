#include "src/storage/dd/sylvan/InternalSylvanAdd.h"

#include "src/storage/dd/sylvan/InternalSylvanDdManager.h"

#include "src/storage/SparseMatrix.h"

#include "src/utility/macros.h"
#include "src/utility/constants.h"
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
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.AbstractPlus(cube.sylvanBdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::minAbstract(InternalBdd<DdType::Sylvan> const& cube) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.AbstractMin(cube.sylvanBdd));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::maxAbstract(InternalBdd<DdType::Sylvan> const& cube) const {
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.AbstractMax(cube.sylvanBdd));
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
            std::vector<uint32_t> fromIndices;
            std::vector<uint32_t> toIndices;
            for (auto it1 = from.begin(), ite1 = from.end(), it2 = to.begin(); it1 != ite1; ++it1, ++it2) {
                fromIndices.push_back(it1->getIndex());
                fromIndices.push_back(it2->getIndex());
                toIndices.push_back(it2->getIndex());
                toIndices.push_back(it1->getIndex());
            }
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, this->sylvanMtbdd.Permute(fromIndices, toIndices));
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
        AddIterator<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::begin(DdManager<DdType::Sylvan> const& fullDdManager, std::set<storm::expressions::Variable> const& metaVariables, bool enumerateDontCareMetaVariables) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        AddIterator<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::end(DdManager<DdType::Sylvan> const& fullDdManager, bool enumerateDontCareMetaVariables) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        Odd InternalAdd<DdType::Sylvan, ValueType>::createOdd(std::vector<uint_fast64_t> const& ddVariableIndices) const {
            // Prepare a unique table for each level that keeps the constructed ODD nodes unique.
            std::vector<std::unordered_map<BDD, std::shared_ptr<Odd>>> uniqueTableForLevels(ddVariableIndices.size() + 1);
            
            // Now construct the ODD structure from the ADD.
            std::shared_ptr<Odd> rootOdd = createOddRec(mtbdd_regular(this->getSylvanMtbdd().GetMTBDD()), 0, ddVariableIndices.size(), ddVariableIndices, uniqueTableForLevels);
            
            // Return a copy of the root node to remove the shared_ptr encapsulation.
            return Odd(*rootOdd);
        }
        
        template<typename ValueType>
        std::shared_ptr<Odd> InternalAdd<DdType::Sylvan, ValueType>::createOddRec(BDD dd, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<std::unordered_map<BDD, std::shared_ptr<Odd>>>& uniqueTableForLevels) {
            // Check whether the ODD for this node has already been computed (for this level) and if so, return this instead.
            auto const& iterator = uniqueTableForLevels[currentLevel].find(dd);
            if (iterator != uniqueTableForLevels[currentLevel].end()) {
                return iterator->second;
            } else {
                // Otherwise, we need to recursively compute the ODD.
                
                // If we are already past the maximal level that is to be considered, we can simply create an Odd without
                // successors
                if (currentLevel == maxLevel) {
                    uint_fast64_t elseOffset = 0;
                    uint_fast64_t thenOffset = 0;
                    
                    STORM_LOG_ASSERT(mtbdd_isleaf(dd), "Expected leaf at last level.");
                    
                    // If the DD is not the zero leaf, then the then-offset is 1.
                    if (!mtbdd_iszero(dd)) {
                        thenOffset = 1;
                    }

                    return std::make_shared<Odd>(nullptr, elseOffset, nullptr, thenOffset);
                } else if (mtbdd_isleaf(dd) || ddVariableIndices[currentLevel] < mtbdd_getvar(dd)) {
                    // If we skipped the level in the DD, we compute the ODD just for the else-successor and use the same
                    // node for the then-successor as well.
                    std::shared_ptr<Odd> elseNode = createOddRec(dd, currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::shared_ptr<Odd> thenNode = elseNode;
                    return std::make_shared<Odd>(elseNode, elseNode->getElseOffset() + elseNode->getThenOffset(), thenNode, thenNode->getElseOffset() + thenNode->getThenOffset());
                } else {
                    // Otherwise, we compute the ODDs for both the then- and else successors.
                    std::shared_ptr<Odd> elseNode = createOddRec(mtbdd_regular(mtbdd_getlow(dd)), currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::shared_ptr<Odd> thenNode = createOddRec(mtbdd_regular(mtbdd_gethigh(dd)), currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    
                    uint_fast64_t totalElseOffset = elseNode->getElseOffset() + elseNode->getThenOffset();
                    uint_fast64_t totalThenOffset = thenNode->getElseOffset() + thenNode->getThenOffset();
                    
                    return std::make_shared<Odd>(elseNode, totalElseOffset, thenNode, totalThenOffset);
                }
            }
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::composeWithExplicitVector(storm::dd::Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, ValueType const&)> const& function) const {
            composeWithExplicitVectorRec(this->getSylvanMtbdd().GetMTBDD(), nullptr, 0, ddVariableIndices.size(), 0, odd, ddVariableIndices, targetVector, function);
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::composeWithExplicitVector(storm::dd::Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<uint_fast64_t> const& offsets, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, ValueType const&)> const& function) const {
            composeWithExplicitVectorRec(mtbdd_regular(this->getSylvanMtbdd().GetMTBDD()), &offsets, 0, ddVariableIndices.size(), 0, odd, ddVariableIndices, targetVector, function);
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::composeWithExplicitVectorRec(MTBDD dd, std::vector<uint_fast64_t> const* offsets, uint_fast64_t currentLevel, uint_fast64_t maxLevel, uint_fast64_t currentOffset, Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, ValueType const&)> const& function) const {
            // For the empty DD, we do not need to add any entries.
            if (mtbdd_isleaf(dd) && mtbdd_iszero(dd)) {
                return;
            }
            
            // If we are at the maximal level, the value to be set is stored as a constant in the DD.
            if (currentLevel == maxLevel) {
                ValueType& targetValue = targetVector[offsets != nullptr ? (*offsets)[currentOffset] : currentOffset];
                targetValue = function(targetValue, getValue(dd));
            } else if (mtbdd_isleaf(dd) || ddVariableIndices[currentLevel] < mtbdd_getvar(dd)) {
                // If we skipped a level, we need to enumerate the explicit entries for the case in which the bit is set
                // and for the one in which it is not set.
                composeWithExplicitVectorRec(dd, offsets, currentLevel + 1, maxLevel, currentOffset, odd.getElseSuccessor(), ddVariableIndices, targetVector, function);
                composeWithExplicitVectorRec(dd, offsets, currentLevel + 1, maxLevel, currentOffset + odd.getElseOffset(), odd.getThenSuccessor(), ddVariableIndices, targetVector, function);
            } else {
                // Otherwise, we simply recursively call the function for both (different) cases.
                composeWithExplicitVectorRec(mtbdd_regular(mtbdd_getlow(dd)), offsets, currentLevel + 1, maxLevel, currentOffset, odd.getElseSuccessor(), ddVariableIndices, targetVector, function);
                composeWithExplicitVectorRec(mtbdd_regular(mtbdd_gethigh(dd)), offsets, currentLevel + 1, maxLevel, currentOffset + odd.getElseOffset(), odd.getThenSuccessor(), ddVariableIndices, targetVector, function);
            }
        }
        
        template<typename ValueType>
        std::vector<InternalAdd<DdType::Sylvan, ValueType>> InternalAdd<DdType::Sylvan, ValueType>::splitIntoGroups(std::vector<uint_fast64_t> const& ddGroupVariableIndices) const {
            std::vector<InternalAdd<DdType::Sylvan, ValueType>> result;
            splitIntoGroupsRec(mtbdd_regular(this->getSylvanMtbdd().GetMTBDD()), mtbdd_isnegated(this->getSylvanMtbdd().GetMTBDD()), result, ddGroupVariableIndices, 0, ddGroupVariableIndices.size());
            return result;
        }
        
        template<typename ValueType>
        std::vector<std::pair<InternalAdd<DdType::Sylvan, ValueType>, InternalAdd<DdType::Sylvan, ValueType>>> InternalAdd<DdType::Sylvan, ValueType>::splitIntoGroups(InternalAdd<DdType::Sylvan, ValueType> vector, std::vector<uint_fast64_t> const& ddGroupVariableIndices) const {
            std::vector<std::pair<InternalAdd<DdType::Sylvan, ValueType>, InternalAdd<DdType::Sylvan, ValueType>>> result;
            splitIntoGroupsRec(mtbdd_regular(this->getSylvanMtbdd().GetMTBDD()), mtbdd_isnegated(this->getSylvanMtbdd().GetMTBDD()), mtbdd_regular(vector.getSylvanMtbdd().GetMTBDD()), mtbdd_isnegated(vector.getSylvanMtbdd().GetMTBDD()), result, ddGroupVariableIndices, 0, ddGroupVariableIndices.size());
            return result;
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::splitIntoGroupsRec(MTBDD dd, bool negated, std::vector<InternalAdd<DdType::Sylvan, ValueType>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel) const {
            // For the empty DD, we do not need to create a group.
            if (mtbdd_isleaf(dd) && mtbdd_iszero(dd)) {
                return;
            }
            
            if (currentLevel == maxLevel) {
                groups.push_back(InternalAdd<DdType::Sylvan, ValueType>(ddManager, sylvan::Mtbdd(negated ? mtbdd_negate(dd) : dd)));
            } else if (mtbdd_isleaf(dd) || ddGroupVariableIndices[currentLevel] < mtbdd_getvar(dd)) {
                splitIntoGroupsRec(dd, negated, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                splitIntoGroupsRec(dd, negated, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
            } else {
                // Otherwise, we compute the ODDs for both the then- and else successors.
                MTBDD thenDdNode = mtbdd_gethigh(dd);
                MTBDD elseDdNode = mtbdd_getlow(dd);
                
                // Determine whether we have to evaluate the successors as if they were complemented.
                bool elseComplemented = mtbdd_isnegated(elseDdNode) ^ negated;
                bool thenComplemented = mtbdd_isnegated(thenDdNode) ^ negated;
                
                splitIntoGroupsRec(mtbdd_regular(elseDdNode), elseComplemented, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                splitIntoGroupsRec(mtbdd_regular(thenDdNode), thenComplemented, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
            }
        }

        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::splitIntoGroupsRec(MTBDD dd1, bool negated1, MTBDD dd2, bool negated2, std::vector<std::pair<InternalAdd<DdType::Sylvan, ValueType>, InternalAdd<DdType::Sylvan, ValueType>>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel) const {
            // For the empty DD, we do not need to create a group.
            if (mtbdd_isleaf(dd1) && mtbdd_isleaf(dd2) && mtbdd_iszero(dd1) && mtbdd_iszero(dd2)) {
                return;
            }
            
            if (currentLevel == maxLevel) {
                groups.push_back(std::make_pair(InternalAdd<DdType::Sylvan, ValueType>(ddManager, sylvan::Mtbdd(negated1 ? mtbdd_negate(dd1) : dd1 )), InternalAdd<DdType::Sylvan, ValueType>(ddManager, sylvan::Mtbdd(negated2 ? mtbdd_negate(dd2) : dd2))));
            } else if (mtbdd_isleaf(dd1) || ddGroupVariableIndices[currentLevel] < mtbdd_getvar(dd1)) {
                if (mtbdd_isleaf(dd2) || ddGroupVariableIndices[currentLevel] < mtbdd_getvar(dd2)) {
                    splitIntoGroupsRec(dd1, negated1, dd2, negated2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                    splitIntoGroupsRec(dd1, negated1, dd2, negated2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                } else {
                    MTBDD dd2ThenNode = mtbdd_gethigh(dd2);
                    MTBDD dd2ElseNode = mtbdd_getlow(dd2);
                    
                    // Determine whether we have to evaluate the successors as if they were complemented.
                    bool elseComplemented = mtbdd_isnegated(dd2ElseNode) ^ negated2;
                    bool thenComplemented = mtbdd_isnegated(dd2ThenNode) ^ negated2;
                    
                    splitIntoGroupsRec(dd1, negated1, mtbdd_regular(dd2ThenNode), thenComplemented, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                    splitIntoGroupsRec(dd1, negated1, mtbdd_regular(dd2ElseNode), elseComplemented, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                }
            } else if (mtbdd_isleaf(dd2) || ddGroupVariableIndices[currentLevel] < mtbdd_getvar(dd2)) {
                MTBDD dd1ThenNode = mtbdd_gethigh(dd1);
                MTBDD dd1ElseNode = mtbdd_getlow(dd1);
                
                // Determine whether we have to evaluate the successors as if they were complemented.
                bool elseComplemented = mtbdd_isnegated(dd1ElseNode) ^ negated1;
                bool thenComplemented = mtbdd_isnegated(dd1ThenNode) ^ negated1;

                splitIntoGroupsRec(mtbdd_regular(dd1ThenNode), thenComplemented, dd2, negated2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                splitIntoGroupsRec(mtbdd_regular(dd1ElseNode), elseComplemented, dd2, negated2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
            } else {
                MTBDD dd1ThenNode = mtbdd_gethigh(dd1);
                MTBDD dd1ElseNode = mtbdd_getlow(dd1);
                MTBDD dd2ThenNode = mtbdd_gethigh(dd2);
                MTBDD dd2ElseNode = mtbdd_getlow(dd2);
                
                // Determine whether we have to evaluate the successors as if they were complemented.
                bool dd1ElseComplemented = mtbdd_isnegated(dd1ElseNode) ^ negated1;
                bool dd1ThenComplemented = mtbdd_isnegated(dd1ThenNode) ^ negated1;
                bool dd2ElseComplemented = mtbdd_isnegated(dd2ElseNode) ^ negated2;
                bool dd2ThenComplemented = mtbdd_isnegated(dd2ThenNode) ^ negated2;
                
                splitIntoGroupsRec(mtbdd_regular(dd1ThenNode), dd1ThenComplemented, mtbdd_regular(dd2ThenNode), dd2ThenComplemented, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
                splitIntoGroupsRec(mtbdd_regular(dd1ElseNode), dd1ElseComplemented, mtbdd_regular(dd2ElseNode), dd2ElseComplemented, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel);
            }
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::toMatrixComponents(std::vector<uint_fast64_t> const& rowGroupIndices, std::vector<uint_fast64_t>& rowIndications, std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>& columnsAndValues, Odd const& rowOdd, Odd const& columnOdd, std::vector<uint_fast64_t> const& ddRowVariableIndices, std::vector<uint_fast64_t> const& ddColumnVariableIndices, bool writeValues) const {
            return toMatrixComponentsRec(mtbdd_regular(this->getSylvanMtbdd().GetMTBDD()), mtbdd_isnegated(this->getSylvanMtbdd().GetMTBDD()), rowGroupIndices, rowIndications, columnsAndValues, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, writeValues);
        }
        
        template<typename ValueType>
        void InternalAdd<DdType::Sylvan, ValueType>::toMatrixComponentsRec(MTBDD dd, bool negated, std::vector<uint_fast64_t> const& rowGroupOffsets, std::vector<uint_fast64_t>& rowIndications, std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>& columnsAndValues, Odd const& rowOdd, Odd const& columnOdd, uint_fast64_t currentRowLevel, uint_fast64_t currentColumnLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset, uint_fast64_t currentColumnOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices, std::vector<uint_fast64_t> const& ddColumnVariableIndices, bool generateValues) const {
            // For the empty DD, we do not need to add any entries.
            if (mtbdd_isleaf(dd) && mtbdd_iszero(dd)) {
                return;
            }
            
            // If we are at the maximal level, the value to be set is stored as a constant in the DD.
            if (currentRowLevel + currentColumnLevel == maxLevel) {
                if (generateValues) {
                    columnsAndValues[rowIndications[rowGroupOffsets[currentRowOffset]]] = storm::storage::MatrixEntry<uint_fast64_t, ValueType>(currentColumnOffset, negated ? -getValue(dd) : getValue(dd));
                }
                ++rowIndications[rowGroupOffsets[currentRowOffset]];
            } else {
                MTBDD elseElse;
                MTBDD elseThen;
                MTBDD thenElse;
                MTBDD thenThen;
                
                if (mtbdd_isleaf(dd) || ddColumnVariableIndices[currentColumnLevel] < mtbdd_getvar(dd)) {
                    elseElse = elseThen = thenElse = thenThen = dd;
                } else if (ddRowVariableIndices[currentColumnLevel] < mtbdd_getvar(dd)) {
                    elseElse = thenElse = mtbdd_getlow(dd);
                    elseThen = thenThen = mtbdd_gethigh(dd);
                } else {
                    MTBDD elseNode = mtbdd_getlow(dd);
                    if (mtbdd_isleaf(elseNode) || ddColumnVariableIndices[currentColumnLevel] < mtbdd_getvar(elseNode)) {
                        elseElse = elseThen = elseNode;
                    } else {
                        elseElse = mtbdd_getlow(elseNode);
                        elseThen = mtbdd_gethigh(elseNode);
                    }
                    
                    MTBDD thenNode = mtbdd_gethigh(dd);
                    if (mtbdd_isleaf(thenNode) || ddColumnVariableIndices[currentColumnLevel] < mtbdd_getvar(thenNode)) {
                        thenElse = thenThen = thenNode;
                    } else {
                        thenElse = mtbdd_getlow(thenNode);
                        thenThen = mtbdd_gethigh(thenNode);
                    }
                }
                
                // Visit else-else.
                toMatrixComponentsRec(mtbdd_regular(elseElse), mtbdd_isnegated(elseElse) ^ negated, rowGroupOffsets, rowIndications, columnsAndValues, rowOdd.getElseSuccessor(), columnOdd.getElseSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset, currentColumnOffset, ddRowVariableIndices, ddColumnVariableIndices, generateValues);
                // Visit else-then.
                toMatrixComponentsRec(mtbdd_regular(elseThen), mtbdd_isnegated(elseThen) ^ negated, rowGroupOffsets, rowIndications, columnsAndValues, rowOdd.getElseSuccessor(), columnOdd.getThenSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset, currentColumnOffset + columnOdd.getElseOffset(), ddRowVariableIndices, ddColumnVariableIndices, generateValues);
                // Visit then-else.
                toMatrixComponentsRec(mtbdd_regular(thenElse), mtbdd_isnegated(thenElse) ^ negated, rowGroupOffsets, rowIndications, columnsAndValues, rowOdd.getThenSuccessor(), columnOdd.getElseSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset + rowOdd.getElseOffset(), currentColumnOffset, ddRowVariableIndices, ddColumnVariableIndices, generateValues);
                // Visit then-then.
                toMatrixComponentsRec(mtbdd_regular(thenThen), mtbdd_isnegated(thenThen) ^ negated, rowGroupOffsets, rowIndications, columnsAndValues, rowOdd.getThenSuccessor(), columnOdd.getThenSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset + rowOdd.getElseOffset(), currentColumnOffset + columnOdd.getElseOffset(), ddRowVariableIndices, ddColumnVariableIndices, generateValues);
            }
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalAdd<DdType::Sylvan, ValueType>::fromVector(InternalDdManager<DdType::Sylvan> const* ddManager, std::vector<ValueType> const& values, storm::dd::Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices) {
            uint_fast64_t offset = 0;
            return InternalAdd<DdType::Sylvan, ValueType>(ddManager, sylvan::Mtbdd(fromVectorRec(offset, 0, ddVariableIndices.size(), values, odd, ddVariableIndices)));
        }
        
        template<typename ValueType>
        MTBDD InternalAdd<DdType::Sylvan, ValueType>::fromVectorRec(uint_fast64_t& currentOffset, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<ValueType> const& values, Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices) {
            if (currentLevel == maxLevel) {
                // If we are in a terminal node of the ODD, we need to check whether the then-offset of the ODD is one
                // (meaning the encoding is a valid one) or zero (meaning the encoding is not valid). Consequently, we
                // need to copy the next value of the vector iff the then-offset is greater than zero.
                if (odd.getThenOffset() > 0) {
                    return getLeaf(values[currentOffset++]);
                } else {
                    return getLeaf(storm::utility::zero<ValueType>());
                }
            } else {
                // If the total offset is zero, we can just return the constant zero DD.
                if (odd.getThenOffset() + odd.getElseOffset() == 0) {
                    return getLeaf(storm::utility::zero<ValueType>());
                }
                
                // Determine the new else-successor.
                MTBDD elseSuccessor;
                if (odd.getElseOffset() > 0) {
                    elseSuccessor = fromVectorRec(currentOffset, currentLevel + 1, maxLevel, values, odd.getElseSuccessor(), ddVariableIndices);
                } else {
                    elseSuccessor = getLeaf(storm::utility::zero<ValueType>());
                }
                mtbdd_refs_push(elseSuccessor);
                
                // Determine the new then-successor.
                MTBDD thenSuccessor;
                if (odd.getThenOffset() > 0) {
                    thenSuccessor = fromVectorRec(currentOffset, currentLevel + 1, maxLevel, values, odd.getThenSuccessor(), ddVariableIndices);
                } else {
                    thenSuccessor = getLeaf(storm::utility::zero<ValueType>());
                }
                mtbdd_refs_push(thenSuccessor);
                
                // Create a node representing ITE(currentVar, thenSuccessor, elseSuccessor);
                MTBDD currentVar = mtbdd_makenode(ddVariableIndices[currentLevel], mtbdd_false, mtbdd_true);
                mtbdd_refs_push(thenSuccessor);
                LACE_ME;
                MTBDD result = mtbdd_ite(currentVar, thenSuccessor, elseSuccessor);
                
                // Dispose of the intermediate results
                mtbdd_refs_pop(3);
                
                return result;
            }
        }
        
        template<typename ValueType>
        MTBDD InternalAdd<DdType::Sylvan, ValueType>::getLeaf(double value) {
            return mtbdd_double(value);
        }
        
        template<typename ValueType>
        MTBDD InternalAdd<DdType::Sylvan, ValueType>::getLeaf(uint_fast64_t value) {
            return mtbdd_uint64(value);
        }
        
        template<typename ValueType>
        ValueType InternalAdd<DdType::Sylvan, ValueType>::getValue(MTBDD const& node) {
            STORM_LOG_ASSERT(mtbdd_isleaf(node), "Expected leaf.");
            
            if (std::is_same<ValueType, double>::value) {
                STORM_LOG_ASSERT(mtbdd_gettype(node) == 1, "Expected a double value.");
                return mtbdd_getuint64(node);
            } else if (std::is_same<ValueType, uint_fast64_t>::value) {
                STORM_LOG_ASSERT(mtbdd_gettype(node) == 0, "Expected an unsigned value.");
                return mtbdd_getdouble(node);
            } else {
                STORM_LOG_ASSERT(false, "Illegal or unknown type in MTBDD.");
            }
        }
        
        template<typename ValueType>
        sylvan::Mtbdd InternalAdd<DdType::Sylvan, ValueType>::getSylvanMtbdd() const {
            return sylvanMtbdd;
        }
        
        template class InternalAdd<DdType::Sylvan, double>;
        template class InternalAdd<DdType::Sylvan, uint_fast64_t>;
    }
}