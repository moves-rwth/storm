#pragma once

#include "storm/settings/SettingsManager.h"

#include "storm/utility/DirectEncodingExporter.h"
#include "storm/utility/DDEncodingExporter.h"
#include "storm/utility/file.h"
#include "storm/utility/macros.h"

namespace storm {
    
    namespace jani {
        class Model;
    }
    
    namespace api {
        
        void exportJaniModelAsDot(storm::jani::Model const& model, std::string const& filename);

        template <typename ValueType>
        void exportSparseModelAsDrn(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename, std::vector<std::string> const& parameterNames) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            storm::exporter::explicitExportSparseModel(stream, model, parameterNames);
            storm::utility::closeFile(stream);
        }

        template<storm::dd::DdType Type, typename ValueType>
        void exportSparseModelAsDrdd(std::shared_ptr<storm::models::symbolic::Model<Type,ValueType>> const& model, std::string const& filename) {
            storm::exporter::explicitExportSymbolicModel(filename, model);
        }
        
        template <typename ValueType>
        void exportSparseModelAsDot(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            model->writeDotToStream(stream);
            storm::utility::closeFile(stream);
        }

        template<storm::dd::DdType Type, typename ValueType>
        void exportSymbolicModelAsDot(std::shared_ptr<storm::models::symbolic::Model<Type,ValueType>> const& model, std::string const& filename) {
            model->writeDotToFile(filename);
        }
    }
}
