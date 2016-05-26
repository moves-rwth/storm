#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERINFORMATION_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERINFORMATION_H_

#include <vector>
#include <memory>

#include "src/logic/Formulas.h"

namespace storm {
    namespace modelchecker {
        

        namespace helper {
            
            template <class SparseModelType>
            class SparseMultiObjectiveModelCheckerInformation {
            public :
                
                typedef typename SparseModelType::ValueType ValueType;
                typedef typename SparseModelType::RewardModelType RewardModelType;
                
                struct ObjectiveInformation {
                    std::shared_ptr<storm::logic::Formula const> originalFormula;
                    bool originalFormulaMinimizes;
                    boost::optional<double> threshold;
                    std::string rewardModelName;
                    boost::optional<uint_fast64_t> stepBound;
                };
                
                
                SparseMultiObjectiveModelCheckerInformation(SparseModelType const& model) : model(model) {
                    //Intentionally left empty
                }
                
                SparseMultiObjectiveModelCheckerInformation(SparseModelType && model) : model(model) {
                    //Intentionally left empty
                }
                
                SparseModelType& getModel(){
                    return model;
                }
                SparseModelType const& getModel() const {
                    return model;
                }
                
                std::vector<uint_fast64_t>& getNewToOldStateMapping(){
                    return newToOldStateMapping;
                }
                std::vector<uint_fast64_t>const& getNewToOldStateMapping() const{
                    return newToOldStateMapping;
                }
                
                bool areNegativeRewardsConsidered() {
                    return negativeRewardsConsidered;
                }
                
                void setNegativeRewardsConsidered(bool value){
                    negativeRewardsConsidered = value;
                }
                
                std::vector<ObjectiveInformation>& getObjectives() {
                    return objectives;
                }
                std::vector<ObjectiveInformation>const& getObjectives() const {
                    return objectives;
                }
                
                
            private:
                SparseModelType model;
                std::vector<uint_fast64_t> newToOldStateMapping;
                bool negativeRewardsConsidered;
                
                std::vector<ObjectiveInformation> objectives;
                
                
                
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERINFORMATION_H_ */
