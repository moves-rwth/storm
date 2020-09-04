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

        double analyzeUnReliability(std::shared_ptr<storm::figaro::FigaroProgram> figaromodel, uint32_t missiontime) {
            std::string property = "Pmin=? [F<=" + std::to_string(missiontime) + "\"failed\"]";
            std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(
                    storm::api::parseProperties(property));

            storm::figaro::modelchecker::FigaroModelChecker<double>::figaro_results results =
                    storm::figaro::api::analyzeFigaro<double>(*figaromodel, properties);
            return boost::get<double>(results[0]);
        }
        double analyzeUnAvailability(std::shared_ptr<storm::figaro::FigaroProgram> figaromodel, uint32_t missiontime) {
            std::string property = "Pmin=? [F[" + std::to_string(missiontime) + "," + std::to_string(missiontime) +"] \"failed\"]";
            std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(
                    storm::api::parseProperties(property));

            storm::figaro::modelchecker::FigaroModelChecker<double>::figaro_results results =
                    storm::figaro::api::analyzeFigaro<double>(*figaromodel, properties);
            return boost::get<double>(results[0]);
        }

    };


//    TEST_F(FigaroAPITest, first) {
//        double result =  this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram1>(storm::figaro::FigaroProgram1()), missiontime);
//        EXPECT_FLOAT_EQ( result, 0.000103935911);
//        result =  this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram1>(storm::figaro::FigaroProgram1()), missiontime);
//        EXPECT_FLOAT_EQ( result, 0.21117046);
//    }

///***********************           Test Case1              ***********************/
    TEST_F(FigaroAPITest, 01_2trainsElec_No_trim_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000103935911);
}
    TEST_F(FigaroAPITest, 01_2trainsElec_Trim_Max_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(
                std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_No_repair>(
                        storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result, 0.000103935911);

    }
    TEST_F(FigaroAPITest, 01_2trainsElec_Trim_Article_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000103935911);

}
    TEST_F(FigaroAPITest, 01_2trainsElec_No_trim_repair) {
        uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.002488490481);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001249092981);
}

    TEST_F(FigaroAPITest, 01_2trainsElec_Trim_Max_repair) {
        uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.002487996285);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001248511209);
}


    TEST_F(FigaroAPITest, 01_2trainsElec_Trim_Article_repair) {
        uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.002487996285);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_01_2trainsElec_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001248611032);
}
///***********************           Test Case 2              ***********************/

    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_No_trim_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0000003399185924);

}

    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_Trim_Max_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0000003399185924);

}

    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_Trim_Article_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0000003399185924);

}

    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_No_trim_repair) {
        uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(
                std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_repair>(
                        storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result, 0.000001146189041);
        result = this->analyzeUnAvailability(
                std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_repair>(
                        storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result, 0.0000000003994478745);
    }

    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_Trim_Max_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001146180739);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0000000003994423063);
}

    TEST_F(FigaroAPITest, 02_3trains_standby_redundancy_Trim_Article_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001146180739);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000000000399442315);
}



///***********************           Test Case 3              ***********************/
    TEST_F(FigaroAPITest, 03_CCF_No_Trim_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_No_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0002951056907);
}
    TEST_F(FigaroAPITest, 03_CCF_Trim_Max_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0002951056907);
    }
    TEST_F(FigaroAPITest, 03_CCF_Trim_Article_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0002951056907);
    }

    TEST_F(FigaroAPITest, 03_CCF_No_Trim_repair) {
        uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.005947705193);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_No_Trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000002992627521);
    }

    TEST_F(FigaroAPITest, 03_CCF_Trim_Max_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.005947704899);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000002991629812);
}

    TEST_F(FigaroAPITest, 03_CCF_Trim_Article_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.005947704899);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000002991630011);
}

