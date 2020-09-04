
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
        class FigaroProgram_BDMP_16_Cab2_eng_Trim_Max_No_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_16_Cab2_eng_Trim_Max_No_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_AND_1" , 0},
            	{"already_S_OF_AND_1" , 1},
            	{"S_OF_AND_1" , 2},
            	{"relevant_evt_OF_AND_1" , 3},
            	{"required_OF_AND_2" , 4},
            	{"already_S_OF_AND_2" , 5},
            	{"S_OF_AND_2" , 6},
            	{"relevant_evt_OF_AND_2" , 7},
            	{"required_OF_C1" , 8},
            	{"already_S_OF_C1" , 9},
            	{"S_OF_C1" , 10},
            	{"relevant_evt_OF_C1" , 11},
            	{"failF_OF_C1" , 12},
            	{"required_OF_D1" , 13},
            	{"already_S_OF_D1" , 14},
            	{"S_OF_D1" , 15},
            	{"relevant_evt_OF_D1" , 16},
            	{"failF_OF_D1" , 17},
            	{"required_OF_E1" , 18},
            	{"already_S_OF_E1" , 19},
            	{"S_OF_E1" , 20},
            	{"relevant_evt_OF_E1" , 21},
            	{"failF_OF_E1" , 22},
            	{"required_OF_OR_1" , 23},
            	{"already_S_OF_OR_1" , 24},
            	{"S_OF_OR_1" , 25},
            	{"relevant_evt_OF_OR_1" , 26},
            	{"required_OF_OR_2" , 27},
            	{"already_S_OF_OR_2" , 28},
            	{"S_OF_OR_2" , 29},
            	{"relevant_evt_OF_OR_2" , 30},
            	{"required_OF_OR_3" , 31},
            	{"already_S_OF_OR_3" , 32},
            	{"S_OF_OR_3" , 33},
            	{"relevant_evt_OF_OR_3" , 34},
            	{"required_OF_OR_4" , 35},
            	{"already_S_OF_OR_4" , 36},
            	{"S_OF_OR_4" , 37},
            	{"relevant_evt_OF_OR_4" , 38},
            	{"required_OF_SF_A" , 39},
            	{"already_S_OF_SF_A" , 40},
            	{"S_OF_SF_A" , 41},
            	{"relevant_evt_OF_SF_A" , 42},
            	{"failF_OF_SF_A" , 43},
            	{"failS_OF_SF_A" , 44},
            	{"required_OF_SF_B" , 45},
            	{"already_S_OF_SF_B" , 46},
            	{"S_OF_SF_B" , 47},
            	{"relevant_evt_OF_SF_B" , 48},
            	{"failF_OF_SF_B" , 49},
            	{"failS_OF_SF_B" , 50},
            	{"required_OF_UE_1" , 51},
            	{"already_S_OF_UE_1" , 52},
            	{"S_OF_UE_1" , 53},
            	{"relevant_evt_OF_UE_1" , 54}},

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
                    55 ,
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
            bool REINITIALISATION_OF_required_OF_C1 ;
            bool REINITIALISATION_OF_S_OF_C1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_C1 ;
            bool REINITIALISATION_OF_required_OF_D1 ;
            bool REINITIALISATION_OF_S_OF_D1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_D1 ;
            bool REINITIALISATION_OF_required_OF_E1 ;
            bool REINITIALISATION_OF_S_OF_E1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_E1 ;
            bool REINITIALISATION_OF_required_OF_OR_1 ;
            bool REINITIALISATION_OF_S_OF_OR_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_1 ;
            bool REINITIALISATION_OF_required_OF_OR_2 ;
            bool REINITIALISATION_OF_S_OF_OR_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_2 ;
            bool REINITIALISATION_OF_required_OF_OR_3 ;
            bool REINITIALISATION_OF_S_OF_OR_3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_3 ;
            bool REINITIALISATION_OF_required_OF_OR_4 ;
            bool REINITIALISATION_OF_S_OF_OR_4 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_4 ;
            bool REINITIALISATION_OF_required_OF_SF_A ;
            bool REINITIALISATION_OF_S_OF_SF_A ;
            bool REINITIALISATION_OF_relevant_evt_OF_SF_A ;
            bool REINITIALISATION_OF_required_OF_SF_B ;
            bool REINITIALISATION_OF_S_OF_SF_B ;
            bool REINITIALISATION_OF_relevant_evt_OF_SF_B ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			bool const force_relevant_events_OF_OR_4 = false;
			double const standby_lambda_OF_SF_B = 1e-05;
			std::string const calculate_required_OF_OR_3 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_OR_2 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_OR_4 = "fn_fathers_and_trig";
			double const lambda_OF_SF_B = 0.0001;
			double const lambda_OF_C1 = 0.0001;
			bool const force_relevant_events_OF_AND_1 = false;
			bool const failF_FROZEN_OF_SF_B = false;
			double const mu_OF_SF_B = 0.1;
			bool const failF_FROZEN_OF_C1 = false;
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			double const mu_OF_C1 = 0.1;
			bool const force_relevant_events_OF_D1 = false;
			double const lambda_OF_E1 = 0.0001;
			bool const failS_FROZEN_OF_SF_B = false;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			bool const Default_OF___ARBRE__EIRM = true;
			std::string const calculate_required_OF_D1 = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const repairable_system_OF_OPTIONS = false;
			bool const failF_FROZEN_OF_E1 = false;
			double const mu_OF_E1 = 0.1;
			bool const force_relevant_events_OF_OR_2 = false;
			double const standby_lambda_OF_SF_A = 1e-05;
			std::string const calculate_required_OF_UE_1 = "always_true";
			double const lambda_OF_SF_A = 0.0001;
			bool const force_relevant_events_OF_SF_B = false;
			bool const force_relevant_events_OF_AND_2 = false;
			bool const No_trim_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_C1 = false;
			bool const failF_FROZEN_OF_SF_A = false;
			std::string const calculate_required_OF_AND_2 = "fn_fathers_and_trig";
			double const mu_OF_SF_A = 0.1;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			std::string const calculate_required_OF_SF_B = "fn_fathers_and_trig";
			std::string const calculate_required_OF_C1 = "fn_fathers_and_trig";
			bool const failS_FROZEN_OF_SF_A = false;
			bool const force_relevant_events_OF_E1 = false;
			bool const force_relevant_events_OF_OR_1 = false;
			bool const force_relevant_events_OF_UE_1 = true;
			std::string const calculate_required_OF_E1 = "fn_fathers_and_trig";
			bool const trimming_OF_OPTIONS = true;
			bool const Profil_base_OF___ARBRE__EIRM = false;
			double const lambda_OF_D1 = 0.0001;
			std::string const calculate_required_OF_OR_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_SF_A = false;
			bool const Trim_article_OF___ARBRE__EIRM = false;
			bool const failF_FROZEN_OF_D1 = false;
			double const mu_OF_D1 = 0.1;
			std::string const calculate_required_OF_SF_A = "fn_fathers_and_trig";
			bool const No_repair_OF___ARBRE__EIRM = true;
			bool const force_relevant_events_OF_OR_3 = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_C1;
            bool FIRE_xx10_OF_D1;
            bool FIRE_xx10_OF_E1;
            bool FIRE_xx17_OF_SF_A;
            bool FIRE_xx18_OF_SF_A;
            bool FIRE_xx17_OF_SF_B;
            bool FIRE_xx18_OF_SF_B;

            int required_OF_AND_1 = 0 ;
            int already_S_OF_AND_1 = 1 ;
            int S_OF_AND_1 = 2 ;
            int relevant_evt_OF_AND_1 = 3 ;
            int required_OF_AND_2 = 4 ;
            int already_S_OF_AND_2 = 5 ;
            int S_OF_AND_2 = 6 ;
            int relevant_evt_OF_AND_2 = 7 ;
            int required_OF_C1 = 8 ;
            int already_S_OF_C1 = 9 ;
            int S_OF_C1 = 10 ;
            int relevant_evt_OF_C1 = 11 ;
            int failF_OF_C1 = 12 ;
            int required_OF_D1 = 13 ;
            int already_S_OF_D1 = 14 ;
            int S_OF_D1 = 15 ;
            int relevant_evt_OF_D1 = 16 ;
            int failF_OF_D1 = 17 ;
            int required_OF_E1 = 18 ;
            int already_S_OF_E1 = 19 ;
            int S_OF_E1 = 20 ;
            int relevant_evt_OF_E1 = 21 ;
            int failF_OF_E1 = 22 ;
            int required_OF_OR_1 = 23 ;
            int already_S_OF_OR_1 = 24 ;
            int S_OF_OR_1 = 25 ;
            int relevant_evt_OF_OR_1 = 26 ;
            int required_OF_OR_2 = 27 ;
            int already_S_OF_OR_2 = 28 ;
            int S_OF_OR_2 = 29 ;
            int relevant_evt_OF_OR_2 = 30 ;
            int required_OF_OR_3 = 31 ;
            int already_S_OF_OR_3 = 32 ;
            int S_OF_OR_3 = 33 ;
            int relevant_evt_OF_OR_3 = 34 ;
            int required_OF_OR_4 = 35 ;
            int already_S_OF_OR_4 = 36 ;
            int S_OF_OR_4 = 37 ;
            int relevant_evt_OF_OR_4 = 38 ;
            int required_OF_SF_A = 39 ;
            int already_S_OF_SF_A = 40 ;
            int S_OF_SF_A = 41 ;
            int relevant_evt_OF_SF_A = 42 ;
            int failF_OF_SF_A = 43 ;
            int failS_OF_SF_A = 44 ;
            int required_OF_SF_B = 45 ;
            int already_S_OF_SF_B = 46 ;
            int S_OF_SF_B = 47 ;
            int relevant_evt_OF_SF_B = 48 ;
            int failF_OF_SF_B = 49 ;
            int failS_OF_SF_B = 50 ;
            int required_OF_UE_1 = 51 ;
            int already_S_OF_UE_1 = 52 ;
            int S_OF_UE_1 = 53 ;
            int relevant_evt_OF_UE_1 = 54 ;




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