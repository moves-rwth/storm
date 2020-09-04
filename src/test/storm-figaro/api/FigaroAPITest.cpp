#include "test/storm_gtest.h"
#include "storm-config.h"
#include "storm/api/verification.h"
#include "storm-figaro/api/storm-figaro.h"
#include "figaro-test-headers.h"

#include <string>
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include <iostream>

namespace {

    class FigaroAPITest : public ::testing::Test {
    public:

        FigaroAPITest() {}

        double analyzeUnReliability(std::shared_ptr<storm::figaro::FigaroProgram> figaromodel) {
            std::string property = "Pmin=? [F<=100 \"failed\"]";
            std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(
                    storm::api::parseProperties(property));

            storm::figaro::modelchecker::FigaroModelChecker<double>::figaro_results results =
                    storm::figaro::api::analyzeFigaro<double>(*figaromodel, properties);
            return boost::get<double>(results[0]);
        }
        double analyzeUnAvailability(std::shared_ptr<storm::figaro::FigaroProgram> figaromodel) {
            std::string property = "Pmin=? [F[100,100] \"failed\"]";
            std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(
                    storm::api::parseProperties(property));

            storm::figaro::modelchecker::FigaroModelChecker<double>::figaro_results results =
                    storm::figaro::api::analyzeFigaro<double>(*figaromodel, properties);
            return boost::get<double>(results[0]);
        }
    };





    TEST_F(FigaroAPITest, first) {
        double result =  this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram1>(storm::figaro::FigaroProgram1()));
        EXPECT_FLOAT_EQ( result, 0.70904977);
        result =  this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram1>(storm::figaro::FigaroProgram1()));
        EXPECT_FLOAT_EQ( result, 0.21117046);
    }
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_No_Trim_No_Repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_No_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_No_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_No_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_No_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_Trim_Max_Repair) {
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_Trim_article_Repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_Trim_Max_No_Repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_No_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_No_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_No_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_No_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_No_Trim_Repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_Trim_article_No_Repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_No_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_No_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_No_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_No_Repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 13_Share1_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 13_Share1_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 13_Share1_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 13_Share1_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 13_Share1_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 13_Share1_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 04_Demoeng_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 04_Demoeng_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 04_Demoeng_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 04_Demoeng_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 04_Demoeng_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 04_Demoeng_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 05_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_05_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_05_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 05_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 05_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 05_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 05_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_05_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_05_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 05_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 08_PC_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 08_PC_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 08_PC_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 08_PC_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 08_PC_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 08_PC_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 03_CCF_No_Trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_No_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_No_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 03_CCF_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 03_CCF_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 03_CCF_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 03_CCF_No_Trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 03_CCF_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 09_Phases_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 09_Phases_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 09_Phases_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 09_Phases_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 09_Phases_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 09_Phases_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 07Excl1_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.45);
}
    TEST_F(FigaroAPITest, 07Excl1_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 07Excl1_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 07Excl1_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 07Excl1_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 07Excl1_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, figaro_Petrinet) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_figaro_Petrinet>(storm::figaro::FigaroProgram_figaro_Petrinet()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_figaro_Petrinet>(storm::figaro::FigaroProgram_figaro_Petrinet()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest,         figaro_BDMP) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_figaro_BDMP>(storm::figaro::FigaroProgram_figaro_BDMP()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_figaro_BDMP>(storm::figaro::FigaroProgram_figaro_BDMP()));
        EXPECT_FLOAT_EQ(result,1234.45);
}
    TEST_F(FigaroAPITest, 19_ESREL_2013_v2_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 19_ESREL_2013_v2_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 19_ESREL_2013_v2_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, figaro_Telecom) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_figaro_Telecom>(storm::figaro::FigaroProgram_figaro_Telecom()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_figaro_Telecom>(storm::figaro::FigaroProgram_figaro_Telecom()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 10_Project_risks_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456
        );
}
    TEST_F(FigaroAPITest, 10_Project_risks_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 10_Project_risks_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 10_Project_risks_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456
        );
}
    TEST_F(FigaroAPITest, 10_Project_risks_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 10_Project_risks_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 16_Cab2_eng_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 16_Cab2_eng_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 16_Cab2_eng_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456
        );
}
    TEST_F(FigaroAPITest, 16_Cab2_eng_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 16_Cab2_eng_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 16_Cab2_eng_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, figaro_RBD) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_figaro_RBD>(storm::figaro::FigaroProgram_figaro_RBD()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_figaro_RBD>(storm::figaro::FigaroProgram_figaro_RBD()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 21_Remote_Access_Server_Security_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 21_Remote_Access_Server_Security_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 21_Remote_Access_Server_Security_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 01_2trainsElec_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 01_2trainsElec_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 01_2trainsElec_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 01_2trainsElec_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 01_2trainsElec_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 01_2trainsElec_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, figaro_Miniplant) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_figaro_Miniplant>(storm::figaro::FigaroProgram_figaro_Miniplant()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_figaro_Miniplant>(storm::figaro::FigaroProgram_figaro_Miniplant()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 15_No_trim_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_15_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_15_No_trim_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 15_Trim_Max_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_15_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_15_Trim_Max_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 15_Trim_Article_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 15_Trim_Max_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_15_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_15_Trim_Max_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 15_No_trim_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_15_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_15_No_trim_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}
    TEST_F(FigaroAPITest, 15_Trim_Article_No_repair) { 
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_15_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_15_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_15_Trim_Article_No_repair()));
        EXPECT_FLOAT_EQ(result,1234.456);
}

} //name space
