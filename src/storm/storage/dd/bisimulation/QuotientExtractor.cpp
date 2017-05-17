#include "storm/storage/dd/bisimulation/QuotientExtractor.h"

#include "storm/storage/dd/DdManager.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Ctmc.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/storage/dd/bisimulation/PreservationInformation.h"

#include "storm/settings/SettingsManager.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            QuotientExtractor<DdType, ValueType>::QuotientExtractor() : useRepresentatives(false) {
                auto const& settings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
                this->useRepresentatives = settings.isUseRepresentativesSet();
                this->quotientFormat = settings.getQuotientFormat();
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extract(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition) {
                auto start = std::chrono::high_resolution_clock::now();
                std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> result;
                if (quotientFormat == storm::settings::modules::BisimulationSettings::QuotientFormat::Sparse) {
                    result = extractSparseQuotient(model, partition);
                } else {
                    result = extractDdQuotient(model, partition);
                }
                auto end = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Quotient extraction completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");
                return result;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extractSparseQuotient(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition) {
                return nullptr;
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extractDdQuotient(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition) {
                return extractQuotientUsingBlockVariables(model, partition);
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extractQuotientUsingBlockVariables(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition) {
                auto modelType = model.getType();
                
                if (modelType == storm::models::ModelType::Dtmc || modelType == storm::models::ModelType::Ctmc) {
                    std::set<storm::expressions::Variable> blockVariableSet = {partition.getBlockVariable()};
                    std::set<storm::expressions::Variable> blockPrimeVariableSet = {partition.getPrimedBlockVariable()};
                    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> blockMetaVariablePairs = {std::make_pair(partition.getBlockVariable(), partition.getPrimedBlockVariable())};
                    
                    storm::dd::Bdd<DdType> partitionAsBdd = partition.storedAsBdd() ? partition.asBdd() : partition.asAdd().notZero();
                    if (useRepresentatives) {
                        storm::dd::Bdd<DdType> partitionAsBddOverPrimedBlockVariable = partitionAsBdd.renameVariables(blockVariableSet, blockPrimeVariableSet);
                        storm::dd::Bdd<DdType> representativePartition = partitionAsBddOverPrimedBlockVariable.existsAbstractRepresentative(model.getColumnVariables()).renameVariables(model.getColumnVariables(), blockVariableSet);
                        partitionAsBdd = (representativePartition && partitionAsBddOverPrimedBlockVariable).existsAbstract(blockPrimeVariableSet);
                    }
                    
                    storm::dd::Add<DdType, ValueType> partitionAsAdd = partitionAsBdd.template toAdd<ValueType>();
                    storm::dd::Add<DdType, ValueType> quotientTransitionMatrix = model.getTransitionMatrix().multiplyMatrix(partitionAsAdd.renameVariables(blockVariableSet, blockPrimeVariableSet), model.getColumnVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.multiplyMatrix(partitionAsAdd.renameVariables(model.getColumnVariables(), model.getRowVariables()), model.getRowVariables());
                    storm::dd::Bdd<DdType> quotientTransitionMatrixBdd = quotientTransitionMatrix.notZero();
                    
                    storm::dd::Bdd<DdType> partitionAsBddOverRowVariables = partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables());
                    storm::dd::Bdd<DdType> reachableStates = partitionAsBdd.existsAbstract(model.getColumnVariables());
                    storm::dd::Bdd<DdType> initialStates = (model.getInitialStates() && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables());
                    storm::dd::Bdd<DdType> deadlockStates = !quotientTransitionMatrixBdd.existsAbstract(blockPrimeVariableSet) && reachableStates;
                    
                    std::map<std::string, storm::dd::Bdd<DdType>> preservedLabelBdds;
                    for (auto const& label : partition.getPreservationInformation().getLabels()) {
                        preservedLabelBdds.emplace(label, (model.getStates(label) && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables()));
                    }
                    for (auto const& expression : partition.getPreservationInformation().getExpressions()) {
                        std::stringstream stream;
                        stream << expression;
                        std::string expressionAsString = stream.str();
                        
                        auto it = preservedLabelBdds.find(expressionAsString);
                        if (it != preservedLabelBdds.end()) {
                            STORM_LOG_WARN("Duplicate label '" << expressionAsString << "', dropping second label definition.");
                        } else {
                            preservedLabelBdds.emplace(stream.str(), (model.getStates(expression) && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables()));
                        }
                    }
                    
                    if (modelType == storm::models::ModelType::Dtmc) {
                        return std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>>(new storm::models::symbolic::Dtmc<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, blockVariableSet, blockPrimeVariableSet, blockMetaVariablePairs, preservedLabelBdds, {}));
                    } else {
                        return std::shared_ptr<storm::models::symbolic::Ctmc<DdType, ValueType>>(new storm::models::symbolic::Ctmc<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, blockVariableSet, blockPrimeVariableSet, blockMetaVariablePairs, preservedLabelBdds, {}));
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Cannot exctract quotient for this model type.");
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> QuotientExtractor<DdType, ValueType>::extractQuotientUsingOriginalVariables(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition) {
                auto modelType = model.getType();
                
                if (modelType == storm::models::ModelType::Dtmc || modelType == storm::models::ModelType::Ctmc) {
                    std::set<storm::expressions::Variable> blockVariableSet = {partition.getBlockVariable()};
                    std::set<storm::expressions::Variable> blockPrimeVariableSet = {partition.getPrimedBlockVariable()};
                    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> blockMetaVariablePairs = {std::make_pair(partition.getBlockVariable(), partition.getPrimedBlockVariable())};
                    
                    storm::dd::Add<DdType, ValueType> partitionAsAdd = partition.storedAsBdd() ? partition.asBdd().template toAdd<ValueType>() : partition.asAdd();
                    storm::dd::Add<DdType, ValueType> quotientTransitionMatrix = model.getTransitionMatrix().multiplyMatrix(partitionAsAdd, model.getColumnVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.renameVariables(blockVariableSet, model.getColumnVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.multiplyMatrix(partitionAsAdd, model.getRowVariables());
                    quotientTransitionMatrix = quotientTransitionMatrix.renameVariables(blockVariableSet, model.getRowVariables());
                    storm::dd::Bdd<DdType> quotientTransitionMatrixBdd = quotientTransitionMatrix.notZero();
                    
                    storm::dd::Bdd<DdType> partitionAsBdd = partition.storedAsBdd() ? partition.asBdd() : partition.asAdd().notZero();
                    storm::dd::Bdd<DdType> partitionAsBddOverRowVariables = partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables());
                    storm::dd::Bdd<DdType> reachableStates = partitionAsBdd.existsAbstract(model.getColumnVariables()).renameVariables(blockVariableSet, model.getRowVariables());
                    storm::dd::Bdd<DdType> initialStates = (model.getInitialStates() && partitionAsBdd.renameVariables(model.getColumnVariables(), model.getRowVariables())).existsAbstract(model.getRowVariables()).renameVariables(blockVariableSet, model.getRowVariables());
                    storm::dd::Bdd<DdType> deadlockStates = !quotientTransitionMatrixBdd.existsAbstract(model.getColumnVariables()) && reachableStates;
                    
                    std::map<std::string, storm::dd::Bdd<DdType>> preservedLabelBdds;
                    for (auto const& label : partition.getPreservationInformation().getLabels()) {
                        preservedLabelBdds.emplace(label, (model.getStates(label) && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables()));
                    }
                    for (auto const& expression : partition.getPreservationInformation().getExpressions()) {
                        std::stringstream stream;
                        stream << expression;
                        std::string expressionAsString = stream.str();
                        
                        auto it = preservedLabelBdds.find(expressionAsString);
                        if (it != preservedLabelBdds.end()) {
                            STORM_LOG_WARN("Duplicate label '" << expressionAsString << "', dropping second label definition.");
                        } else {
                            preservedLabelBdds.emplace(stream.str(), (model.getStates(expression) && partitionAsBddOverRowVariables).existsAbstract(model.getRowVariables()));
                        }
                    }
                    
                    if (modelType == storm::models::ModelType::Dtmc) {
                        return std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>>(new storm::models::symbolic::Dtmc<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, model.getRowVariables(), model.getColumnVariables(), model.getRowColumnMetaVariablePairs(), preservedLabelBdds, {}));
                    } else {
                        return std::shared_ptr<storm::models::symbolic::Ctmc<DdType, ValueType>>(new storm::models::symbolic::Ctmc<DdType, ValueType>(model.getManager().asSharedPointer(), reachableStates, initialStates, deadlockStates, quotientTransitionMatrix, blockVariableSet, blockPrimeVariableSet, blockMetaVariablePairs, preservedLabelBdds, {}));
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Cannot exctract quotient for this model type.");
                }
            }
            
            template class QuotientExtractor<storm::dd::DdType::CUDD, double>;
            
            template class QuotientExtractor<storm::dd::DdType::Sylvan, double>;
            template class QuotientExtractor<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class QuotientExtractor<storm::dd::DdType::Sylvan, storm::RationalFunction>;
            
        }
    }
}
