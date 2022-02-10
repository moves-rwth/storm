#pragma once

#include <string>

#include "storm/solver/LinearEquationSolverRequirements.h"
#include "storm/solver/SolverRequirement.h"

namespace storm {
namespace solver {

class MinMaxLinearEquationSolverRequirements {
   public:
    // The different requirements a solver can have.
    enum class Element {
        // Requirements that are related to the graph structure of the system. Note that the requirements in this
        // category are to be interpreted incrementally in the following sense: whenever the system has a unique
        // solution then a valid initial scheduler is no longer required.
        Acyclic,
        UniqueSolution,
        ValidInitialScheduler,

        // Requirements that are related to bounds for the actual solution.
        LowerBounds,
        UpperBounds
    };

    // The type of a requirement.

    MinMaxLinearEquationSolverRequirements(LinearEquationSolverRequirements const& linearEquationSolverRequirements = LinearEquationSolverRequirements());

    MinMaxLinearEquationSolverRequirements& requireAcyclic(bool critical = true);
    MinMaxLinearEquationSolverRequirements& requireUniqueSolution(bool critical = true);
    MinMaxLinearEquationSolverRequirements& requireValidInitialScheduler(bool critical = true);
    MinMaxLinearEquationSolverRequirements& requireLowerBounds(bool critical = true);
    MinMaxLinearEquationSolverRequirements& requireUpperBounds(bool critical = true);
    MinMaxLinearEquationSolverRequirements& requireBounds(bool critical = true);

    SolverRequirement const& acyclic() const;
    SolverRequirement const& uniqueSolution() const;
    SolverRequirement const& validInitialScheduler() const;
    SolverRequirement const& lowerBounds() const;
    SolverRequirement const& upperBounds() const;
    SolverRequirement const& get(Element const& element) const;

    void clearAcyclic();
    void clearUniqueSolution();
    void clearValidInitialScheduler();
    void clearLowerBounds();
    void clearUpperBounds();
    void clearBounds();

    bool hasEnabledRequirement() const;
    bool hasEnabledCriticalRequirement() const;

    /*!
     * Returns a string that enumerates the enabled requirements
     */
    std::string getEnabledRequirementsAsString() const;

   private:
    SolverRequirement acyclicRequirement;
    SolverRequirement uniqueSolutionRequirement;
    SolverRequirement validInitialSchedulerRequirement;
    SolverRequirement lowerBoundsRequirement;
    SolverRequirement upperBoundsRequirement;
};

}  // namespace solver
}  // namespace storm
