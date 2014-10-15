// Include generated headers.
#include "storm-config.h"
#include "storm-version.h"

// Include other headers.
#include "src/exceptions/BaseException.h"
#include "src/utility/macros.h"
#include "src/utility/cli.h"
#include "src/utility/export.h"
#include "src/modelchecker/reachability/CollectConstraints.h"

#include "src/modelchecker/reachability/DirectEncoding.h"
#include "src/storage/DeterministicModelStrongBisimulationDecomposition.h"
#include "src/modelchecker/reachability/SparseSccModelChecker.h"
#include "src/storage/parameters.h"
/*!
 * Main entry point of the executable storm.
 */
int main(const int argc, const char** argv) {
    try {
        storm::utility::cli::setUp();
        storm::utility::cli::printHeader(argc, argv);
        bool optionsCorrect = storm::utility::cli::parseOptions(argc, argv);
        if (!optionsCorrect) {
            return -1;
        }
        
        // From this point on we are ready to carry out the actual computations.
        // Program Translation Time Measurement, Start
        std::chrono::high_resolution_clock::time_point programTranslationStart = std::chrono::high_resolution_clock::now();

        // First, we build the model using the given symbolic model description and constant definitions.
        std::string const& programFile = storm::settings::generalSettings().getSymbolicModelFilename();
        std::string const& constants = storm::settings::generalSettings().getConstantDefinitionString();
        storm::prism::Program program = storm::parser::PrismParser::parse(programFile);
        std::shared_ptr<storm::models::AbstractModel<storm::RationalFunction>> model = storm::adapters::ExplicitModelAdapter<storm::RationalFunction>::translateProgram(program, constants);
        
        model->printModelInformationToStream(std::cout);

        // Program Translation Time Measurement, End
        std::chrono::high_resolution_clock::time_point programTranslationEnd = std::chrono::high_resolution_clock::now();
        std::cout << "Parsing and translating the model took " << std::chrono::duration_cast<std::chrono::milliseconds>(programTranslationEnd - programTranslationStart).count() << "ms." << std::endl << std::endl;

        std::shared_ptr<storm::models::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::Dtmc<storm::RationalFunction>>();
        
        // Perform bisimulation minimization if requested.
        if (storm::settings::generalSettings().isBisimulationSet()) {
            storm::storage::DeterministicModelStrongBisimulationDecomposition<storm::RationalFunction> bisimulationDecomposition(*dtmc, true);
            dtmc = bisimulationDecomposition.getQuotient()->as<storm::models::Dtmc<storm::RationalFunction>>();
        }
        
        assert(dtmc);
        storm::modelchecker::reachability::CollectConstraints<storm::RationalFunction> constraintCollector;
        constraintCollector(*dtmc);

        
        storm::modelchecker::reachability::SparseSccModelChecker<storm::RationalFunction> modelChecker;
        
        STORM_LOG_THROW(storm::settings::generalSettings().isPctlPropertySet(), storm::exceptions::InvalidSettingsException, "Unable to perform model checking without a property.");
        std::shared_ptr<storm::properties::prctl::PrctlFilter<double>> filterFormula = storm::parser::PrctlParser::parsePrctlFormula(storm::settings::generalSettings().getPctlProperty());
        
        storm::RationalFunction value = modelChecker.computeReachabilityProbability(*dtmc, filterFormula);
        STORM_PRINT_AND_LOG(std::endl << "computed value " << value << std::endl);
        
        // Get variables from parameter definitions in prism program.
        std::set<storm::Variable> parameters;
        for(auto constant : program.getConstants())
        {
            if(!constant.isDefined())
            {
                carl::Variable p = carl::VariablePool::getInstance().findVariableWithName(constant.getName());
                assert(p != storm::Variable::NO_VARIABLE);
                parameters.insert(p);
            }
        }
        // 
        STORM_LOG_ASSERT(parameters == value.gatherVariables(), "Parameters in result and program definition do not coincide.");
        
        if(storm::settings::parametricSettings().exportResultToFile()) {
            storm::utility::exportParametricMcResult(value, constraintCollector);
        }
        
//        if (storm::settings::parametricSettings().exportToSmt2File()) {
//            storm::modelchecker::reachability::DirectEncoding dec;
//            storm::utility::exportStringStreamToFile(dec.encodeAsSmt2(modelChecker.getMatrix(), parameters,));
//        }        

        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cli::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM to terminate. The message of the exception is: " << exception.what());
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM to terminate. The message of this exception is: " << exception.what());
    }
}

