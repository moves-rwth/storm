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
            struct SparseMultiObjectivePreprocessorReturnType {
                
                enum class QueryType { Achievability, Quantitative, Pareto };
                
                storm::logic::MultiObjectiveFormula const& originalFormula;
                SparseModelType const& originalModel;
                std::shared_ptr<SparseModelType> preprocessedModel;
                QueryType queryType;
                
                // The (preprocessed) objectives
                std::vector<Objective<typename SparseModelType::ValueType>> objectives;
                
                // The states for which there is a scheduler such that all objectives have value zero
                storm::storage::BitVector possibleBottomStates;
                // Overapproximation of the set of choices that are part of an end component.
                storm::storage::BitVector possibleECChoices;
                
                SparseMultiObjectivePreprocessorReturnType(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel) : originalFormula(originalFormula), originalModel(originalModel) {
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
                    preprocessedModel->printModelInformationToStream(out);
                    out << std::endl;
                    out << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                }
           
                friend std::ostream& operator<<(std::ostream& out, SparseMultiObjectivePreprocessorReturnType<SparseModelType> const& ret) {
                    ret.printToStream(out);
                    return out;
                }
                
            };
        }
    }
}

