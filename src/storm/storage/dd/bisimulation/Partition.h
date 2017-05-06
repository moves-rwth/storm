#pragma once

#include <boost/variant.hpp>

#include "storm/storage/dd/DdType.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

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
                
                /*!
                 * Creates a new partition from the given data.
                 *
                 * @param partitionBdd A BDD that maps encoding over the state/row variables and the block variable to
                 * true iff the state is in the block.
                 * @param blockVariable The variable to use for the block encoding. Its range must be [0, x] where x is
                 * the number of states in the partition.
                 * @param nextFreeBlockIndex The next free block index. The existing blocks must be encoded with indices
                 * between 0 and this number.
                 */
                Partition(storm::dd::Bdd<DdType> const& partitionBdd, storm::expressions::Variable const& blockVariable, uint64_t nextFreeBlockIndex);
                
                bool operator==(Partition<DdType, ValueType> const& other);
                
                Partition<DdType, ValueType> replacePartition(storm::dd::Add<DdType, ValueType> const& newPartitionAdd, uint64_t nextFreeBlockIndex) const;

                Partition<DdType, ValueType> replacePartition(storm::dd::Bdd<DdType> const& newPartitionBdd, uint64_t nextFreeBlockIndex) const;

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
                
                bool storedAsAdd() const;
                bool storedAsBdd() const;
                
                storm::dd::Add<DdType, ValueType> const& asAdd() const;
                storm::dd::Bdd<DdType> const& asBdd() const;
                
                storm::expressions::Variable const& getBlockVariable() const;
                
                uint64_t getNextFreeBlockIndex() const;
                
                uint64_t getNodeCount() const;
                
            private:
                static std::pair<storm::dd::Bdd<DdType>, uint64_t> createPartitionBdd(storm::dd::DdManager<DdType> const& manager, storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::dd::Bdd<DdType>> const& stateSets, storm::expressions::Variable const& blockVariable);
                
                static storm::expressions::Variable createBlockVariable(storm::dd::DdManager<DdType>& manager, uint64_t numberOfStates);
                
                /// The DD representing the partition. The DD is over the row variables of the model and the block variable.
                boost::variant<storm::dd::Bdd<DdType>, storm::dd::Add<DdType, ValueType>> partition;
                
                /// The meta variable used to encode the block of each state in this partition.
                storm::expressions::Variable blockVariable;
                
                /// The next free block index.
                uint64_t nextFreeBlockIndex;
            };
            
        }
    }
}
