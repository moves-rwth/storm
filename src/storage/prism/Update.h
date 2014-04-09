#ifndef STORM_STORAGE_PRISM_UPDATE_H_
#define STORM_STORAGE_PRISM_UPDATE_H_

#include <vector>

#include "src/storage/prism/LocatedInformation.h"
#include "src/storage/prism/Assignment.h"

namespace storm {
    namespace prism {
        class Update : public LocatedInformation {
        public:
            /*!
             * Creates an update with the given expression specifying the likelihood and assignments.
             *
             * @param globalIndex The global index of the update.
             * @param likelihoodExpression An expression specifying the likelihood of this update.
             * @param assignments A assignments to variables.
             * @param filename The filename in which the update is defined.
             * @param lineNumber The line number in which the update is defined.
             */
            Update(uint_fast64_t globalIndex, storm::expressions::Expression const& likelihoodExpression, std::vector<storm::prism::Assignment> const& assignments, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            /*!
             * Creates a copy of the given update and performs the provided renaming.
             *
             * @param update The update that is to be copied.
             * @param newGlobalIndex The global index of the resulting update.
             * @param renaming A mapping from names that are to be renamed to the names they are to be replaced with.
             * @param filename The filename in which the update is defined.
             * @param lineNumber The line number in which the update is defined.
             */
            Update(Update const& update, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            // Create default implementations of constructors/assignment.
            Update() = default;
            Update(Update const& other) = default;
            Update& operator=(Update const& other)= default;
            Update(Update&& other) = default;
            Update& operator=(Update&& other) = default;
            
            /*!
             * Retrieves the expression for the likelihood of this update.
             *
             * @return The expression for the likelihood of this update.
             */
            storm::expressions::Expression const& getLikelihoodExpression() const;
            
            /*!
             * Retrieves the number of assignments associated with this update.
             *
             * @return The number of assignments associated with this update.
             */
            std::size_t getNumberOfAssignments() const;
            
            /*!
             * Retrieves a reference to the map of variable names to their respective assignments.
             *
             * @return A reference to the map of variable names to their respective assignments.
             */
            std::vector<storm::prism::Assignment> const& getAssignments() const;
            
            /*!
             * Retrieves a reference to the assignment for the variable with the given name.
             *
             * @return A reference to the assignment for the variable with the given name.
             */
            storm::prism::Assignment const& getAssignment(std::string const& variableName) const;
            
            /*!
             * Retrieves the global index of the update, that is, a unique index over all modules.
             *
             * @return The global index of the update.
             */
            uint_fast64_t getGlobalIndex() const;
            
            friend std::ostream& operator<<(std::ostream& stream, Update const& assignment);
            
        private:
            // An expression specifying the likelihood of taking this update.
            storm::expressions::Expression likelihoodExpression;
            
            // The assignments of this update.
            std::vector<storm::prism::Assignment> assignments;
            
            // A mapping from variable names to their assignments.
            std::map<std::string, uint_fast64_t> variableToAssignmentIndexMap;
            
            // The global index of the update.
            uint_fast64_t globalIndex;
        };
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_UPDATE_H_ */
