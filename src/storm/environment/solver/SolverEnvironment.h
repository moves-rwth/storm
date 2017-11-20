#pragma once

#include<memory>

#include "storm/environment/Environment.h"
#include "storm/environment/SubEnvironment.h"
#include "storm/solver/SolverSelectionOptions.h"
#include "storm/adapters/RationalNumberAdapter.h"

namespace storm {
    
    // Forward declare subenvironments
    class EigenSolverEnvironment;
    class GmmxxSolverEnvironment;
    class NativeSolverEnvironment;
    class MinMaxSolverEnvironment;
    class GameSolverEnvironment;
    
    class SolverEnvironment {
    public:
        
        SolverEnvironment();
        ~SolverEnvironment();
        
        EigenSolverEnvironment& eigen();
        EigenSolverEnvironment const& eigen() const;
        GmmxxSolverEnvironment& gmmxx();
        GmmxxSolverEnvironment const& gmmxx() const;
        NativeSolverEnvironment& native();
        NativeSolverEnvironment const& native() const;
        MinMaxSolverEnvironment& minMax();
        MinMaxSolverEnvironment const& minMax() const;
        GameSolverEnvironment& game();
        GameSolverEnvironment const& game() const;

        bool isForceSoundness() const;
        void setForceSoundness(bool value);
        
        storm::solver::EquationSolverType const& getLinearEquationSolverType() const;
        void setLinearEquationSolverType(storm::solver::EquationSolverType const& value);
        bool isLinearEquationSolverTypeSetFromDefaultValue() const;
        
        void setLinearEquationSolverPrecision(storm::RationalNumber const& value);
        void setLinearEquationSolverRelativeTerminationCriterion(bool value);
    
    private:
        SubEnvironment<EigenSolverEnvironment> eigenSolverEnvironment;
        SubEnvironment<GmmxxSolverEnvironment> gmmxxSolverEnvironment;
        SubEnvironment<NativeSolverEnvironment> nativeSolverEnvironment;
        SubEnvironment<GameSolverEnvironment> gameSolverEnvironment;
        SubEnvironment<MinMaxSolverEnvironment> minMaxSolverEnvironment;
      
        storm::solver::EquationSolverType linearEquationSolverType;
        bool linearEquationSolverTypeSetFromDefault;
        bool forceSoundness;
    };
}

