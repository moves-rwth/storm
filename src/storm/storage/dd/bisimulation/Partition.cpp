#include "storm/storage/dd/bisimulation/Partition.h"

#include "storm/storage/dd/DdManager.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType>::Partition(storm::dd::Add<DdType, ValueType> const& partitionAdd, storm::expressions::Variable const& blockVariable, uint64_t nextFreeBlockIndex) : partitionAdd(partitionAdd), blockVariable(blockVariable), nextFreeBlockIndex(nextFreeBlockIndex) {
                // Intentionally left empty.
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            bool Partition<DdType, ValueType>::operator==(Partition<DdType, ValueType> const& other) {
                return this->partitionAdd == other.partitionAdd && this->blockVariable == other.blockVariable && this->nextFreeBlockIndex == other.nextFreeBlockIndex;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::replacePartitionAdd(storm::dd::Add<DdType, ValueType> const& newPartitionAdd, uint64_t nextFreeBlockIndex) const {
                return Partition<DdType, ValueType>(newPartitionAdd, blockVariable, nextFreeBlockIndex);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::create(storm::models::symbolic::Model<DdType, ValueType> const& model) {
                return create(model, model.getLabels());
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::create(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::string> const& labels) {
                std::vector<storm::expressions::Expression> expressions;
                for (auto const& label : labels) {
                    expressions.push_back(model.getExpression(label));
                }
                return create(model, expressions);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::create(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::expressions::Expression> const& expressions) {
                storm::dd::DdManager<DdType>& manager = model.getManager();
                
                std::vector<storm::dd::Bdd<DdType>> stateSets;
                for (auto const& expression : expressions) {
                    stateSets.emplace_back(model.getStates(expression));
                }
                
                storm::expressions::Variable blockVariable = createBlockVariable(manager, model.getReachableStates().getNonZeroCount());
                std::pair<storm::dd::Add<DdType, ValueType>, uint64_t> partitionAddAndBlockCount = createPartitionAdd(manager, model, stateSets, blockVariable);
                return Partition<DdType, ValueType>(partitionAddAndBlockCount.first, blockVariable, partitionAddAndBlockCount.second);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            uint64_t Partition<DdType, ValueType>::getNumberOfBlocks() const {
                return nextFreeBlockIndex;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            storm::dd::Add<DdType, ValueType> const& Partition<DdType, ValueType>::getPartitionAdd() const {
                return partitionAdd;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            storm::expressions::Variable const& Partition<DdType, ValueType>::getBlockVariable() const {
                return blockVariable;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            uint64_t Partition<DdType, ValueType>::getNextFreeBlockIndex() const {
                return nextFreeBlockIndex;
            }
            
            template<storm::dd::DdType DdType>
            void enumerateBlocksRec(std::vector<storm::dd::Bdd<DdType>> const& stateSets, storm::dd::Bdd<DdType> const& currentStateSet, uint64_t offset, storm::expressions::Variable const& blockVariable, std::function<void (storm::dd::Bdd<DdType> const&)> const& callback) {
                if (currentStateSet.isZero()) {
                    return;
                }
                if (offset == stateSets.size()) {
                    callback(currentStateSet);
                } else {
                    enumerateBlocksRec(stateSets, currentStateSet && stateSets[offset], offset + 1, blockVariable, callback);
                    enumerateBlocksRec(stateSets, currentStateSet && !stateSets[offset], offset + 1, blockVariable, callback);
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::pair<storm::dd::Add<DdType, ValueType>, uint64_t> Partition<DdType, ValueType>::createPartitionAdd(storm::dd::DdManager<DdType> const& manager, storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::dd::Bdd<DdType>> const& stateSets, storm::expressions::Variable const& blockVariable) {
                uint64_t blockCount = 0;
                storm::dd::Add<DdType, ValueType> partitionAdd = manager.template getAddZero<ValueType>();
                
                // Enumerate all realizable blocks.
                enumerateBlocksRec<DdType>(stateSets, model.getReachableStates(), 0, blockVariable, [&manager, &partitionAdd, &blockVariable, &blockCount](storm::dd::Bdd<DdType> const& stateSet) {
                    stateSet.template toAdd<ValueType>().exportToDot("states_" + std::to_string(blockCount) + ".dot");
                    partitionAdd += (stateSet && manager.getEncoding(blockVariable, blockCount)).template toAdd<ValueType>();
                    blockCount++;
                } );
                
                return std::make_pair(partitionAdd, blockCount);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            storm::expressions::Variable Partition<DdType, ValueType>::createBlockVariable(storm::dd::DdManager<DdType>& manager, uint64_t numberOfStates) {
                storm::expressions::Variable blockVariable;
                if (manager.hasMetaVariable("blocks")) {
                    int64_t counter = 0;
                    while (manager.hasMetaVariable("block" + std::to_string(counter))) {
                        ++counter;
                    }
                    blockVariable = manager.addMetaVariable("blocks" + std::to_string(counter), 0, numberOfStates, 1).front();
                } else {
                    blockVariable = manager.addMetaVariable("blocks", 0, numberOfStates, 1).front();
                }
                return blockVariable;
            }
            
            template class Partition<storm::dd::DdType::CUDD, double>;

            template class Partition<storm::dd::DdType::Sylvan, double>;
            template class Partition<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class Partition<storm::dd::DdType::Sylvan, storm::RationalFunction>;

        }
    }
}
