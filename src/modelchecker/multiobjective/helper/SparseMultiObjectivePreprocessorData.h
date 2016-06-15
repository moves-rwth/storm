#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORDATA_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORDATA_H_

#include <vector>
#include <memory>
#include <iomanip>
#include <boost/optional.hpp>

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveObjectiveInformation.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseModelType>
            struct SparseMultiObjectivePreprocessorData {
                
                enum class QueryType { Achievability, Numerical, Pareto };
                
                SparseModelType const& originalModel;
                SparseModelType preprocessedModel;
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                
                QueryType queryType;
                std::vector<SparseMultiObjectiveObjectiveInformation<typename SparseModelType::ValueType>> objectives;
                boost::optional<uint_fast64_t> indexOfOptimizingObjective;
                
                bool produceSchedulers;
                
                SparseMultiObjectivePreprocessorData(SparseModelType const& originalModel, SparseModelType&& preprocessedModel, std::vector<uint_fast64_t>&& newToOldStateIndexMapping) : originalModel(originalModel), preprocessedModel(preprocessedModel), newToOldStateIndexMapping(newToOldStateIndexMapping), produceSchedulers(false) {
                    //Intentionally left empty
                }
                
                void printToStream(std::ostream& out) {
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << "                                                       Multi-objective Preprocessor Data                                               " << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << std::endl;
                    out << "Objectives:" << std::endl;
                    out << "--------------------------------------------------------------" << std::endl;
                    for (auto const& obj : objectives) {
                        obj.printToStream(out);
                    }
                    out << "--------------------------------------------------------------" << std::endl;
                    out << std::endl;
                    out << "Original Model Information:" << std::endl;
                    originalModel.printModelInformationToStream(out);
                    out << std::endl;
                    out << "Preprocessed Model Information:" << std::endl;
                    preprocessedModel.printModelInformationToStream(out);
                    out << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                }
           
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORDATA_H_ */
