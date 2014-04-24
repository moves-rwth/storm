#include <memory>
#include <stdint.h>

#include "stormParametric.h"
#include "adapters/ExplicitModelAdapter.h"
#include "utility/graph.h"
#include "modelchecker/reachability/DirectEncoding.h"
#include "storage/BitVector.h"
#include "storage/DeterministicTransition.h"

using storm::storage::StateId;

namespace storm
{


void ParametricStormEntryPoint::createModel()
{
    mModel = storm::adapters::ExplicitModelAdapter<RationalFunction>::translateProgram(mProgram, mConstants);
    mModel->printModelInformationToStream(std::cout);
}

std::string ParametricStormEntryPoint::reachabilityToSmt2(std::string const& label)
{
    
    storm::storage::BitVector phiStates(mModel->getNumberOfStates(), true);
    storm::storage::BitVector initStates = mModel->getInitialStates();
    storm::storage::BitVector targetStates = mModel->getLabeledStates(label);
    
    std::shared_ptr<models::Dtmc<RationalFunction>> dtmc = mModel->as<models::Dtmc<RationalFunction>>();
    // 1. make target states absorbing.
    dtmc->makeAbsorbing(targetStates);
    // 2. throw away anything which does not add to the reachability probability.
    // 2a. remove non productive states
    storm::storage::BitVector productive = utility::graph::performProbGreater0(*dtmc, dtmc->getBackwardTransitions(), phiStates, targetStates);
    // 2b. throw away non reachable states
    storm::storage::BitVector reachable = utility::graph::performProbGreater0(*dtmc, dtmc->getTransitionMatrix(), phiStates, initStates);
    storm::storage::BitVector bv = productive & reachable;
    models::Dtmc<RationalFunction> subdtmc = dtmc->getSubDtmc(bv);
    
    phiStates = storm::storage::BitVector(subdtmc.getNumberOfStates(), true);
    initStates = subdtmc.getInitialStates();
    targetStates = subdtmc.getLabeledStates(label);
    storm::storage::BitVector deadlockStates(phiStates);
    deadlockStates.set(subdtmc.getNumberOfStates()-1,false);
    // Calculate whether there are states which surely lead into the target.
    storm::storage::BitVector potentialIntoDeadlock = utility::graph::performProbGreater0(subdtmc, subdtmc.getBackwardTransitions(), phiStates, deadlockStates);
    storm::storage::BitVector extraTargets = ~potentialIntoDeadlock & ~targetStates;
    if(extraTargets.empty())
    {
        // TODO implement this if necessary.
        std::cout << "Extra targets exist. Please implement!" << std::endl;
    }
    // Search for states with only one non-deadlock successor.
    std::map<StateId, storage::DeterministicTransition<RationalFunction>> chainedStates;
    StateId nrStates = subdtmc.getNumberOfStates();
    StateId deadlockState = nrStates - 1;
    for(StateId source = 0; source < nrStates; ++source)
    {
        storage::DeterministicTransition<RationalFunction> productiveTransition(nrStates);
        for(auto const& transition : subdtmc.getRows(source))
        {
            if(productiveTransition.targetState() == nrStates)
            {
                // first transition.
                productiveTransition = transition;
            }
            else
            {
                // second transition
                if(transition.first != deadlockState)
                {
                    productiveTransition.targetState() = nrStates;
                    break;
                }
            }
        }
        if(productiveTransition.targetState() != nrStates)
        {
            chainedStates.emplace(source, productiveTransition);
        }
        for(auto chainedState : chainedStates)
        {
            auto it = chainedStates.find(chainedState.second.targetState());
            if(it != chainedStates.end())
            {
                chainedState.second.targetState() = it->second.targetState();
                chainedState.second.probability() *= it->second.probability();
            }
        }
    }

    modelchecker::reachability::DirectEncoding dec;
    std::vector<carl::Variable> parameters;
    for(auto constant : mProgram.getConstants())
    {
        if(!constant.isDefined())
        {
            std::cout << constant.getName() << std::endl;
            carl::Variable p = carl::VariablePool::getInstance().findVariableWithName(constant.getName());
            assert(p != carl::Variable::NO_VARIABLE);
            parameters.push_back(p);
        }
    }
    return dec.encodeAsSmt2(subdtmc, parameters, subdtmc.getLabeledStates("init"), subdtmc.getLabeledStates(label), mpq_class(1,2));
    
}


void storm_parametric(const std::string& constants, const storm::prism::Program& program)
{
    ParametricStormEntryPoint entry(constants, program);
    entry.createModel();
    storm::settings::Settings* s = storm::settings::Settings::getInstance();
    if(s->isSet("reachability"))
    {
        std::ofstream fstream("test.smt2");
        fstream << entry.reachabilityToSmt2(s->getOptionByLongName("reachability").getArgument(0).getValueAsString());
        fstream.close();
    }
    
    
}

}
