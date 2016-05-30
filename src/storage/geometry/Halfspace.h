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
            class Halfspace {
                
            public:
                
                Halfspace(std::vector<ValueType> const& normalVector, ValueType const& offset) : mNormalVector(normalVector), mOffset(offset)  {
                    //Intentionally left empty
                }
                
                Halfspace(std::vector<ValueType>&& normalVector, ValueType&& offset) : mNormalVector(normalVector), mOffset(offset) {
                    //Intentionally left empty
                }
                
                bool contains(std::vector<ValueType> const& point) {
                    return storm::utility::vector::multiplyVectors(point, normalVector()) <= offset();
                }

                std::string toString() {
                    std::stringstream stream;
                    stream << "(";
                    for(auto it = normalVector().begin(); it != normalVector().end(); ++it){
                        if(it != normalVector().begin()){
                            stream << ", ";
                        }
                        stream << *it;
                    }
                    stream << ") * x <= " << offset();
                    return stream.str();
                }
                
                std::vector<ValueType> const& normalVector() const {
                    return mNormalVector;
                }
                
                std::vector<ValueType>& normalVector(){
                    return mNormalVector;
                }
                
                ValueType const& offset() const {
                    return mOffset;
                }
                
                ValueType& offset(){
                    return mOffset;
                }
                
            private:
                
                std::vector<ValueType> mNormalVector;
                ValueType mOffset;
                
            };
        }
    }
}

#endif /* STORM_STORAGE_GEOMETRY_HALFSPACE_H_ */
