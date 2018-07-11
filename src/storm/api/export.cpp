#include "storm/api/export.h"
#include "storm/storage/jani/JaniLocationExpander.h"

namespace storm {
    namespace api {
        
        void exportJaniModel(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::string const& filename) {
            auto janiSettings = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();

            storm::jani::Model exportModel = model;
            if (janiSettings.isLocationVariablesSet()) {
                for(auto const& pair : janiSettings.getLocationVariables()) {
                    storm::jani::JaniLocationExpander expander(exportModel);
                    expander.transform(pair.first, pair.second);
                    exportModel = expander.getResult();
                }
            }

            if (janiSettings.isExportAsStandardJaniSet()) {
                storm::jani::Model normalisedModel = exportModel;
                normalisedModel.makeStandardJaniCompliant();
                storm::jani::JsonExporter::toFile(normalisedModel, properties, filename);
            } else {
                storm::jani::JsonExporter::toFile(exportModel, properties, filename);
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
