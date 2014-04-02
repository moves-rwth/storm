#ifndef STORM_STORAGE_EXPRESSIONS_UNARYEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_UNARYEXPRESSION_H_

namespace storm {
    namespace expressions {
        class UnaryExpression : public BaseExpression {
        public:
            UnaryExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& argument);
            virtual ~UnaryExpression() = default;
            
            std::unique_ptr<BaseExpression> const& getArgument() const;
        private:
            std::unique_ptr<BaseExpression> argument;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_UNARYEXPRESSION_H_ */