
    #pragma once
    #include "storm-figaro/model/FigaroModelTemplate.h"
    #include <array>
    #include <map>
    #include <vector>
    #include <sstream>
    #include<math.h>
    #include <set>

    namespace storm{
        namespace figaro{
        class FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_Repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_23_Two_proc_comp_sys_Trim_Max_Repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_CM1_loss" , 0},
            	{"already_S_OF_CM1_loss" , 1},
            	{"S_OF_CM1_loss" , 2},
            	{"relevant_evt_OF_CM1_loss" , 3},
            	{"required_OF_CM2_loss" , 4},
            	{"already_S_OF_CM2_loss" , 5},
            	{"S_OF_CM2_loss" , 6},
            	{"relevant_evt_OF_CM2_loss" , 7},
            	{"required_OF_D11" , 8},
            	{"already_S_OF_D11" , 9},
            	{"S_OF_D11" , 10},
            	{"relevant_evt_OF_D11" , 11},
            	{"failF_OF_D11" , 12},
            	{"required_OF_D12" , 13},
            	{"already_S_OF_D12" , 14},
            	{"S_OF_D12" , 15},
            	{"relevant_evt_OF_D12" , 16},
            	{"failF_OF_D12" , 17},
            	{"failS_OF_D12" , 18},
            	{"required_OF_D21" , 19},
            	{"already_S_OF_D21" , 20},
            	{"S_OF_D21" , 21},
            	{"relevant_evt_OF_D21" , 22},
            	{"failF_OF_D21" , 23},
            	{"required_OF_D22" , 24},
            	{"already_S_OF_D22" , 25},
            	{"S_OF_D22" , 26},
            	{"relevant_evt_OF_D22" , 27},
            	{"failF_OF_D22" , 28},
            	{"failS_OF_D22" , 29},
            	{"required_OF_Disk1_loss" , 30},
            	{"already_S_OF_Disk1_loss" , 31},
            	{"S_OF_Disk1_loss" , 32},
            	{"relevant_evt_OF_Disk1_loss" , 33},
            	{"required_OF_Disk2_loss" , 34},
            	{"already_S_OF_Disk2_loss" , 35},
            	{"S_OF_Disk2_loss" , 36},
            	{"relevant_evt_OF_Disk2_loss" , 37},
            	{"required_OF_Loss_of_both_calculators" , 38},
            	{"already_S_OF_Loss_of_both_calculators" , 39},
            	{"S_OF_Loss_of_both_calculators" , 40},
            	{"relevant_evt_OF_Loss_of_both_calculators" , 41},
            	{"required_OF_M1" , 42},
            	{"already_S_OF_M1" , 43},
            	{"S_OF_M1" , 44},
            	{"relevant_evt_OF_M1" , 45},
            	{"failF_OF_M1" , 46},
            	{"required_OF_M2" , 47},
            	{"already_S_OF_M2" , 48},
            	{"S_OF_M2" , 49},
            	{"relevant_evt_OF_M2" , 50},
            	{"failF_OF_M2" , 51},
            	{"required_OF_M3" , 52},
            	{"already_S_OF_M3" , 53},
            	{"S_OF_M3" , 54},
            	{"relevant_evt_OF_M3" , 55},
            	{"failF_OF_M3" , 56},
            	{"failS_OF_M3" , 57},
            	{"required_OF_Mem1_loss" , 58},
            	{"already_S_OF_Mem1_loss" , 59},
            	{"S_OF_Mem1_loss" , 60},
            	{"relevant_evt_OF_Mem1_loss" , 61},
            	{"required_OF_Mem2_loss" , 62},
            	{"already_S_OF_Mem2_loss" , 63},
            	{"S_OF_Mem2_loss" , 64},
            	{"relevant_evt_OF_Mem2_loss" , 65},
            	{"required_OF_N" , 66},
            	{"already_S_OF_N" , 67},
            	{"S_OF_N" , 68},
            	{"relevant_evt_OF_N" , 69},
            	{"failF_OF_N" , 70},
            	{"required_OF_OR_1" , 71},
            	{"already_S_OF_OR_1" , 72},
            	{"S_OF_OR_1" , 73},
            	{"relevant_evt_OF_OR_1" , 74},
            	{"required_OF_One_main_mem_fail" , 75},
            	{"already_S_OF_One_main_mem_fail" , 76},
            	{"S_OF_One_main_mem_fail" , 77},
            	{"relevant_evt_OF_One_main_mem_fail" , 78},
            	{"required_OF_P1_loss" , 79},
            	{"already_S_OF_P1_loss" , 80},
            	{"S_OF_P1_loss" , 81},
            	{"relevant_evt_OF_P1_loss" , 82},
            	{"required_OF_P2_loss" , 83},
            	{"already_S_OF_P2_loss" , 84},
            	{"S_OF_P2_loss" , 85},
            	{"relevant_evt_OF_P2_loss" , 86},
            	{"required_OF_PS" , 87},
            	{"already_S_OF_PS" , 88},
            	{"S_OF_PS" , 89},
            	{"relevant_evt_OF_PS" , 90},
            	{"failF_OF_PS" , 91},
            	{"required_OF_Pa" , 92},
            	{"already_S_OF_Pa" , 93},
            	{"S_OF_Pa" , 94},
            	{"relevant_evt_OF_Pa" , 95},
            	{"failF_OF_Pa" , 96},
            	{"required_OF_Pb" , 97},
            	{"already_S_OF_Pb" , 98},
            	{"S_OF_Pb" , 99},
            	{"relevant_evt_OF_Pb" , 100},
            	{"failF_OF_Pb" , 101},
            	{"required_OF_UE_1" , 102},
            	{"already_S_OF_UE_1" , 103},
            	{"S_OF_UE_1" , 104},
            	{"relevant_evt_OF_UE_1" , 105}},

//            std::map<std::string, size_t> mFigaroelementfailureindex =
                    {  { "exp0",0}},

//            std::map<std::string, size_t> mFigarofloatelementindex =
                     { },

//            std::map<std::string, size_t> mFigarointelementindex =
                     { },

//            std::map<std::string, size_t> mFigaroenumelementindex =
                     { },

//            std::map<std::string, size_t> failure_variable_names =
                    {  "exp0"},

//            std::set<std::string> enum_variables_names =
                     { },

//            std::set<std::string> float_variables_names =
                     { },


//            std::string const topevent=
                    "exp0",
//            static int const numBoolState = 
                    106 ,
//             numBoolFailureState = 
                    1 ,
//            static int const numFloatState = 
                    0 ,
//            static int const numIntState = 
                    0 ,
//            static int const numEnumState = 
                    0 ,
//            bool ins_transition_found = 
                     false){} 

            /* ---------- CODING ENUMERATED VARIABLES STATES ------------ */
            enum enum_status {};

