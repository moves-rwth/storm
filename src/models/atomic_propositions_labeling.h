/*
 * labeling.h
 *
 *  Created on: 10.09.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_MODELS_ATOMIC_PROPOSITIONS_LABELING_H_
#define MRMC_MODELS_ATOMIC_PROPOSITIONS_LABELING_H_

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

#include <ostream>

#include "single_atomic_proposition_labeling.h"

#define USE_STD_UNORDERED_MAP

/* Map types: By default, the boost hash map is used.
 * If the macro USE_STD_MAP is defined, the default C++ class (std::map)
 * is used instead.
 */
#ifdef USE_STD_MAP
#  include <map>
#  define MAP std::map
#else
#  ifdef USE_STD_UNORDERED_MAP
#     include <unordered_map>
#     define MAP std::unordered_map
#  else
#     include "boost/unordered_map.hpp"
#     define MAP boost::unordered_map
#  endif
#endif

#include <stdexcept>

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

namespace mrmc {

namespace models {

/*!
 * This class manages the labeling of the state space with a fixed number of
 * atomic propositions.
 */
class AtomicPropositionsLabeling {

public:

	//! Constructor
	/*!
	 * Constructs an empty atomic propositions labeling for the given number
	 * of states and amount of atomic propositions.
	 * @param state_count The number of states of the model.
	 * @param ap_count The number of atomic propositions.
	 */
	AtomicPropositionsLabeling(const uint_fast32_t state_count, const uint_fast32_t ap_count)
			: state_count(state_count), ap_count(ap_count), aps_current(0) {
		this->single_labelings = new SingleAtomicPropositionLabeling*[ap_count];
		for (uint_fast32_t i = 0; i < ap_count; ++i) {
			this->single_labelings[i] = new SingleAtomicPropositionLabeling(state_count);
		}
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given atomic proposition
	 * labeling.
	 */
	AtomicPropositionsLabeling(const AtomicPropositionsLabeling& atomic_propositions_labeling)
			: state_count(atomic_propositions_labeling.state_count),
			  ap_count(atomic_propositions_labeling.ap_count),
			  aps_current(atomic_propositions_labeling.aps_current),
			  name_to_labeling_map(atomic_propositions_labeling.name_to_labeling_map) {
		this->single_labelings = new SingleAtomicPropositionLabeling*[ap_count];
		for (uint_fast32_t i = 0; i < ap_count; ++i) {
			this->single_labelings[i] = new SingleAtomicPropositionLabeling(*atomic_propositions_labeling.single_labelings[i]);
		}
	}

	//! Destructor
	/*
	 * Destructor. Needs to take care of deleting all single labelings.
	 */
	virtual ~AtomicPropositionsLabeling() {
		// delete all the single atomic proposition labelings in the map
		for (uint_fast32_t i = 0; i < ap_count; ++i) {
			delete this->single_labelings[i];
			this->single_labelings[i] = NULL;
		}
		delete[] this->single_labelings;
		this->single_labelings = NULL;
	}

	/*!
	 * Registers the name of a proposition.
	 * Will throw an error if more atomic propositions are added than were
	 * originally declared or if an atomic proposition is registered twice.
	 * @param ap The name of the atomic proposition to add.
	 * @return The index of the new proposition.
	 */
	uint_fast32_t addAtomicProposition(std::string ap) {
		if (name_to_labeling_map.count(ap) != 0) {
			throw std::out_of_range("Atomic Proposition already exists.");
		}
		if (aps_current >= ap_count) {
			throw std::out_of_range("Added more atomic propositions than"
					"previously declared.");
		}
		name_to_labeling_map[ap] = aps_current;

		uint_fast32_t returnValue = aps_current++;
		return returnValue;
	}

	/*!
	 * Checks whether the name of an atomic proposition is already registered
	 * within this labeling.
	 * @return True if the proposition was added to the labeling, false otherwise.
	 */
	bool containsAtomicProposition(std::string proposition) {
		return (name_to_labeling_map.count(proposition) != 0);
	}

	/*!
	 * Adds an atomic proposition to a given state.
	 * @param ap The name of the atomic proposition.
	 * @param state The index of the state to label.
	 */
	void addAtomicPropositionToState(std::string ap, const uint_fast32_t state) {
		if (name_to_labeling_map.count(ap) == 0) {
			throw std::out_of_range("Atomic Proposition '" + ap + "' unknown.");
		}
		if (state >= state_count) {
			throw std::out_of_range("State index out of range.");
		}
		this->single_labelings[name_to_labeling_map[ap]]->addLabelToState(state);
	}

	/*!
	 * Checks whether a given state is labeled with the given atomic proposition.
	 * @param ap The name of the atomic proposition.
	 * @param state The index of the state to check.
	 * @return True if the node is labeled with the atomic proposition, false
	 * otherwise.
	 */
	bool stateHasAtomicProposition(std::string ap,
			const uint_fast32_t state) {
		return this->single_labelings[name_to_labeling_map[ap]]->hasLabel(state);
	}

	/*!
	 * Returns the number of atomic propositions managed by this object (set in
	 * the initialization).
	 * @return The number of atomic propositions.
	 */
	uint_fast32_t getNumberOfAtomicPropositions() {
		return ap_count;
	}

	/*!
	 * Returns the labeling of states associated with the given atomic proposition.
	 * @param ap The name of the atomic proposition.
	 * @return A pointer to an instance of SingleAtomicPropositionLabeling that
	 * represents the labeling of the states with the given atomic proposition.
	 */
	SingleAtomicPropositionLabeling* getAtomicProposition(std::string ap) {
		return (this->single_labelings[name_to_labeling_map[ap]]);
	}

   /*!
    * Returns the size of the labeling in memory measured in bytes.
    * @return The size of the labeling in memory measured in bytes.
    */
	uint_fast64_t getSizeInMemory() {
		uint_fast64_t size = sizeof(*this);
		// add sizes of all single  labelings
		for (uint_fast32_t i = 0; i < ap_count; i++) {
			size += this->single_labelings[i]->getSizeInMemory();
		}
		return size;
	}

	/*!
	 * Prints information about the labeling to the specified stream.
	 * @param out The stream the information is to be printed to.
	 */
	void printAtomicPropositionsInformationToStream(std::ostream& out) {
		out << "Atomic Propositions: \t" << this->getNumberOfAtomicPropositions()
			<< std::endl;
		for(auto ap : this->name_to_labeling_map) {
			out << "\t * " << ap.first << " -> "
				<< this->single_labelings[ap.second]->getNumberOfLabeledStates();
			out << " state(s)" << std::endl;
		}
	}

private:

	/*! The number of states whose labels are to be stored by this object. */
	uint_fast32_t state_count;

	/*! The number of different atomic propositions this object can store. */
	uint_fast32_t ap_count;

	/*!
	 * The number of different atomic propositions currently associated with
	 * this labeling. Used to prevent too many atomic propositions from being
	 * added to this object.
	 */
	uint_fast32_t aps_current;

	/*!
	 * Associates a name of an atomic proposition to its corresponding labeling
	 * by mapping the name to a specific index in the array of all
	 * individual labelings.
	 */
	MAP<std::string, uint_fast32_t> name_to_labeling_map;

	/*!
	 * Stores all individual labelings. To find the labeling associated with
	 * a particular atomic proposition, the map from names to indices in this
	 * array has to be used.
	 */
	SingleAtomicPropositionLabeling** single_labelings;
};

} // namespace models

} // namespace mrmc

#endif /* MRMC_MODELS_ATOMIC_PROPOSITIONS_LABELING_H_ */
