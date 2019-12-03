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
    class LongRunAverageSolverEnvironment;
    class MinMaxSolverEnvironment;
    class MultiplierEnvironment;
    class GameSolverEnvironment;
    class TopologicalSolverEnvironment;
    
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
        LongRunAverageSolverEnvironment& lra();
        LongRunAverageSolverEnvironment const& lra() const;
        MinMaxSolverEnvironment& minMax();
        MinMaxSolverEnvironment const& minMax() const;
        MultiplierEnvironment& multiplier();
        MultiplierEnvironment const& multiplier() const;
        GameSolverEnvironment& game();
        GameSolverEnvironment const& game() const;
        TopologicalSolverEnvironment& topological();
        TopologicalSolverEnvironment const& topological() const;

        bool isForceSoundness() const;
        void setForceSoundness(bool value);
        
        storm::solver::EquationSolverType const& getLinearEquationSolverType() const;
        void setLinearEquationSolverType(storm::solver::EquationSolverType const& value, bool isSetFromDefault = false);
        bool isLinearEquationSolverTypeSetFromDefaultValue() const;

        std::pair<boost::optional<storm::RationalNumber>, boost::optional<bool>> getPrecisionOfLinearEquationSolver(storm::solver::EquationSolverType const& solverType) const;
        void setLinearEquationSolverPrecision(boost::optional<storm::RationalNumber> const& newPrecision, boost::optional<bool> const& relativePrecision = boost::none);
    
    private:
        SubEnvironment<EigenSolverEnvironment> eigenSolverEnvironment;
        SubEnvironment<GmmxxSolverEnvironment> gmmxxSolverEnvironment;
        SubEnvironment<NativeSolverEnvironment> nativeSolverEnvironment;
        SubEnvironment<GameSolverEnvironment> gameSolverEnvironment;
        SubEnvironment<TopologicalSolverEnvironment> topologicalSolverEnvironment;
        SubEnvironment<LongRunAverageSolverEnvironment> longRunAverageSolverEnvironment;
        SubEnvironment<MinMaxSolverEnvironment> minMaxSolverEnvironment;
        SubEnvironment<MultiplierEnvironment> multiplierEnvironment;
      
        storm::solver::EquationSolverType linearEquationSolverType;
        bool linearEquationSolverTypeSetFromDefault;
        bool forceSoundness;
    };
}

