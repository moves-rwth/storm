#ifndef STORM_STORAGE_PRISM_PLAYER_H_
#define STORM_STORAGE_PRISM_PLAYER_H_

#include <string>
#include <vector>

#include "storm/storage/prism/Module.h"
#include "storm/storage/prism/Command.h"

// needed?
#include "storm/storage/BoostTypes.h"
#include "storm/utility/OsDetection.h"

namespace storm {
    namespace prism {
        class Player : public LocatedInformation {
        public:
            /*!
             * Creates a player with the given name, controlled modules and actions.
             *
             * @param playerName The name of the player.
             * @param controlledModules The controlled modules.
             * @param controlledCommands The controlled actions.
             * @param filename The filename in which the player is defined.
             * @param lineNumber The line number in which the player is defined.
             */
            Player(std::string const& playerName, std::map<std::string, uint_fast32_t> const& controlledModules, std::map<std::string, uint_fast32_t> const& controlledCommands, std::string const& filename = "", uint_fast32_t lineNumber = 0);

            // Create default implementations of constructors/assignment.
            Player() = default;
            Player(Player const& other) = default;
            Player& operator=(Player const& other) = default;
            Player(Player&& other) = default;
            Player& operator=(Player&& other) = default;

            /*!
             * Retrieves the name of the player.
             *
             * @return The name of the player.
             */
            std::string const& getName() const;

            /*!
             * Retrieves all controlled Modules of the player.
             *
             * @return The modules controlled by the player.
             */
            std::map<std::string, uint_fast32_t> const& getModules() const; // TODO

            /*!
             * Retrieves all controlled Commands of the player.
             *
             * @return The commands controlled by the player.
             */
            std::map<std::string, uint_fast32_t> const& getCommands() const;

            friend std::ostream& operator<<(std::ostream& stream, Player const& player);
        private:
            // The name of the player.
            std::string playerName;

            // The modules associated with this player.
            std::map<std::string, uint_fast32_t> controlledModules;

            // The commands associated with this player.
            std::map<std::string, uint_fast32_t> controlledCommands;
        };

    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_PLAYER_H_ */
