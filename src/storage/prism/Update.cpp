#include "Update.h"
#include "src/exceptions/OutOfRangeException.h"

namespace storm {
    namespace prism {
        Update::Update(uint_fast64_t globalIndex, storm::expressions::Expression const& likelihoodExpression, std::map<std::string, storm::prism::Assignment> const& booleanAssignments, std::map<std::string, storm::prism::Assignment> const& integerAssignments) : likelihoodExpression(likelihoodExpression), booleanAssignments(booleanAssignments), integerAssignments(integerAssignments), globalIndex(globalIndex) {
            // Nothing to do here.
        }
        
        Update::Update(Update const& update, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming) : likelihoodExpression(update.getLikelihoodExpression().substitute<std::map>(renaming)), booleanAssignments(), integerAssignments(), globalIndex(newGlobalIndex) {
            for (auto const& variableAssignmentPair : update.getBooleanAssignments()) {
                auto const& namePair = renaming.find(variableAssignmentPair.first);
                if (namePair != renaming.end()) {
                    this->booleanAssignments.emplace(namePair->second, Assignment(variableAssignmentPair.second, renaming));
                } else {
                    this->booleanAssignments.emplace(variableAssignmentPair.first, Assignment(variableAssignmentPair.second, renaming));
                }
            }
            for (auto const& variableAssignmentPair : update.getIntegerAssignments()) {
                auto const& namePair = renaming.find(variableAssignmentPair.first);
                if (renaming.count(variableAssignmentPair.first) > 0) {
                    this->integerAssignments.emplace(namePair->second, Assignment(variableAssignmentPair.second, renaming));
                } else {
                    this->integerAssignments.emplace(variableAssignmentPair.first, Assignment(variableAssignmentPair.second, renaming));
                }
            }
            this->likelihoodExpression = update.getLikelihoodExpression().substitute<std::map>(renaming);
        }
        
        storm::expressions::Expression const& Update::getLikelihoodExpression() const {
            return likelihoodExpression;
        }
        
        std::size_t Update::getNumberOfBooleanAssignments() const {
            return booleanAssignments.size();
        }
        
        std::size_t Update::getNumberOfIntegerAssignments() const {
            return integerAssignments.size();
        }
        
        std::map<std::string, storm::prism::Assignment> const& Update::getBooleanAssignments() const {
            return booleanAssignments;
        }
        
        std::map<std::string, storm::prism::Assignment> const& Update::getIntegerAssignments() const {
            return integerAssignments;
        }
        
        storm::prism::Assignment const& Update::getBooleanAssignment(std::string const& variableName) const {
            auto variableAssignmentPair = booleanAssignments.find(variableName);
            if (variableAssignmentPair == booleanAssignments.end()) {
                throw storm::exceptions::OutOfRangeException() << "Cannot find boolean assignment for variable '" << variableName << "' in update " << *this << ".";
            }
            
            return variableAssignmentPair->second;
        }
        
        storm::prism::Assignment const& Update::getIntegerAssignment(std::string const& variableName) const {
            auto variableAssignmentPair = integerAssignments.find(variableName);
            if (variableAssignmentPair == integerAssignments.end()) {
                throw storm::exceptions::OutOfRangeException() << "Cannot find integer assignment for variable '" << variableName << "' in update " << *this << ".";
            }
            
            return variableAssignmentPair->second;
        }
        
        uint_fast64_t Update::getGlobalIndex() const {
            return this->globalIndex;
        }
        
        std::ostream& operator<<(std::ostream& stream, Update const& update) {
            stream << update.getLikelihoodExpression() << " : ";
            uint_fast64_t i = 0;
            for (auto const& assignment : update.getBooleanAssignments()) {
                stream << assignment.second;
                if (i < update.getBooleanAssignments().size() - 1 || update.getIntegerAssignments().size() > 0) {
                    stream << " & ";
                }
                ++i;
            }
            i = 0;
            for (auto const& assignment : update.getIntegerAssignments()) {
                stream << assignment.second;
                if (i < update.getIntegerAssignments().size() - 1) {
                    stream << " & ";
                }
                ++i;
            }
            return stream;
        }
        
    } // namespace ir
} // namespace storm
