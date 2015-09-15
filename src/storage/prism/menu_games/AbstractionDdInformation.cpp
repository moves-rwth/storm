#include "src/storage/prism/menu_games/AbstractionDdInformation.h"

#include <sstream>

#include "src/storage/expressions/Expression.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddBdd.h"
#include "src/storage/dd/CuddAdd.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractionDdInformation<DdType, ValueType>::AbstractionDdInformation(std::shared_ptr<storm::dd::DdManager<DdType>> const& manager) : manager(manager) {
                // Intentionally left empty.
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractionDdInformation<DdType, ValueType>::encodeDistributionIndex(uint_fast64_t numberOfVariables, uint_fast64_t distributionIndex) const {
                storm::dd::Bdd<DdType> result = manager->getBddOne();
                for (uint_fast64_t bitIndex = 0; bitIndex < numberOfVariables; ++bitIndex) {
                    if ((distributionIndex & 1) != 0) {
                        result &= optionDdVariables[bitIndex].second;
                    } else {
                        result &= !optionDdVariables[bitIndex].second;
                    }
                    distributionIndex >>= 1;
                }
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractionDdInformation<DdType, ValueType>::addPredicate(storm::expressions::Expression const& predicate) {
                std::stringstream stream;
                stream << predicate;
                std::pair<storm::expressions::Variable, storm::expressions::Variable> newMetaVariable = manager->addMetaVariable(stream.str());
                predicateDdVariables.push_back(newMetaVariable);
                predicateBdds.emplace_back(manager->getRange(newMetaVariable.first), manager->getRange(newMetaVariable.second));
                predicateIdentities.push_back(manager->getIdentity(newMetaVariable.first).equals(manager->getIdentity(newMetaVariable.second)).toBdd());
            }
         
            template struct AbstractionDdInformation<storm::dd::DdType::CUDD, double>;
            
        }
    }
}