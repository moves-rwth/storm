#pragma once

#include "JaniType.h"

namespace storm {
    namespace jani {
        class BasicType : public JaniType {

        public:
             enum class Type {
                 Bool,
                 Int,
                 Real
             };

            BasicType (Type const& type);
            virtual bool isBasicType() const override;
            
            Type const& get() const;
            bool isBooleanType() const;
            bool isIntegerType() const;
            bool isRealType() const;
            
            virtual std::string getStringRepresentation() const override;
            virtual std::unique_ptr<JaniType> clone() const override;

        private:
            Type type;
        };
    }
}
