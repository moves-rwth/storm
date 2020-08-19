#include "storm/storage/prism/Player.h"

namespace storm {
    namespace prism {
        Player::Player(std::string const& playerName, std::map<std::string, uint_fast32_t> const& controlledModules, std::map<std::string, uint_fast32_t> const& controlledCommands, std::string const& filename, uint_fast32_t lineNumber) : LocatedInformation(filename, lineNumber), playerName(playerName), controlledModules(controlledModules), controlledCommands(controlledCommands) {
            // Nothing to do here.
        }

        std::string const& Player::getName() const {
            return this->playerName;
        }

        std::map<std::string, uint_fast32_t> const& Player::getModules() const {
            return this->controlledModules;
        }

        std::map<std::string, uint_fast32_t> const& Player::getCommands() const {
            return this->controlledCommands;
        }

        std::ostream& operator<<(std::ostream& stream, Player const& player) {
            stream << "player";
            if (player.getName() != "") {
                stream << " " << player.getName();
            }
            stream << std::endl;
            for (auto const& module : player.getModules()) {
                stream << "\t" << module.first << std::endl;
            }
            for (auto const& command : player.getCommands()) {
                stream << "\t[" << command.first << "]" << std::endl;
            }
            stream << "endplayer" << std::endl;
            return stream;
        }
    } // namespace prism
} // namespace storm
