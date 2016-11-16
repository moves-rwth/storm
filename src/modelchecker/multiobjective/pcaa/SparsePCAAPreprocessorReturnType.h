#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPREPROCESSORRETURNTYPE_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPREPROCESSORRETURNTYPE_H_

#include <vector>
#include <memory>
#include <iomanip>
#include <type_traits>
#include <boost/optional.hpp>

#include "src/logic/Formulas.h"
#include "src/modelchecker/multiobjective/pcaa/PcaaObjective.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/storage/BitVector.h"
#include "src/utility/macros.h"
#include "src/utility/constants.h"

#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseModelType>
            struct SparsePcaaPreprocessorReturnType {
                
                enum class QueryType { Achievability, Quantitative, Pareto };
                
                storm::logic::MultiObjectiveFormula const& originalFormula;
                SparseModelType const& originalModel;
                SparseModelType preprocessedModel;
                QueryType queryType;
                
                // The (preprocessed) objectives
                std::vector<PcaaObjective<typename SparseModelType::ValueType>> objectives;
                
                // The index of the objective that is to be maximized (or minimized) in case of a quantitative Query
                boost::optional<uint_fast64_t> indexOfOptimizingObjective;

                // Maps any state of the preprocessed model to the corresponding state of the original Model
                std::vector<uint_fast64_t> newToOldStateIndexMapping;
                
                // The actions of the preprocessed model that have positive reward assigned for at least one objective (objectives with an upper time-bound are ignored!)
                storm::storage::BitVector actionsWithPositiveReward;
                // The actions of the preprocessed model that have negative reward assigned for at least one objective (objectives with an upper time-bound are ignored!)
                storm::storage::BitVector actionsWithNegativeReward;
                // The actions of the preprocessed model that are part of an EC
                storm::storage::BitVector ecActions;
                
                // The set of states of the preprocessed model for which there is a scheduler such that
                // the state is visited infinitely often and the induced reward is finite for all objectives
                storm::storage::BitVector possiblyRecurrentStates;
                
                SparsePcaaPreprocessorReturnType(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel, SparseModelType&& preprocessedModel) : originalFormula(originalFormula), originalModel(originalModel), preprocessedModel(preprocessedModel) {
                    // Intentionally left empty
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
                    preprocessedModel.printModelInformationToStream(out);
                    out << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                }
           
                friend std::ostream& operator<<(std::ostream& out, SparsePcaaPreprocessorReturnType<SparseModelType> const& ret) {
                    ret.printToStream(out);
                    return out;
                }
                
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPREPROCESSORRETURNTYPE_H_ */
