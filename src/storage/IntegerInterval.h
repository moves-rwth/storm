#pragma once 

#include <boost/optional.hpp>

namespace storm {
    namespace storage {
        class IntegerInterval {
            
        public:
            explicit IntegerInterval(uint64_t v) : leftBound(v), rightBound(v) {
                
            }
            
            IntegerInterval(uint64_t lb, uint64_t rb) : leftBound(lb), rightBound(rb) {
                
            }
            
            bool hasLeftBound() {
                return leftBound != boost::none;
            }
            
            bool hasRightBound() {
                return rightBound != boost::none;
            }
            
            boost::optional<uint64_t> getLeftBound() {
                return leftBound;
            }
            
            boost::optional<uint64_t> getRightBound() {
                return rightBound;
            }
            
        private:
            boost::optional<uint64_t> leftBound;
            boost::optional<uint64_t> rightBound;
            
        };
        
        std::ostream& operator<<(std::ostream& os, IntegerInterval const& i);
    }
}