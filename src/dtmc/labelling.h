/*
 * labelling.h
 *
 *  Created on: 10.09.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_DTMC_LABELLING_H_
#define MRMC_DTMC_LABELLING_H_

#include "atomic_proposition.h"

#include "boost/unordered_map.hpp"

#include <stdexcept>

namespace mrmc {

namespace dtmc {


class labelling {
   public:


      labelling(const uint_fast32_t p_nodes) {
         nodes = p_nodes;
      }

      virtual ~labelling() {
         //deleting all the labelling vectors in the map.
         boost::unordered_map<std::string, AtomicProposition*>::iterator it;
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
      boost::unordered_map<std::string, AtomicProposition*> proposition_map;
};

} //namespace dtmc

} //namespace mrmc

#endif /* MRMC_DTMC_LABELLING_H_ */
