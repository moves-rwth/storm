#include "storm/storage/dd/bisimulation/Partition.h"

#include "storm/storage/dd/DdManager.h"

#include "storm/storage/dd/bisimulation/PreservationInformation.h"

#include "storm/logic/Formula.h"
#include "storm/logic/AtomicExpressionFormula.h"
#include "storm/logic/AtomicLabelFormula.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BisimulationSettings.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType>::Partition(std::shared_ptr<PreservationInformation> preservationInformation, storm::dd::Add<DdType, ValueType> const& partitionAdd, std::pair<storm::expressions::Variable, storm::expressions::Variable> const& blockVariables, uint64_t nextFreeBlockIndex) : preservationInformation(preservationInformation), partition(partitionAdd), blockVariables(blockVariables), nextFreeBlockIndex(nextFreeBlockIndex) {
                // Intentionally left empty.
            }

            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType>::Partition(std::shared_ptr<PreservationInformation> preservationInformation, storm::dd::Bdd<DdType> const& partitionBdd, std::pair<storm::expressions::Variable, storm::expressions::Variable> const& blockVariables, uint64_t nextFreeBlockIndex) : preservationInformation(preservationInformation), partition(partitionBdd), blockVariables(blockVariables), nextFreeBlockIndex(nextFreeBlockIndex) {
                // Intentionally left empty.
            }

            template<storm::dd::DdType DdType, typename ValueType>
            bool Partition<DdType, ValueType>::operator==(Partition<DdType, ValueType> const& other) {
                return this->partition == other.partition && this->blockVariables == other.blockVariables && this->nextFreeBlockIndex == other.nextFreeBlockIndex;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::replacePartition(storm::dd::Add<DdType, ValueType> const& newPartitionAdd, uint64_t nextFreeBlockIndex) const {
                return Partition<DdType, ValueType>(preservationInformation, newPartitionAdd, blockVariables, nextFreeBlockIndex);
            }

            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::replacePartition(storm::dd::Bdd<DdType> const& newPartitionBdd, uint64_t nextFreeBlockIndex) const {
                return Partition<DdType, ValueType>(preservationInformation, newPartitionBdd, blockVariables, nextFreeBlockIndex);
            }

            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::create(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::storage::BisimulationType const& bisimulationType) {
                return create(model, model.getLabels(), bisimulationType);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::create(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::string> const& labels, storm::storage::BisimulationType const& bisimulationType) {
                std::shared_ptr<PreservationInformation> preservationInformation = std::make_shared<PreservationInformation>();
                std::vector<storm::expressions::Expression> expressions;
                for (auto const& label : labels) {
                    preservationInformation->addLabel(label);
                    expressions.push_back(model.getExpression(label));
                }
                return create(model, expressions, preservationInformation, bisimulationType);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::create(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::expressions::Expression> const& expressions, storm::storage::BisimulationType const& bisimulationType) {
                std::shared_ptr<PreservationInformation> preservationInformation = std::make_shared<PreservationInformation>();
                for (auto const& expression : expressions) {
                    preservationInformation->addExpression(expression);
                }
                return create(model, expressions, preservationInformation, bisimulationType);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::create(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType const& bisimulationType) {
                if (formulas.empty()) {
                    return create(model, bisimulationType);
                }
                    
                std::shared_ptr<PreservationInformation> preservationInformation = std::make_shared<PreservationInformation>();
                std::set<std::string> labels;
                std::set<storm::expressions::Expression> expressions;
                
                for (auto const& formula : formulas) {
                    for (auto const& expressionFormula : formula->getAtomicExpressionFormulas()) {
                        expressions.insert(expressionFormula->getExpression());
                        preservationInformation->addExpression(expressionFormula->getExpression());
                    }
                    for (auto const& labelFormula : formula->getAtomicLabelFormulas()) {
                        std::string const& label = labelFormula->getLabel();
                        STORM_LOG_THROW(model.hasLabel(label), storm::exceptions::InvalidPropertyException, "Property refers to illegal label '" << label << "'.");
                        preservationInformation->addLabel(label);
                        expressions.insert(model.getExpression(label));
                    }
                }
                
                std::vector<storm::expressions::Expression> expressionVector;
                for (auto const& expression : expressions) {
                    expressionVector.emplace_back(expression);
                }
                
                return create(model, expressionVector, preservationInformation, bisimulationType);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> Partition<DdType, ValueType>::create(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::expressions::Expression> const& expressions, std::shared_ptr<PreservationInformation> const& preservationInformation, storm::storage::BisimulationType const& bisimulationType) {
                
                STORM_LOG_THROW(bisimulationType == storm::storage::BisimulationType::Strong, storm::exceptions::NotSupportedException, "Currently only strong bisimulation is supported.");
                
                storm::dd::DdManager<DdType>& manager = model.getManager();
                
                std::vector<storm::dd::Bdd<DdType>> stateSets;
                for (auto const& expression : expressions) {
                    stateSets.emplace_back(model.getStates(expression));
                }
                
                uint64_t numberOfDdVariables = 0;
                for (auto const& metaVariable : model.getRowVariables()) {
                    auto const& ddMetaVariable = manager.getMetaVariable(metaVariable);
                    numberOfDdVariables += ddMetaVariable.getNumberOfDdVariables();
                }
                
                std::pair<storm::expressions::Variable, storm::expressions::Variable> blockVariables = createBlockVariables(manager, numberOfDdVariables);
                std::pair<storm::dd::Bdd<DdType>, uint64_t> partitionBddAndBlockCount = createPartitionBdd(manager, model, stateSets, blockVariables.first);
                
                // Store the partition as an ADD only in the case of CUDD.
                if (DdType == storm::dd::DdType::CUDD) {
                    return Partition<DdType, ValueType>(preservationInformation, partitionBddAndBlockCount.first.template toAdd<ValueType>(), blockVariables, partitionBddAndBlockCount.second);
                } else {
                    return Partition<DdType, ValueType>(preservationInformation, partitionBddAndBlockCount.first, blockVariables, partitionBddAndBlockCount.second);
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            uint64_t Partition<DdType, ValueType>::getNumberOfStates() const {
                return this->getStates().getNonZeroCount();
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> Partition<DdType, ValueType>::getStates() const {
                if (this->storedAsAdd()) {
                    return this->asAdd().notZero().existsAbstract({this->getBlockVariable()});
                } else {
                    return this->asBdd().existsAbstract({this->getBlockVariable()});
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
                return blockVariables.first;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            storm::expressions::Variable const& Partition<DdType, ValueType>::getPrimedBlockVariable() const {
                return blockVariables.second;
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
            
            template<storm::dd::DdType DdType, typename ValueType>
            PreservationInformation const& Partition<DdType, ValueType>::getPreservationInformation() const {
                return *preservationInformation;
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
                storm::dd::Bdd<DdType> partitionBdd = manager.getBddZero();
                
                // Enumerate all realizable blocks.
                enumerateBlocksRec<DdType>(stateSets, model.getReachableStates(), 0, blockVariable, [&manager, &partitionBdd, &blockVariable, &blockCount](storm::dd::Bdd<DdType> const& stateSet) {
                    partitionBdd |= (stateSet && manager.getEncoding(blockVariable, blockCount, false));
                    blockCount++;
                } );

                // Move the partition over to the primed variables.
                partitionBdd = partitionBdd.swapVariables(model.getRowColumnMetaVariablePairs());

                return std::make_pair(partitionBdd, blockCount);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::pair<storm::expressions::Variable, storm::expressions::Variable> Partition<DdType, ValueType>::createBlockVariables(storm::dd::DdManager<DdType>& manager, uint64_t numberOfDdVariables) {
                std::vector<storm::expressions::Variable> blockVariables;
                if (manager.hasMetaVariable("blocks")) {
                    int64_t counter = 0;
                    while (manager.hasMetaVariable("block" + std::to_string(counter))) {
                        ++counter;
                    }
                    blockVariables = manager.addBitVectorMetaVariable("blocks" + std::to_string(counter), numberOfDdVariables, 2);
                } else {
                    blockVariables = manager.addBitVectorMetaVariable("blocks", numberOfDdVariables, 2);
                }
                return std::make_pair(blockVariables[0], blockVariables[1]);
            }
            
            template class Partition<storm::dd::DdType::CUDD, double>;

            template class Partition<storm::dd::DdType::Sylvan, double>;
            template class Partition<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class Partition<storm::dd::DdType::Sylvan, storm::RationalFunction>;

        }
    }
}
