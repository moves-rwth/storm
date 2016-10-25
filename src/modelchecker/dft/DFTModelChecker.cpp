#include "src/modelchecker/dft/DFTModelChecker.h"

#include "src/builder/ExplicitDFTModelBuilder.h"
#include "src/builder/ExplicitDFTModelBuilderApprox.h"
#include "src/storage/dft/DFTIsomorphism.h"
#include "src/settings/modules/DFTSettings.h"
#include "src/utility/bitoperations.h"

namespace storm {
    namespace modelchecker {

        template<typename ValueType>
        DFTModelChecker<ValueType>::DFTModelChecker() {
            checkResult = storm::utility::zero<ValueType>();
        }

        template<typename ValueType>
        void DFTModelChecker<ValueType>::check(storm::storage::DFT<ValueType> const& origDft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool allowModularisation, bool enableDC, double approximationError) {
            // Initialize
            this->buildingTime = std::chrono::duration<double>::zero();
            this->explorationTime = std::chrono::duration<double>::zero();
            this->bisimulationTime = std::chrono::duration<double>::zero();
            this->modelCheckingTime = std::chrono::duration<double>::zero();
            this->totalTime = std::chrono::duration<double>::zero();
            this->approximationError = approximationError;
            std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();

            // Optimizing DFT
            storm::storage::DFT<ValueType> dft = origDft.optimize();

            // TODO Matthias: check that all paths reach the target state!

            // Checking DFT
            checkResult = checkHelper(dft, formula, symred, allowModularisation, enableDC, approximationError);
            this->totalTime = std::chrono::high_resolution_clock::now() - totalStart;
        }

        template<typename ValueType>
        typename DFTModelChecker<ValueType>::dft_result DFTModelChecker<ValueType>::checkHelper(storm::storage::DFT<ValueType> const& dft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool allowModularisation, bool enableDC, double approximationError)  {
            STORM_LOG_TRACE("Check helper called");
            bool modularisationPossible = allowModularisation;

            // Try modularisation
            if(modularisationPossible) {
                bool invResults = false;
                std::vector<storm::storage::DFT<ValueType>> dfts;
                size_t nrK = 0; // K out of M
                size_t nrM = 0; // K out of M

                switch (dft.topLevelType()) {
                    case storm::storage::DFTElementType::AND:
                        STORM_LOG_TRACE("top modularisation called AND");
                        dfts = dft.topModularisation();
                        STORM_LOG_TRACE("Modularsation into " << dfts.size() << " submodules.");
                        nrK = dfts.size();
                        nrM = dfts.size();
                        modularisationPossible = dfts.size() > 1;
                        break;
                    case storm::storage::DFTElementType::OR:
                        STORM_LOG_TRACE("top modularisation called OR");
                        dfts = dft.topModularisation();
                        STORM_LOG_TRACE("Modularsation into " << dfts.size() << " submodules.");
                        nrK = 0;
                        nrM = dfts.size();
                        invResults = true;
                        modularisationPossible = dfts.size() > 1;
                        break;
                    case storm::storage::DFTElementType::VOT:
                        STORM_LOG_TRACE("top modularisation called VOT");
                        dfts = dft.topModularisation();
                        STORM_LOG_TRACE("Modularsation into " << dfts.size() << " submodules.");
                        nrK = std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(dft.getTopLevelGate())->threshold();
                        nrM = dfts.size();
                        if(nrK <= nrM/2) {
                            nrK -= 1;
                            invResults = true;
                        }
                        modularisationPossible = dfts.size() > 1;
                        break;
                    default:
                        // No static gate -> no modularisation applicable
                        modularisationPossible = false;
                        break;
                }

                if(modularisationPossible) {
                    STORM_LOG_TRACE("Recursive CHECK Call");
                    // TODO Matthias: enable modularisation for approximation
                    STORM_LOG_ASSERT(approximationError == 0.0, "Modularisation not possible for approximation.");

                    // Recursively call model checking
                    std::vector<ValueType> res;
                    for(auto const ft : dfts) {
                        dft_result ftResult = checkHelper(ft, formula, symred, true, enableDC, 0.0);
                        res.push_back(boost::get<ValueType>(ftResult));
                    }

                    // Combine modularisation results
                    STORM_LOG_TRACE("Combining all results... K=" << nrK << "; M=" << nrM << "; invResults=" << (invResults?"On":"Off"));
                    ValueType result = storm::utility::zero<ValueType>();
                    int limK = invResults ? -1 : nrM+1;
                    int chK = invResults ? -1 : 1;
                    for(int cK = nrK; cK != limK; cK += chK ) {
                        STORM_LOG_ASSERT(cK >= 0, "ck negative.");
                        size_t permutation = smallestIntWithNBitsSet(static_cast<size_t>(cK));
                        do {
                            STORM_LOG_TRACE("Permutation="<<permutation);
                            ValueType permResult = storm::utility::one<ValueType>();
                            for(size_t i = 0; i < res.size(); ++i) {
                                if(permutation & (1 << i)) {
                                    permResult *= res[i];
                                } else {
                                    permResult *= storm::utility::one<ValueType>() - res[i];
                                }
                            }
                            STORM_LOG_TRACE("Result for permutation:"<<permResult);
                            permutation = nextBitPermutation(permutation);
                            result += permResult;
                        } while(permutation < (1 << nrM) && permutation != 0);
                    }
                    if(invResults) {
                        result = storm::utility::one<ValueType>() - result;
                    }
                    return result;
                }
            }

            // If we are here, no modularisation was possible
            STORM_LOG_ASSERT(!modularisationPossible, "Modularisation should not be possible.");
            return checkDFT(dft, formula, symred, enableDC, approximationError);
        }

