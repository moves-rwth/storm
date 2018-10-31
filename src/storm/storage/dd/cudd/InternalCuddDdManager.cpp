#include "storm/storage/dd/cudd/InternalCuddDdManager.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CuddSettings.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace dd {
        
        InternalDdManager<DdType::CUDD>::InternalDdManager() : cuddManager(), reorderingTechnique(CUDD_REORDER_NONE), numberOfDdVariables(0), allowReorder(false), reorderingInhibitionCounter(0) {
            this->cuddManager.SetMaxMemory(static_cast<unsigned long>(storm::settings::getModule<storm::settings::modules::CuddSettings>().getMaximalMemory() * 1024ul * 1024ul));
            
            auto const& settings = storm::settings::getModule<storm::settings::modules::CuddSettings>();
            this->cuddManager.SetEpsilon(settings.getConstantPrecision());
            
            // Now set the selected reordering technique.
            storm::settings::modules::CuddSettings::ReorderingTechnique reorderingTechniqueAsSetting = settings.getReorderingTechnique();
            switch (reorderingTechniqueAsSetting) {
                case storm::settings::modules::CuddSettings::ReorderingTechnique::None: this->reorderingTechnique = CUDD_REORDER_NONE; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Random: this->reorderingTechnique = CUDD_REORDER_RANDOM; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::RandomPivot: this->reorderingTechnique = CUDD_REORDER_RANDOM_PIVOT; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Sift: this->reorderingTechnique = CUDD_REORDER_SIFT; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::SiftConv: this->reorderingTechnique = CUDD_REORDER_SIFT_CONVERGE; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::SymmetricSift: this->reorderingTechnique = CUDD_REORDER_SYMM_SIFT; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::SymmetricSiftConv: this->reorderingTechnique = CUDD_REORDER_SYMM_SIFT_CONV; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::GroupSift: this->reorderingTechnique = CUDD_REORDER_GROUP_SIFT; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::GroupSiftConv: this->reorderingTechnique = CUDD_REORDER_GROUP_SIFT_CONV; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Win2: this->reorderingTechnique = CUDD_REORDER_WINDOW2; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Win2Conv: this->reorderingTechnique = CUDD_REORDER_WINDOW2_CONV; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Win3: this->reorderingTechnique = CUDD_REORDER_WINDOW3; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Win3Conv: this->reorderingTechnique = CUDD_REORDER_WINDOW3_CONV; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Win4: this->reorderingTechnique = CUDD_REORDER_WINDOW4; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Win4Conv: this->reorderingTechnique = CUDD_REORDER_WINDOW4_CONV; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Annealing: this->reorderingTechnique = CUDD_REORDER_ANNEALING; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Genetic: this->reorderingTechnique = CUDD_REORDER_GENETIC; break;
                case storm::settings::modules::CuddSettings::ReorderingTechnique::Exact: this->reorderingTechnique = CUDD_REORDER_EXACT; break;
            }
            
            this->allowDynamicReordering(settings.isReorderingEnabled());
        }
        
        InternalDdManager<DdType::CUDD>::~InternalDdManager() {
            // Intentionally left empty.
        }
        
        InternalBdd<DdType::CUDD> InternalDdManager<DdType::CUDD>::getBddOne() const {
            return InternalBdd<DdType::CUDD>(this, cuddManager.bddOne());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalDdManager<DdType::CUDD>::getAddOne() const {
            return InternalAdd<DdType::CUDD, ValueType>(this, cuddManager.addOne());
        }
        
        InternalBdd<DdType::CUDD> InternalDdManager<DdType::CUDD>::getBddZero() const {
            return InternalBdd<DdType::CUDD>(this, cuddManager.bddZero());
        }
        
        InternalBdd<DdType::CUDD> InternalDdManager<DdType::CUDD>::getBddEncodingLessOrEqualThan(uint64_t bound, InternalBdd<DdType::CUDD> const& cube, uint64_t numberOfDdVariables) const {

            struct Recursion {
                Recursion(cudd::Cudd const& cuddManager, uint64_t bound, DdNodePtr cube, uint64_t numberOfDdVariables) : cuddManager(cuddManager), bound(bound) {
                    // As initialisation, we first obtain the index and the level (i.e., the position in the current variable order)
                    // for each variable in the cube. We assume here that the highest bit in the encoding corresponds to the
                    // variable with the lowest index, etc
                    std::map<uint64_t, uint64_t> indicesAndLevels;
                    DdNodePtr cur = cube;
                    while (!Cudd_IsConstant(cur)) {
                        uint64_t index = Cudd_NodeReadIndex(cur);
                        uint64_t level = Cudd_ReadPerm(cuddManager.getManager(), index);
                        indicesAndLevels[index]=level;
                        cur = Cudd_T(cur);
                    }
                    STORM_LOG_ASSERT(indicesAndLevels.size() == numberOfDdVariables, "Fewer/more variables in cube than expected.");

                    // now, we are iterating over the indicesAndLevels map
                    // in increasing index order and store the the corresponding
                    // bit (mask) in the levelToBitMap, which is indexed by the level
                    uint64_t bit = 1ull << (numberOfDdVariables-1);
                    for (auto& element : indicesAndLevels) {
                        STORM_LOG_ASSERT(bit != 0, "More variables in cube than expected.");
                        // ordered with ascending index, lower index = more significant bit
                        levelToBit[element.second] = bit;
                        bit = bit >> 1;
                    }
                }

                DdNodePtr rec(uint64_t minimalValue, uint64_t maximalValue, DdNodePtr cube) const {
                    if (maximalValue <= bound) {
                        return Cudd_ReadOne(cuddManager.getManager());
                    } else if (minimalValue > bound) {
                        return Cudd_ReadLogicZero(cuddManager.getManager());
                    }

                    STORM_LOG_ASSERT(!Cudd_IsConstant(cube), "Expected non-constant cube.");
                    // get index and current level in the ordering
                    uint64_t index = Cudd_NodeReadIndex(cube);
                    uint64_t level = Cudd_ReadPerm(cuddManager.getManager(), index);
                    // get the corresponding bit mask
                    uint64_t bit = levelToBit.at(level);

                    // for else, we have to delete the bit from maximalValue
                    // and recurse to the next variable in the cube
                    DdNodePtr elseResult = rec(minimalValue, maximalValue & ~bit, Cudd_T(cube));
                    Cudd_Ref(elseResult);

                    // for then, we have to set the bit in minimalValue
                    // and recurse to the next variable in the cube
                    DdNodePtr thenResult = rec(minimalValue | bit, maximalValue, Cudd_T(cube));
                    Cudd_Ref(thenResult);
                    STORM_LOG_ASSERT(thenResult != elseResult, "Expected different results.");

                    // construct the result node for this level using cuddUniqueInter
                    bool complemented = Cudd_IsComplement(thenResult);
                    DdNodePtr result = cuddUniqueInter(cuddManager.getManager(), index, Cudd_Regular(thenResult), complemented ? Cudd_Not(elseResult) : elseResult);
                    if (complemented) {
                        result = Cudd_Not(result);
                    }
                    Cudd_Deref(thenResult);
                    Cudd_Deref(elseResult);
                    return result;
                }

                cudd::Cudd const& cuddManager;
                std::map<uint64_t, uint64_t> levelToBit;
                uint64_t bound;
            };

            // inhibit reordering until end of function, so that we don't confuse the variable levels
            auto inhibitReorder = getDynamicReorderingInhibitor();

            // setup recursion
            Recursion rec(cuddManager, bound, cube.getCuddDdNode(), numberOfDdVariables);
            // do recursion and return result as a BDD
            return InternalBdd<DdType::CUDD>(this, cudd::BDD(cuddManager, rec.rec(0, (1ull << numberOfDdVariables) - 1, cube.getCuddDdNode())));
        }
        
        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalDdManager<DdType::CUDD>::getAddZero() const {
            return InternalAdd<DdType::CUDD, ValueType>(this, cuddManager.addZero());
        }

        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalDdManager<DdType::CUDD>::getAddUndefined() const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Undefined values are not supported by CUDD.");
        }

        template<typename ValueType>
        InternalAdd<DdType::CUDD, ValueType> InternalDdManager<DdType::CUDD>::getConstant(ValueType const& value) const {
            return InternalAdd<DdType::CUDD, ValueType>(this, cuddManager.constant(value));
        }

        template<>
        InternalAdd<DdType::CUDD, storm::RationalNumber> InternalDdManager<DdType::CUDD>::getConstant(storm::RationalNumber const& value) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation not supported.");
        }

        std::vector<InternalBdd<DdType::CUDD>> InternalDdManager<DdType::CUDD>::createDdVariables(uint64_t numberOfLayers, boost::optional<uint_fast64_t> const& position) {
            std::vector<InternalBdd<DdType::CUDD>> result;
            
            if (position) {
                for (uint64_t layer = 0; layer < numberOfLayers; ++layer) {
                    result.emplace_back(InternalBdd<DdType::CUDD>(this, cuddManager.bddNewVarAtLevel(position.get() + layer)));
                }
            } else {
                for (uint64_t layer = 0; layer < numberOfLayers; ++layer) {
                    result.emplace_back(InternalBdd<DdType::CUDD>(this, cuddManager.bddVar()));
                }
            }
            
            // Connect the variables so they are not 'torn apart' by dynamic reordering.
            // Note that MTR_FIXED preserves the order of the layers. While this is not always necessary to preserve,
            // (for example) the hybrid engine relies on this connection, so we choose MTR_FIXED instead of MTR_DEFAULT.
            cuddManager.MakeTreeNode(result.front().getIndex(), numberOfLayers, MTR_FIXED);
            
            // Keep track of the number of variables.
            numberOfDdVariables += numberOfLayers;
            
            return result;
        }
        
        bool InternalDdManager<DdType::CUDD>::supportsOrderedInsertion() const {
            return true;
        }
        
        void InternalDdManager<DdType::CUDD>::allowDynamicReordering(bool value) {
            allowReorder = value;
            setDynamicReorderingState();
        }

        void InternalDdManager<DdType::CUDD>::setDynamicReorderingState() {
            if (allowReorder && reorderingInhibitionCounter == 0) {
                this->getCuddManager().AutodynEnable(this->reorderingTechnique);
            } else {
                this->getCuddManager().AutodynDisable();
            }
        }
        
        bool InternalDdManager<DdType::CUDD>::isDynamicReorderingAllowed() const {
            return allowReorder;
        }
        
        void InternalDdManager<DdType::CUDD>::triggerReordering() {
            this->getCuddManager().ReduceHeap(this->reorderingTechnique, 0);
        }

        std::unique_ptr<InternalDdManager<DdType::CUDD>::DynamicReorderingInhibitor> InternalDdManager<DdType::CUDD>::getDynamicReorderingInhibitor() const {
            return std::make_unique<DynamicReorderingInhibitor>(const_cast<InternalDdManager&>(*this));
        }

        
        void InternalDdManager<DdType::CUDD>::debugCheck() const {
            this->getCuddManager().CheckKeys();
            this->getCuddManager().DebugCheck();
        }
        
        cudd::Cudd& InternalDdManager<DdType::CUDD>::getCuddManager() {
            return cuddManager;
        }
        
        cudd::Cudd const& InternalDdManager<DdType::CUDD>::getCuddManager() const {
            return cuddManager;
        }
        
        uint_fast64_t InternalDdManager<DdType::CUDD>::getNumberOfDdVariables() const {
            return numberOfDdVariables;
        }
        
        InternalDdManager<DdType::CUDD>::DynamicReorderingInhibitor::DynamicReorderingInhibitor(InternalDdManager<DdType::CUDD>& manager) : manager(manager) {
            manager.reorderingInhibitionCounter++;
            if (manager.allowReorder && manager.reorderingInhibitionCounter == 1) {
                // edge from allow reorder to inhibit
                manager.setDynamicReorderingState();
            }
        }

        InternalDdManager<DdType::CUDD>::DynamicReorderingInhibitor::~DynamicReorderingInhibitor() {
            STORM_LOG_THROW(manager.reorderingInhibitionCounter > 0, storm::exceptions::InvalidOperationException, "Invalid value of dynamic reordering inhibition counter");
            --manager.reorderingInhibitionCounter;
            if (manager.allowReorder && manager.reorderingInhibitionCounter == 0) {
                // edge from inhibit reorder to allow
                manager.setDynamicReorderingState();
            }
        }


        template InternalAdd<DdType::CUDD, double> InternalDdManager<DdType::CUDD>::getAddOne() const;
        template InternalAdd<DdType::CUDD, uint_fast64_t> InternalDdManager<DdType::CUDD>::getAddOne() const;
        template InternalAdd<DdType::CUDD, storm::RationalNumber> InternalDdManager<DdType::CUDD>::getAddOne() const;

        template InternalAdd<DdType::CUDD, double> InternalDdManager<DdType::CUDD>::getAddZero() const;
        template InternalAdd<DdType::CUDD, uint_fast64_t> InternalDdManager<DdType::CUDD>::getAddZero() const;
        template InternalAdd<DdType::CUDD, storm::RationalNumber> InternalDdManager<DdType::CUDD>::getAddZero() const;

        template InternalAdd<DdType::CUDD, double> InternalDdManager<DdType::CUDD>::getConstant(double const& value) const;
        template InternalAdd<DdType::CUDD, uint_fast64_t> InternalDdManager<DdType::CUDD>::getConstant(uint_fast64_t const& value) const;
    }
}
