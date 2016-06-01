#ifndef STORM_STORAGE_GEOMETRY_POLYTOPE_H_
#define STORM_STORAGE_GEOMETRY_POLYTOPE_H_

#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "src/storage/geometry/Halfspace.h"

namespace storm {
    namespace storage {
        namespace geometry {
            
            // Forward declaration
            template <typename ValueType>
            class HyproPolytope;
            
            template <typename ValueType>
            class Polytope {
            public:
                
                typedef std::vector<ValueType> Point;
            
                ~Polytope();
                
                /*!
                 * Creates a polytope from the given halfspaces.
                 * If the given vector of halfspaces is empty, the resulting polytope is universal (i.e., equals R^n).
                 */
                static std::shared_ptr<Polytope<ValueType>> create(std::vector<Halfspace<ValueType>> const& halfspaces);
                
                /*!
                 * Creates a polytope from the given points (i.e., the convex hull of the points).
                 * If the vector of points is empty, the resulting polytope be empty.
                 */
                static std::shared_ptr<Polytope<ValueType>> create(std::vector<Point> const& points);
                
                /*!
                 * Returns the vertices of this polytope.
                 */
                virtual std::vector<Point> getVertices() const;
                
                /*!
                 * Returns the halfspaces of this polytope.
                 */
                virtual std::vector<Halfspace<ValueType>> getHalfspaces() const;
                
                /*!
                 * Returns whether this polytope is the empty set.
                 */
                virtual bool isEmpty() const;
                
                /*!
                 * Returns whether this polytope is universal (i.e., equals R^n).
                 */
                virtual bool isUniversal() const;
                
                /*!
                 * Returns true iff the given point is inside of the polytope.
                 */
                virtual bool contains(Point const& point) const;
                
                /*!
                 * Returns true iff the given polytope is a subset of this polytope.
                 */
                virtual bool contains(std::shared_ptr<Polytope<ValueType>> const& other) const;
                
                /*!
                 * Intersects this polytope with rhs and returns the result.
                 */
                virtual std::shared_ptr<Polytope<ValueType>> intersection(std::shared_ptr<Polytope<ValueType>> const& rhs) const;
                virtual std::shared_ptr<Polytope<ValueType>> intersection(Halfspace<ValueType> const& halfspace) const;
                
                /*!
                 * Returns the convex union of this polytope and rhs.
                 */
                virtual std::shared_ptr<Polytope<ValueType>> convexUnion(std::shared_ptr<Polytope<ValueType>> const& rhs) const;
        
                /*!
                 * Returns the minkowskiSum of this polytope and rhs.
                 */
                virtual std::shared_ptr<Polytope<ValueType>> minkowskiSum(std::shared_ptr<Polytope<ValueType>> const& rhs) const;
                
                /*!
                 * Returns the downward closure of this, i.e., the set { x | ex. y \in P : x<=y} where P is this Polytope.
                 * Put differently, the resulting polytope corresponds to this polytope, where
                 * 1. a vector y with y_i=max{x_i | x \in P} is computed and for each i, a halfspace with offset y_i and
                 *    normal vector n (where n_i = 1 and the remaining entries are 0) is inserted.
                 * 2. all halfspaces where the normal vector has at least one negative entry are removed
                 *
                 * @param upperBounds If given, this vector is considered for y (hence, max{x_i | x i \in P does not need to be computed)
                 */
                virtual std::shared_ptr<Polytope<ValueType>> downwardClosure(boost::optional<Point> const& upperBounds = boost::none) const;
                
                /*
                 * Returns a string representation of this polytope.
                 * If the given flag is true, the occurring numbers are converted to double before printing to increase readability
                 */
                virtual std::string toString(bool numbersAsDouble = false) const;
               
                
                virtual bool isHyproPolytope() const;
                
                HyproPolytope<ValueType>& asHyproPolytope();
                HyproPolytope<ValueType> const& asHyproPolytope() const;
                
            protected:
                
                Polytope();
                
            private:
                /*!
                 * Creates a polytope from the given halfspaces or vertices.
                 */
                static std::shared_ptr<Polytope<ValueType>> create(boost::optional<std::vector<Halfspace<ValueType>>> const& halfspaces,
                                                                   boost::optional<std::vector<Point>> const& points);
                
                
            };
            
        }
    }
}

#endif /* STORM_STORAGE_GEOMETRY_POLYTOPE_H_ */
