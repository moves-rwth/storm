/* 
 * File:   SamplingModel.h
 * Author: tim
 *
 * Created on August 7, 2015, 9:31 AM
 */

#ifndef STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H
#define	STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H

#include <unordered_map>
#include <memory>

#include "src/utility/region.h"

#include "src/logic/Formulas.h"
#include "src/models/sparse/Model.h"
#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace modelchecker{
        namespace region {
            template<typename ParametricSparseModelType, typename ConstantType>
            class SamplingModel {
            public:
                typedef typename ParametricSparseModelType::ValueType ParametricType;
                typedef typename storm::utility::region::VariableType<ParametricType> VariableType;
                typedef typename storm::utility::region::CoefficientType<ParametricType> CoefficientType;
                
                /*!
                 * Creates a sampling model.
                 */
                SamplingModel(ParametricSparseModelType const& parametricModel, std::shared_ptr<storm::logic::OperatorFormula> formula);
                virtual ~SamplingModel();

                /*!
                 * returns the underlying model
                 */
                std::shared_ptr<storm::models::sparse::Model<ConstantType>> const& getModel() const;

                /*!
                 * Instantiates the underlying model according to the given point
                 */
                void instantiate(std::map<VariableType, CoefficientType>const& point);

                /*!
                 * Returns the reachability probabilities (or the expected rewards) for every state according to the current instantiation.
                 * Undefined behavior if model has not been instantiated first!
                 */
                std::vector<ConstantType> const& computeValues();


            private:

                typedef typename std::unordered_map<ParametricType, ConstantType>::value_type TableEntry;

                void initializeProbabilities(ParametricSparseModelType const& parametricModel, storm::storage::SparseMatrix<ConstantType>& probabilityMatrix, std::vector<TableEntry*>& matrixEntryToEvalTableMapping, TableEntry* constantEntry);
                void initializeRewards(ParametricSparseModelType const& parametricModel, boost::optional<std::vector<ConstantType>>& stateRewards, std::vector<TableEntry*>& rewardEntryToEvalTableMapping, TableEntry* constantEntry);

                //The model with which we work
                std::shared_ptr<storm::models::sparse::Model<ConstantType>> model;
                //The formula for which we will compute the values
                std::shared_ptr<storm::logic::OperatorFormula> formula;
                //A flag that denotes whether we compute probabilities or rewards
                bool computeRewards;

                // We store one (unique) entry for every occurring function.
                // Whenever a sampling point is given, we can then evaluate the functions
                // and store the result to the target value of this map
                std::unordered_map<ParametricType, ConstantType> probabilityEvaluationTable;
                std::unordered_map<ParametricType, ConstantType> rewardEvaluationTable;

                //This Vector connects the probability evaluation table with the probability matrix of the model.
                //Vector has one entry for every (non-constant) matrix entry.
                //pair.first points to an entry in the evaluation table,
                //pair.second is an iterator to the corresponding matrix entry
                std::vector<std::pair<ConstantType*, typename storm::storage::SparseMatrix<ConstantType>::iterator>> probabilityMapping;
                std::vector<std::pair<ConstantType*, typename std::vector<ConstantType>::iterator>> stateRewardMapping;

            };
        } //namespace region
    }
}
#endif	/* STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H */

