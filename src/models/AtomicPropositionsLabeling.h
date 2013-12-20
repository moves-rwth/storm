/*
 * AtomicPropositionsLabeling.h
 *
 *  Created on: 10.09.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELS_ATOMICPROPOSITIONSLABELING_H_
#define STORM_MODELS_ATOMICPROPOSITIONSLABELING_H_

#include "src/storage/BitVector.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include <ostream>
#include <stdexcept>
#include <set>
#include <map>

#include "src/utility/Hash.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace models {

/*!
 * This class manages the labeling of the state space with a number of
 * atomic propositions.
 */
class AtomicPropositionsLabeling {

public:   
	/*!
	 * Constructs an empty atomic propositions labeling for the given number of states and amount of atomic propositions.
     *
	 * @param stateCount The number of states of the model.
	 * @param apCountMax The number of atomic propositions.
	 */
	AtomicPropositionsLabeling(const uint_fast64_t stateCount = 0, uint_fast64_t const apCountMax = 0)
			: stateCount(stateCount), apCountMax(apCountMax), apsCurrent(0), nameToLabelingMap(), singleLabelings() {
        singleLabelings.reserve(apCountMax);
	}

	/*!
	 * Copy Constructor that performs a deep copy of the given atomic proposition labeling.
     *
	 * @param atomicPropositionsLabeling The atomic propositions labeling to copy.
	 */
	AtomicPropositionsLabeling(AtomicPropositionsLabeling const& atomicPropositionsLabeling)
			: stateCount(atomicPropositionsLabeling.stateCount),
			  apCountMax(atomicPropositionsLabeling.apCountMax),
			  apsCurrent(atomicPropositionsLabeling.apsCurrent),
			  nameToLabelingMap(atomicPropositionsLabeling.nameToLabelingMap),
              singleLabelings(atomicPropositionsLabeling.singleLabelings) {
        // Intentionally left empty.
    }
    
    /*!
     * Copy constructor that performs a deep copy of the given atomic proposition labeling while restricting to the states
     * given by the bit vector.
     *
	 * @param atomicPropositionsLabeling The atomic propositions labeling to copy.
     * @param substates A subset of the states that is to be copied.
     */
    AtomicPropositionsLabeling(AtomicPropositionsLabeling const& atomicPropositionsLabeling, storm::storage::BitVector const& substates)
            : stateCount(substates.getNumberOfSetBits()), apCountMax(atomicPropositionsLabeling.apCountMax), apsCurrent(atomicPropositionsLabeling.apsCurrent),
            nameToLabelingMap(atomicPropositionsLabeling.nameToLabelingMap) {
        // Now we need to copy the fraction of the single labelings given by the substates.
        singleLabelings.reserve(apCountMax);
        for (auto const& labeling : atomicPropositionsLabeling.singleLabelings) {
            singleLabelings.emplace_back(labeling % substates);
        }
    }

	/*!
	 * Move Constructor that performs a move copy on the given atomic proposition labeling.
     *
	 * @param atomicPropositionsLabeling The atomic propositions labeling to copy.
	 */
	AtomicPropositionsLabeling(AtomicPropositionsLabeling&& atomicPropositionsLabeling)
			: stateCount(atomicPropositionsLabeling.stateCount),
			  apCountMax(atomicPropositionsLabeling.apCountMax),
			  apsCurrent(atomicPropositionsLabeling.apsCurrent),
			  nameToLabelingMap(std::move(atomicPropositionsLabeling.nameToLabelingMap)),
              singleLabelings(std::move(atomicPropositionsLabeling.singleLabelings)) {
        // Intentionally left empty.
    }
    
    /*!
     * Assignment operator that copies the contents of the right-hand-side to the current labeling.
     *
     * @param other The atomic propositions labeling to copy.
     */
    AtomicPropositionsLabeling& operator=(AtomicPropositionsLabeling const& other) {
        if (this != &other) {
            this->stateCount = other.stateCount;
            this->apCountMax = other.apCountMax;
            this->apsCurrent = other.apsCurrent;
            this->nameToLabelingMap = other.nameToLabelingMap;
            this->singleLabelings = other.singleLabelings;
        }
        return *this;
    }
    
