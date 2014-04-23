#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrismParser.h"

TEST(PrismParser, SimpleParsingOnlyTest) {
    std::string testInput =
    R"(dtmc
    module mod1
        b : bool;
        [a] true -> 1: (b'=true);
    endmodule)";
    
    storm::prism::Program result;
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile", false));
    EXPECT_EQ(1, result.getNumberOfModules());
    EXPECT_EQ(storm::prism::Program::ModelType::DTMC, result.getModelType());
}

TEST(PrismParser, StandardModelParsingTest) {
    storm::prism::Program result;
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/coin2.nm", false));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/crowds5_5.pm", false));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/csma2_2.nm", false));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/die.pm", false));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/firewire.nm", false));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/leader3.nm", false));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/leader3_5.pm", false));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/two_dice.nm", false));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/wlan0_collide.nm", false));
}

TEST(PrismParser, StandardModelFullTest) {
    storm::prism::Program result;
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/coin2.nm", true));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/crowds5_5.pm", true));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/csma2_2.nm", true));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/die.pm", true));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/firewire.nm", true));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/leader3.nm", true));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/leader3_5.pm", true));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/two_dice.nm", true));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/wlan0_collide.nm", true));
}

TEST(PrismParser, SimpleFullTest) {
    std::string testInput =
    R"(dtmc
    module mod1
        b : bool;
        [a] true -> 1: (b'=true != false = b => false);
    endmodule)";
    
    storm::prism::Program result;
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile", true));
    EXPECT_EQ(1, result.getNumberOfModules());
    EXPECT_EQ(storm::prism::Program::ModelType::DTMC, result.getModelType());
}

TEST(PrismParser, ComplexFullTest) {
    std::string testInput =
    R"(ma
    
    const int a;
    const int b = 10;
    const bool c;
    const bool d = true | false;
    const double e;
    const double f = 9;
    
    formula test = a >= 10 & (max(a,b) > floor(e));
    formula test2 = a+b;
    formula test3 = (a + b > 10 ? floor(a) : h) + a;
    
    global g : bool init false;
    global h : [0 .. b];
    
    module mod1
        i : bool;
        j : bool init c;
        k : [125..a] init a;

        [a] test&false -> (i'=true)&(k'=1+1) + 1 : (k'=floor(a) <= max(k, b) - 1 + k);
    endmodule
                                              
    module mod2
        [b] (k > 3) & false & (min(a, 0) < max(h, k)) -> 1-e: (j'=(1-a) * 2 + floor(f));
    endmodule
    
    module mod3 = mod1 [ i = i1, j = j1, k = k1 ] endmodule
                                                               
    label "mal" = max(a, 10) > 0;
                                                               
    init
        true
    endinit
    
    rewards "testrewards"
        [stateRew] true : a + 7;
        max(f, a) <= 8 : 2*b;
    endrewards
                                                               
    rewards "testrewards2"
        [stateRew] true : a + 7;
        max(f, a) <= 8 : 2*b;
    endrewards)";
    
    storm::prism::Program result;
    result = storm::parser::PrismParser::parseFromString(testInput, "testfile", true);
    EXPECT_EQ(storm::prism::Program::ModelType::MA, result.getModelType());
    EXPECT_EQ(3, result.getNumberOfModules());
    EXPECT_EQ(2, result.getNumberOfRewardModels());
    EXPECT_EQ(1, result.getNumberOfLabels());
    EXPECT_TRUE(result.definesInitialStatesExpression());
}

TEST(PrismParser, ComplexParsingTest) {
    std::string testInput =
    R"(ma
    
    const int a;
    const int b = 10;
    const bool c;
    const bool d = true | false;
    const double e;
    const double f = 9;
    
    formula test = (a >= 10 & (max(a, b) > floor(e)));

    global g : bool init false;
    global h : [0 .. b];
    
    module mod1
        i : bool;
        j : bool init c;
        k : [125..a] init a;
        [a] true -> (i'=true)&(h'=1+1) + 1 : (j'=floor(a) <= max(k, b) - 1 + k);
    endmodule
                                    
    module mod2
        [b] (x > 3) & false & (min(a, b0) < max(as8, b)) -> y: (x'=(1-g) * 2 + a);
        [] s=1 -> (a'=a);
        [read] c<N-1 -> 1 : (c'=floor(c) + 1);
    endmodule
                             
    module mod3 = mod2 [ x = y ] endmodule
             
    label "mal" = max(x, i) > 0;
                             
    init
        true
    endinit
                             
    rewards "testrewards"
        [stateRew] true : a + 7;
        max(z, f) <= 8 : 2*b;
    endrewards
                             
    rewards "testrewards2"
        [stateRew] true : a + 7;
        max(z, f) <= 8 : 2*b;
    endrewards)";
    
    storm::prism::Program result;
    result = storm::parser::PrismParser::parseFromString(testInput, "testfile", false);
    EXPECT_EQ(storm::prism::Program::ModelType::MA, result.getModelType());
    EXPECT_EQ(3, result.getNumberOfModules());
    EXPECT_EQ(2, result.getNumberOfRewardModels());
    EXPECT_EQ(1, result.getNumberOfLabels());
    EXPECT_TRUE(result.definesInitialStatesExpression());
}