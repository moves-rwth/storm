#include "Command.h"

namespace storm {
    namespace prism {
        Command::Command(uint_fast64_t globalIndex, std::string const& actionName, storm::expressions::Expression const& guardExpression, std::vector<storm::prism::Update> const& updates, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), actionName(actionName), guardExpression(guardExpression), updates(updates), globalIndex(globalIndex) {
            // Nothing to do here.
        }

        std::string const& Command::getActionName() const {
            return this->actionName;
        }
        
        storm::expressions::Expression const& Command::getGuardExpression() const {
            return guardExpression;
        }
        
        std::size_t Command::getNumberOfUpdates() const {
            return this->updates.size();
        }
        
        storm::prism::Update const& Command::getUpdate(uint_fast64_t index) const {
            return this->updates[index];
        }
        
        std::vector<storm::prism::Update> const& Command::getUpdates() const {
            return this->updates;
        }
        
        uint_fast64_t Command::getGlobalIndex() const {
            return this->globalIndex;
        }
        
        std::ostream& operator<<(std::ostream& stream, Command const& command) {
            stream << "[" << command.getActionName() << "] " << command.getGuardExpression() << " -> ";
            for (uint_fast64_t i = 0; i < command.getUpdates().size(); ++i) {
                stream << command.getUpdate(i);
                if (i < command.getUpdates().size() - 1) {
                    stream << " + ";
                }
            }
            stream << ";";
            return stream;
        }
    } // namespace ir
} // namespace storm
