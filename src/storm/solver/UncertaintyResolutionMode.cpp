#include "UncertaintyResolutionMode.h"

namespace storm {
namespace solver {
std::ostream& operator<<(std::ostream& out, UncertaintyResolutionMode mode) {
    if (mode == UncertaintyResolutionMode::Minimize) {
        return out << "minimize";
    } else if (mode == UncertaintyResolutionMode::Maximize) {
        return out << "maximize";
    } else if (mode == UncertaintyResolutionMode::Robust) {
        return out << "robust";
    } else if (mode == UncertaintyResolutionMode::Cooperative) {
        return out << "cooperative";
    } else {
        STORM_LOG_WARN("Uncertainty resolution mode not set.");
        return out << "unset";
    }
}

bool isSet(UncertaintyResolutionMode uncertaintyResolutionMode) {
    return uncertaintyResolutionMode != UncertaintyResolutionMode::Unset;
}

bool isUncertaintyResolvedRobust(UncertaintyResolutionMode uncertaintyResolutionMode, OptimizationDirection optimizationDirection) {
    if (uncertaintyResolutionMode == UncertaintyResolutionMode::Minimize) {
        if (optimizationDirection != OptimizationDirection::Minimize) {
            return true;
        } else {
            return false;
        }
    }

    if (uncertaintyResolutionMode == UncertaintyResolutionMode::Maximize) {
        if (optimizationDirection != OptimizationDirection::Maximize) {
            return true;
        } else {
            return false;
        }
    }

    if (uncertaintyResolutionMode == UncertaintyResolutionMode::Cooperative) {
        return false;
    }

    if (uncertaintyResolutionMode == UncertaintyResolutionMode::Robust) {
        return true;
    }

    STORM_LOG_WARN("Uncertainty resolution mode not set properly, assuming to resolve uncertainty in a robust fashion.");
    return true;
}

UncertaintyResolutionMode convert(UncertaintyResolutionModeSetting settingMode) {
    STORM_LOG_THROW(settingMode != UncertaintyResolutionModeSetting::Both, storm::exceptions::InvalidSettingsException,
                    "Cannot convert uncertainty resolution setting mode 'both'");
    return static_cast<UncertaintyResolutionMode>(settingMode);
}

}  // namespace solver
}  // namespace storm