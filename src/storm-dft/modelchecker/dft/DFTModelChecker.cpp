#include "DFTModelChecker.h"

#include "storm/builder/ParallelCompositionBuilder.h"
#include "storm/utility/bitoperations.h"

#include "storm-dft/builder/ExplicitDFTModelBuilder.h"
#include "storm-dft/builder/ExplicitDFTModelBuilderApprox.h"
#include "storm-dft/storage/dft/DFTIsomorphism.h"
#include "storm-dft/settings/modules/DFTSettings.h"


namespace storm {
    namespace modelchecker {

        template<typename ValueType>
        DFTModelChecker<ValueType>::DFTModelChecker() {
            checkResult = storm::utility::zero<ValueType>();
        }

        template<typename ValueType>
        void DFTModelChecker<ValueType>::check(storm::storage::DFT<ValueType> const& origDft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool allowModularisation, bool enableDC, double approximationError) {
            // Initialize
            this->approximationError = approximationError;
            totalTimer.start();

            // Optimizing DFT
            storm::storage::DFT<ValueType> dft = origDft.optimize();

            // TODO Matthias: check that all paths reach the target state for approximation

            // Checking DFT
            if (formula->isProbabilityOperatorFormula() || !allowModularisation) {
                checkResult = checkHelper(dft, formula, symred, allowModularisation, enableDC, approximationError);
            } else {
                std::shared_ptr<storm::models::sparse::Model<ValueType>> model = buildModelComposition(dft, formula, symred, allowModularisation, enableDC);
                // Model checking
                std::unique_ptr<storm::modelchecker::CheckResult> result = checkModel(model, formula);
                result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                checkResult = result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;

            }
            totalTimer.stop();
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
                    if (formula->isProbabilityOperatorFormula()) {
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
                        // WARNING: there is a bug for computing permutations with more than 32 elements
                        STORM_LOG_ASSERT(res.size() < 32, "Permutations work only for < 32 elements");
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
            }

            // If we are here, no modularisation was possible
            STORM_LOG_ASSERT(!modularisationPossible, "Modularisation should not be possible.");
            return checkDFT(dft, formula, symred, enableDC, approximationError);
        }

        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> DFTModelChecker<ValueType>::buildModelComposition(storm::storage::DFT<ValueType> const& dft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool allowModularisation, bool enableDC)  {
            STORM_LOG_TRACE("Build model via composition");
            // Use parallel composition for CTMCs for expected time
            STORM_LOG_ASSERT(formula->isTimeOperatorFormula(), "Formula is not a time operator formula");
            bool modularisationPossible = allowModularisation;

            // Try modularisation
            if(modularisationPossible) {
                std::vector<storm::storage::DFT<ValueType>> dfts;
                bool isAnd = true;

                switch (dft.topLevelType()) {
                    case storm::storage::DFTElementType::AND:
                        STORM_LOG_TRACE("top modularisation called AND");
                        dfts = dft.topModularisation();
                        STORM_LOG_TRACE("Modularisation into " << dfts.size() << " submodules.");
                        modularisationPossible = dfts.size() > 1;
                        isAnd = true;
                        break;
                    case storm::storage::DFTElementType::OR:
                        STORM_LOG_TRACE("top modularisation called OR");
                        dfts = dft.topModularisation();
                        STORM_LOG_TRACE("Modularsation into " << dfts.size() << " submodules.");
                        modularisationPossible = dfts.size() > 1;
                        isAnd = false;
                        break;
                    case storm::storage::DFTElementType::VOT:
                        /*STORM_LOG_TRACE("top modularisation called VOT");
                        dfts = dft.topModularisation();
                        STORM_LOG_TRACE("Modularsation into " << dfts.size() << " submodules.");
                        std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(dft.getTopLevelGate())->threshold();
                         */
                        // TODO enable modularisation for voting gate
                        modularisationPossible = false;
                        break;
                    default:
                        // No static gate -> no modularisation applicable
                        modularisationPossible = false;
                        break;
                }

                if(modularisationPossible) {
                    STORM_LOG_TRACE("Recursive CHECK Call");
                    bool firstTime = true;
                    std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> composedModel;
                    for (auto const ft : dfts) {
                        STORM_LOG_INFO("Building Model via parallel composition...");
                        explorationTimer.start();

                        // Find symmetries
                        std::map<size_t, std::vector<std::vector<size_t>>> emptySymmetry;
                        storm::storage::DFTIndependentSymmetries symmetries(emptySymmetry);
                        if(symred) {
                            auto colouring = ft.colourDFT();
                            symmetries = ft.findSymmetries(colouring);
                            STORM_LOG_INFO("Found " << symmetries.groups.size() << " symmetries.");
                            STORM_LOG_TRACE("Symmetries: " << std::endl << symmetries);
                        }

                        // Build a single CTMC
                        STORM_LOG_INFO("Building Model...");
                        storm::builder::ExplicitDFTModelBuilderApprox<ValueType> builder(ft, symmetries, enableDC);
                        typename storm::builder::ExplicitDFTModelBuilderApprox<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula
                        builder.buildModel(labeloptions, 0, 0.0);
                        std::shared_ptr<storm::models::sparse::Model<ValueType>> model = builder.getModel();
                        //model->printModelInformationToStream(std::cout);
                        STORM_LOG_INFO("No. states (Explored): " << model->getNumberOfStates());
                        STORM_LOG_INFO("No. transitions (Explored): " << model->getNumberOfTransitions());
                        explorationTimer.stop();

                        STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Ctmc), storm::exceptions::NotSupportedException, "Parallel composition only applicable for CTMCs");
                        std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> ctmc = model->template as<storm::models::sparse::Ctmc<ValueType>>();

                        ctmc =  storm::performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(ctmc, {formula}, storm::storage::BisimulationType::Weak)->template as<storm::models::sparse::Ctmc<ValueType>>();

                        if (firstTime) {
                            composedModel = ctmc;
                            firstTime = false;
                        } else {
                            composedModel = storm::builder::ParallelCompositionBuilder<ValueType>::compose(composedModel, ctmc, isAnd);
                        }

                        // Apply bisimulation
                        bisimulationTimer.start();
                        composedModel =  storm::performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(composedModel, {formula}, storm::storage::BisimulationType::Weak)->template as<storm::models::sparse::Ctmc<ValueType>>();
                        std::chrono::high_resolution_clock::time_point bisimulationEnd = std::chrono::high_resolution_clock::now();
                        bisimulationTimer.stop();

                        STORM_LOG_INFO("No. states (Composed): " << composedModel->getNumberOfStates());
                        STORM_LOG_INFO("No. transitions (Composed): " << composedModel->getNumberOfTransitions());
                        if (composedModel->getNumberOfStates() <= 15) {
                            STORM_LOG_TRACE("Transition matrix: " << std::endl << composedModel->getTransitionMatrix());
                        } else {
                            STORM_LOG_TRACE("Transition matrix: too big to print");
                        }

                    }
                    return composedModel;
                }
            }

