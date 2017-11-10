#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"

namespace storm {
    
    SolverEnvironment::SolverEnvironment() :
//        eigenSolverEnvironment(std::make_unique<EigenSolverEnvironment>()),
 //       gmmxxSolverEnvironment(std::make_unique<GmmxxSolverEnvironment>()),
        minMaxSolverEnvironment(std::make_unique<MinMaxSolverEnvironment>()) //,
//        nativeSolverEnvironment(std::make_unique<NativeSolverEnvironment>()) {
    { }
    
    
    MinMaxSolverEnvironment& SolverEnvironment::minMax() {
        return *minMaxSolverEnvironment;
    }
    
    MinMaxSolverEnvironment const& SolverEnvironment::minMax() const{
        return *minMaxSolverEnvironment;
    }

    bool SolverEnvironment::isForceSoundness() const {
        return forceSoundness;
    }
    
    void SolverEnvironment::setForceSoundness(bool value) {
        SolverEnvironment::forceSoundness = value;
    }
}
    

