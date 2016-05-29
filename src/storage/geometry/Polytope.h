#ifndef STORM_STORAGE_GEOMETRY_POLYTOPE_H_
#define STORM_STORAGE_GEOMETRY_POLYTOPE_H_

#include <vector>
#include <memory>

#include "src/storage/geometry/HalfSpace.h"

namespace storm {
    namespace storage {
        namespace geometry {
            template <typename ValueType>
            class Polytope {
            public:
                
                typedef std::vector<ValueType> point_t;
                typedef HalfSpace<ValueType> halfspace_t;
                
                /*!
                 * Creates a polytope from the given points (i.e., the convex hull of the points).
                 * If the vector of points is empty, the resulting polytope be empty.
                 */
                static std::shared_ptr<Polytope<ValueType>> create(std::vector<point_t> const& points);
                
                /*!
                 * Creates a polytope from the given halfspaces.
                 * If the given vector of halfspaces is empty, the resulting polytope is universal (i.e., equals |R^n).
                 */
                static std::shared_ptr<Polytope<ValueType>> create(std::vector<Halfspace<ValueType>> const& halfspaces);
                
                /*!
                 * Creates a polytope P from the given points (i.e., the convex hull of the points)
                 * and returns the downward closure of P, i.e., the set { x | ex. y \in P: x<=y}
                 * If the vector of points is empty, the resulting polytope be empty.
                 */
                static std::shared_ptr<Polytope<ValueType>> createDownwardClosure(std::vector<point_t> const& points);

                /*!
                 * Returns the vertices of this polytope.
                 */
                virtual std::vector<point_t> getVertices();
                
                /*!
                 * Returns the halfspaces of this polytope.
                 */
                virtual std::vector<Halfspace<ValueType>> getHalfspaces();
                
                /*!
                 * Returns true iff the given point is inside of the polytope.
                 */
                virtual bool contains(point_t const& point) const;
                
                /*!
                 * Intersects this polytope with rhs and returns the result.
                 */
                virtual unstd::shared_ptr<Polytope<ValueType>> intersect(std::shared_ptr<Polytope<ValueType>> const& rhs) const;
                virtual unstd::shared_ptr<Polytope<ValueType>> intersect(Halfspace<ValueType> const& rhs) const;
                
                /*!
                 * Returns the convex union of this polytope and rhs.
                 */
                virtual std::shared_ptr<Polytope<ValueType>> convexUnion(std::shared_ptr<Polytope<ValueType>> const& rhs) const;
                virtual std::shared_ptr<Polytope<ValueType>> convexUnion(point_t const& rhs) const;
                
                /*!
                 * Bloats the polytope
                 * The resulting polytope is an overapproximation of the minkowski sum of this polytope and the hyperrectangle given by point1
                 * and point2 but does not introduce new halfspaces.
                 * In more detail, let P={ x | A*x<=b} be the current polytope and R be the smallest hyperrectangle containing the two points.
                 * The result is the smallest polytope P' with 
                 * 1. P'={ x | A*x<=b'}
                 * 2. For each p \in P and r \in R it holds that (p+r) \in P'
                 */
                virtual std::shared_ptr<Polytope<ValueType>> bloat(point_t const& point1, point_t const& point2) const;

                
            private:
                virtual Polytope();
                
                /*!
                 * Creates a polytope from the given points (i.e., the convex hull of the points).
                 * If the vector of points is empty, the resulting polytope be empty.
                 */
                virtual Polytope(std::vector<point_t> const& points);
                
                /*!
                 * Creates a polytope from the given halfspaces.
                 * If the given vector of halfspaces is empty, the resulting polytope is universal (i.e., equals |R^n).
                 */
                virtual Polytope(std::vector<Halfspace<ValueType>> const& halfspaces);
                
            };
            
        }
    }
}

#endif /* STORM_STORAGE_GEOMETRY_POLYTOPE_H_ */
