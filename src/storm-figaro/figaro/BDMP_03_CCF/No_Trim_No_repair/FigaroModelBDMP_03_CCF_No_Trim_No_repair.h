
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
        class FigaroProgram_BDMP_03_CCF_No_Trim_No_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_03_CCF_No_Trim_No_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_CCF_fail_A" , 0},
            	{"already_S_OF_CCF_fail_A" , 1},
            	{"S_OF_CCF_fail_A" , 2},
            	{"relevant_evt_OF_CCF_fail_A" , 3},
            	{"failI_OF_CCF_fail_A" , 4},
            	{"to_be_fired_OF_CCF_fail_A" , 5},
            	{"already_standby_OF_CCF_fail_A" , 6},
            	{"already_required_OF_CCF_fail_A" , 7},
            	{"required_OF_CCF_fail_B" , 8},
            	{"already_S_OF_CCF_fail_B" , 9},
            	{"S_OF_CCF_fail_B" , 10},
            	{"relevant_evt_OF_CCF_fail_B" , 11},
            	{"failI_OF_CCF_fail_B" , 12},
            	{"to_be_fired_OF_CCF_fail_B" , 13},
            	{"already_standby_OF_CCF_fail_B" , 14},
            	{"already_required_OF_CCF_fail_B" , 15},
            	{"required_OF_CCF_fail_C" , 16},
            	{"already_S_OF_CCF_fail_C" , 17},
            	{"S_OF_CCF_fail_C" , 18},
            	{"relevant_evt_OF_CCF_fail_C" , 19},
            	{"failI_OF_CCF_fail_C" , 20},
            	{"to_be_fired_OF_CCF_fail_C" , 21},
            	{"already_standby_OF_CCF_fail_C" , 22},
            	{"already_required_OF_CCF_fail_C" , 23},
            	{"required_OF_Indep_fail_A" , 24},
            	{"already_S_OF_Indep_fail_A" , 25},
            	{"S_OF_Indep_fail_A" , 26},
            	{"relevant_evt_OF_Indep_fail_A" , 27},
            	{"failF_OF_Indep_fail_A" , 28},
            	{"required_OF_Indep_fail_B" , 29},
            	{"already_S_OF_Indep_fail_B" , 30},
            	{"S_OF_Indep_fail_B" , 31},
            	{"relevant_evt_OF_Indep_fail_B" , 32},
            	{"failF_OF_Indep_fail_B" , 33},
            	{"required_OF_Indep_fail_C" , 34},
            	{"already_S_OF_Indep_fail_C" , 35},
            	{"S_OF_Indep_fail_C" , 36},
            	{"relevant_evt_OF_Indep_fail_C" , 37},
            	{"failF_OF_Indep_fail_C" , 38},
            	{"required_OF_Shock" , 39},
            	{"already_S_OF_Shock" , 40},
            	{"S_OF_Shock" , 41},
            	{"relevant_evt_OF_Shock" , 42},
            	{"failF_OF_Shock" , 43},
            	{"required_OF_UE_1" , 44},
            	{"already_S_OF_UE_1" , 45},
            	{"S_OF_UE_1" , 46},
            	{"relevant_evt_OF_UE_1" , 47},
            	{"required_OF_loss_of_A" , 48},
            	{"already_S_OF_loss_of_A" , 49},
            	{"S_OF_loss_of_A" , 50},
            	{"relevant_evt_OF_loss_of_A" , 51},
            	{"required_OF_loss_of_B" , 52},
            	{"already_S_OF_loss_of_B" , 53},
            	{"S_OF_loss_of_B" , 54},
            	{"relevant_evt_OF_loss_of_B" , 55},
            	{"required_OF_loss_of_C" , 56},
            	{"already_S_OF_loss_of_C" , 57},
            	{"S_OF_loss_of_C" , 58},
            	{"relevant_evt_OF_loss_of_C" , 59},
            	{"required_OF_system_loss" , 60},
            	{"already_S_OF_system_loss" , 61},
            	{"S_OF_system_loss" , 62},
            	{"relevant_evt_OF_system_loss" , 63}},

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
                    64 ,
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
            bool REINITIALISATION_OF_required_OF_CCF_fail_A ;
            bool REINITIALISATION_OF_S_OF_CCF_fail_A ;
            bool REINITIALISATION_OF_relevant_evt_OF_CCF_fail_A ;
            bool REINITIALISATION_OF_to_be_fired_OF_CCF_fail_A ;
            bool REINITIALISATION_OF_required_OF_CCF_fail_B ;
            bool REINITIALISATION_OF_S_OF_CCF_fail_B ;
            bool REINITIALISATION_OF_relevant_evt_OF_CCF_fail_B ;
            bool REINITIALISATION_OF_to_be_fired_OF_CCF_fail_B ;
            bool REINITIALISATION_OF_required_OF_CCF_fail_C ;
            bool REINITIALISATION_OF_S_OF_CCF_fail_C ;
            bool REINITIALISATION_OF_relevant_evt_OF_CCF_fail_C ;
            bool REINITIALISATION_OF_to_be_fired_OF_CCF_fail_C ;
            bool REINITIALISATION_OF_required_OF_Indep_fail_A ;
            bool REINITIALISATION_OF_S_OF_Indep_fail_A ;
            bool REINITIALISATION_OF_relevant_evt_OF_Indep_fail_A ;
            bool REINITIALISATION_OF_required_OF_Indep_fail_B ;
            bool REINITIALISATION_OF_S_OF_Indep_fail_B ;
            bool REINITIALISATION_OF_relevant_evt_OF_Indep_fail_B ;
            bool REINITIALISATION_OF_required_OF_Indep_fail_C ;
            bool REINITIALISATION_OF_S_OF_Indep_fail_C ;
            bool REINITIALISATION_OF_relevant_evt_OF_Indep_fail_C ;
            bool REINITIALISATION_OF_required_OF_Shock ;
            bool REINITIALISATION_OF_S_OF_Shock ;
            bool REINITIALISATION_OF_relevant_evt_OF_Shock ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            bool REINITIALISATION_OF_required_OF_loss_of_A ;
            bool REINITIALISATION_OF_S_OF_loss_of_A ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_A ;
            bool REINITIALISATION_OF_required_OF_loss_of_B ;
            bool REINITIALISATION_OF_S_OF_loss_of_B ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_B ;
            bool REINITIALISATION_OF_required_OF_loss_of_C ;
            bool REINITIALISATION_OF_S_OF_loss_of_C ;
            bool REINITIALISATION_OF_relevant_evt_OF_loss_of_C ;
            bool REINITIALISATION_OF_required_OF_system_loss ;
            bool REINITIALISATION_OF_S_OF_system_loss ;
            bool REINITIALISATION_OF_relevant_evt_OF_system_loss ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			bool const force_relevant_events_OF_Indep_fail_B = false;
			std::string const trigger_kind_OF_t_3 = "fn_fathers_and_trig";
			double const lambda_OF_Shock = 0.0001;
			std::string const calculate_required_OF_Indep_fail_A = "fn_fathers_and_trig";
			double const mu_OF_Indep_fail_A = 0.1;
			bool const force_relevant_events_OF_CCF_fail_A = false;
			std::string const calculate_required_OF_Indep_fail_B = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const No_repair_OF___ARBRE__EIRM = true;
			double const mu_OF_Indep_fail_B = 0.1;
			int const K_OF_system_loss = 2;
			bool const failF_FROZEN_OF_Indep_fail_C = false;
			std::string const calculate_required_OF_UE_1 = "always_true";
			std::string const when_to_check_OF_CCF_fail_B = "not_req_to_req";
			bool const repairable_system_OF_OPTIONS = false;
			double const mu_OF_CCF_fail_A = 0.1;
			std::string const calculate_required_OF_CCF_fail_A = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_Shock = false;
			bool const default_OF___ARBRE__EIRM = true;
			double const gamma_OF_CCF_fail_A = 0.0001;
			bool const failI_FROZEN_OF_CCF_fail_B = false;
			bool const force_relevant_events_OF_loss_of_A = false;
			bool const force_relevant_events_OF_CCF_fail_C = false;
			double const lambda_OF_Indep_fail_C = 0.0001;
			bool const Profil1_OF___ARBRE__EIRM = false;
			bool const No_trim_OF___ARBRE__EIRM = true;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			std::string const calculate_required_OF_Shock = "fn_fathers_and_trig";
			double const mu_OF_Shock = 0.1;
			std::string const calculate_required_OF_loss_of_A = "fn_fathers_and_trig";
			std::string const calculate_required_OF_CCF_fail_C = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			double const gamma_OF_CCF_fail_C = 0.0001;
			bool const force_relevant_events_OF_system_loss = false;
			double const mu_OF_CCF_fail_C = 0.1;
			bool const force_relevant_events_OF_Indep_fail_C = false;
			bool const force_relevant_events_OF_loss_of_B = false;
			bool const force_relevant_events_OF_loss_of_C = false;
			bool const force_relevant_events_OF_UE_1 = true;
			bool const failF_FROZEN_OF_Indep_fail_A = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
			bool const force_relevant_events_OF_CCF_fail_B = false;
			bool const failF_FROZEN_OF_Indep_fail_B = false;
			std::string const calculate_required_OF_system_loss = "fn_fathers_and_trig";
			std::string const calculate_required_OF_Indep_fail_C = "fn_fathers_and_trig";
			std::string const calculate_required_OF_loss_of_B = "fn_fathers_and_trig";
			std::string const calculate_required_OF_loss_of_C = "fn_fathers_and_trig";
			bool const Trim_article_OF___ARBRE__EIRM = false;
			std::string const when_to_check_OF_CCF_fail_A = "not_req_to_req";
			double const lambda_OF_Indep_fail_A = 0.0001;
			double const lambda_OF_Indep_fail_B = 0.0001;
			std::string const calculate_required_OF_CCF_fail_B = "fn_fathers_and_trig";
			double const mu_OF_Indep_fail_C = 0.1;
			bool const failI_FROZEN_OF_CCF_fail_A = false;
			double const gamma_OF_CCF_fail_B = 0.0001;
			double const mu_OF_CCF_fail_B = 0.1;
			bool const failF_FROZEN_OF_Shock = false;
			std::string const when_to_check_OF_CCF_fail_C = "not_req_to_req";
			bool const force_relevant_events_OF_Indep_fail_A = false;
			bool const failI_FROZEN_OF_CCF_fail_C = false;
			bool const trimming_OF_OPTIONS = false;
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx23_OF_CCF_fail_A_INS_0;
            bool FIRE_xx23_OF_CCF_fail_A_INS_1;
            bool FIRE_xx23_OF_CCF_fail_B_INS_2;
            bool FIRE_xx23_OF_CCF_fail_B_INS_3;
            bool FIRE_xx23_OF_CCF_fail_C_INS_4;
            bool FIRE_xx23_OF_CCF_fail_C_INS_5;
            bool FIRE_xx10_OF_Indep_fail_A;
            bool FIRE_xx10_OF_Indep_fail_B;
            bool FIRE_xx10_OF_Indep_fail_C;
            bool FIRE_xx10_OF_Shock;

            int required_OF_CCF_fail_A = 0 ;
            int already_S_OF_CCF_fail_A = 1 ;
            int S_OF_CCF_fail_A = 2 ;
            int relevant_evt_OF_CCF_fail_A = 3 ;
            int failI_OF_CCF_fail_A = 4 ;
            int to_be_fired_OF_CCF_fail_A = 5 ;
            int already_standby_OF_CCF_fail_A = 6 ;
            int already_required_OF_CCF_fail_A = 7 ;
            int required_OF_CCF_fail_B = 8 ;
            int already_S_OF_CCF_fail_B = 9 ;
            int S_OF_CCF_fail_B = 10 ;
            int relevant_evt_OF_CCF_fail_B = 11 ;
            int failI_OF_CCF_fail_B = 12 ;
            int to_be_fired_OF_CCF_fail_B = 13 ;
            int already_standby_OF_CCF_fail_B = 14 ;
            int already_required_OF_CCF_fail_B = 15 ;
            int required_OF_CCF_fail_C = 16 ;
            int already_S_OF_CCF_fail_C = 17 ;
            int S_OF_CCF_fail_C = 18 ;
            int relevant_evt_OF_CCF_fail_C = 19 ;
            int failI_OF_CCF_fail_C = 20 ;
            int to_be_fired_OF_CCF_fail_C = 21 ;
            int already_standby_OF_CCF_fail_C = 22 ;
            int already_required_OF_CCF_fail_C = 23 ;
            int required_OF_Indep_fail_A = 24 ;
            int already_S_OF_Indep_fail_A = 25 ;
            int S_OF_Indep_fail_A = 26 ;
            int relevant_evt_OF_Indep_fail_A = 27 ;
            int failF_OF_Indep_fail_A = 28 ;
            int required_OF_Indep_fail_B = 29 ;
            int already_S_OF_Indep_fail_B = 30 ;
            int S_OF_Indep_fail_B = 31 ;
            int relevant_evt_OF_Indep_fail_B = 32 ;
            int failF_OF_Indep_fail_B = 33 ;
            int required_OF_Indep_fail_C = 34 ;
            int already_S_OF_Indep_fail_C = 35 ;
            int S_OF_Indep_fail_C = 36 ;
            int relevant_evt_OF_Indep_fail_C = 37 ;
            int failF_OF_Indep_fail_C = 38 ;
            int required_OF_Shock = 39 ;
            int already_S_OF_Shock = 40 ;
            int S_OF_Shock = 41 ;
            int relevant_evt_OF_Shock = 42 ;
            int failF_OF_Shock = 43 ;
            int required_OF_UE_1 = 44 ;
            int already_S_OF_UE_1 = 45 ;
            int S_OF_UE_1 = 46 ;
            int relevant_evt_OF_UE_1 = 47 ;
            int required_OF_loss_of_A = 48 ;
            int already_S_OF_loss_of_A = 49 ;
            int S_OF_loss_of_A = 50 ;
            int relevant_evt_OF_loss_of_A = 51 ;
            int required_OF_loss_of_B = 52 ;
            int already_S_OF_loss_of_B = 53 ;
            int S_OF_loss_of_B = 54 ;
            int relevant_evt_OF_loss_of_B = 55 ;
            int required_OF_loss_of_C = 56 ;
            int already_S_OF_loss_of_C = 57 ;
            int S_OF_loss_of_C = 58 ;
            int relevant_evt_OF_loss_of_C = 59 ;
            int required_OF_system_loss = 60 ;
            int already_S_OF_system_loss = 61 ;
            int S_OF_system_loss = 62 ;
            int relevant_evt_OF_system_loss = 63 ;




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