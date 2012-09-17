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

namespace mrmc {

namespace dtmc {


class labeling {
   public:


      labeling(const uint_fast32_t p_nodes) {
         nodes = p_nodes;
      }

      virtual ~labeling() {
         //deleting all the labeling vectors in the map.
         MAP<std::string, AtomicProposition*>::iterator it;
         for (it = proposition_map.begin(); it != proposition_map.end(); ++it) {
            if (it->second != NULL) {
               delete (it->second);
            }
         }
      }

      void addProposition(std::string proposition) {
         if (proposition_map.count(proposition) != 0) {
            throw std::out_of_range("Proposition does already exist.");
         }
         proposition_map[proposition] = new AtomicProposition(nodes);
      }

      bool containsProposition(std::string proposition) {
         return (proposition_map.count(proposition) != 0);
      }


      void addLabelToNode(std::string proposition, const uint_fast32_t node) {
         //TODO (Thomas Heinemann): Differentiate exceptions?
         if (proposition_map.count(proposition) == 0) {
            throw std::out_of_range("Proposition does not exist.");
         }
         if (node >= nodes) {
            throw std::out_of_range("Node number out of range");
         }
         AtomicProposition* prop = (proposition_map[proposition]);
         prop->addLabelToNode(node);
      }

      bool nodeHasProposition(std::string proposition, const uint_fast32_t node) {
         return proposition_map[proposition]->hasNodeLabel(node);
      }

      uint_fast32_t getNumberOfPropositions() {
         return proposition_map.size();
      }

      AtomicProposition* getProposition(std::string proposition) {
         return (proposition_map[proposition]);
      }

   private:
      uint_fast32_t nodes;
      MAP<std::string, AtomicProposition*> proposition_map;
      //AtomicProposition** propositions;
      //boost::unordered_map<std::string, AtomicProposition*> proposition_map;
};

} //namespace dtmc

} //namespace mrmc

#endif /* MRMC_DTMC_LABELING_H_ */
