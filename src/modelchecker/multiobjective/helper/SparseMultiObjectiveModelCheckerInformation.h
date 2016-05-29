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
                    bool originalFormulaMinimizes;
                    boost::optional<double> threshold;
                    std::string rewardModelName;
                    boost::optional<uint_fast64_t> stepBound;
                    
                    void printInformationToStream(std::ostream& out) const {
                        out << std::setw(30) << originalFormula->toString();
                        out << " \t(";
                        out << (originalFormulaMinimizes ? "minimizes, \t" : "maximizes, \t");
                        out << "intern threshold:";
                        if(threshold){
                            out << std::setw(5) << (*threshold) << ",";
                        } else {
                            out << " none,";
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
                
                
                SparseMultiObjectiveModelCheckerInformation(SparseModelType const& model) : model(model) {
                    //Intentionally left empty
                }
                
                SparseMultiObjectiveModelCheckerInformation(SparseModelType && model) : model(model) {
                    //Intentionally left empty
                }
                
                SparseModelType model;
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                bool negatedRewardsConsidered;
                
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
                    out << "Preprocessed Model Information:" << std::endl;
                    model.printModelInformationToStream(out);
                    out << std::endl;
                    if(negatedRewardsConsidered){
                        out << "The rewards in the preprocessed model are negated" << std::endl;
                        out << std::endl;
                    }
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                }
                
                
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERINFORMATION_H_ */
