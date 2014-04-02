#ifndef STORM_STORAGE_EXPRESSIONS_UNARYNUMERICALFUNCTIONEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_UNARYNUMERICALFUNCTIONEXPRESSION_H_

namespace storm {
    namespace expressions {
        class UnaryNumericalFunctionExpression : public UnaryExpression {
            /*!
             * An enum type specifying the different functions applicable.
             */
            enum FunctionType {MINUS, FLOOR, CEIL};
            
            UnaryNumericalFunctionExpression(ReturnType returnType, std::unique_ptr<BaseExpression>&& argument, FunctionType functionType);
            virtual ~UnaryNumericalFunctionExpression() = default;
            
        private:
            FunctionType FunctionType;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_UNARYNUMERICALFUNCTIONEXPRESSION_H_ */
