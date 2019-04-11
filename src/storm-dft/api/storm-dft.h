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
        std::shared_ptr<storm::storage::DFT<ValueType>> loadDFTGalileoFile(std::string const& file) {
             return std::make_shared<storm::storage::DFT<ValueType>>(storm::parser::DFTGalileoParser<ValueType>::parseDFT(file));
        }

        /*!
         * Load DFT from JSON string.
         *
         * @param jsonString String containing DFT description in JSON format.
         *
         * @return DFT.
         */
        template<typename ValueType>
        std::shared_ptr<storm::storage::DFT<ValueType>> loadDFTJsonString(std::string const& jsonString) {
                storm::parser::DFTJsonParser<ValueType> parser;
                return std::make_shared<storm::storage::DFT<ValueType>>(parser.parseJsonFromString(jsonString));
        }

        /*!
         * Load DFT from JSON file.
         *
         * @param file File containing DFT description in JSON format.
         *
         * @return DFT.
         */
        template<typename ValueType>
        std::shared_ptr<storm::storage::DFT<ValueType>> loadDFTJsonFile(std::string const& file) {
                storm::parser::DFTJsonParser<ValueType> parser;
                return std::make_shared<storm::storage::DFT<ValueType>>(parser.parseJsonFromFile(file));
        }

        template<typename ValueType>
        bool isWellFormed(storm::storage::DFT<ValueType> const& dft) {
            std::stringstream stream;
            return dft.checkWellFormedness(stream);
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
        template<typename ValueType>
        typename storm::modelchecker::DFTModelChecker<ValueType>::dft_results
        analyzeDFT(storm::storage::DFT<ValueType> const& dft, std::vector<std::shared_ptr<storm::logic::Formula const>> const& properties, bool symred, bool allowModularisation,
                   bool enableDC, bool printOutput) {
            storm::modelchecker::DFTModelChecker<ValueType> modelChecker(printOutput);
            typename storm::modelchecker::DFTModelChecker<ValueType>::dft_results results = modelChecker.check(dft, properties, symred, allowModularisation, enableDC, 0.0);
            if (printOutput) {
                modelChecker.printTimings();
                modelChecker.printResults();
            }
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
        template<typename ValueType>
        typename storm::modelchecker::DFTModelChecker<ValueType>::dft_results
        analyzeDFTApprox(storm::storage::DFT<ValueType> const& dft, std::vector<std::shared_ptr<storm::logic::Formula const>> const& properties, bool symred,
                         bool allowModularisation, bool enableDC, double approximationError, bool printOutput) {
            storm::modelchecker::DFTModelChecker<ValueType> modelChecker(printOutput);
            typename storm::modelchecker::DFTModelChecker<ValueType>::dft_results results = modelChecker.check(dft, properties, symred, allowModularisation, enableDC,
                                                                                                               approximationError);
            if (printOutput) {
                modelChecker.printTimings();
                modelChecker.printResults();
            }
            return results;
        }

        /*!
         * Analyze the DFT using the SMT encoding
         *
         * @param dft DFT.
         *
         * @return Result result vector
         */
        template<typename ValueType>
        std::vector<storm::solver::SmtSolver::CheckResult>
        analyzeDFTSMT(storm::storage::DFT<ValueType> const &dft, bool printOutput);

        /*!
         * Export DFT to JSON file.
         *
         * @param dft DFT.
         * @param file File.
         */
        template<typename ValueType>
        void exportDFTToJsonFile(storm::storage::DFT<ValueType> const& dft, std::string const& file);

        /*!
         * Export DFT to JSON string.
         *
         * @param dft DFT.
         */
        template<typename ValueType>
        std::string exportDFTToJsonString(storm::storage::DFT<ValueType> const& dft);

        /*!
         * Export DFT to SMT encoding.
         *
         * @param dft DFT.
         * @param file File.
         */
        template<typename ValueType>
        void exportDFTToSMT(storm::storage::DFT<ValueType> const& dft, std::string const& file);

        /*!
         * Transform DFT to GSPN.
         *
         * @param dft DFT.
         *
         * @return Pair of GSPN and id of failed place corresponding to the top level element.
         */
        template<typename ValueType>
        std::pair<std::shared_ptr<storm::gspn::GSPN>, uint64_t> transformToGSPN(storm::storage::DFT<ValueType> const& dft);

        /*!
         * Transform GSPN to Jani model.
         *
         * @param gspn GSPN.
         * @param toplevelFailedPlace Id of the failed place in the GSPN for the top level element in the DFT.
         *
         * @return JANI model.
         */
        std::shared_ptr<storm::jani::Model> transformToJani(storm::gspn::GSPN const& gspn, uint64_t toplevelFailedPlace);

    }
}
