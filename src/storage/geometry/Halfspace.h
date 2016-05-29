#ifndef STORM_STORAGE_GEOMETRY_HALFSPACE_H_
#define STORM_STORAGE_GEOMETRY_HALFSPACE_H_

#include <iostream>
#include "src/utility/vector.h"

namespace storm {
    namespace storage {
        namespace geometry {
            
            /* 
             * This class represents a closed Halfspace, i.e., the set { x | a*x<=c } for a normalVector a and an offset c
             */
             
            template <typename ValueType>
            struct HalfSpace {
                
                HalfSpace(std::vector<ValueType> const& normalVector, ValueType const& offset) : normalVector(normalVector), offset(offset)  {
                    //Intentionally left empty
                }
                
                HalfSpace(std::vector<ValueType>&& normalVector, ValueType&& offset) : normalVector(normalVector), offset(offset) {
                    //Intentionally left empty
                }
                
                bool contains(std::vector<ValueType> const& point) {
                    return storm::utility::vector::multiplyVectors(point, normalVector) <= offset;
                }

                std::vector<ValueType> normalVector;
                ValueType offset;
                
                std::string toString() {
                    std::stringstream stream;
                    stream << "(";
                    for(auto it = normalVector.begin(); it != normalVector.end(); ++it){
                        if(it != normalVector.begin()){
                            stream << ", ";
                        }
                        stream << *it;
                    }
                    stream << ") * x <= " << offset;
                    return stream.str();
                }
                
            }
        }
    }
}

#endif /* STORM_STORAGE_GEOMETRY_HALFSPACE_H_ */
