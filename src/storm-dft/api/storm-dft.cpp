#include "storm-dft/api/storm-dft.h"

#include "storm-dft/settings/modules/FaultTreeSettings.h"
#include "storm-dft/settings/modules/DftGspnSettings.h"
#include "storm-conv/settings/modules/JaniExportSettings.h"
#include "storm-conv/api/storm-conv.h"

namespace storm {
    namespace api {

        template<>
        void exportDFTToJsonFile(storm::storage::DFT<double> const& dft, std::string const& file) {
            storm::storage::DftJsonExporter<double>::toFile(dft, file);
        }

        template<>
        void exportDFTToJsonFile(storm::storage::DFT<storm::RationalFunction> const& dft, std::string const& file) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Export to JSON not supported for this data type.");
        }

        template<>
        std::string exportDFTToJsonString(storm::storage::DFT<double> const& dft) {
            std::stringstream stream;
            storm::storage::DftJsonExporter<double>::toStream(dft, stream);
            return stream.str();
        }

        template<>
        std::string exportDFTToJsonString(storm::storage::DFT<storm::RationalFunction> const& dft) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Export to JSON not supported for this data type.");
        }

        template<>
        void exportDFTToSMT(storm::storage::DFT<double> const &dft, std::string const &file) {
            storm::modelchecker::DFTASFChecker asfChecker(dft);
            asfChecker.convert();
            asfChecker.toFile(file);
        }

        template<>
        void exportDFTToSMT(storm::storage::DFT<storm::RationalFunction> const &dft, std::string const &file) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Export to SMT does not support this data type.");
        }

        template<>
        void
        analyzeDFTSMT(storm::storage::DFT<double> const &dft, bool printOutput) {
            uint64_t solverTimeout = 10;

            storm::modelchecker::DFTASFChecker smtChecker(dft);
            smtChecker.toSolver();
            // Removed bound computation etc. here
            smtChecker.setSolverTimeout(solverTimeout);
            smtChecker.checkTleNeverFailed();
            smtChecker.unsetSolverTimeout();
        }

        template<>
        void
        analyzeDFTSMT(storm::storage::DFT<storm::RationalFunction> const &dft, bool printOutput) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                            "Analysis by SMT not supported for this data type.");
        }

        template<>
        std::pair<std::shared_ptr<storm::gspn::GSPN>, uint64_t> transformToGSPN(storm::storage::DFT<double> const& dft) {
            storm::settings::modules::FaultTreeSettings const& ftSettings = storm::settings::getModule<storm::settings::modules::FaultTreeSettings>();
            storm::settings::modules::DftGspnSettings const& dftGspnSettings = storm::settings::getModule<storm::settings::modules::DftGspnSettings>();

            // Set Don't Care elements
            std::set<uint64_t> dontCareElements;
            if (!ftSettings.isDisableDC()) {
                // Insert all elements as Don't Care elements
                for (std::size_t i = 0; i < dft.nrElements(); i++) {
                    dontCareElements.insert(dft.getElement(i)->id());
                }
            }

            // Transform to GSPN
            storm::transformations::dft::DftToGspnTransformator<double> gspnTransformator(dft);
            auto priorities = gspnTransformator.computePriorities(dftGspnSettings.isExtendPriorities());
            gspnTransformator.transform(priorities, dontCareElements, !dftGspnSettings.isDisableSmartTransformation(),
                                        dftGspnSettings.isMergeDCFailed(), dftGspnSettings.isExtendPriorities());
            std::shared_ptr<storm::gspn::GSPN> gspn(gspnTransformator.obtainGSPN());
            return std::make_pair(gspn, gspnTransformator.toplevelFailedPlaceId());
        }

        std::shared_ptr<storm::jani::Model> transformToJani(storm::gspn::GSPN const& gspn, uint64_t toplevelFailedPlace) {
            // Build Jani model
            storm::builder::JaniGSPNBuilder builder(gspn);
            std::shared_ptr<storm::jani::Model> model(builder.build("dft_gspn"));

            // Build properties
            std::shared_ptr<storm::expressions::ExpressionManager> const& exprManager = gspn.getExpressionManager();
            storm::jani::Variable const& topfailedVar = builder.getPlaceVariable(toplevelFailedPlace);
            storm::expressions::Expression targetExpression = exprManager->integer(1) == topfailedVar.getExpressionVariable().getExpression();
            // Add variable for easier access to 'failed' state
            builder.addTransientVariable(model.get(), "failed", targetExpression);
            auto failedFormula = std::make_shared<storm::logic::AtomicExpressionFormula>(targetExpression);
            auto properties = builder.getStandardProperties(model.get(), failedFormula, "Failed", "a failed state", true);

            // Export Jani to file
            storm::settings::modules::DftGspnSettings const& dftGspnSettings = storm::settings::getModule<storm::settings::modules::DftGspnSettings>();
            if (dftGspnSettings.isWriteToJaniSet()) {
                auto const& jani = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();
                storm::api::exportJaniToFile(*model, properties, dftGspnSettings.getWriteToJaniFilename(), jani.isCompactJsonSet());
            }

            return model;
        }

        template<>
        std::pair<std::shared_ptr<storm::gspn::GSPN>, uint64_t> transformToGSPN(storm::storage::DFT<storm::RationalFunction> const& dft) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Transformation to GSPN not supported for this data type.");
        }

    }
}
