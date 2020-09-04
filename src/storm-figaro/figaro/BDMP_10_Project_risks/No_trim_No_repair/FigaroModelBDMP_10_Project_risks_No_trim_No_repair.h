
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
        class FigaroProgram_BDMP_10_Project_risks_No_trim_No_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_10_Project_risks_No_trim_No_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_AND_1" , 0},
            	{"already_S_OF_AND_1" , 1},
            	{"S_OF_AND_1" , 2},
            	{"relevant_evt_OF_AND_1" , 3},
            	{"required_OF_AND_1_1" , 4},
            	{"already_S_OF_AND_1_1" , 5},
            	{"S_OF_AND_1_1" , 6},
            	{"relevant_evt_OF_AND_1_1" , 7},
            	{"required_OF_F_1" , 8},
            	{"already_S_OF_F_1" , 9},
            	{"S_OF_F_1" , 10},
            	{"relevant_evt_OF_F_1" , 11},
            	{"failF_OF_F_1" , 12},
            	{"required_OF_F_2" , 13},
            	{"already_S_OF_F_2" , 14},
            	{"S_OF_F_2" , 15},
            	{"relevant_evt_OF_F_2" , 16},
            	{"failF_OF_F_2" , 17},
            	{"required_OF_F_3" , 18},
            	{"already_S_OF_F_3" , 19},
            	{"S_OF_F_3" , 20},
            	{"relevant_evt_OF_F_3" , 21},
            	{"failF_OF_F_3" , 22},
            	{"required_OF_F_3_1" , 23},
            	{"already_S_OF_F_3_1" , 24},
            	{"S_OF_F_3_1" , 25},
            	{"relevant_evt_OF_F_3_1" , 26},
            	{"failF_OF_F_3_1" , 27},
            	{"required_OF_F_4" , 28},
            	{"already_S_OF_F_4" , 29},
            	{"S_OF_F_4" , 30},
            	{"relevant_evt_OF_F_4" , 31},
            	{"failF_OF_F_4" , 32},
            	{"required_OF_I_1" , 33},
            	{"already_S_OF_I_1" , 34},
            	{"S_OF_I_1" , 35},
            	{"relevant_evt_OF_I_1" , 36},
            	{"failI_OF_I_1" , 37},
            	{"to_be_fired_OF_I_1" , 38},
            	{"already_standby_OF_I_1" , 39},
            	{"already_required_OF_I_1" , 40},
            	{"required_OF_Major_risk" , 41},
            	{"already_S_OF_Major_risk" , 42},
            	{"S_OF_Major_risk" , 43},
            	{"relevant_evt_OF_Major_risk" , 44},
            	{"required_OF_UE_1" , 45},
            	{"already_S_OF_UE_1" , 46},
            	{"S_OF_UE_1" , 47},
            	{"relevant_evt_OF_UE_1" , 48}},

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
                    49 ,
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
            bool REINITIALISATION_OF_required_OF_AND_1_1 ;
            bool REINITIALISATION_OF_S_OF_AND_1_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_AND_1_1 ;
            bool REINITIALISATION_OF_required_OF_F_1 ;
            bool REINITIALISATION_OF_S_OF_F_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_F_1 ;
            bool REINITIALISATION_OF_required_OF_F_2 ;
            bool REINITIALISATION_OF_S_OF_F_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_F_2 ;
            bool REINITIALISATION_OF_required_OF_F_3 ;
            bool REINITIALISATION_OF_S_OF_F_3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_F_3 ;
            bool REINITIALISATION_OF_required_OF_F_3_1 ;
            bool REINITIALISATION_OF_S_OF_F_3_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_F_3_1 ;
            bool REINITIALISATION_OF_required_OF_F_4 ;
            bool REINITIALISATION_OF_S_OF_F_4 ;
            bool REINITIALISATION_OF_relevant_evt_OF_F_4 ;
            bool REINITIALISATION_OF_required_OF_I_1 ;
            bool REINITIALISATION_OF_S_OF_I_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_I_1 ;
            bool REINITIALISATION_OF_to_be_fired_OF_I_1 ;
            bool REINITIALISATION_OF_required_OF_Major_risk ;
            bool REINITIALISATION_OF_S_OF_Major_risk ;
            bool REINITIALISATION_OF_relevant_evt_OF_Major_risk ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			std::string const trigger_kind_OF_t_1_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_AND_1_1 = false;
			bool const force_relevant_events_OF_AND_1 = false;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_F_3 = false;
			bool const No_repair_OF___ARBRE__EIRM = true;
			bool const force_relevant_events_OF_Major_risk = false;
			std::string const calculate_required_OF_AND_1_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_F_4 = false;
			bool const failF_FROZEN_OF_F_1 = false;
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_UE_1 = "always_true";
			bool const repairable_system_OF_OPTIONS = false;
			double const lambda_OF_F_3 = 0.0001;
			double const lambda_OF_F_1 = 0.0001;
			bool const force_relevant_events_OF_I_1 = false;
			std::string const calculate_required_OF_Major_risk = "fn_fathers_and_trig";
			bool const No_trim_OF___ARBRE__EIRM = true;
			bool const Profil1_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_F_4 = "fn_fathers_and_trig";
			double const mu_OF_F_4 = 0.1;
			bool const Default_OF___ARBRE__EIRM = true;
			bool const failF_FROZEN_OF_F_3_1 = false;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			double const gamma_OF_I_1 = 0.0001;
			bool const failF_FROZEN_OF_F_2 = false;
			std::string const calculate_required_OF_I_1 = "fn_fathers_and_trig";
			double const mu_OF_I_1 = 0.1;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_F_3 = false;
			bool const force_relevant_events_OF_F_1 = false;
			double const lambda_OF_F_3_1 = 0.0001;
			double const lambda_OF_F_2 = 0.0001;
			bool const force_relevant_events_OF_UE_1 = true;
			std::string const calculate_required_OF_F_3 = "fn_fathers_and_trig";
			std::string const trimming_option_OF_OPTIONS = "maximum";
			double const mu_OF_F_3 = 0.1;
			std::string const calculate_required_OF_F_1 = "fn_fathers_and_trig";
			double const mu_OF_F_1 = 0.1;
			bool const Trim_article_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_F_3_1 = false;
			bool const force_relevant_events_OF_F_2 = false;
			bool const failF_FROZEN_OF_F_4 = false;
			std::string const when_to_check_OF_I_1 = "not_req_to_req";
			std::string const calculate_required_OF_F_3_1 = "fn_fathers_and_trig";
			double const mu_OF_F_3_1 = 0.1;
			std::string const calculate_required_OF_F_2 = "fn_fathers_and_trig";
			double const mu_OF_F_2 = 0.1;
			bool const failI_FROZEN_OF_I_1 = false;
			double const lambda_OF_F_4 = 0.0001;
			bool const trimming_OF_OPTIONS = false;
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_F_1;
            bool FIRE_xx10_OF_F_2;
            bool FIRE_xx10_OF_F_3;
            bool FIRE_xx10_OF_F_3_1;
            bool FIRE_xx10_OF_F_4;
            bool FIRE_xx23_OF_I_1_INS_5;
            bool FIRE_xx23_OF_I_1_INS_6;

            int required_OF_AND_1 = 0 ;
            int already_S_OF_AND_1 = 1 ;
            int S_OF_AND_1 = 2 ;
            int relevant_evt_OF_AND_1 = 3 ;
            int required_OF_AND_1_1 = 4 ;
            int already_S_OF_AND_1_1 = 5 ;
            int S_OF_AND_1_1 = 6 ;
            int relevant_evt_OF_AND_1_1 = 7 ;
            int required_OF_F_1 = 8 ;
            int already_S_OF_F_1 = 9 ;
            int S_OF_F_1 = 10 ;
            int relevant_evt_OF_F_1 = 11 ;
            int failF_OF_F_1 = 12 ;
            int required_OF_F_2 = 13 ;
            int already_S_OF_F_2 = 14 ;
            int S_OF_F_2 = 15 ;
            int relevant_evt_OF_F_2 = 16 ;
            int failF_OF_F_2 = 17 ;
            int required_OF_F_3 = 18 ;
            int already_S_OF_F_3 = 19 ;
            int S_OF_F_3 = 20 ;
            int relevant_evt_OF_F_3 = 21 ;
            int failF_OF_F_3 = 22 ;
            int required_OF_F_3_1 = 23 ;
            int already_S_OF_F_3_1 = 24 ;
            int S_OF_F_3_1 = 25 ;
            int relevant_evt_OF_F_3_1 = 26 ;
            int failF_OF_F_3_1 = 27 ;
            int required_OF_F_4 = 28 ;
            int already_S_OF_F_4 = 29 ;
            int S_OF_F_4 = 30 ;
            int relevant_evt_OF_F_4 = 31 ;
            int failF_OF_F_4 = 32 ;
            int required_OF_I_1 = 33 ;
            int already_S_OF_I_1 = 34 ;
            int S_OF_I_1 = 35 ;
            int relevant_evt_OF_I_1 = 36 ;
            int failI_OF_I_1 = 37 ;
            int to_be_fired_OF_I_1 = 38 ;
            int already_standby_OF_I_1 = 39 ;
            int already_required_OF_I_1 = 40 ;
            int required_OF_Major_risk = 41 ;
            int already_S_OF_Major_risk = 42 ;
            int S_OF_Major_risk = 43 ;
            int relevant_evt_OF_Major_risk = 44 ;
            int required_OF_UE_1 = 45 ;
            int already_S_OF_UE_1 = 46 ;
            int S_OF_UE_1 = 47 ;
            int relevant_evt_OF_UE_1 = 48 ;




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