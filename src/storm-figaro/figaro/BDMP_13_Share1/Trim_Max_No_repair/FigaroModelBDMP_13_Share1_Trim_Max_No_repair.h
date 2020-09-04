
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
        class FigaroProgram_BDMP_13_Share1_Trim_Max_No_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_13_Share1_Trim_Max_No_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_AND_1" , 0},
            	{"already_S_OF_AND_1" , 1},
            	{"S_OF_AND_1" , 2},
            	{"relevant_evt_OF_AND_1" , 3},
            	{"required_OF_FailureOf_A" , 4},
            	{"already_S_OF_FailureOf_A" , 5},
            	{"S_OF_FailureOf_A" , 6},
            	{"relevant_evt_OF_FailureOf_A" , 7},
            	{"failF_OF_FailureOf_A" , 8},
            	{"required_OF_FailureOf_B" , 9},
            	{"already_S_OF_FailureOf_B" , 10},
            	{"S_OF_FailureOf_B" , 11},
            	{"relevant_evt_OF_FailureOf_B" , 12},
            	{"failF_OF_FailureOf_B" , 13},
            	{"required_OF_FailureOf_S" , 14},
            	{"already_S_OF_FailureOf_S" , 15},
            	{"S_OF_FailureOf_S" , 16},
            	{"relevant_evt_OF_FailureOf_S" , 17},
            	{"failF_OF_FailureOf_S" , 18},
            	{"required_OF_FunctionOf_A_lost" , 19},
            	{"already_S_OF_FunctionOf_A_lost" , 20},
            	{"S_OF_FunctionOf_A_lost" , 21},
            	{"relevant_evt_OF_FunctionOf_A_lost" , 22},
            	{"required_OF_OR_3" , 23},
            	{"already_S_OF_OR_3" , 24},
            	{"S_OF_OR_3" , 25},
            	{"relevant_evt_OF_OR_3" , 26},
            	{"required_OF_OnDemandFailureOf_S" , 27},
            	{"already_S_OF_OnDemandFailureOf_S" , 28},
            	{"S_OF_OnDemandFailureOf_S" , 29},
            	{"relevant_evt_OF_OnDemandFailureOf_S" , 30},
            	{"failI_OF_OnDemandFailureOf_S" , 31},
            	{"to_be_fired_OF_OnDemandFailureOf_S" , 32},
            	{"already_standby_OF_OnDemandFailureOf_S" , 33},
            	{"already_required_OF_OnDemandFailureOf_S" , 34},
            	{"required_OF_S_unavailable" , 35},
            	{"already_S_OF_S_unavailable" , 36},
            	{"S_OF_S_unavailable" , 37},
            	{"relevant_evt_OF_S_unavailable" , 38},
            	{"required_OF_THEN_1" , 39},
            	{"already_S_OF_THEN_1" , 40},
            	{"S_OF_THEN_1" , 41},
            	{"relevant_evt_OF_THEN_1" , 42},
            	{"required_OF_UE_1" , 43},
            	{"already_S_OF_UE_1" , 44},
            	{"S_OF_UE_1" , 45},
            	{"relevant_evt_OF_UE_1" , 46}},

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
                    47 ,
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
            bool REINITIALISATION_OF_required_OF_FailureOf_A ;
            bool REINITIALISATION_OF_S_OF_FailureOf_A ;
            bool REINITIALISATION_OF_relevant_evt_OF_FailureOf_A ;
            bool REINITIALISATION_OF_required_OF_FailureOf_B ;
            bool REINITIALISATION_OF_S_OF_FailureOf_B ;
            bool REINITIALISATION_OF_relevant_evt_OF_FailureOf_B ;
            bool REINITIALISATION_OF_required_OF_FailureOf_S ;
            bool REINITIALISATION_OF_S_OF_FailureOf_S ;
            bool REINITIALISATION_OF_relevant_evt_OF_FailureOf_S ;
            bool REINITIALISATION_OF_required_OF_FunctionOf_A_lost ;
            bool REINITIALISATION_OF_S_OF_FunctionOf_A_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_FunctionOf_A_lost ;
            bool REINITIALISATION_OF_required_OF_OR_3 ;
            bool REINITIALISATION_OF_S_OF_OR_3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_3 ;
            bool REINITIALISATION_OF_required_OF_OnDemandFailureOf_S ;
            bool REINITIALISATION_OF_S_OF_OnDemandFailureOf_S ;
            bool REINITIALISATION_OF_relevant_evt_OF_OnDemandFailureOf_S ;
            bool REINITIALISATION_OF_to_be_fired_OF_OnDemandFailureOf_S ;
            bool REINITIALISATION_OF_required_OF_S_unavailable ;
            bool REINITIALISATION_OF_S_OF_S_unavailable ;
            bool REINITIALISATION_OF_relevant_evt_OF_S_unavailable ;
            bool REINITIALISATION_OF_required_OF_THEN_1 ;
            bool REINITIALISATION_OF_S_OF_THEN_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_THEN_1 ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			bool const force_relevant_events_OF_FailureOf_A = false;
			std::string const calculate_required_OF_S_unavailable = "fn_fathers_and_trig";
			std::string const calculate_required_OF_OR_3 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_FailureOf_A = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_AND_1 = false;
			std::string const step_down_OF_THEN_1 = "rep_any";
			bool const force_relevant_events_OF_OnDemandFailureOf_S = false;
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			double const lambda_OF_FailureOf_S = 0.0001;
			double const lambda_OF_FailureOf_B = 0.0001;
			std::string const calculate_required_OF_OnDemandFailureOf_S = "fn_fathers_and_trig";
			double const gamma_OF_OnDemandFailureOf_S = 0.0001;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			bool const Default_OF___ARBRE__EIRM = true;
			bool const repairable_system_OF_OPTIONS = false;
			bool const failF_FROZEN_OF_FailureOf_S = false;
			double const mu_OF_FailureOf_S = 0.1;
			bool const failF_FROZEN_OF_FailureOf_B = false;
			double const mu_OF_FailureOf_B = 0.1;
			bool const force_relevant_events_OF_THEN_1 = false;
			std::string const calculate_required_OF_UE_1 = "always_true";
			std::string const calculate_required_OF_THEN_1 = "fn_fathers_and_trig";
			bool const No_trim_OF___ARBRE__EIRM = false;
			double const lambda_OF_FailureOf_A = 0.0001;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			bool const failI_FROZEN_OF_OnDemandFailureOf_S = false;
			bool const force_relevant_events_OF_FunctionOf_A_lost = false;
			bool const failF_FROZEN_OF_FailureOf_A = false;
			double const mu_OF_FailureOf_A = 0.1;
			bool const force_relevant_events_OF_FailureOf_S = false;
			std::string const calculate_required_OF_FunctionOf_A_lost = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_FailureOf_B = false;
			bool const force_relevant_events_OF_UE_1 = true;
			std::string const calculate_required_OF_FailureOf_S = "fn_fathers_and_trig";
			bool const trimming_OF_OPTIONS = true;
			std::string const calculate_required_OF_FailureOf_B = "fn_fathers_and_trig";
			std::string const when_to_check_OF_OnDemandFailureOf_S = "not_req_to_req";
			bool const Trim_article_OF___ARBRE__EIRM = false;
			double const mu_OF_OnDemandFailureOf_S = 0.1;
			bool const force_relevant_events_OF_S_unavailable = false;
			bool const Profil1_OF___ARBRE__EIRM = false;
			bool const No_repair_OF___ARBRE__EIRM = true;
			bool const force_relevant_events_OF_OR_3 = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_FailureOf_A;
            bool FIRE_xx10_OF_FailureOf_B;
            bool FIRE_xx10_OF_FailureOf_S;
            bool FIRE_xx23_OF_OnDemandFailureOf_S_INS_3;
            bool FIRE_xx23_OF_OnDemandFailureOf_S_INS_4;

            int required_OF_AND_1 = 0 ;
            int already_S_OF_AND_1 = 1 ;
            int S_OF_AND_1 = 2 ;
            int relevant_evt_OF_AND_1 = 3 ;
            int required_OF_FailureOf_A = 4 ;
            int already_S_OF_FailureOf_A = 5 ;
            int S_OF_FailureOf_A = 6 ;
            int relevant_evt_OF_FailureOf_A = 7 ;
            int failF_OF_FailureOf_A = 8 ;
            int required_OF_FailureOf_B = 9 ;
            int already_S_OF_FailureOf_B = 10 ;
            int S_OF_FailureOf_B = 11 ;
            int relevant_evt_OF_FailureOf_B = 12 ;
            int failF_OF_FailureOf_B = 13 ;
            int required_OF_FailureOf_S = 14 ;
            int already_S_OF_FailureOf_S = 15 ;
            int S_OF_FailureOf_S = 16 ;
            int relevant_evt_OF_FailureOf_S = 17 ;
            int failF_OF_FailureOf_S = 18 ;
            int required_OF_FunctionOf_A_lost = 19 ;
            int already_S_OF_FunctionOf_A_lost = 20 ;
            int S_OF_FunctionOf_A_lost = 21 ;
            int relevant_evt_OF_FunctionOf_A_lost = 22 ;
            int required_OF_OR_3 = 23 ;
            int already_S_OF_OR_3 = 24 ;
            int S_OF_OR_3 = 25 ;
            int relevant_evt_OF_OR_3 = 26 ;
            int required_OF_OnDemandFailureOf_S = 27 ;
            int already_S_OF_OnDemandFailureOf_S = 28 ;
            int S_OF_OnDemandFailureOf_S = 29 ;
            int relevant_evt_OF_OnDemandFailureOf_S = 30 ;
            int failI_OF_OnDemandFailureOf_S = 31 ;
            int to_be_fired_OF_OnDemandFailureOf_S = 32 ;
            int already_standby_OF_OnDemandFailureOf_S = 33 ;
            int already_required_OF_OnDemandFailureOf_S = 34 ;
            int required_OF_S_unavailable = 35 ;
            int already_S_OF_S_unavailable = 36 ;
            int S_OF_S_unavailable = 37 ;
            int relevant_evt_OF_S_unavailable = 38 ;
            int required_OF_THEN_1 = 39 ;
            int already_S_OF_THEN_1 = 40 ;
            int S_OF_THEN_1 = 41 ;
            int relevant_evt_OF_THEN_1 = 42 ;
            int required_OF_UE_1 = 43 ;
            int already_S_OF_UE_1 = 44 ;
            int S_OF_UE_1 = 45 ;
            int relevant_evt_OF_UE_1 = 46 ;




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