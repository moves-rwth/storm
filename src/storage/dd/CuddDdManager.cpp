#include <cmath>
#include <string>
#include <algorithm>

#include "src/storage/dd/CuddDdManager.h"
#include "src/utility/macros.h"
#include "src/storage/expressions/Variable.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/CuddSettings.h"
#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/dd/CuddAdd.h"
#include "CuddBdd.h"


namespace storm {
    namespace dd {
        DdManager<DdType::CUDD>::DdManager() : metaVariableMap(), cuddManager(), reorderingTechnique(CUDD_REORDER_NONE), manager(new storm::expressions::ExpressionManager()) {
            this->cuddManager.SetMaxMemory(static_cast<unsigned long>(storm::settings::cuddSettings().getMaximalMemory() * 1024ul * 1024ul));
            this->cuddManager.SetEpsilon(storm::settings::cuddSettings().getConstantPrecision());
            
            // Now set the selected reordering technique.
            storm::settings::modules::CuddSettings::ReorderingTechnique reorderingTechniqueAsSetting = storm::settings::cuddSettings().getReorderingTechnique();
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
        }
        
        Bdd<DdType::CUDD> DdManager<DdType::CUDD>::getBddOne() const {
            return Bdd<DdType::CUDD>(this->shared_from_this(), cuddManager.bddOne());
        }

        Add<DdType::CUDD> DdManager<DdType::CUDD>::getAddOne() const {
            return Add<DdType::CUDD>(this->shared_from_this(), cuddManager.addOne());
        }
        
        Bdd<DdType::CUDD> DdManager<DdType::CUDD>::getBddZero() const {
            return Bdd<DdType::CUDD>(this->shared_from_this(), cuddManager.bddZero());
        }
        
        Add<DdType::CUDD> DdManager<DdType::CUDD>::getAddZero() const {
            return Add<DdType::CUDD>(this->shared_from_this(), cuddManager.addZero());
        }
        
        
        Add<DdType::CUDD> DdManager<DdType::CUDD>::getConstant(double value) const {
            return Add<DdType::CUDD>(this->shared_from_this(), cuddManager.constant(value));
        }
        
        Bdd<DdType::CUDD> DdManager<DdType::CUDD>::getEncoding(storm::expressions::Variable const& variable, int_fast64_t value) const {
            DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(variable);
            
            STORM_LOG_THROW(value >= metaVariable.getLow() && value <= metaVariable.getHigh(), storm::exceptions::InvalidArgumentException, "Illegal value " << value << " for meta variable '" << variable.getName() << "'.");
            
            // Now compute the encoding relative to the low value of the meta variable.
            value -= metaVariable.getLow();
            
            std::vector<Bdd<DdType::CUDD>> const& ddVariables = metaVariable.getDdVariables();

            Bdd<DdType::CUDD> result;
            if (value & (1ull << (ddVariables.size() - 1))) {
                result = ddVariables[0];
            } else {
                result = !ddVariables[0];
            }
            
            for (std::size_t i = 1; i < ddVariables.size(); ++i) {
                if (value & (1ull << (ddVariables.size() - i - 1))) {
                    result &= ddVariables[i];
                } else {
                    result &= !ddVariables[i];
                }
            }
            
            return result;
        }
        
        Bdd<DdType::CUDD> DdManager<DdType::CUDD>::getRange(storm::expressions::Variable const& variable) const {
            storm::dd::DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(variable);
            
            Bdd<DdType::CUDD> result = this->getBddZero();
            
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result |= this->getEncoding(variable, value);
            }
            
            return result;
        }
        
