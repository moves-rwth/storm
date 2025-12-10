#include "UncertaintyResolutionMode.h"

namespace storm {
namespace solver {
std::ostream& operator<<(std::ostream& out, UncertaintyResolutionMode mode) {
    switch (mode) {
        case UncertaintyResolutionMode::Minimize:
            out << "minimize";
            break;
        case UncertaintyResolutionMode::Maximize:
            out << "maximize";
            break;
        case UncertaintyResolutionMode::Robust:
            out << "robust";
            break;
        case UncertaintyResolutionMode::Cooperative:
            out << "cooperative";
            break;
        case UncertaintyResolutionMode::Unset:
            out << "unset";
            STORM_LOG_WARN("Uncertainty resolution mode not set.");
            break;
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

    switch (settingMode) {
        case UncertaintyResolutionModeSetting::Maximize:
            return UncertaintyResolutionMode::Maximize;
        case UncertaintyResolutionModeSetting::Minimize:
            return UncertaintyResolutionMode::Minimize;
        case UncertaintyResolutionModeSetting::Robust:
            return UncertaintyResolutionMode::Robust;
        case UncertaintyResolutionModeSetting::Cooperative:
            return UncertaintyResolutionMode::Cooperative;
    }
}

}  // namespace solver
}  // namespace storm