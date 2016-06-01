#ifndef STORM_STORAGE_GEOMETRY_HYPROPOLYTOPE_H_
#define STORM_STORAGE_GEOMETRY_HYPROPOLYTOPE_H_

#include "src/storage/geometry/Polytope.h"
#include "src/adapters/HyproAdapter.h"

#ifdef STORM_HAVE_HYPRO

namespace storm {
    namespace storage {
        namespace geometry {
            
            template <typename ValueType>
            class HyproPolytope : public Polytope<ValueType> {
            public:
                
                typedef typename Polytope<ValueType>::Point Point;
                typedef hypro::HPolytope<ValueType> HyproPolytopeType;
                
                /*!
                 * Creates a HyproPolytope from the given halfspaces or points.
                 * If both representations are given, one of them might be ignored
                 */
                static std::shared_ptr<Polytope<ValueType>> create(boost::optional<std::vector<Halfspace<ValueType>>> const& halfspaces,
                                                                   boost::optional<std::vector<Point>> const& points);
                
                /*!
                 * Creates a HyproPolytope from the given halfspaces
                 * The resulting polytope is defined as the intersection of the halfspaces.
                 */
                HyproPolytope(std::vector<Halfspace<ValueType>> const& halfspaces);
                
                /*!
                 * Creates a HyproPolytope from the given points.
                 * The resulting polytope is defined as the convex hull of the points'
                 */
                HyproPolytope(std::vector<Point> const& points);
                
                /*!
                 * Copy and move constructors
                 */
                HyproPolytope(HyproPolytope<ValueType> const& other);
                HyproPolytope(HyproPolytope<ValueType>&& other);
                
                /*!
                 * Construction from a plain hypro polytope
                 */
                HyproPolytope(HyproPolytopeType const& polytope);
                HyproPolytope(HyproPolytopeType&& polytope);
                
                ~HyproPolytope();

                /*!
                 * Returns the vertices of this polytope.
                 */
                virtual std::vector<Point> getVertices() const override;
                
                /*!
                 * Returns the halfspaces of this polytope.
                 */
                virtual std::vector<Halfspace<ValueType>> getHalfspaces() const override;
                
                
                /*!
                 * Returns whether this polytope is the empty set.
                 */
                virtual bool isEmpty() const override;
                
                /*!
                 * Returns whether this polytope is universal (i.e., equals R^n).
                 */
                virtual bool isUniversal() const override;
                
                /*!
                 * Returns true iff the given point is inside of the polytope.
                 */
                virtual bool contains(Point const& point) const override;
                
                /*!
                 * Returns true iff the given polytope is a subset of this polytope.
                 */
                virtual bool contains(std::shared_ptr<Polytope<ValueType>> const& other) const override;
                
                /*!
                 * Intersects this polytope with rhs and returns the result.
                 */
                virtual std::shared_ptr<Polytope<ValueType>> intersection(std::shared_ptr<Polytope<ValueType>> const& rhs) const override;
                virtual std::shared_ptr<Polytope<ValueType>> intersection(Halfspace<ValueType> const& halfspace) const override;
                
                /*!
                 * Returns the convex union of this polytope and rhs.
                 */
                virtual std::shared_ptr<Polytope<ValueType>> convexUnion(std::shared_ptr<Polytope<ValueType>> const& rhs) const override;
                
                /*!
                 * Returns the minkowskiSum of this polytope and rhs.
                 */
                virtual std::shared_ptr<Polytope<ValueType>> minkowskiSum(std::shared_ptr<Polytope<ValueType>> const& rhs) const override;
                
                /*!
                 * Returns the downward closure of this, i.e., the set { x | ex. y \in P : x<=y} where P is this Polytope.
                 * Put differently, the resulting polytope corresponds to this polytope, where
                 * 1. a vector y with y_i=max{x_i | x \in P} is computed and for each i, a halfspace with offset y_i and
                 *    normal vector n (where n_i = 1 and the remaining entries are 0) is inserted.
                 * 2. all halfspaces where the normal vector has at least one negative entry are removed
                 *
                 * @param upperBounds If given, this vector is considered for y (hence, max{x_i | x i \in P does not need to be computed)
                 */
                virtual std::shared_ptr<Polytope<ValueType>> downwardClosure(boost::optional<Point> const& upperBounds = boost::none) const override;
            
                virtual bool isHyproPolytope() const override;
                
            private:
                
                HyproPolytopeType internPolytope;
            };
            
        }
    }
}

#endif /* STORM_HAVE_HYPRO */

#endif /* STORM_STORAGE_GEOMETRY_HYPROPOLYTOPE_H_ */