//#include <memory>
//#include <stdint.h>
//
//#include "stormParametric.h"
//#include "adapters/ExplicitModelAdapter.h"
//#include "utility/graph.h"
//#include "modelchecker/reachability/DirectEncoding.h"
//#include "storage/BitVector.h"
//#include "storage/DeterministicTransition.h"
//
//using storm::storage::StateId;
//
//namespace storm
//{
//
//
//void ParametricStormEntryPoint::createModel()
//{
//    mModel = storm::adapters::ExplicitModelAdapter<RationalFunction>::translateProgram(mProgram, mConstants);
//    mModel->printModelInformationToStream(std::cout);
//}
//
//std::string ParametricStormEntryPoint::reachabilityToSmt2(std::string const& label)
//{
//    
//    storm::storage::BitVector phiStates(mModel->getNumberOfStates(), true);
//    storm::storage::BitVector initStates = mModel->getInitialStates();
//    storm::storage::BitVector targetStates = mModel->getLabeledStates(label);
//    
//    std::shared_ptr<models::Dtmc<RationalFunction>> dtmc = mModel->as<models::Dtmc<RationalFunction>>();
//    // 1. make target states absorbing.
//    dtmc->makeAbsorbing(targetStates);
//    // 2. throw away anything which does not add to the reachability probability.
//    // 2a. remove non productive states
//    storm::storage::BitVector productiveStates = utility::graph::performProbGreater0(*dtmc, dtmc->getBackwardTransitions(), phiStates, targetStates);
//    // 2b. calculate set of states wich 
//    storm::storage::BitVector almostSurelyReachingTargetStates = ~utility::graph::performProbGreater0(*dtmc, dtmc->getBackwardTransitions(), phiStates, ~productiveStates);
//    // 2c. Make such states also target states.
//    dtmc->makeAbsorbing(almostSurelyReachingTargetStates);
//    // 2d. throw away non reachable states 
//    storm::storage::BitVector reachableStates = utility::graph::performProbGreater0(*dtmc, dtmc->getTransitionMatrix(), phiStates, initStates);
//    storm::storage::BitVector bv = productiveStates & reachableStates;
//    dtmc->getStateLabeling().addAtomicProposition("__targets__", targetStates | almostSurelyReachingTargetStates);
//    models::Dtmc<RationalFunction> subdtmc = dtmc->getSubDtmc(bv);
//    
//    phiStates = storm::storage::BitVector(subdtmc.getNumberOfStates(), true);
//    initStates = subdtmc.getInitialStates();
//    targetStates = subdtmc.getLabeledStates("__targets__");
//    storm::storage::BitVector deadlockStates(phiStates);
//    deadlockStates.set(subdtmc.getNumberOfStates()-1,false);
//    
//    // Search for states with only one non-deadlock successor.
//    std::map<StateId, storage::DeterministicTransition<RationalFunction>> chainedStates;
//    StateId nrStates = subdtmc.getNumberOfStates();
//    StateId deadlockState = nrStates - 1;
//    for(StateId source = 0; source < nrStates - 1; ++source)
//    {
//        if(targetStates[source])
//        {
//            continue;
//        }
//        storage::DeterministicTransition<RationalFunction> productiveTransition(nrStates);
//        for(auto const& transition : subdtmc.getRows(source))
//        {
//            if(productiveTransition.targetState() == nrStates)
//            {
//                // first transition.
//                productiveTransition = transition;
//            }
//            else
//            {
//                // second transition
//                if(transition.first != deadlockState)
//                {
//                    productiveTransition.targetState() = nrStates;
//                    break;
//                }
//            }
//        }
//        if(productiveTransition.targetState() != nrStates)
//        {
//            chainedStates.emplace(source, productiveTransition);
//        }
//    }
//    storage::BitVector eliminatedStates(nrStates, false);
//    for(auto & chainedState : chainedStates)
//    {
//        assert(chainedState.first != chainedState.second.targetState());
//        auto it = chainedStates.find(chainedState.second.targetState());
//        if(it != chainedStates.end())
//        {
//            //std::cout << "----------------------------" << std::endl;
//            //std::cout << chainedState.first << " -- " << chainedState.second.probability() << " --> " << chainedState.second.targetState() << std::endl;
//            //std::cout << it->first << " -- " << it->second.probability() << " --> " << it->second.targetState() << std::endl;
//            chainedState.second.targetState() = it->second.targetState();
//            chainedState.second.probability() *= it->second.probability();
//            //std::cout << chainedState.first << " -- " << chainedState.second.probability() << " --> " << chainedState.second.targetState() << std::endl;
//            //std::cout << "----------------------------" << std::endl;
//            chainedStates.erase(it);
//            eliminatedStates.set(it->first, true);
//        }
//    }
//    
//    
//    for(auto chainedState : chainedStates)
//    {
//        if(!eliminatedStates[chainedState.first])
//        {
//            std::cout << chainedState.first << " -- " << chainedState.second.probability() << " --> " << chainedState.second.targetState() << std::endl;
//        }
//    }
//    
//    storage::StronglyConnectedComponentDecomposition<RationalFunction> sccs(subdtmc);
//    std::cout << sccs << std::endl;
//
//    modelchecker::reachability::DirectEncoding dec;
//    std::vector<carl::Variable> parameters;
    
//    return dec.encodeAsSmt2(subdtmc, parameters, subdtmc.getLabeledStates("init"), subdtmc.getLabeledStates("__targets__"), mpq_class(1,2));
//    
//}
//
//
//void storm_parametric(const std::string& constants, const storm::prism::Program& program)
//{
//    ParametricStormEntryPoint entry(constants, program);
//    entry.createModel();
//    storm::settings::Settings* s = storm::settings::Settings::getInstance();
//    if(s->isSet("reachability"))
//    {
//        std::ofstream fstream("test.smt2");
//        fstream << entry.reachabilityToSmt2(s->getOptionByLongName("reachability").getArgument(0).getValueAsString());
//        fstream.close();
//    }
//    
//    
//}
//
//}
