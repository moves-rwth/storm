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
            std::vector<ValueType> exitRates; // Only for CTMCs and MAs.
            if (sparseModel->getType() == storm::models::ModelType::Ctmc) {
                exitRates = sparseModel->template as<storm::models::sparse::Ctmc<ValueType>>()->getExitRateVector();
            } else if (sparseModel->getType() == storm::models::ModelType::MarkovAutomaton) {
                exitRates = sparseModel->template as<storm::models::sparse::MarkovAutomaton<ValueType>>()->getExitRates();
            }

            // Write header
            os << "// Exported by storm" << std::endl;
            os << "// Original model type: " << sparseModel->getType() << std::endl;
            os << "@type: " << sparseModel->getType() << std::endl;
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
            os << "@reward_models" << std::endl;
            for (auto const& rewardModel : sparseModel->getRewardModels()) {
                os << rewardModel.first << " ";
            }
            os << std::endl;
            os << "@nr_states" << std::endl  << sparseModel->getNumberOfStates() <<  std::endl;
            os << "@model" << std::endl;

            storm::storage::SparseMatrix<ValueType> const& matrix = sparseModel->getTransitionMatrix();

            // Iterate over states and export state information and outgoing transitions
            for (typename storm::storage::SparseMatrix<ValueType>::index_type group = 0; group < matrix.getRowGroupCount(); ++group) {
                os << "state " << group;

                // Write exit rates for CTMCs and MAs
                if (!exitRates.empty()) {
                    os << " !" << exitRates.at(group);
                }

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
                    // Write choice
                    if (sparseModel->hasChoiceLabeling()) {
                        os << "\taction ";
                        bool lfirst = true;
                        for (auto const& label : sparseModel->getChoiceLabeling().getLabelsOfChoice(row)) {
                            if (!lfirst) {
                                os << "_";
                                lfirst = false;
                            }
                            os << label;
                        }
                    } else {
                        os << "\taction " << row - start;
                    }

                    // Write action rewards
                    bool first = true;
                    for (auto const& rewardModelEntry : sparseModel->getRewardModels()) {
                        if (first) {
                            os << " [";
                            first = false;
                        } else {
                            os << ", ";
                        }

                        if (rewardModelEntry.second.hasStateActionRewards()) {
                            os << storm::utility::to_string(rewardModelEntry.second.getStateActionRewardVector().at(row));
                        } else {
                            os << "0";
                        }

                    }
                    if (!first) {
                        os << "]";
                    }
                    os << std::endl;

                    // Write transitions
                    for (auto it = matrix.begin(row); it != matrix.end(row); ++it) {
                        ValueType prob = it->getValue();
                        os << "\t\t" << it->getColumn() << " : ";
                        os << storm::utility::to_string(prob) << std::endl;
                    }

                }
            } // end state iteration

        }

        template<typename ValueType>
        std::vector<std::string> getParameters(std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel) {
            return {};
        }

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

        // Template instantiations
        template void explicitExportSparseModel<double>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<double>> sparseModel, std::vector<std::string> const& parameters);
        template void explicitExportSparseModel<storm::RationalNumber>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> sparseModel, std::vector<std::string> const& parameters);
        template void explicitExportSparseModel<storm::RationalFunction>(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> sparseModel, std::vector<std::string> const& parameters);
    }
}
