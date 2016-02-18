#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/FormulaParser.h"
#include "src/logic/FragmentSpecification.h"
#include "src/exceptions/WrongFormatException.h"

TEST(FragmentCheckerTest, PctlTest) {
    storm::parser::FormulaParser formulaParser;
    
    std::string input = "\"label\"";
    std::shared_ptr<const storm::logic::Formula> formula(nullptr);
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString(input));
    
}
