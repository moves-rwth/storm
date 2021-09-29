#pragma once

#include "JaniType.h"

namespace storm {
    namespace jani {
        class ArrayType : public JaniType {
        public:
            ArrayType(JaniType const& baseType);
            ArrayType(std::unique_ptr<JaniType>&& baseType);
            virtual ~ArrayType() = default;

            bool isArrayType() const override;
            
            /*!
             * @return the type of which this is an array, e.g., the baseType of int[][] is int[].
             */
            JaniType& getBaseType();
            JaniType const& getBaseType() const;
            
            /*!
             * @return the most basic base type that is not an array, e.g., for int[][] it is int.
             */
            JaniType const& getBaseTypeRecursive() const;
            
            virtual std::string getStringRepresentation() const override;
            virtual void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) override;
            virtual std::unique_ptr<JaniType> clone() const override;

        private:
            std::unique_ptr<JaniType> baseType;
        };
    }
}

