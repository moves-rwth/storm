#ifndef STORM_UTILITY_HASH_H_
#define STORM_UTILITY_HASH_H_

#include <boost/functional/hash.hpp>
#include <functional>
#include <vector>

namespace storm {
namespace utility {

template<class T>
class Hash {
   public:
    static std::size_t getHash(std::vector<T> const& target) {
        return boost::hash_range(target.begin(), target.end());
    }

   private:
    Hash() {}
    ~Hash() {}
};

}  // namespace utility
}  // namespace storm

#endif  // STORM_UTILITY_HASH_H_
