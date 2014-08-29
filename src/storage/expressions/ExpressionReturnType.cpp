#include "src/storage/expressions/ExpressionReturnType.h"

namespace storm {
    namespace expressions {
        std::ostream& operator<<(std::ostream& stream, ExpressionReturnType const& enumValue) {
            switch (enumValue) {
                case ExpressionReturnType::Undefined: stream << "undefined"; break;
                case ExpressionReturnType::Bool: stream << "bool"; break;
                case ExpressionReturnType::Int: stream << "int"; break;
                case ExpressionReturnType::Double: stream << "double"; break;
            }
            return stream;
        }
    }
}