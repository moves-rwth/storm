#include "ExplicitExporter.h"

#include "src/adapters/CarlAdapter.h"
#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"

#include "src/models/sparse/StandardRewardModel.h"


namespace storm {
    namespace exporter {
        template<typename ValueType>
        void explicitExportSparseModel(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel,  std::vector<std::string> const& parameters) {
            STORM_LOG_THROW(sparseModel->getType() == storm::models::ModelType::Mdp || sparseModel->getType() == storm::models::ModelType::Dtmc , storm::exceptions::NotImplementedException, "This functionality is not yet implemented." );
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp = sparseModel->template as<storm::models::sparse::Mdp<ValueType>>();

            os << "// Exported by Storm" << std::endl;
            os << "@type: mdp" << std::endl;
            os << "@parameters" << std::endl;
            for (auto const& p : parameters) {
               os << p << " ";
            }
            os << std::endl;
            os << "@nr_states" << std::endl  << sparseModel->getNumberOfStates() <<  std::endl;
            os << "@model" << std::endl;
            storm::storage::SparseMatrix<ValueType> const& matrix = sparseModel->getTransitionMatrix();
            
            for (typename storm::storage::SparseMatrix<ValueType>::index_type group = 0; group < matrix.getRowGroupCount(); ++group) {
                os << "state " << group;
                
                bool first = true;
                for (auto const& rewardModelEntry : sparseModel->getRewardModels()) {
                    if (first) {
                        os << " [";
                        first = false;
                    } else {
                        os << ", ";
                    }
                    
                    if(rewardModelEntry.second.hasStateRewards()) {
                        os << rewardModelEntry.second.getStateRewardVector().at(group);
                    } else {
                        os << "0";
                    }
                    
                }
                if (!first) {
                    os << "]";
                }
                
                for(auto const& label : sparseModel->getStateLabeling().getLabelsOfState(group)) {
                    os << " " << label;
                }
                os << std::endl;
                typename storm::storage::SparseMatrix<ValueType>::index_type start = matrix.hasTrivialRowGrouping() ? group : matrix.getRowGroupIndices()[group];
                typename storm::storage::SparseMatrix<ValueType>::index_type end = matrix.hasTrivialRowGrouping() ? group + 1 : matrix.getRowGroupIndices()[group + 1];
                
                for (typename storm::storage::SparseMatrix<ValueType>::index_type i = start; i < end; ++i) {
                    // Print the actual row.
                    os << "\taction";
                    bool first = true;
                    for (auto const& rewardModelEntry : sparseModel->getRewardModels()) {
                        if (first) {
                            os << " [";
                            first = false;
                        } else {
                            os << ", ";
                        }
                        
                        if(rewardModelEntry.second.hasStateActionRewards()) {
                            os << storm::utility::to_string(rewardModelEntry.second.getStateActionRewardVector().at(i));
                        } else {
                            os << "0";
                        }
                        
                    }
                    if (!first) {
                        os << "]";
                    }

                    
                    
                    if(sparseModel->hasChoiceLabeling()) {
                        //TODO
                    }
                    os << std::endl;
                    for(auto it = matrix.begin(i); it != matrix.end(i); ++it) {
                        os << "\t\t" << it->getColumn() << " : " << storm::utility::to_string(it->getValue()) << std::endl;
                    }
                    
                }
            }
            
        }
        
        
        
        template void explicitExportSparseModel<double>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<double>> sparseModel, std::vector<std::string> const& parameters);
        
        template void explicitExportSparseModel<storm::RationalNumber>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> sparseModel, std::vector<std::string> const& parameters);
        template void explicitExportSparseModel<storm::RationalFunction>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> sparseModel, std::vector<std::string> const& parameters);
    }
}