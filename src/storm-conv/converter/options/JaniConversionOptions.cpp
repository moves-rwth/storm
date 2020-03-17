#include "storm-conv/converter/options/PrismToJaniConverterOptions.h"

namespace storm {
    namespace converter {

        JaniConversionOptions::JaniConversionOptions() : edgeAssignments(false), flatten(false), substituteConstants(true), localVars(false), globalVars(false), allowedModelFeatures(storm::jani::getAllKnownModelFeatures()), addPropertyConstants(true), replaceUnassignedVariablesWithConstants(false), simplifyComposition(false) {
            // Intentionally left empty
        }

        JaniConversionOptions::JaniConversionOptions(storm::settings::modules::JaniExportSettings const& settings) : locationVariables(settings.getLocationVariables()), edgeAssignments(settings.isAllowEdgeAssignmentsSet()), flatten(settings.isExportFlattenedSet()), substituteConstants(true), localVars(settings.isLocalVarsSet()), globalVars(settings.isGlobalVarsSet()), allowedModelFeatures(storm::jani::getAllKnownModelFeatures()), addPropertyConstants(true), replaceUnassignedVariablesWithConstants(settings.isReplaceUnassignedVariablesWithConstantsSet()), simplifyComposition(settings.isSimplifyCompositionSet()) {
            if (settings.isEliminateFunctionsSet()) {
                allowedModelFeatures.remove(storm::jani::ModelFeature::Functions);
            }
            if (settings.isEliminateArraysSet()) {
                allowedModelFeatures.remove(storm::jani::ModelFeature::Arrays);
            }
        }
    }
}

