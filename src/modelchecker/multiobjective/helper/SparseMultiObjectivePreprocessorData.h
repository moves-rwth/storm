#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORDATA_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORDATA_H_

#include <vector>
#include <memory>
#include <iomanip>
#include <boost/optional.hpp>

#include "src/logic/Formulas.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveObjectiveInformation.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseModelType>
            struct SparseMultiObjectivePreprocessorData {
                
                enum class QueryType { Achievability, Numerical, Pareto };
                
                storm::logic::MultiObjectiveFormula const& originalFormula;
                storm::storage::BitVector objectivesSolvedInPreprocessing;
                
                SparseModelType const& originalModel;
                SparseModelType preprocessedModel;
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                std::string prob1StatesLabel;
                
                QueryType queryType;
                std::vector<SparseMultiObjectiveObjectiveInformation<typename SparseModelType::ValueType>> objectives;
                boost::optional<uint_fast64_t> indexOfOptimizingObjective;
                
                bool produceSchedulers;
                
                SparseMultiObjectivePreprocessorData(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel, SparseModelType&& preprocessedModel, std::vector<uint_fast64_t>&& newToOldStateIndexMapping) : originalFormula(originalFormula), originalModel(originalModel), preprocessedModel(preprocessedModel), newToOldStateIndexMapping(newToOldStateIndexMapping), produceSchedulers(false) {
                    //Intentionally left empty
                }
                
                void printToStream(std::ostream& out) {
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << "                                                       Multi-objective Preprocessor Data                                               " << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << std::endl;
                    out << "Original Formula: " << std::endl;
                    out << "--------------------------------------------------------------" << std::endl;
                    out << "\t" << originalFormula << std::endl;
                    out << std::endl;
                    if(!objectivesSolvedInPreprocessing.empty()) {
                        out << "Objectives solved while preprocessing: " << std::endl;
                        out << "--------------------------------------------------------------" << std::endl;
                        for(auto const& subFIndex : objectivesSolvedInPreprocessing) {
                            out<< "\t" << subFIndex << ": \t" << originalFormula.getSubformula(subFIndex) << std::endl;
                        }
                        out << std::endl;
                    }
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
