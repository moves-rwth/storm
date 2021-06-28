#pragma once

#include "JaniType.h"

namespace storm {
    namespace jani {
        class ContinuousType : public JaniType {
        public:
            ContinuousType();
            virtual bool isContinuousType() const override;
            std::string getStringRepresentation() const override;

        private:

        };
    }
}
