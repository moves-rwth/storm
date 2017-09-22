#pragma once

namespace storm {
    namespace solver {
        
        enum class MinMaxLinearEquationSolverSystemType {
            UntilProbabilities,
            ReachabilityRewards,
            StochasticShortestPath
        };
        
    }
}
