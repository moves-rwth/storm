#include "MinMaxLinearEquationSolver.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"
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
        
        void AbstractMinMaxLinearEquationSolver::setPolicyTracking(bool setToTrue) {
            trackPolicy = setToTrue;
        }
        
        std::vector<storm::storage::sparse::state_type> AbstractMinMaxLinearEquationSolver::getPolicy() const {
            STORM_LOG_THROW(!useValueIteration, storm::exceptions::NotImplementedException, "Getting policies after value iteration is not yet supported!");
            return policy;
        }
    }
}
