#pragma once

#include <string>
#include <vector>

#include "storm/storage/prism/Module.h"
#include "storm/storage/prism/Command.h"

// needed?
#include "storm/storage/BoostTypes.h"
#include "storm/utility/OsDetection.h"

namespace storm {
    namespace prism {
        
        typedef uint_fast64_t PlayerIndex;
        
        class Player : public LocatedInformation {
        public:
            /*!
             * Creates a player with the given name, controlled modules and actions.
             *
             * @param playerName The name of the player.
             * @param controlledModules The controlled modules.
             * @param controlledActions The controlled actions.
             * @param filename The filename in which the player is defined.
             * @param lineNumber The line number in which the player is defined.
             */
            Player(std::string const& playerName, std::map<std::string, uint_fast64_t> const& controlledModules, std::map<std::string, uint_fast64_t> const& controlledActions, std::string const& filename = "", uint_fast32_t lineNumber = 0);

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
            std::map<std::string, uint_fast64_t> const& getModules() const; // TODO

            /*!
             * Retrieves all controlled Actions of the player.
             *
             * @return The Actions controlled by the player.
             */
            std::map<std::string, uint_fast64_t> const& getActions() const;

            friend std::ostream& operator<<(std::ostream& stream, Player const& player);
        private:
            // The name of the player.
            std::string playerName;

            // The modules associated with this player.
            std::map<std::string, uint_fast64_t> controlledModules;

            // The Actions associated with this player.
            std::map<std::string, uint_fast64_t> controlledActions;
        };

    } // namespace prism
} // namespace storm

