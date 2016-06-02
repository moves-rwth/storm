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
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::downwardClosure(boost::optional<Point> const& upperBounds) const {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
                return nullptr;
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
