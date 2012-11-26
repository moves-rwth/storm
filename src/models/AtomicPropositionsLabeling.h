/*
 * labeling.h
 *
 *  Created on: 10.09.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_MODELS_ATOMIC_PROPOSITIONS_LABELING_H_
#define MRMC_MODELS_ATOMIC_PROPOSITIONS_LABELING_H_

#include "src/storage/BitVector.h"
#include <ostream>

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
	 * @param stateCount The number of states of the model.
	 * @param apCountMax The number of atomic propositions.
	 */
	AtomicPropositionsLabeling(const uint_fast32_t stateCount, const uint_fast32_t apCountMax)
			: stateCount(stateCount), apCountMax(apCountMax), apsCurrent(0) {
		this->singleLabelings = new mrmc::storage::BitVector*[apCountMax];
		for (uint_fast32_t i = 0; i < apCountMax; ++i) {
			this->singleLabelings[i] = new mrmc::storage::BitVector(stateCount);
		}
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given atomic proposition
	 * labeling.
	 * @param atomicPropositionsLabeling The atomic propositions labeling to copy.
	 */
	AtomicPropositionsLabeling(const AtomicPropositionsLabeling& atomicPropositionsLabeling)
			: stateCount(atomicPropositionsLabeling.stateCount),
			  apCountMax(atomicPropositionsLabeling.apCountMax),
			  apsCurrent(atomicPropositionsLabeling.apsCurrent),
			  nameToLabelingMap(atomicPropositionsLabeling.nameToLabelingMap) {
		this->singleLabelings = new mrmc::storage::BitVector*[apCountMax];
		for (uint_fast32_t i = 0; i < apCountMax; ++i) {
			this->singleLabelings[i] = new mrmc::storage::BitVector(*atomicPropositionsLabeling.singleLabelings[i]);
		}
	}

	//! Destructor
	/*
	 * Destructor. Needs to take care of deleting all single labelings.
	 */
	virtual ~AtomicPropositionsLabeling() {
		// delete all the single atomic proposition labelings in the map
		for (uint_fast32_t i = 0; i < apCountMax; ++i) {
			delete this->singleLabelings[i];
			this->singleLabelings[i] = NULL;
		}
		delete[] this->singleLabelings;
		this->singleLabelings = NULL;
	}

	/*!
	 * Registers the name of a proposition.
	 * Will throw an error if more atomic propositions are added than were
	 * originally declared or if an atomic proposition is registered twice.
	 * @param ap The name of the atomic proposition to add.
	 * @return The index of the new proposition.
	 */
	uint_fast32_t addAtomicProposition(std::string ap) {
		if (nameToLabelingMap.count(ap) != 0) {
			throw std::out_of_range("Atomic Proposition already exists.");
		}
		if (apsCurrent >= apCountMax) {
			throw std::out_of_range("Added more atomic propositions than"
					"previously declared.");
		}
		nameToLabelingMap[ap] = apsCurrent;

		uint_fast32_t returnValue = apsCurrent++;
		return returnValue;
	}

	/*!
	 * Checks whether the name of an atomic proposition is already registered
	 * within this labeling.
	 * @return True if the proposition was added to the labeling, false otherwise.
	 */
	bool containsAtomicProposition(std::string proposition) {
		return (nameToLabelingMap.count(proposition) != 0);
	}

	/*!
	 * Adds an atomic proposition to a given state.
	 * @param ap The name of the atomic proposition.
	 * @param state The index of the state to label.
	 */
	void addAtomicPropositionToState(std::string ap, const uint_fast32_t state) {
		if (nameToLabelingMap.count(ap) == 0) {
			throw std::out_of_range("Atomic Proposition '" + ap + "' unknown.");
		}
		if (state >= stateCount) {
			throw std::out_of_range("State index out of range.");
		}
		this->singleLabelings[nameToLabelingMap[ap]]->set(state, true);
	}

	/*!
	 * Checks whether a given state is labeled with the given atomic proposition.
	 * @param ap The name of the atomic proposition.
	 * @param state The index of the state to check.
	 * @return True if the node is labeled with the atomic proposition, false
	 * otherwise.
	 */
	bool stateHasAtomicProposition(std::string ap, const uint_fast32_t state) {
		return this->singleLabelings[nameToLabelingMap[ap]]->get(state);
	}

	/*!
	 * Returns the number of atomic propositions managed by this object (set in
	 * the initialization).
	 * @return The number of atomic propositions.
	 */
	uint_fast32_t getNumberOfAtomicPropositions() {
		return apCountMax;
	}

	/*!
	 * Returns the labeling of states associated with the given atomic proposition.
	 * @param ap The name of the atomic proposition.
	 * @return A pointer to an instance of SingleAtomicPropositionLabeling that
	 * represents the labeling of the states with the given atomic proposition.
	 */
	mrmc::storage::BitVector* getAtomicProposition(std::string ap) {
		return (this->singleLabelings[nameToLabelingMap[ap]]);
	}

   /*!
    * Returns the size of the labeling in memory measured in bytes.
    * @return The size of the labeling in memory measured in bytes.
    */
	uint_fast64_t getSizeInMemory() {
		uint_fast64_t size = sizeof(*this);
		// add sizes of all single  labelings
		for (uint_fast32_t i = 0; i < apCountMax; i++) {
			size += this->singleLabelings[i]->getSizeInMemory();
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
		for(auto ap : this->nameToLabelingMap) {
			out << "\t * " << ap.first << " -> "
				<< this->singleLabelings[ap.second]->getNumberOfSetBits();
			out << " state(s)" << std::endl;
		}
		for(unsigned int i = 0; i < apCountMax; ++i) {
			std::cout << "iterator " << i << std::endl;
			for(auto it = this->singleLabelings[i]->begin(); it != this->singleLabelings[i]->end(); ++it) {
				out << "next value: " << *it << std::endl;
			}
		}
	}

private:

	/*! The number of states whose labels are to be stored by this object. */
	uint_fast32_t stateCount;

	/*! The number of different atomic propositions this object can store. */
	uint_fast32_t apCountMax;

	/*!
	 * The number of different atomic propositions currently associated with
	 * this labeling. Used to prevent too many atomic propositions from being
	 * added to this object.
	 */
	uint_fast32_t apsCurrent;

	/*!
	 * Associates a name of an atomic proposition to its corresponding labeling
	 * by mapping the name to a specific index in the array of all
	 * individual labelings.
	 */
	MAP<std::string, uint_fast32_t> nameToLabelingMap;

	/*!
	 * Stores all individual labelings. To find the labeling associated with
	 * a particular atomic proposition, the map from names to indices in this
	 * array has to be used.
	 */
	mrmc::storage::BitVector** singleLabelings;
};

} // namespace models

} // namespace mrmc

#endif /* MRMC_MODELS_ATOMIC_PROPOSITIONS_LABELING_H_ */
