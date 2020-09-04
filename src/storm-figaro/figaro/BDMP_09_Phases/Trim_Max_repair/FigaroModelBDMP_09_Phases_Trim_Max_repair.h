
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
        class FigaroProgram_BDMP_09_Phases_Trim_Max_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_09_Phases_Trim_Max_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_AND_1" , 0},
            	{"already_S_OF_AND_1" , 1},
            	{"S_OF_AND_1" , 2},
            	{"relevant_evt_OF_AND_1" , 3},
            	{"required_OF_AND_2" , 4},
            	{"already_S_OF_AND_2" , 5},
            	{"S_OF_AND_2" , 6},
            	{"relevant_evt_OF_AND_2" , 7},
            	{"required_OF_AND_3" , 8},
            	{"already_S_OF_AND_3" , 9},
            	{"S_OF_AND_3" , 10},
            	{"relevant_evt_OF_AND_3" , 11},
            	{"required_OF_Cpt_A" , 12},
            	{"already_S_OF_Cpt_A" , 13},
            	{"S_OF_Cpt_A" , 14},
            	{"relevant_evt_OF_Cpt_A" , 15},
            	{"failF_OF_Cpt_A" , 16},
            	{"required_OF_Cpt_B" , 17},
            	{"already_S_OF_Cpt_B" , 18},
            	{"S_OF_Cpt_B" , 19},
            	{"relevant_evt_OF_Cpt_B" , 20},
            	{"failF_OF_Cpt_B" , 21},
            	{"required_OF_Cpt_C" , 22},
            	{"already_S_OF_Cpt_C" , 23},
            	{"S_OF_Cpt_C" , 24},
            	{"relevant_evt_OF_Cpt_C" , 25},
            	{"failF_OF_Cpt_C" , 26},
            	{"required_OF_OR_1" , 27},
            	{"already_S_OF_OR_1" , 28},
            	{"S_OF_OR_1" , 29},
            	{"relevant_evt_OF_OR_1" , 30},
            	{"required_OF_OR_2" , 31},
            	{"already_S_OF_OR_2" , 32},
            	{"S_OF_OR_2" , 33},
            	{"relevant_evt_OF_OR_2" , 34},
            	{"required_OF_OR_3" , 35},
            	{"already_S_OF_OR_3" , 36},
            	{"S_OF_OR_3" , 37},
            	{"relevant_evt_OF_OR_3" , 38},
            	{"required_OF_UE_1" , 39},
            	{"already_S_OF_UE_1" , 40},
            	{"S_OF_UE_1" , 41},
            	{"relevant_evt_OF_UE_1" , 42},
            	{"required_OF_phase_1" , 43},
            	{"already_S_OF_phase_1" , 44},
            	{"S_OF_phase_1" , 45},
            	{"relevant_evt_OF_phase_1" , 46},
            	{"in_progress_OF_phase_1" , 47},
            	{"already_required_OF_phase_1" , 48},
            	{"start_phase_OF_phase_1" , 49},
            	{"required_OF_phase_2" , 50},
            	{"already_S_OF_phase_2" , 51},
            	{"S_OF_phase_2" , 52},
            	{"relevant_evt_OF_phase_2" , 53},
            	{"in_progress_OF_phase_2" , 54},
            	{"already_required_OF_phase_2" , 55},
            	{"start_phase_OF_phase_2" , 56}},

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
                    57 ,
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
            bool REINITIALISATION_OF_required_OF_AND_3 ;
            bool REINITIALISATION_OF_S_OF_AND_3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_AND_3 ;
            bool REINITIALISATION_OF_required_OF_Cpt_A ;
            bool REINITIALISATION_OF_S_OF_Cpt_A ;
            bool REINITIALISATION_OF_relevant_evt_OF_Cpt_A ;
            bool REINITIALISATION_OF_required_OF_Cpt_B ;
            bool REINITIALISATION_OF_S_OF_Cpt_B ;
            bool REINITIALISATION_OF_relevant_evt_OF_Cpt_B ;
            bool REINITIALISATION_OF_required_OF_Cpt_C ;
            bool REINITIALISATION_OF_S_OF_Cpt_C ;
            bool REINITIALISATION_OF_relevant_evt_OF_Cpt_C ;
            bool REINITIALISATION_OF_required_OF_OR_1 ;
            bool REINITIALISATION_OF_S_OF_OR_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_1 ;
            bool REINITIALISATION_OF_required_OF_OR_2 ;
            bool REINITIALISATION_OF_S_OF_OR_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_2 ;
            bool REINITIALISATION_OF_required_OF_OR_3 ;
            bool REINITIALISATION_OF_S_OF_OR_3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_3 ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            bool REINITIALISATION_OF_required_OF_phase_1 ;
            bool REINITIALISATION_OF_S_OF_phase_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_phase_1 ;
            bool REINITIALISATION_OF_required_OF_phase_2 ;
            bool REINITIALISATION_OF_S_OF_phase_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_phase_2 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			std::string const calculate_required_OF_OR_3 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_OR_2 = "fn_fathers_and_trig";
			bool const repairable_system_OF_OPTIONS = true;
			std::string const trigger_kind_OF_t_4 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_AND_3 = false;
			double const lambda_OF_Cpt_B = 0.0001;
			double const duration_OF_phase_1 = 10;
			bool const force_relevant_events_OF_AND_1 = false;
			bool const force_relevant_events_OF_Cpt_A = false;
			std::string const calculate_required_OF_AND_3 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_Cpt_B = false;
			double const mu_OF_Cpt_B = 0.1;
			std::string const trigger_kind_OF_t_3 = "fn_fathers_and_trig";
			bool const No_repair_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_Cpt_A = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			bool const Default_OF___ARBRE__EIRM = true;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_OR_2 = false;
			std::string const calculate_required_OF_UE_1 = "always_true";
			std::string const Dist_kind_OF_phase_2 = "exponential";
			double const lambda_OF_Cpt_C = 0.0001;
			bool const force_relevant_events_OF_AND_2 = false;
			bool const No_trim_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_Cpt_B = false;
			std::string const calculate_required_OF_AND_2 = "fn_fathers_and_trig";
			bool const to_be_taken_into_account_OF_UE_1 = true;
			bool const force_relevant_events_OF_phase_2 = false;
			bool const failF_FROZEN_OF_Cpt_C = false;
			double const mu_OF_Cpt_C = 0.1;
			std::string const calculate_required_OF_phase_2 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_Cpt_B = "fn_fathers_and_trig";
			std::string const Dist_kind_OF_phase_1 = "exponential";
			bool const force_relevant_events_OF_OR_1 = false;
			bool const force_relevant_events_OF_UE_1 = true;
			bool const trimming_OF_OPTIONS = true;
			double const lambda_OF_Cpt_A = 0.0001;
			double const duration_OF_phase_2 = 10;
			std::string const calculate_required_OF_OR_1 = "fn_fathers_and_trig";
			bool const Trim_article_OF___ARBRE__EIRM = false;
			bool const failF_FROZEN_OF_Cpt_A = false;
			double const mu_OF_Cpt_A = 0.1;
			bool const force_relevant_events_OF_phase_1 = false;
			bool const force_relevant_events_OF_Cpt_C = false;
			std::string const calculate_required_OF_phase_1 = "fn_fathers_and_trig";
			bool const Profil1_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_Cpt_C = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_OR_3 = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_Cpt_A;
            bool FIRE_xx11_OF_Cpt_A;
            bool FIRE_xx10_OF_Cpt_B;
            bool FIRE_xx11_OF_Cpt_B;
            bool FIRE_xx10_OF_Cpt_C;
            bool FIRE_xx11_OF_Cpt_C;
            bool FIRE_xx43_a_OF_phase_1;
            bool FIRE_xx47_OF_phase_1_INS_7;
            bool FIRE_xx43_a_OF_phase_2;
            bool FIRE_xx47_OF_phase_2_INS_9;

            int required_OF_AND_1 = 0 ;
            int already_S_OF_AND_1 = 1 ;
            int S_OF_AND_1 = 2 ;
            int relevant_evt_OF_AND_1 = 3 ;
            int required_OF_AND_2 = 4 ;
            int already_S_OF_AND_2 = 5 ;
            int S_OF_AND_2 = 6 ;
            int relevant_evt_OF_AND_2 = 7 ;
            int required_OF_AND_3 = 8 ;
            int already_S_OF_AND_3 = 9 ;
            int S_OF_AND_3 = 10 ;
            int relevant_evt_OF_AND_3 = 11 ;
            int required_OF_Cpt_A = 12 ;
            int already_S_OF_Cpt_A = 13 ;
            int S_OF_Cpt_A = 14 ;
            int relevant_evt_OF_Cpt_A = 15 ;
            int failF_OF_Cpt_A = 16 ;
            int required_OF_Cpt_B = 17 ;
            int already_S_OF_Cpt_B = 18 ;
            int S_OF_Cpt_B = 19 ;
            int relevant_evt_OF_Cpt_B = 20 ;
            int failF_OF_Cpt_B = 21 ;
            int required_OF_Cpt_C = 22 ;
            int already_S_OF_Cpt_C = 23 ;
            int S_OF_Cpt_C = 24 ;
            int relevant_evt_OF_Cpt_C = 25 ;
            int failF_OF_Cpt_C = 26 ;
            int required_OF_OR_1 = 27 ;
            int already_S_OF_OR_1 = 28 ;
            int S_OF_OR_1 = 29 ;
            int relevant_evt_OF_OR_1 = 30 ;
            int required_OF_OR_2 = 31 ;
            int already_S_OF_OR_2 = 32 ;
            int S_OF_OR_2 = 33 ;
            int relevant_evt_OF_OR_2 = 34 ;
            int required_OF_OR_3 = 35 ;
            int already_S_OF_OR_3 = 36 ;
            int S_OF_OR_3 = 37 ;
            int relevant_evt_OF_OR_3 = 38 ;
            int required_OF_UE_1 = 39 ;
            int already_S_OF_UE_1 = 40 ;
            int S_OF_UE_1 = 41 ;
            int relevant_evt_OF_UE_1 = 42 ;
            int required_OF_phase_1 = 43 ;
            int already_S_OF_phase_1 = 44 ;
            int S_OF_phase_1 = 45 ;
            int relevant_evt_OF_phase_1 = 46 ;
            int in_progress_OF_phase_1 = 47 ;
            int already_required_OF_phase_1 = 48 ;
            int start_phase_OF_phase_1 = 49 ;
            int required_OF_phase_2 = 50 ;
            int already_S_OF_phase_2 = 51 ;
            int S_OF_phase_2 = 52 ;
            int relevant_evt_OF_phase_2 = 53 ;
            int in_progress_OF_phase_2 = 54 ;
            int already_required_OF_phase_2 = 55 ;
            int start_phase_OF_phase_2 = 56 ;




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