#include "storm/storage/dd/bisimulation/Partition.h"

#include "storm/storage/dd/DdManager.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType>::Partition(storm::dd::Add<DdType, ValueType> const& partitionAdd, storm::expressions::Variable const& blockVariable, uint64_t nextFreeBlockIndex) : partition(partitionAdd), blockVariable(blockVariable), nextFreeBlockIndex(nextFreeBlockIndex) {
                // Intentionally left empty.
            }

            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType>::Partition(storm::dd::Bdd<DdType> const& partitionBdd, storm::expressions::Variable const& blockVariable, uint64_t nextFreeBlockIndex) : partition(partitionBdd), blockVariable(blockVariable), nextFreeBlockIndex(nextFreeBlockIndex) {
                // Intentionally left empty.
            }

            template<storm::dd::DdType DdType, typename ValueType>
            bool Partition<DdType, ValueType>::operator==(Partition<DdType, ValueType> const& other) {
                return this->partition == other.partition && this->blockVariable == other.blockVariable && this->nextFreeBlockIndex == other.nextFreeBlockIndex;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::replacePartition(storm::dd::Add<DdType, ValueType> const& newPartitionAdd, uint64_t nextFreeBlockIndex) const {
                return Partition<DdType, ValueType>(newPartitionAdd, blockVariable, nextFreeBlockIndex);
            }

            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::replacePartition(storm::dd::Bdd<DdType> const& newPartitionBdd, uint64_t nextFreeBlockIndex) const {
                return Partition<DdType, ValueType>(newPartitionBdd, blockVariable, nextFreeBlockIndex);
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
                std::pair<storm::dd::Bdd<DdType>, uint64_t> partitionBddAndBlockCount = createPartitionBdd(manager, model, stateSets, blockVariable);
                
                // Store the partition as an ADD only in the case of CUDD.
                if (DdType == storm::dd::DdType::CUDD) {
                    return Partition<DdType, ValueType>(partitionBddAndBlockCount.first.template toAdd<ValueType>(), blockVariable, partitionBddAndBlockCount.second);
                } else {
                    return Partition<DdType, ValueType>(partitionBddAndBlockCount.first, blockVariable, partitionBddAndBlockCount.second);
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            uint64_t Partition<DdType, ValueType>::getNumberOfBlocks() const {
                return nextFreeBlockIndex;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            bool Partition<DdType, ValueType>::storedAsAdd() const {
                return partition.which() == 1;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            bool Partition<DdType, ValueType>::storedAsBdd() const {
                return partition.which() == 0;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            storm::dd::Add<DdType, ValueType> const& Partition<DdType, ValueType>::asAdd() const {
                return boost::get<storm::dd::Add<DdType, ValueType>>(partition);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> const& Partition<DdType, ValueType>::asBdd() const {
                return boost::get<storm::dd::Bdd<DdType>>(partition);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            storm::expressions::Variable const& Partition<DdType, ValueType>::getBlockVariable() const {
                return blockVariable;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            uint64_t Partition<DdType, ValueType>::getNextFreeBlockIndex() const {
                return nextFreeBlockIndex;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            uint64_t Partition<DdType, ValueType>::getNodeCount() const {
                if (this->storedAsBdd()) {
                    return asBdd().getNodeCount();
                } else {
                    return asAdd().getNodeCount();
                }
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
            std::pair<storm::dd::Bdd<DdType>, uint64_t> Partition<DdType, ValueType>::createPartitionBdd(storm::dd::DdManager<DdType> const& manager, storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::dd::Bdd<DdType>> const& stateSets, storm::expressions::Variable const& blockVariable) {
                uint64_t blockCount = 0;
                storm::dd::Bdd<DdType> partitionAdd = manager.getBddZero();
                
                // Enumerate all realizable blocks.
                enumerateBlocksRec<DdType>(stateSets, model.getReachableStates(), 0, blockVariable, [&manager, &partitionAdd, &blockVariable, &blockCount](storm::dd::Bdd<DdType> const& stateSet) {
                    stateSet.template toAdd<ValueType>().exportToDot("states_" + std::to_string(blockCount) + ".dot");
                    partitionAdd |= (stateSet && manager.getEncoding(blockVariable, blockCount, false));
                    blockCount++;
                } );
                
                // Move the partition over to the primed variables.
                partitionAdd = partitionAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                
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
