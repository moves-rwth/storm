/* 
 * File:   ApproximationModel.h
 * Author: tim
 *
 * Created on August 7, 2015, 9:29 AM
 */

#ifndef STORM_MODELCHECKER_REGION_APPROXIMATIONMODEL_H
#define	STORM_MODELCHECKER_REGION_APPROXIMATIONMODEL_H

#include <unordered_map>
#include <memory>
#include "src/utility/region.h"

#include "src/logic/Formulas.h"
#include "src/modelchecker/region/ParameterRegion.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace modelchecker {
        namespace region {
            template<typename ParametricSparseModelType, typename ConstantType>
            class ApproximationModel{
            public:
                typedef typename ParametricSparseModelType::ValueType ParametricType;
                typedef typename storm::utility::region::VariableType<ParametricType> VariableType;
                typedef typename storm::utility::region::CoefficientType<ParametricType> CoefficientType;

                /*!
                 * @note this will not check whether approximation is applicable
                 */
                ApproximationModel(ParametricSparseModelType const& parametricModel, std::shared_ptr<storm::logic::OperatorFormula> formula);
                virtual ~ApproximationModel();

                /*!
                 * returns the underlying model
                 */
                std::shared_ptr<storm::models::sparse::Mdp<ConstantType>> const& getModel() const;

                /*!
                 * Instantiates the underlying model according to the given region
                 */
                void instantiate(ParameterRegion<ParametricType> const& region);

                /*!
                 * Returns the approximated reachability probabilities for every state.
                 * Undefined behavior if model has not been instantiated first!
                 * @param optimalityType Use MAXIMIZE to get upper bounds or MINIMIZE to get lower bounds
                 */
                std::vector<ConstantType> const& computeValues(storm::solver::OptimizationDirection const& optDir);


            private:
                //This enum helps to store how a parameter will be substituted.
                enum class TypeOfBound { 
                    LOWER,
                    UPPER,
                    CHOSEOPTIMAL
                }; 

                //A class that represents a function and how it should be substituted (i.e. which variables should be replaced with lower and which with upper bounds of the region)
                //The substitution is given as an index in probability or reward substitutions. (This allows to instantiate the substitutions more easily)
                class FunctionSubstitution {
                public:
                    FunctionSubstitution(ParametricType const& fun, std::size_t const& sub) : hash(computeHash(fun,sub)), function(fun), substitution(sub) {
                        //intentionally left empty
                    }

                    FunctionSubstitution(ParametricType&& fun, std::size_t&& sub) : hash(computeHash(fun,sub)), function(std::move(fun)), substitution(std::move(sub)) {
                        //intentionally left empty
                    }

                    FunctionSubstitution(FunctionSubstitution const& other) : hash(other.hash), function(other.function), substitution(other.substitution){
                        //intentionally left empty
                    }

                    FunctionSubstitution(FunctionSubstitution&& other) : hash(std::move(other.hash)), function(std::move(other.function)), substitution(std::move(other.substitution)){
                        //intentionally left empty
                    }

                    FunctionSubstitution() = default;

                    ~FunctionSubstitution() = default;

                    bool operator==(FunctionSubstitution const& other) const {
                        return this->hash==other.hash && this->substitution==other.substitution && this->function==other.function;
                    }

                    ParametricType const& getFunction() const{
                        return this->function;
                    }

                    std::size_t const& getSubstitution() const{
                        return this->substitution;
                    }

                    std::size_t const& getHash() const{
                        return this->hash;
                    }

                private:

                    static std::size_t computeHash(ParametricType const& fun, std::size_t const& sub) {
                            std::size_t hf = std::hash<ParametricType>()(fun);
                            std::size_t hs = std::hash<std::size_t>()(sub);
                            return hf ^(hs << 1);
                    }

                    std::size_t hash;
                    ParametricType function;
                    std::size_t substitution;
                };

                class FuncSubHash{
                    public:
                        std::size_t operator()(FunctionSubstitution const& fs) const {
                            return fs.getHash();
                        }
                };

                typedef typename std::unordered_map<FunctionSubstitution, ConstantType, FuncSubHash>::value_type ProbTableEntry;
                typedef typename std::unordered_map<FunctionSubstitution, std::pair<ConstantType, ConstantType>, FuncSubHash>::value_type RewTableEntry;

                void initializeProbabilities(ParametricSparseModelType const& parametricModel, storm::storage::SparseMatrix<ConstantType>& probabilityMatrix, std::vector<std::size_t>& rowSubstitutions, std::vector<ProbTableEntry*>& matrixEntryToEvalTableMapping,  ProbTableEntry* constantEntry);
                void initializeRewards(ParametricSparseModelType const& parametricModel, storm::storage::SparseMatrix<ConstantType> const& probabilityMatrix, std::vector<std::size_t> const& rowSubstitutions, std::vector<ConstantType>& stateActionRewardVector, std::vector<RewTableEntry*>& rewardEntryToEvalTableMapping, RewTableEntry* constantEntry);

                //The Model with which we work
                std::shared_ptr<storm::models::sparse::Mdp<ConstantType>> model;
                //The formula for which we will compute the values
                std::shared_ptr<storm::logic::OperatorFormula> formula;
                //A flag that denotes whether we compute probabilities or rewards
                bool computeRewards;

                // We store one (unique) entry for every occurring pair of a non-constant function and substitution.
                // Whenever a region is given, we can then evaluate the functions (w.r.t. the corresponding substitutions)
                // and store the result to the target value of this map
                std::unordered_map<FunctionSubstitution, ConstantType, FuncSubHash> probabilityEvaluationTable;
                //For rewards, we map to the minimal value and the maximal value (depending on the CHOSEOPTIMAL parameters).
                std::unordered_map<FunctionSubstitution, std::pair<ConstantType, ConstantType>, FuncSubHash> rewardEvaluationTable;

                //Vector has one entry for every required substitution (=replacement of parameters with lower/upper bounds of region)
                std::vector<std::map<VariableType, TypeOfBound>> probabilitySubstitutions;
                //Same for the different substitutions for the reward functions.
                //In addition, we store the parameters for which the correct substitution 
                //depends on the region and whether to minimize/maximize (i.e. CHOSEOPTIMAL parameters)
                std::vector<std::map<VariableType, TypeOfBound>> rewardSubstitutions;
                std::vector<std::set<VariableType>> choseOptimalRewardPars;

                //This Vector connects the probability evaluation table with the probability matrix of the model.
                //Vector has one entry for every (non-constant) matrix entry.
                //pair.first points to an entry in the evaluation table,
                //pair.second is an iterator to the corresponding matrix entry
                std::vector<std::pair<ConstantType*, typename storm::storage::SparseMatrix<ConstantType>::iterator>> probabilityMapping;
                //Similar for the rewards. But now the first entry points to a minimal and the second one to a maximal value.
                //The third entry points to the state reward vector.
                std::vector<std::tuple<ConstantType*, ConstantType*, typename std::vector<ConstantType>::iterator>> rewardMapping;

            };
        } //namespace region
    }
}


#endif	/* STORM_MODELCHECKER_REGION_APPROXIMATIONMODEL_H */