        template<typename ValueType>
        typename DFTModelChecker<ValueType>::dft_result DFTModelChecker<ValueType>::checkDFT(storm::storage::DFT<ValueType> const& dft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool enableDC, double approximationError) {
            std::chrono::high_resolution_clock::time_point buildingStart = std::chrono::high_resolution_clock::now();

            // Find symmetries
            std::map<size_t, std::vector<std::vector<size_t>>> emptySymmetry;
            storm::storage::DFTIndependentSymmetries symmetries(emptySymmetry);
            if(symred) {
                auto colouring = dft.colourDFT();
                symmetries = dft.findSymmetries(colouring);
                STORM_LOG_INFO("Found " << symmetries.groups.size() << " symmetries.");
                STORM_LOG_TRACE("Symmetries: " << std::endl << symmetries);
            }
            std::chrono::high_resolution_clock::time_point buildingEnd = std::chrono::high_resolution_clock::now();
            buildingTime += buildingEnd - buildingStart;

            if (approximationError > 0.0) {
                // Comparator for checking the error of the approximation
                storm::utility::ConstantsComparator<ValueType> comparator;
                // Build approximate Markov Automata for lower and upper bound
                approximation_result approxResult = std::make_pair(storm::utility::zero<ValueType>(), storm::utility::zero<ValueType>());
                std::chrono::high_resolution_clock::time_point explorationStart;
                std::shared_ptr<storm::models::sparse::Model<ValueType>> model;
                storm::builder::ExplicitDFTModelBuilderApprox<ValueType> builder(dft, symmetries, enableDC);
                typename storm::builder::ExplicitDFTModelBuilderApprox<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula

                size_t iteration = 0;
                do {
                    // Iteratively build finer models
                    explorationStart = std::chrono::high_resolution_clock::now();
                    STORM_LOG_INFO("Building model...");
                    // TODO Matthias refine model using existing model and MC results
                    builder.buildModel(labeloptions, iteration, approximationError);

                    // TODO Matthias: possible to do bisimulation on approximated model and not on concrete one?

                    // Build model for lower bound
                    STORM_LOG_INFO("Getting model for lower bound...");
                    model = builder.getModelApproximation(true);
                    // We only output the info from the lower bound as the info for the upper bound is the same
                    STORM_LOG_INFO("No. states: " << model->getNumberOfStates());
                    STORM_LOG_INFO("No. transitions: " << model->getNumberOfTransitions());
                    explorationTime += std::chrono::high_resolution_clock::now() - explorationStart;
                    // Check lower bound
                    std::unique_ptr<storm::modelchecker::CheckResult> result = checkModel(model, formula);
                    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                    ValueType newResult = result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
                    STORM_LOG_ASSERT(iteration == 0 || !comparator.isLess(newResult, approxResult.first), "New under-approximation " << newResult << " is smaller than old result " << approxResult.first);
                    approxResult.first = newResult;

                    // Build model for upper bound
                    STORM_LOG_INFO("Getting model for upper bound...");
                    explorationStart = std::chrono::high_resolution_clock::now();
                    model = builder.getModelApproximation(false);
                    explorationTime += std::chrono::high_resolution_clock::now() - explorationStart;
                    // Check upper bound
                    result = checkModel(model, formula);
                    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                    newResult = result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
                    STORM_LOG_ASSERT(iteration == 0 || !comparator.isLess(approxResult.second, newResult), "New over-approximation " << newResult << " is greater than old result " << approxResult.second);
                    approxResult.second = newResult;

                    ++iteration;
                    STORM_LOG_INFO("Result after iteration " << iteration << ": (" << std::setprecision(10) << approxResult.first << ", " << approxResult.second << ")");
                    STORM_LOG_THROW(!storm::utility::isInfinity<ValueType>(approxResult.first) && !storm::utility::isInfinity<ValueType>(approxResult.second), storm::exceptions::NotSupportedException, "Approximation does not work if result might be infinity.");
                } while (!isApproximationSufficient(approxResult.first, approxResult.second, approximationError));

                STORM_LOG_INFO("Finished approximation after " << iteration << " iteration" << (iteration > 1 ? "s." : "."));
                return approxResult;
            } else {
                // Build a single Markov Automaton
                STORM_LOG_INFO("Building Model...");
                std::shared_ptr<storm::models::sparse::Model<ValueType>> model;
                // TODO Matthias: use only one builder if everything works again
                if (storm::settings::getModule<storm::settings::modules::DFTSettings>().isApproximationErrorSet()) {
                    storm::builder::ExplicitDFTModelBuilderApprox<ValueType> builder(dft, symmetries, enableDC);
                    typename storm::builder::ExplicitDFTModelBuilderApprox<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula
                    builder.buildModel(labeloptions, 0);
                    model = builder.getModel();
                } else {
                    storm::builder::ExplicitDFTModelBuilder<ValueType> builder(dft, symmetries, enableDC);
                    typename storm::builder::ExplicitDFTModelBuilder<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula
                    model = builder.buildModel(labeloptions);
                }
                //model->printModelInformationToStream(std::cout);
                STORM_LOG_INFO("No. states (Explored): " << model->getNumberOfStates());
                STORM_LOG_INFO("No. transitions (Explored): " << model->getNumberOfTransitions());
                explorationTime += std::chrono::high_resolution_clock::now() - buildingEnd;

                // Model checking
                std::unique_ptr<storm::modelchecker::CheckResult> result = checkModel(model, formula);
                result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                return result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
            }
        }

