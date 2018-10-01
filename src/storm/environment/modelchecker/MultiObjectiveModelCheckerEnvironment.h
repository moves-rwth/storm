#pragma once

#include <string>

#include "storm/environment/modelchecker/ModelCheckerEnvironment.h"
#include "storm/modelchecker/multiobjective/MultiObjectiveModelCheckingMethod.h"
#include "storm/adapters/RationalNumberAdapter.h"

namespace storm {
    
    class MultiObjectiveModelCheckerEnvironment {
    public:
        
        MultiObjectiveModelCheckerEnvironment();
        ~MultiObjectiveModelCheckerEnvironment();
        
        storm::modelchecker::multiobjective::MultiObjectiveMethod const& getMethod() const;
        void setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod value);
        
        bool isExportPlotSet() const;
        boost::optional<std::string> getPlotPathUnderApproximation() const;
        void setPlotPathUnderApproximation(std::string const& path);
        void unsetPlotPathUnderApproximation();
        boost::optional<std::string> getPlotPathOverApproximation() const;
        void setPlotPathOverApproximation(std::string const& path);
        void unsetPlotPathOverApproximation();
        boost::optional<std::string> getPlotPathParetoPoints() const;
        void setPlotPathParetoPoints(std::string const& path);
        void unsetPlotPathParetoPoints();
        
        storm::RationalNumber const& getPrecision() const;
        void setPrecision(storm::RationalNumber const& value);
        
        uint64_t const& getMaxSteps() const;
        bool isMaxStepsSet() const;
        void setMaxSteps(uint64_t const& value);
        void unsetMaxSteps();
        
        
    private:
        storm::modelchecker::multiobjective::MultiObjectiveMethod method;
        boost::optional<std::string> plotPathUnderApprox, plotPathOverApprox, plotPathParetoPoints;
        storm::RationalNumber precision;
        boost::optional<uint64_t> maxSteps;
        
    };
}

