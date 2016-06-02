#include "src/storage/geometry/HyproPolytope.h"
#
#ifdef STORM_HAVE_HYPRO

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace storage {
        namespace geometry {
            
            template <typename ValueType>
            HyproPolytope<ValueType>::HyproPolytope(std::vector<Halfspace<ValueType>> const& halfspaces) {
                if(halfspaces.empty()){
                    internPolytope = HyproPolytopeType();
                } else {
                    std::vector<hypro::Halfspace<ValueType>> hyproHalfspaces;
                    hyproHalfspaces.reserve(halfspaces.size());
                    for(auto& h : halfspaces) {
                        hyproHalfspaces.emplace_back(storm::adapters::toHypro(h));
                    }
                    internPolytope = HyproPolytopeType(std::move(hyproHalfspaces));
                }
            }
            
            template <typename ValueType>
            HyproPolytope<ValueType>::HyproPolytope(std::vector<Point> const& points) {
                if(points.empty()){
                    internPolytope = HyproPolytopeType::Empty();
                } else {
                    std::vector<hypro::Point<ValueType>> hyproPoints;
                    hyproPoints.reserve(points.size());
                    for(auto const& p : points){
                        hyproPoints.emplace_back(storm::adapters::toHypro(p));
                    }
                    internPolytope = HyproPolytopeType(std::move(hyproPoints));
                }
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::create(boost::optional<std::vector<Halfspace<ValueType>>> const& halfspaces,
                                                                             boost::optional<std::vector<Point>> const& points) {
                if(halfspaces) {
                    STORM_LOG_WARN_COND(!points, "Creating a HyproPolytope where halfspaces AND points are given. The points will be ignored.");
                    return std::make_shared<HyproPolytope<ValueType>>(*halfspaces);
                } else if(points) {
                    return std::make_shared<HyproPolytope<ValueType>>(*points);
                }
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Creating a HyproPolytope but no representation was given.");
                return nullptr;
            }
            
            template <typename ValueType>
            HyproPolytope<ValueType>::HyproPolytope(HyproPolytope<ValueType> const& other) : internPolytope(other.internPolytope) {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            HyproPolytope<ValueType>::HyproPolytope(HyproPolytope<ValueType>&& other) : internPolytope(std::move(other.internPolytope)) {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            HyproPolytope<ValueType>::HyproPolytope(HyproPolytopeType const& p) : internPolytope(p) {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            HyproPolytope<ValueType>::HyproPolytope(HyproPolytopeType&& p) : internPolytope(p) {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            HyproPolytope<ValueType>::~HyproPolytope() {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            std::vector<typename Polytope<ValueType>::Point> HyproPolytope<ValueType>::getVertices() const {
                std::vector<hypro::Point<ValueType>> hyproVertices = internPolytope.vertices();
                std::vector<Point> result;
                result.reserve(hyproVertices.size());
                for(auto const& p : hyproVertices) {
                    result.push_back(storm::adapters::fromHypro(p.rawCoordinates()));
                }
                return result;
            }
            
            template <typename ValueType>
            std::vector<Halfspace<ValueType>> HyproPolytope<ValueType>::getHalfspaces() const {
                std::vector<hypro::Halfspace<ValueType>> hyproHalfspaces = internPolytope.constraints();
                std::vector<Halfspace<ValueType>> result;
                result.reserve(hyproHalfspaces.size());
                for(auto const& h : hyproHalfspaces) {
                    result.push_back(storm::adapters::fromHypro(h));
                }
                return result;
            }
            
            template <typename ValueType>
            bool HyproPolytope<ValueType>::isEmpty() const {
                return internPolytope.empty();
            }
            
            template <typename ValueType>
            bool HyproPolytope<ValueType>::isUniversal() const {
                return internPolytope.constraints().empty();
            }
            
            template <typename ValueType>
            bool HyproPolytope<ValueType>::contains(Point const& point) const{
                return internPolytope.contains(storm::adapters::toHypro(point));
            }
            
            template <typename ValueType>
            bool HyproPolytope<ValueType>::contains(std::shared_ptr<Polytope<ValueType>> const& other) const {
                STORM_LOG_THROW(other->isHyproPolytope(), storm::exceptions::InvalidArgumentException, "Invoked operation between a HyproPolytope and a different polytope implementation. This is not supported");
                return internPolytope.contains(dynamic_cast<HyproPolytope<ValueType> const&>(*other).internPolytope);
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::intersection(std::shared_ptr<Polytope<ValueType>> const& rhs) const{
                STORM_LOG_THROW(rhs->isHyproPolytope(), storm::exceptions::InvalidArgumentException, "Invoked operation between a HyproPolytope and a different polytope implementation. This is not supported");
                return std::make_shared<HyproPolytope<ValueType>>(internPolytope.intersect(dynamic_cast<HyproPolytope<ValueType> const&>(*rhs).internPolytope));
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::intersection(Halfspace<ValueType> const& halfspace) const{
                HyproPolytopeType result(internPolytope);
                result.insert(storm::adapters::toHypro(halfspace));
                return std::make_shared<HyproPolytope<ValueType>>(std::move(result));
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::convexUnion(std::shared_ptr<Polytope<ValueType>> const& rhs) const{
                STORM_LOG_THROW(rhs->isHyproPolytope(), storm::exceptions::InvalidArgumentException, "Invoked operation between a HyproPolytope and a different polytope implementation. This is not supported");
                return std::make_shared<HyproPolytope<ValueType>>(internPolytope.unite(dynamic_cast<HyproPolytope<ValueType> const&>(*rhs).internPolytope));
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::minkowskiSum(std::shared_ptr<Polytope<ValueType>> const& rhs) const{
                STORM_LOG_THROW(rhs->isHyproPolytope(), storm::exceptions::InvalidArgumentException, "Invoked operation between a HyproPolytope and a different polytope implementation. This is not supported");
                return std::make_shared<HyproPolytope<ValueType>>(internPolytope.minkowskiSum(dynamic_cast<HyproPolytope<ValueType> const&>(*rhs).internPolytope));
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::downwardClosure(boost::optional<Point> const& upperBounds) const {
                if(this->isUniversal() || this->isEmpty()) {
                    // In these cases, the polytope does not change, i.e., we return a copy of this
                    return std::make_shared<HyproPolytope<ValueType>>(internPolytope);
                }
                // Only keep the halfspaces where all entries of the normal vector are non-negative
                std::vector<hypro::Halfspace<ValueType>> halfspaces;
                for(auto& h : internPolytope.constraints()){
                    if((h.normal().array() >= storm::utility::zero<ValueType>()).all()){
                        halfspaces.push_back(h);
                    }
                }
                // Add Halfspaces to bound the polytope in each (positive) direction
                for(uint_fast64_t dim = 0; dim < internPolytope.dimension(); ++dim){
                    hypro::vector_t<ValueType> direction = hypro::vector_t<ValueType>::Zero(internPolytope.dimension());
                    direction(dim) = storm::utility::one<ValueType>();
                    if(upperBounds){
                        ValueType upperBound = (*upperBounds)[dim];
                        halfspaces.emplace_back(std::move(direction), std::move(upperBound));
                    } else {
                        // Compute max{x_dim | x \in P }
                        hypro::EvaluationResult<ValueType> evalRes = internPolytope.evaluate(direction);
                        switch (evalRes.errorCode) {
                            case hypro::SOLUTION::FEAS:
                                halfspaces.emplace_back(std::move(direction), std::move(evalRes.supportValue));
                                break;
                            case hypro::SOLUTION::INFTY:
                                // if this polytope is not bounded in the current direction, no halfspace needs to be added
                                break;
                            default:
                                // there should never be an infeasible solution as we already excluded the empty polytope.
                                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected eror code for Polytope evaluation");
                                break;
                        }
                    }
                }
                return std::make_shared<HyproPolytope<ValueType>>(HyproPolytopeType(std::move(halfspaces)));
            }
            
            template <typename ValueType>
            bool HyproPolytope<ValueType>::isHyproPolytope() const {
                return true;
            }
            
#ifdef STORM_HAVE_CARL
            template class HyproPolytope<storm::RationalNumber>;
#endif
            // Note that hypro's polytopes only support exact arithmetic
        }
    }
}
#endif
