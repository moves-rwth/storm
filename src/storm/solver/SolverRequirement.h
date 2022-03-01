#pragma once

namespace storm {
namespace solver {

class SolverRequirement {
   public:
    SolverRequirement();
    SolverRequirement(SolverRequirement const& other) = default;

    /*!
     * Returns true if this is a requirement of the considered solver.
     */
    operator bool() const;

    /*!
     * Enables this requirement.
     * @param critical if set, it is assumed that the solver will fail in case this requirement is not met
     */
    void enable(bool critical = true);

    /*!
     * Clears this requirement.
     */
    void clear();

    /*!
     * Returns true if the solver fails in case this requirement is not met.
     */
    bool isCritical() const;

   private:
    bool enabled;
    bool critical;
};

}  // namespace solver
}  // namespace storm
