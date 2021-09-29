#pragma once

#include "JaniType.h"

namespace storm {
    namespace jani {
        class ArrayType : public JaniType {
        public:
            ArrayType(JaniType const& baseType);
            ArrayType(std::unique_ptr<JaniType>&& baseType);
            
            bool isArrayType() const override;
            JaniType const& getBaseType() const;
            virtual std::string getStringRepresentation() const override;
            virtual void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) override;
            virtual std::unique_ptr<JaniType> clone() const override;

        private:
            std::unique_ptr<JaniType> baseType;
        };
    }
}

