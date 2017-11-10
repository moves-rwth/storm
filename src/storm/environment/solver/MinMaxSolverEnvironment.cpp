#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/utility/macros.h"

namespace storm {
    
    MinMaxSolverEnvironment::MinMaxSolverEnvironment() {
        auto const& minMaxSettings = storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>();
        
        minMaxMethod = minMaxSettings.getMinMaxEquationSolvingMethod();
        methodSetFromDefault = minMaxSettings.isMinMaxEquationSolvingMethodSetFromDefaultValue();
        maxIterationCount = minMaxSettings.getMaximalIterationCount();
        precision = minMaxSettings.getPrecision();
        considerRelativeTerminationCriterion = minMaxSettings.getConvergenceCriterion() == storm::settings::modules::MinMaxEquationSolverSettings::ConvergenceCriterion::Relative;
        STORM_LOG_ASSERT(considerRelativeTerminationCriterion || minMaxSettings.getConvergenceCriterion() == storm::settings::modules::MinMaxEquationSolverSettings::ConvergenceCriterion::Absolute, "Unknown convergence criterion");
        multiplicationStyle = minMaxSettings.getValueIterationMultiplicationStyle();
    }

    storm::solver::MinMaxMethod const& MinMaxSolverEnvironment::getMethod() const {
        return minMaxMethod;
    }
    
    bool const& MinMaxSolverEnvironment::isMethodSetFromDefault() const {
        return methodSetFromDefault;
    }
    
    void MinMaxSolverEnvironment::setMethod(storm::solver::MinMaxMethod value) {
        methodSetFromDefault = false;
        minMaxMethod = value;
    }
    
    uint64_t const& MinMaxSolverEnvironment::getMaximalNumberOfIterations() const {
        return maxIterationCount;
    }
    
    void MinMaxSolverEnvironment::setMaximalNumberOfIterations(uint64_t value) {
        maxIterationCount = value;
    }
    
    storm::RationalNumber const& MinMaxSolverEnvironment::getPrecision() const {
        return precision;
    }
    
    void MinMaxSolverEnvironment::setPrecision(storm::RationalNumber value) {
        precision = value;
    }
    
    bool const& MinMaxSolverEnvironment::getRelativeTerminationCriterion() const {
        return considerRelativeTerminationCriterion;
    }
    
    void MinMaxSolverEnvironment::setRelativeTerminationCriterion(bool value) {
        considerRelativeTerminationCriterion = value;
    }
    
    storm::solver::MultiplicationStyle const& MinMaxSolverEnvironment::getMultiplicationStyle() const {
        return multiplicationStyle;
    }
    
    void MinMaxSolverEnvironment::setMultiplicationStyle(storm::solver::MultiplicationStyle value) {
        multiplicationStyle = value;
    }
    


}
