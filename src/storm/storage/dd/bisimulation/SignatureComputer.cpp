#include "storm/storage/dd/bisimulation/SignatureComputer.h"

#include "storm/storage/dd/DdManager.h"

namespace storm {
    namespace dd {
        namespace bisimulation {

            template<storm::dd::DdType DdType, typename ValueType>
            SignatureComputer<DdType, ValueType>::SignatureComputer(storm::models::symbolic::Model<DdType, ValueType> const& model, SignatureMode const& mode) : model(model), transitionMatrix(model.getTransitionMatrix()), mode(mode) {
                if (DdType == storm::dd::DdType::Sylvan) {
                    this->transitionMatrix = this->transitionMatrix.notZero().ite(this->transitionMatrix, this->transitionMatrix.getDdManager().template getAddUndefined<ValueType>());
                }
                if (mode == SignatureMode::Lazy) {
                    if (DdType == storm::dd::DdType::Sylvan) {
                        this->transitionMatrix01 = model.getQualitativeTransitionMatrix().ite(this->transitionMatrix.getDdManager().template getAddOne<ValueType>(), this->transitionMatrix.getDdManager().template getAddUndefined<ValueType>());
                    } else {
                        this->transitionMatrix01 = model.getQualitativeTransitionMatrix().template toAdd<ValueType>();
                    }
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            uint64_t SignatureComputer<DdType, ValueType>::getNumberOfSignatures() const {
                return this->mode == SignatureMode::Lazy ? 2 : 1;
            }

            template<storm::dd::DdType DdType, typename ValueType>
            Signature<DdType, ValueType> SignatureComputer<DdType, ValueType>::compute(Partition<DdType, ValueType> const& partition, uint64_t index) {
                if (mode == SignatureMode::Lazy && index == 1) {
                    return getQualitativeSignature(partition);
                } else {
                    return getFullSignature(partition);
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Signature<DdType, ValueType> SignatureComputer<DdType, ValueType>::getFullSignature(Partition<DdType, ValueType> const& partition) const {
                if (partition.storedAsBdd()) {
                    return Signature<DdType, ValueType>(this->transitionMatrix.multiplyMatrix(partition.asBdd(), model.getColumnVariables()));
                } else {
                    return Signature<DdType, ValueType>(this->transitionMatrix.multiplyMatrix(partition.asAdd(), model.getColumnVariables()));
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Signature<DdType, ValueType> SignatureComputer<DdType, ValueType>::getQualitativeSignature(Partition<DdType, ValueType> const& partition) const {
                if (partition.storedAsBdd()) {
                    return Signature<DdType, ValueType>(this->transitionMatrix01.multiplyMatrix(partition.asBdd(), model.getColumnVariables()));
                } else {
                    return Signature<DdType, ValueType>(this->transitionMatrix01.multiplyMatrix(partition.asAdd(), model.getColumnVariables()));
                }
            }
            
            template class SignatureComputer<storm::dd::DdType::CUDD, double>;

            template class SignatureComputer<storm::dd::DdType::Sylvan, double>;
            template class SignatureComputer<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class SignatureComputer<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        }
    }
}
