#pragma once

#include <memory>
#include <string>
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

        bool isLtl2daSet() const;
        boost::optional<std::string> const& getLtl2da() const;
        void setLtl2da(std::string const& value);
        void unsetLtl2da();


    private:
        SubEnvironment<MultiObjectiveModelCheckerEnvironment> multiObjectiveModelCheckerEnvironment;
        boost::optional<std::string> ltl2da;
    };
}

