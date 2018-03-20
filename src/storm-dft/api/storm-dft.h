#pragma once

#include <type_traits>

#include "storm-dft/parser/DFTGalileoParser.h"
#include "storm-dft/parser/DFTJsonParser.h"
#include "storm-dft/storage/dft/DftJsonExporter.h"

#include "storm-dft/modelchecker/dft/DFTModelChecker.h"
#include "storm-dft/modelchecker/dft/DFTASFChecker.h"

#include "storm-dft/transformations/DftToGspnTransformator.h"
#include "storm-gspn/api/storm-gspn.h"

namespace storm {
    namespace api {

        /*!
         * Load DFT from Galileo file.
         *
         * @param file File containing DFT description in Galileo format.
         *
         * @return DFT.
         */
        template<typename ValueType>
        std::shared_ptr<storm::storage::DFT<ValueType>> loadDFTGalileo(std::string const& file) {
             return std::make_shared<storm::storage::DFT<ValueType>>(storm::parser::DFTGalileoParser<ValueType>::parseDFT(file));
        }

        /*!
         * Load DFT from JSON file.
         *
         * @param file File containing DFT description in JSON format.
         *
         * @return DFT.
         */
        template<typename ValueType>
        std::shared_ptr<storm::storage::DFT<ValueType>> loadDFTJson(std::string const& file) {
                storm::parser::DFTJsonParser<ValueType> parser;
                return std::make_shared<storm::storage::DFT<ValueType>>(parser.parseJson(file));
        }

        /*!
         * Analyse the given DFT according to the given properties.
         * First the Markov model is built from the DFT and then this model is checked against the given properties.
         *
         * @param dft DFT.
         * @param properties PCTL formulas capturing the properties to check.
         * @param symred Flag whether symmetry reduction should be used.
         * @param allowModularisation Flag whether modularisation should be applied if possible.
         * @param enableDC Flag whether Don't Care propagation should be used.
         *
         * @return Result.
         */
        template <typename ValueType>
        typename storm::modelchecker::DFTModelChecker<ValueType>::dft_results analyzeDFT(storm::storage::DFT<ValueType> const& dft, std::vector<std::shared_ptr<storm::logic::Formula const>> const& properties, bool symred, bool allowModularisation, bool enableDC) {
            storm::modelchecker::DFTModelChecker<ValueType> modelChecker;
            typename storm::modelchecker::DFTModelChecker<ValueType>::dft_results results = modelChecker.check(dft, properties, symred, allowModularisation, enableDC, 0.0);
            modelChecker.printTimings();
            modelChecker.printResults();
            return results;
        }

        /*!
         * Approximate the analysis result of the given DFT according to the given properties.
         * First the Markov model is built from the DFT and then this model is checked against the given properties.
         *
         * @param dft DFT.
         * @param properties PCTL formulas capturing the properties to check.
         * @param symred Flag whether symmetry reduction should be used.
         * @param allowModularisation Flag whether modularisation should be applied if possible.
         * @param enableDC Flag whether Don't Care propagation should be used.
         * @param approximationError Allowed approximation error.
         *
         * @return Result.
         */
        template <typename ValueType>
        typename storm::modelchecker::DFTModelChecker<ValueType>::dft_results analyzeDFTApprox(storm::storage::DFT<ValueType> const& dft, std::vector<std::shared_ptr<storm::logic::Formula const>> const& properties, bool symred, bool allowModularisation, bool enableDC, double approximationError) {
            storm::modelchecker::DFTModelChecker<ValueType> modelChecker;
            typename storm::modelchecker::DFTModelChecker<ValueType>::dft_results results = modelChecker.check(dft, properties, symred, allowModularisation, enableDC, approximationError);
            modelChecker.printTimings();
            modelChecker.printResults();
            return results;
        }


        /*!
         * Export DFT to JSON file.
         *
         * @param dft DFT.
         * @param file File.
         */
        template<typename ValueType>
        typename std::enable_if<std::is_same<ValueType, double>::value, void>::type exportDFTToJson(storm::storage::DFT<ValueType> const& dft, std::string const& file) {
            storm::storage::DftJsonExporter<ValueType>::toFile(dft, file);
        }

