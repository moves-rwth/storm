#ifndef STORM_STORAGE_PRISM_COMMAND_H_
#define STORM_STORAGE_PRISM_COMMAND_H_

#include <vector>
#include <string>
#include <map>

#include "src/storage/expressions/Expression.h"
#include "src/storage/prism/Update.h"

namespace storm {
    namespace prism {
        class Command : public LocatedInformation {
        public:
            /*!
             * Creates a command with the given action name, guard and updates.
             *
             * @param globalIndex The global index of the command.
             * @param actionName The action name of the command.
             * @param guardExpression the expression that defines the guard of the command.
             * @param updates A list of updates that is associated with this command.
             * @param filename The filename in which the command is defined.
             * @param lineNumber The line number in which the command is defined.
             */
            Command(uint_fast64_t globalIndex, std::string const& actionName, storm::expressions::Expression const& guardExpression, std::vector<storm::prism::Update> const& updates, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            /*!
             * Creates a copy of the given command and performs the provided renaming.
             *
             * @param oldCommand The command to copy.
             * @param newGlobalIndex The global index of the copy of the command.
             * @param renaming A mapping from names that are to be renamed to the names they are to be replaced with.
             * @param filename The filename in which the command is defined.
             * @param lineNumber The line number in which the command is defined.
             */
            Command(Command const& oldCommand, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            // Create default implementations of constructors/assignment.
            Command() = default;
            Command(Command const& other) = default;
            Command& operator=(Command const& other)= default;
            Command(Command&& other) = default;
            Command& operator=(Command&& other) = default;
            
            /*!
             * Retrieves the action name of this command.
             *
             * @return The action name of this command.
             */
            std::string const& getActionName() const;
            
            /*!
             * Retrieves a reference to the guard of the command.
             *
             * @return A reference to the guard of the command.
             */
            storm::expressions::Expression const& getGuardExpression() const;
            
            /*!
             * Retrieves the number of updates associated with this command.
             *
             * @return The number of updates associated with this command.
             */
            std::size_t getNumberOfUpdates() const;
            
            /*!
             * Retrieves a reference to the update with the given index.
             *
             * @return A reference to the update with the given index.
             */
            storm::prism::Update const& getUpdate(uint_fast64_t index) const;
            
            /*!
             * Retrieves a vector of all updates associated with this command.
             *
             * @return A vector of updates associated with this command.
             */
            std::vector<storm::prism::Update> const& getUpdates() const;
            
            /*!
             * Retrieves the global index of the command, that is, a unique index over all modules.
             *
             * @return The global index of the command.
             */
            uint_fast64_t getGlobalIndex() const;
            
            friend std::ostream& operator<<(std::ostream& stream, Command const& command);
            
        private:
            // The name of the command.
            std::string actionName;
            
            // The expression that defines the guard of the command.
            storm::expressions::Expression guardExpression;
            
            // The list of updates of the command.
            std::vector<storm::prism::Update> updates;
            
            // The global index of the command.
            uint_fast64_t globalIndex;
        };
        
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_COMMAND_H_ */
