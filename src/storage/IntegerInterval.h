#pragma once 

#include <boost/optional.hpp>

namespace storm {
    namespace storage {
        class IntegerInterval {
            
        public:
            explicit IntegerInterval(int64_t v) : leftBound(v), rightBound(v) {
                
            }
            
            IntegerInterval(int64_t lb, int64_t rb) : leftBound(lb), rightBound(rb) {
                
            }
            
            bool hasLeftBound() const {
                return leftBound != boost::none;
            }
            
            bool hasRightBound() const {
                return rightBound != boost::none;
            }
            
            bool contains(int64_t val) const {
                if (hasLeftBound()) {
                    if (val < leftBound.get()) {
                        return false;
                    }
                }
                if (hasRightBound()) {
                    if (val > rightBound.get()) {
                        return false;
                    }
                }
                return true;
            }
            
            boost::optional<int64_t> getLeftBound() const {
                return leftBound;
            }
            
            boost::optional<int64_t> getRightBound() const {
                return rightBound;
            }
            
        private:
            boost::optional<int64_t> leftBound;
            boost::optional<int64_t> rightBound;
            
        };
        
        std::ostream& operator<<(std::ostream& os, IntegerInterval const& i);
    }
}