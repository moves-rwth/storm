/*
 * labelling.h
 *
 *  Created on: 10.09.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_DTMC_LABELING_H_
#define MRMC_DTMC_LABELING_H_

#include "atomic_proposition.h"



/* Map types: By default, the boost hash map is used.
 * When the macro DEFAULT_MAP is defined, the default C++ class (std::map)
 * is used instead.
 */
#ifdef DEFAULT_MAP
#include <map>
#define MAP std::map
#else
#include "boost/unordered_map.hpp"
#define MAP boost::unordered_map
#endif

#include <stdexcept>

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

namespace mrmc {

namespace dtmc {


class labeling {
   public:


      labeling(const uint_fast32_t nodeCount,
               const uint_fast32_t propositionCount) {
         node_count = nodeCount;
         proposition_count = propositionCount;
         propositions_current = 0;
         propositions = new AtomicProposition*[proposition_count];
		 for (uint_fast32_t i = 0; i < proposition_count; ++i) {
            propositions[i] = new AtomicProposition(node_count);
         }
      }

      virtual ~labeling() {
         //deleting all the labeling vectors in the map.
         MAP<std::string, AtomicProposition*>::iterator it;
         for (uint_fast32_t i = 0; i < proposition_count; ++i) {
            delete propositions[i];
            propositions[i] = NULL;
         }
         delete[] propositions;
         propositions = NULL;
      }

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

      bool containsProposition(std::string proposition) {
         return (proposition_map.count(proposition) != 0);
      }


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

      bool nodeHasProposition(std::string proposition, const uint_fast32_t node) {
         return propositions[proposition_map[proposition]]->hasNodeLabel(node);
      }

      uint_fast32_t getNumberOfPropositions() {
		  return proposition_count;
      }

      AtomicProposition* getProposition(std::string proposition) {
         return (propositions[proposition_map[proposition]]);
      }

   private:
      uint_fast32_t node_count, proposition_count, propositions_current;
      MAP<std::string, uint_fast32_t> proposition_map;
      AtomicProposition** propositions;
};

} //namespace dtmc

} //namespace mrmc

#endif /* MRMC_DTMC_LABELING_H_ */
