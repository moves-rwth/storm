#include "storm-conv/converter/options/PrismToJaniConverterOptions.h"

namespace storm {
    namespace converter {

        JaniConversionOptions::JaniConversionOptions() : standardCompliant(false), exportFlattened(false) {
            // Intentionally left empty
        };

        JaniConversionOptions::JaniConversionOptions(storm::settings::modules::JaniExportSettings const& settings) : locationVariables(settings.getLocationVariables()), standardCompliant(settings.isExportAsStandardJaniSet()), exportFlattened(settings.isExportFlattenedSet()) {
            // Intentionally left empty
        };
    }
}

