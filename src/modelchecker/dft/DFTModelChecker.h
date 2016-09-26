#ifndef STORM_MODELCHECKER_DFT_DFTMODELCHECKER_H_

#include "src/logic/Formula.h"
#include "src/modelchecker/results/CheckResult.h"
#include "src/storage/dft/DFT.h"
#include "src/utility/storm.h"

#include <chrono>


namespace storm {
    namespace modelchecker {

        /**
         * Analyser for DFTs.
         */
        template<typename ValueType>
        class DFTModelChecker {

        public:

            /**
             * Constructor.
             */
            DFTModelChecker();

            /**
             * Main method for checking DFTs.
             *
             * @param origDft             Original DFT
             * @param formula             Formula to check for
             * @param symred              Flag indicating if symmetry reduction should be used
             * @param allowModularisation Flag indication if modularisation is allowed
             * @param enableDC            Flag indicating if dont care propagation should be used
             */
            void check(storm::storage::DFT<ValueType> const& origDft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred = true, bool allowModularisation = true, bool enableDC = true);

            /**
             * Print timings of all operations to stream.
             *
             * @param os Output stream to write to.
             */
            void printTimings(std::ostream& os = std::cout);

            /**
             * Print result to stream.
             *
             * @param os Output stream to write to.
             */
            void printResult(std::ostream& os = std::cout);

        private:
            // Timing values
            std::chrono::duration<double> buildingTime = std::chrono::duration<double>::zero();
            std::chrono::duration<double> explorationTime = std::chrono::duration<double>::zero();
            std::chrono::duration<double> bisimulationTime = std::chrono::duration<double>::zero();
            std::chrono::duration<double> modelCheckingTime = std::chrono::duration<double>::zero();
            std::chrono::duration<double> totalTime = std::chrono::duration<double>::zero();
            // Model checking result
            ValueType checkResult = storm::utility::zero<ValueType>();

            /**
             * Internal helper for model checking a DFT.
             *
             * @param dft                 DFT
             * @param formula             Formula to check for
             * @param symred              Flag indicating if symmetry reduction should be used
             * @param allowModularisation Flag indication if modularisation is allowed
             * @param enableDC            Flag indicating if dont care propagation should be used
             *
             * @return Model checking result
             */
            ValueType checkHelper(storm::storage::DFT<ValueType> const& dft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool allowModularisation, bool enableDC);

            /**
             * Check the Dfts via model checking.
             *
             * @param dfts               Vector of Dfts
             * @param formula            Formula to check for
             * @param symred             Flag indicating if symmetry reduction should be used
             * @param enableDC           Flag indicating if dont care propagation should be used
             * @param approximationError Error allowed for approximation. Value 0 indicates no approximation
             *
             * @return Vector of results for each model
             */
            std::vector<ValueType> checkModel(std::vector<storm::storage::DFT<ValueType>> const& dfts, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool enableDC, double approximationError = 0.0);

            /**
             * Build markov models from DFTs.
             *
             * @param dfts               Vector of Dfts
             * @param formula            Formula to check for
             * @param symred             Flag indicating if symmetry reduction should be used
             * @param enableDC           Flag indicating if dont care propagation should be used
             * @param approximationError Error allowed for approximation. Value 0 indicates no approximation
             *
             * @return Vector of markov models corresponding to DFTs.
             */
            std::vector<std::shared_ptr<storm::models::sparse::Model<ValueType>>> buildMarkovModels(std::vector<storm::storage::DFT<ValueType>> const& dfts, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool enableDC, double approximationError = 0.0);

        };
    }
}
#endif /* STORM_MODELCHECKER_DFT_DFTMODELCHECKER_H_ */
