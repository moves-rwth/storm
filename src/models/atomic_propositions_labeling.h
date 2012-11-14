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

#include <iostream>

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

	/*! Constructor creating an object of class labeling.
	 * @param nodeCount The number of nodes; necessary for class AtomicProposition.
	 * @param propositionCount The number of atomic propositions.
	 */
	AtomicPropositionsLabeling(const uint_fast32_t nodeCount,
			const uint_fast32_t propositionCount) {
		// add one for 1-bound indices
		node_count = nodeCount + 1;
		proposition_count = propositionCount;
		propositions_current = 0;
		propositions = new SingleAtomicPropositionLabeling*[proposition_count];
		for (uint_fast32_t i = 0; i < proposition_count; ++i) {
			propositions[i] = new SingleAtomicPropositionLabeling(node_count);
		}
	}

	//! Copy Constructor
	/*! Copy Constructor. Performs a deep copy of this Labeling object.
	 *
	 */
	AtomicPropositionsLabeling(const AtomicPropositionsLabeling& atomic_propositions_labeling) :
			node_count(atomic_propositions_labeling.node_count), proposition_count(
					atomic_propositions_labeling.proposition_count), propositions_current(
					atomic_propositions_labeling.propositions_current), proposition_map(
					atomic_propositions_labeling.proposition_map) {
		propositions = new SingleAtomicPropositionLabeling*[proposition_count];
		for (uint_fast32_t i = 0; i < proposition_count; ++i) {
			propositions[i] = new SingleAtomicPropositionLabeling(
					*atomic_propositions_labeling.propositions[i]);
		}
	}

	virtual ~AtomicPropositionsLabeling() {
		//deleting all the labeling vectors in the map.
		MAP<std::string, SingleAtomicPropositionLabeling*>::iterator it;
		for (uint_fast32_t i = 0; i < proposition_count; ++i) {
			delete propositions[i];
			propositions[i] = NULL;
		}
		delete[] propositions;
		propositions = NULL;
	}

	/*! Registers the name of a proposition.
	 * Will throw an error if more propositions are added than were initialized,
	 * or if a proposition is registered twice.
	 * @param proposition The name of the proposition to add.
	 * @return The index of the new proposition.
	 */
	uint_fast32_t addProposition(std::string proposition) {
		if (proposition_map.count(proposition) != 0) {
			throw std::out_of_range("Proposition does already exist.");
		}
		if (propositions_current >= proposition_count) {
			throw std::out_of_range("Added more propositions than initialized for");
		}
		proposition_map[proposition] = propositions_current;

		uint_fast32_t returnValue = propositions_current++;
		//pantheios::log_INFO("returning ", pantheios::integer(returnValue), " for position ");
		return returnValue;
	}

	/*! Checks whether the name of a proposition is already registered with the labeling.
	 * @return True if the proposition was added to the labeling, false otherwise.
	 */
	bool containsProposition(std::string proposition) {
		return (proposition_map.count(proposition) != 0);
	}

	/*! Labels a node with an atomic proposition.
	 * @param proposition   The name of the proposition
	 * @param node The index of the node to label
	 */
	void addLabelToNode(std::string proposition, const uint_fast32_t node) {
		//TODO (Thomas Heinemann): Differentiate exceptions?
		if (proposition_map.count(proposition) == 0) {
			throw std::out_of_range("Proposition does not exist.");
		}
		if (node >= node_count) {
			throw std::out_of_range("Node number out of range");
		}
		propositions[proposition_map[proposition]]->addLabelToNode(node);
	}

	/*! Checks whether a node is labeled with a proposition.
	 * @param proposition The name of the proposition
	 * @param node The index of the node
	 * @return True if the node is labeled with the proposition, false otherwise.
	 */
	bool nodeHasProposition(std::string proposition, const uint_fast32_t node) {
		return propositions[proposition_map[proposition]]->hasNodeLabel(node);
	}

	/*! Returns the number of propositions managed by this object (which was set in the initialization)
	 * @return The number of propositions.
	 */
	uint_fast32_t getNumberOfAtomicPropositions() {
		return proposition_count;
	}

	/*! This function provides direct access to an atomic_proposition class object
	 * by its string identifier. This object manages the nodes that are labeled with the
	 * respective atomic proposition.
	 * @param proposition The name of the proposition.
	 * @return A pointer to the atomic_proposition object of the proposition.
	 */
	SingleAtomicPropositionLabeling* getAtomicProposition(std::string proposition) {
		return (propositions[proposition_map[proposition]]);
	}

   /*!
    * Returns the size of the labeling in memory measured in bytes.
    * @return The size of the labeling in memory measured in bytes.
    */
	uint_fast64_t getSizeInMemory() {
		uint_fast64_t size = sizeof(*this);
		for (uint_fast32_t i = 0; i < proposition_count; i++) {
			size += propositions[i]->getSizeInMemory(); // count sizes of bit vectors
		}
		return size;
	}

	void printAtomicPropositionsInformation() {
		std::cout << "Atomic Propositions: \t" << this->getNumberOfAtomicPropositions() << std::endl;
		for(auto ap : this->proposition_map) {
			std::cout << "\t * " << ap.first << " -> " << propositions[ap.second]->getNumberOfLabeledStates();
			std::cout << " state(s)" << std::endl;
		}
	}

private:
	uint_fast32_t node_count, proposition_count, propositions_current;
	MAP<std::string, uint_fast32_t> proposition_map;
	SingleAtomicPropositionLabeling** propositions;
};

} //namespace models

} //namespace mrmc

#endif /* MRMC_MODELS_ATOMIC_PROPOSITIONS_LABELING_H_ */
