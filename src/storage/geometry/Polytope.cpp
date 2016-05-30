#include "src/storage/geometry/Polytope.h"

#include "src/utility/macros.h"
#include "src/adapters/HyproAdapter.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace storage {
        namespace geometry {
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::create(std::vector<storm::storage::geometry::Halfspace<ValueType>> const& halfspaces){
                return create(halfspaces, boost::none, false);
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::create(std::vector<Point> const& points){
                return create(boost::none, points, false);
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::createDownwardClosure(std::vector<Halfspace<ValueType>> const& halfspaces){
                return create(halfspaces, boost::none, false);
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::createDownwardClosure(std::vector<Point> const& points){
                return create(boost::none, points, true);
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::create(boost::optional<std::vector<Halfspace<ValueType>>> const& halfspaces,
                                                                             boost::optional<std::vector<Point>> const& points,
                                                                             bool downwardClosure) {
                
#ifdef STORM_HAVE_HYPRO
                std::cout << "HyPro available!!" << std::endl;
                // return std::make_shared(HyproPolytope<ValueType>(halfspaces, points, downwardClosure));
#endif
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "No polytope implementation specified.");
                return nullptr;
            }
            
            template <typename ValueType>
            std::vector<typename Polytope<ValueType>::Point> Polytope<ValueType>::getVertices(){
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
            }
            
            template <typename ValueType>
            std::vector<Halfspace<ValueType>> Polytope<ValueType>::getHalfspaces(){
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
            }
            
            template <typename ValueType>
            bool Polytope<ValueType>::contains(Point const& point) const{
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::intersect(std::shared_ptr<Polytope<ValueType>> const& rhs) const{
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
            }
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::intersect(Halfspace<ValueType> const& rhs) const{
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::convexUnion(std::shared_ptr<Polytope<ValueType>> const& rhs) const{
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
            }
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::convexUnion(Point const& rhs) const{
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> Polytope<ValueType>::bloat(Point const& point1, Point const& point2) const{
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
            }
            
            
            template <typename ValueType>
            std::string Polytope<ValueType>::toString() const{
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Functionality not implemented.");
            }
            
            template class Polytope<double>;
#ifdef STORM_HAVE_CARL
            template class Polytope<storm::RationalNumber>;
#endif
        }
    }
}
