
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
        class FigaroProgram_BDMP_15_Trim_Max_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_15_Trim_Max_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_A1" , 0},
            	{"already_S_OF_A1" , 1},
            	{"S_OF_A1" , 2},
            	{"relevant_evt_OF_A1" , 3},
            	{"failF_OF_A1" , 4},
            	{"required_OF_A1_lost" , 5},
            	{"already_S_OF_A1_lost" , 6},
            	{"S_OF_A1_lost" , 7},
            	{"relevant_evt_OF_A1_lost" , 8},
            	{"required_OF_A2" , 9},
            	{"already_S_OF_A2" , 10},
            	{"S_OF_A2" , 11},
            	{"relevant_evt_OF_A2" , 12},
            	{"failF_OF_A2" , 13},
            	{"required_OF_A2_lost" , 14},
            	{"already_S_OF_A2_lost" , 15},
            	{"S_OF_A2_lost" , 16},
            	{"relevant_evt_OF_A2_lost" , 17},
            	{"required_OF_AND_1" , 18},
            	{"already_S_OF_AND_1" , 19},
            	{"S_OF_AND_1" , 20},
            	{"relevant_evt_OF_AND_1" , 21},
            	{"required_OF_A_lost" , 22},
            	{"already_S_OF_A_lost" , 23},
            	{"S_OF_A_lost" , 24},
            	{"relevant_evt_OF_A_lost" , 25},
            	{"required_OF_B1" , 26},
            	{"already_S_OF_B1" , 27},
            	{"S_OF_B1" , 28},
            	{"relevant_evt_OF_B1" , 29},
            	{"failF_OF_B1" , 30},
            	{"required_OF_B1_lost" , 31},
            	{"already_S_OF_B1_lost" , 32},
            	{"S_OF_B1_lost" , 33},
            	{"relevant_evt_OF_B1_lost" , 34},
            	{"required_OF_B2" , 35},
            	{"already_S_OF_B2" , 36},
            	{"S_OF_B2" , 37},
            	{"relevant_evt_OF_B2" , 38},
            	{"failF_OF_B2" , 39},
            	{"required_OF_B2_lost" , 40},
            	{"already_S_OF_B2_lost" , 41},
            	{"S_OF_B2_lost" , 42},
            	{"relevant_evt_OF_B2_lost" , 43},
            	{"required_OF_B_lost" , 44},
            	{"already_S_OF_B_lost" , 45},
            	{"S_OF_B_lost" , 46},
            	{"relevant_evt_OF_B_lost" , 47},
            	{"required_OF_K1" , 48},
            	{"already_S_OF_K1" , 49},
            	{"S_OF_K1" , 50},
            	{"relevant_evt_OF_K1" , 51},
            	{"failF_OF_K1" , 52},
            	{"required_OF_K2" , 53},
            	{"already_S_OF_K2" , 54},
            	{"S_OF_K2" , 55},
            	{"relevant_evt_OF_K2" , 56},
            	{"failF_OF_K2" , 57},
            	{"required_OF_K3" , 58},
            	{"already_S_OF_K3" , 59},
            	{"S_OF_K3" , 60},
            	{"relevant_evt_OF_K3" , 61},
            	{"failF_OF_K3" , 62},
            	{"required_OF_OR_1" , 63},
            	{"already_S_OF_OR_1" , 64},
            	{"S_OF_OR_1" , 65},
            	{"relevant_evt_OF_OR_1" , 66},
            	{"required_OF_OR_2" , 67},
            	{"already_S_OF_OR_2" , 68},
            	{"S_OF_OR_2" , 69},
            	{"relevant_evt_OF_OR_2" , 70},
            	{"required_OF_SF_1" , 71},
            	{"already_S_OF_SF_1" , 72},
            	{"S_OF_SF_1" , 73},
            	{"relevant_evt_OF_SF_1" , 74},
            	{"failF_OF_SF_1" , 75},
            	{"failS_OF_SF_1" , 76},
            	{"required_OF_UE_1" , 77},
            	{"already_S_OF_UE_1" , 78},
            	{"S_OF_UE_1" , 79},
            	{"relevant_evt_OF_UE_1" , 80}},

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
                    81 ,
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
            bool REINITIALISATION_OF_required_OF_A1 ;
            bool REINITIALISATION_OF_S_OF_A1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_A1 ;
            bool REINITIALISATION_OF_required_OF_A1_lost ;
            bool REINITIALISATION_OF_S_OF_A1_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_A1_lost ;
            bool REINITIALISATION_OF_required_OF_A2 ;
            bool REINITIALISATION_OF_S_OF_A2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_A2 ;
            bool REINITIALISATION_OF_required_OF_A2_lost ;
            bool REINITIALISATION_OF_S_OF_A2_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_A2_lost ;
            bool REINITIALISATION_OF_required_OF_AND_1 ;
            bool REINITIALISATION_OF_S_OF_AND_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_AND_1 ;
            bool REINITIALISATION_OF_required_OF_A_lost ;
            bool REINITIALISATION_OF_S_OF_A_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_A_lost ;
            bool REINITIALISATION_OF_required_OF_B1 ;
            bool REINITIALISATION_OF_S_OF_B1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_B1 ;
            bool REINITIALISATION_OF_required_OF_B1_lost ;
            bool REINITIALISATION_OF_S_OF_B1_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_B1_lost ;
            bool REINITIALISATION_OF_required_OF_B2 ;
            bool REINITIALISATION_OF_S_OF_B2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_B2 ;
            bool REINITIALISATION_OF_required_OF_B2_lost ;
            bool REINITIALISATION_OF_S_OF_B2_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_B2_lost ;
            bool REINITIALISATION_OF_required_OF_B_lost ;
            bool REINITIALISATION_OF_S_OF_B_lost ;
            bool REINITIALISATION_OF_relevant_evt_OF_B_lost ;
            bool REINITIALISATION_OF_required_OF_K1 ;
            bool REINITIALISATION_OF_S_OF_K1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_K1 ;
            bool REINITIALISATION_OF_required_OF_K2 ;
            bool REINITIALISATION_OF_S_OF_K2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_K2 ;
            bool REINITIALISATION_OF_required_OF_K3 ;
            bool REINITIALISATION_OF_S_OF_K3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_K3 ;
            bool REINITIALISATION_OF_required_OF_OR_1 ;
            bool REINITIALISATION_OF_S_OF_OR_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_1 ;
            bool REINITIALISATION_OF_required_OF_OR_2 ;
            bool REINITIALISATION_OF_S_OF_OR_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_2 ;
            bool REINITIALISATION_OF_required_OF_SF_1 ;
            bool REINITIALISATION_OF_S_OF_SF_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_SF_1 ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			std::string const calculate_required_OF_A1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_A2 = false;
			bool const force_relevant_events_OF_B2_lost = false;
			std::string const calculate_required_OF_B_lost = "fn_fathers_and_trig";
			bool const repairable_system_OF_OPTIONS = true;
			std::string const calculate_required_OF_A2 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_A2_lost = "fn_fathers_and_trig";
			double const lambda_OF_K1 = 0.0001;
			std::string const calculate_required_OF_OR_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_SF_1 = false;
			bool const force_relevant_events_OF_AND_1 = false;
			std::string const calculate_required_OF_B2_lost = "fn_fathers_and_trig";
			double const lambda_OF_B2 = 0.0001;
			double const lambda_OF_K3 = 0.0001;
			std::string const calculate_required_OF_SF_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_A_lost = false;
			double const mu_OF_K1 = 0.1;
			bool const failF_FROZEN_OF_K1 = false;
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_3 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_B1 = false;
			bool const No_repair_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_A_lost = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_B2 = false;
			double const mu_OF_B2 = 0.1;
			double const mu_OF_K3 = 0.1;
			bool const failF_FROZEN_OF_K3 = false;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			bool const Default_OF___ARBRE__EIRM = true;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_OR_2 = false;
			std::string const calculate_required_OF_B1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_UE_1 = "always_true";
			double const lambda_OF_K2 = 0.0001;
			bool const failF_FROZEN_OF_K2 = false;
			double const mu_OF_K2 = 0.1;
			bool const No_trim_OF___ARBRE__EIRM = false;
			double const lambda_OF_A1 = 0.0001;
			bool const force_relevant_events_OF_K1 = false;
			double const lambda_OF_A2 = 0.0001;
			std::string const calculate_required_OF_K1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_B2 = false;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			bool const force_relevant_events_OF_A1_lost = false;
			bool const failF_FROZEN_OF_A1 = false;
			double const mu_OF_A1 = 0.1;
			bool const force_relevant_events_OF_B1_lost = false;
			bool const force_relevant_events_OF_K3 = false;
			double const lambda_OF_SF_1 = 0.0001;
			std::string const calculate_required_OF_A1_lost = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_A2 = false;
			double const mu_OF_A2 = 0.1;
			std::string const calculate_required_OF_B2 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_K3 = "fn_fathers_and_trig";
			double const standby_lambda_OF_SF_1 = 1e-05;
			bool const force_relevant_events_OF_OR_1 = false;
			std::string const calculate_required_OF_B1_lost = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_UE_1 = true;
			bool const trimming_OF_OPTIONS = true;
			bool const failF_FROZEN_OF_SF_1 = false;
			double const mu_OF_SF_1 = 0.1;
			bool const force_relevant_events_OF_K2 = false;
			std::string const calculate_required_OF_OR_1 = "fn_fathers_and_trig";
			bool const Trim_article_OF___ARBRE__EIRM = false;
			double const lambda_OF_B1 = 0.0001;
			std::string const calculate_required_OF_K2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_B1 = false;
			bool const Profil1_OF___ARBRE__EIRM = false;
			bool const failS_FROZEN_OF_SF_1 = false;
			bool const force_relevant_events_OF_B_lost = false;
			bool const force_relevant_events_OF_A2_lost = false;
			bool const force_relevant_events_OF_A1 = false;
			double const mu_OF_B1 = 0.1;
			std::string const trimming_option_OF_OPTIONS = "maximum";
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_A1;
            bool FIRE_xx11_OF_A1;
            bool FIRE_xx10_OF_A2;
            bool FIRE_xx11_OF_A2;
            bool FIRE_xx10_OF_B1;
            bool FIRE_xx11_OF_B1;
            bool FIRE_xx10_OF_B2;
            bool FIRE_xx11_OF_B2;
            bool FIRE_xx10_OF_K1;
            bool FIRE_xx11_OF_K1;
            bool FIRE_xx10_OF_K2;
            bool FIRE_xx11_OF_K2;
            bool FIRE_xx10_OF_K3;
            bool FIRE_xx11_OF_K3;
            bool FIRE_xx17_OF_SF_1;
            bool FIRE_xx18_OF_SF_1;
            bool FIRE_xx19_OF_SF_1;

            int required_OF_A1 = 0 ;
            int already_S_OF_A1 = 1 ;
            int S_OF_A1 = 2 ;
            int relevant_evt_OF_A1 = 3 ;
            int failF_OF_A1 = 4 ;
            int required_OF_A1_lost = 5 ;
            int already_S_OF_A1_lost = 6 ;
            int S_OF_A1_lost = 7 ;
            int relevant_evt_OF_A1_lost = 8 ;
            int required_OF_A2 = 9 ;
            int already_S_OF_A2 = 10 ;
            int S_OF_A2 = 11 ;
            int relevant_evt_OF_A2 = 12 ;
            int failF_OF_A2 = 13 ;
            int required_OF_A2_lost = 14 ;
            int already_S_OF_A2_lost = 15 ;
            int S_OF_A2_lost = 16 ;
            int relevant_evt_OF_A2_lost = 17 ;
            int required_OF_AND_1 = 18 ;
            int already_S_OF_AND_1 = 19 ;
            int S_OF_AND_1 = 20 ;
            int relevant_evt_OF_AND_1 = 21 ;
            int required_OF_A_lost = 22 ;
            int already_S_OF_A_lost = 23 ;
            int S_OF_A_lost = 24 ;
            int relevant_evt_OF_A_lost = 25 ;
            int required_OF_B1 = 26 ;
            int already_S_OF_B1 = 27 ;
            int S_OF_B1 = 28 ;
            int relevant_evt_OF_B1 = 29 ;
            int failF_OF_B1 = 30 ;
            int required_OF_B1_lost = 31 ;
            int already_S_OF_B1_lost = 32 ;
            int S_OF_B1_lost = 33 ;
            int relevant_evt_OF_B1_lost = 34 ;
            int required_OF_B2 = 35 ;
            int already_S_OF_B2 = 36 ;
            int S_OF_B2 = 37 ;
            int relevant_evt_OF_B2 = 38 ;
            int failF_OF_B2 = 39 ;
            int required_OF_B2_lost = 40 ;
            int already_S_OF_B2_lost = 41 ;
            int S_OF_B2_lost = 42 ;
            int relevant_evt_OF_B2_lost = 43 ;
            int required_OF_B_lost = 44 ;
            int already_S_OF_B_lost = 45 ;
            int S_OF_B_lost = 46 ;
            int relevant_evt_OF_B_lost = 47 ;
            int required_OF_K1 = 48 ;
            int already_S_OF_K1 = 49 ;
            int S_OF_K1 = 50 ;
            int relevant_evt_OF_K1 = 51 ;
            int failF_OF_K1 = 52 ;
            int required_OF_K2 = 53 ;
            int already_S_OF_K2 = 54 ;
            int S_OF_K2 = 55 ;
            int relevant_evt_OF_K2 = 56 ;
            int failF_OF_K2 = 57 ;
            int required_OF_K3 = 58 ;
            int already_S_OF_K3 = 59 ;
            int S_OF_K3 = 60 ;
            int relevant_evt_OF_K3 = 61 ;
            int failF_OF_K3 = 62 ;
            int required_OF_OR_1 = 63 ;
            int already_S_OF_OR_1 = 64 ;
            int S_OF_OR_1 = 65 ;
            int relevant_evt_OF_OR_1 = 66 ;
            int required_OF_OR_2 = 67 ;
            int already_S_OF_OR_2 = 68 ;
            int S_OF_OR_2 = 69 ;
            int relevant_evt_OF_OR_2 = 70 ;
            int required_OF_SF_1 = 71 ;
            int already_S_OF_SF_1 = 72 ;
            int S_OF_SF_1 = 73 ;
            int relevant_evt_OF_SF_1 = 74 ;
            int failF_OF_SF_1 = 75 ;
            int failS_OF_SF_1 = 76 ;
            int required_OF_UE_1 = 77 ;
            int already_S_OF_UE_1 = 78 ;
            int S_OF_UE_1 = 79 ;
            int relevant_evt_OF_UE_1 = 80 ;




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