    /*!
     * Assignment operator that moves the contents of the right-hand-side to the current labeling.
     *
     * @param other The atomic propositions labeling to move.
     */
    AtomicPropositionsLabeling& operator=(AtomicPropositionsLabeling&& other) {
        if (this != &other) {
            this->stateCount = other.stateCount;
            this->apCountMax = other.apCountMax;
            this->apsCurrent = other.apsCurrent;
            this->nameToLabelingMap = std::move(other.nameToLabelingMap);
            this->singleLabelings = std::move(other.singleLabelings);
        }
        return *this;
    }

	/*!
	 * (Empty) destructor.
	 */
	~AtomicPropositionsLabeling() {
        // Intentionally left empty.
	}

	/*!
	 * Registers the name of a proposition.
	 * Will throw an error if an atomic proposition is registered twice.
	 * If more atomic propositions are added than previously declared, the maximum number of propositions
	 * is increased and the capacity of the singleLabelings vector is matched with the new maximum.
	 * 
     * @param ap The name of the atomic proposition to add.
	 * @return The index of the new proposition.
	 */
	uint_fast64_t addAtomicProposition(std::string const& ap) {
		if (nameToLabelingMap.count(ap) != 0) {
			LOG4CPLUS_ERROR(logger, "Atomic Proposition already exists.");
			throw storm::exceptions::OutOfRangeException("Atomic Proposition already exists.");
		}
		if (apsCurrent >= apCountMax) {
			apCountMax++;
			singleLabelings.reserve(apCountMax);
		}
		nameToLabelingMap.emplace(ap, apsCurrent);
        singleLabelings.emplace_back(stateCount);

		uint_fast64_t returnValue = apsCurrent++;
		return returnValue;
	}

	/*!
	 * Checks whether the name of an atomic proposition is registered within this labeling.
     *
	 * @return True if the proposition is registered within the labeling, false otherwise.
	 */
	bool containsAtomicProposition(std::string const& ap) const {
		return nameToLabelingMap.find(ap) != nameToLabelingMap.end();
	}

	/*!
	 * Adds an atomic proposition to a given state.
     *
	 * @param ap The name of the atomic proposition.
	 * @param state The index of the state to label.
	 */
	void addAtomicPropositionToState(std::string const& ap, uint_fast64_t const state) {
		if (!this->containsAtomicProposition(ap)) {
			LOG4CPLUS_ERROR(logger, "Atomic Proposition '" << ap << "' unknown.");
			throw storm::exceptions::OutOfRangeException() << "Atomic Proposition '" << ap << "' unknown.";
		}
		if (state >= stateCount) {
			LOG4CPLUS_ERROR(logger, "State index out of range.");
			throw storm::exceptions::OutOfRangeException("State index out of range.");
		}
		this->singleLabelings[nameToLabelingMap[ap]].set(state, true);
	}

	/*!
	 * Returns a set of atomic propositions of a given state.
     *
	 * @param state The index of a state.
	 * @returns The set of atomic propositions for the given state.
	 */
	std::set<std::string> getPropositionsForState(uint_fast64_t state) const {
		if (state >= stateCount) {
			LOG4CPLUS_ERROR(logger, "State index out of range.");
			throw storm::exceptions::OutOfRangeException("State index out of range.");
		}
		std::set<std::string> result;
		for (auto apIndexPair : nameToLabelingMap) {
			if (this->getStateHasAtomicProposition(apIndexPair.first, state)) {
				result.insert(apIndexPair.first);
			}
		}
		return result;
	}

	/*!
	 * Checks whether a given state is labeled with the given atomic proposition.
     *
	 * @param ap The name of the atomic proposition.
	 * @param state The index of the state to check.
	 * @return True if the node is labeled with the atomic proposition, false otherwise.
	 */
	bool getStateHasAtomicProposition(std::string const& ap, const uint_fast64_t state) const {
		if (!this->containsAtomicProposition(ap)) {
			LOG4CPLUS_ERROR(logger, "The atomic proposition " << ap << " is invalid for the labeling of the model.");
			throw storm::exceptions::InvalidArgumentException() << "The atomic proposition " << ap << " is invalid for the labeling of the model.";
		}
        auto apIndexPair = nameToLabelingMap.find(ap);
		return this->singleLabelings[apIndexPair->second].get(state);
	}

	/*!
	 * Returns the number of atomic propositions managed by this object (set in the initialization).
     *
	 * @return The number of atomic propositions.
	 */
	uint_fast64_t getNumberOfAtomicPropositions() const {
		return apCountMax;
	}

