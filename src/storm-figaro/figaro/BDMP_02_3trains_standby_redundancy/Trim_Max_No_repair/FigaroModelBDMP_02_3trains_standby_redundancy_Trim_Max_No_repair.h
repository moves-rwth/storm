
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
        class FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_No_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Max_No_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_AND_1" , 0},
            	{"already_S_OF_AND_1" , 1},
            	{"S_OF_AND_1" , 2},
            	{"relevant_evt_OF_AND_1" , 3},
            	{"required_OF_AND_2" , 4},
            	{"already_S_OF_AND_2" , 5},
            	{"S_OF_AND_2" , 6},
            	{"relevant_evt_OF_AND_2" , 7},
            	{"required_OF_A_lost" , 8},
            	{"already_S_OF_A_lost" , 9},
            	{"S_OF_A_lost" , 10},
            	{"relevant_evt_OF_A_lost" , 11},
            	{"required_OF_A_op" , 12},
            	{"already_S_OF_A_op" , 13},
            	{"S_OF_A_op" , 14},
            	{"relevant_evt_OF_A_op" , 15},
            	{"failF_OF_A_op" , 16},
            	{"required_OF_A_start" , 17},
            	{"already_S_OF_A_start" , 18},
            	{"S_OF_A_start" , 19},
            	{"relevant_evt_OF_A_start" , 20},
            	{"failI_OF_A_start" , 21},
            	{"to_be_fired_OF_A_start" , 22},
            	{"already_standby_OF_A_start" , 23},
            	{"already_required_OF_A_start" , 24},
            	{"required_OF_B_op" , 25},
            	{"already_S_OF_B_op" , 26},
            	{"S_OF_B_op" , 27},
            	{"relevant_evt_OF_B_op" , 28},
            	{"failF_OF_B_op" , 29},
            	{"required_OF_C_fail" , 30},
            	{"already_S_OF_C_fail" , 31},
            	{"S_OF_C_fail" , 32},
            	{"relevant_evt_OF_C_fail" , 33},
            	{"failF_OF_C_fail" , 34},
            	{"required_OF_C_start" , 35},
            	{"already_S_OF_C_start" , 36},
            	{"S_OF_C_start" , 37},
            	{"relevant_evt_OF_C_start" , 38},
            	{"failI_OF_C_start" , 39},
            	{"to_be_fired_OF_C_start" , 40},
            	{"already_standby_OF_C_start" , 41},
            	{"already_required_OF_C_start" , 42},
            	{"required_OF_OR_1" , 43},
            	{"already_S_OF_OR_1" , 44},
            	{"S_OF_OR_1" , 45},
            	{"relevant_evt_OF_OR_1" , 46},
            	{"required_OF_OR_2" , 47},
            	{"already_S_OF_OR_2" , 48},
            	{"S_OF_OR_2" , 49},
            	{"relevant_evt_OF_OR_2" , 50},
            	{"required_OF_P_op" , 51},
            	{"already_S_OF_P_op" , 52},
            	{"S_OF_P_op" , 53},
            	{"relevant_evt_OF_P_op" , 54},
            	{"failF_OF_P_op" , 55},
            	{"required_OF_UE_1" , 56},
            	{"already_S_OF_UE_1" , 57},
            	{"S_OF_UE_1" , 58},
            	{"relevant_evt_OF_UE_1" , 59}},

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
                    60 ,
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
            bool REINITIALISATION_OF_required_OF_AND_1 ;
            bool REINITIALISATION_OF_S_OF_AND_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_AND_1 ;
            bool REINITIALISATION_OF_required_OF_AND_2 ;
            bool REINITIALISATION_OF_S_OF_AND_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_AND_2 ;
            bool REINITIALISATION_OF_required_OF_A_lost ;
            bool REINITIALISATION_OF_S_OF_A_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_A_lost ;
            bool REINITIALISATION_OF_required_OF_A_op ;
            bool REINITIALISATION_OF_S_OF_A_op ;
            bool REINITIALISATION_OF_relevant_evt_OF_A_op ;
            bool REINITIALISATION_OF_required_OF_A_start ;
            bool REINITIALISATION_OF_S_OF_A_start ;
            bool REINITIALISATION_OF_relevant_evt_OF_A_start ;
            bool REINITIALISATION_OF_to_be_fired_OF_A_start ;
            bool REINITIALISATION_OF_required_OF_B_op ;
            bool REINITIALISATION_OF_S_OF_B_op ;
            bool REINITIALISATION_OF_relevant_evt_OF_B_op ;
            bool REINITIALISATION_OF_required_OF_C_fail ;
            bool REINITIALISATION_OF_S_OF_C_fail ;
            bool REINITIALISATION_OF_relevant_evt_OF_C_fail ;
            bool REINITIALISATION_OF_required_OF_C_start ;
            bool REINITIALISATION_OF_S_OF_C_start ;
            bool REINITIALISATION_OF_relevant_evt_OF_C_start ;
            bool REINITIALISATION_OF_to_be_fired_OF_C_start ;
            bool REINITIALISATION_OF_required_OF_OR_1 ;
            bool REINITIALISATION_OF_S_OF_OR_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_1 ;
            bool REINITIALISATION_OF_required_OF_OR_2 ;
            bool REINITIALISATION_OF_S_OF_OR_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_2 ;
            bool REINITIALISATION_OF_required_OF_P_op ;
            bool REINITIALISATION_OF_S_OF_P_op ;
            bool REINITIALISATION_OF_relevant_evt_OF_P_op ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			double const gamma_OF_C_start = 0.0001;
			std::string const trigger_kind_OF_t_3 = "fn_fathers_and_trig";
			double const mu_OF_C_start = 0.1;
			bool const failF_FROZEN_OF_A_op = false;
			bool const force_relevant_events_OF_OR_1 = false;
			std::string const calculate_required_OF_A_lost = "fn_fathers_and_trig";
			std::string const calculate_required_OF_P_op = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_AND_1 = false;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_B_op = "fn_fathers_and_trig";
			double const mu_OF_P_op = 0.1;
			bool const No_repair_OF___ARBRE__EIRM = true;
			double const lambda_OF_A_op = 0.0001;
			bool const trimming_OF_OPTIONS = true;
			bool const failF_FROZEN_OF_C_fail = false;
			double const mu_OF_B_op = 0.1;
			std::string const calculate_required_OF_OR_1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			bool const repairable_system_OF_OPTIONS = false;
			bool const default_OF___ARBRE__EIRM = true;
			bool const No_trim_OF___ARBRE__EIRM = false;
			double const lambda_OF_C_fail = 0.0001;
			bool const Profil1_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_A_op = false;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			std::string const when_to_check_OF_A_start = "not_req_to_req";
			std::string const when_to_check_OF_C_start = "not_req_to_req";
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			bool const failI_FROZEN_OF_A_start = false;
			bool const force_relevant_events_OF_C_fail = false;
			std::string const calculate_required_OF_A_op = "fn_fathers_and_trig";
			bool const failI_FROZEN_OF_C_start = false;
			double const mu_OF_A_op = 0.1;
			bool const failF_FROZEN_OF_P_op = false;
			bool const force_relevant_events_OF_UE_1 = true;
			bool const force_relevant_events_OF_B_op = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
			bool const failF_FROZEN_OF_B_op = false;
			std::string const calculate_required_OF_C_fail = "fn_fathers_and_trig";
			double const mu_OF_C_fail = 0.1;
			double const lambda_OF_P_op = 0.0001;
			bool const Trim_article_OF___ARBRE__EIRM = false;
			double const lambda_OF_B_op = 0.0001;
			bool const force_relevant_events_OF_A_start = false;
			bool const force_relevant_events_OF_OR_2 = false;
			bool const force_relevant_events_OF_AND_2 = false;
			bool const force_relevant_events_OF_C_start = false;
			std::string const calculate_required_OF_A_start = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_A_lost = false;
			std::string const calculate_required_OF_OR_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_P_op = false;
			std::string const calculate_required_OF_AND_2 = "fn_fathers_and_trig";
			double const gamma_OF_A_start = 0.0001;
			std::string const calculate_required_OF_C_start = "fn_fathers_and_trig";
			double const mu_OF_A_start = 0.1;
			std::string const calculate_required_OF_UE_1 = "always_true";
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_A_op;
            bool FIRE_xx23_OF_A_start_INS_1;
            bool FIRE_xx23_OF_A_start_INS_2;
            bool FIRE_xx10_OF_B_op;
            bool FIRE_xx10_OF_C_fail;
            bool FIRE_xx23_OF_C_start_INS_5;
            bool FIRE_xx23_OF_C_start_INS_6;
            bool FIRE_xx10_OF_P_op;

            int required_OF_AND_1 = 0 ;
            int already_S_OF_AND_1 = 1 ;
            int S_OF_AND_1 = 2 ;
            int relevant_evt_OF_AND_1 = 3 ;
            int required_OF_AND_2 = 4 ;
            int already_S_OF_AND_2 = 5 ;
            int S_OF_AND_2 = 6 ;
            int relevant_evt_OF_AND_2 = 7 ;
            int required_OF_A_lost = 8 ;
            int already_S_OF_A_lost = 9 ;
            int S_OF_A_lost = 10 ;
            int relevant_evt_OF_A_lost = 11 ;
            int required_OF_A_op = 12 ;
            int already_S_OF_A_op = 13 ;
            int S_OF_A_op = 14 ;
            int relevant_evt_OF_A_op = 15 ;
            int failF_OF_A_op = 16 ;
            int required_OF_A_start = 17 ;
            int already_S_OF_A_start = 18 ;
            int S_OF_A_start = 19 ;
            int relevant_evt_OF_A_start = 20 ;
            int failI_OF_A_start = 21 ;
            int to_be_fired_OF_A_start = 22 ;
            int already_standby_OF_A_start = 23 ;
            int already_required_OF_A_start = 24 ;
            int required_OF_B_op = 25 ;
            int already_S_OF_B_op = 26 ;
            int S_OF_B_op = 27 ;
            int relevant_evt_OF_B_op = 28 ;
            int failF_OF_B_op = 29 ;
            int required_OF_C_fail = 30 ;
            int already_S_OF_C_fail = 31 ;
            int S_OF_C_fail = 32 ;
            int relevant_evt_OF_C_fail = 33 ;
            int failF_OF_C_fail = 34 ;
            int required_OF_C_start = 35 ;
            int already_S_OF_C_start = 36 ;
            int S_OF_C_start = 37 ;
            int relevant_evt_OF_C_start = 38 ;
            int failI_OF_C_start = 39 ;
            int to_be_fired_OF_C_start = 40 ;
            int already_standby_OF_C_start = 41 ;
            int already_required_OF_C_start = 42 ;
            int required_OF_OR_1 = 43 ;
            int already_S_OF_OR_1 = 44 ;
            int S_OF_OR_1 = 45 ;
            int relevant_evt_OF_OR_1 = 46 ;
            int required_OF_OR_2 = 47 ;
            int already_S_OF_OR_2 = 48 ;
            int S_OF_OR_2 = 49 ;
            int relevant_evt_OF_OR_2 = 50 ;
            int required_OF_P_op = 51 ;
            int already_S_OF_P_op = 52 ;
            int S_OF_P_op = 53 ;
            int relevant_evt_OF_P_op = 54 ;
            int failF_OF_P_op = 55 ;
            int required_OF_UE_1 = 56 ;
            int already_S_OF_UE_1 = 57 ;
            int S_OF_UE_1 = 58 ;
            int relevant_evt_OF_UE_1 = 59 ;




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
            void runOnceInteractionStep_tops();
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