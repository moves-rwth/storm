#include "Update.h"

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <storm/exceptions/IllegalArgumentException.h>
#include "storm/adapters/RationalNumberAdapter.h"

#include "storm/exceptions/OutOfRangeException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace prism {
Update::Update(uint_fast64_t globalIndex, storm::expressions::Expression const& likelihoodExpression, std::vector<storm::prism::Assignment> const& assignments,
               std::string const& filename, uint_fast64_t lineNumber)
    : LocatedInformation(filename, lineNumber),
      likelihoodExpression(likelihoodExpression),
      assignments(assignments),
      variableToAssignmentIndexMap(),
      globalIndex(globalIndex) {
    STORM_LOG_ASSERT(likelihoodExpression.containsVariables() || likelihoodExpression.evaluateAsRational() >= 0,
                     "Negative likelihood expressions are not allowed.");
    std::sort(this->assignments.begin(), this->assignments.end(), [](storm::prism::Assignment const& assignment1, storm::prism::Assignment const& assignment2) {
        bool smaller = assignment1.getVariable().getType().isBooleanType() && !assignment2.getVariable().getType().isBooleanType();
        if (!smaller) {
            smaller = assignment1.getVariable() < assignment2.getVariable();
        }
        return smaller;
    });
    this->createAssignmentMapping();
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

std::vector<storm::prism::Assignment>& Update::getAssignments() {
    return this->assignments;
}

storm::prism::Assignment const& Update::getAssignment(std::string const& variableName) const {
    auto const& variableIndexPair = this->variableToAssignmentIndexMap.find(variableName);
    STORM_LOG_THROW(variableIndexPair != this->variableToAssignmentIndexMap.end(), storm::exceptions::OutOfRangeException,
                    "Variable '" << variableName << "' is not assigned in update.");
    return this->getAssignments()[variableIndexPair->second];
}

std::map<storm::expressions::Variable, storm::expressions::Expression> Update::getAsVariableToExpressionMap() const {
    std::map<storm::expressions::Variable, storm::expressions::Expression> result;

    for (auto const& assignment : this->getAssignments()) {
        result.emplace(assignment.getVariable(), assignment.getExpression());
    }

    return result;
}

uint_fast64_t Update::getGlobalIndex() const {
    return this->globalIndex;
}

void Update::createAssignmentMapping() {
    this->variableToAssignmentIndexMap.clear();
    for (uint_fast64_t assignmentIndex = 0; assignmentIndex < this->getAssignments().size(); ++assignmentIndex) {
        this->variableToAssignmentIndexMap[this->getAssignments()[assignmentIndex].getVariableName()] = assignmentIndex;
    }
}

Update Update::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
    std::vector<Assignment> newAssignments;
    newAssignments.reserve(this->getNumberOfAssignments());
    for (auto const& assignment : this->getAssignments()) {
        newAssignments.emplace_back(assignment.substitute(substitution));
    }
    auto newLikelihoodExpression = this->getLikelihoodExpression().substitute(substitution);
    STORM_LOG_THROW(likelihoodExpression.containsVariables() || likelihoodExpression.evaluateAsRational() >= 0, storm::exceptions::IllegalArgumentException,
                    "Substitution yielding negative probabilities in '" << this->getLikelihoodExpression() << "' are not allowed.");
    // FIXME: The expression could be simplified, but 1/K (where K is an int) is then resolved to 0, which is incorrect (for probabilities).
    return Update(this->getGlobalIndex(), newLikelihoodExpression, newAssignments, this->getFilename(), this->getLineNumber());
}

Update Update::substituteNonStandardPredicates() const {
    std::vector<Assignment> newAssignments;
    newAssignments.reserve(this->getNumberOfAssignments());
    for (auto const& assignment : this->getAssignments()) {
        newAssignments.emplace_back(assignment.substituteNonStandardPredicates());
    }
    auto newLikelihoodExpression = this->getLikelihoodExpression().substituteNonStandardPredicates();
    STORM_LOG_THROW(likelihoodExpression.containsVariables() || likelihoodExpression.evaluateAsRational() >= 0, storm::exceptions::IllegalArgumentException,
                    "Substitution yielding negative probabilities in '" << this->getLikelihoodExpression() << "' are not allowed.");
    return Update(this->getGlobalIndex(), newLikelihoodExpression, newAssignments, this->getFilename(), this->getLineNumber());
}

Update Update::removeIdentityAssignments() const {
    std::vector<Assignment> newAssignments;
    newAssignments.reserve(getNumberOfAssignments());
    for (auto const& ass : this->assignments) {
        if (!ass.isIdentity()) {
            newAssignments.push_back(ass);
        }
    }
    return Update(this->globalIndex, this->likelihoodExpression, newAssignments, getFilename(), getLineNumber());
}

Update Update::simplify() const {
    std::vector<Assignment> newAssignments;
    newAssignments.reserve(getNumberOfAssignments());
    for (auto const& ass : this->assignments) {
        Assignment newAssignment(ass.getVariable(), ass.getExpression().simplify().reduceNesting(), ass.getFilename(), ass.getLineNumber());
        if (!newAssignment.isIdentity()) {
            newAssignments.push_back(std::move(newAssignment));
        }
    }
    return Update(this->globalIndex, this->likelihoodExpression.simplify().reduceNesting(), newAssignments, getFilename(), getLineNumber());
}

std::ostream& operator<<(std::ostream& stream, Update const& update) {
    stream << update.getLikelihoodExpression() << " : ";
    if (update.getAssignments().empty()) {
        stream << "true";
    } else {
        std::vector<std::string> assignmentsAsStrings;
        std::for_each(update.getAssignments().cbegin(), update.getAssignments().cend(), [&assignmentsAsStrings](Assignment const& assignment) {
            std::stringstream stream;
            stream << assignment;
            assignmentsAsStrings.push_back(stream.str());
        });
        stream << boost::algorithm::join(assignmentsAsStrings, " & ");
    }
    return stream;
}

}  // namespace prism
}  // namespace storm
