#include "storm-conv/converter/options/PrismToJaniConverterOptions.h"

namespace storm {
    namespace converter {

        JaniConversionOptions::JaniConversionOptions() : standardCompliant(false), flatten(false), allowArrays(true), allowFunctions(true) {
            // Intentionally left empty
        };

        JaniConversionOptions::JaniConversionOptions(storm::settings::modules::JaniExportSettings const& settings) : locationVariables(settings.getLocationVariables()), standardCompliant(settings.isExportAsStandardJaniSet()), flatten(settings.isExportFlattenedSet()), allowArrays(!settings.isEliminateArraysSet()), allowFunctions(!settings.isEliminateFunctionsSet()) {
            // Intentionally left empty
        };
    }
}

