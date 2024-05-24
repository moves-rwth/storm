#include "test/storm_gtest.h"

#include "storm/exceptions/GurobiLicenseException.h"
#include "storm/solver/GurobiLpSolver.h"
#include "storm/utility/solver.h"

namespace storm::test {
#ifdef STORM_HAVE_GUROBI
bool noGurobi = false;

bool testGurobiLicense() {
    try {
        auto lpSolver = storm::utility::solver::getLpSolver<double>("test", storm::solver::LpSolverTypeSelection::Gurobi);
    } catch (storm::exceptions::GurobiLicenseException) {
        return false;
    }
    return true;
}
#endif
}  // namespace storm::test
