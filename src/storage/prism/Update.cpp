#include "Update.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/OutOfRangeException.h"

namespace storm {
    namespace prism {
        Update::Update(uint_fast64_t globalIndex, storm::expressions::Expression const& likelihoodExpression, std::vector<storm::prism::Assignment> const& assignments, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), likelihoodExpression(likelihoodExpression), assignments(assignments), variableToAssignmentIndexMap(), globalIndex(globalIndex) {
            // FIXME: construct the mapping from variable names to assignments and check for uniqueness.
        }
        
        Update::Update(Update const& update, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), likelihoodExpression(update.getLikelihoodExpression().substitute<std::map>(renaming)), assignments(), variableToAssignmentIndexMap(), globalIndex(newGlobalIndex) {
            for (auto const& assignment : update.getAssignments()) {
                auto const& namePair = renaming.find(assignment.getVariableName());
                if (namePair != renaming.end()) {
                    this->assignments.emplace_back(Assignment(assignment, renaming, filename, lineNumber));
                } else {
                    this->assignments.emplace_back(Assignment(assignment));
                }
            }
            // FIXME: construct the mapping from variable names to assignments and check for uniqueness.
            this->likelihoodExpression = update.getLikelihoodExpression().substitute<std::map>(renaming);
        }
        
        storm::expressions::Expression const& Update::getLikelihoodExpression() const {
            return likelihoodExpression;
        }
        
        std::size_t Update::getNumberOfAssignments() const {
            return this->assignments.size();
        }
        
        std::vector<storm::prism::Assignment> const& Update::getAssignments() const {
            return this->assignments;
        }
        
        storm::prism::Assignment const& Update::getAssignment(std::string const& variableName) const {
            auto const& variableIndexPair = this->variableToAssignmentIndexMap.find(variableName);
            LOG_THROW(variableIndexPair != this->variableToAssignmentIndexMap.end(), storm::exceptions::OutOfRangeException, "Variable '" << variableName << "' is not assigned in update.");
            return this->getAssignments()[variableIndexPair->second];
        }
        
        uint_fast64_t Update::getGlobalIndex() const {
            return this->globalIndex;
        }
        
        std::ostream& operator<<(std::ostream& stream, Update const& update) {
            stream << update.getLikelihoodExpression() << " : ";
            uint_fast64_t i = 0;
            for (auto const& assignment : update.getAssignments()) {
                stream << assignment;
                if (i < update.getAssignments().size() - 1) {
                    stream << " & ";
                }
                ++i;
            }
            return stream;
        }
        
    } // namespace ir
} // namespace storm
