#ifndef STORM_STORAGE_GEOMETRY_POLYTOPE_H_
#define STORM_STORAGE_GEOMETRY_POLYTOPE_H_

#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "src/storage/geometry/Halfspace.h"

namespace storm {
    namespace storage {
        namespace geometry {
            
            template <typename ValueType>
            class Polytope {
            public:
                
                typedef std::vector<ValueType> Point;
                
                Polytope() = delete; //Use create methods to assemble a polytope
                
                /*!
                 * Creates a polytope from the given halfspaces.
                 * If the given vector of halfspaces is empty, the resulting polytope is universal (i.e., equals |R^n).
                 */
                static std::shared_ptr<Polytope<ValueType>> create(std::vector<Halfspace<ValueType>> const& halfspaces);
                
                /*!
                 * Creates a polytope from the given points (i.e., the convex hull of the points).
                 * If the vector of points is empty, the resulting polytope be empty.
                 */
                static std::shared_ptr<Polytope<ValueType>> create(std::vector<Point> const& points);
                
                /*!
                 * Creates a polytope P from the given halfspaces
                 * and returns the downward closure of P, i.e., the set { x | ex. y \in P: x<=y}
                 * If the vector of halfspaces is empty, the resulting polytope will be universal.
                 */
                static std::shared_ptr<Polytope<ValueType>> createDownwardClosure(std::vector<Halfspace<ValueType>> const& halfspaces);
                
                /*!
                 * Creates a polytope P from the given points (i.e., the convex hull of the points)
                 * and returns the downward closure of P, i.e., the set { x | ex. y \in P: x<=y}
                 * If the vector of points is empty, the resulting polytope be empty.
                 */
                static std::shared_ptr<Polytope<ValueType>> createDownwardClosure(std::vector<Point> const& points);

                /*!
                 * Returns the vertices of this polytope.
                 */
                virtual std::vector<Point> getVertices();
                
                /*!
                 * Returns the halfspaces of this polytope.
                 */
                virtual std::vector<Halfspace<ValueType>> getHalfspaces();
                
                /*!
                 * Returns true iff the given point is inside of the polytope.
                 */
                virtual bool contains(Point const& point) const;
                
                /*!
                 * Intersects this polytope with rhs and returns the result.
                 */
                virtual std::shared_ptr<Polytope<ValueType>> intersect(std::shared_ptr<Polytope<ValueType>> const& rhs) const;
                virtual std::shared_ptr<Polytope<ValueType>> intersect(Halfspace<ValueType> const& rhs) const;
                
                /*!
                 * Returns the convex union of this polytope and rhs.
                 */
                virtual std::shared_ptr<Polytope<ValueType>> convexUnion(std::shared_ptr<Polytope<ValueType>> const& rhs) const;
                virtual std::shared_ptr<Polytope<ValueType>> convexUnion(Point const& rhs) const;
                
                /*!
                 * Bloats the polytope
                 * The resulting polytope is (possibly an overapproximation of) the minkowski sum of this polytope and the 
                 * hyperrectangle given by point1 and point2.
                 */
                virtual std::shared_ptr<Polytope<ValueType>> bloat(Point const& point1, Point const& point2) const;

                
                /*
                 * Returns a string representation of this polytope.
                 */
                virtual std::string toString() const;
               
            private:
                /*!
                 * Creates a polytope from the given halfspaces or vertices.
                 * if the given flag is true, the downward closure will be created.
                 */
                static std::shared_ptr<Polytope<ValueType>> create(boost::optional<std::vector<Halfspace<ValueType>>> const& halfspaces,
                                                                   boost::optional<std::vector<Point>> const& points,
                                                                   bool downwardClosure);
                
                Polytope(boost::optional<std::vector<Halfspace<ValueType>>> const& halfspaces,
                                 boost::optional<std::vector<Point>> const& points,
                                 bool downwardClosure);
                
            };
            
        }
    }
}

#endif /* STORM_STORAGE_GEOMETRY_POLYTOPE_H_ */
