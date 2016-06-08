#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERINFORMATION_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERINFORMATION_H_

#include <vector>
#include <memory>
#include <iomanip>

#include "src/logic/Formulas.h"

namespace storm {
    namespace modelchecker {
        

        namespace helper {
            
            template <class SparseModelType>
            struct SparseMultiObjectiveModelCheckerInformation {
                
                typedef typename SparseModelType::ValueType ValueType;
                typedef typename SparseModelType::RewardModelType RewardModelType;
                
                struct ObjectiveInformation {
                    std::shared_ptr<storm::logic::Formula const> originalFormula;
                    std::string rewardModelName;
                    bool isNegative = false;
                    bool isInverted = false;
                    boost::optional<double> threshold;
                    bool thresholdIsStrict = false;
                    boost::optional<uint_fast64_t> stepBound;
                    
                    void printInformationToStream(std::ostream& out) const {
                        out << std::setw(30) << originalFormula->toString();
                        out << " \t(";
                        out << (isNegative ? "-x, " : "    ");
                        out << (isInverted ? "1-x, " : "     ");
                        out << "intern threshold:";
                        if(threshold){
                            out << (thresholdIsStrict ? " >" : ">=");
                            out << std::setw(5) << (*threshold) << ",";
                        } else {
                            out << "   none,";
                        }
                        out << " \t";
                        out << "intern reward model: " << std::setw(10) << rewardModelName << ", \t";
                        out << "step bound:";
                        if(stepBound) {
                            out << std::setw(5) << (*stepBound);
                        } else {
                            out << " none";
                        }
                        out << ")" << std::endl;
                     }
                };
                
                
                SparseMultiObjectiveModelCheckerInformation(SparseModelType const& model) : preprocessedModel(model),  originalModel(model) {
                    //Intentionally left empty
                }

                SparseModelType preprocessedModel;
                SparseModelType const& originalModel;
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                
                std::vector<ObjectiveInformation> objectives;
                
                void printInformationToStream(std::ostream& out) {
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << "                                                   Multi-objective Model Checker Information                                           " << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << std::endl;
                    out << "Objectives:" << std::endl;
                    out << "--------------------------------------------------------------" << std::endl;
                    for (auto const& obj : objectives) {
                        obj.printInformationToStream(out);
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

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERINFORMATION_H_ */
