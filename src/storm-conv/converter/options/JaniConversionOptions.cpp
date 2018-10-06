#include "storm-conv/converter/options/PrismToJaniConverterOptions.h"

namespace storm {
    namespace converter {

        JaniConversionOptions::JaniConversionOptions() : edgeAssignments(false), flatten(false), substituteConstants(true), allowedModelFeatures(storm::jani::getAllKnownModelFeatures()) {
            // Intentionally left empty
        }

        JaniConversionOptions::JaniConversionOptions(storm::settings::modules::JaniExportSettings const& settings) : locationVariables(settings.getLocationVariables()), edgeAssignments(settings.isAllowEdgeAssignmentsSet()), flatten(settings.isExportFlattenedSet()), substituteConstants(true), allowedModelFeatures(storm::jani::getAllKnownModelFeatures()) {
            if (settings.isEliminateFunctionsSet()) {
                allowedModelFeatures.remove(storm::jani::ModelFeature::Functions);
            }
            if (settings.isEliminateArraysSet()) {
                allowedModelFeatures.remove(storm::jani::ModelFeature::Arrays);
            }
        }
    }
}

