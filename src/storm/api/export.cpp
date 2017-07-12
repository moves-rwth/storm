#include "storm/api/export.h"

namespace storm {
    namespace api {
        
        void exportJaniModel(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::string const& filename) {
            auto janiSettings = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();
            
            if (janiSettings.isExportAsStandardJaniSet()) {
                storm::jani::Model normalisedModel = model;
                normalisedModel.makeStandardJaniCompliant();
                storm::jani::JsonExporter::toFile(normalisedModel, properties, filename);
            } else {
                storm::jani::JsonExporter::toFile(model, properties, filename);
            }
        }

        void exportJaniModelAsDot(storm::jani::Model const& model, std::string const& filename) {
            std::ofstream out;
            storm::utility::openFile(filename, out);
            model.writeDotToStream(out);
            storm::utility::closeFile(out);
        }

        
    }
}
