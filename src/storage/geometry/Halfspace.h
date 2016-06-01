#ifndef STORM_STORAGE_GEOMETRY_HALFSPACE_H_
#define STORM_STORAGE_GEOMETRY_HALFSPACE_H_

#include <iostream>
#include <iomanip>
#include "src/utility/constants.h"
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
                
                /*
                 * Returns true iff the given point is contained in this halfspace, i.e., normalVector*point <= offset holds.
                 */
                bool contains(std::vector<ValueType> const& point) {
                    return storm::utility::vector::dotProduct(point, normalVector()) <= offset();
                }

                /*
                 * Returns a string representation of this Halfspace.
                 * If the given flag is true, the occurring numbers are converted to double before printing to increase readability
                 */
                std::string toString(bool numbersAsDouble = false) const {
                    std::stringstream stream;
                    stream << "(";
                    for(auto it = normalVector().begin(); it != normalVector().end(); ++it){
                        if(it != normalVector().begin()){
                            stream << ", ";
                        }
                        if(numbersAsDouble) {
                            stream << std::setw(10) << storm::utility::convertNumber<double>(*it);
                        } else {
                            stream << std::setw(10) << *it;
                        }
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
