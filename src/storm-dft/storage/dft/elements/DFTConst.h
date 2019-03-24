#pragma once


#include "DFTElement.h"
namespace storm {
    namespace storage {
        template<typename ValueType>
        class DFTConst : public DFTElement<ValueType> {

            bool mFailed;

        public:
            DFTConst(size_t id, std::string const& name, bool failed) :
                    DFTElement<ValueType>(id, name), mFailed(failed)
            {}

            DFTElementType type() const override {
                if(mFailed) {
                    return DFTElementType::CONSTF;
                } else {
                    return DFTElementType::CONSTS;
                }
            }


            bool failed() const {
                return mFailed;
            }
            
            virtual bool isConstant() const override {
                return true;
            }
            
            virtual size_t nrChildren() const override {
                return 0;
            }

            std::string toString() const override {
                std::stringstream stream;
                stream << *this;
                return stream.str();
            }

            bool isTypeEqualTo(DFTElement<ValueType> const& other) const override {
                if(!DFTElement<ValueType>::isTypeEqualTo(other)) return false;
                DFTConst<ValueType> const& otherCNST = static_cast<DFTConst<ValueType> const&>(other);
                return (mFailed == otherCNST.mFailed);
            }

        };

        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTConst<ValueType> const& be) {
            return os << "{" << be.name() << "} BE(const " << (be.failed() ? "failed" : "failsafe") << ")";
        }

    }
}