///***********************           Test Case 4              ***********************/
    TEST_F(FigaroAPITest, 04_Demoeng_No_trim_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0001040405415);

}
    TEST_F(FigaroAPITest, 04_Demoeng_Trim_Max_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0001040405415);

    }
    TEST_F(FigaroAPITest, 04_Demoeng_Trim_Article_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0001040405415);

    }
    TEST_F(FigaroAPITest, 04_Demoeng_No_trim_repair) {
        uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.002200475631);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001102397375);
    }

    TEST_F(FigaroAPITest, 04_Demoeng_Trim_Max_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.002200433779);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001102173139);
}
    TEST_F(FigaroAPITest, 04_Demoeng_Trim_Article_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.002200433779);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_04_Demoeng_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001102173139);
}
///***********************           Test Case 05              ***********************/
    TEST_F(FigaroAPITest, 05_No_trim_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_05_No_trim_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0001019244817);

}
    TEST_F(FigaroAPITest, 05_Trim_Max_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Max_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0001019244817);

    }
    TEST_F(FigaroAPITest, 05_Trim_Article_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Article_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0001019244817);

    }
    TEST_F(FigaroAPITest, 05_No_trim_repair) {
        uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_05_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.001998998636);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_05_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001001677339);
    }
    TEST_F(FigaroAPITest, 05_Trim_Max_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.001998961649);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001001489273);
}
    TEST_F(FigaroAPITest, 05_Trim_Article_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.001998961649);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_05_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_05_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001001489273);
}

///***********************           Test Case 08              ***********************/
    TEST_F(FigaroAPITest, 08_PC_No_trim_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.04981195917);

}
    TEST_F(FigaroAPITest, 08_PC_Trim_Article_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.04981195917);

    }
    TEST_F(FigaroAPITest, 08_PC_Trim_Max_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.04981195917);

    }
    TEST_F(FigaroAPITest, 08_PC_No_trim_repair) {
        uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.9939153995);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.005086413952);
    }
    TEST_F(FigaroAPITest, 08_PC_Trim_Max_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.9939153995);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.005075551948);
}
    TEST_F(FigaroAPITest, 08_PC_Trim_Article_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.9939153995);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_08_PC_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.005077532246);
}
/***********************           Test Case 10              ***********************/

    TEST_F(FigaroAPITest, 10_Project_risks_No_trim_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.01985001112);


}
    TEST_F(FigaroAPITest, 10_Project_risks_Trim_Article_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.01985001112);


}
    TEST_F(FigaroAPITest, 10_Project_risks_Trim_Max_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.01985001112);


}
    TEST_F(FigaroAPITest, 10_Project_risks_No_trim_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.86479958);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.001997502498);
}
    TEST_F(FigaroAPITest, 10_Project_risks_Trim_Max_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.86479958);
        std::cout<<result;
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0019965051);
        std::cout<<result;
}
    TEST_F(FigaroAPITest, 10_Project_risks_Trim_Article_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.86479958);
        std::cout<<result;
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0019965058);
        std::cout<<result;
}



   /***********************           Test Case 13              ***********************/
    TEST_F(FigaroAPITest, 13_Share1_No_trim_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.00009999092446);

}
    TEST_F(FigaroAPITest, 13_Share1_Trim_Max_No_repair) {
        uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.00009999092446);

    }
    TEST_F(FigaroAPITest, 13_Share1_Trim_Article_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.00009999092446);

}
    TEST_F(FigaroAPITest, 13_Share1_No_trim_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.002090587692);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001048608958);
}
    TEST_F(FigaroAPITest, 13_Share1_Trim_Max_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.002090587675);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Max_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001048517353);
}
    TEST_F(FigaroAPITest, 13_Share1_Trim_Article_repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.002090587675);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_repair>(storm::figaro::FigaroProgram_BDMP_13_Share1_Trim_Article_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000001048517353);
}



