#ifndef STORM_STORAGE_EXPRESSIONS_INTEGERCONSTANTEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_INTEGERCONSTANTEXPRESSION_H_

namespace storm {
    namespace expressions {
        class IntegerConstantExpression : public ConstantExpression {
        public:
            IntegerConstantExpression(std::string const& constantName);
            virtual ~IntegerConstantExpression() = default;
            
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_INTEGERCONSTANTEXPRESSION_H_ */