//            std::array<bool, numBoolState> boolState;
//            std::array<bool, numBoolState> backupBoolState;
//            std::array<float, numFloatState> floatState;
//            std::array<float, numFloatState> backupFloatState;
//            std::array<int, numIntState> intState;
//            std::array<int, numIntState> backupIntState;
//            std::array<int, numEnumState> enumState;
//            std::array<int, numEnumState> backupEnumState;
            bool REINITIALISATION_OF_required_OF_CM1_loss ;
            bool REINITIALISATION_OF_S_OF_CM1_loss ;
            bool REINITIALISATION_OF_relevant_evt_OF_CM1_loss ;
            bool REINITIALISATION_OF_required_OF_CM2_loss ;
            bool REINITIALISATION_OF_S_OF_CM2_loss ;
            bool REINITIALISATION_OF_relevant_evt_OF_CM2_loss ;
            bool REINITIALISATION_OF_required_OF_D11 ;
            bool REINITIALISATION_OF_S_OF_D11 ;
            bool REINITIALISATION_OF_relevant_evt_OF_D11 ;
            bool REINITIALISATION_OF_required_OF_D12 ;
            bool REINITIALISATION_OF_S_OF_D12 ;
            bool REINITIALISATION_OF_relevant_evt_OF_D12 ;
            bool REINITIALISATION_OF_required_OF_D21 ;
            bool REINITIALISATION_OF_S_OF_D21 ;
            bool REINITIALISATION_OF_relevant_evt_OF_D21 ;
            bool REINITIALISATION_OF_required_OF_D22 ;
            bool REINITIALISATION_OF_S_OF_D22 ;
            bool REINITIALISATION_OF_relevant_evt_OF_D22 ;
            bool REINITIALISATION_OF_required_OF_Disk1_loss ;
            bool REINITIALISATION_OF_S_OF_Disk1_loss ;
            bool REINITIALISATION_OF_relevant_evt_OF_Disk1_loss ;
            bool REINITIALISATION_OF_required_OF_Disk2_loss ;
            bool REINITIALISATION_OF_S_OF_Disk2_loss ;
            bool REINITIALISATION_OF_relevant_evt_OF_Disk2_loss ;
            bool REINITIALISATION_OF_required_OF_Loss_of_both_calculators ;
            bool REINITIALISATION_OF_S_OF_Loss_of_both_calculators ;
            bool REINITIALISATION_OF_relevant_evt_OF_Loss_of_both_calculators ;
            bool REINITIALISATION_OF_required_OF_M1 ;
            bool REINITIALISATION_OF_S_OF_M1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_M1 ;
            bool REINITIALISATION_OF_required_OF_M2 ;
            bool REINITIALISATION_OF_S_OF_M2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_M2 ;
            bool REINITIALISATION_OF_required_OF_M3 ;
            bool REINITIALISATION_OF_S_OF_M3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_M3 ;
            bool REINITIALISATION_OF_required_OF_Mem1_loss ;
            bool REINITIALISATION_OF_S_OF_Mem1_loss ;
            bool REINITIALISATION_OF_relevant_evt_OF_Mem1_loss ;
            bool REINITIALISATION_OF_required_OF_Mem2_loss ;
            bool REINITIALISATION_OF_S_OF_Mem2_loss ;
            bool REINITIALISATION_OF_relevant_evt_OF_Mem2_loss ;
            bool REINITIALISATION_OF_required_OF_N ;
            bool REINITIALISATION_OF_S_OF_N ;
            bool REINITIALISATION_OF_relevant_evt_OF_N ;
            bool REINITIALISATION_OF_required_OF_OR_1 ;
            bool REINITIALISATION_OF_S_OF_OR_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_1 ;
            bool REINITIALISATION_OF_required_OF_One_main_mem_fail ;
            bool REINITIALISATION_OF_S_OF_One_main_mem_fail ;
            bool REINITIALISATION_OF_relevant_evt_OF_One_main_mem_fail ;
            bool REINITIALISATION_OF_required_OF_P1_loss ;
            bool REINITIALISATION_OF_S_OF_P1_loss ;
            bool REINITIALISATION_OF_relevant_evt_OF_P1_loss ;
            bool REINITIALISATION_OF_required_OF_P2_loss ;
            bool REINITIALISATION_OF_S_OF_P2_loss ;
            bool REINITIALISATION_OF_relevant_evt_OF_P2_loss ;
            bool REINITIALISATION_OF_required_OF_PS ;
            bool REINITIALISATION_OF_S_OF_PS ;
            bool REINITIALISATION_OF_relevant_evt_OF_PS ;
            bool REINITIALISATION_OF_required_OF_Pa ;
            bool REINITIALISATION_OF_S_OF_Pa ;
            bool REINITIALISATION_OF_relevant_evt_OF_Pa ;
            bool REINITIALISATION_OF_required_OF_Pb ;
            bool REINITIALISATION_OF_S_OF_Pb ;
            bool REINITIALISATION_OF_relevant_evt_OF_Pb ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			double const lambda_OF_D11 = 0.0001;
			bool const force_relevant_events_OF_M2 = false;
			double const mu_OF_D22 = 0.1;
			std::string const calculate_required_OF_Pa = "fn_fathers_and_trig";
			std::string const calculate_required_OF_One_main_mem_fail = "fn_fathers_and_trig";
			double const mu_OF_PS = 0.1;
			double const mu_OF_D21 = 0.1;
			std::string const calculate_required_OF_D12 = "fn_fathers_and_trig";
			double const standby_lambda_OF_M3 = 1e-05;
			double const mu_OF_M3 = 0.1;
			bool const force_relevant_events_OF_Mem2_loss = false;
			bool const force_relevant_events_OF_Loss_of_both_calculators = false;
			bool const failF_FROZEN_OF_M1 = false;
			double const mu_OF_M1 = 0.1;
			bool const force_relevant_events_OF_P1_loss = false;
			std::string const calculate_required_OF_CM1_loss = "fn_fathers_and_trig";
			std::string const calculate_required_OF_Disk1_loss = "fn_fathers_and_trig";
			double const mu_OF_M2 = 0.1;
			bool const failF_FROZEN_OF_N = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
			std::string const calculate_required_OF_Pb = "fn_fathers_and_trig";
			bool const repairable_system_OF_OPTIONS = true;
			bool const force_relevant_events_OF_Pa = false;
			std::string const calculate_required_OF_P2_loss = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_One_main_mem_fail = false;
			bool const failF_FROZEN_OF_D11 = false;
			bool const failS_FROZEN_OF_D12 = false;
			bool const failF_FROZEN_OF_D22 = false;
			std::string const calculate_required_OF_N = "fn_fathers_and_trig";
			double const mu_OF_D11 = 0.1;
			bool const force_relevant_events_OF_D12 = false;
			bool const failF_FROZEN_OF_PS = false;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_D21 = false;
			std::string const calculate_required_OF_UE_1 = "always_true";
			std::string const calculate_required_OF_D22 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_M3 = false;
			double const lambda_OF_Pa = 0.0001;
			bool const force_relevant_events_OF_CM1_loss = false;
			bool const force_relevant_events_OF_Disk1_loss = false;
			std::string const calculate_required_OF_PS = "fn_fathers_and_trig";
			std::string const calculate_required_OF_D21 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_UE_1 = true;
			bool const failF_FROZEN_OF_M2 = false;
			std::string const calculate_required_OF_OR_1 = "fn_fathers_and_trig";
			bool const Trim_article_OF___ARBRE__EIRM = false;
			double const lambda_OF_D12 = 0.0001;
			bool const force_relevant_events_OF_Pb = false;
			std::string const calculate_required_OF_CM2_loss = "fn_fathers_and_trig";
			std::string const calculate_required_OF_M3 = "fn_fathers_and_trig";
			bool const Profil1_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_M1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_N = false;
			std::string const calculate_required_OF_Mem1_loss = "fn_fathers_and_trig";
			std::string const calculate_required_OF_M2 = "fn_fathers_and_trig";
			bool const No_Trim_OF___ARBRE__EIRM = false;
			bool const failS_FROZEN_OF_D22 = false;
			double const lambda_OF_Pb = 0.0001;
			double const mu_OF_Pb = 0.1;
			std::string const calculate_required_OF_D11 = "fn_fathers_and_trig";
			bool const to_be_taken_into_account_OF_UE_1 = true;
			std::string const calculate_required_OF_Mem2_loss = "fn_fathers_and_trig";
			std::string const calculate_required_OF_Loss_of_both_calculators = "fn_fathers_and_trig";
			double const lambda_OF_N = 0.0001;
			bool const failS_FROZEN_OF_M3 = false;
			bool const force_relevant_events_OF_OR_1 = false;
			bool const failF_FROZEN_OF_Pa = false;
			bool const trimming_OF_OPTIONS = true;
			double const mu_OF_Pa = 0.1;
			double const standby_lambda_OF_D12 = 1e-05;
			bool const force_relevant_events_OF_M1 = false;
			double const mu_OF_D12 = 0.1;
			bool const force_relevant_events_OF_Mem1_loss = false;
			double const lambda_OF_D22 = 0.0001;
			bool const force_relevant_events_OF_P2_loss = false;
			std::string const calculate_required_OF_Disk2_loss = "fn_fathers_and_trig";
			double const lambda_OF_PS = 0.0001;
			double const lambda_OF_D21 = 0.0001;
			bool const No_repair_OF___ARBRE__EIRM = false;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			bool const Default_OF___ARBRE__EIRM = true;
			double const lambda_OF_M3 = 0.0001;
			bool const force_relevant_events_OF_D11 = false;
			bool const force_relevant_events_OF_D22 = false;
			std::string const trigger_kind_OF_t_1_1 = "fn_fathers_and_trig";
			double const lambda_OF_M1 = 0.0001;
			bool const failF_FROZEN_OF_Pb = false;
			bool const force_relevant_events_OF_PS = false;
			double const lambda_OF_M2 = 0.0001;
			bool const force_relevant_events_OF_D21 = false;
			double const mu_OF_N = 0.1;
			std::string const calculate_required_OF_P1_loss = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_CM2_loss = false;
			bool const force_relevant_events_OF_M3 = false;
			double const standby_lambda_OF_D22 = 1e-05;
			bool const failF_FROZEN_OF_D12 = false;
			bool const force_relevant_events_OF_Disk2_loss = false;
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_D11;
            bool FIRE_xx11_OF_D11;
            bool FIRE_xx17_OF_D12;
            bool FIRE_xx18_OF_D12;
            bool FIRE_xx19_OF_D12;
            bool FIRE_xx10_OF_D21;
            bool FIRE_xx11_OF_D21;
            bool FIRE_xx17_OF_D22;
            bool FIRE_xx18_OF_D22;
            bool FIRE_xx19_OF_D22;
            bool FIRE_xx10_OF_M1;
            bool FIRE_xx11_OF_M1;
            bool FIRE_xx10_OF_M2;
            bool FIRE_xx11_OF_M2;
            bool FIRE_xx17_OF_M3;
            bool FIRE_xx18_OF_M3;
            bool FIRE_xx19_OF_M3;
            bool FIRE_xx10_OF_N;
            bool FIRE_xx11_OF_N;
            bool FIRE_xx10_OF_PS;
            bool FIRE_xx11_OF_PS;
            bool FIRE_xx10_OF_Pa;
            bool FIRE_xx11_OF_Pa;
            bool FIRE_xx10_OF_Pb;
            bool FIRE_xx11_OF_Pb;

            int required_OF_CM1_loss = 0 ;
            int already_S_OF_CM1_loss = 1 ;
            int S_OF_CM1_loss = 2 ;
            int relevant_evt_OF_CM1_loss = 3 ;
            int required_OF_CM2_loss = 4 ;
            int already_S_OF_CM2_loss = 5 ;
            int S_OF_CM2_loss = 6 ;
            int relevant_evt_OF_CM2_loss = 7 ;
            int required_OF_D11 = 8 ;
            int already_S_OF_D11 = 9 ;
            int S_OF_D11 = 10 ;
            int relevant_evt_OF_D11 = 11 ;
            int failF_OF_D11 = 12 ;
            int required_OF_D12 = 13 ;
            int already_S_OF_D12 = 14 ;
            int S_OF_D12 = 15 ;
            int relevant_evt_OF_D12 = 16 ;
            int failF_OF_D12 = 17 ;
            int failS_OF_D12 = 18 ;
            int required_OF_D21 = 19 ;
            int already_S_OF_D21 = 20 ;
            int S_OF_D21 = 21 ;
            int relevant_evt_OF_D21 = 22 ;
            int failF_OF_D21 = 23 ;
            int required_OF_D22 = 24 ;
            int already_S_OF_D22 = 25 ;
            int S_OF_D22 = 26 ;
            int relevant_evt_OF_D22 = 27 ;
            int failF_OF_D22 = 28 ;
            int failS_OF_D22 = 29 ;
            int required_OF_Disk1_loss = 30 ;
            int already_S_OF_Disk1_loss = 31 ;
            int S_OF_Disk1_loss = 32 ;
            int relevant_evt_OF_Disk1_loss = 33 ;
            int required_OF_Disk2_loss = 34 ;
            int already_S_OF_Disk2_loss = 35 ;
            int S_OF_Disk2_loss = 36 ;
            int relevant_evt_OF_Disk2_loss = 37 ;
            int required_OF_Loss_of_both_calculators = 38 ;
            int already_S_OF_Loss_of_both_calculators = 39 ;
            int S_OF_Loss_of_both_calculators = 40 ;
            int relevant_evt_OF_Loss_of_both_calculators = 41 ;
            int required_OF_M1 = 42 ;
            int already_S_OF_M1 = 43 ;
            int S_OF_M1 = 44 ;
            int relevant_evt_OF_M1 = 45 ;
            int failF_OF_M1 = 46 ;
            int required_OF_M2 = 47 ;
            int already_S_OF_M2 = 48 ;
            int S_OF_M2 = 49 ;
            int relevant_evt_OF_M2 = 50 ;
            int failF_OF_M2 = 51 ;
            int required_OF_M3 = 52 ;
            int already_S_OF_M3 = 53 ;
            int S_OF_M3 = 54 ;
            int relevant_evt_OF_M3 = 55 ;
            int failF_OF_M3 = 56 ;
            int failS_OF_M3 = 57 ;
            int required_OF_Mem1_loss = 58 ;
            int already_S_OF_Mem1_loss = 59 ;
            int S_OF_Mem1_loss = 60 ;
            int relevant_evt_OF_Mem1_loss = 61 ;
            int required_OF_Mem2_loss = 62 ;
            int already_S_OF_Mem2_loss = 63 ;
            int S_OF_Mem2_loss = 64 ;
            int relevant_evt_OF_Mem2_loss = 65 ;
            int required_OF_N = 66 ;
            int already_S_OF_N = 67 ;
            int S_OF_N = 68 ;
            int relevant_evt_OF_N = 69 ;
            int failF_OF_N = 70 ;
            int required_OF_OR_1 = 71 ;
            int already_S_OF_OR_1 = 72 ;
            int S_OF_OR_1 = 73 ;
            int relevant_evt_OF_OR_1 = 74 ;
            int required_OF_One_main_mem_fail = 75 ;
            int already_S_OF_One_main_mem_fail = 76 ;
            int S_OF_One_main_mem_fail = 77 ;
            int relevant_evt_OF_One_main_mem_fail = 78 ;
            int required_OF_P1_loss = 79 ;
            int already_S_OF_P1_loss = 80 ;
            int S_OF_P1_loss = 81 ;
            int relevant_evt_OF_P1_loss = 82 ;
            int required_OF_P2_loss = 83 ;
            int already_S_OF_P2_loss = 84 ;
            int S_OF_P2_loss = 85 ;
            int relevant_evt_OF_P2_loss = 86 ;
            int required_OF_PS = 87 ;
            int already_S_OF_PS = 88 ;
            int S_OF_PS = 89 ;
            int relevant_evt_OF_PS = 90 ;
            int failF_OF_PS = 91 ;
            int required_OF_Pa = 92 ;
            int already_S_OF_Pa = 93 ;
            int S_OF_Pa = 94 ;
            int relevant_evt_OF_Pa = 95 ;
            int failF_OF_Pa = 96 ;
            int required_OF_Pb = 97 ;
            int already_S_OF_Pb = 98 ;
            int S_OF_Pb = 99 ;
            int relevant_evt_OF_Pb = 100 ;
            int failF_OF_Pb = 101 ;
            int required_OF_UE_1 = 102 ;
            int already_S_OF_UE_1 = 103 ;
            int S_OF_UE_1 = 104 ;
            int relevant_evt_OF_UE_1 = 105 ;




            int exp0 = 0 ;


            /* ---------- DECLARATION OF FUNCTIONS ------------ */
            void init();
            void saveCurrentState();
            void printState();
            void fireOccurrence(int numFire);
            std::vector<std::tuple<int, double, std::string, int>> showFireableOccurrences();
            void runOnceInteractionStep_initialization();
            void runOnceInteractionStep_propagate_effect_S();
            void runOnceInteractionStep_propagate_effect_required();
            void runOnceInteractionStep_propagate_leaves();
            int compareStates();
            void doReinitialisations();
            void runInteractions();
            void printstatetuple();
            void fireinsttransitiongroup(std::string);
            int_fast64_t stateSize() const;
            bool figaromodelhasinstransitions();
            };
        }
    }