#pragma once

#include "storm/storage/jani/JSONExporter.h"


#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/JaniExportSettings.h"

#include "storm/utility/DirectEncodingExporter.h"
#include "storm/utility/file.h"
#include "storm/utility/macros.h"

namespace storm {
    namespace api {
        
        void exportJaniModel(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::string const& filename);
        
        void exportJaniModelAsDot(storm::jani::Model const& model, std::string const& filename);

        template <typename ValueType>
        void exportSparseModelAsDrn(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename, std::vector<std::string> const& parameterNames) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            storm::exporter::explicitExportSparseModel(stream, model, parameterNames);
            storm::utility::closeFile(stream);
        }
        
        template <typename ValueType>
        void exportSparseModelAsDot(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            model->writeDotToStream(stream);
            storm::utility::closeFile(stream);
        }
        
    }
}