        template<typename ValueType>
        std::unique_ptr<storm::modelchecker::CheckResult> DFTModelChecker<ValueType>::checkModel(std::shared_ptr<storm::models::sparse::Model<ValueType>>& model, std::shared_ptr<const storm::logic::Formula> const& formula) {
            // Bisimulation
            std::chrono::high_resolution_clock::time_point bisimulationStart = std::chrono::high_resolution_clock::now();
            if (model->isOfType(storm::models::ModelType::Ctmc) && storm::settings::getModule<storm::settings::modules::GeneralSettings>().isBisimulationSet()) {
                STORM_LOG_INFO("Bisimulation...");
                model =  storm::performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(model->template as<storm::models::sparse::Ctmc<ValueType>>(), {formula}, storm::storage::BisimulationType::Weak)->template as<storm::models::sparse::Ctmc<ValueType>>();
                STORM_LOG_INFO("No. states (Bisimulation): " << model->getNumberOfStates());
                STORM_LOG_INFO("No. transitions (Bisimulation): " << model->getNumberOfTransitions());
            }
            std::chrono::high_resolution_clock::time_point bisimulationEnd = std::chrono::high_resolution_clock::now();
            bisimulationTime += bisimulationEnd - bisimulationStart;

            // Check the model
            STORM_LOG_INFO("Model checking...");
            std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formula));
            STORM_LOG_INFO("Model checking done.");
            STORM_LOG_ASSERT(result, "Result does not exist.");
            modelCheckingTime += std::chrono::high_resolution_clock::now() - bisimulationEnd;
            return result;
        }

        template<typename ValueType>
        bool DFTModelChecker<ValueType>::isApproximationSufficient(ValueType lowerBound, ValueType upperBound, double approximationError) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Approximation works only for double.");
        }

        template<>
        bool DFTModelChecker<double>::isApproximationSufficient(double lowerBound, double upperBound, double approximationError) {
            STORM_LOG_THROW(!std::isnan(lowerBound) && !std::isnan(upperBound), storm::exceptions::NotSupportedException, "Approximation does not work if result is NaN.");
            return upperBound - lowerBound <= approximationError * (lowerBound + upperBound) / 2;
        }

        template<typename ValueType>
        void DFTModelChecker<ValueType>::printTimings(std::ostream& os) {
            os << "Times:" << std::endl;
            os << "Building:\t" << buildingTime.count() << std::endl;
            os << "Exploration:\t" << explorationTime.count() << std::endl;
            os << "Bisimulation:\t" << bisimulationTime.count() << std::endl;
            os << "Modelchecking:\t" << modelCheckingTime.count() << std::endl;
            os << "Total:\t\t" << totalTime.count() << std::endl;
        }

        template<typename ValueType>
        void DFTModelChecker<ValueType>::printResult(std::ostream& os) {
            os << "Result: [";
            if (this->approximationError > 0.0) {
                approximation_result result = boost::get<approximation_result>(checkResult);
                os << "(" << result.first << ", " << result.second << ")";
            } else {
                os << boost::get<ValueType>(checkResult);
            }
            os << "]" << std::endl;
        }


        template class DFTModelChecker<double>;

#ifdef STORM_HAVE_CARL
        template class DFTModelChecker<storm::RationalFunction>;
#endif
    }
}
