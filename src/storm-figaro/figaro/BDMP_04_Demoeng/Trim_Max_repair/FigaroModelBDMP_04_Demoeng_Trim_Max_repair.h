
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
        class FigaroProgram_BDMP_04_Demoeng_Trim_Max_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_04_Demoeng_Trim_Max_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_Busbar_not_powered" , 0},
            	{"already_S_OF_Busbar_not_powered" , 1},
            	{"S_OF_Busbar_not_powered" , 2},
            	{"relevant_evt_OF_Busbar_not_powered" , 3},
            	{"required_OF_CB_dies" , 4},
            	{"already_S_OF_CB_dies" , 5},
            	{"S_OF_CB_dies" , 6},
            	{"relevant_evt_OF_CB_dies" , 7},
            	{"failF_OF_CB_dies" , 8},
            	{"required_OF_CB_dw_1" , 9},
            	{"already_S_OF_CB_dw_1" , 10},
            	{"S_OF_CB_dw_1" , 11},
            	{"relevant_evt_OF_CB_dw_1" , 12},
            	{"failF_OF_CB_dw_1" , 13},
            	{"required_OF_CB_dw_2" , 14},
            	{"already_S_OF_CB_dw_2" , 15},
            	{"S_OF_CB_dw_2" , 16},
            	{"relevant_evt_OF_CB_dw_2" , 17},
            	{"failF_OF_CB_dw_2" , 18},
            	{"required_OF_CB_up_1" , 19},
            	{"already_S_OF_CB_up_1" , 20},
            	{"S_OF_CB_up_1" , 21},
            	{"relevant_evt_OF_CB_up_1" , 22},
            	{"failF_OF_CB_up_1" , 23},
            	{"required_OF_CB_up_2" , 24},
            	{"already_S_OF_CB_up_2" , 25},
            	{"S_OF_CB_up_2" , 26},
            	{"relevant_evt_OF_CB_up_2" , 27},
            	{"failF_OF_CB_up_2" , 28},
            	{"required_OF_GRID" , 29},
            	{"already_S_OF_GRID" , 30},
            	{"S_OF_GRID" , 31},
            	{"relevant_evt_OF_GRID" , 32},
            	{"failF_OF_GRID" , 33},
            	{"required_OF_LossOfAllBackups" , 34},
            	{"already_S_OF_LossOfAllBackups" , 35},
            	{"S_OF_LossOfAllBackups" , 36},
            	{"relevant_evt_OF_LossOfAllBackups" , 37},
            	{"required_OF_LossOfDieselLine" , 38},
            	{"already_S_OF_LossOfDieselLine" , 39},
            	{"S_OF_LossOfDieselLine" , 40},
            	{"relevant_evt_OF_LossOfDieselLine" , 41},
            	{"required_OF_LossOfLine2" , 42},
            	{"already_S_OF_LossOfLine2" , 43},
            	{"S_OF_LossOfLine2" , 44},
            	{"relevant_evt_OF_LossOfLine2" , 45},
            	{"required_OF_LossOfLine_1" , 46},
            	{"already_S_OF_LossOfLine_1" , 47},
            	{"S_OF_LossOfLine_1" , 48},
            	{"relevant_evt_OF_LossOfLine_1" , 49},
            	{"required_OF_RC_CB_dies" , 50},
            	{"already_S_OF_RC_CB_dies" , 51},
            	{"S_OF_RC_CB_dies" , 52},
            	{"relevant_evt_OF_RC_CB_dies" , 53},
            	{"failI_OF_RC_CB_dies" , 54},
            	{"to_be_fired_OF_RC_CB_dies" , 55},
            	{"already_standby_OF_RC_CB_dies" , 56},
            	{"already_required_OF_RC_CB_dies" , 57},
            	{"required_OF_RC_CB_dw_2" , 58},
            	{"already_S_OF_RC_CB_dw_2" , 59},
            	{"S_OF_RC_CB_dw_2" , 60},
            	{"relevant_evt_OF_RC_CB_dw_2" , 61},
            	{"failI_OF_RC_CB_dw_2" , 62},
            	{"to_be_fired_OF_RC_CB_dw_2" , 63},
            	{"already_standby_OF_RC_CB_dw_2" , 64},
            	{"already_required_OF_RC_CB_dw_2" , 65},
            	{"required_OF_RC_CB_up_2" , 66},
            	{"already_S_OF_RC_CB_up_2" , 67},
            	{"S_OF_RC_CB_up_2" , 68},
            	{"relevant_evt_OF_RC_CB_up_2" , 69},
            	{"failI_OF_RC_CB_up_2" , 70},
            	{"to_be_fired_OF_RC_CB_up_2" , 71},
            	{"already_standby_OF_RC_CB_up_2" , 72},
            	{"already_required_OF_RC_CB_up_2" , 73},
            	{"required_OF_RS_dies" , 74},
            	{"already_S_OF_RS_dies" , 75},
            	{"S_OF_RS_dies" , 76},
            	{"relevant_evt_OF_RS_dies" , 77},
            	{"failI_OF_RS_dies" , 78},
            	{"to_be_fired_OF_RS_dies" , 79},
            	{"already_standby_OF_RS_dies" , 80},
            	{"already_required_OF_RS_dies" , 81},
            	{"required_OF_Transfo1" , 82},
            	{"already_S_OF_Transfo1" , 83},
            	{"S_OF_Transfo1" , 84},
            	{"relevant_evt_OF_Transfo1" , 85},
            	{"failF_OF_Transfo1" , 86},
            	{"required_OF_Transfo2" , 87},
            	{"already_S_OF_Transfo2" , 88},
            	{"S_OF_Transfo2" , 89},
            	{"relevant_evt_OF_Transfo2" , 90},
            	{"failF_OF_Transfo2" , 91},
            	{"required_OF_UE_1" , 92},
            	{"already_S_OF_UE_1" , 93},
            	{"S_OF_UE_1" , 94},
            	{"relevant_evt_OF_UE_1" , 95},
            	{"required_OF_dies_generator" , 96},
            	{"already_S_OF_dies_generator" , 97},
            	{"S_OF_dies_generator" , 98},
            	{"relevant_evt_OF_dies_generator" , 99},
            	{"failF_OF_dies_generator" , 100}},

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
                    101 ,
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
            bool REINITIALISATION_OF_required_OF_Busbar_not_powered ;
            bool REINITIALISATION_OF_S_OF_Busbar_not_powered ;
            bool REINITIALISATION_OF_relevant_evt_OF_Busbar_not_powered ;
            bool REINITIALISATION_OF_required_OF_CB_dies ;
            bool REINITIALISATION_OF_S_OF_CB_dies ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB_dies ;
            bool REINITIALISATION_OF_required_OF_CB_dw_1 ;
            bool REINITIALISATION_OF_S_OF_CB_dw_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB_dw_1 ;
            bool REINITIALISATION_OF_required_OF_CB_dw_2 ;
            bool REINITIALISATION_OF_S_OF_CB_dw_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB_dw_2 ;
            bool REINITIALISATION_OF_required_OF_CB_up_1 ;
            bool REINITIALISATION_OF_S_OF_CB_up_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB_up_1 ;
            bool REINITIALISATION_OF_required_OF_CB_up_2 ;
            bool REINITIALISATION_OF_S_OF_CB_up_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB_up_2 ;
            bool REINITIALISATION_OF_required_OF_GRID ;
            bool REINITIALISATION_OF_S_OF_GRID ;
            bool REINITIALISATION_OF_relevant_evt_OF_GRID ;
            bool REINITIALISATION_OF_required_OF_LossOfAllBackups ;
            bool REINITIALISATION_OF_S_OF_LossOfAllBackups ;
            bool REINITIALISATION_OF_relevant_evt_OF_LossOfAllBackups ;
            bool REINITIALISATION_OF_required_OF_LossOfDieselLine ;
            bool REINITIALISATION_OF_S_OF_LossOfDieselLine ;
            bool REINITIALISATION_OF_relevant_evt_OF_LossOfDieselLine ;
            bool REINITIALISATION_OF_required_OF_LossOfLine2 ;
            bool REINITIALISATION_OF_S_OF_LossOfLine2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_LossOfLine2 ;
            bool REINITIALISATION_OF_required_OF_LossOfLine_1 ;
            bool REINITIALISATION_OF_S_OF_LossOfLine_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_LossOfLine_1 ;
            bool REINITIALISATION_OF_required_OF_RC_CB_dies ;
            bool REINITIALISATION_OF_S_OF_RC_CB_dies ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_dies ;
            bool REINITIALISATION_OF_to_be_fired_OF_RC_CB_dies ;
            bool REINITIALISATION_OF_required_OF_RC_CB_dw_2 ;
            bool REINITIALISATION_OF_S_OF_RC_CB_dw_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_dw_2 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RC_CB_dw_2 ;
            bool REINITIALISATION_OF_required_OF_RC_CB_up_2 ;
            bool REINITIALISATION_OF_S_OF_RC_CB_up_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_CB_up_2 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RC_CB_up_2 ;
            bool REINITIALISATION_OF_required_OF_RS_dies ;
            bool REINITIALISATION_OF_S_OF_RS_dies ;
            bool REINITIALISATION_OF_relevant_evt_OF_RS_dies ;
            bool REINITIALISATION_OF_to_be_fired_OF_RS_dies ;
            bool REINITIALISATION_OF_required_OF_Transfo1 ;
            bool REINITIALISATION_OF_S_OF_Transfo1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_Transfo1 ;
            bool REINITIALISATION_OF_required_OF_Transfo2 ;
            bool REINITIALISATION_OF_S_OF_Transfo2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_Transfo2 ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            bool REINITIALISATION_OF_required_OF_dies_generator ;
            bool REINITIALISATION_OF_S_OF_dies_generator ;
            bool REINITIALISATION_OF_relevant_evt_OF_dies_generator ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			std::string const calculate_required_OF_LossOfAllBackups = "fn_fathers_and_trig";
			std::string const calculate_required_OF_CB_dies = "fn_fathers_and_trig";
			double const mu_OF_CB_up_2 = 0.1;
			bool const failF_FROZEN_OF_CB_up_1 = false;
			bool const No_repair_OF___ARBRE__EIRM = false;
			bool const failI_FROZEN_OF_RC_CB_up_2 = false;
			bool const force_relevant_events_OF_LossOfLine2 = false;
			double const lambda_OF_Transfo2 = 0.0001;
			bool const repairable_system_OF_OPTIONS = true;
			bool const No_trim_OF___ARBRE__EIRM = false;
			bool const Profil1_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_LossOfDieselLine = "fn_fathers_and_trig";
			bool const to_be_taken_into_account_OF_UE_1 = true;
			std::string const calculate_required_OF_RC_CB_dies = "fn_fathers_and_trig";
			std::string const calculate_required_OF_RC_CB_dw_2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_CB_up_2 = false;
			bool const force_relevant_events_OF_UE_1 = true;
			bool const force_relevant_events_OF_CB_dw_1 = false;
			double const lambda_OF_CB_dies = 0.0001;
			bool const force_relevant_events_OF_GRID = false;
			double const mu_OF_CB_dw_2 = 0.1;
			double const mu_OF_GRID = 0.1;
			bool const failI_FROZEN_OF_RC_CB_dw_2 = false;
			std::string const calculate_required_OF_CB_up_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_Transfo1 = false;
			bool const force_relevant_events_OF_RS_dies = false;
			bool const failF_FROZEN_OF_CB_dw_1 = false;
			bool const failF_FROZEN_OF_CB_dw_2 = false;
			bool const failF_FROZEN_OF_GRID = false;
			double const mu_OF_RS_dies = 0.1;
			std::string const calculate_required_OF_dies_generator = "fn_fathers_and_trig";
			std::string const calculate_required_OF_LossOfLine_1 = "fn_fathers_and_trig";
			double const gamma_OF_RS_dies = 0.0001;
			bool const force_relevant_events_OF_Transfo2 = false;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_RC_CB_up_2 = false;
			std::string const calculate_required_OF_CB_up_1 = "fn_fathers_and_trig";
			double const lambda_OF_CB_up_2 = 0.0001;
			double const gamma_OF_RC_CB_up_2 = 0.0001;
			bool const failF_FROZEN_OF_Transfo1 = false;
			bool const Trim_article_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_CB_dw_2 = "fn_fathers_and_trig";
			double const mu_OF_CB_dw_1 = 0.1;
			bool const force_relevant_events_OF_LossOfAllBackups = false;
			bool const force_relevant_events_OF_CB_dies = false;
			bool const failI_FROZEN_OF_RC_CB_dies = false;
			double const lambda_OF_dies_generator = 0.0001;
			bool const failF_FROZEN_OF_Transfo2 = false;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const trimming_OF_OPTIONS = true;
			double const lambda_OF_CB_up_1 = 0.0001;
			bool const force_relevant_events_OF_LossOfDieselLine = false;
			double const mu_OF_Transfo1 = 0.1;
			bool const force_relevant_events_OF_RC_CB_dies = false;
			bool const force_relevant_events_OF_RC_CB_dw_2 = false;
			bool const default_OF___ARBRE__EIRM = true;
			std::string const calculate_required_OF_LossOfLine2 = "fn_fathers_and_trig";
			double const lambda_OF_CB_dw_2 = 0.0001;
			double const gamma_OF_RC_CB_dies = 0.0001;
			double const gamma_OF_RC_CB_dw_2 = 0.0001;
			double const mu_OF_Transfo2 = 0.1;
			std::string const calculate_required_OF_Busbar_not_powered = "fn_fathers_and_trig";
			double const mu_OF_RC_CB_up_2 = 0.1;
			std::string const calculate_required_OF_CB_dw_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_CB_up_2 = false;
			std::string const calculate_required_OF_GRID = "fn_fathers_and_trig";
			std::string const when_to_check_OF_RS_dies = "not_req_to_req";
			double const mu_OF_CB_dies = 0.1;
			std::string const when_to_check_OF_RC_CB_up_2 = "not_req_to_req";
			std::string const calculate_required_OF_Transfo1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_dies_generator = false;
			bool const force_relevant_events_OF_LossOfLine_1 = false;
			std::string const calculate_required_OF_RS_dies = "fn_fathers_and_trig";
			double const lambda_OF_CB_dw_1 = 0.0001;
			double const mu_OF_dies_generator = 0.1;
			bool const force_relevant_events_OF_CB_up_1 = false;
			double const mu_OF_RC_CB_dies = 0.1;
			double const mu_OF_RC_CB_dw_2 = 0.1;
			bool const force_relevant_events_OF_Busbar_not_powered = false;
			double const lambda_OF_GRID = 0.0001;
			double const mu_OF_CB_up_1 = 0.1;
			std::string const calculate_required_OF_Transfo2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_CB_dies = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
			bool const force_relevant_events_OF_CB_dw_2 = false;
			std::string const calculate_required_OF_RC_CB_up_2 = "fn_fathers_and_trig";
			double const lambda_OF_Transfo1 = 0.0001;
			std::string const when_to_check_OF_RC_CB_dies = "not_req_to_req";
			std::string const when_to_check_OF_RC_CB_dw_2 = "not_req_to_req";
			bool const failI_FROZEN_OF_RS_dies = false;
			bool const failF_FROZEN_OF_dies_generator = false;
			std::string const calculate_required_OF_UE_1 = "always_true";
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_CB_dies;
            bool FIRE_xx11_OF_CB_dies;
            bool FIRE_xx10_OF_CB_dw_1;
            bool FIRE_xx11_OF_CB_dw_1;
            bool FIRE_xx10_OF_CB_dw_2;
            bool FIRE_xx11_OF_CB_dw_2;
            bool FIRE_xx10_OF_CB_up_1;
            bool FIRE_xx11_OF_CB_up_1;
            bool FIRE_xx10_OF_CB_up_2;
            bool FIRE_xx11_OF_CB_up_2;
            bool FIRE_xx10_OF_GRID;
            bool FIRE_xx11_OF_GRID;
            bool FIRE_xx23_OF_RC_CB_dies_INS_12;
            bool FIRE_xx23_OF_RC_CB_dies_INS_13;
            bool FIRE_xx24_OF_RC_CB_dies;
            bool FIRE_xx23_OF_RC_CB_dw_2_INS_15;
            bool FIRE_xx23_OF_RC_CB_dw_2_INS_16;
            bool FIRE_xx24_OF_RC_CB_dw_2;
            bool FIRE_xx23_OF_RC_CB_up_2_INS_18;
            bool FIRE_xx23_OF_RC_CB_up_2_INS_19;
            bool FIRE_xx24_OF_RC_CB_up_2;
            bool FIRE_xx23_OF_RS_dies_INS_21;
            bool FIRE_xx23_OF_RS_dies_INS_22;
            bool FIRE_xx24_OF_RS_dies;
            bool FIRE_xx10_OF_Transfo1;
            bool FIRE_xx11_OF_Transfo1;
            bool FIRE_xx10_OF_Transfo2;
            bool FIRE_xx11_OF_Transfo2;
            bool FIRE_xx10_OF_dies_generator;
            bool FIRE_xx11_OF_dies_generator;

            int required_OF_Busbar_not_powered = 0 ;
            int already_S_OF_Busbar_not_powered = 1 ;
            int S_OF_Busbar_not_powered = 2 ;
            int relevant_evt_OF_Busbar_not_powered = 3 ;
            int required_OF_CB_dies = 4 ;
            int already_S_OF_CB_dies = 5 ;
            int S_OF_CB_dies = 6 ;
            int relevant_evt_OF_CB_dies = 7 ;
            int failF_OF_CB_dies = 8 ;
            int required_OF_CB_dw_1 = 9 ;
            int already_S_OF_CB_dw_1 = 10 ;
            int S_OF_CB_dw_1 = 11 ;
            int relevant_evt_OF_CB_dw_1 = 12 ;
            int failF_OF_CB_dw_1 = 13 ;
            int required_OF_CB_dw_2 = 14 ;
            int already_S_OF_CB_dw_2 = 15 ;
            int S_OF_CB_dw_2 = 16 ;
            int relevant_evt_OF_CB_dw_2 = 17 ;
            int failF_OF_CB_dw_2 = 18 ;
            int required_OF_CB_up_1 = 19 ;
            int already_S_OF_CB_up_1 = 20 ;
            int S_OF_CB_up_1 = 21 ;
            int relevant_evt_OF_CB_up_1 = 22 ;
            int failF_OF_CB_up_1 = 23 ;
            int required_OF_CB_up_2 = 24 ;
            int already_S_OF_CB_up_2 = 25 ;
            int S_OF_CB_up_2 = 26 ;
            int relevant_evt_OF_CB_up_2 = 27 ;
            int failF_OF_CB_up_2 = 28 ;
            int required_OF_GRID = 29 ;
            int already_S_OF_GRID = 30 ;
            int S_OF_GRID = 31 ;
            int relevant_evt_OF_GRID = 32 ;
            int failF_OF_GRID = 33 ;
            int required_OF_LossOfAllBackups = 34 ;
            int already_S_OF_LossOfAllBackups = 35 ;
            int S_OF_LossOfAllBackups = 36 ;
            int relevant_evt_OF_LossOfAllBackups = 37 ;
            int required_OF_LossOfDieselLine = 38 ;
            int already_S_OF_LossOfDieselLine = 39 ;
            int S_OF_LossOfDieselLine = 40 ;
            int relevant_evt_OF_LossOfDieselLine = 41 ;
            int required_OF_LossOfLine2 = 42 ;
            int already_S_OF_LossOfLine2 = 43 ;
            int S_OF_LossOfLine2 = 44 ;
            int relevant_evt_OF_LossOfLine2 = 45 ;
            int required_OF_LossOfLine_1 = 46 ;
            int already_S_OF_LossOfLine_1 = 47 ;
            int S_OF_LossOfLine_1 = 48 ;
            int relevant_evt_OF_LossOfLine_1 = 49 ;
            int required_OF_RC_CB_dies = 50 ;
            int already_S_OF_RC_CB_dies = 51 ;
            int S_OF_RC_CB_dies = 52 ;
            int relevant_evt_OF_RC_CB_dies = 53 ;
            int failI_OF_RC_CB_dies = 54 ;
            int to_be_fired_OF_RC_CB_dies = 55 ;
            int already_standby_OF_RC_CB_dies = 56 ;
            int already_required_OF_RC_CB_dies = 57 ;
            int required_OF_RC_CB_dw_2 = 58 ;
            int already_S_OF_RC_CB_dw_2 = 59 ;
            int S_OF_RC_CB_dw_2 = 60 ;
            int relevant_evt_OF_RC_CB_dw_2 = 61 ;
            int failI_OF_RC_CB_dw_2 = 62 ;
            int to_be_fired_OF_RC_CB_dw_2 = 63 ;
            int already_standby_OF_RC_CB_dw_2 = 64 ;
            int already_required_OF_RC_CB_dw_2 = 65 ;
            int required_OF_RC_CB_up_2 = 66 ;
            int already_S_OF_RC_CB_up_2 = 67 ;
            int S_OF_RC_CB_up_2 = 68 ;
            int relevant_evt_OF_RC_CB_up_2 = 69 ;
            int failI_OF_RC_CB_up_2 = 70 ;
            int to_be_fired_OF_RC_CB_up_2 = 71 ;
            int already_standby_OF_RC_CB_up_2 = 72 ;
            int already_required_OF_RC_CB_up_2 = 73 ;
            int required_OF_RS_dies = 74 ;
            int already_S_OF_RS_dies = 75 ;
            int S_OF_RS_dies = 76 ;
            int relevant_evt_OF_RS_dies = 77 ;
            int failI_OF_RS_dies = 78 ;
            int to_be_fired_OF_RS_dies = 79 ;
            int already_standby_OF_RS_dies = 80 ;
            int already_required_OF_RS_dies = 81 ;
            int required_OF_Transfo1 = 82 ;
            int already_S_OF_Transfo1 = 83 ;
            int S_OF_Transfo1 = 84 ;
            int relevant_evt_OF_Transfo1 = 85 ;
            int failF_OF_Transfo1 = 86 ;
            int required_OF_Transfo2 = 87 ;
            int already_S_OF_Transfo2 = 88 ;
            int S_OF_Transfo2 = 89 ;
            int relevant_evt_OF_Transfo2 = 90 ;
            int failF_OF_Transfo2 = 91 ;
            int required_OF_UE_1 = 92 ;
            int already_S_OF_UE_1 = 93 ;
            int S_OF_UE_1 = 94 ;
            int relevant_evt_OF_UE_1 = 95 ;
            int required_OF_dies_generator = 96 ;
            int already_S_OF_dies_generator = 97 ;
            int S_OF_dies_generator = 98 ;
            int relevant_evt_OF_dies_generator = 99 ;
            int failF_OF_dies_generator = 100 ;




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