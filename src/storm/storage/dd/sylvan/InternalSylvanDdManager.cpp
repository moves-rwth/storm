#include "storm/storage/dd/sylvan/InternalSylvanDdManager.h"

#include <cmath>
#include <iostream>

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/SylvanSettings.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidSettingsException.h"

#include "storm/adapters/sylvan.h"

#include "storm-config.h"

namespace storm {
    namespace dd {
        
#ifndef NDEBUG
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-length-array"
#pragma clang diagnostic ignored "-Wc99-extensions"
#endif
        
        VOID_TASK_0(gc_start) {
            STORM_LOG_TRACE("Starting sylvan garbage collection...");
        }
        
        VOID_TASK_0(gc_end) {
            STORM_LOG_TRACE("Sylvan garbage collection done.");
        }
        
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        
#endif
        
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
                    lace_init(settings.getNumberOfThreads(), 1024*1024*16);
                } else {
                    lace_init(0, 1024*1024*16);
                }
                lace_startup(0, 0, 0);
                
                // Table/cache size computation taken from newer version of sylvan.
                uint64_t memorycap = storm::settings::getModule<storm::settings::modules::SylvanSettings>().getMaximalMemory() * 1024 * 1024;
                
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
                STORM_LOG_THROW(cur <= memorycap, storm::exceptions::InvalidSettingsException, "Memory cap incompatible with default table ratio.");
                STORM_LOG_WARN_COND(memorycap < 60 * 0x0000040000000000, "Sylvan only supports tablesizes <= 42 bits. Memory limit is changed accordingly.");

                while (2*cur < memorycap && max_t < 0x0000040000000000) {
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
                
                STORM_LOG_DEBUG("Initializing sylvan library. Initial/max table size: " << min_t << "/" << max_t << ", initial/max cache size: " << min_c << "/" << max_c << ".");
                sylvan::Sylvan::initPackage(min_t, max_t, min_c, max_c);

                sylvan::Sylvan::initBdd();
                sylvan::Sylvan::initMtbdd();
                sylvan::Sylvan::initCustomMtbdd();
                
#ifndef NDEBUG
                sylvan_gc_hook_pregc(TASK(gc_start));
                sylvan_gc_hook_postgc(TASK(gc_end));
#endif

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
        
        InternalBdd<DdType::Sylvan> InternalDdManager<DdType::Sylvan>::getBddEncodingLessOrEqualThan(uint64_t bound, InternalBdd<DdType::Sylvan> const& cube, uint64_t numberOfDdVariables) const {
            return InternalBdd<DdType::Sylvan>(this, sylvan::Bdd(this->getBddEncodingLessOrEqualThanRec(0, (1ull << numberOfDdVariables) - 1, bound, cube.getSylvanBdd().GetBDD(), numberOfDdVariables)));
        }
        
        BDD InternalDdManager<DdType::Sylvan>::getBddEncodingLessOrEqualThanRec(uint64_t minimalValue, uint64_t maximalValue, uint64_t bound, BDD cube, uint64_t remainingDdVariables) const {
            if (maximalValue <= bound) {
                return sylvan_true;
            } else if (minimalValue > bound) {
                return sylvan_false;
            }
            
            STORM_LOG_ASSERT(remainingDdVariables > 0, "Expected more remaining DD variables.");
            uint64_t newRemainingDdVariables = remainingDdVariables - 1;
            BDD elseResult = getBddEncodingLessOrEqualThanRec(minimalValue, maximalValue & ~(1ull << newRemainingDdVariables), bound, sylvan_high(cube), newRemainingDdVariables);
            bdd_refs_push(elseResult);
            BDD thenResult = getBddEncodingLessOrEqualThanRec(minimalValue | (1ull << newRemainingDdVariables), maximalValue, bound, sylvan_high(cube), newRemainingDdVariables);
            bdd_refs_push(elseResult);
            BDD result = sylvan_makenode(sylvan_var(cube), elseResult, thenResult);
            bdd_refs_pop(2);
            return result;
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

        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalDdManager<DdType::Sylvan>::getAddUndefined() const {
            return InternalAdd<DdType::Sylvan, ValueType>(this, sylvan::Mtbdd(sylvan::Bdd::bddZero()));
        }

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
        
        void InternalDdManager<DdType::Sylvan>::debugCheck() const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation is not supported by sylvan.");
        }
        
        uint_fast64_t InternalDdManager<DdType::Sylvan>::getNumberOfDdVariables() const {
            return nextFreeVariableIndex;
        }
        
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddUndefined() const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddUndefined() const;
        
        template InternalAdd<DdType::Sylvan, storm::RationalNumber> InternalDdManager<DdType::Sylvan>::getAddUndefined() const;

#ifdef STORM_HAVE_CARL
        template InternalAdd<DdType::Sylvan, storm::RationalFunction> InternalDdManager<DdType::Sylvan>::getAddUndefined() const;
#endif
    }
}
