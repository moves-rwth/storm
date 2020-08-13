#include "storm/storage/prism/Player.h"

namespace storm {
    namespace prism {
        Player::Player(std::string const& playerName, std::vector<storm::prism::Module> const& controlledModules, std::vector<storm::prism::Command> const& controlledCommands, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), playerName(playerName), controlledModules(controlledModules), controlledCommands(controlledCommands) {
            // Nothing to do here.
        }

        std::string const& Player::getName() const {
            return this->playerName;
        }

        std::vector<storm::prism::Module> const& Player::getModules() const {
            return this->controlledModules;
        }

        std::vector<storm::prism::Command> const& Player::getCommands() const {
            return this->controlledCommands;
        }

        std::ostream& operator<<(std::ostream& stream, Player const& player) {
            stream << "player";
            if (player.getName() != "") {
                stream << " " << player.getName();
            }
            stream << std::endl;
            for (auto const& module : player.getModules()) {
                stream << module.getName() << " ";
                //&module != (player.getModules()).back ? std::cout << "," : std::cout << std::endl;
            }
            stream << std::endl;
            for (auto const& command : player.getCommands()) {
                stream << "[" << command.getActionName() << "] ";
                //&command != (player.getCommands()).back ? std::cout << "," : std::cout << std::endl;
            }
            stream << std::endl;
            stream << "endplayer" << std::endl;
            return stream;
        }
    } // namespace prism
} // namespace storm
