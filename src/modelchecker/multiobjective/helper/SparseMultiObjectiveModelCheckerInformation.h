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
                
                void setModel(SparseModelType&& newModel){
                    model = newModel;
                }
                
                void setModel(SparseModelType const& newModel){
                    model = newModel;
                }
                
                SparseModelType const& getModel() const {
                    return model;
                }
                
                void setNewToOldStateIndexMapping(std::vector<uint_fast64_t> const& newMapping){
                    newToOldStateIndexMapping = newMapping;
                }
                
                void setNewToOldStateIndexMapping(std::vector<uint_fast64_t>&& newMapping){
                    newToOldStateIndexMapping = newMapping;
                }
                
                std::vector<uint_fast64_t>const& getNewToOldStateIndexMapping() const{
                    return newToOldStateIndexMapping;
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
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                bool negativeRewardsConsidered;
                
                std::vector<ObjectiveInformation> objectives;
                
                
                
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERINFORMATION_H_ */
