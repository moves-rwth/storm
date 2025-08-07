#include "test/storm_gtest.h"

#include "storm/exceptions/GurobiLicenseException.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GurobiSettings.h"
#include "storm/solver/GurobiLpSolver.h"
#include "storm/utility/solver.h"

namespace storm::test {
bool noGurobi = false;

bool testGurobiLicense() {
#ifdef STORM_HAVE_GUROBI
    if (!storm::settings::hasModule<storm::settings::modules::GurobiSettings>()) {
        return true;  // Gurobi not relevant for this test suite
    }
    try {
        auto lpSolver = storm::utility::solver::getLpSolver<double>("test", storm::solver::LpSolverTypeSelection::Gurobi);
    } catch (storm::exceptions::GurobiLicenseException) {
        return false;
    }
    return true;
#else
    return false;
#endif
}
}  // namespace storm::test
