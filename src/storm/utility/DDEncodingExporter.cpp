#include "storm/utility/DDEncodingExporter.h"
#include "storm/utility/file.h"
#include "storm/models/symbolic/StandardRewardModel.h"

namespace storm {
    namespace exporter {


        template<storm::dd::DdType Type, typename ValueType>
        void explicitExportSymbolicModel(std::string const& filename, std::shared_ptr <storm::models::symbolic::Model<Type, ValueType>> symbolicModel) {
            std::ofstream filestream;
            storm::utility::openFile(filename,filestream);
            filestream << "// storm exported dd" << std::endl;
            filestream << "%transitions" << std::endl;
            storm::utility::closeFile(filestream);
            symbolicModel->getTransitionMatrix().exportToText(filename);
            storm::utility::openFile(filename,filestream,true,true);
            filestream << "%initial" << std::endl;
            storm::utility::closeFile(filestream);
            symbolicModel->getInitialStates().template toAdd<ValueType>().exportToText(filename);
            for(auto const& label : symbolicModel->getLabels()) {
                storm::utility::openFile(filename,filestream,true,true);
                filestream << std::endl << "%label " << label << std::endl;
                storm::utility::closeFile(filestream);
                symbolicModel->getStates(label).template toAdd<ValueType>().exportToText(filename);
            }
        }

        template void explicitExportSymbolicModel<storm::dd::DdType::CUDD,double>(std::string const&, std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>> sparseModel);
        template void explicitExportSymbolicModel<storm::dd::DdType::Sylvan,double>(std::string const&, std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, double>> sparseModel);

        template void explicitExportSymbolicModel<storm::dd::DdType::Sylvan, storm::RationalNumber>(std::string const&, std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, storm::RationalNumber>> sparseModel);
        template void explicitExportSymbolicModel<storm::dd::DdType::Sylvan, storm::RationalFunction>(std::string const&, std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, storm::RationalFunction>> sparseModel);
    }
}
