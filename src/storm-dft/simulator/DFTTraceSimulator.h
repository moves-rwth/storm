#include "storm-dft/generator/DftNextStateGenerator.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/storage/dft/DFTState.h"
#include "storm-dft/storage/dft/FailableElements.h"

#include "storm/utility/random.h"


namespace storm {
    namespace dft {
        namespace simulator {

            /*!
             * Simulator for DFTs.
             * A step in the simulation corresponds to the failure of one BE (either on its own or triggered by a dependency)
             * and the failure propagation through the DFT.
             * The simulator also allows to randomly generate a next failure according to the failure rates.
             */
            template<typename ValueType>
            class DFTTraceSimulator {
                using DFTStatePointer = std::shared_ptr<storm::storage::DFTState<ValueType>>;
            public:
                /*!
                 * Constructor.
                 * 
                 * @param dft DFT.
                 * @param stateGenerationInfo Info for state generation.
                 * @param randomGenerator Random number generator.
                 */
                DFTTraceSimulator(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTStateGenerationInfo const& stateGenerationInfo, boost::mt19937& randomGenerator);

                /*!
                 * Set the random number generator.
                 * 
                 * @param randomNumberGenerator Random number generator.
                 */
                void setRandomNumberGenerator(boost::mt19937& randomNumberGenerator);

                /*!
                 * Set the current state back to the intial state in order to start a new simulation.
                 */
                void resetToInitial();

                /*!
                 * Get the current DFT state.
                 * 
                 * @return DFTStatePointer DFT state.
                 */
                DFTStatePointer getCurrentState() const;

                /*!
                 * Perform one simulation step by letting the next element fail.
                 * 
                 * @param nextFailElement Iterator giving the next element which should fail.
                 * @return True iff step could be performed successfully.
                 */
                bool step(storm::dft::storage::FailableElements::const_iterator nextFailElement);

                /*!
                 * Perform a random step by using the random number generator.
                 * 
                 * @return double The time which progessed between the last step and this step.
                 *      Returns -1 if no next step could be created.
                 */
                double randomStep();

                /*!
                 * Perform a complete simulation of a failure trace by using the random number generator.
                 * The simulation starts in the initial state and tries to reach a state where the top-level event of the DFT has failed.
                 * If this target state can be reached within the given timebound, the simulation was successful.
                 * 
                 * @param timebound Time bound in which the system failure should occur.
                 * @return True iff a system failure occurred for the generated trace within the time bound.
                 */
                bool simulateCompleteTrace(double timebound);

            protected:

                // The DFT used for the generation of next states.
                storm::storage::DFT<ValueType> const& dft;

                // General information for the state generation.
                storm::storage::DFTStateGenerationInfo const& stateGenerationInfo;

                // Generator for creating next state in DFT
                storm::generator::DftNextStateGenerator<ValueType> generator;

                // Current state
                DFTStatePointer state;

                // Random number generator
                boost::mt19937& randomGenerator;
            };
        }
    }
}

