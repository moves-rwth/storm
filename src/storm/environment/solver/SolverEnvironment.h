#pragma once

#include<memory>
#include <boost/optional.hpp>

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
    class TopologicalLinearEquationSolverEnvironment;
    
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
        TopologicalLinearEquationSolverEnvironment& topological();
        TopologicalLinearEquationSolverEnvironment const& topological() const;

        bool isForceSoundness() const;
        void setForceSoundness(bool value);
        
        storm::solver::EquationSolverType const& getLinearEquationSolverType() const;
        void setLinearEquationSolverType(storm::solver::EquationSolverType const& value, bool assumeSetFromDefault = false);
        bool isLinearEquationSolverTypeSetFromDefaultValue() const;

        std::pair<boost::optional<storm::RationalNumber>, boost::optional<bool>> getPrecisionOfLinearEquationSolver(storm::solver::EquationSolverType const& solverType) const;
        void setLinearEquationSolverPrecision(boost::optional<storm::RationalNumber> const& newPrecision, boost::optional<bool> const& relativePrecision = boost::none);
    
    private:
        SubEnvironment<EigenSolverEnvironment> eigenSolverEnvironment;
        SubEnvironment<GmmxxSolverEnvironment> gmmxxSolverEnvironment;
        SubEnvironment<NativeSolverEnvironment> nativeSolverEnvironment;
        SubEnvironment<GameSolverEnvironment> gameSolverEnvironment;
        SubEnvironment<TopologicalLinearEquationSolverEnvironment> topologicalSolverEnvironment;
        SubEnvironment<MinMaxSolverEnvironment> minMaxSolverEnvironment;
      
        storm::solver::EquationSolverType linearEquationSolverType;
        bool linearEquationSolverTypeSetFromDefault;
        bool forceSoundness;
    };
}

