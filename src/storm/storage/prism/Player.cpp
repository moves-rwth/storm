#include "storm/storage/prism/Player.h"

namespace storm {
    namespace prism {
        Player::Player(std::string const& playerName, std::unordered_set<std::string> const& controlledModules, std::unordered_set<std::string> const& controlledActions, std::string const& filename, uint_fast32_t lineNumber) : LocatedInformation(filename, lineNumber), playerName(playerName), controlledModules(controlledModules), controlledActions(controlledActions) {
            // Nothing to do here.
        }

        std::string const& Player::getName() const {
            return this->playerName;
        }

        std::unordered_set<std::string> const& Player::getModules() const {
            return this->controlledModules;
        }

        std::unordered_set<std::string> const& Player::getActions() const {
            return this->controlledActions;
        }

        std::ostream& operator<<(std::ostream& stream, Player const& player) {
            stream << "player";
            if (player.getName() != "") {
                stream << " " << player.getName();
            }
            stream << std::endl;
            for (auto const& module : player.getModules()) {
                stream << "\t" << module << std::endl;
            }
            for (auto const& action : player.getActions()) {
                stream << "\t[" << action << "]" << std::endl;
            }
            stream << "endplayer" << std::endl;
            return stream;
        }
    } // namespace prism
} // namespace storm
