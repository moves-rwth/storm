#include "MinMaxLinearEquationSolver.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"
#include <cstdint>

namespace storm {
    namespace solver {
        AbstractMinMaxLinearEquationSolver::AbstractMinMaxLinearEquationSolver(double precision, bool relativeError, uint_fast64_t maximalIterations, bool trackPolicy, MinMaxTechniqueSelection prefTech) :
        precision(precision), relative(relativeError), maximalNumberOfIterations(maximalIterations), trackPolicy(trackPolicy)
        {
            
            if(prefTech == MinMaxTechniqueSelection::FROMSETTINGS) {
                useValueIteration = (storm::settings::generalSettings().getMinMaxEquationSolvingTechnique() == storm::solver::MinMaxTechnique::ValueIteration);
            } else {
                useValueIteration = (prefTech == MinMaxTechniqueSelection::ValueIteration);
            }
        }
    }
}
