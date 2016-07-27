#include "src/storage/dd/sylvan/InternalSylvanDdManager.h"

#include <cmath>

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/SylvanSettings.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/NotSupportedException.h"

#include "src/utility/sylvan.h"

#include "storm-config.h"
// TODO: Remove this later on.
#ifndef STORM_HAVE_CARL
#define STORM_HAVE_CARL 1
#endif

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
                // Initialize lace: auto-detect number of workers.
                lace_init(storm::settings::getModule<storm::settings::modules::SylvanSettings>().getNumberOfThreads(), 1000000);
                lace_startup(0, 0, 0);
                
                // Each node takes 24 bytes and the maximal memory is specified in megabytes.
                uint_fast64_t totalNodesToStore = storm::settings::getModule<storm::settings::modules::SylvanSettings>().getMaximalMemory() * 1024 * 1024 / 24;
                
                // Compute the power of two that still fits within the total numbers to store.
                uint_fast64_t powerOfTwo = findLargestPowerOfTwoFitting(totalNodesToStore);
                
                sylvan::Sylvan::initPackage(1ull << std::max(16ull, powerOfTwo > 24 ? powerOfTwo - 8 : 0ull), 1ull << (powerOfTwo - 1), 1ull << std::max(16ull, powerOfTwo > 24 ? powerOfTwo - 12 : 0ull), 1ull << (powerOfTwo - 1));
                sylvan::Sylvan::initBdd(1);
                sylvan::Sylvan::initMtbdd();
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

#ifdef STORM_HAVE_CARL
		template<>
		InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getAddOne() const {
			storm::RationalFunction rationalFunction = storm::utility::one<storm::RationalFunction>();
			storm_rational_function_ptr_struct helperStruct;
			helperStruct.storm_rational_function = (void*)(&rationalFunction);
			uint64_t value = (uint64_t)&helperStruct;

			return InternalAdd<DdType::Sylvan, storm::RationalFunction>(this, sylvan::Mtbdd::terminal(sylvan_storm_rational_function_get_type(), value));
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

#ifdef STORM_HAVE_CARL
		template<>
		InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getAddZero() const {
			storm::RationalFunction rationalFunction = storm::utility::zero<storm::RationalFunction>();
			storm_rational_function_ptr_struct helperStruct;
			helperStruct.storm_rational_function = (void*)(&rationalFunction);
			uint64_t value = (uint64_t)&helperStruct;

			return InternalAdd<DdType::Sylvan, storm::RationalFunction>(this, sylvan::Mtbdd::terminal(sylvan_storm_rational_function_get_type(), value));
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
        
#ifdef STORM_HAVE_CARL
		template<>
		InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getConstant(storm::RationalFunction const& value) const {
			return InternalAdd<DdType::Sylvan, storm::RationalFunction>(this, sylvan::Mtbdd::stormRationalFunctionTerminal(value));
		}
#endif

        std::pair<InternalBdd<DdType::Sylvan>, InternalBdd<DdType::Sylvan>> InternalDdManager<DdType::Sylvan>::createNewDdVariablePair() {
            InternalBdd<DdType::Sylvan> first = InternalBdd<DdType::Sylvan>(this, sylvan::Bdd::bddVar(nextFreeVariableIndex));
            InternalBdd<DdType::Sylvan> second = InternalBdd<DdType::Sylvan>(this, sylvan::Bdd::bddVar(nextFreeVariableIndex + 1));
            nextFreeVariableIndex += 2;
            return std::make_pair(first, second);
        }
        
        void InternalDdManager<DdType::Sylvan>::allowDynamicReordering(bool value) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation is not supported by sylvan.");
        }
        
        bool InternalDdManager<DdType::Sylvan>::isDynamicReorderingAllowed() const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation is not supported by sylvan.");
        }
        
        void InternalDdManager<DdType::Sylvan>::triggerReordering() {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation is not supported by sylvan.");
        }
                
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddOne() const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddOne() const;
#ifdef STORM_HAVE_CARL
		template InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getAddOne() const;
#endif
        
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddZero() const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddZero() const;
#ifdef STORM_HAVE_CARL
		template InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getAddZero() const;
#endif
        
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getConstant(double const& value) const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getConstant(uint_fast64_t const& value) const;
#ifdef STORM_HAVE_CARL
		template InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getConstant(storm::RationalFunction const& value) const;
#endif
    }
}