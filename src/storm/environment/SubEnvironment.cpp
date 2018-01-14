#include<memory>
#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/environment/solver/EigenSolverEnvironment.h"
#include "storm/environment/solver/GmmxxSolverEnvironment.h"
#include "storm/environment/solver/NativeSolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/GameSolverEnvironment.h"

namespace storm {
    
    template<typename EnvironmentType>
    SubEnvironment<EnvironmentType>::SubEnvironment() : subEnv(std::make_unique<EnvironmentType>()) {
        // Intentionally left empty
    }
    
    template<typename EnvironmentType>
    SubEnvironment<EnvironmentType>::SubEnvironment(SubEnvironment const& other) : subEnv(new EnvironmentType(*other.subEnv)) {
        // Intentionally left empty
    }
    
    template<typename EnvironmentType>
    SubEnvironment<EnvironmentType>& SubEnvironment<EnvironmentType>::operator=(SubEnvironment const& other) {
        subEnv = std::make_unique<EnvironmentType>(*other.subEnv);
        return *this;
    }
    
    template<typename EnvironmentType>
    EnvironmentType const& SubEnvironment<EnvironmentType>::get() const {
        return *subEnv;
    }
    
    template<typename EnvironmentType>
    EnvironmentType& SubEnvironment<EnvironmentType>::get() {
        return *subEnv;
    }
    
    template class SubEnvironment<SolverEnvironment>;
    template class SubEnvironment<EigenSolverEnvironment>;
    template class SubEnvironment<GmmxxSolverEnvironment>;
    template class SubEnvironment<NativeSolverEnvironment>;
    template class SubEnvironment<MinMaxSolverEnvironment>;
    template class SubEnvironment<GameSolverEnvironment>;
    
}

