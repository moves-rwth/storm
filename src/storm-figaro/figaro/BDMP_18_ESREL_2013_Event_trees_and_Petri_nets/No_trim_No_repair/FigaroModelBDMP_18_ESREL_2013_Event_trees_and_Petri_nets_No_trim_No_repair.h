
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
        class FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_A_ND_by_ship" , 0},
            	{"already_S_OF_A_ND_by_ship" , 1},
            	{"S_OF_A_ND_by_ship" , 2},
            	{"relevant_evt_OF_A_ND_by_ship" , 3},
            	{"required_OF_A_ND_by_ship_1" , 4},
            	{"already_S_OF_A_ND_by_ship_1" , 5},
            	{"S_OF_A_ND_by_ship_1" , 6},
            	{"relevant_evt_OF_A_ND_by_ship_1" , 7},
            	{"required_OF_B_ND_by_platform" , 8},
            	{"already_S_OF_B_ND_by_platform" , 9},
            	{"S_OF_B_ND_by_platform" , 10},
            	{"relevant_evt_OF_B_ND_by_platform" , 11},
            	{"required_OF_UE_1" , 12},
            	{"already_S_OF_UE_1" , 13},
            	{"S_OF_UE_1" , 14},
            	{"relevant_evt_OF_UE_1" , 15},
            	{"required_OF_collision" , 16},
            	{"already_S_OF_collision" , 17},
            	{"S_OF_collision" , 18},
            	{"relevant_evt_OF_collision" , 19},
            	{"required_OF_course_not_changed" , 20},
            	{"already_S_OF_course_not_changed" , 21},
            	{"S_OF_course_not_changed" , 22},
            	{"relevant_evt_OF_course_not_changed" , 23},
            	{"failI_OF_course_not_changed" , 24},
            	{"to_be_fired_OF_course_not_changed" , 25},
            	{"already_standby_OF_course_not_changed" , 26},
            	{"already_required_OF_course_not_changed" , 27},
            	{"required_OF_non_detection" , 28},
            	{"already_S_OF_non_detection" , 29},
            	{"S_OF_non_detection" , 30},
            	{"relevant_evt_OF_non_detection" , 31},
            	{"required_OF_radar" , 32},
            	{"already_S_OF_radar" , 33},
            	{"S_OF_radar" , 34},
            	{"relevant_evt_OF_radar" , 35},
            	{"failI_OF_radar" , 36},
            	{"to_be_fired_OF_radar" , 37},
            	{"already_standby_OF_radar" , 38},
            	{"already_required_OF_radar" , 39},
            	{"required_OF_radar_1" , 40},
            	{"already_S_OF_radar_1" , 41},
            	{"S_OF_radar_1" , 42},
            	{"relevant_evt_OF_radar_1" , 43},
            	{"failI_OF_radar_1" , 44},
            	{"to_be_fired_OF_radar_1" , 45},
            	{"already_standby_OF_radar_1" , 46},
            	{"already_required_OF_radar_1" , 47},
            	{"required_OF_ship_on_collision_course" , 48},
            	{"already_S_OF_ship_on_collision_course" , 49},
            	{"S_OF_ship_on_collision_course" , 50},
            	{"relevant_evt_OF_ship_on_collision_course" , 51},
            	{"failF_OF_ship_on_collision_course" , 52},
            	{"required_OF_visual" , 53},
            	{"already_S_OF_visual" , 54},
            	{"S_OF_visual" , 55},
            	{"relevant_evt_OF_visual" , 56},
            	{"failI_OF_visual" , 57},
            	{"to_be_fired_OF_visual" , 58},
            	{"already_standby_OF_visual" , 59},
            	{"already_required_OF_visual" , 60},
            	{"required_OF_visual_1" , 61},
            	{"already_S_OF_visual_1" , 62},
            	{"S_OF_visual_1" , 63},
            	{"relevant_evt_OF_visual_1" , 64},
            	{"failI_OF_visual_1" , 65},
            	{"to_be_fired_OF_visual_1" , 66},
            	{"already_standby_OF_visual_1" , 67},
            	{"already_required_OF_visual_1" , 68},
            	{"required_OF_warning" , 69},
            	{"already_S_OF_warning" , 70},
            	{"S_OF_warning" , 71},
            	{"relevant_evt_OF_warning" , 72},
            	{"failI_OF_warning" , 73},
            	{"to_be_fired_OF_warning" , 74},
            	{"already_standby_OF_warning" , 75},
            	{"already_required_OF_warning" , 76}},

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
                    77 ,
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
            bool REINITIALISATION_OF_required_OF_A_ND_by_ship ;
            bool REINITIALISATION_OF_S_OF_A_ND_by_ship ;
            bool REINITIALISATION_OF_relevant_evt_OF_A_ND_by_ship ;
            bool REINITIALISATION_OF_required_OF_A_ND_by_ship_1 ;
            bool REINITIALISATION_OF_S_OF_A_ND_by_ship_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_A_ND_by_ship_1 ;
            bool REINITIALISATION_OF_required_OF_B_ND_by_platform ;
            bool REINITIALISATION_OF_S_OF_B_ND_by_platform ;
            bool REINITIALISATION_OF_relevant_evt_OF_B_ND_by_platform ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            bool REINITIALISATION_OF_required_OF_collision ;
            bool REINITIALISATION_OF_S_OF_collision ;
            bool REINITIALISATION_OF_relevant_evt_OF_collision ;
            bool REINITIALISATION_OF_required_OF_course_not_changed ;
            bool REINITIALISATION_OF_S_OF_course_not_changed ;
            bool REINITIALISATION_OF_relevant_evt_OF_course_not_changed ;
            bool REINITIALISATION_OF_to_be_fired_OF_course_not_changed ;
            bool REINITIALISATION_OF_required_OF_non_detection ;
            bool REINITIALISATION_OF_S_OF_non_detection ;
            bool REINITIALISATION_OF_relevant_evt_OF_non_detection ;
            bool REINITIALISATION_OF_required_OF_radar ;
            bool REINITIALISATION_OF_S_OF_radar ;
            bool REINITIALISATION_OF_relevant_evt_OF_radar ;
            bool REINITIALISATION_OF_to_be_fired_OF_radar ;
            bool REINITIALISATION_OF_required_OF_radar_1 ;
            bool REINITIALISATION_OF_S_OF_radar_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_radar_1 ;
            bool REINITIALISATION_OF_to_be_fired_OF_radar_1 ;
            bool REINITIALISATION_OF_required_OF_ship_on_collision_course ;
            bool REINITIALISATION_OF_S_OF_ship_on_collision_course ;
            bool REINITIALISATION_OF_relevant_evt_OF_ship_on_collision_course ;
            bool REINITIALISATION_OF_required_OF_visual ;
            bool REINITIALISATION_OF_S_OF_visual ;
            bool REINITIALISATION_OF_relevant_evt_OF_visual ;
            bool REINITIALISATION_OF_to_be_fired_OF_visual ;
            bool REINITIALISATION_OF_required_OF_visual_1 ;
            bool REINITIALISATION_OF_S_OF_visual_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_visual_1 ;
            bool REINITIALISATION_OF_to_be_fired_OF_visual_1 ;
            bool REINITIALISATION_OF_required_OF_warning ;
            bool REINITIALISATION_OF_S_OF_warning ;
            bool REINITIALISATION_OF_relevant_evt_OF_warning ;
            bool REINITIALISATION_OF_to_be_fired_OF_warning ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			std::string const calculate_required_OF_collision = "fn_fathers_and_trig";
			double const mu_OF_ship_on_collision_course = 0.1;
			bool const failF_FROZEN_OF_ship_on_collision_course = false;
			double const mu_OF_radar_1 = 0.1;
			bool const failI_FROZEN_OF_warning = false;
			bool const force_relevant_events_OF_visual_1 = false;
			bool const force_relevant_events_OF_course_not_changed = false;
			bool const failI_FROZEN_OF_radar = false;
			std::string const calculate_required_OF_visual_1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_course_not_changed = "fn_fathers_and_trig";
			bool const trimming_OF_OPTIONS = false;
			bool const force_relevant_events_OF_visual = false;
			double const gamma_OF_visual_1 = 0.0001;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			double const gamma_OF_course_not_changed = 0.0001;
			bool const force_relevant_events_OF_non_detection = false;
			std::string const calculate_required_OF_visual = "fn_fathers_and_trig";
			std::string const when_to_check_OF_warning = "not_req_to_req";
			bool const Default_OF___ARBRE__EIRM = true;
			bool const repairable_system_OF_OPTIONS = false;
			bool const force_relevant_events_OF_ship_on_collision_course = false;
			double const gamma_OF_visual = 0.0001;
			std::string const calculate_required_OF_UE_1 = "always_true";
			bool const force_relevant_events_OF_radar_1 = false;
			std::string const calculate_required_OF_non_detection = "fn_fathers_and_trig";
			bool const no_repair_OF___ARBRE__EIRM = true;
			std::string const calculate_required_OF_radar_1 = "fn_fathers_and_trig";
			std::string const when_to_check_OF_radar = "not_req_to_req";
			std::string const calculate_required_OF_ship_on_collision_course = "fn_fathers_and_trig";
			double const mu_OF_warning = 0.1;
			bool const force_relevant_events_OF_B_ND_by_platform = false;
			double const gamma_OF_radar_1 = 0.0001;
			bool const failI_FROZEN_OF_visual_1 = false;
			double const mu_OF_radar = 0.1;
			bool const failI_FROZEN_OF_course_not_changed = false;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			std::string const calculate_required_OF_B_ND_by_platform = "fn_fathers_and_trig";
			bool const failI_FROZEN_OF_visual = false;
			bool const no_trim_OF___ARBRE__EIRM = true;
			bool const force_relevant_events_OF_UE_1 = true;
			bool const force_relevant_events_OF_A_ND_by_ship_1 = false;
			bool const failI_FROZEN_OF_radar_1 = false;
			std::string const when_to_check_OF_course_not_changed = "not_req_to_req";
			std::string const when_to_check_OF_visual_1 = "not_req_to_req";
			bool const force_relevant_events_OF_warning = false;
			std::string const calculate_required_OF_A_ND_by_ship_1 = "fn_fathers_and_trig";
			bool const Trim_article_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_A_ND_by_ship = false;
			std::string const when_to_check_OF_visual = "not_req_to_req";
			double const mu_OF_course_not_changed = 0.1;
			double const mu_OF_visual_1 = 0.1;
			bool const force_relevant_events_OF_radar = false;
			std::string const calculate_required_OF_warning = "fn_fathers_and_trig";
			double const gamma_OF_warning = 0.0001;
			bool const force_relevant_events_OF_collision = false;
			std::string const calculate_required_OF_radar = "fn_fathers_and_trig";
			double const mu_OF_visual = 0.1;
			std::string const calculate_required_OF_A_ND_by_ship = "fn_fathers_and_trig";
			double const lambda_OF_ship_on_collision_course = 0.0001;
			bool const Profil1_OF___ARBRE__EIRM = false;
			std::string const when_to_check_OF_radar_1 = "not_req_to_req";
			double const gamma_OF_radar = 0.0001;
			std::string const trimming_option_OF_OPTIONS = "maximum";
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx23_OF_course_not_changed_INS_0;
            bool FIRE_xx23_OF_course_not_changed_INS_1;
            bool FIRE_xx23_OF_radar_INS_2;
            bool FIRE_xx23_OF_radar_INS_3;
            bool FIRE_xx23_OF_radar_1_INS_4;
            bool FIRE_xx23_OF_radar_1_INS_5;
            bool FIRE_xx10_OF_ship_on_collision_course;
            bool FIRE_xx23_OF_visual_INS_7;
            bool FIRE_xx23_OF_visual_INS_8;
            bool FIRE_xx23_OF_visual_1_INS_9;
            bool FIRE_xx23_OF_visual_1_INS_10;
            bool FIRE_xx23_OF_warning_INS_11;
            bool FIRE_xx23_OF_warning_INS_12;

            int required_OF_A_ND_by_ship = 0 ;
            int already_S_OF_A_ND_by_ship = 1 ;
            int S_OF_A_ND_by_ship = 2 ;
            int relevant_evt_OF_A_ND_by_ship = 3 ;
            int required_OF_A_ND_by_ship_1 = 4 ;
            int already_S_OF_A_ND_by_ship_1 = 5 ;
            int S_OF_A_ND_by_ship_1 = 6 ;
            int relevant_evt_OF_A_ND_by_ship_1 = 7 ;
            int required_OF_B_ND_by_platform = 8 ;
            int already_S_OF_B_ND_by_platform = 9 ;
            int S_OF_B_ND_by_platform = 10 ;
            int relevant_evt_OF_B_ND_by_platform = 11 ;
            int required_OF_UE_1 = 12 ;
            int already_S_OF_UE_1 = 13 ;
            int S_OF_UE_1 = 14 ;
            int relevant_evt_OF_UE_1 = 15 ;
            int required_OF_collision = 16 ;
            int already_S_OF_collision = 17 ;
            int S_OF_collision = 18 ;
            int relevant_evt_OF_collision = 19 ;
            int required_OF_course_not_changed = 20 ;
            int already_S_OF_course_not_changed = 21 ;
            int S_OF_course_not_changed = 22 ;
            int relevant_evt_OF_course_not_changed = 23 ;
            int failI_OF_course_not_changed = 24 ;
            int to_be_fired_OF_course_not_changed = 25 ;
            int already_standby_OF_course_not_changed = 26 ;
            int already_required_OF_course_not_changed = 27 ;
            int required_OF_non_detection = 28 ;
            int already_S_OF_non_detection = 29 ;
            int S_OF_non_detection = 30 ;
            int relevant_evt_OF_non_detection = 31 ;
            int required_OF_radar = 32 ;
            int already_S_OF_radar = 33 ;
            int S_OF_radar = 34 ;
            int relevant_evt_OF_radar = 35 ;
            int failI_OF_radar = 36 ;
            int to_be_fired_OF_radar = 37 ;
            int already_standby_OF_radar = 38 ;
            int already_required_OF_radar = 39 ;
            int required_OF_radar_1 = 40 ;
            int already_S_OF_radar_1 = 41 ;
            int S_OF_radar_1 = 42 ;
            int relevant_evt_OF_radar_1 = 43 ;
            int failI_OF_radar_1 = 44 ;
            int to_be_fired_OF_radar_1 = 45 ;
            int already_standby_OF_radar_1 = 46 ;
            int already_required_OF_radar_1 = 47 ;
            int required_OF_ship_on_collision_course = 48 ;
            int already_S_OF_ship_on_collision_course = 49 ;
            int S_OF_ship_on_collision_course = 50 ;
            int relevant_evt_OF_ship_on_collision_course = 51 ;
            int failF_OF_ship_on_collision_course = 52 ;
            int required_OF_visual = 53 ;
            int already_S_OF_visual = 54 ;
            int S_OF_visual = 55 ;
            int relevant_evt_OF_visual = 56 ;
            int failI_OF_visual = 57 ;
            int to_be_fired_OF_visual = 58 ;
            int already_standby_OF_visual = 59 ;
            int already_required_OF_visual = 60 ;
            int required_OF_visual_1 = 61 ;
            int already_S_OF_visual_1 = 62 ;
            int S_OF_visual_1 = 63 ;
            int relevant_evt_OF_visual_1 = 64 ;
            int failI_OF_visual_1 = 65 ;
            int to_be_fired_OF_visual_1 = 66 ;
            int already_standby_OF_visual_1 = 67 ;
            int already_required_OF_visual_1 = 68 ;
            int required_OF_warning = 69 ;
            int already_S_OF_warning = 70 ;
            int S_OF_warning = 71 ;
            int relevant_evt_OF_warning = 72 ;
            int failI_OF_warning = 73 ;
            int to_be_fired_OF_warning = 74 ;
            int already_standby_OF_warning = 75 ;
            int already_required_OF_warning = 76 ;




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