	/*!
	 * Returns the internal index of a given atomic proposition.
	 *
	 * @return The index of the atomic proposition.
	 */
	uint_fast64_t getIndexOfProposition(std::string const& ap) const {
		if (!this->containsAtomicProposition(ap)) {
			LOG4CPLUS_ERROR(logger, "The atomic proposition " << ap << " is invalid for the labeling of the model.");
			throw storm::exceptions::InvalidArgumentException() << "The atomic proposition " << ap << " is invalid for the labeling of the model.";
		}
		auto apIndexPair = nameToLabelingMap.find(ap);
		return apIndexPair->second;
	}

	/*!
	 * Returns the labeling of states associated with the given atomic proposition.
     *
	 * @param ap The name of the atomic proposition.
	 * @return A bit vector that represents the labeling of the states with the given atomic proposition.
	 */
	storm::storage::BitVector const& getLabeledStates(std::string const& ap) const {
		if (!this->containsAtomicProposition(ap)) {
			LOG4CPLUS_ERROR(logger, "The atomic proposition " << ap << " is invalid for the labeling of the model.");
			throw storm::exceptions::InvalidArgumentException() << "The atomic proposition " << ap << " is invalid for the labeling of the model.";
		}
        auto apIndexPair = nameToLabelingMap.find(ap);
		return this->singleLabelings[apIndexPair->second];
	}

   /*!
    * Returns the size of the labeling in memory measured in bytes.
    *
    * @return The size of the labeling in memory measured in bytes.
    */
	uint_fast64_t getSizeInMemory() const {
        // Add size of this very object to the sizes of the single labelings.
		return sizeof(*this) + apCountMax * sizeof(storm::storage::BitVector);
	}

	/*!
	 * Prints information about the labeling to the specified stream.
     *
	 * @param out The stream the information is to be printed to.
	 */
	void printAtomicPropositionsInformationToStream(std::ostream& out) const {
		out << "Atomic Propositions: \t" << this->getNumberOfAtomicPropositions()
			<< std::endl;
		for (auto apIndexPair : this->nameToLabelingMap) {
			out << "\t * " << apIndexPair.first << " -> "
				<< this->singleLabelings[apIndexPair.second].getNumberOfSetBits();
			out << " state(s)" << std::endl;
		}
	}

	/*!
	 * Adds a state to the labeling.
	 * Since this operation is quite expensive (resizing of all BitVectors containing the labeling), it should
	 * only be used in special cases to add one or two states.
	 * If you want to build a new AtomicPropositionlabeling:
	 *   - Count the number of states you need.
	 *   - Then add the labelings using addAtomicProposition() and addAtomicPropositionToState().
	 * Do NOT use this method for this purpose.
	 */
	void addState() {
		for(uint_fast64_t i = 0; i < apsCurrent; i++) {
			singleLabelings[i].resize(singleLabelings[i].size() + 1);
		}
		stateCount++;
	}

	/*!
	 * Calculates a hash over all values contained in this Atomic Proposition Labeling.
	 * @return size_t A Hash Value
	 */
	std::size_t getHash() const {
		std::size_t result = 0;

		boost::hash_combine(result, stateCount);
		boost::hash_combine(result, apCountMax);
		boost::hash_combine(result, apsCurrent);
		
		for (auto it = nameToLabelingMap.begin(); it != nameToLabelingMap.end(); ++it) {
			boost::hash_combine(result, it->first);
			boost::hash_combine(result, it->second);
		}

		for (auto it = singleLabelings.begin(); it != singleLabelings.end(); ++it) {
			boost::hash_combine(result, it->hash());
		}

		return result;
	}

private:

	/*! The number of states whose labels are to be stored by this object. */
	uint_fast64_t stateCount;

	/*! The number of different atomic propositions this object can store. */
	uint_fast64_t apCountMax;

	/*!
	 * The number of different atomic propositions currently associated with
	 * this labeling. Used to prevent too many atomic propositions from being
	 * added to this object.
	 */
	uint_fast64_t apsCurrent;

	/*!
	 * Associates a name of an atomic proposition to its corresponding labeling
	 * by mapping the name to a specific index in the array of all
	 * individual labelings.
	 */
	std::map<std::string, uint_fast64_t> nameToLabelingMap;

	/*!
	 * Stores all individual labelings. To find the labeling associated with
	 * a particular atomic proposition, the map from names to indices in this
	 * array has to be used.
	 */
    std::vector<storm::storage::BitVector> singleLabelings;
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_ATOMICPROPOSITIONSLABELING_H_ */
