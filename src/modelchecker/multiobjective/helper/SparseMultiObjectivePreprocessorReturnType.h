#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORRETURNTYPE_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORRETURNTYPE_H_

#include <vector>
#include <memory>
#include <iomanip>
#include <boost/optional.hpp>

#include "src/logic/Formulas.h"

namespace storm {
    namespace modelchecker {
        

        namespace helper {
            
            template <class SparseModelType>
            struct SparseMultiObjectivePreprocessorReturnType {
                
                typedef typename SparseModelType::ValueType ValueType;
                typedef typename SparseModelType::RewardModelType RewardModelType;
                
                struct ObjectiveInformation {
                    // the original input formula
                    std::shared_ptr<storm::logic::Formula const> originalFormula;
                    
                    // the name of the considered reward model in the preprocessedModel
                    std::string rewardModelName;
                    
                    // true if all rewards for this objective are positive, false if all rewards are negative.
                    bool rewardsArePositive;
                    
                    // transformation from the values of the preprocessed model to the ones for the actual input model, i.e.,
                    // x is achievable in the preprocessed model iff factor*x + offset is achievable in the original model
                    ValueType toOriginalValueTransformationFactor;
                    ValueType toOriginalValueTransformationOffset;
                    
                    // The probability/reward threshold for the preprocessed model (if originalFormula specifies one).
                    // This is always a lower bound.
                    boost::optional<ValueType> threshold;
                    // True iff the specified threshold is strict, i.e., >
                    bool thresholdIsStrict = false;
                    
                    // The (discrete) stepBound for the formula (if given by the originalFormula)
                    boost::optional<uint_fast64_t> stepBound;
                    
                    void printToStream(std::ostream& out) const {
                        out << std::setw(30) << originalFormula->toString();
                        out << " \t(toOrigVal:" << std::setw(3) << toOriginalValueTransformationFactor << "*x +" << std::setw(3) << toOriginalValueTransformationOffset << ", \t";
                        out << "intern threshold:";
                        if(threshold){
                            out << (thresholdIsStrict ? " >" : ">=");
                            out << std::setw(5) << (*threshold) << ",";
                        } else {
                            out << "   none,";
                        }
                        out << " \t";
                        out << "intern reward model: " << std::setw(10) << rewardModelName;
                        out << (rewardsArePositive ? " (positive)" : " (negative)") << ", \t";
                        out << "step bound:";
                        if(stepBound) {
                            out << std::setw(5) << (*stepBound);
                        } else {
                            out << " none";
                        }
                        out << ")" << std::endl;
                     }
                };
                
                SparseMultiObjectivePreprocessorReturnType(SparseModelType const& model) : preprocessedModel(model),  originalModel(model) , produceSchedulers(false) {
                    //Intentionally left empty
                }
                
                void printToStream(std::ostream& out) {
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << "                                                      Multi-objective Preprocessor Result                                              " << std::endl;
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
                
                SparseModelType preprocessedModel;
                SparseModelType const& originalModel;
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                std::vector<ObjectiveInformation> objectives;
                bool produceSchedulers;
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSORRETURNTYPE_H_ */
