#include "DirectEncodingExporter.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"

#include "storm/models/sparse/StandardRewardModel.h"


namespace storm {
    namespace exporter {

        template<typename ValueType>
        void explicitExportSparseModel(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel, std::vector<std::string> const& parameters) {

            // Notice that for CTMCs we write the rate matrix instead of probabilities

            // Initialize
            storm::models::ModelType type = sparseModel->getType();
            std::vector<ValueType> exitRates; // Only for CTMCs and MAs.
            if(sparseModel->getType() == storm::models::ModelType::Ctmc) {
                exitRates = sparseModel->template as<storm::models::sparse::Ctmc<ValueType>>()->getExitRateVector();
            } else if(sparseModel->getType() == storm::models::ModelType::MarkovAutomaton) {
                type = storm::models::ModelType::Mdp;
                STORM_LOG_WARN("Markov automaton is exported as MDP (indication of Markovian choices is not supported in DRN format).");
                exitRates = sparseModel->template as<storm::models::sparse::MarkovAutomaton<ValueType>>()->getExitRates();
            }

            // Write header
            os << "// Exported by storm" << std::endl;
            os << "// Original model type: " << sparseModel->getType() << std::endl;
            os << "@type: " << type << std::endl;
            os << "@parameters" << std::endl;
            if (parameters.empty()) {
                for (std::string const& parameter : getParameters(sparseModel)) {
                    os << parameter << " ";
                }
            } else {
                for (std::string const& parameter : parameters) {
                    os << parameter << " ";
                }
            }
            os << std::endl;
            os << "@nr_states" << std::endl  << sparseModel->getNumberOfStates() <<  std::endl;
            os << "@model" << std::endl;

            storm::storage::SparseMatrix<ValueType> const& matrix = sparseModel->getTransitionMatrix();
            
            for (typename storm::storage::SparseMatrix<ValueType>::index_type group = 0; group < matrix.getRowGroupCount(); ++group) {
                os << "state " << group;

                // Write state rewards
                bool first = true;
                for (auto const& rewardModelEntry : sparseModel->getRewardModels()) {
                    if (first) {
                        os << " [";
                        first = false;
                    } else {
                        os << ", ";
                    }

                    if(rewardModelEntry.second.hasStateRewards()) {
                        os << rewardModelEntry.second.getStateRewardVector().at(group);
                    } else {
                        os << "0";
                    }
                }

                if (!first) {
                    os << "]";
                }

                // Write labels
                for(auto const& label : sparseModel->getStateLabeling().getLabelsOfState(group)) {
                    os << " " << label;
                }
                os << std::endl;

                // Write probabilities
                typename storm::storage::SparseMatrix<ValueType>::index_type start = matrix.hasTrivialRowGrouping() ? group : matrix.getRowGroupIndices()[group];
                typename storm::storage::SparseMatrix<ValueType>::index_type end = matrix.hasTrivialRowGrouping() ? group + 1 : matrix.getRowGroupIndices()[group + 1];

                // Iterate over all actions
                for (typename storm::storage::SparseMatrix<ValueType>::index_type row = start; row < end; ++row) {
                    // Print the actual row.
                    if (sparseModel->hasChoiceLabeling()) {
                        os << "\taction ";
                        bool lfirst = true;
                        for (auto const& label : sparseModel->getChoiceLabeling().getLabelsOfChoice(row)) {
                            if (!lfirst) {
                                os << "_";
                            }
                            os << label;
                            lfirst = false;
                        }
                    } else {
                        os << "\taction " << row - start;
                    }
                    bool first = true;
                    // Write transition rewards
                    for (auto const& rewardModelEntry : sparseModel->getRewardModels()) {
                        if (first) {
                            os << " [";
                            first = false;
                        } else {
                            os << ", ";
                        }

                        if(rewardModelEntry.second.hasStateActionRewards()) {
                            os << storm::utility::to_string(rewardModelEntry.second.getStateActionRewardVector().at(row));
                        } else {
                            os << "0";
                        }

                    }
                    if (!first) {
                        os << "]";
                    }

                    os << std::endl;
                    
                    // Write probabilities
                    for(auto it = matrix.begin(row); it != matrix.end(row); ++it) {
                        ValueType prob = it->getValue();
                        os << "\t\t" << it->getColumn() << " : ";
                        os << storm::utility::to_string(prob) << std::endl;
                    }
                    
                }
            } // end matrix iteration
            
        }

        template<typename ValueType>
        std::vector<std::string> getParameters(std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel) {
            return {};
        }

        template void explicitExportSparseModel<double>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<double>> sparseModel, std::vector<std::string> const& parameters);

#ifdef STORM_HAVE_CARL
        template<>
        std::vector<std::string> getParameters(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> sparseModel) {
            std::vector<std::string> parameters;
            std::set<storm::RationalFunctionVariable> parametersProb = storm::models::sparse::getProbabilityParameters(*sparseModel);
            for (auto const& parameter : parametersProb) {
                std::stringstream stream;
                stream << parameter;
                parameters.push_back(stream.str());
            }
            std::set<storm::RationalFunctionVariable> parametersReward = storm::models::sparse::getRewardParameters(*sparseModel);
            for (auto const& parameter : parametersReward) {
                std::stringstream stream;
                stream << parameter;
                parameters.push_back(stream.str());
            }
            return parameters;
        }

        template void explicitExportSparseModel<storm::RationalNumber>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> sparseModel, std::vector<std::string> const& parameters);
        template void explicitExportSparseModel<storm::RationalFunction>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> sparseModel, std::vector<std::string> const& parameters);
#endif
    }
}