/***********************           Test Case 18              ***********************/
    TEST_F(FigaroAPITest, 18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000000995016635);

}
    TEST_F(FigaroAPITest, 18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Max_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Max_No_repair()), missiontime);

        EXPECT_FLOAT_EQ(result,0.000000995016635);

}
    TEST_F(FigaroAPITest, 18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Article_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_Trim_Article_No_repair()), missiontime);

        EXPECT_FLOAT_EQ(result,0.000000995016635);
}
/***********************           Test Case 21              ***********************/
    TEST_F(FigaroAPITest, 21_Remote_Access_Server_Security_No_trim_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_No_trim_No_repair>(storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_No_trim_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.00009916876808);

}
    TEST_F(FigaroAPITest, 21_Remote_Access_Server_Security_Trim_Max_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Max_No_repair>(storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Max_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.00009916876808);

}
    TEST_F(FigaroAPITest, 21_Remote_Access_Server_Security_Trim_Article_No_repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair>(storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.00009916876808);

}
/***********************           Test Case 23              ***********************/

    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_No_Trim_No_Repair) {
uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_No_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_No_Repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.01990115696);

}
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_Trim_Max_No_Repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_No_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_No_Repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.01990115696);

}
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_Trim_article_No_Repair) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_No_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_No_Repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.01990115696);
}
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_No_Trim_Repair) {
uint32_t missiontime = 10000;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_Repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.8649352087);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_No_Trim_Repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.001998002891);
}
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_Trim_Max_Repair) {
uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_Repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.8649352083);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_Repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.001997003889);
}
    TEST_F(FigaroAPITest, 23_Two_proc_comp_sys_Trim_article_Repair) {
uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_Repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.8649352084);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_Repair>(storm::figaro::FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_article_Repair()), missiontime);
        EXPECT_FLOAT_EQ(result,0.001997003891);
}

/***********************           Test Case BDMP              ***********************/
    TEST_F(FigaroAPITest,         figaro_BDMP) {
uint32_t missiontime = 100;
    double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_figaro_BDMP>(storm::figaro::FigaroProgram_figaro_BDMP()), missiontime);
        EXPECT_FLOAT_EQ(result,0.01787733191);
        std::cout<<result;
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_figaro_BDMP>(storm::figaro::FigaroProgram_figaro_BDMP()), missiontime);
        EXPECT_FLOAT_EQ(result,0.016357312);
        std::cout<<result;
}
/***********************           Test Case Petri net              ***********************/
    TEST_F(FigaroAPITest, figaro_Petrinet) {
uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_figaro_Petrinet>(storm::figaro::FigaroProgram_figaro_Petrinet()), missiontime);
        EXPECT_FLOAT_EQ(result,0.7090497745);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_figaro_Petrinet>(storm::figaro::FigaroProgram_figaro_Petrinet()), missiontime);
        EXPECT_FLOAT_EQ(result,0.21117046);
}
/***********************           Test Case Telecom              ***********************/

    TEST_F(FigaroAPITest, figaro_Telecom) {
uint32_t missiontime = 100;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_figaro_Telecom>(storm::figaro::FigaroProgram_figaro_Telecom()), missiontime);
        EXPECT_FLOAT_EQ(result,0.002014698);
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_figaro_Telecom>(storm::figaro::FigaroProgram_figaro_Telecom()), missiontime);
        EXPECT_FLOAT_EQ(result,0.00021015652);
}
/***********************           Test Case RBD              ***********************/
    TEST_F(FigaroAPITest, figaro_RBD) {
    uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_figaro_RBD>(storm::figaro::FigaroProgram_figaro_RBD()), missiontime);
        EXPECT_FLOAT_EQ(result,0.010702526);
        std::cout<<result;
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_figaro_RBD>(storm::figaro::FigaroProgram_figaro_RBD()), missiontime);
        EXPECT_FLOAT_EQ(result,0.000041469983);
}
/***********************           Test Case Miniplant              ***********************/

    TEST_F(FigaroAPITest, figaro_Miniplant) {
uint32_t missiontime = 10000;
        double result = this->analyzeUnReliability(std::make_shared<storm::figaro::FigaroProgram_figaro_Miniplant>(storm::figaro::FigaroProgram_figaro_Miniplant()), missiontime);
        EXPECT_FLOAT_EQ(result,0.00201696);
        std::cout<<result;
        result = this->analyzeUnAvailability(std::make_shared<storm::figaro::FigaroProgram_figaro_Miniplant>(storm::figaro::FigaroProgram_figaro_Miniplant()), missiontime);
        EXPECT_FLOAT_EQ(result,0.0000020271448);
        std::cout<<result;
}

} //name space
