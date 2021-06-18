#include "storm/environment/modelchecker/ModelCheckerEnvironment.h"

#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/ModelCheckerSettings.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidEnvironmentException.h"
#include "storm/exceptions/UnexpectedException.h"


namespace storm {
    
    ModelCheckerEnvironment::ModelCheckerEnvironment() {
        auto const& mcSettings = storm::settings::getModule<storm::settings::modules::ModelCheckerSettings>();
        if (mcSettings.isLtl2daSet()) {
            ltl2da = mcSettings.getLtl2da();
        }
    }
    
    ModelCheckerEnvironment::~ModelCheckerEnvironment() {
        // Intentionally left empty
    }
    
    MultiObjectiveModelCheckerEnvironment& ModelCheckerEnvironment::multi() {
        return multiObjectiveModelCheckerEnvironment.get();
    }
    
    MultiObjectiveModelCheckerEnvironment const& ModelCheckerEnvironment::multi() const {
        return multiObjectiveModelCheckerEnvironment.get();
    }

    bool ModelCheckerEnvironment::isLtl2daSet() const {
        return ltl2da.is_initialized();
    }

    boost::optional<std::string> const& ModelCheckerEnvironment::getLtl2da() const {
        return ltl2da;
    }

    void ModelCheckerEnvironment::setLtl2da(std::string const& value) {
        ltl2da = value;
    }

    void ModelCheckerEnvironment::unsetLtl2da() {
        ltl2da = boost::none;
    }



}
    

