#include "ExplicitExporter.h"

#include "src/adapters/CarlAdapter.h"
#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"

#include "src/models/sparse/StandardRewardModel.h"


namespace storm {
    namespace exporter {
        template<typename ValueType>
        void explicitExportSparseModel(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel,  std::vector<std::string> const& parameters) {
            STORM_LOG_THROW(sparseModel->getType() == storm::models::ModelType::Mdp, storm::exceptions::NotImplementedException, "This functionality is not yet implemented." );
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp = sparseModel->template as<storm::models::sparse::Mdp<ValueType>>();

            os << "// Exported by Storm" << std::endl;
            os << "@type: mdp" << std::endl;
            os << "@parameters" << std::endl;
            for (auto const& p : parameters) {
               os << p << " ";
            }
            os << std::endl;
            os << "@nr_states" << std::endl  << mdp->getNumberOfStates() <<  std::endl;
            os << "@model" << std::endl;
            storm::storage::SparseMatrix<ValueType> const& matrix = mdp->getTransitionMatrix();
            
            for (typename storm::storage::SparseMatrix<ValueType>::index_type group = 0; group < matrix.getRowGroupCount(); ++group) {
                os << "state " << group;
                for(auto const& label : mdp->getStateLabeling().getLabelsOfState(group)) {
                    os << " " << label;
                }
                os << std::endl;
                typename storm::storage::SparseMatrix<ValueType>::index_type start = matrix.getRowGroupIndices()[group];
                typename storm::storage::SparseMatrix<ValueType>::index_type end = matrix.getRowGroupIndices()[group + 1];
                
                for (typename storm::storage::SparseMatrix<ValueType>::index_type i = start; i < end; ++i) {
                    // Print the actual row.
                    os << "\taction";
                    if(mdp->hasChoiceLabeling()) {
                        //TODO
                    }
                    os << std::endl;
                    for(auto it = matrix.begin(i); it != matrix.end(i); ++it) {
                        os << "\t\t" << it->getColumn() << " : " << it->getValue() << std::endl;
                    }
                    
                }
            }
            
        }
        
        
        
        template void explicitExportSparseModel<double>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<double>> sparseModel, std::vector<std::string> const& parameters);
        
        template void explicitExportSparseModel<storm::RationalNumber>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> sparseModel, std::vector<std::string> const& parameters);
        template void explicitExportSparseModel<storm::RationalFunction>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> sparseModel, std::vector<std::string> const& parameters);
    }
}