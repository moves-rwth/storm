#include "storm/storage/dd/bisimulation/SignatureComputer.h"

#include "storm/storage/dd/DdManager.h"

namespace storm {
    namespace dd {
        namespace bisimulation {

            template<storm::dd::DdType DdType, typename ValueType>
            SignatureComputer<DdType, ValueType>::SignatureComputer(storm::models::symbolic::Model<DdType, ValueType> const& model) : model(model), transitionMatrix(model.getTransitionMatrix()) {
                if (DdType == storm::dd::DdType::Sylvan) {
                    this->transitionMatrix = this->transitionMatrix.notZero().ite(this->transitionMatrix, this->transitionMatrix.getDdManager().template getAddUndefined<ValueType>());
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Signature<DdType, ValueType> SignatureComputer<DdType, ValueType>::compute(Partition<DdType, ValueType> const& partition) {
                if (partition.storedAsBdd()) {
                    return Signature<DdType, ValueType>(this->transitionMatrix.multiplyMatrix(partition.asBdd(), model.getColumnVariables()));
                } else {
                    return Signature<DdType, ValueType>(this->transitionMatrix.multiplyMatrix(partition.asAdd(), model.getColumnVariables()));
                }
            }
            
            template class SignatureComputer<storm::dd::DdType::CUDD, double>;

            template class SignatureComputer<storm::dd::DdType::Sylvan, double>;
            template class SignatureComputer<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class SignatureComputer<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        }
    }
}
