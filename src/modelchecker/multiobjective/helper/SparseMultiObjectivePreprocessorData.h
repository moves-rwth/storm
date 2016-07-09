#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORDATA_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORDATA_H_

#include <vector>
#include <memory>
#include <iomanip>
#include <type_traits>
#include <boost/optional.hpp>

#include "src/logic/Formulas.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveObjectiveInformation.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/storage/BitVector.h"
#include "src/utility/macros.h"
#include "src/utility/constants.h"

#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseModelType>
            struct SparseMultiObjectivePreprocessorData {
                
                enum class QueryType { Achievability, Numerical, Pareto };
                enum class PreprocessorObjectiveSolution { None, False, True, Numerical, Unbounded, Undefined };
                
                storm::logic::MultiObjectiveFormula const& originalFormula;
                
                // Stores the result for this objective obtained from preprocessing.
                // In case of a numerical result, the value is store in the second entry of the pair. Otherwise, the second entry can be ignored.
                std::vector<std::pair<PreprocessorObjectiveSolution, typename SparseModelType::ValueType>> solutionsFromPreprocessing;
                
                SparseModelType const& originalModel;
                SparseModelType preprocessedModel;
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                std::string prob1StatesLabel;
                
                QueryType queryType;
                std::vector<SparseMultiObjectiveObjectiveInformation<typename SparseModelType::ValueType>> objectives;
                boost::optional<uint_fast64_t> indexOfOptimizingObjective;
                
                bool produceSchedulers;
                
                SparseMultiObjectivePreprocessorData(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel, SparseModelType&& preprocessedModel, std::vector<uint_fast64_t>&& newToOldStateIndexMapping) : originalFormula(originalFormula), solutionsFromPreprocessing(originalFormula.getNumberOfSubformulas(), std::make_pair(PreprocessorObjectiveSolution::None, storm::utility::zero<typename SparseModelType::ValueType>())), originalModel(originalModel), preprocessedModel(preprocessedModel), newToOldStateIndexMapping(newToOldStateIndexMapping), produceSchedulers(false) {
                    
                    // get a unique name for the labels of states that have to be reached with probability 1 and add the label
                    this->prob1StatesLabel = "prob1";
                    while(this->preprocessedModel.hasLabel(this->prob1StatesLabel)) {
                        this->prob1StatesLabel = "_" + this->prob1StatesLabel;
                    }
                    this->preprocessedModel.getStateLabeling().addLabel(this->prob1StatesLabel, storm::storage::BitVector(this->preprocessedModel.getNumberOfStates(), true));
                }
                
                template<typename MT = SparseModelType>
                typename std::enable_if<std::is_same<MT, storm::models::sparse::MarkovAutomaton<typename SparseModelType::ValueType>>::value, storm::storage::BitVector const&>::type
                getMarkovianStatesOfPreprocessedModel() const {
                    return preprocessedModel.getMarkovianStates();
                }
                
                template<typename MT = SparseModelType>
                typename std::enable_if<!std::is_same<MT, storm::models::sparse::MarkovAutomaton<typename SparseModelType::ValueType>>::value, storm::storage::BitVector const&>::type
                getMarkovianStatesOfPreprocessedModel() const {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Tried to retrieve Markovian states but the considered model is not an MA.");
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
                        if(!hasOneObjectiveSolvedInPreprocessing && solutionsFromPreprocessing[subformulaIndex].first != PreprocessorObjectiveSolution::None) {
                            hasOneObjectiveSolvedInPreprocessing = true;
                            out << std::endl;
                            out << "Solutions of objectives obtained from Preprocessing: " << std::endl;
                            out << "--------------------------------------------------------------" << std::endl;
                        }
                        switch(solutionsFromPreprocessing[subformulaIndex].first) {
                            case PreprocessorObjectiveSolution::None:
                                break;
                            case PreprocessorObjectiveSolution::False:
                                out<< "\t" << subformulaIndex << ": " << originalFormula.getSubformula(subformulaIndex) << " \t= false" << std::endl;
                                break;
                            case PreprocessorObjectiveSolution::True:
                                out<< "\t" << subformulaIndex << ": " << originalFormula.getSubformula(subformulaIndex) << " \t= true" << std::endl;
                                break;
                            case PreprocessorObjectiveSolution::Numerical:
                                out<< "\t" << subformulaIndex << ": " << originalFormula.getSubformula(subformulaIndex) << " \t= " << solutionsFromPreprocessing[subformulaIndex].second << std::endl;
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
