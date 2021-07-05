#pragma once

#include "JaniType.h"

namespace storm {
    namespace jani {
        class BasicType : public JaniType {

        public:
            BasicType (ElementType const& type);
            virtual bool isBooleanType() const override;
            virtual bool isIntegerType() const override;
            virtual bool isRealType() const override;
            std::string getStringRepresentation() const override;


        private:
            ElementType type;
            bool bounded;
        };
    }
}
