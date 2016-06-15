#include "src/storage/geometry/Polytope.h"

#include <iostream>

#include "src/adapters/CarlAdapter.h"
#include "src/adapters/HyproAdapter.h"
#include "src/storage/geometry/HyproPolytope.h"
#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace storage {
        namespace geometry {
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::create(std::vector<storm::storage::geometry::Halfspace<ValueType>> const& halfspaces) {
                return create(halfspaces, boost::none);
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::create(std::vector<Point> const& points) {
                return create(boost::none, points);
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::createUniversalPolytope() {
                return create(std::vector<Halfspace<ValueType>>(), boost::none);
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::createEmptyPolytope() {
                return create(boost::none, std::vector<Point>());
            }
            
#ifdef STORM_HAVE_CARL
            template <>
            std::shared_ptr<Polytope<storm::RationalNumber>> Polytope<storm::RationalNumber>::create(boost::optional<std::vector<Halfspace<storm::RationalNumber>>> const& halfspaces,
                                                                             boost::optional<std::vector<Point>> const& points) {
#ifdef STORM_HAVE_HYPRO
                return HyproPolytope<storm::RationalNumber>::create(halfspaces, points);
#endif
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "No polytope implementation specified.");
                return nullptr;
            }
#endif
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::create(boost::optional<std::vector<Halfspace<ValueType>>> const& halfspaces,
                                                                             boost::optional<std::vector<Point>> const& points) {
                //Note: hypro polytopes (currently) do not work with non-exact arithmetic (e.g., double)
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "No polytope implementation specified.");
                return nullptr;
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::createDownwardClosure(std::vector<Point> const& points) {
                if(points.empty()) {
                    // In this case, the downwardclosure is empty
                    return createEmptyPolytope();
                }
                uint_fast64_t const dimensions = points.front().size();
                std::vector<Halfspace<ValueType>> halfspaces;
                // We build the convex hull of the given points.
                // However, auxiliary points (that will always be in the downward closure) are added.
                // Then, the halfspaces of the resulting polytope are a superset of the halfspaces of the downward closure.
                std::vector<Point> auxiliaryPoints = points;
                auxiliaryPoints.reserve(auxiliaryPoints.size()*(1+dimensions));
                for(auto const& point : points) {
                    for(uint_fast64_t dim=0; dim<dimensions; ++dim) {
                        auxiliaryPoints.push_back(point);
                        auxiliaryPoints.back()[dim] -= storm::utility::one<ValueType>();
                    }
                }
                std::vector<Halfspace<ValueType>> auxiliaryHalfspaces = create(auxiliaryPoints)->getHalfspaces();
                // The downward closure is obtained by selecting the halfspaces for which the normal vector is non-negative (coordinate wise).
                // Note that due to the auxiliary points, the polytope is never degenerated and thus there is always one unique halfspace-representation which is necessary:
                // Consider, e.g., the convex hull of two points (1,0,0) and (0,1,1).
                // There are multiple halfspace-representations for this set. In particular, there is one where all but one normalVectors have negative entries.
                // However, the downward closure of this set can only be represented with 5 halfspaces.
                for(auto& h : auxiliaryHalfspaces){
                    bool allGreaterZero = true;
                    for(auto const& value : h.normalVector()) {
                        allGreaterZero &= (value >= storm::utility::zero<ValueType>());
                    }
                    if(allGreaterZero){
                        halfspaces.push_back(std::move(h));
                    }
                }
                return create(halfspaces);
            }
            
            template <typename ValueType>
            Polytope<ValueType>::Polytope() {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            Polytope<ValueType>::~Polytope() {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            std::vector<typename Polytope<ValueType>::Point> Polytope<ValueType>::getVertices() const{
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return std::vector<Point>();
            }
            
            template <typename ValueType>
            std::vector<Halfspace<ValueType>> Polytope<ValueType>::getHalfspaces() const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return std::vector<Halfspace<ValueType>>();
            }
            
            template <typename ValueType>
            bool Polytope<ValueType>::isEmpty() const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return false;
            }
            
            template <typename ValueType>
            bool Polytope<ValueType>::isUniversal() const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return false;
            }
            
            template <typename ValueType>
            bool Polytope<ValueType>::contains(Point const& point) const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return false;
            }
            
            template <typename ValueType>
            bool Polytope<ValueType>::contains(std::shared_ptr<Polytope<ValueType>> const& other) const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return false;
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::intersection(std::shared_ptr<Polytope<ValueType>> const& rhs) const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return nullptr;
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::intersection(Halfspace<ValueType> const& halfspace) const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return nullptr;
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::convexUnion(std::shared_ptr<Polytope<ValueType>> const& rhs) const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return nullptr;
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::minkowskiSum(std::shared_ptr<Polytope<ValueType>> const& rhs) const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return nullptr;
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::linearTransformation(std::vector<Point> const& matrix, Point const& vector) const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return nullptr;
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::downwardClosure() const {
                return createDownwardClosure(this->getVertices());
            }
            
            template <typename ValueType>
            std::pair<typename Polytope<ValueType>::Point, bool> Polytope<ValueType>::optimize(Point const& direction) const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return std::make_pair(Point(), false);
            }
            
            template <typename ValueType>
            std::string Polytope<ValueType>::toString(bool numbersAsDouble) const {
                auto halfspaces = this->getHalfspaces();
                std::stringstream stream;
                stream << "Polytope with " << halfspaces.size() << " Halfspaces" << (halfspaces.empty() ? "" : ":") << std::endl;
                for (auto const& h : halfspaces) {
                    stream << "|   " << h.toString(numbersAsDouble) << std::endl;
                }
                return stream.str();
            }
            
            template <typename ValueType>
            bool Polytope<ValueType>::isHyproPolytope() const {
                return false;
            }
            
#ifdef STORM_HAVE_CARL
            template class Polytope<storm::RationalNumber>;
#endif
            template class Polytope<double>;
            // Note that HyproPolytopes only support exact arithmetic
        }
    }
}
