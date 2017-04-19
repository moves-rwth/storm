#include "storm/storage/dd/sylvan/InternalSylvanDdManager.h"

#include <cmath>
#include <iostream>

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/SylvanSettings.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidSettingsException.h"

#include "storm/utility/sylvan.h"

#include "storm-config.h"

namespace storm {
    namespace dd {
        uint_fast64_t InternalDdManager<DdType::Sylvan>::numberOfInstances = 0;
        
        // It is important that the variable pairs start at an even offset, because sylvan assumes this to be true for
        // some operations.
        uint_fast64_t InternalDdManager<DdType::Sylvan>::nextFreeVariableIndex = 0;
        
        uint_fast64_t findLargestPowerOfTwoFitting(uint_fast64_t number) {
            for (uint_fast64_t index = 0; index < 64; ++index) {
                if ((number & (1ull << (63 - index))) != 0) {
                    return 63 - index;
                }
            }
            return 0;
        }
        
        InternalDdManager<DdType::Sylvan>::InternalDdManager() {
            if (numberOfInstances == 0) {
                storm::settings::modules::SylvanSettings const& settings = storm::settings::getModule<storm::settings::modules::SylvanSettings>();
                if (settings.isNumberOfThreadsSet()) {
                    lace_init(settings.getNumberOfThreads(), 1000000);
                } else {
                    lace_init(0, 1000000);
                }
                lace_startup(0, 0, 0);
                
                // Each node takes 24 bytes and the maximal memory is specified in megabytes.
                uint_fast64_t totalNodesToStore = storm::settings::getModule<storm::settings::modules::SylvanSettings>().getMaximalMemory() * 1024 * 1024 / 24;
                
                // Compute the power of two that still fits within the total numbers to store.
                uint_fast64_t powerOfTwo = findLargestPowerOfTwoFitting(totalNodesToStore);
                
                STORM_LOG_THROW(powerOfTwo >= 16, storm::exceptions::InvalidSettingsException, "Too little memory assigned to sylvan.");
                
                STORM_LOG_TRACE("Assigning " << (1ull << (powerOfTwo - 1)) << " slots to both sylvan's unique table and its cache.");
                sylvan::Sylvan::initPackage(1ull << (powerOfTwo - 1), 1ull << (powerOfTwo - 1), 1ull << (powerOfTwo - 1), 1ull << (powerOfTwo - 1));
                sylvan::Sylvan::setGranularity(3);
                sylvan::Sylvan::initBdd();
                sylvan::Sylvan::initMtbdd();
                sylvan::Sylvan::initCustomMtbdd();
            }
            ++numberOfInstances;
        }
        
        InternalDdManager<DdType::Sylvan>::~InternalDdManager() {
            --numberOfInstances;
            if (numberOfInstances == 0) {
                // Enable this to print the sylvan statistics to a file.
//                FILE* filePointer = fopen("sylvan.stats", "w");
//                sylvan_stats_report(filePointer, 0);
//                fclose(filePointer);
                
                sylvan::Sylvan::quitPackage();
                lace_exit();
            }
        }
        
        InternalBdd<DdType::Sylvan> InternalDdManager<DdType::Sylvan>::getBddOne() const {
            return InternalBdd<DdType::Sylvan>(this, sylvan::Bdd::bddOne());
        }
        
        template<>
        InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddOne() const {
            return InternalAdd<DdType::Sylvan, double>(this, sylvan::Mtbdd::doubleTerminal(storm::utility::one<double>()));
        }

        template<>
        InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddOne() const {
            return InternalAdd<DdType::Sylvan, uint_fast64_t>(this, sylvan::Mtbdd::int64Terminal(storm::utility::one<uint_fast64_t>()));
        }

        template<>
        InternalAdd<DdType::Sylvan, storm::RationalNumber> InternalDdManager<DdType::Sylvan>::getAddOne() const {
            return InternalAdd<DdType::Sylvan, storm::RationalNumber>(this, sylvan::Mtbdd::stormRationalNumberTerminal(storm::utility::one<storm::RationalNumber>()));
        }
        
#ifdef STORM_HAVE_CARL
		template<>
		InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getAddOne() const {
			return InternalAdd<DdType::Sylvan, storm::RationalFunction>(this, sylvan::Mtbdd::stormRationalFunctionTerminal(storm::utility::one<storm::RationalFunction>()));
		}
#endif
        
        InternalBdd<DdType::Sylvan> InternalDdManager<DdType::Sylvan>::getBddZero() const {
            return InternalBdd<DdType::Sylvan>(this, sylvan::Bdd::bddZero());
        }
        
