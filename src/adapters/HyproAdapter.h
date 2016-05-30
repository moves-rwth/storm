#ifndef STORM_ADAPTERS_HYPROADAPTER_H_
#define STORM_ADAPTERS_HYPROADAPTER_H_

// Include config to know whether HyPro is available or not.
#include "storm-config.h"

#ifdef STORM_HAVE_HYPRO

#include </Users/tim/hypro/src/lib/datastructures/Halfspace.h>
//#include <lib/types.h>
#include <lib/representations/Polytope/Polytope.h>

#include "src/adapters/CarlAdapter.h"
#include "src/storage/geometry/HalfSpace.h"

namespace storm {
    namespace adapters {
        
            
        template <typename T>
        std::vector<T> fromHypro(hypro::vector_t<T> const& v) {
            return std::vector<T>(v.data(), v.data() + v.rows());
        }
            
        template <typename T>
        hypro::vector_t<T> toHypro(std::vector<T> const& v) {
            hypro::vector_t<T> res(v.size());
            for ( auto const& value : v){
                res << value;
            }
            return res;
        }
        
        template <typename T>
        hypro::Halfspace<T> toHypro(storm::storage::geometry::Halfspace<T> const& h){
            return hypro::Halfspace<T>(toHypro(h.normalVector()), h.offset());
        }
        
        template <typename T>
        hypro::Halfspace<T> fromHypro(hypro::Halfspace<T> const& h){
            return storm::storage::geometry::Halfspace<T>(fromHypro(h.normal()), h.offset());
        }
        
    }
}
#endif //STORM_HAVE_HYPRO
        
#endif /* STORM_ADAPTERS_HYPROADAPTER_H_ */
