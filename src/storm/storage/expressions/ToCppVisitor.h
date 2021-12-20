#pragma once

#include <sstream>
#include <unordered_map>

#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/storage/expressions/Variable.h"

namespace storm {
namespace expressions {
class Expression;

enum class ToCppTranslationMode { KeepType, CastDouble, CastRationalNumber, CastRationalFunction };

class ToCppTranslationOptions {
   public:
    ToCppTranslationOptions(std::unordered_map<storm::expressions::Variable, std::string> const& prefixes,
                            std::unordered_map<storm::expressions::Variable, std::string> const& names,
                            ToCppTranslationMode mode = ToCppTranslationMode::KeepType);

    std::unordered_map<storm::expressions::Variable, std::string> const& getPrefixes() const;
    std::unordered_map<storm::expressions::Variable, std::string> const& getNames() const;
    ToCppTranslationMode const& getMode() const;

   private:
    std::reference_wrapper<std::unordered_map<storm::expressions::Variable, std::string> const> prefixes;
    std::reference_wrapper<std::unordered_map<storm::expressions::Variable, std::string> const> names;
    ToCppTranslationMode mode;
};

class ToCppVisitor : public ExpressionVisitor {
   public:
    std::string translate(storm::expressions::Expression const& expression, ToCppTranslationOptions const& options);

    virtual boost::any visit(IfThenElseExpression const& expression, boost::any const& data) override;
    virtual boost::any visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) override;
    virtual boost::any visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) override;
    virtual boost::any visit(BinaryRelationExpression const& expression, boost::any const& data) override;
    virtual boost::any visit(VariableExpression const& expression, boost::any const& data) override;
    virtual boost::any visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) override;
    virtual boost::any visit(UnaryNumericalFunctionExpression const& expression, boost::any const& data) override;
    virtual boost::any visit(BooleanLiteralExpression const& expression, boost::any const& data) override;
    virtual boost::any visit(IntegerLiteralExpression const& expression, boost::any const& data) override;
    virtual boost::any visit(RationalLiteralExpression const& expression, boost::any const& data) override;

   private:
    std::stringstream stream;
};

}  // namespace expressions
}  // namespace storm
