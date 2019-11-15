#pragma once

#include <type_traits>
#include <utility>

#include "storm-dft/parser/DFTGalileoParser.h"
#include "storm-dft/parser/DFTJsonParser.h"
#include "storm-dft/storage/dft/DftJsonExporter.h"
#include "storm-dft/modelchecker/dft/DFTModelChecker.h"
#include "storm-dft/modelchecker/dft/DFTASFChecker.h"
#include "storm-dft/transformations/DftToGspnTransformator.h"
#include "storm-dft/transformations/DftTransformator.h"
#include "storm-dft/utility/FDEPConflictFinder.h"
#include "storm-dft/utility/FailureBoundFinder.h"

#include "storm-gspn/api/storm-gspn.h"

namespace storm {
    namespace api {
        struct PreprocessingResult {
            uint64_t lowerBEBound;
            uint64_t upperBEBound;
            std::vector<std::pair<uint64_t, uint64_t>> fdepConflicts;
        };


        /*!
         * Load DFT from Galileo file.
         *
         * @param file File containing DFT description in Galileo format.
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
         * @return DFT.
         */
        template<typename ValueType>
        std::shared_ptr<storm::storage::DFT<ValueType>> loadDFTJsonString(std::string const& jsonString) {
            return std::make_shared<storm::storage::DFT<ValueType>>(storm::parser::DFTJsonParser<ValueType>::parseJsonFromString(jsonString));
        }

        /*!
         * Load DFT from JSON file.
         *
         * @param file File containing DFT description in JSON format.
         * @return DFT.
         */
        template<typename ValueType>
        std::shared_ptr<storm::storage::DFT<ValueType>> loadDFTJsonFile(std::string const& file) {
            return std::make_shared<storm::storage::DFT<ValueType>>(storm::parser::DFTJsonParser<ValueType>::parseJsonFromFile(file));
        }

        /*!
         * Check whether the DFT is well-formed.
         *
         * @param dft DFT.
         * @param validForAnalysis  If true, additional (more restrictive) checks are performed to check whether the DFT is valid for analysis.
         * @return Pair where the first entry is true iff the DFT is well-formed. The second entry contains the error messages for illformed parts.
         */
        template<typename ValueType>
        std::pair<bool, std::string> isWellFormed(storm::storage::DFT<ValueType> const& dft, bool validForAnalysis = true) {
            std::stringstream stream;
            bool wellFormed = dft.checkWellFormedness(validForAnalysis, stream);
            return std::pair<bool, std::string>(wellFormed, stream.str());
        }

        /*!
         * Apply transformations for DFT.
         *
         * @param dft DFT.
         * @param uniqueBE Flag whether a unique constant failed BE is created.
         * @param binaryFDEP Flag whether all dependencies should be binary (only one dependent child).
         * @return Transformed DFT.
         */
        template<typename ValueType>
        std::shared_ptr<storm::storage::DFT<ValueType>> applyTransformations(storm::storage::DFT<ValueType> const& dft, bool uniqueBE, bool binaryFDEP) {
            std::shared_ptr<storm::storage::DFT<ValueType>> transformedDFT = std::make_shared<storm::storage::DFT<ValueType>>(dft);
            auto dftTransformator = storm::transformations::dft::DftTransformator<ValueType>();
            if (uniqueBE) {
                transformedDFT = dftTransformator.transformUniqueFailedBe(*transformedDFT);
            }
            if (binaryFDEP && !dft.getDependencies().empty()) {
                transformedDFT = dftTransformator.transformBinaryFDEPs(*transformedDFT);
            }
            return transformedDFT;
        }

        template <typename ValueType>
        bool computeDependencyConflicts(storm::storage::DFT<ValueType> const& dft, bool useSMT, double solverTimeout) {
            std::vector<std::pair<uint64_t, uint64_t>> fdepConflicts = storm::dft::utility::FDEPConflictFinder<ValueType>::getDependencyConflicts(*dft, useSMT, solverTimeout);

            if (fdepConflicts.empty()) {
                return false;
            }
            for (auto pair: fdepConflicts) {
                STORM_LOG_DEBUG("Conflict between " << dft.getElement(pair.first)->name() << " and " << dft.getElement(pair.second)->name());
            }

            // Set the conflict map of the dft
            std::set<size_t> conflict_set;
            for (auto conflict : fdepConflicts) {
                conflict_set.insert(size_t(conflict.first));
                conflict_set.insert(size_t(conflict.second));
            }
            for (size_t depId : dft->getDependencies()) {
                if (!conflict_set.count(depId)) {
                    dft->setDependencyNotInConflict(depId);
                }
            }
            return true;
        }

        /*!
         * Compute the exact or approximate analysis result of the given DFT according to the given properties.
         * First the Markov model is built from the DFT and then this model is checked against the given properties.
         *
         * @param dft DFT.
         * @param properties PCTL formulas capturing the properties to check.
         * @param symred Flag whether symmetry reduction should be used.
         * @param allowModularisation Flag whether modularisation should be applied if possible.
         * @param relevantEvents List of relevant events which should be observed.
         * @param allowDCForRelevantEvents If true, Don't Care propagation is allowed even for relevant events.
         * @param approximationError Allowed approximation error.  Value 0 indicates no approximation.
         * @param approximationHeuristic Heuristic used for state space exploration.
         * @param eliminateChains If true, chains of non-Markovian states are eliminated from the resulting MA.
         * @param labelBehavior Behavior of labels of eliminated states
         * @param printOutput If true, model information, timings, results, etc. are printed.
         * @return Results.
         */
        template<typename ValueType>
        typename storm::modelchecker::DFTModelChecker<ValueType>::dft_results
        analyzeDFT(storm::storage::DFT<ValueType> const& dft, std::vector<std::shared_ptr<storm::logic::Formula const>> const& properties, bool symred = true,
                   bool allowModularisation = true, std::set<size_t> const& relevantEvents = {}, bool allowDCForRelevantEvents = true, double approximationError = 0.0,
                   storm::builder::ApproximationHeuristic approximationHeuristic = storm::builder::ApproximationHeuristic::DEPTH, bool eliminateChains = false,
                   storm::transformer::EliminationLabelBehavior labelBehavior = storm::transformer::EliminationLabelBehavior::KeepLabels, bool printOutput = false) {
            storm::modelchecker::DFTModelChecker<ValueType> modelChecker(printOutput);
            typename storm::modelchecker::DFTModelChecker<ValueType>::dft_results results = modelChecker.check(dft, properties, symred, allowModularisation, relevantEvents,
                                                                                                               allowDCForRelevantEvents, approximationError, approximationHeuristic,
                                                                                                               eliminateChains, labelBehavior);
            if (printOutput) {
                modelChecker.printTimings();
                modelChecker.printResults(results);
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
        void analyzeDFTSMT(storm::storage::DFT<ValueType> const& dft, bool printOutput);

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
         * @return DFT in JSON format.
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
         * @return Pair of GSPN and id of failed place corresponding to the top level element.
         */
        template<typename ValueType>
        std::pair<std::shared_ptr<storm::gspn::GSPN>, uint64_t> transformToGSPN(storm::storage::DFT<ValueType> const& dft);

        /*!
         * Transform GSPN to Jani model.
         *
         * @param gspn GSPN.
         * @param toplevelFailedPlace Id of the failed place in the GSPN for the top level element in the DFT.
         * @return JANI model.
         */
        std::shared_ptr<storm::jani::Model> transformToJani(storm::gspn::GSPN const& gspn, uint64_t toplevelFailedPlace);

    }
}
