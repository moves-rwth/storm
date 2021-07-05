#pragma once

#include "JaniType.h"

namespace storm {
    namespace jani {
        class BoundedType : public JaniType {

        public:
            BoundedType (ElementType const& type, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound);
            virtual bool isBoundedType() const override;
            virtual bool isIntegerType() const override;
            virtual bool isRealType() const override;
            std::string getStringRepresentation() const override;
            void setLowerBound(storm::expressions::Expression const& expression) override;
            void setUpperBound(storm::expressions::Expression const& expression) override;
            storm::expressions::Expression const& getLowerBound() const override;
            storm::expressions::Expression const& getUpperBound() const override;



        private:
            ElementType type;
            storm::expressions::Expression lowerBound;
            storm::expressions::Expression upperBound;
        };
    }
}
