#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/FormulaParser.h"
#include "src/exceptions/WrongFormatException.h"

TEST(FormulaParserTest, LabelTest) {
    storm::parser::FormulaParser parser;

    std::string input = "\"label\"";
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
	ASSERT_NO_THROW(formula = parser.parseFromString(input));

    EXPECT_TRUE(formula->isAtomicLabelFormula());
}

TEST(FormulaParserTest, ComplexLabelTest) {
    storm::parser::FormulaParser parser;

    std::string input = "!(\"a\" & \"b\") | \"a\" & !\"c\"";
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
	ASSERT_NO_THROW(formula = parser.parseFromString(input));

    EXPECT_TRUE(formula->isPropositionalFormula());
    EXPECT_TRUE(formula->isBinaryBooleanStateFormula());
}

TEST(FormulaParserTest, ExpressionTest) {
    std::shared_ptr<storm::expressions::ExpressionManager> manager(new storm::expressions::ExpressionManager());
    manager->declareBooleanVariable("x");
    manager->declareIntegerVariable("y");
    
    storm::parser::FormulaParser parser(manager);
    
    std::string input = "!(x | y > 3)";
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
    ASSERT_NO_THROW(formula = parser.parseFromString(input));
    
    EXPECT_TRUE(formula->isPropositionalFormula());
    EXPECT_TRUE(formula->isUnaryBooleanStateFormula());
}

TEST(FormulaParserTest, LabelAndExpressionTest) {
    std::shared_ptr<storm::expressions::ExpressionManager> manager(new storm::expressions::ExpressionManager());
    manager->declareBooleanVariable("x");
    manager->declareIntegerVariable("y");
    
    storm::parser::FormulaParser parser(manager);
    
    std::string input = "!\"a\" | x | y > 3";
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
    ASSERT_NO_THROW(formula = parser.parseFromString(input));
    
    EXPECT_TRUE(formula->isPropositionalFormula());
    
    input = "x | y > 3 | !\"a\"";
    ASSERT_NO_THROW(formula = parser.parseFromString(input));
    EXPECT_TRUE(formula->isPropositionalFormula());
}

TEST(FormulaParserTest, ProbabilityOperatorTest) {
    storm::parser::FormulaParser parser;

    std::string input = "P<0.9 [\"a\" U \"b\"]";
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
	ASSERT_NO_THROW(formula = parser.parseFromString(input));

    EXPECT_TRUE(formula->isProbabilityOperator());
    EXPECT_TRUE(formula->asProbabilityOperatorFormula().hasBound());
    EXPECT_FALSE(formula->asProbabilityOperatorFormula().hasOptimalityType());
}

TEST(FormulaParserTest, RewardOperatorTest) {
    storm::parser::FormulaParser parser;
    
    std::string input = "Rmin<0.9 [F \"a\"]";
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
    ASSERT_NO_THROW(formula = parser.parseFromString(input));
    
    EXPECT_TRUE(formula->isRewardOperator());
    EXPECT_TRUE(formula->asRewardOperatorFormula().hasBound());
    EXPECT_TRUE(formula->asRewardOperatorFormula().hasOptimalityType());
    
    input = "R=? [I=10]";
    ASSERT_NO_THROW(formula = parser.parseFromString(input));
    
    EXPECT_TRUE(formula->isRewardOperator());
    EXPECT_FALSE(formula->asRewardOperatorFormula().hasBound());
    EXPECT_FALSE(formula->asRewardOperatorFormula().hasOptimalityType());
    EXPECT_TRUE(formula->asRewardOperatorFormula().getSubformula().isInstantaneousRewardFormula());
}

TEST(FormulaParserTest, ConditionalProbabilityTest) {
    storm::parser::FormulaParser parser;
    
    std::string input = "P<0.9 [F \"a\" || F \"b\"]";
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
    ASSERT_NO_THROW(formula = parser.parseFromString(input));
    
    EXPECT_TRUE(formula->isProbabilityOperator());
    storm::logic::ProbabilityOperatorFormula const& probFormula = formula->asProbabilityOperatorFormula();
    EXPECT_TRUE(probFormula.getSubformula().isConditionalPathFormula());
}

TEST(FormulaParserTest, NestedPathFormulaTest) {
    storm::parser::FormulaParser parser;
    
    std::string input = "P<0.9 [F X \"a\"]";
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
    ASSERT_NO_THROW(formula = parser.parseFromString(input));
    
    EXPECT_TRUE(formula->isProbabilityOperator());
    ASSERT_TRUE(formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula());
    ASSERT_TRUE(formula->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula().isNextFormula());
}

TEST(FormulaParserTest, CommentTest) {
    storm::parser::FormulaParser parser;

    std::string input = "// This is a comment. And this is a commented out formula: P<=0.5 [ F \"a\" ] The next line contains the actual formula. \n P<=0.5 [ X \"b\" ] // Another comment \n // And again: another comment.";
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
	ASSERT_NO_THROW(formula = parser.parseFromString(input));
    EXPECT_TRUE(formula->isProbabilityOperator());
    ASSERT_TRUE(formula->asProbabilityOperatorFormula().getSubformula().isNextFormula());
    ASSERT_TRUE(formula->asProbabilityOperatorFormula().getSubformula().asNextFormula().getSubformula().isAtomicLabelFormula());
}


TEST(FormulaParserTest, WrongFormatTest) {
    std::shared_ptr<storm::expressions::ExpressionManager> manager(new storm::expressions::ExpressionManager());
    manager->declareBooleanVariable("x");
    manager->declareIntegerVariable("y");
    
    storm::parser::FormulaParser parser(manager);
    std::string input = "P>0.5 [ a ]";
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
	EXPECT_THROW(formula = parser.parseFromString(input), storm::exceptions::WrongFormatException);
    
    input = "P=0.5 [F \"a\"]";
    EXPECT_THROW(formula = parser.parseFromString(input), storm::exceptions::WrongFormatException);

    input = "P>0.5 [F !(x = 0)]";
    EXPECT_THROW(formula = parser.parseFromString(input), storm::exceptions::WrongFormatException);

    input = "P>0.5 [F !y]";
    EXPECT_THROW(formula = parser.parseFromString(input), storm::exceptions::WrongFormatException);

    input = "P>0.5 [F y!=0)]";
    EXPECT_THROW(formula = parser.parseFromString(input), storm::exceptions::WrongFormatException);
}
