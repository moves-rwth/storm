#pragma once

#include "JaniType.h"

namespace storm {
    namespace jani {
        class ClockType : public JaniType {
        public:
            ClockType();
            virtual bool isClockType() const override;
            std::string getStringRepresentation() const override;


        private:

        };
    }
}
