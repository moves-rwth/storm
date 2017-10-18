#include "storm/storage/dd/bisimulation/PartialQuotientExtractor.h"

#include "storm/storage/dd/DdManager.h"

#include "storm/settings/SettingsManager.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace dd {
        namespace bisimulation {

            template<storm::dd::DdType DdType, typename ValueType>
            PartialQuotientExtractor<DdType, ValueType>::PartialQuotientExtractor(storm::models::symbolic::Model<DdType, ValueType> const& model) : model(model) {
                auto const& settings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
                this->quotientFormat = settings.getQuotientFormat();
                
                STORM_LOG_THROW(this->quotientFormat == storm::settings::modules::BisimulationSettings::QuotientFormat::Dd, storm::exceptions::NotSupportedException, "Only DD-based partial quotient extraction is currently supported.");
                
                // We need to create auxiliary variables that encode the nondeterminism arising from partion quotient extraction.
                auto& manager = model.getManager();
                uint64_t numberOfDdVariables = 0;
                for (auto const& metaVariable : model.getRowVariables()) {
                    auto const& ddMetaVariable = manager.getMetaVariable(metaVariable);
                    numberOfDdVariables += ddMetaVariable.getNumberOfDdVariables();
                }
                std::vector<storm::expressions::Variable> auxVariables = manager.addBitVectorMetaVariable("quot", numberOfDdVariables, 1);
                STORM_LOG_ASSERT(auxVariables.size() == 1, "Expected exactly one meta variable.");
                nondeterminismVariable = auxVariables.front();
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::Model<ValueType>> PartialQuotientExtractor<DdType, ValueType>::extract(Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation) {
                auto start = std::chrono::high_resolution_clock::now();
                std::shared_ptr<storm::models::Model<ValueType>> result;

                STORM_LOG_THROW(this->quotientFormat == storm::settings::modules::BisimulationSettings::QuotientFormat::Dd, storm::exceptions::NotSupportedException, "Only DD-based partial quotient extraction is currently supported.");
                result = extractDdQuotient(partition, preservationInformation);
                auto end = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Quotient extraction completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");
                
                STORM_LOG_THROW(result, storm::exceptions::NotSupportedException, "Quotient could not be extracted.");
                
                return result;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> PartialQuotientExtractor<DdType, ValueType>::extractDdQuotient(Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation) {
                return nullptr;
            }
            
            template class PartialQuotientExtractor<storm::dd::DdType::CUDD, double>;
            template class PartialQuotientExtractor<storm::dd::DdType::Sylvan, double>;

#ifdef STORM_HAVE_CARL
            template class PartialQuotientExtractor<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class PartialQuotientExtractor<storm::dd::DdType::Sylvan, storm::RationalFunction>;
#endif
            
        }
    }
}
