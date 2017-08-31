#pragma once

#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "storm/logic/Formulas.h"
#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/storage/BitVector.h"

#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseModelType>
            struct SparseMultiObjectivePreprocessorResult {
                
                enum class QueryType { Achievability, Quantitative, Pareto };
                enum class RewardFinitenessType {
                    AllFinite, // The expected reward is finite for all objectives and all schedulers
                    ExistsParetoFinite, // There is a Pareto optimal scheduler yielding finite rewards for all objectives
                    Infinite // All Pareto optimal schedulers yield infinite reward for at least one objective
                };
                
                // Original data
                storm::logic::MultiObjectiveFormula const& originalFormula;
                SparseModelType const& originalModel;
                
                // The preprocessed model and objectives
                std::shared_ptr<SparseModelType> preprocessedModel;
                std::vector<Objective<typename SparseModelType::ValueType>> objectives;
                
                // Data about the query
                QueryType queryType;
                RewardFinitenessType rewardFinitenessType;
                
                // The states of the preprocessed model for which...
                storm::storage::BitVector reward0EStates; // ... there is a scheduler such that all expected reward objectives have value zero
                storm::storage::BitVector reward0AStates; // ... all schedulers induce value 0 for all expected reward objectives
                boost::optional<storm::storage::BitVector> rewardLessInfinityEStates; // ... there is a scheduler yielding finite reward for all expected reward objectives
                // Note that other types of objectives (e.g., reward bounded reachability objectives) are not considered.
                
                // Encodes a superset of the set of choices of preprocessedModel that are part of an end component (if given).
                //boost::optional<storm::storage::BitVector> ecChoicesHint;
                
                SparseMultiObjectivePreprocessorResult(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel) : originalFormula(originalFormula), originalModel(originalModel) {
                    // Intentionally left empty
                }
                
                bool containsOnlyRewardObjectives() {
                    for (auto const& obj : objectives) {
                        if (!(obj.formula->isRewardOperatorFormula() && (obj.formula->getSubformula().isTotalRewardFormula() || obj.formula->getSubformula().isCumulativeRewardFormula()))) {
                            return false;
                        }
                    }
                    return true;
                }
                
                void printToStream(std::ostream& out) const {
                    out << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << "                                                       Multi-objective Query                                              " << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                    out << std::endl;
                    out << "Original Formula: " << std::endl;
                    out << "--------------------------------------------------------------" << std::endl;
                    out << "\t" << originalFormula << std::endl;
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
                    preprocessedModel->printModelInformationToStream(out);
                    out << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                }
           
                friend std::ostream& operator<<(std::ostream& out, SparseMultiObjectivePreprocessorResult<SparseModelType> const& ret) {
                    ret.printToStream(out);
                    return out;
                }
                
            };
        }
    }
}