        Add<DdType::CUDD> DdManager<DdType::CUDD>::getIdentity(storm::expressions::Variable const& variable) const {
            storm::dd::DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(variable);
            
            Add<DdType::CUDD> result = this->getAddZero();
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result += this->getEncoding(variable, value).toAdd() * this->getConstant(value);
            }
            return result;
        }

        std::pair<storm::expressions::Variable, storm::expressions::Variable> DdManager<DdType::CUDD>::addMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high, boost::optional<std::pair<MetaVariablePosition, storm::expressions::Variable>> const& position) {
            // Check whether the variable name is legal.
            STORM_LOG_THROW(name != "" && name.back() != '\'', storm::exceptions::InvalidArgumentException, "Illegal name of meta variable: '" << name << "'.");
            
            // Check whether a meta variable already exists.
            STORM_LOG_THROW(!this->hasMetaVariable(name), storm::exceptions::InvalidArgumentException, "A meta variable '" << name << "' already exists.");

            // Check that the range is legal.
            STORM_LOG_THROW(high != low, storm::exceptions::InvalidArgumentException, "Range of meta variable must be at least 2 elements.");
            
            std::size_t numberOfBits = static_cast<std::size_t>(std::ceil(std::log2(high - low + 1)));
            
            // If a specific position was requested, we compute it now.
            boost::optional<uint_fast64_t> level;
            if (position) {
                storm::dd::DdMetaVariable<DdType::CUDD> beforeVariable = this->getMetaVariable(position.get().second);
                level = position.get().first == MetaVariablePosition::Above ? std::numeric_limits<uint_fast64_t>::max() : std::numeric_limits<uint_fast64_t>::min();
                for (auto const& ddVariable : beforeVariable.getDdVariables()) {
                    level = position.get().first == MetaVariablePosition::Above ? std::min(level.get(), ddVariable.getLevel()) : std::max(level.get(), ddVariable.getLevel());
                }
                if (position.get().first == MetaVariablePosition::Below) {
                    ++level.get();
                }
            }
            
            storm::expressions::Variable unprimed = manager->declareBitVectorVariable(name, numberOfBits);
            storm::expressions::Variable primed = manager->declareBitVectorVariable(name + "'", numberOfBits);
            
            std::vector<Bdd<DdType::CUDD>> variables;
            std::vector<Bdd<DdType::CUDD>> variablesPrime;
            for (std::size_t i = 0; i < numberOfBits; ++i) {
                if (level) {
                    variables.emplace_back(Bdd<DdType::CUDD>(this->shared_from_this(), cuddManager.bddNewVarAtLevel(level.get() + 2 * i), {unprimed}));
                    variablesPrime.emplace_back(Bdd<DdType::CUDD>(this->shared_from_this(), cuddManager.bddNewVarAtLevel(level.get() + 2 * i + 1), {primed}));
                } else {
                    variables.emplace_back(Bdd<DdType::CUDD>(this->shared_from_this(), cuddManager.bddVar(), {unprimed}));
                    variablesPrime.emplace_back(Bdd<DdType::CUDD>(this->shared_from_this(), cuddManager.bddVar(), {primed}));
                }
            }
            
            // Now group the non-primed and primed variable.
            for (uint_fast64_t i = 0; i < numberOfBits; ++i) {
                this->getCuddManager().MakeTreeNode(variables[i].getIndex(), 2, MTR_FIXED);
            }
            
            metaVariableMap.emplace(unprimed, DdMetaVariable<DdType::CUDD>(name, low, high, variables, this->shared_from_this()));
            metaVariableMap.emplace(primed, DdMetaVariable<DdType::CUDD>(name + "'", low, high, variablesPrime, this->shared_from_this()));
            
            return std::make_pair(unprimed, primed);
        }
        
        std::pair<storm::expressions::Variable, storm::expressions::Variable> DdManager<DdType::CUDD>::addMetaVariable(std::string const& name, boost::optional<std::pair<MetaVariablePosition, storm::expressions::Variable>> const& position) {
            // Check whether the variable name is legal.
            STORM_LOG_THROW(name != "" && name.back() != '\'', storm::exceptions::InvalidArgumentException, "Illegal name of meta variable: '" << name << "'.");
            
            // Check whether a meta variable already exists.
            STORM_LOG_THROW(!this->hasMetaVariable(name), storm::exceptions::InvalidArgumentException, "A meta variable '" << name << "' already exists.");
            
            // If a specific position was requested, we compute it now.
            boost::optional<uint_fast64_t> level;
            if (position) {
                storm::dd::DdMetaVariable<DdType::CUDD> beforeVariable = this->getMetaVariable(position.get().second);
                level = position.get().first == MetaVariablePosition::Above ? std::numeric_limits<uint_fast64_t>::max() : std::numeric_limits<uint_fast64_t>::min();
                for (auto const& ddVariable : beforeVariable.getDdVariables()) {
                    level = position.get().first == MetaVariablePosition::Above ? std::min(level.get(), ddVariable.getLevel()) : std::max(level.get(), ddVariable.getLevel());
                }
                if (position.get().first == MetaVariablePosition::Below) {
                    ++level.get();
                }
            }
            
            storm::expressions::Variable unprimed = manager->declareBooleanVariable(name);
            storm::expressions::Variable primed = manager->declareBooleanVariable(name + "'");

            std::vector<Bdd<DdType::CUDD>> variables;
            std::vector<Bdd<DdType::CUDD>> variablesPrime;
            if (position) {
                variables.emplace_back(Bdd<DdType::CUDD>(this->shared_from_this(), cuddManager.bddNewVarAtLevel(level.get()), {unprimed}));
                variablesPrime.emplace_back(Bdd<DdType::CUDD>(this->shared_from_this(), cuddManager.bddNewVarAtLevel(level.get() + 1), {primed}));
            } else {
                variables.emplace_back(Bdd<DdType::CUDD>(this->shared_from_this(), cuddManager.bddVar(), {unprimed}));
                variablesPrime.emplace_back(Bdd<DdType::CUDD>(this->shared_from_this(), cuddManager.bddVar(), {primed}));
            }

            // Now group the non-primed and primed variable.
            this->getCuddManager().MakeTreeNode(variables.front().getIndex(), 2, MTR_FIXED);
            
            metaVariableMap.emplace(unprimed, DdMetaVariable<DdType::CUDD>(name, variables, this->shared_from_this()));
            metaVariableMap.emplace(primed, DdMetaVariable<DdType::CUDD>(name + "'", variablesPrime, this->shared_from_this()));
            
            return std::make_pair(unprimed, primed);
        }
        
        DdMetaVariable<DdType::CUDD> const& DdManager<DdType::CUDD>::getMetaVariable(storm::expressions::Variable const& variable) const {
            auto const& variablePair = metaVariableMap.find(variable);
            
            // Check whether the meta variable exists.
            STORM_LOG_THROW(variablePair != metaVariableMap.end(), storm::exceptions::InvalidArgumentException, "Unknown meta variable name '" << variable.getName() << "'.");
            
            return variablePair->second;
        }
        
        std::set<std::string> DdManager<DdType::CUDD>::getAllMetaVariableNames() const {
            std::set<std::string> result;
            for (auto const& variablePair : metaVariableMap) {
                result.insert(variablePair.first.getName());
            }
            return result;
        }
        
        std::size_t DdManager<DdType::CUDD>::getNumberOfMetaVariables() const {
            return this->metaVariableMap.size();
        }
        
        bool DdManager<DdType::CUDD>::hasMetaVariable(std::string const& metaVariableName) const {
            return manager->hasVariable(metaVariableName);
        }
        
        Cudd& DdManager<DdType::CUDD>::getCuddManager() {
            return this->cuddManager;
        }

        Cudd const& DdManager<DdType::CUDD>::getCuddManager() const {
            return this->cuddManager;
        }
        
        storm::expressions::ExpressionManager const& DdManager<DdType::CUDD>::getExpressionManager() const {
            return *manager;
        }
        
        storm::expressions::ExpressionManager& DdManager<DdType::CUDD>::getExpressionManager() {
            return *manager;
        }
        
        std::vector<std::string> DdManager<DdType::CUDD>::getDdVariableNames() const {
            // First, we initialize a list DD variables and their names.
            std::vector<std::pair<uint_fast64_t, std::string>> variablePairs;
            for (auto const& variablePair : this->metaVariableMap) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = variablePair.second;
                // If the meta variable is of type bool, we don't need to suffix it with the bit number.
                if (metaVariable.getType() == DdMetaVariable<storm::dd::DdType::CUDD>::MetaVariableType::Bool) {
                    variablePairs.emplace_back(metaVariable.getDdVariables().front().getIndex(), variablePair.first.getName());
                } else {
                    // For integer-valued meta variables, we, however, have to add the suffix.
                    for (uint_fast64_t variableIndex = 0; variableIndex < metaVariable.getNumberOfDdVariables(); ++variableIndex) {
                        variablePairs.emplace_back(metaVariable.getDdVariables()[variableIndex].getIndex(), variablePair.first.getName() + '.' + std::to_string(variableIndex));
                    }
                }
            }
            
            // Then, we sort this list according to the indices of the ADDs.
            std::sort(variablePairs.begin(), variablePairs.end(), [](std::pair<uint_fast64_t, std::string> const& a, std::pair<uint_fast64_t, std::string> const& b) { return a.first < b.first; });
            
            // Now, we project the sorted vector to its second component.
            std::vector<std::string> result;
            for (auto const& element : variablePairs) {
                result.push_back(element.second);
            }
            
            return result;
        }
        
        std::vector<storm::expressions::Variable> DdManager<DdType::CUDD>::getDdVariables() const {
            // First, we initialize a list DD variables and their names.
            std::vector<std::pair<uint_fast64_t, storm::expressions::Variable>> variablePairs;
            for (auto const& variablePair : this->metaVariableMap) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = variablePair.second;
                // If the meta variable is of type bool, we don't need to suffix it with the bit number.
                if (metaVariable.getType() == DdMetaVariable<storm::dd::DdType::CUDD>::MetaVariableType::Bool) {
                    variablePairs.emplace_back(metaVariable.getDdVariables().front().getIndex(), variablePair.first);
                } else {
                    // For integer-valued meta variables, we, however, have to add the suffix.
                    for (uint_fast64_t variableIndex = 0; variableIndex < metaVariable.getNumberOfDdVariables(); ++variableIndex) {
                        variablePairs.emplace_back(metaVariable.getDdVariables()[variableIndex].getIndex(), variablePair.first);
                    }
                }
            }
            
            // Then, we sort this list according to the indices of the ADDs.
            std::sort(variablePairs.begin(), variablePairs.end(), [](std::pair<uint_fast64_t, storm::expressions::Variable> const& a, std::pair<uint_fast64_t, storm::expressions::Variable> const& b) { return a.first < b.first; });
            
            // Now, we project the sorted vector to its second component.
            std::vector<storm::expressions::Variable> result;
            for (auto const& element : variablePairs) {
                result.push_back(element.second);
            }
            
            return result;
        }
        
        void DdManager<DdType::CUDD>::allowDynamicReordering(bool value) {
            if (value) {
                this->getCuddManager().AutodynEnable(this->reorderingTechnique);
            } else {
                this->getCuddManager().AutodynDisable();
            }
        }
        
        bool DdManager<DdType::CUDD>::isDynamicReorderingAllowed() const {
            Cudd_ReorderingType type;
            return this->getCuddManager().ReorderingStatus(&type);
        }
        
        void DdManager<DdType::CUDD>::triggerReordering() {
            this->getCuddManager().ReduceHeap(this->reorderingTechnique, 0);
        }
        
        std::shared_ptr<DdManager<DdType::CUDD> const> DdManager<DdType::CUDD>::asSharedPointer() const {
            return this->shared_from_this();
        }
    }
}