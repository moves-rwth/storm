#pragma once

#include <memory>
#include <boost/optional.hpp>

#include "storm/environment/Environment.h"
#include "storm/environment/SubEnvironment.h"

namespace storm {
    
    // Forward declare subenvironments
    class MultiObjectiveModelCheckerEnvironment;
    
    class ModelCheckerEnvironment {
    public:
        
        ModelCheckerEnvironment();
        ~ModelCheckerEnvironment();
        
        MultiObjectiveModelCheckerEnvironment& multi();
        MultiObjectiveModelCheckerEnvironment const& multi() const;

    
    private:
        SubEnvironment<MultiObjectiveModelCheckerEnvironment> multiObjectiveModelCheckerEnvironment;
    };
}

