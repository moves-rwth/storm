#include "storm/storage/geometry/HyproPolytope.h"
#
#ifdef STORM_HAVE_HYPRO

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace storage {
namespace geometry {

template<typename ValueType>
HyproPolytope<ValueType>::HyproPolytope(std::vector<Halfspace<ValueType>> const& halfspaces) {
    if (halfspaces.empty()) {
        internPolytope = HyproPolytopeType();
    } else {
        std::vector<hypro::Halfspace<ValueType>> hyproHalfspaces;
        hyproHalfspaces.reserve(halfspaces.size());
        for (auto& h : halfspaces) {
            hyproHalfspaces.emplace_back(storm::adapters::toHypro(h));
        }
        internPolytope = HyproPolytopeType(std::move(hyproHalfspaces));
    }
}

template<typename ValueType>
HyproPolytope<ValueType>::HyproPolytope(std::vector<Point> const& points) {
    if (points.empty()) {
        internPolytope = HyproPolytopeType::Empty();
    } else {
        std::vector<hypro::Point<ValueType>> hyproPoints;
        hyproPoints.reserve(points.size());
        for (auto const& p : points) {
            hyproPoints.emplace_back(storm::adapters::toHypro(p));
        }
        internPolytope = HyproPolytopeType(std::move(hyproPoints));
    }
}

template<typename ValueType>
std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::create(boost::optional<std::vector<Halfspace<ValueType>>> const& halfspaces,
                                                                      boost::optional<std::vector<Point>> const& points) {
    if (halfspaces) {
        STORM_LOG_WARN_COND(!points, "Creating a HyproPolytope where halfspaces AND points are given. The points will be ignored.");
        return std::make_shared<HyproPolytope<ValueType>>(*halfspaces);
    } else if (points) {
        return std::make_shared<HyproPolytope<ValueType>>(*points);
    }
    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Creating a HyproPolytope but no representation was given.");
    return nullptr;
}

template<typename ValueType>
HyproPolytope<ValueType>::HyproPolytope(HyproPolytope<ValueType> const& other) : internPolytope(other.internPolytope) {
    // Intentionally left empty
}

template<typename ValueType>
HyproPolytope<ValueType>::HyproPolytope(HyproPolytope<ValueType>&& other) : internPolytope(std::move(other.internPolytope)) {
    // Intentionally left empty
}

template<typename ValueType>
HyproPolytope<ValueType>::HyproPolytope(HyproPolytopeType const& p) : internPolytope(p) {
    // Intentionally left empty
}

template<typename ValueType>
HyproPolytope<ValueType>::HyproPolytope(HyproPolytopeType&& p) : internPolytope(p) {
    // Intentionally left empty
}

template<typename ValueType>
HyproPolytope<ValueType>::~HyproPolytope() {
    // Intentionally left empty
}

template<typename ValueType>
std::vector<typename Polytope<ValueType>::Point> HyproPolytope<ValueType>::getVertices() const {
    std::vector<hypro::Point<ValueType>> hyproVertices = internPolytope.vertices();
    std::vector<Point> result;
    result.reserve(hyproVertices.size());
    for (auto const& p : hyproVertices) {
        result.push_back(storm::adapters::fromHypro(p.rawCoordinates()));
    }
    return result;
}

template<typename ValueType>
std::vector<Halfspace<ValueType>> HyproPolytope<ValueType>::getHalfspaces() const {
    std::vector<hypro::Halfspace<ValueType>> hyproHalfspaces = internPolytope.constraints();
    std::vector<Halfspace<ValueType>> result;
    result.reserve(hyproHalfspaces.size());
    for (auto const& h : hyproHalfspaces) {
        result.push_back(storm::adapters::fromHypro(h));
    }
    return result;
}

template<typename ValueType>
bool HyproPolytope<ValueType>::isEmpty() const {
    return internPolytope.empty();
}

template<typename ValueType>
bool HyproPolytope<ValueType>::isUniversal() const {
    return internPolytope.constraints().empty();
}

template<typename ValueType>
bool HyproPolytope<ValueType>::contains(Point const& point) const {
    return internPolytope.contains(storm::adapters::toHypro(point));
}

template<typename ValueType>
bool HyproPolytope<ValueType>::contains(std::shared_ptr<Polytope<ValueType>> const& other) const {
    STORM_LOG_THROW(other->isHyproPolytope(), storm::exceptions::InvalidArgumentException,
                    "Invoked operation between a HyproPolytope and a different polytope implementation. This is not supported");
    return internPolytope.contains(dynamic_cast<HyproPolytope<ValueType> const&>(*other).internPolytope);
}

template<typename ValueType>
std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::intersection(std::shared_ptr<Polytope<ValueType>> const& rhs) const {
    STORM_LOG_THROW(rhs->isHyproPolytope(), storm::exceptions::InvalidArgumentException,
                    "Invoked operation between a HyproPolytope and a different polytope implementation. This is not supported");
    return std::make_shared<HyproPolytope<ValueType>>(internPolytope.intersect(dynamic_cast<HyproPolytope<ValueType> const&>(*rhs).internPolytope));
}

template<typename ValueType>
std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::intersection(Halfspace<ValueType> const& halfspace) const {
    HyproPolytopeType result(internPolytope);
    result.insert(storm::adapters::toHypro(halfspace));
    return std::make_shared<HyproPolytope<ValueType>>(std::move(result));
}

template<typename ValueType>
std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::convexUnion(std::shared_ptr<Polytope<ValueType>> const& rhs) const {
    STORM_LOG_THROW(rhs->isHyproPolytope(), storm::exceptions::InvalidArgumentException,
                    "Invoked operation between a HyproPolytope and a different polytope implementation. This is not supported");
    return std::make_shared<HyproPolytope<ValueType>>(internPolytope.unite(dynamic_cast<HyproPolytope<ValueType> const&>(*rhs).internPolytope));
}

template<typename ValueType>
std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::minkowskiSum(std::shared_ptr<Polytope<ValueType>> const& rhs) const {
    STORM_LOG_THROW(rhs->isHyproPolytope(), storm::exceptions::InvalidArgumentException,
                    "Invoked operation between a HyproPolytope and a different polytope implementation. This is not supported");
    return std::make_shared<HyproPolytope<ValueType>>(internPolytope.minkowskiSum(dynamic_cast<HyproPolytope<ValueType> const&>(*rhs).internPolytope));
}

template<typename ValueType>
std::shared_ptr<Polytope<ValueType>> HyproPolytope<ValueType>::affineTransformation(std::vector<Point> const& matrix, Point const& vector) const {
    STORM_LOG_THROW(!matrix.empty(), storm::exceptions::InvalidArgumentException, "Invoked affine transformation with a matrix without rows.");
    hypro::matrix_t<ValueType> hyproMatrix(matrix.size(), matrix.front().size());
    for (uint_fast64_t row = 0; row < matrix.size(); ++row) {
        hyproMatrix.row(row) = storm::adapters::toHypro(matrix[row]);
    }
    return std::make_shared<HyproPolytope<ValueType>>(internPolytope.affineTransformation(std::move(hyproMatrix), storm::adapters::toHypro(vector)));
}

template<typename ValueType>
std::pair<typename HyproPolytope<ValueType>::Point, bool> HyproPolytope<ValueType>::optimize(Point const& direction) const {
    hypro::EvaluationResult<ValueType> evalRes = internPolytope.evaluate(storm::adapters::toHypro(direction));
    switch (evalRes.errorCode) {
        case hypro::SOLUTION::FEAS:
            return std::make_pair(storm::adapters::fromHypro(evalRes.optimumValue), true);
        case hypro::SOLUTION::UNKNOWN:
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected eror code for Polytope evaluation");
            return std::make_pair(Point(), false);
        default:
            // solution is infinity or infeasible
            return std::make_pair(Point(), false);
    }
}

template<typename ValueType>
bool HyproPolytope<ValueType>::isHyproPolytope() const {
    return true;
}

template class HyproPolytope<double>;
#ifdef STORM_HAVE_CARL
template class HyproPolytope<storm::RationalNumber>;
#endif
}  // namespace geometry
}  // namespace storage
}  // namespace storm
#endif
