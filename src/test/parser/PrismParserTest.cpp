#include "gtest/gtest.h"
#include "storm-config.h"
#include "storm/parser/PrismParser.h"

TEST(PrismParser, StandardModelTest) {
    storm::prism::Program result;
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2.nm"));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds5_5.pm"));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2_2.nm"));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm"));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/firewire.nm"));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm"));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/leader3_5.pm"));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm"));
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0_collide.nm"));
}

TEST(PrismParser, SimpleTest) {
    std::string testInput =
    R"(dtmc
    module mod1
        b : bool;
        [a] true -> 1: (b'=true != false = b => false);
    endmodule)";
    
    storm::prism::Program result;
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"));
    EXPECT_EQ(1ul, result.getNumberOfModules());
    EXPECT_EQ(storm::prism::Program::ModelType::DTMC, result.getModelType());
    
    testInput =
    R"(mdp
    
    module main
        x : [1..5] init 1;
        [] x=1 -> 1:(x'=2);
        [] x=2 -> 1:(x'=3);
        [] x=3 -> 1:(x'=1);
        [] x=3 -> 1:(x'=4);
        [] x=4 -> 1:(x'=5);
        [] x=5 -> 1: true;
    endmodule)";
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"));
    EXPECT_EQ(1ul, result.getNumberOfModules());
    EXPECT_EQ(storm::prism::Program::ModelType::MDP, result.getModelType());
}

TEST(PrismParser, ComplexTest) {
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
    formula test3 = (a + b > 10 ? floor(e) : h) + a;
    
    global g : bool init false;
    global h : [0 .. b];
    
    module mod1
        i : bool;
        j : bool init c;
        k : [125..a] init a;

        [a] test&false -> (i'=true)&(k'=1+1) + 1 : (k'=floor(e) + max(k, b) - 1 + k);
        [b] true -> (i'=i);
    endmodule
                                              
    module mod2
        [] (k > 3) & false & (min(a, 0) < max(h, k)) -> 1-e: (g'=(1-a) * 2 + floor(f) > 2);
    endmodule
    
    module mod3 = mod1 [ i = i1, j = j1, k = k1 ] endmodule
                                                               
    label "mal" = max(a, 10) > 0;
    
    rewards "testrewards"
        [a] true : a + 7;
        max(f, a) <= 8 : 2*b;
    endrewards
                                                               
    rewards "testrewards2"
        [b] true : a + 7;
        max(f, a) <= 8 : 2*b;
    endrewards)";
    
    storm::prism::Program result;
    EXPECT_NO_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"));
    EXPECT_EQ(storm::prism::Program::ModelType::MA, result.getModelType());
    EXPECT_EQ(3ul, result.getNumberOfModules());
    EXPECT_EQ(2ul, result.getNumberOfRewardModels());
    EXPECT_EQ(1ul, result.getNumberOfLabels());
}

TEST(PrismParser, IllegalInputTest) {
    std::string testInput =
    R"(ctmc

    const int a;
    const bool a = true;
    
    module mod1
        c : [0 .. 8] init 1;
        [] c < 3 -> 2: (c' = c+1); 
    endmodule
    )";
    
    storm::prism::Program result;
    EXPECT_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"), storm::exceptions::WrongFormatException);
    
    testInput =
    R"(dtmc
    
    const int a;
    
    module mod1
        a : [0 .. 8] init 1;
        [] a < 3 -> 1: (a' = a+1); 
    endmodule)";
    
    EXPECT_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"), storm::exceptions::WrongFormatException);
    
    testInput =
    R"(dtmc
    
    const int a = 2;
    formula a = 41;
    
    module mod1
        c : [0 .. 8] init 1;
        [] c < 3 -> 1: (c' = c+1); 
    endmodule)";
    
    EXPECT_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"), storm::exceptions::WrongFormatException);
    
    testInput =
    R"(dtmc
    
    const int a = 2;
    
    init
        c > 3
    endinit
    
    module mod1
        c : [0 .. 8] init 1;
        [] c < 3 -> 1: (c' = c+1); 
    endmodule
        
    init
        c > 3
    endinit

    )";
    
    EXPECT_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"), storm::exceptions::WrongFormatException);
    
    testInput =
    R"(dtmc
    
    module mod1
        c : [0 .. 8] init 1;
        [] c < 3 -> 1: (c' = c+1);
    endmodule
                    
    module mod2
        [] c < 3 -> 1: (c' = c+1);
    endmodule)";
    
    EXPECT_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"), storm::exceptions::WrongFormatException);
                        
    testInput =
    R"(dtmc
                        
    module mod1
        c : [0 .. 8] init 1;
        [] c < 3 -> 1: (c' = c+1)&(c'=c-1);
    endmodule)";
                                        
    EXPECT_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"), storm::exceptions::WrongFormatException);
    
    testInput =
    R"(dtmc
    
    module mod1
        c : [0 .. 8] init 1;
        [] c < 3 -> 1: (c' = true || false);
    endmodule)";
    
    EXPECT_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"), storm::exceptions::WrongFormatException);
    
    testInput =
    R"(dtmc
    
    module mod1
        c : [0 .. 8] init 1;
        [] c + 3 -> 1: (c' = 1);
    endmodule)";
    
    EXPECT_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"), storm::exceptions::WrongFormatException);
    
    testInput =
    R"(dtmc
    
    module mod1
        c : [0 .. 8] init 1;
        [] c + 3 -> 1: (c' = 1);
    endmodule
    
    label "test" = c + 1;
    
    )";
    
    EXPECT_THROW(result = storm::parser::PrismParser::parseFromString(testInput, "testfile"), storm::exceptions::WrongFormatException);
}
