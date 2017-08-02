#include "storm/storage/dd/bisimulation/SignatureComputer.h"

#include "storm/storage/dd/DdManager.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/OutOfRangeException.h"

namespace storm {
    namespace dd {
        namespace bisimulation {

            template<storm::dd::DdType DdType, typename ValueType>
            SignatureIterator<DdType, ValueType>::SignatureIterator(SignatureComputer<DdType, ValueType> const& signatureComputer, Partition<DdType, ValueType> const& partition) : signatureComputer(signatureComputer), partition(partition), position(0) {
                // Intentionally left empty.
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            bool SignatureIterator<DdType, ValueType>::hasNext() const {
                if (signatureComputer.getSignatureMode() == SignatureMode::Eager) {
                    return position < 1;
                } else {
                    return position < 2;
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Signature<DdType, ValueType> SignatureIterator<DdType, ValueType>::next() {
                auto mode = signatureComputer.getSignatureMode();
                STORM_LOG_THROW((mode == SignatureMode::Eager && position < 1) || (mode == SignatureMode::Lazy && position < 2), storm::exceptions::OutOfRangeException, "Iterator is out of range.");
                Signature<DdType, ValueType> result;
                
                if (mode == SignatureMode::Eager || position == 1) {
                    result = signatureComputer.getFullSignature(partition);
                } else if (position == 0) {
                    result = signatureComputer.getQualitativeSignature(partition);
                }
                
                ++position;
                return result;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            SignatureComputer<DdType, ValueType>::SignatureComputer(storm::models::symbolic::Model<DdType, ValueType> const& model, SignatureMode const& mode) : SignatureComputer(model.getTransitionMatrix(), boost::none, model.getColumnVariables(), mode) {
                // Intentionally left empty.
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            SignatureComputer<DdType, ValueType>::SignatureComputer(storm::dd::Add<DdType, ValueType> const& transitionMatrix, std::set<storm::expressions::Variable> const& columnVariables, SignatureMode const& mode) : SignatureComputer(transitionMatrix, boost::none, columnVariables, mode) {
                // Intentionally left empty.
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            SignatureComputer<DdType, ValueType>::SignatureComputer(storm::dd::Bdd<DdType> const& qualitativeTransitionMatrix, std::set<storm::expressions::Variable> const& columnVariables, SignatureMode const& mode) : SignatureComputer(qualitativeTransitionMatrix.template toAdd<ValueType>(), boost::none, columnVariables, mode) {
                // Intentionally left empty.
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            SignatureComputer<DdType, ValueType>::SignatureComputer(storm::dd::Add<DdType, ValueType> const& transitionMatrix, boost::optional<storm::dd::Bdd<DdType>> const& qualitativeTransitionMatrix, std::set<storm::expressions::Variable> const& columnVariables, SignatureMode const& mode) : transitionMatrix(transitionMatrix), columnVariables(columnVariables), mode(mode) {
                if (DdType == storm::dd::DdType::Sylvan) {
                    this->transitionMatrix = this->transitionMatrix.notZero().ite(this->transitionMatrix, this->transitionMatrix.getDdManager().template getAddUndefined<ValueType>());
                }
                
                if (qualitativeTransitionMatrix) {
                    if (DdType == storm::dd::DdType::Sylvan) {
                        this->transitionMatrix01 = qualitativeTransitionMatrix.get().ite(this->transitionMatrix.getDdManager().template getAddOne<ValueType>(), this->transitionMatrix.getDdManager().template getAddUndefined<ValueType>());
                    } else {
                        this->transitionMatrix01 = qualitativeTransitionMatrix.get().template toAdd<ValueType>();
                    }
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            SignatureIterator<DdType, ValueType> SignatureComputer<DdType, ValueType>::compute(Partition<DdType, ValueType> const& partition) {
                return SignatureIterator<DdType, ValueType>(*this, partition);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            void SignatureComputer<DdType, ValueType>::setSignatureMode(SignatureMode const& newMode) {
                this->mode = newMode;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            SignatureMode const& SignatureComputer<DdType, ValueType>::getSignatureMode() const {
                return mode;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Signature<DdType, ValueType> SignatureComputer<DdType, ValueType>::getFullSignature(Partition<DdType, ValueType> const& partition) const {
                this->transitionMatrix.exportToDot("trans.dot");
                if (partition.storedAsBdd()) {
                    return Signature<DdType, ValueType>(this->transitionMatrix.multiplyMatrix(partition.asBdd(), columnVariables));
                } else {
                    auto result = Signature<DdType, ValueType>(this->transitionMatrix.multiplyMatrix(partition.asAdd(), columnVariables));
                    
                    std::cout << "abstracting vars" << std::endl;
                    for (auto const& v : columnVariables) {
                        std::cout << v.getName() << std::endl;
                    }
                    std::cout << "----" << std::endl;
                    result.getSignatureAdd().exportToDot("fullsig.dot");
                    
                    return result;
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Signature<DdType, ValueType> SignatureComputer<DdType, ValueType>::getQualitativeSignature(Partition<DdType, ValueType> const& partition) const {
                if (this->mode == SignatureMode::Lazy && !transitionMatrix01) {
                    if (DdType == storm::dd::DdType::Sylvan) {
                        this->transitionMatrix01 = this->transitionMatrix.notZero().ite(this->transitionMatrix.getDdManager().template getAddOne<ValueType>(), this->transitionMatrix.getDdManager().template getAddUndefined<ValueType>());
                    } else {
                        this->transitionMatrix01 = this->transitionMatrix.notZero().template toAdd<ValueType>();
                    }
                }

                if (partition.storedAsBdd()) {
                    return Signature<DdType, ValueType>(this->transitionMatrix01.get().multiplyMatrix(partition.asBdd(), columnVariables));
                } else {
                    return Signature<DdType, ValueType>(this->transitionMatrix01.get().multiplyMatrix(partition.asAdd(), columnVariables));
                }
            }

            template class SignatureIterator<storm::dd::DdType::CUDD, double>;
            template class SignatureIterator<storm::dd::DdType::Sylvan, double>;
            template class SignatureIterator<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class SignatureIterator<storm::dd::DdType::Sylvan, storm::RationalFunction>;
            
            template class SignatureComputer<storm::dd::DdType::CUDD, double>;
            template class SignatureComputer<storm::dd::DdType::Sylvan, double>;
            template class SignatureComputer<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class SignatureComputer<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        }
    }
}
