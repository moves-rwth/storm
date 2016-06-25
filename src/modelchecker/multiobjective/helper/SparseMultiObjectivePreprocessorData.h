#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORDATA_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORDATA_H_

#include <vector>
#include <memory>
#include <iomanip>
#include <boost/optional.hpp>

#include "src/logic/Formulas.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveObjectiveInformation.h"
#include "src/storage/BitVector.h"
#include "src/utility/macros.h"

#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseModelType>
            struct SparseMultiObjectivePreprocessorData {
                
                enum class QueryType { Achievability, Numerical, Pareto };
                enum class PreprocessorObjectiveSolution { None, False, True, Zero, Unbounded, Undefined };
                
                storm::logic::MultiObjectiveFormula const& originalFormula;
                std::vector<PreprocessorObjectiveSolution> solutionsFromPreprocessing;
                
                SparseModelType const& originalModel;
                SparseModelType preprocessedModel;
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                std::string prob1StatesLabel;
                
                QueryType queryType;
                std::vector<SparseMultiObjectiveObjectiveInformation<typename SparseModelType::ValueType>> objectives;
                boost::optional<uint_fast64_t> indexOfOptimizingObjective;
                
                bool produceSchedulers;
                
                SparseMultiObjectivePreprocessorData(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel, SparseModelType&& preprocessedModel, std::vector<uint_fast64_t>&& newToOldStateIndexMapping) : originalFormula(originalFormula), solutionsFromPreprocessing(originalFormula.getNumberOfSubformulas(), PreprocessorObjectiveSolution::None), originalModel(originalModel), preprocessedModel(preprocessedModel), newToOldStateIndexMapping(newToOldStateIndexMapping), produceSchedulers(false) {
                    
                    // get a unique name for the labels of states that have to be reached with probability 1 and add the label
                    this->prob1StatesLabel = "prob1";
                    while(this->preprocessedModel.hasLabel(this->prob1StatesLabel)) {
                        this->prob1StatesLabel = "_" + this->prob1StatesLabel;
                    }
                    this->preprocessedModel.getStateLabeling().addLabel(this->prob1StatesLabel, storm::storage::BitVector(this->preprocessedModel.getNumberOfStates(), true));
                }
                
                void printToStream(std::ostream& out) const {
                    out << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << "                                                       Multi-objective Preprocessor Data                                               " << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << std::endl;
                    out << "Original Formula: " << std::endl;
                    out << "--------------------------------------------------------------" << std::endl;
                    out << "\t" << originalFormula << std::endl;
                    bool hasOneObjectiveSolvedInPreprocessing = false;
                    for(uint_fast64_t subformulaIndex = 0; subformulaIndex < originalFormula.getNumberOfSubformulas(); ++subformulaIndex) {
                        if(!hasOneObjectiveSolvedInPreprocessing && solutionsFromPreprocessing[subformulaIndex]!= PreprocessorObjectiveSolution::None) {
                            hasOneObjectiveSolvedInPreprocessing = true;
                            out << std::endl;
                            out << "Solutions of objectives obtained from Preprocessing: " << std::endl;
                            out << "--------------------------------------------------------------" << std::endl;
                        }
                        switch(solutionsFromPreprocessing[subformulaIndex]) {
                            case PreprocessorObjectiveSolution::None:
                                break;
                            case PreprocessorObjectiveSolution::False:
                                out<< "\t" << subformulaIndex << ": " << originalFormula.getSubformula(subformulaIndex) << " \t= false" << std::endl;
                                break;
                            case PreprocessorObjectiveSolution::True:
                                out<< "\t" << subformulaIndex << ": " << originalFormula.getSubformula(subformulaIndex) << " \t= true" << std::endl;
                                break;
                            case PreprocessorObjectiveSolution::Zero:
                                out<< "\t" << subformulaIndex << ": " << originalFormula.getSubformula(subformulaIndex) << " \t= zero" << std::endl;
                                break;
                            case PreprocessorObjectiveSolution::Unbounded:
                                out<< "\t" << subformulaIndex << ": " << originalFormula.getSubformula(subformulaIndex) << " \t= unbounded" << std::endl;
                                break;
                            case PreprocessorObjectiveSolution::Undefined:
                                out<< "\t" << subformulaIndex << ": " << originalFormula.getSubformula(subformulaIndex) << " \t= undefined" << std::endl;
                                break;
                            default:
                                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "no case for PreprocessorObjectiveSolution.");
                        }
                    }
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
           
                
                friend std::ostream& operator<<(std::ostream& out, SparseMultiObjectivePreprocessorData<SparseModelType> const& data) {
                    data.printToStream(out);
                    return out;
                }
                
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORDATA_H_ */