        template<>
        InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddZero() const {
            return InternalAdd<DdType::Sylvan, double>(this, sylvan::Mtbdd::doubleTerminal(storm::utility::zero<double>()));
        }

        template<>
        InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddZero() const {
            return InternalAdd<DdType::Sylvan, uint_fast64_t>(this, sylvan::Mtbdd::int64Terminal(storm::utility::zero<uint_fast64_t>()));
        }

        template<>
        InternalAdd<DdType::Sylvan, storm::RationalNumber> InternalDdManager<DdType::Sylvan>::getAddZero() const {
            return InternalAdd<DdType::Sylvan, storm::RationalNumber>(this, sylvan::Mtbdd::stormRationalNumberTerminal(storm::utility::zero<storm::RationalNumber>()));
        }

#ifdef STORM_HAVE_CARL
		template<>
		InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getAddZero() const {
			return InternalAdd<DdType::Sylvan, storm::RationalFunction>(this, sylvan::Mtbdd::stormRationalFunctionTerminal(storm::utility::zero<storm::RationalFunction>()));
		}
#endif
        
        template<>
        InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getConstant(double const& value) const {
            return InternalAdd<DdType::Sylvan, double>(this, sylvan::Mtbdd::doubleTerminal(value));
        }

        template<>
        InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getConstant(uint_fast64_t const& value) const {
            return InternalAdd<DdType::Sylvan, uint_fast64_t>(this, sylvan::Mtbdd::int64Terminal(value));
        }

        template<>
        InternalAdd<DdType::Sylvan, storm::RationalNumber> InternalDdManager<DdType::Sylvan>::getConstant(storm::RationalNumber const& value) const {
            return InternalAdd<DdType::Sylvan, storm::RationalNumber>(this, sylvan::Mtbdd::stormRationalNumberTerminal(value));
        }

#ifdef STORM_HAVE_CARL
		template<>
		InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getConstant(storm::RationalFunction const& value) const {
			return InternalAdd<DdType::Sylvan, storm::RationalFunction>(this, sylvan::Mtbdd::stormRationalFunctionTerminal(value));
		}
#endif

        std::vector<InternalBdd<DdType::Sylvan>> InternalDdManager<DdType::Sylvan>::createDdVariables(uint64_t numberOfLayers, boost::optional<uint_fast64_t> const& position) {
            STORM_LOG_THROW(!position, storm::exceptions::NotSupportedException, "The manager does not support ordered insertion.");
			
            std::vector<InternalBdd<DdType::Sylvan>> result;
            
            for (uint64_t layer = 0; layer < numberOfLayers; ++layer) {
                result.emplace_back(InternalBdd<DdType::Sylvan>(this, sylvan::Bdd::bddVar(nextFreeVariableIndex)));
                ++nextFreeVariableIndex;
            }
            
            return result;
        }
        
        bool InternalDdManager<DdType::Sylvan>::supportsOrderedInsertion() const {
            return false;
        }
        
        void InternalDdManager<DdType::Sylvan>::allowDynamicReordering(bool) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation is not supported by sylvan.");
        }
        
        bool InternalDdManager<DdType::Sylvan>::isDynamicReorderingAllowed() const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation is not supported by sylvan.");
        }
        
        void InternalDdManager<DdType::Sylvan>::triggerReordering() {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation is not supported by sylvan.");
        }
        
        uint_fast64_t InternalDdManager<DdType::Sylvan>::getNumberOfDdVariables() const {
            return nextFreeVariableIndex;
        }
        
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddOne() const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddOne() const;

        template InternalAdd<DdType::Sylvan, storm::RationalNumber> InternalDdManager<DdType::Sylvan>::getAddOne() const;

#ifdef STORM_HAVE_CARL
		template InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getAddOne() const;
#endif
        
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddZero() const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddZero() const;
        
        template InternalAdd<DdType::Sylvan, storm::RationalNumber> InternalDdManager<DdType::Sylvan>::getAddZero() const;

#ifdef STORM_HAVE_CARL
		template InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getAddZero() const;
#endif
        
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getConstant(double const& value) const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getConstant(uint_fast64_t const& value) const;

        template InternalAdd<DdType::Sylvan, storm::RationalNumber> InternalDdManager<DdType::Sylvan>::getConstant(storm::RationalNumber const& value) const;

#ifdef STORM_HAVE_CARL
		template InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getConstant(storm::RationalFunction const& value) const;
#endif
    }
}
