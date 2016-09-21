#include "src/storage/jani/OrderedAssignments.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        OrderedAssignments::OrderedAssignments(std::vector<Assignment> const& assignments) {
            for (auto const& assignment : assignments) {
                add(assignment);
            }
        }
        
        OrderedAssignments::OrderedAssignments(Assignment const& assignment) {
            add(assignment);
        }
        
        bool OrderedAssignments::add(Assignment const& assignment) {
            // If the element is contained in this set of assignment, nothing needs to be added.
            if (this->contains(assignment)) {
                return false;
            }
            
            // Otherwise, we find the spot to insert it.
            auto it = lowerBound(assignment, allAssignments);

            if (it != allAssignments.end()) {
                STORM_LOG_THROW(assignment.getExpressionVariable() != (*it)->getExpressionVariable(), storm::exceptions::InvalidArgumentException, "Cannot add assignment as an assignment to this variable already exists.");
            }
            
            // Finally, insert the new element in the correct vectors.
            auto elementToInsert = std::make_shared<Assignment>(assignment);
            allAssignments.emplace(it, elementToInsert);
            if (assignment.isTransient()) {
                auto transientIt = lowerBound(assignment, transientAssignments);
                transientAssignments.emplace(transientIt, elementToInsert);
            } else {
                auto nonTransientIt = lowerBound(assignment, nonTransientAssignments);
                nonTransientAssignments.emplace(nonTransientIt, elementToInsert);
            }
            
            return true;
        }
        
        bool OrderedAssignments::remove(Assignment const& assignment) {
            // If the element is contained in this set of assignment, nothing needs to be removed.
            if (!this->contains(assignment)) {
                return false;
            }
            
            // Otherwise, we find the element to delete.
            auto it = lowerBound(assignment, allAssignments);
            STORM_LOG_ASSERT(it != allAssignments.end(), "Invalid iterator, expected existing element.");
            STORM_LOG_ASSERT(assignment == **it, "Wrong iterator position.");
            allAssignments.erase(it);
            
            if (assignment.isTransient()) {
                auto transientIt = lowerBound(assignment, transientAssignments);
                STORM_LOG_ASSERT(transientIt != transientAssignments.end(), "Invalid iterator, expected existing element.");
                STORM_LOG_ASSERT(assignment == **transientIt, "Wrong iterator position.");
                transientAssignments.erase(transientIt);
            } else {
                auto nonTransientIt = lowerBound(assignment, nonTransientAssignments);
                STORM_LOG_ASSERT(nonTransientIt != nonTransientAssignments.end(), "Invalid iterator, expected existing element.");
                STORM_LOG_ASSERT(assignment == **nonTransientIt, "Wrong iterator position.");
                nonTransientAssignments.erase(nonTransientIt);
            }
            return true;
        }
        
        bool OrderedAssignments::hasMultipleLevels() const {
            if(allAssignments.empty()) {
                return false;
            }
            uint64_t firstLevel = allAssignments.front()->getLevel();
            for(auto const& assignment : allAssignments) {
                if(assignment->getLevel() != firstLevel) {
                    return true;
                }
            }
            return false;
        }
        
        bool OrderedAssignments::contains(Assignment const& assignment) const {
            auto it = lowerBound(assignment, allAssignments);
            if (it != allAssignments.end() && assignment == **it) {
                return true;
            } else {
                return false;
            }
        }
        
        detail::ConstAssignments OrderedAssignments::getAllAssignments() const {
            return detail::ConstAssignments(allAssignments.begin(), allAssignments.end());
        }
        
        detail::ConstAssignments OrderedAssignments::getTransientAssignments() const {
            return detail::ConstAssignments(transientAssignments.begin(), transientAssignments.end());
        }
        
        detail::ConstAssignments OrderedAssignments::getNonTransientAssignments() const {
            return detail::ConstAssignments(nonTransientAssignments.begin(), nonTransientAssignments.end());
        }
        
        detail::Assignments::iterator OrderedAssignments::begin() {
            return detail::Assignments::make_iterator(allAssignments.begin());
        }
        
        detail::ConstAssignments::iterator OrderedAssignments::begin() const {
            return detail::ConstAssignments::make_iterator(allAssignments.begin());
        }
        
        detail::Assignments::iterator OrderedAssignments::end() {
            return detail::Assignments::make_iterator(allAssignments.end());
        }
        
        detail::ConstAssignments::iterator OrderedAssignments::end() const {
            return detail::ConstAssignments::make_iterator(allAssignments.end());
        }
        
        void OrderedAssignments::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            for (auto& assignment : allAssignments) {
                assignment->substitute(substitution);
            }
        }
        
        std::vector<std::shared_ptr<Assignment>>::const_iterator OrderedAssignments::lowerBound(Assignment const& assignment, std::vector<std::shared_ptr<Assignment>> const& assignments) {
            return std::lower_bound(assignments.begin(), assignments.end(), assignment, storm::jani::AssignmentPartialOrderByVariable());
        }
        
    }
}