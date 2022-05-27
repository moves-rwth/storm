#pragma once

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace solver {
enum class GurobiSolverMethod;
}

namespace settings {
namespace modules {

/*!
 * This class represents the settings for Gurobi.
 */
class GurobiSettings : public ModuleSettings {
   public:
    /*!
     * Creates a new set of Gurobi settings.
     */
    GurobiSettings();

    /*!
     * Retrieves whether the integer tolerance has been set.
     *
     * @return True iff the integer tolerance has been set.
     */
    bool isIntegerToleranceSet() const;

    /*!
     * Retrieves the integer tolerance to be used.
     *
     * @return The integer tolerance to be used.
     */
    double getIntegerTolerance() const;

    /*!
     * Retrieves whether the number of threads has been set.
     *
     * @return True iff the number of threads has been set.
     */
    bool isNumberOfThreadsSet() const;

    /*!
     * Retrieves the maximal number of threads Gurobi is allowed to use.
     *
     * @return The maximally allowed number of threads.
     */
    uint64_t getNumberOfThreads() const;

    /*!
     * Retrieves the selected high-level solution strategy for MILPs.
     *
     * @return The high-level solution strategy.
     */
    uint64_t getMIPFocus() const;

    /*!
     * Retrieves the number of MIP solvers, Gurobi spawns in parallel.
     *
     * @return The number of MIP solvers Gurobi spawns in parallel..
     */
    uint64_t getNumberOfConcurrentMipThreads() const;

    /*!
     * Retrieves the solver method
     *
     * @return
     */
    solver::GurobiSolverMethod getMethod() const;

    /*!
     * Retrieves whether the output option was set.
     *
     * @return True iff the output option was set.
     */
    bool isOutputSet() const;

    bool check() const override;

    // The name of the module.
    static const std::string moduleName;
};

}  // namespace modules
}  // namespace settings
}  // namespace storm
