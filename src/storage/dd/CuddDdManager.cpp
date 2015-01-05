#include <cmath>
#include <string>
#include <algorithm>

#include "src/storage/dd/CuddDdManager.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/settings/SettingsManager.h"

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
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getOne() {
            return Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addOne());
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getZero() {
            return Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addZero());
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getConstant(double value) {
            return Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.constant(value));
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getEncoding(storm::expressions::Variable const& variable, int_fast64_t value) {
            DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(variable);
            
            STORM_LOG_THROW(value >= metaVariable.getLow() && value <= metaVariable.getHigh(), storm::exceptions::InvalidArgumentException, "Illegal value " << value << " for meta variable '" << variable.getName() << "'.");
            
            // Now compute the encoding relative to the low value of the meta variable.
            value -= metaVariable.getLow();
            
            std::vector<Dd<DdType::CUDD>> const& ddVariables = metaVariable.getDdVariables();

            Dd<DdType::CUDD> result;
            if (value & (1ull << (ddVariables.size() - 1))) {
                result = ddVariables[0];
            } else {
                result = !ddVariables[0];
            }
            
            for (std::size_t i = 1; i < ddVariables.size(); ++i) {
                if (value & (1ull << (ddVariables.size() - i - 1))) {
                    result *= ddVariables[i];
                } else {
                    result *= !ddVariables[i];
                }
            }
                        
            return result;
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getRange(storm::expressions::Variable const& variable) {
            storm::dd::DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(variable);
            
            Dd<DdType::CUDD> result = this->getZero();
            
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result.setValue(variable, value, static_cast<double>(1));
            }
            return result;
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getIdentity(storm::expressions::Variable const& variable) {
            storm::dd::DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(variable);
            
            Dd<DdType::CUDD> result = this->getZero();
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result.setValue(variable, value, static_cast<double>(value));
            }
            return result;
        }

        std::pair<storm::expressions::Variable, storm::expressions::Variable> DdManager<DdType::CUDD>::addMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high) {
            // Check whether the variable name is legal.
            STORM_LOG_THROW(name != "" && name.back() != '\'', storm::exceptions::InvalidArgumentException, "Illegal name of meta variable: '" << name << "'.");
            
            // Check whether a meta variable already exists.
            STORM_LOG_THROW(!this->hasMetaVariable(name), storm::exceptions::InvalidArgumentException, "A meta variable '" << name << "' already exists.");

            // Check that the range is legal.
            STORM_LOG_THROW(high != low, storm::exceptions::InvalidArgumentException, "Range of meta variable must be at least 2 elements.");
            
            std::size_t numberOfBits = static_cast<std::size_t>(std::ceil(std::log2(high - low + 1)));
            
            storm::expressions::Variable unprimed = manager->declareVariable(name, manager->getBoundedIntegerType(numberOfBits));
            storm::expressions::Variable primed = manager->declareVariable(name + "'", manager->getBoundedIntegerType(numberOfBits));
            
            std::vector<Dd<DdType::CUDD>> variables;
            std::vector<Dd<DdType::CUDD>> variablesPrime;
            for (std::size_t i = 0; i < numberOfBits; ++i) {
                variables.emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {unprimed}));
                variablesPrime.emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {primed}));
            }
            
            // Now group the non-primed and primed variable.
            for (uint_fast64_t i = 0; i < numberOfBits; ++i) {
                this->getCuddManager().MakeTreeNode(variables[i].getCuddAdd().NodeReadIndex(), 2, MTR_FIXED);
            }
            
            metaVariableMap.emplace(unprimed, DdMetaVariable<DdType::CUDD>(name, low, high, variables, this->shared_from_this()));
            metaVariableMap.emplace(primed, DdMetaVariable<DdType::CUDD>(name + "'", low, high, variablesPrime, this->shared_from_this()));
            
            return std::make_pair(unprimed, primed);
        }
        
        std::pair<storm::expressions::Variable, storm::expressions::Variable> DdManager<DdType::CUDD>::addMetaVariable(std::string const& name) {
            // Check whether the variable name is legal.
            STORM_LOG_THROW(name != "" && name.back() != '\'', storm::exceptions::InvalidArgumentException, "Illegal name of meta variable: '" << name << "'.");
            
            // Check whether a meta variable already exists.
            STORM_LOG_THROW(!this->hasMetaVariable(name), storm::exceptions::InvalidArgumentException, "A meta variable '" << name << "' already exists.");
            
            storm::expressions::Variable unprimed = manager->declareVariable(name, manager->getBooleanType());
            storm::expressions::Variable primed = manager->declareVariable(name + "'", manager->getBooleanType());

            std::vector<Dd<DdType::CUDD>> variables;
            std::vector<Dd<DdType::CUDD>> variablesPrime;
            variables.emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {unprimed}));
            variablesPrime.emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {primed}));

            // Now group the non-primed and primed variable.
            this->getCuddManager().MakeTreeNode(variables.front().getCuddAdd().NodeReadIndex(), 2, MTR_FIXED);
            
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
            std::vector<std::pair<ADD, std::string>> variableNamePairs;
            for (auto const& nameMetaVariablePair : this->metaVariableMap) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = nameMetaVariablePair.second;
                // If the meta variable is of type bool, we don't need to suffix it with the bit number.
                if (metaVariable.getType() == DdMetaVariable<storm::dd::DdType::CUDD>::MetaVariableType::Bool) {
                    variableNamePairs.emplace_back(metaVariable.getDdVariables().front().getCuddAdd(), metaVariable.getName());
                } else {
                    // For integer-valued meta variables, we, however, have to add the suffix.
                    for (uint_fast64_t variableIndex = 0; variableIndex < metaVariable.getNumberOfDdVariables(); ++variableIndex) {
                        variableNamePairs.emplace_back(metaVariable.getDdVariables()[variableIndex].getCuddAdd(), metaVariable.getName() + "." + std::to_string(variableIndex));
                    }
                }
            }
            
            // Then, we sort this list according to the indices of the ADDs.
            std::sort(variableNamePairs.begin(), variableNamePairs.end(), [](std::pair<ADD, std::string> const& a, std::pair<ADD, std::string> const& b) { return a.first.NodeReadIndex() < b.first.NodeReadIndex(); });
            
            // Now, we project the sorted vector to its second component.
            std::vector<std::string> result;
            for (auto const& element : variableNamePairs) {
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
    }
}