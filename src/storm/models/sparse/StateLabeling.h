#ifndef STORM_MODELS_SPARSE_STATELABELING_H_
#define STORM_MODELS_SPARSE_STATELABELING_H_

#include <unordered_map>
#include <set>
#include <ostream>

#include "storm/storage/sparse/StateType.h"

#include "storm/storage/BitVector.h"
#include "storm/utility/macros.h"
#include "storm/utility/OsDetection.h"

            
namespace storm {
    namespace models {
        namespace sparse {

            /*!
             * This class manages the labeling of the state space with a number of (atomic) labels.
             */
            class StateLabeling {
            public:
                /*!
                 * Constructs an empty labeling for the given number of states.
                 *
                 * @param stateCount The number of states for which this labeling can hold the labels.
                 */
                StateLabeling(uint_fast64_t stateCount = 0);
                
                StateLabeling(StateLabeling const& other) = default;
                StateLabeling& operator=(StateLabeling const& other) = default;
                
#ifndef WINDOWS
                StateLabeling(StateLabeling&& StateLabeling) = default;
                StateLabeling& operator=(StateLabeling&& other) = default;
#endif
                
                /*!
                 * Checks whether the two labelings are equal.
                 *
                 * @param other The labeling with which the current one is compared.
                 * @return True iff the labelings are equal.
                 */
                bool operator==(StateLabeling const& other) const;
                
                /*!
                 * Retrieves the sub labeling that represents the same labeling as the current one for all selected states.
                 *
                 * @param states The selected set of states.
                 */
                StateLabeling getSubLabeling(storm::storage::BitVector const& states) const;
                
                /*!
                 * Adds a new label to the labelings. Initially, no state is labeled with this label.
                 *
                 * @param label The name of the new label.
                 */
                void addLabel(std::string const& label);
                
                /*!
                 * Retrieves the set of labels contained in this labeling.
                 *
                 * @return The set of known labels.
                 */
                std::set<std::string> getLabels() const;
                
                /*!
                 * Retrieves the set of labels attached to the given state.
                 *
                 * @param state The state for which to retrieve the labels.
                 * @return The labels attached to the given state.
                 */
                std::set<std::string> getLabelsOfState(storm::storage::sparse::state_type state) const;
                
                /*!
                 * Creates a new label and attaches it to the given states. Note that the dimension of given labeling must
                 * match the number of states for which this state labeling is valid.
                 *
                 * @param label The new label.
                 * @param labeling A bit vector that indicates whether or not the new label is attached to a state.
                 */
                void addLabel(std::string const& label, storage::BitVector const& labeling);
                
                /*!
                 * Creates a new label and attaches it to the given states. Note that the dimension of given labeling must
                 * match the number of states for which this state labeling is valid.
                 *
                 * @param label The new label.
                 * @param labeling A bit vector that indicates whether or not the new label is attached to a state.
                 */
                void addLabel(std::string const& label, storage::BitVector&& labeling);
                
                /*!
                 * Checks whether a label is registered within this labeling.
                 *
                 * @return True if the label is known, false otherwise.
                 */
                bool containsLabel(std::string const& label) const;
                
                /*!
                 * Adds a label to a given state.
                 *
                 * @param label The name of the label to add.
                 * @param state The index of the state to label.
                 */
                void addLabelToState(std::string const& label, storm::storage::sparse::state_type state);
                
                /*!
                 * Checks whether a given state is labeled with the given label.
                 *
                 * @param label The name of the label.
                 * @param state The index of the state to check.
                 * @return True if the node is labeled with the label, false otherwise.
                 */
                bool getStateHasLabel(std::string const& label, storm::storage::sparse::state_type state) const;
                
                /*!
                 * Returns the number of labels managed by this object.
                 *
                 * @return The number of labels.
                 */
                std::size_t getNumberOfLabels() const;
                
                /*!
                 * Returns the labeling of states associated with the given label.
                 *
                 * @param label The name of the label.
                 * @return A bit vector that represents the labeling of the states with the given label.
                 */
                storm::storage::BitVector const& getStates(std::string const& label) const;
                
                /*!
                 * Sets the labeling of states associated with the given label.
                 *
                 * @param label The name of the label.
                 * @param labeling A bit vector that represents the set of states that will get this label.
                 */
                void setStates(std::string const& label, storage::BitVector const& labeling);
                
                /*!
                 * Sets the labeling of states associated with the given label.
                 *
                 * @param label The name of the label.
                 * @param labeling A bit vector that represents the set of states that will get this label.
                 */
                void setStates(std::string const& label, storage::BitVector&& labeling);
                                
                /*!
                 * Prints information about the labeling to the specified stream.
                 *
                 * @param out The stream the information is to be printed to.
                 */
                void printLabelingInformationToStream(std::ostream& out) const;

                /*!
                 * Prints the complete labeling to the specified stream.
                 *
                 * @param out The stream the information is to be printed to.
                 */
                void printCompleteLabelingInformationToStream(std::ostream& out) const;

                friend std::ostream& operator<<(std::ostream& out, StateLabeling const& labeling);

            private:
                // The number of states for which this object can hold the labeling.
                uint_fast64_t stateCount;
                
                // A mapping from labels to the index of the corresponding bit vector in the vector.
                std::unordered_map<std::string, uint_fast64_t> nameToLabelingIndexMap;
                
                // A vector that holds the labeling for all known labels.
                std::vector<storm::storage::BitVector> labelings;
            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_STATELABELING_H_ */
