#include <memory>

#include "storm/environment/Environment.h"
#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"
#include "storm/environment/modelchecker/ModelCheckerEnvironment.h"

#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/environment/solver/EigenSolverEnvironment.h"
#include "storm/environment/solver/GmmxxSolverEnvironment.h"
#include "storm/environment/solver/NativeSolverEnvironment.h"
#include "storm/environment/solver/LongRunAverageSolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/MultiplierEnvironment.h"
#include "storm/environment/solver/GameSolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"

namespace storm {
    
    template<typename EnvironmentType>
    SubEnvironment<EnvironmentType>::SubEnvironment() : subEnv(nullptr) {
        // Intentionally left empty
    }
    
    template<typename EnvironmentType>
    SubEnvironment<EnvironmentType>::SubEnvironment(SubEnvironment const& other) : subEnv(other.subEnv ? new EnvironmentType(*other.subEnv) : nullptr) {
        // Intentionally left empty
    }
    
    template<typename EnvironmentType>
    SubEnvironment<EnvironmentType>& SubEnvironment<EnvironmentType>::operator=(SubEnvironment const& other) {
        if (other.subEnv) {
            subEnv = std::make_unique<EnvironmentType>(*other.subEnv);
        } else {
            subEnv.reset();
        }
        return *this;
    }
    
    template<typename EnvironmentType>
    EnvironmentType const& SubEnvironment<EnvironmentType>::get() const {
        assertInitialized();
        return *subEnv;
    }
    
    template<typename EnvironmentType>
    EnvironmentType& SubEnvironment<EnvironmentType>::get() {
        assertInitialized();
        return *subEnv;
    }
    
    template<typename EnvironmentType>
    void SubEnvironment<EnvironmentType>::assertInitialized() const {
        if (!subEnv) {
            subEnv = std::make_unique<EnvironmentType>();
        }
    }
    
    template class SubEnvironment<InternalEnvironment>;
    
    template class SubEnvironment<MultiObjectiveModelCheckerEnvironment>;
    template class SubEnvironment<ModelCheckerEnvironment>;
    
    template class SubEnvironment<SolverEnvironment>;
    template class SubEnvironment<EigenSolverEnvironment>;
    template class SubEnvironment<GmmxxSolverEnvironment>;
    template class SubEnvironment<NativeSolverEnvironment>;
    template class SubEnvironment<LongRunAverageSolverEnvironment>;
    template class SubEnvironment<MinMaxSolverEnvironment>;
    template class SubEnvironment<MultiplierEnvironment>;
    template class SubEnvironment<GameSolverEnvironment>;
    template class SubEnvironment<TopologicalSolverEnvironment>;
    
}

