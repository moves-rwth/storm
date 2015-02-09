// Include generated headers.
#include "storm-config.h"

#include <tuple>
#include <sstream>

// Include other headers.
#include "src/exceptions/BaseException.h"
#include "src/utility/macros.h"
#include "src/utility/cli.h"
#include "src/utility/export.h"
#include "src/modelchecker/reachability/CollectConstraints.h"

//#include "src/modelchecker/reachability/DirectEncoding.h"
#include "src/storage/DeterministicModelBisimulationDecomposition.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "src/storage/parameters.h"
#include "src/models/Dtmc.h"

template<typename ValueType>
void printApproximateResult(ValueType const& value) {
    // Intentionally left empty.
}

template<>
void printApproximateResult(storm::RationalFunction const& value) {
    if (value.isConstant()) {
        STORM_PRINT_AND_LOG(" (approximately " << std::setprecision(30) << carl::toDouble(value.constantPart()) << ")" << std::endl);
    }
}

template<typename ValueType>
void check() {
    // From this point on we are ready to carry out the actual computations.
    // Program Translation Time Measurement, Start
    std::chrono::high_resolution_clock::time_point programTranslationStart = std::chrono::high_resolution_clock::now();
    
    if (storm::settings::generalSettings().isSymbolicSet()) {
        std::string programFile = storm::settings::generalSettings().getSymbolicModelFilename();
        std::string constants = storm::settings::generalSettings().getConstantDefinitionString();
        storm::prism::Program program = storm::parser::PrismParser::parse(programFile);
        
        boost::optional<std::shared_ptr<storm::logic::Formula>> formula;
        if (storm::settings::generalSettings().isPropertySet()) {
            formula = storm::parser::FormulaParser(program.getManager().getSharedPointer()).parseFromString(storm::settings::generalSettings().getProperty());
        }
        
        typename storm::builder::ExplicitPrismModelBuilder<ValueType>::Options options;
        if (formula) {
            options = typename storm::builder::ExplicitPrismModelBuilder<ValueType>::Options(*formula.get());
        }
        options.addConstantDefinitionsFromString(program, storm::settings::generalSettings().getConstantDefinitionString());
        
        std::shared_ptr<storm::models::AbstractModel<ValueType>> model = storm::builder::ExplicitPrismModelBuilder<ValueType>::translateProgram(program, options);
        
        // Convert the transition rewards to state rewards if necessary.
        if (model->hasTransitionRewards()) {
            model->convertTransitionRewardsToStateRewards();
        }
        
        // Program Translation Time Measurement, End
        std::chrono::high_resolution_clock::time_point programTranslationEnd = std::chrono::high_resolution_clock::now();
        std::cout << "Parsing and translating the model took " << std::chrono::duration_cast<std::chrono::milliseconds>(programTranslationEnd - programTranslationStart).count() << "ms." << std::endl << std::endl;
        
        model->printModelInformationToStream(std::cout);
        
        if (formula) {
            STORM_LOG_THROW(model->getType() == storm::models::DTMC, storm::exceptions::InvalidArgumentException, "The given model is not a DTMC and, hence, not currently supported.");
            std::shared_ptr<storm::models::Dtmc<ValueType>> dtmc = model->template as<storm::models::Dtmc<ValueType>>();
            
            storm::modelchecker::SparseDtmcEliminationModelChecker<ValueType> modelchecker(*dtmc);
            STORM_LOG_THROW(modelchecker.canHandle(*formula.get()), storm::exceptions::InvalidPropertyException, "Model checker cannot handle the property: '" << *formula.get() << "'.");
            
            std::cout << "Checking formula " << *formula.get() << std::endl;
            
            // Perform bisimulation minimization if requested.
            if (storm::settings::generalSettings().isBisimulationSet()) {
                typename storm::storage::DeterministicModelBisimulationDecomposition<ValueType>::Options options(*dtmc, *formula.get());
                options.weak = storm::settings::bisimulationSettings().isWeakBisimulationSet();
                
                storm::storage::DeterministicModelBisimulationDecomposition<ValueType> bisimulationDecomposition(*dtmc, options);
                *dtmc = std::move(*bisimulationDecomposition.getQuotient()->template as<storm::models::Dtmc<ValueType>>());
                
                dtmc->printModelInformationToStream(std::cout);
            }
            
            STORM_LOG_THROW(dtmc, storm::exceptions::InvalidStateException, "Preprocessing went wrong.");
            
            storm::modelchecker::reachability::CollectConstraints<ValueType> constraintCollector;
            constraintCollector(*dtmc);
            
            std::unique_ptr<storm::modelchecker::CheckResult> result = modelchecker.check(*formula.get());
            ValueType valueFunction = result->asExplicitQuantitativeCheckResult<ValueType>()[*model->getInitialStates().begin()];
            
            if (storm::settings::parametricSettings().exportResultToFile()) {
                storm::utility::exportParametricMcResult(valueFunction, constraintCollector);
            }
            
            // Report the result.
            STORM_PRINT_AND_LOG(std::endl << "Result (initial state): ");
            result->writeToStream(std::cout, model->getInitialStates());
            if (std::is_same<ValueType, storm::RationalFunction>::value) {
                printApproximateResult(valueFunction);
            }
            std::cout << std::endl;
            
            // Generate derivatives for sensitivity analysis if requested.
            if (std::is_same<ValueType, storm::RationalFunction>::value && storm::settings::parametricSettings().isDerivativesSet()) {
                auto allVariables = valueFunction.gatherVariables();
                
                if (!allVariables.empty()) {
                    std::map<storm::Variable, storm::RationalFunction> derivatives;
                    for (auto const& variable : allVariables) {
                        derivatives[variable] = valueFunction.derivative(variable);
                    }
                    
                    std::cout << std::endl << "Derivatives (variable; derivative):" << std::endl;
                    for (auto const& variableDerivativePair : derivatives) {
                        std::cout << "(" << variableDerivativePair.first << "; " << variableDerivativePair.second << ")" << std::endl;
                    }
                }
            }
        }
    }
}

/*!
 * Main entry point of the executable pstorm.
 */
int main(const int argc, const char** argv) {
    try {
        storm::utility::cli::setUp();
        storm::utility::cli::printHeader(argc, argv);
        bool optionsCorrect = storm::utility::cli::parseOptions(argc, argv);
        
        check<storm::RationalFunction>();
        
        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cli::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM to terminate. The message of the exception is: " << exception.what());
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM to terminate. The message of this exception is: " << exception.what());
    }
}