        /*!
         * Export DFT to JSON file.
         *
         * @param dft DFT.
         * @param file File.
         */
        template<typename ValueType>
        typename std::enable_if<!std::is_same<ValueType, double>::value, void>::type exportDFTToJson(storm::storage::DFT<ValueType> const& dft, std::string const& file) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Export to JSON not supported for this data type.");
        }

        /*!
         * Export DFT to SMT encoding.
         *
         * @param dft DFT.
         * @param file File.
         */
        template<typename ValueType>
        typename std::enable_if<std::is_same<ValueType, double>::value, void>::type exportDFTToSMT(storm::storage::DFT<ValueType> const& dft, std::string const& file) {
            storm::modelchecker::DFTASFChecker asfChecker(dft);
            asfChecker.convert();
            asfChecker.toFile(file);
        }

        /*!
         * Export DFT to SMT encoding.
         *
         * @param dft DFT.
         * @param file File.
         */
        template<typename ValueType>
        typename std::enable_if<!std::is_same<ValueType, double>::value, void>::type exportDFTToSMT(storm::storage::DFT<ValueType> const& dft, std::string const& file) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Export to SMT does not support this data type.");
        }

        /*!
         * Transform DFT to GSPN.
         *
         * @param dft DFT.
         */
        template<typename ValueType>
        typename std::enable_if<std::is_same<ValueType, double>::value, void>::type transformToGSPN(storm::storage::DFT<ValueType> const& dft) {
            // Transform to GSPN
            storm::transformations::dft::DftToGspnTransformator<double> gspnTransformator(dft);
            bool smart = true;
            gspnTransformator.transform(smart);
            storm::gspn::GSPN* gspn = gspnTransformator.obtainGSPN();
            uint64_t toplevelFailedPlace = gspnTransformator.toplevelFailedPlaceId();

            storm::api::handleGSPNExportSettings(*gspn);

            std::shared_ptr<storm::expressions::ExpressionManager> const& exprManager = gspn->getExpressionManager();
            storm::builder::JaniGSPNBuilder builder(*gspn);
            storm::jani::Model* model =  builder.build();
            storm::jani::Variable const& topfailedVar = builder.getPlaceVariable(toplevelFailedPlace);

            storm::expressions::Expression targetExpression = exprManager->integer(1) == topfailedVar.getExpressionVariable().getExpression();
            auto evtlFormula = std::make_shared<storm::logic::AtomicExpressionFormula>(targetExpression);
            auto tbFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), evtlFormula, storm::logic::TimeBound(false, exprManager->integer(0)), storm::logic::TimeBound(false, exprManager->integer(10)), storm::logic::TimeBoundReference(storm::logic::TimeBoundType::Time));
            auto tbUntil = std::make_shared<storm::logic::ProbabilityOperatorFormula>(tbFormula);

            auto evFormula = std::make_shared<storm::logic::EventuallyFormula>(evtlFormula, storm::logic::FormulaContext::Time);
            auto rewFormula = std::make_shared<storm::logic::TimeOperatorFormula>(evFormula, storm::logic::OperatorInformation(), storm::logic::RewardMeasureType::Expectation);

            storm::settings::modules::JaniExportSettings const& janiSettings = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();
            if (janiSettings.isJaniFileSet()) {
                storm::api::exportJaniModel(*model, {storm::jani::Property("time-bounded", tbUntil), storm::jani::Property("mttf", rewFormula)}, janiSettings.getJaniFilename());
            }

            delete model;
            delete gspn;
        }

        /*!
         * Transform DFT to GSPN.
         *
         * @param dft DFT.
         */
        template<typename ValueType>
        typename std::enable_if<!std::is_same<ValueType, double>::value, void>::type transformToGSPN(storm::storage::DFT<ValueType> const& dft) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Transformation to GSPN not supported for this data type.");
        }

    }
}
