#pragma once

#include <vector>
#include <memory>

#include <boost/variant.hpp>

#include "storm/storage/dd/DdType.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/storage/bisimulation/BisimulationType.h"

#include "storm/models/symbolic/Model.h"
#include "storm/models/symbolic/NondeterministicModel.h"

namespace storm {
    namespace logic {
        class Formula;
    }
    
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class PreservationInformation;
            
            template<storm::dd::DdType DdType, typename ValueType>
            class Partition {
            public:
                Partition();
                
                bool operator==(Partition<DdType, ValueType> const& other);
                
                Partition<DdType, ValueType> replacePartition(storm::dd::Add<DdType, ValueType> const& newPartitionAdd, uint64_t numberOfBlocks, uint64_t nextFreeBlockIndex) const;
                Partition<DdType, ValueType> replacePartition(storm::dd::Bdd<DdType> const& newPartitionBdd, uint64_t numberOfBlocks, uint64_t nextFreeBlockIndex) const;

                static Partition create(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::storage::BisimulationType const& bisimulationType, PreservationInformation<DdType, ValueType> const& preservationInformation);
                static Partition create(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::storage::BisimulationType const& bisimulationType, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas);
                static Partition createTrivialChoicePartition(storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, std::pair<storm::expressions::Variable, storm::expressions::Variable> const& blockVariables);
                
                uint64_t getNumberOfStates() const;
                uint64_t getNumberOfBlocks() const;
                
                bool storedAsAdd() const;
                bool storedAsBdd() const;
                
                storm::dd::Add<DdType, ValueType> const& asAdd() const;
                storm::dd::Bdd<DdType> const& asBdd() const;

                std::pair<storm::expressions::Variable, storm::expressions::Variable> const& getBlockVariables() const;
                storm::expressions::Variable const& getBlockVariable() const;
                storm::expressions::Variable const& getPrimedBlockVariable() const;
                
                uint64_t getNextFreeBlockIndex() const;
                uint64_t getNodeCount() const;

                storm::dd::Bdd<DdType> getStates() const;
                                
            private:
                /*!
                 * Creates a new partition from the given data.
                 *
                 * @param partitionAdd An ADD that maps encoding over the state/row variables and the block variable to
                 * one iff the state is in the block.
                 * @param blockVariables The variables to use for the block encoding. Its range must be [0, x] where x is
                 * greater or equal than the number of states in the partition.
                 * @param numberOfBlocks The number of blocks in this partition.
                 * @param nextFreeBlockIndex The next free block index.
                 */
                Partition(storm::dd::Add<DdType, ValueType> const& partitionAdd, std::pair<storm::expressions::Variable, storm::expressions::Variable> const& blockVariables, uint64_t numberOfBlocks, uint64_t nextFreeBlockIndex);
                
                /*!
                 * Creates a new partition from the given data.
                 *
                 * @param partitionBdd A BDD that maps encoding over the state/row variables and the block variable to
                 * true iff the state is in the block.
                 * @param blockVariables The variables to use for the block encoding. Their range must be [0, x] where x is
                 * greater or equal than the number of states in the partition.
                 * @param numberOfBlocks The number of blocks in this partition.
                 * @param nextFreeBlockIndex The next free block index.
                 */
                Partition(storm::dd::Bdd<DdType> const& partitionBdd, std::pair<storm::expressions::Variable, storm::expressions::Variable> const& blockVariables, uint64_t numberOfBlocks, uint64_t nextFreeBlockIndex);
                
                /*!
                 * Creates a partition from the given model that respects the given expressions.
                 */
                static Partition create(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::expressions::Expression> const& expressions, storm::storage::BisimulationType const& bisimulationType);
                
                static Partition<DdType, ValueType> createDistanceBased(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::logic::Formula const& constraintFormula, storm::logic::Formula const& targetFormula);
                static Partition<DdType, ValueType> createDistanceBased(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Bdd<DdType> const& constraintStates, storm::dd::Bdd<DdType> const& targetStates);
                static boost::optional<std::pair<std::shared_ptr<storm::logic::Formula const>, std::shared_ptr<storm::logic::Formula const>>> extractConstraintTargetFormulas(storm::logic::Formula const& formula);
                
                static std::pair<storm::dd::Bdd<DdType>, uint64_t> createPartitionBdd(storm::dd::DdManager<DdType> const& manager, storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::dd::Bdd<DdType>> const& stateSets, storm::expressions::Variable const& blockVariable);
                
                static std::pair<storm::expressions::Variable, storm::expressions::Variable> createBlockVariables(storm::models::symbolic::Model<DdType, ValueType> const& model);
                static std::pair<storm::expressions::Variable, storm::expressions::Variable> createBlockVariables(storm::dd::DdManager<DdType>& manager, uint64_t numberOfDdVariables);
                
                /// The DD representing the partition. The DD is over the row variables of the model and the block variable.
                boost::variant<storm::dd::Bdd<DdType>, storm::dd::Add<DdType, ValueType>> partition;
                
                /// The meta variables used to encode the block of each state in this partition.
                std::pair<storm::expressions::Variable, storm::expressions::Variable> blockVariables;
                
                /// The number of blocks in this partition.
                uint64_t numberOfBlocks;
                
                /// The next free block index.
                uint64_t nextFreeBlockIndex;
            };
            
        }
    }
}
