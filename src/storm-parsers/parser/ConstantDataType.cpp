#include "ConstantDataType.h"

namespace storm {
namespace parser {
std::ostream& operator<<(std::ostream& out, ConstantDataType const& constantDataType) {
    switch (constantDataType) {
        case storm::parser::ConstantDataType::Bool:
            out << "Bool";
            break;
        case storm::parser::ConstantDataType::Integer:
            out << "Integer";
            break;
        case storm::parser::ConstantDataType::Rational:
            out << "Rational";
            break;
    }
    return out;
}
}  // namespace parser
}  // namespace storm
