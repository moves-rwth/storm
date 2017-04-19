#pragma once

#include "storm/storage/dd/DdType.h"
#include "storm/storage/dd/Add.h"

#include "storm/models/symbolic/Model.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class Partition {
            public:
                Partition() = default;
                
                /*!
                 * Creates a new partition from the given data.
                 *
                 * @param partitionAdd An ADD that maps encoding over the state/row variables and the block variable to
                 * one iff the state is in the block.
                 * @param blockVariable The variable to use for the block encoding. Its range must be [0, x] where x is
                 * the number of states in the partition.
                 * @param nextFreeBlockIndex The next free block index. The existing blocks must be encoded with indices
                 * between 0 and this number.
                 */
                Partition(storm::dd::Add<DdType, ValueType> const& partitionAdd, storm::expressions::Variable const& blockVariable, uint64_t nextFreeBlockIndex);
                
                bool operator==(Partition<DdType, ValueType> const& other);
                
                Partition<DdType, ValueType> replacePartitionAdd(storm::dd::Add<DdType, ValueType> const& newPartitionAdd, uint64_t nextFreeBlockIndex) const;
                
                /*!
                 * Creates a partition from the given model that respects all labels.
                 */
                static Partition create(storm::models::symbolic::Model<DdType, ValueType> const& model);

                /*!
                 * Creates a partition from the given model that respects the given labels.
                 */
                static Partition create(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::string> const& labels);

                /*!
                 * Creates a partition from the given model that respects the given expressions.
                 */
                static Partition create(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::expressions::Expression> const& expressions);

                uint64_t getNumberOfBlocks() const;
                
                storm::dd::Add<DdType, ValueType> const& getPartitionAdd() const;
                
                storm::expressions::Variable const& getBlockVariable() const;
                
                uint64_t getNextFreeBlockIndex() const;
                
            private:
                static std::pair<storm::dd::Add<DdType, ValueType>, uint64_t> createPartitionAdd(storm::dd::DdManager<DdType> const& manager, storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::dd::Bdd<DdType>> const& stateSets, storm::expressions::Variable const& blockVariable);
                
                static storm::expressions::Variable createBlockVariable(storm::dd::DdManager<DdType>& manager, uint64_t numberOfStates);
                
                // The ADD representing the partition. The ADD is over the row variables of the model and the block variable.
                storm::dd::Add<DdType, ValueType> partitionAdd;
                
                // The meta variable used to encode the block of each state in this partition.
                storm::expressions::Variable blockVariable;
                
                // The next free block index.
                uint64_t nextFreeBlockIndex;
            };
            
        }
    }
}
