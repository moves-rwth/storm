#include "src/settings/Option.h"

#include <iomanip>

namespace storm {
    namespace settings {
        uint_fast64_t Option::getPrintLength() const {
            uint_fast64_t length = 2;
            if (!this->getRequiresModulePrefix()) {
                length += 2;
            }
            length += this->getModuleName().length() + 1;
            length += this->getLongName().length();
            if (this->getHasShortName()) {
                length += 4;
                if (!this->getRequiresModulePrefix()) {
                    length += 2;
                }
                length += this->getModuleName().length() + 1;
                length += this->getShortName().length();
            }
            return length;
        }
        
        std::vector<std::shared_ptr<ArgumentBase>> const& Option::getArguments() const {
            return this->arguments;
        }
        
        std::ostream& operator<<(std::ostream& out, Option const& option) {
            std::streamsize width = out.width();
            
            uint_fast64_t charactersPrinted = 0;
            out << std::setw(0) << "--";
            charactersPrinted += 2;
            if (!option.getRequiresModulePrefix()) {
                out << "[";
                ++charactersPrinted;
            }
            out <<  option.getModuleName() << ":";
            charactersPrinted += option.getModuleName().length() + 1;
            if (!option.getRequiresModulePrefix()) {
                out << "]";
                ++charactersPrinted;
            }
            out << option.getLongName();
            charactersPrinted += option.getLongName().length();
            if (option.getHasShortName()) {
                out << " | -";
                charactersPrinted += 4;
                if (!option.getRequiresModulePrefix()) {
                    out << "[";
                    ++charactersPrinted;
                }
                out << option.getModuleName() << ":";
                charactersPrinted += option.getModuleName().length() + 1;
                if (!option.getRequiresModulePrefix()) {
                    out << "]";
                    ++charactersPrinted;
                }
                out << option.getShortName();
                charactersPrinted += option.getShortName().length();
            }
            
            // Now fill the width.
            for (uint_fast64_t i = charactersPrinted; i < width; ++i) {
                out << out.fill();
            }
            
            out << "\t" << option.getDescription();
            
            if (option.getArgumentCount() > 0) {
                // Start by determining the longest print length of the arguments.
                uint_fast64_t maxLength = 0;
                for (auto const& argument : option.getArguments()) {
                    maxLength = std::max(maxLength, argument->getPrintLength());
                }
                
                for (auto const& argument : option.getArguments()) {
                    out << std::endl;
                    out << "\t* " << std::setw(maxLength) << std::left << *argument;
                }
            }
            
            return out;
        }
    }
}