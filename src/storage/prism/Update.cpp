#include "Update.h"
#include "src/exceptions/OutOfRangeException.h"

namespace storm {
    namespace prism {
        Update::Update(uint_fast64_t globalIndex, storm::expressions::Expression const& likelihoodExpression, std::map<std::string, storm::prism::Assignment> const& assignments, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), likelihoodExpression(likelihoodExpression), assignments(assignments), globalIndex(globalIndex) {
            // Nothing to do here.
        }
        
        Update::Update(Update const& update, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), likelihoodExpression(update.getLikelihoodExpression().substitute<std::map>(renaming)), assignments(), globalIndex(newGlobalIndex) {
            for (auto const& variableAssignmentPair : update.getAssignments()) {
                auto const& namePair = renaming.find(variableAssignmentPair.first);
                if (namePair != renaming.end()) {
                    this->assignments.emplace(namePair->second, Assignment(variableAssignmentPair.second, renaming, filename, lineNumber));
                } else {
                    this->assignments.emplace(variableAssignmentPair.first, Assignment(variableAssignmentPair.second, renaming, filename, lineNumber));
                }
            }
            this->likelihoodExpression = update.getLikelihoodExpression().substitute<std::map>(renaming);
        }
        
        storm::expressions::Expression const& Update::getLikelihoodExpression() const {
            return likelihoodExpression;
        }
        
        std::size_t Update::getNumberOfAssignments() const {
            return this->assignments.size();
        }
        
        std::map<std::string, storm::prism::Assignment> const& Update::getAssignments() const {
            return this->assignments;
        }
        
        storm::prism::Assignment const& Update::getAssignment(std::string const& variableName) const {
            auto variableAssignmentPair = this->getAssignments().find(variableName);
            if (variableAssignmentPair == this->getAssignments().end()) {
                throw storm::exceptions::OutOfRangeException() << "Cannot find assignment for variable '" << variableName << "' in update " << *this << ".";
            }
            
            return variableAssignmentPair->second;
        }
        
        uint_fast64_t Update::getGlobalIndex() const {
            return this->globalIndex;
        }
        
        std::ostream& operator<<(std::ostream& stream, Update const& update) {
            stream << update.getLikelihoodExpression() << " : ";
            uint_fast64_t i = 0;
            for (auto const& assignment : update.getAssignments()) {
                stream << assignment.second;
                if (i < update.getAssignments().size() - 1) {
                    stream << " & ";
                }
                ++i;
            }
            return stream;
        }
        
    } // namespace ir
} // namespace storm
