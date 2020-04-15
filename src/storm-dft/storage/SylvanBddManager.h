#pragma once

#include <map>

#include "storm-config.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/SylvanSettings.h"
#include "storm/utility/macros.h"
#include "storm/utility/sylvan.h"

namespace storm {
namespace storage {

/**
 * Simple Manager for Sylvan
 *
 * \note
 * Not thread safe.
 *
 * Initialises Sylvan
 * which can only be initialize once per programm invocation
 */
class SylvanBddManager {
   public:
    /**
     * Initilizes Sylvan
     */
    SylvanBddManager() {
        storm::settings::modules::SylvanSettings const& settings =
            storm::settings::getModule<
                storm::settings::modules::SylvanSettings>();
        if (settings.isNumberOfThreadsSet()) {
            lace_init(settings.getNumberOfThreads(), 1024 * 1024 * 16);
        } else {
            lace_init(0, 1024 * 1024 * 16);
        }
        lace_startup(0, 0, 0);

        // Table/cache size computation taken from newer version of sylvan.
        uint64_t memorycap = storm::settings::getModule<
                                 storm::settings::modules::SylvanSettings>()
                                 .getMaximalMemory() *
                             1024 * 1024;

        uint64_t table_ratio = 0;
        uint64_t initial_ratio = 0;

        uint64_t max_t = 1;
        uint64_t max_c = 1;
        if (table_ratio > 0) {
            max_t <<= table_ratio;
        } else {
            max_c <<= -table_ratio;
        }

        uint64_t cur = max_t * 24 + max_c * 36;
        STORM_LOG_THROW(cur <= memorycap,
                        storm::exceptions::InvalidSettingsException,
                        "Memory cap incompatible with default table ratio.");
        STORM_LOG_WARN_COND(memorycap < 60 * 0x0000040000000000,
                            "Sylvan only supports tablesizes <= 42 bits. "
                            "Memory limit is changed accordingly.");

        while (2 * cur < memorycap && max_t < 0x0000040000000000) {
            max_t *= 2;
            max_c *= 2;
            cur *= 2;
        }

        uint64_t min_t = max_t, min_c = max_c;
        while (initial_ratio > 0 && min_t > 0x1000 && min_c > 0x1000) {
            min_t >>= 1;
            min_c >>= 1;
            initial_ratio--;
        }
        // End of copied code.

        STORM_LOG_DEBUG("Initializing sylvan library. Initial/max table size: "
                        << min_t << "/" << max_t << ", initial/max cache size: "
                        << min_c << "/" << max_c << ".");
        sylvan::Sylvan::initPackage(min_t, max_t, min_c, max_c);

        sylvan::Sylvan::initBdd();
    }

    // We can only initialize Sylvan once therefore no copy semantics
    SylvanBddManager(SylvanBddManager const&) = delete;

    SylvanBddManager(SylvanBddManager&&) = default;
    SylvanBddManager& operator=(SylvanBddManager&&) = default;

    /**
     * Destroys Sylvan
     */
    ~SylvanBddManager() {
        sylvan::Sylvan::quitPackage();
        lace_exit();
    }

    /**
     * Creates a variable with a unique name
     *
     * \return Index of the variable
     */
    uint32_t createVariable(std::string const name) {
        nameToIndex[name] = nextFreeVariableIndex;
        indexToName[nextFreeVariableIndex] = name;
        sylvan::Bdd::bddVar(nextFreeVariableIndex);
        return nextFreeVariableIndex++;
    }

    /**
     * \return
     * The bdd representation of the variable.
     * Therefore ITE(x, 1, 0).
     */
    sylvan::Bdd getPositiveLiteral(std::string const name) const {
        return sylvan::Bdd::bddVar(nameToIndex.at(name));
    }

    /**
     * \note equivalent with !getPositiveLiteral(name)
     *
     * \return
     * The negative bdd representation of the variable.
     * Therefore ITE(x, 0, 1).
     */
    sylvan::Bdd getNegativeLiteral(std::string const name) const {
        return sylvan::Bdd::bddVar(nameToIndex.at(name));
    }

    /**
     * \return
     * The bdd representation of the variable.
     * Therefore ITE(x, 1, 0).
     */
    sylvan::Bdd getPositiveLiteral(uint32_t const index) const {
        return sylvan::Bdd::bddVar(index);
    }

    /**
     * \note equivalent with !getPositiveLiteral(index)
     *
     * \return
     * The negative bdd representation of the variable.
     * Therefore ITE(x, 0, 1).
     */
    sylvan::Bdd getNegativeLiteral(uint32_t const index) const {
        return sylvan::Bdd::bddVar(index);
    }

    /**
     * \return
     * The bdd represenatation of the constant function 1.
     */
    sylvan::Bdd getOne() { return sylvan::Bdd::bddOne(); }

    /**
     * \return
     * The bdd represenatation of the constant function 1.
     */
    sylvan::Bdd getZero() { return sylvan::Bdd::bddZero(); }

    /**
     * \return
     * The index of the variable with the given name.
     */
    uint32_t getIndex(std::string const name) const {
        return nameToIndex.at(name);
    }

    /**
     * \return
     * The name of the variable with the given index.
     */
    std::string getName(uint32_t const index) const {
        return indexToName.at(index);
    }

   private:
    uint32_t nextFreeVariableIndex{0};

    std::map<std::string, uint32_t> nameToIndex{};
    std::map<uint32_t, std::string> indexToName{};
};

}  // namespace storage
}  // namespace storm
