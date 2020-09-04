
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
        class FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Max_No_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Max_No_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_UE_1" , 0},
            	{"already_S_OF_UE_1" , 1},
            	{"S_OF_UE_1" , 2},
            	{"relevant_evt_OF_UE_1" , 3},
            	{"required_OF_collision_avoided" , 4},
            	{"already_S_OF_collision_avoided" , 5},
            	{"S_OF_collision_avoided" , 6},
            	{"relevant_evt_OF_collision_avoided" , 7},
            	{"required_OF_course_changed" , 8},
            	{"already_S_OF_course_changed" , 9},
            	{"S_OF_course_changed" , 10},
            	{"relevant_evt_OF_course_changed" , 11},
            	{"failF_OF_course_changed" , 12},
            	{"required_OF_detection" , 13},
            	{"already_S_OF_detection" , 14},
            	{"S_OF_detection" , 15},
            	{"relevant_evt_OF_detection" , 16},
            	{"required_OF_detection_by_platform" , 17},
            	{"already_S_OF_detection_by_platform" , 18},
            	{"S_OF_detection_by_platform" , 19},
            	{"relevant_evt_OF_detection_by_platform" , 20},
            	{"required_OF_detection_by_platform_1" , 21},
            	{"already_S_OF_detection_by_platform_1" , 22},
            	{"S_OF_detection_by_platform_1" , 23},
            	{"relevant_evt_OF_detection_by_platform_1" , 24},
            	{"required_OF_detection_by_ship" , 25},
            	{"already_S_OF_detection_by_ship" , 26},
            	{"S_OF_detection_by_ship" , 27},
            	{"relevant_evt_OF_detection_by_ship" , 28},
            	{"required_OF_radar" , 29},
            	{"already_S_OF_radar" , 30},
            	{"S_OF_radar" , 31},
            	{"relevant_evt_OF_radar" , 32},
            	{"failF_OF_radar" , 33},
            	{"required_OF_radar_1" , 34},
            	{"already_S_OF_radar_1" , 35},
            	{"S_OF_radar_1" , 36},
            	{"relevant_evt_OF_radar_1" , 37},
            	{"failF_OF_radar_1" , 38},
            	{"required_OF_radar_and_visual" , 39},
            	{"already_S_OF_radar_and_visual" , 40},
            	{"S_OF_radar_and_visual" , 41},
            	{"relevant_evt_OF_radar_and_visual" , 42},
            	{"in_progress_OF_radar_and_visual" , 43},
            	{"already_required_OF_radar_and_visual" , 44},
            	{"start_phase_OF_radar_and_visual" , 45},
            	{"required_OF_radar_only" , 46},
            	{"already_S_OF_radar_only" , 47},
            	{"S_OF_radar_only" , 48},
            	{"relevant_evt_OF_radar_only" , 49},
            	{"in_progress_OF_radar_only" , 50},
            	{"already_required_OF_radar_only" , 51},
            	{"start_phase_OF_radar_only" , 52},
            	{"required_OF_too_late" , 53},
            	{"already_S_OF_too_late" , 54},
            	{"S_OF_too_late" , 55},
            	{"relevant_evt_OF_too_late" , 56},
            	{"in_progress_OF_too_late" , 57},
            	{"already_required_OF_too_late" , 58},
            	{"start_phase_OF_too_late" , 59},
            	{"required_OF_visual" , 60},
            	{"already_S_OF_visual" , 61},
            	{"S_OF_visual" , 62},
            	{"relevant_evt_OF_visual" , 63},
            	{"failF_OF_visual" , 64},
            	{"required_OF_visual_1" , 65},
            	{"already_S_OF_visual_1" , 66},
            	{"S_OF_visual_1" , 67},
            	{"relevant_evt_OF_visual_1" , 68},
            	{"failF_OF_visual_1" , 69},
            	{"required_OF_warning" , 70},
            	{"already_S_OF_warning" , 71},
            	{"S_OF_warning" , 72},
            	{"relevant_evt_OF_warning" , 73},
            	{"failF_OF_warning" , 74}},

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
                    75 ,
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
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            bool REINITIALISATION_OF_required_OF_collision_avoided ;
            bool REINITIALISATION_OF_S_OF_collision_avoided ;
            bool REINITIALISATION_OF_relevant_evt_OF_collision_avoided ;
            bool REINITIALISATION_OF_required_OF_course_changed ;
            bool REINITIALISATION_OF_S_OF_course_changed ;
            bool REINITIALISATION_OF_relevant_evt_OF_course_changed ;
            bool REINITIALISATION_OF_required_OF_detection ;
            bool REINITIALISATION_OF_S_OF_detection ;
            bool REINITIALISATION_OF_relevant_evt_OF_detection ;
            bool REINITIALISATION_OF_required_OF_detection_by_platform ;
            bool REINITIALISATION_OF_S_OF_detection_by_platform ;
            bool REINITIALISATION_OF_relevant_evt_OF_detection_by_platform ;
            bool REINITIALISATION_OF_required_OF_detection_by_platform_1 ;
            bool REINITIALISATION_OF_S_OF_detection_by_platform_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_detection_by_platform_1 ;
            bool REINITIALISATION_OF_required_OF_detection_by_ship ;
            bool REINITIALISATION_OF_S_OF_detection_by_ship ;
            bool REINITIALISATION_OF_relevant_evt_OF_detection_by_ship ;
            bool REINITIALISATION_OF_required_OF_radar ;
            bool REINITIALISATION_OF_S_OF_radar ;
            bool REINITIALISATION_OF_relevant_evt_OF_radar ;
            bool REINITIALISATION_OF_required_OF_radar_1 ;
            bool REINITIALISATION_OF_S_OF_radar_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_radar_1 ;
            bool REINITIALISATION_OF_required_OF_radar_and_visual ;
            bool REINITIALISATION_OF_S_OF_radar_and_visual ;
            bool REINITIALISATION_OF_relevant_evt_OF_radar_and_visual ;
            bool REINITIALISATION_OF_required_OF_radar_only ;
            bool REINITIALISATION_OF_S_OF_radar_only ;
            bool REINITIALISATION_OF_relevant_evt_OF_radar_only ;
            bool REINITIALISATION_OF_required_OF_too_late ;
            bool REINITIALISATION_OF_S_OF_too_late ;
            bool REINITIALISATION_OF_relevant_evt_OF_too_late ;
            bool REINITIALISATION_OF_required_OF_visual ;
            bool REINITIALISATION_OF_S_OF_visual ;
            bool REINITIALISATION_OF_relevant_evt_OF_visual ;
            bool REINITIALISATION_OF_required_OF_visual_1 ;
            bool REINITIALISATION_OF_S_OF_visual_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_visual_1 ;
            bool REINITIALISATION_OF_required_OF_warning ;
            bool REINITIALISATION_OF_S_OF_warning ;
            bool REINITIALISATION_OF_relevant_evt_OF_warning ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			bool const failF_FROZEN_OF_radar_1 = false;
			bool const force_relevant_events_OF_detection_by_platform_1 = false;
			bool const repairable_system_OF_OPTIONS = true;
			double const mu_OF_radar_1 = 0.1;
			double const lambda_OF_course_changed = 0.0001;
			bool const force_relevant_events_OF_detection_by_ship = false;
			std::string const trigger_kind_OF_t_4 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_detection_by_platform_1 = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_8 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_radar_only = false;
			std::string const calculate_required_OF_radar_only = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_visual_1 = false;
			std::string const calculate_required_OF_detection_by_ship = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_course_changed = false;
			double const mu_OF_course_changed = 0.1;
			std::string const calculate_required_OF_visual_1 = "fn_fathers_and_trig";
			bool const no_trim_OF___ARBRE__EIRM = false;
			std::string const trigger_kind_OF_t_3 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_visual = false;
			std::string const Dist_kind_OF_radar_and_visual = "exponential";
			std::string const trigger_kind_OF_t_7 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_visual = "fn_fathers_and_trig";
			std::string const Dist_kind_OF_too_late = "exponential";
			double const lambda_OF_warning = 0.0001;
			bool const Default_OF___ARBRE__EIRM = true;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_collision_avoided = false;
			double const duration_OF_radar_only = 10;
			std::string const calculate_required_OF_UE_1 = "always_true";
			double const lambda_OF_radar = 0.0001;
			bool const force_relevant_events_OF_radar_1 = false;
			std::string const calculate_required_OF_radar_1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_collision_avoided = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_radar_and_visual = false;
			double const mu_OF_warning = 0.1;
			bool const failF_FROZEN_OF_warning = false;
			bool const force_relevant_events_OF_course_changed = false;
			bool const force_relevant_events_OF_too_late = false;
			bool const failF_FROZEN_OF_radar = false;
			bool const force_relevant_events_OF_detection_by_platform = false;
			double const mu_OF_radar = 0.1;
			std::string const calculate_required_OF_radar_and_visual = "fn_fathers_and_trig";
			bool const to_be_taken_into_account_OF_UE_1 = true;
			std::string const calculate_required_OF_course_changed = "fn_fathers_and_trig";
			std::string const calculate_required_OF_too_late = "fn_fathers_and_trig";
			std::string const calculate_required_OF_detection_by_platform = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_detection = false;
			double const duration_OF_radar_and_visual = 10;
			double const lambda_OF_visual_1 = 0.0001;
			std::string const trigger_kind_OF_t_5 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_UE_1 = true;
			double const duration_OF_too_late = 10;
			bool const trimming_OF_OPTIONS = true;
			bool const force_relevant_events_OF_warning = false;
			std::string const calculate_required_OF_detection = "fn_fathers_and_trig";
			double const lambda_OF_visual = 0.0001;
			bool const no_repair_OF___ARBRE__EIRM = false;
			bool const Trim_article_OF___ARBRE__EIRM = false;
			bool const failF_FROZEN_OF_visual_1 = false;
			double const mu_OF_visual_1 = 0.1;
			std::string const calculate_required_OF_warning = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_radar = false;
			std::string const calculate_required_OF_radar = "fn_fathers_and_trig";
			double const lambda_OF_radar_1 = 0.0001;
			std::string const Dist_kind_OF_radar_only = "exponential";
			std::string const trigger_kind_OF_t_9 = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_6 = "fn_fathers_and_trig";
			double const mu_OF_visual = 0.1;
			bool const failF_FROZEN_OF_visual = false;
			bool const Profil1_OF___ARBRE__EIRM = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_course_changed;
            bool FIRE_xx11_OF_course_changed;
            bool FIRE_xx10_OF_radar;
            bool FIRE_xx11_OF_radar;
            bool FIRE_xx10_OF_radar_1;
            bool FIRE_xx11_OF_radar_1;
            bool FIRE_xx43_a_OF_radar_and_visual;
            bool FIRE_xx47_OF_radar_and_visual_INS_7;
            bool FIRE_xx43_a_OF_radar_only;
            bool FIRE_xx47_OF_radar_only_INS_9;
            bool FIRE_xx43_a_OF_too_late;
            bool FIRE_xx47_OF_too_late_INS_11;
            bool FIRE_xx10_OF_visual;
            bool FIRE_xx11_OF_visual;
            bool FIRE_xx10_OF_visual_1;
            bool FIRE_xx11_OF_visual_1;
            bool FIRE_xx10_OF_warning;
            bool FIRE_xx11_OF_warning;

            int required_OF_UE_1 = 0 ;
            int already_S_OF_UE_1 = 1 ;
            int S_OF_UE_1 = 2 ;
            int relevant_evt_OF_UE_1 = 3 ;
            int required_OF_collision_avoided = 4 ;
            int already_S_OF_collision_avoided = 5 ;
            int S_OF_collision_avoided = 6 ;
            int relevant_evt_OF_collision_avoided = 7 ;
            int required_OF_course_changed = 8 ;
            int already_S_OF_course_changed = 9 ;
            int S_OF_course_changed = 10 ;
            int relevant_evt_OF_course_changed = 11 ;
            int failF_OF_course_changed = 12 ;
            int required_OF_detection = 13 ;
            int already_S_OF_detection = 14 ;
            int S_OF_detection = 15 ;
            int relevant_evt_OF_detection = 16 ;
            int required_OF_detection_by_platform = 17 ;
            int already_S_OF_detection_by_platform = 18 ;
            int S_OF_detection_by_platform = 19 ;
            int relevant_evt_OF_detection_by_platform = 20 ;
            int required_OF_detection_by_platform_1 = 21 ;
            int already_S_OF_detection_by_platform_1 = 22 ;
            int S_OF_detection_by_platform_1 = 23 ;
            int relevant_evt_OF_detection_by_platform_1 = 24 ;
            int required_OF_detection_by_ship = 25 ;
            int already_S_OF_detection_by_ship = 26 ;
            int S_OF_detection_by_ship = 27 ;
            int relevant_evt_OF_detection_by_ship = 28 ;
            int required_OF_radar = 29 ;
            int already_S_OF_radar = 30 ;
            int S_OF_radar = 31 ;
            int relevant_evt_OF_radar = 32 ;
            int failF_OF_radar = 33 ;
            int required_OF_radar_1 = 34 ;
            int already_S_OF_radar_1 = 35 ;
            int S_OF_radar_1 = 36 ;
            int relevant_evt_OF_radar_1 = 37 ;
            int failF_OF_radar_1 = 38 ;
            int required_OF_radar_and_visual = 39 ;
            int already_S_OF_radar_and_visual = 40 ;
            int S_OF_radar_and_visual = 41 ;
            int relevant_evt_OF_radar_and_visual = 42 ;
            int in_progress_OF_radar_and_visual = 43 ;
            int already_required_OF_radar_and_visual = 44 ;
            int start_phase_OF_radar_and_visual = 45 ;
            int required_OF_radar_only = 46 ;
            int already_S_OF_radar_only = 47 ;
            int S_OF_radar_only = 48 ;
            int relevant_evt_OF_radar_only = 49 ;
            int in_progress_OF_radar_only = 50 ;
            int already_required_OF_radar_only = 51 ;
            int start_phase_OF_radar_only = 52 ;
            int required_OF_too_late = 53 ;
            int already_S_OF_too_late = 54 ;
            int S_OF_too_late = 55 ;
            int relevant_evt_OF_too_late = 56 ;
            int in_progress_OF_too_late = 57 ;
            int already_required_OF_too_late = 58 ;
            int start_phase_OF_too_late = 59 ;
            int required_OF_visual = 60 ;
            int already_S_OF_visual = 61 ;
            int S_OF_visual = 62 ;
            int relevant_evt_OF_visual = 63 ;
            int failF_OF_visual = 64 ;
            int required_OF_visual_1 = 65 ;
            int already_S_OF_visual_1 = 66 ;
            int S_OF_visual_1 = 67 ;
            int relevant_evt_OF_visual_1 = 68 ;
            int failF_OF_visual_1 = 69 ;
            int required_OF_warning = 70 ;
            int already_S_OF_warning = 71 ;
            int S_OF_warning = 72 ;
            int relevant_evt_OF_warning = 73 ;
            int failF_OF_warning = 74 ;




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