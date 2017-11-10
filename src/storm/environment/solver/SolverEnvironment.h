#pragma once

#include<memory>

namespace storm {
    
    // Forward declare subenvironments
//    class EigenSolverEnvironment;
//    class GmmxxSolverEnvironment;
    class MinMaxSolverEnvironment;
 //   class NativeSolverEnvironment;
    
    class SolverEnvironment {
    public:
        
        SolverEnvironment();
        
//        EigenSolverEnvironment& eigen();
//        EigenSolverEnvironment const& eigen() const;
//        GmmxxSolverEnvironment& gmmxx();
//        GmmxxSolverEnvironment const& gmmxx() const;
        MinMaxSolverEnvironment& minMax();
        MinMaxSolverEnvironment const& minMax() const;
//        NativeSolverEnvironment& native();
//        NativeSolverEnvironment const& native() const;

        bool isForceSoundness() const;
        void setForceSoundness(bool value);
    
    private:
//        std::unique_ptr<EigenSolverEnvironment> eigenSolverEnvironment;
//        std::unique_ptr<GmmxxSolverEnvironment> gmmxxSolverEnvironment;
        std::unique_ptr<MinMaxSolverEnvironment> minMaxSolverEnvironment;
 //       std::unique_ptr<NativeSolverEnvironment> nativeSolverEnvironment;
      
        bool forceSoundness;
    };
}