            // If we are here, no composition was possible
            STORM_LOG_ASSERT(!modularisationPossible, "Modularisation should not be possible.");
            explorationTimer.start();
            // Find symmetries
            std::map<size_t, std::vector<std::vector<size_t>>> emptySymmetry;
            storm::storage::DFTIndependentSymmetries symmetries(emptySymmetry);
            if(symred) {
                auto colouring = dft.colourDFT();
                symmetries = dft.findSymmetries(colouring);
                STORM_LOG_INFO("Found " << symmetries.groups.size() << " symmetries.");
                STORM_LOG_TRACE("Symmetries: " << std::endl << symmetries);
            }
            // Build a single CTMC
            STORM_LOG_INFO("Building Model...");


            storm::builder::ExplicitDFTModelBuilderApprox<ValueType> builder(dft, symmetries, enableDC);
            typename storm::builder::ExplicitDFTModelBuilderApprox<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula
            builder.buildModel(labeloptions, 0, 0.0);
            std::shared_ptr<storm::models::sparse::Model<ValueType>> model = builder.getModel();
            //model->printModelInformationToStream(std::cout);
            STORM_LOG_INFO("No. states (Explored): " << model->getNumberOfStates());
            STORM_LOG_INFO("No. transitions (Explored): " << model->getNumberOfTransitions());
            explorationTimer.stop();
            STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Ctmc), storm::exceptions::NotSupportedException, "Parallel composition only applicable for CTMCs");

            return model->template as<storm::models::sparse::Ctmc<ValueType>>();
        }

        template<typename ValueType>
        typename DFTModelChecker<ValueType>::dft_result DFTModelChecker<ValueType>::checkDFT(storm::storage::DFT<ValueType> const& dft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool enableDC, double approximationError) {
            explorationTimer.start();

            // Find symmetries
            std::map<size_t, std::vector<std::vector<size_t>>> emptySymmetry;
            storm::storage::DFTIndependentSymmetries symmetries(emptySymmetry);
            if(symred) {
                auto colouring = dft.colourDFT();
                symmetries = dft.findSymmetries(colouring);
                STORM_LOG_INFO("Found " << symmetries.groups.size() << " symmetries.");
                STORM_LOG_TRACE("Symmetries: " << std::endl << symmetries);
            }

            if (approximationError > 0.0) {
                // Comparator for checking the error of the approximation
                storm::utility::ConstantsComparator<ValueType> comparator;
                // Build approximate Markov Automata for lower and upper bound
                approximation_result approxResult = std::make_pair(storm::utility::zero<ValueType>(), storm::utility::zero<ValueType>());
                std::shared_ptr<storm::models::sparse::Model<ValueType>> model;
                storm::builder::ExplicitDFTModelBuilderApprox<ValueType> builder(dft, symmetries, enableDC);
                typename storm::builder::ExplicitDFTModelBuilderApprox<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula

                bool probabilityFormula = formula->isProbabilityOperatorFormula();
                STORM_LOG_ASSERT((formula->isTimeOperatorFormula() && !probabilityFormula) || (!formula->isTimeOperatorFormula() && probabilityFormula), "Probability formula not initialized correctly");
                size_t iteration = 0;
                do {
                    // Iteratively build finer models
                    if (iteration > 0) {
                        explorationTimer.start();
                    }
                    STORM_LOG_INFO("Building model...");
                    // TODO Matthias refine model using existing model and MC results
                    builder.buildModel(labeloptions, iteration, approximationError);
                    explorationTimer.stop();
                    buildingTimer.start();

                    // TODO Matthias: possible to do bisimulation on approximated model and not on concrete one?

                    // Build model for lower bound
                    STORM_LOG_INFO("Getting model for lower bound...");
                    model = builder.getModelApproximation(probabilityFormula ? false : true);
                    // We only output the info from the lower bound as the info for the upper bound is the same
                    STORM_LOG_INFO("No. states: " << model->getNumberOfStates());
                    STORM_LOG_INFO("No. transitions: " << model->getNumberOfTransitions());
                    buildingTimer.stop();

                    // Check lower bound
                    std::unique_ptr<storm::modelchecker::CheckResult> result = checkModel(model, formula);
                    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                    ValueType newResult = result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
                    STORM_LOG_ASSERT(iteration == 0 || !comparator.isLess(newResult, approxResult.first), "New under-approximation " << newResult << " is smaller than old result " << approxResult.first);
                    approxResult.first = newResult;

                    // Build model for upper bound
                    STORM_LOG_INFO("Getting model for upper bound...");
                    buildingTimer.start();
                    model = builder.getModelApproximation(probabilityFormula ? true : false);
                    buildingTimer.stop();
                    // Check upper bound
                    result = checkModel(model, formula);
                    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                    newResult = result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
                    STORM_LOG_ASSERT(iteration == 0 || !comparator.isLess(approxResult.second, newResult), "New over-approximation " << newResult << " is greater than old result " << approxResult.second);
                    approxResult.second = newResult;

                    ++iteration;
                    STORM_LOG_INFO("Result after iteration " << iteration << ": (" << std::setprecision(10) << approxResult.first << ", " << approxResult.second << ")");
                    totalTimer.stop();
                    printTimings();
                    totalTimer.start();
                    STORM_LOG_THROW(!storm::utility::isInfinity<ValueType>(approxResult.first) && !storm::utility::isInfinity<ValueType>(approxResult.second), storm::exceptions::NotSupportedException, "Approximation does not work if result might be infinity.");
                } while (!isApproximationSufficient(approxResult.first, approxResult.second, approximationError, probabilityFormula));

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
                    builder.buildModel(labeloptions, 0, 0.0);
                    model = builder.getModel();
                } else {
                    storm::builder::ExplicitDFTModelBuilder<ValueType> builder(dft, symmetries, enableDC);
                    typename storm::builder::ExplicitDFTModelBuilder<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula
                    model = builder.buildModel(labeloptions);
                }
                //model->printModelInformationToStream(std::cout);
                STORM_LOG_INFO("No. states (Explored): " << model->getNumberOfStates());
                STORM_LOG_INFO("No. transitions (Explored): " << model->getNumberOfTransitions());
                explorationTimer.stop();

                // Model checking
                std::unique_ptr<storm::modelchecker::CheckResult> result = checkModel(model, formula);
                result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                return result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
            }
        }

        template<typename ValueType>
        std::unique_ptr<storm::modelchecker::CheckResult> DFTModelChecker<ValueType>::checkModel(std::shared_ptr<storm::models::sparse::Model<ValueType>>& model, std::shared_ptr<const storm::logic::Formula> const& formula) {
            // Bisimulation
            bisimulationTimer.start();
            if (model->isOfType(storm::models::ModelType::Ctmc) && storm::settings::getModule<storm::settings::modules::GeneralSettings>().isBisimulationSet()) {
                STORM_LOG_INFO("Bisimulation...");
                model =  storm::performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(model->template as<storm::models::sparse::Ctmc<ValueType>>(), {formula}, storm::storage::BisimulationType::Weak)->template as<storm::models::sparse::Ctmc<ValueType>>();
                STORM_LOG_INFO("No. states (Bisimulation): " << model->getNumberOfStates());
                STORM_LOG_INFO("No. transitions (Bisimulation): " << model->getNumberOfTransitions());
            }
            bisimulationTimer.stop();
            modelCheckingTimer.start();

            // Check the model
            STORM_LOG_INFO("Model checking...");
            std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formula));
            STORM_LOG_INFO("Model checking done.");
            STORM_LOG_ASSERT(result, "Result does not exist.");
            modelCheckingTimer.stop();
            return result;
        }

        template<typename ValueType>
        bool DFTModelChecker<ValueType>::isApproximationSufficient(ValueType , ValueType , double , bool ) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Approximation works only for double.");
        }

        template<>
        bool DFTModelChecker<double>::isApproximationSufficient(double lowerBound, double upperBound, double approximationError, bool relative) {
            STORM_LOG_THROW(!std::isnan(lowerBound) && !std::isnan(upperBound), storm::exceptions::NotSupportedException, "Approximation does not work if result is NaN.");
            if (relative) {
                return upperBound - lowerBound <= approximationError;
            } else {
                return upperBound - lowerBound <= approximationError * (lowerBound + upperBound) / 2;
            }
        }

        template<typename ValueType>
        void DFTModelChecker<ValueType>::printTimings(std::ostream& os) {
            os << "Times:" << std::endl;
            os << "Exploration:\t" << explorationTimer.getTimeSeconds() << "s" << std::endl;
            os << "Building:\t" << buildingTimer.getTimeSeconds() << "s" << std::endl;
            os << "Bisimulation:\t" << bisimulationTimer.getTimeSeconds() << "s" << std::endl;
            os << "Modelchecking:\t" << modelCheckingTimer.getTimeSeconds() << "s" << std::endl;
            os << "Total:\t\t" << totalTimer.getTimeSeconds() << "s" << std::endl;
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
