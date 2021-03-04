#ifndef STORM_DERIVATIVEEVALUATIONHELPER_H
#define STORM_DERIVATIVEEVALUATIONHELPER_H

#include <map>
#include "analysis/GraphConditions.h"
#include "logic/Formula.h"
#include "solver/LinearEquationSolver.h"
#include "storm-pars/utility/parametric.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/utility/Stopwatch.h"
#include "storm-pars/derivative/ResultType.h"

namespace storm {
    namespace derivative {
        template <typename ValueType, typename ConstantType>
        class DerivativeEvaluationHelper {
        public:
            /**
             * Computes the derivative of the reachability probability or the derivative of the expected reward of a given Dtmc at an instantiation
             * @param model The Dtmc to compute the derivatives of. This must have _one_
             * target state labeled "target" and _one_ initial state labeled "init". Note this is exactly the
             * kind of Dtmc the SparseParametricDtmcSimplifier spits out.
             * The constructor will setup the matrices used for computing the derivatives. Note this can consume
             * a substantial amount of memory if the model is big and there's a large number of parameters.
             * @param parameters The Dtmc's parameters. See storm::models::sparse::getAllParameters
             * @param formulas The first element of this is considered, which needs to be an eventually formula.
             * @param mode The DerivativeEvaluationHelper's mode, whether it should compute the derivative of a probability
             * or the derivative of an expected reward.
             * @param rewardModelName When computing an expected reward, the name of the reward model in the Dtmc.
             */
            DerivativeEvaluationHelper<ValueType, ConstantType>(
                    std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> const model,
                    std::set<typename utility::parametric::VariableType<ValueType>::type> const& parameters,
                    std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas,
                    ResultType mode = ResultType::PROBABILITY,
                    boost::optional<std::string> rewardModelName = boost::none
                ) : model(model), mode(mode), rewardModelName(rewardModelName) {
                std::cout << "Setting up matrices..." << std::endl;
                setup(parameters, formulas);
                std::cout << "Done setting up, starting the search" << std::endl;
            }
            /**
             * calculateDerivative calculates the deriative of the model w.r.t. a parameter at an instantiation
             * @param parameter The parameter w.r.t. the derivivative will be computed
             * @param substitutions The instantiation at which the derivivative will be computed.
             * @param valueVector A vector of reachability probabilities or expected rewards of eventually reaching the model's
             * target (which must have the label "target", see constructor) from the initial state (labeled with "init")
             * which must previously be computed by the user,
             * for instance using the SparseDtmcInstantiationModelChecker. This vector must have exactly as many elements as the
             * model has states, and of course the ith element of the vector is the reachability probability of the ith state.
             */
            ConstantType calculateDerivative(
                const typename utility::parametric::VariableType<ValueType>::type parameter,
                const std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type> &substitutions,
                const std::vector<ConstantType> valueVector
            );
        private:
            std::shared_ptr<models::sparse::Dtmc<ValueType>> model;
            // formula is possibly without bound, but formulaWithoutBound is definitely without bound
            std::shared_ptr<storm::logic::Formula const> formula;
            std::shared_ptr<storm::logic::Formula const> formulaWithoutBound;
            std::map<typename utility::parametric::VariableType<ValueType>::type, std::unique_ptr<storm::solver::LinearEquationSolver<ConstantType>>> linearEquationSolvers;
            std::unique_ptr<storm::Environment> environment;

            std::vector<std::pair<typename storm::storage::SparseMatrix<ConstantType>::iterator, ConstantType*>> matrixMapping; 
            std::unordered_map<ValueType, ConstantType> functions; 

            storage::SparseMatrix<ValueType> constrainedMatrix;
            storage::SparseMatrix<ConstantType> constrainedMatrixInstantiated;
            std::unique_ptr<std::map<typename utility::parametric::VariableType<ValueType>::type, storage::SparseMatrix<ValueType>>> deltaConstrainedMatrices;
            std::unique_ptr<std::map<typename utility::parametric::VariableType<ValueType>::type, storage::SparseMatrix<ConstantType>>> deltaConstrainedMatricesInstantiated;
            std::unique_ptr<std::map<typename utility::parametric::VariableType<ValueType>::type, std::vector<ValueType>>> derivedOutputVecs;

            // next states: states that have a relevant successor
            storage::BitVector next;
            uint_fast64_t initialState;

            ResultType mode;
            boost::optional<std::string> rewardModelName;

            void initializeInstantiatedMatrix(
                storage::SparseMatrix<ValueType> &matrix,
                storage::SparseMatrix<ConstantType> &matrixInstantiated
            );
            void setup(
                std::set<typename utility::parametric::VariableType<ValueType>::type> const& parameters,
                std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas);

            utility::Stopwatch instantiationWatch;
            utility::Stopwatch approximationWatch;
            utility::Stopwatch generalSetupWatch;
        };
    }
}

#endif // STORM_DERIVATIVEEVALUATIONHELPER_H
