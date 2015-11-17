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
            assert(!policy.empty());
            return policy;
        }
        
        double AbstractMinMaxLinearEquationSolver::getPrecision() const {
            return precision;
        }
        
        bool AbstractMinMaxLinearEquationSolver::getRelative() const {
            return relative;
        }
        
    }
}
