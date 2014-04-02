#ifndef STORM_STORAGE_EXPRESSIONS_UNARYBOOLEANFUNCTIONEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_UNARYBOOLEANFUNCTIONEXPRESSION_H_

namespace storm {
    namespace expressions {
        class UnaryBooleanFunctionExpression : public UnaryExpression {
            /*!
             * An enum type specifying the different functions applicable.
             */
            enum FunctionType {NOT};
            
            UnaryBooleanFunctionExpression(ReturnType returnType, std::unique_ptr<BaseExpression>&& argument, FunctionType functionType);
            virtual ~UnaryBooleanFunctionExpression() = default;
            
        private:
            FunctionType FunctionType;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_UNARYBOOLEANFUNCTIONEXPRESSION_H_ */
