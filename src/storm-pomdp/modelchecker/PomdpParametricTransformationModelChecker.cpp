#include "PomdpParametricTransformationModelChecker.h"

#include "storm-pomdp/transformer/ApplyFiniteSchedulerToPomdp.h"
#include "storm-pomdp/transformer/PomdpMemoryUnfolder.h"
#include "storm-pomdp/transformer/BinaryPomdpTransformer.h"
#include "storm-pars/api/region.h"
#include "storm/analysis/GraphConditions.h"

#include "storm-pars/settings/modules/MonotonicitySettings.h"
#include "storm-pars/derivative/GradientDescentInstantiationSearcher.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

namespace storm {
    namespace pomdp {
        namespace modelchecker {
            template <typename ValueType>
            PomdpParametricTransformationModelChecker<ValueType>::PomdpParametricTransformationModelChecker(storm::models::sparse::Pomdp<ValueType> const& pomdp) : pomdp(pomdp) {
                // Intentionally left empty
            }

            template <typename ValueType>
            std::vector<ValueType> PomdpParametricTransformationModelChecker<ValueType>::computeValuesForFMPolicy(storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& formulaInfo, uint64_t memoryBound, storm::storage::PomdpMemoryPattern memoryPattern, double precision){
                // Apply memory structure to POMDP
                STORM_LOG_ERROR_COND(memoryBound > 0, "Invalid memory bound" << memoryBound << "for transformation from POMDP to pMC. Memory bound needs to be positive!");
                STORM_PRINT_AND_LOG("Compute values in POMDP by transformation to pMC.");
                auto memPomdp = std::make_shared<storm::models::sparse::Pomdp<ValueType>>(pomdp);
                if (memoryBound > 1) {
                    STORM_PRINT_AND_LOG("Computing the unfolding for memory bound " << memoryBound << " and memory pattern '" << storm::storage::toString(memoryPattern) << "' ...");
                    storm::storage::PomdpMemory memory = storm::storage::PomdpMemoryBuilder().build(memoryPattern, memoryBound);
                    storm::transformer::PomdpMemoryUnfolder<ValueType> memoryUnfolder(pomdp, memory);
                    memPomdp = memoryUnfolder.transform(false);
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    memPomdp->printModelInformationToStream(std::cout);
                }

                STORM_PRINT_AND_LOG("Simplification POMDP...");
                auto transformResult = storm::transformer::BinaryPomdpTransformer<ValueType>().transform(*memPomdp, true);
                memPomdp = transformResult.transformedPomdp;
                STORM_PRINT_AND_LOG(" done." << std::endl);
                memPomdp->printModelInformationToStream(std::cout);
                STORM_PRINT_AND_LOG("Transforming POMDP to pMC...");
                storm::transformer::ApplyFiniteSchedulerToPomdp<ValueType> toPMCTransformer(*memPomdp);

                //TODO make mode a setting?
                auto pmc = toPMCTransformer.transform(storm::transformer::PomdpFscApplicationMode::STANDARD);
                STORM_PRINT_AND_LOG(" done." << std::endl);
                pmc->printModelInformationToStream(std::cout);

                // END TRANSFORMATION

                storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>> simplifier(*(pmc->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>()));

                if (!simplifier.simplify(formula)){
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Simplifying the model was not successfull.");
                }
                auto modelSimplified = simplifier.getSimplifiedModel();

                modelSimplified->printModelInformationToStream(std::cout);

                storm::derivative::GradientDescentInstantiationSearcher<storm::RationalFunction, double> gradientDescentInstantiationSearcher(*(modelSimplified->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>()));

                gradientDescentInstantiationSearcher.specifyFormula(Environment(), storm::api::createTask<storm::RationalFunction>(formula.asSharedPointer(), false));

                auto gradDescResult = gradientDescentInstantiationSearcher.gradientDescentOpt(Environment());

                /*
                std::map<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient> lowerBoundaries;
                std::map<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient> upperBoundaries;

                storm::analysis::ConstraintCollector<storm::RationalFunction> constraints(*pmc);
                auto const& parameterSet = constraints.getVariables();
                std::vector<storm::RationalFunctionVariable> parameters(parameterSet.begin(), parameterSet.end());
                //TODO bound
                storm::RationalFunctionCoefficient bound = storm::utility::convertNumber<storm::RationalFunctionCoefficient>(0.01);
                for (auto const& parameter : parameters) {
                    lowerBoundaries.emplace(std::make_pair(parameter, 0+bound));
                    upperBoundaries.emplace(std::make_pair(parameter, 1-bound));
                }
                auto parRegion = storm::storage::ParameterRegion<storm::RationalFunction>(std::move(lowerBoundaries), std::move(upperBoundaries));

                boost::optional<storm::RationalFunction> prec;
                prec = storm::utility::convertNumber<storm::RationalFunction>(precision);

                auto extremalValues = storm::api::computeExtremalValue(pmc, storm::api::createTask<storm::RationalFunction>(formula.asSharedPointer(), false), parRegion, storm::modelchecker::RegionCheckEngine::ParameterLifting, formulaInfo.getOptimizationDirection(), prec , storm::api::MonotonicitySetting(false, false, false), true);
*/
                // the POMDPs we consider are binary, thus one value suffices for each observation
                std::vector<double> obsChoiceWeight(memPomdp->getNrObservations(), 1);

                //for(auto const &parVal : extremalValues.second){
                for(auto const &parVal : gradDescResult.first){

                    auto par = parVal.first;
                    auto val = parVal.second;
                    std::string parName = par.name();
                    STORM_LOG_DEBUG("PAR " << parName << " --  VAL: " << val);
                    parName.erase (std::remove(parName.begin(), parName.end(), 'p'), parName.end());

                    std::vector<std::string> splitValues;
                    boost::split(splitValues, parName, boost::is_any_of("_"));

                    obsChoiceWeight[std::stoi(splitValues[0])] = storm::utility::convertNumber<double>(val);
                }

                std::vector<storm::storage::Distribution<ValueType, uint_fast64_t>> choiceDistributions(memPomdp->getNrObservations());
                for (uint64_t obs = 0; obs < memPomdp->getNrObservations(); ++obs) {
                    auto& choiceDistribution = choiceDistributions[obs];
                    choiceDistribution.addProbability(0, obsChoiceWeight[obs]);
                    if(memPomdp->getNumberOfChoices(memPomdp->getStatesWithObservation(obs).front()) > 1){
                        choiceDistribution.addProbability(1, 1 - obsChoiceWeight[obs]);
                    }
                }

                storm::storage::Scheduler<ValueType> pomdpScheduler(memPomdp->getNumberOfStates());

                // Set the scheduler for all states
                for (uint64_t state = 0; state < memPomdp->getNumberOfStates(); ++state) {
                    pomdpScheduler.setChoice(choiceDistributions[memPomdp->getObservation(state)], state);
                }
                auto underlyingMdp = std::make_shared<storm::models::sparse::Mdp<ValueType>>(memPomdp->getTransitionMatrix(), memPomdp->getStateLabeling(), memPomdp->getRewardModels());
                auto scheduledModel = underlyingMdp->applyScheduler(pomdpScheduler, false);

                auto resultPtr = storm::api::verifyWithSparseEngine<ValueType>(scheduledModel, storm::api::createTask<ValueType>(formula.asSharedPointer(), false));
                STORM_LOG_THROW(resultPtr, storm::exceptions::UnexpectedException, "No check result obtained.");
                STORM_LOG_THROW(resultPtr->isExplicitQuantitativeCheckResult(), storm::exceptions::UnexpectedException, "Unexpected Check result Type");
                std::vector<ValueType> pomdpSchedulerResult = std::move(resultPtr->template asExplicitQuantitativeCheckResult<ValueType>().getValueVector());

                std::stringstream resStr;
                resStr << "Parametric Preprocessing Result: ";
                resStr << (formulaInfo.maximize() ? "≥ " : "≤ ");
                resStr << pomdpSchedulerResult[0];
                STORM_PRINT_AND_LOG(resStr.str() << std::endl);

                std::vector<ValueType> result(pomdp.getNumberOfStates());

                // Initialise the result vector
                for(uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state){
                    if(formulaInfo.isNonNestedExpectedRewardFormula()){
                        result[state] =  formulaInfo.maximize() ? -storm::utility::infinity<ValueType>() : storm::utility::infinity<ValueType>();
                    } else {
                        result[state] =  formulaInfo.maximize() ? storm::utility::zero<ValueType>() : storm::utility::one<ValueType>();
                    }
                }

                for (uint64_t state = 0; state < pomdp.getNumberOfStates() * memoryBound; ++state) {
                    result[state / memoryBound] = formulaInfo.maximize() ? storm::utility::max(result[state / memoryBound], pomdpSchedulerResult[state]) :  storm::utility::min(result[state / memoryBound], pomdpSchedulerResult[state]);
                }

                // Cleanup values close to zero
                for (uint64_t state = 0; state < result.size(); ++state) {
                    if(storm::utility::isAlmostZero<ValueType>(result[state])){
                        result[state] = storm::utility::zero<ValueType>();
                    }
                    STORM_LOG_ASSERT(formulaInfo.isNonNestedExpectedRewardFormula() || (result[state] <= storm::utility::one<ValueType>()), "Result " << result[state] << " for state " << state << " is greater than 1.");
                }

                return result;
            }

            template class PomdpParametricTransformationModelChecker<double>;

            template class PomdpParametricTransformationModelChecker<storm::RationalNumber>;
        }
    }
}