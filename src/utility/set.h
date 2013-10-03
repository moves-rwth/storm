/*
 * set.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_SET_H_
#define STORM_UTILITY_SET_H_

#include <set>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {
    namespace utility {
        namespace set {
            
            template<typename T, typename Compare>
            bool isSubsetOf(std::set<T, Compare> const& set1, std::set<T, Compare> const& set2) {
                // First, get a comparator object.
                typename std::set<T, Compare>::key_compare comparator = set1.key_comp();
                
                for (typename std::set<T, Compare>::const_iterator it1 = set1.begin(), it2 = set2.begin(); it1 != set1.end() && it2 != set2.end(); ++it1) {
                    // If the value in set1 is smaller than the value in set2, set1 is not a subset of set2.
                    if (comparator(*it1, *it2)) {
                        return false;
                    }
                    
                    // If the value in the second set is smaller, we need to move the iterator until the comparison is false.
                    while(comparator(*it2, *it1) && it2 != set2.end()) {
                        ++it2;
                    }
                        
                    // If we have reached the end of set2 or the element we found is actually larger than the one in set1
                    // we know that the subset property is violated.
                    if (it2 == set2.end() || comparator(*it1, *it2)) {
                        return false;
                    }
                        
                    // Otherwise, we have found an equivalent element and can continue with the next one.
                }
                return true;
            }
            
        } // namespace set
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_SET_H_ */
