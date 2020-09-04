
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
        class FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_No_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_No_trim_No_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_AND_1" , 0},
            	{"already_S_OF_AND_1" , 1},
            	{"S_OF_AND_1" , 2},
            	{"relevant_evt_OF_AND_1" , 3},
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
            	{"required_OF_Loss_of_GRID" , 50},
            	{"already_S_OF_Loss_of_GRID" , 51},
            	{"S_OF_Loss_of_GRID" , 52},
            	{"relevant_evt_OF_Loss_of_GRID" , 53},
            	{"required_OF_PropagationShortCircuitTransfo1" , 54},
            	{"already_S_OF_PropagationShortCircuitTransfo1" , 55},
            	{"S_OF_PropagationShortCircuitTransfo1" , 56},
            	{"relevant_evt_OF_PropagationShortCircuitTransfo1" , 57},
            	{"required_OF_PropagationShortCircuitTransfo2" , 58},
            	{"already_S_OF_PropagationShortCircuitTransfo2" , 59},
            	{"S_OF_PropagationShortCircuitTransfo2" , 60},
            	{"relevant_evt_OF_PropagationShortCircuitTransfo2" , 61},
            	{"required_OF_RC_CB_dies" , 62},
            	{"already_S_OF_RC_CB_dies" , 63},
            	{"S_OF_RC_CB_dies" , 64},
            	{"relevant_evt_OF_RC_CB_dies" , 65},
            	{"failI_OF_RC_CB_dies" , 66},
            	{"to_be_fired_OF_RC_CB_dies" , 67},
            	{"already_standby_OF_RC_CB_dies" , 68},
            	{"already_required_OF_RC_CB_dies" , 69},
            	{"required_OF_RC_CB_dw_2" , 70},
            	{"already_S_OF_RC_CB_dw_2" , 71},
            	{"S_OF_RC_CB_dw_2" , 72},
            	{"relevant_evt_OF_RC_CB_dw_2" , 73},
            	{"failI_OF_RC_CB_dw_2" , 74},
            	{"to_be_fired_OF_RC_CB_dw_2" , 75},
            	{"already_standby_OF_RC_CB_dw_2" , 76},
            	{"already_required_OF_RC_CB_dw_2" , 77},
            	{"required_OF_RC_CB_up_2" , 78},
            	{"already_S_OF_RC_CB_up_2" , 79},
            	{"S_OF_RC_CB_up_2" , 80},
            	{"relevant_evt_OF_RC_CB_up_2" , 81},
            	{"failI_OF_RC_CB_up_2" , 82},
            	{"to_be_fired_OF_RC_CB_up_2" , 83},
            	{"already_standby_OF_RC_CB_up_2" , 84},
            	{"already_required_OF_RC_CB_up_2" , 85},
            	{"required_OF_RO_CB_up_1" , 86},
            	{"already_S_OF_RO_CB_up_1" , 87},
            	{"S_OF_RO_CB_up_1" , 88},
            	{"relevant_evt_OF_RO_CB_up_1" , 89},
            	{"failI_OF_RO_CB_up_1" , 90},
            	{"to_be_fired_OF_RO_CB_up_1" , 91},
            	{"already_standby_OF_RO_CB_up_1" , 92},
            	{"already_required_OF_RO_CB_up_1" , 93},
            	{"required_OF_RO_CB_up_2" , 94},
            	{"already_S_OF_RO_CB_up_2" , 95},
            	{"S_OF_RO_CB_up_2" , 96},
            	{"relevant_evt_OF_RO_CB_up_2" , 97},
            	{"failI_OF_RO_CB_up_2" , 98},
            	{"to_be_fired_OF_RO_CB_up_2" , 99},
            	{"already_standby_OF_RO_CB_up_2" , 100},
            	{"already_required_OF_RO_CB_up_2" , 101},
            	{"required_OF_RS_dies" , 102},
            	{"already_S_OF_RS_dies" , 103},
            	{"S_OF_RS_dies" , 104},
            	{"relevant_evt_OF_RS_dies" , 105},
            	{"failI_OF_RS_dies" , 106},
            	{"to_be_fired_OF_RS_dies" , 107},
            	{"already_standby_OF_RS_dies" , 108},
            	{"already_required_OF_RS_dies" , 109},
            	{"required_OF_Transfo1" , 110},
            	{"already_S_OF_Transfo1" , 111},
            	{"S_OF_Transfo1" , 112},
            	{"relevant_evt_OF_Transfo1" , 113},
            	{"failF_OF_Transfo1" , 114},
            	{"required_OF_Transfo2" , 115},
            	{"already_S_OF_Transfo2" , 116},
            	{"S_OF_Transfo2" , 117},
            	{"relevant_evt_OF_Transfo2" , 118},
            	{"failF_OF_Transfo2" , 119},
            	{"required_OF_UE_1" , 120},
            	{"already_S_OF_UE_1" , 121},
            	{"S_OF_UE_1" , 122},
            	{"relevant_evt_OF_UE_1" , 123},
            	{"required_OF_dies_generator" , 124},
            	{"already_S_OF_dies_generator" , 125},
            	{"S_OF_dies_generator" , 126},
            	{"relevant_evt_OF_dies_generator" , 127},
            	{"failF_OF_dies_generator" , 128}},

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
                    129 ,
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
            bool REINITIALISATION_OF_required_OF_Loss_of_GRID ;
            bool REINITIALISATION_OF_S_OF_Loss_of_GRID ;
            bool REINITIALISATION_OF_relevant_evt_OF_Loss_of_GRID ;
            bool REINITIALISATION_OF_required_OF_PropagationShortCircuitTransfo1 ;
            bool REINITIALISATION_OF_S_OF_PropagationShortCircuitTransfo1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_PropagationShortCircuitTransfo1 ;
            bool REINITIALISATION_OF_required_OF_PropagationShortCircuitTransfo2 ;
            bool REINITIALISATION_OF_S_OF_PropagationShortCircuitTransfo2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_PropagationShortCircuitTransfo2 ;
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
            bool REINITIALISATION_OF_required_OF_RO_CB_up_1 ;
            bool REINITIALISATION_OF_S_OF_RO_CB_up_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_CB_up_1 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RO_CB_up_1 ;
            bool REINITIALISATION_OF_required_OF_RO_CB_up_2 ;
            bool REINITIALISATION_OF_S_OF_RO_CB_up_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_CB_up_2 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RO_CB_up_2 ;
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
			bool const failF_FROZEN_OF_CB_dw_1 = false;
			std::string const calculate_required_OF_CB_dies = "fn_fathers_and_trig";
			double const mu_OF_RO_CB_up_1 = 0.1;
			double const mu_OF_RC_CB_dw_2 = 0.1;
			bool const trimming_OF_OPTIONS = false;
			bool const failF_FROZEN_OF_CB_dw_2 = false;
			std::string const calculate_required_OF_CB_dw_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_LossOfAllBackups = false;
			std::string const calculate_required_OF_PropagationShortCircuitTransfo2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_Transfo1 = false;
			std::string const calculate_required_OF_LossOfLine2 = "fn_fathers_and_trig";
			double const gamma_OF_RO_CB_up_1 = 0.0001;
			std::string const calculate_required_OF_LossOfDieselLine = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_CB_up_2 = false;
			double const gamma_OF_RC_CB_dw_2 = 0.0001;
			bool const force_relevant_events_OF_PropagationShortCircuitTransfo1 = false;
			double const mu_OF_GRID = 0.1;
			bool const force_relevant_events_OF_RS_dies = false;
			std::string const calculate_required_OF_CB_dw_2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_CB_up_1 = false;
			bool const failI_FROZEN_OF_RO_CB_up_1 = false;
			bool const failI_FROZEN_OF_RC_CB_dw_2 = false;
			double const mu_OF_Transfo2 = 0.1;
			bool const failF_FROZEN_OF_dies_generator = false;
			std::string const calculate_required_OF_CB_up_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_CB_dies = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
			std::string const when_to_check_OF_RC_CB_dies = "not_req_to_req";
			double const mu_OF_RC_CB_dies = 0.1;
			double const lambda_OF_Transfo1 = 0.0001;
			bool const force_relevant_events_OF_RC_CB_up_2 = false;
			std::string const calculate_required_OF_CB_up_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_CB_dw_1 = false;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_UE_1 = "always_true";
			bool const force_relevant_events_OF_PropagationShortCircuitTransfo2 = false;
			bool const force_relevant_events_OF_LossOfDieselLine = false;
			std::string const calculate_required_OF_RO_CB_up_1 = "fn_fathers_and_trig";
			double const lambda_OF_CB_dies = 0.0001;
			std::string const calculate_required_OF_RC_CB_dw_2 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_Loss_of_GRID = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_RO_CB_up_2 = false;
			bool const failF_FROZEN_OF_GRID = false;
			std::string const trigger_kind_OF_t_3_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_UE_1 = true;
			bool const failI_FROZEN_OF_RC_CB_dies = false;
			std::string const when_to_check_OF_RC_CB_up_2 = "not_req_to_req";
			bool const Trim_article_OF___ARBRE__EIRM = false;
			double const mu_OF_RC_CB_up_2 = 0.1;
			bool const force_relevant_events_OF_CB_up_2 = false;
			double const lambda_OF_CB_dw_1 = 0.0001;
			bool const Profil1_OF___ARBRE__EIRM = false;
			bool const failF_FROZEN_OF_Transfo2 = false;
			std::string const calculate_required_OF_LossOfLine_1 = "fn_fathers_and_trig";
			bool const No_repair_OF___ARBRE__EIRM = true;
			std::string const calculate_required_OF_GRID = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_CB_up_1 = false;
			bool const No_trim_OF___ARBRE__EIRM = true;
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			std::string const when_to_check_OF_RO_CB_up_2 = "not_req_to_req";
			double const mu_OF_Transfo1 = 0.1;
			double const lambda_OF_CB_dw_2 = 0.0001;
			double const mu_OF_RO_CB_up_2 = 0.1;
			std::string const calculate_required_OF_Transfo2 = "fn_fathers_and_trig";
			std::string const when_to_check_OF_RS_dies = "not_req_to_req";
			bool const repairable_system_OF_OPTIONS = false;
			std::string const calculate_required_OF_dies_generator = "fn_fathers_and_trig";
			double const mu_OF_RS_dies = 0.1;
			bool const force_relevant_events_OF_RO_CB_up_1 = false;
			double const lambda_OF_CB_up_2 = 0.0001;
			bool const force_relevant_events_OF_LossOfLine2 = false;
			std::string const calculate_required_OF_RC_CB_dies = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_RC_CB_dw_2 = false;
			double const gamma_OF_RC_CB_dies = 0.0001;
			bool const failI_FROZEN_OF_RC_CB_up_2 = false;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			double const lambda_OF_CB_up_1 = 0.0001;
			double const mu_OF_CB_dies = 0.1;
			bool const force_relevant_events_OF_CB_dw_2 = false;
			bool const force_relevant_events_OF_LossOfLine_1 = false;
			bool const force_relevant_events_OF_GRID = false;
			bool const failI_FROZEN_OF_RO_CB_up_2 = false;
			std::string const when_to_check_OF_RO_CB_up_1 = "not_req_to_req";
			double const mu_OF_CB_dw_1 = 0.1;
			std::string const when_to_check_OF_RC_CB_dw_2 = "not_req_to_req";
			bool const force_relevant_events_OF_AND_1 = false;
			bool const failI_FROZEN_OF_RS_dies = false;
			bool const force_relevant_events_OF_Transfo2 = false;
			bool const failF_FROZEN_OF_Transfo1 = false;
			std::string const trigger_kind_OF_t_3 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_RC_CB_up_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_dies_generator = false;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			bool const Default_OF___ARBRE__EIRM = true;
			double const gamma_OF_RC_CB_up_2 = 0.0001;
			bool const force_relevant_events_OF_RC_CB_dies = false;
			double const mu_OF_CB_dw_2 = 0.1;
			double const lambda_OF_GRID = 0.0001;
			std::string const calculate_required_OF_LossOfAllBackups = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_Loss_of_GRID = false;
			double const mu_OF_CB_up_2 = 0.1;
			std::string const calculate_required_OF_Transfo1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_RO_CB_up_2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_CB_dies = false;
			std::string const calculate_required_OF_PropagationShortCircuitTransfo1 = "fn_fathers_and_trig";
			double const gamma_OF_RO_CB_up_2 = 0.0001;
			double const lambda_OF_Transfo2 = 0.0001;
			std::string const calculate_required_OF_RS_dies = "fn_fathers_and_trig";
			double const mu_OF_CB_up_1 = 0.1;
			double const lambda_OF_dies_generator = 0.0001;
			double const gamma_OF_RS_dies = 0.0001;
			double const mu_OF_dies_generator = 0.1;
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_CB_dies;
            bool FIRE_xx10_OF_CB_dw_1;
            bool FIRE_xx10_OF_CB_dw_2;
            bool FIRE_xx10_OF_CB_up_1;
            bool FIRE_xx10_OF_CB_up_2;
            bool FIRE_xx10_OF_GRID;
            bool FIRE_xx23_OF_RC_CB_dies_INS_6;
            bool FIRE_xx23_OF_RC_CB_dies_INS_7;
            bool FIRE_xx23_OF_RC_CB_dw_2_INS_8;
            bool FIRE_xx23_OF_RC_CB_dw_2_INS_9;
            bool FIRE_xx23_OF_RC_CB_up_2_INS_10;
            bool FIRE_xx23_OF_RC_CB_up_2_INS_11;
            bool FIRE_xx23_OF_RO_CB_up_1_INS_12;
            bool FIRE_xx23_OF_RO_CB_up_1_INS_13;
            bool FIRE_xx23_OF_RO_CB_up_2_INS_14;
            bool FIRE_xx23_OF_RO_CB_up_2_INS_15;
            bool FIRE_xx23_OF_RS_dies_INS_16;
            bool FIRE_xx23_OF_RS_dies_INS_17;
            bool FIRE_xx10_OF_Transfo1;
            bool FIRE_xx10_OF_Transfo2;
            bool FIRE_xx10_OF_dies_generator;

            int required_OF_AND_1 = 0 ;
            int already_S_OF_AND_1 = 1 ;
            int S_OF_AND_1 = 2 ;
            int relevant_evt_OF_AND_1 = 3 ;
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
            int required_OF_Loss_of_GRID = 50 ;
            int already_S_OF_Loss_of_GRID = 51 ;
            int S_OF_Loss_of_GRID = 52 ;
            int relevant_evt_OF_Loss_of_GRID = 53 ;
            int required_OF_PropagationShortCircuitTransfo1 = 54 ;
            int already_S_OF_PropagationShortCircuitTransfo1 = 55 ;
            int S_OF_PropagationShortCircuitTransfo1 = 56 ;
            int relevant_evt_OF_PropagationShortCircuitTransfo1 = 57 ;
            int required_OF_PropagationShortCircuitTransfo2 = 58 ;
            int already_S_OF_PropagationShortCircuitTransfo2 = 59 ;
            int S_OF_PropagationShortCircuitTransfo2 = 60 ;
            int relevant_evt_OF_PropagationShortCircuitTransfo2 = 61 ;
            int required_OF_RC_CB_dies = 62 ;
            int already_S_OF_RC_CB_dies = 63 ;
            int S_OF_RC_CB_dies = 64 ;
            int relevant_evt_OF_RC_CB_dies = 65 ;
            int failI_OF_RC_CB_dies = 66 ;
            int to_be_fired_OF_RC_CB_dies = 67 ;
            int already_standby_OF_RC_CB_dies = 68 ;
            int already_required_OF_RC_CB_dies = 69 ;
            int required_OF_RC_CB_dw_2 = 70 ;
            int already_S_OF_RC_CB_dw_2 = 71 ;
            int S_OF_RC_CB_dw_2 = 72 ;
            int relevant_evt_OF_RC_CB_dw_2 = 73 ;
            int failI_OF_RC_CB_dw_2 = 74 ;
            int to_be_fired_OF_RC_CB_dw_2 = 75 ;
            int already_standby_OF_RC_CB_dw_2 = 76 ;
            int already_required_OF_RC_CB_dw_2 = 77 ;
            int required_OF_RC_CB_up_2 = 78 ;
            int already_S_OF_RC_CB_up_2 = 79 ;
            int S_OF_RC_CB_up_2 = 80 ;
            int relevant_evt_OF_RC_CB_up_2 = 81 ;
            int failI_OF_RC_CB_up_2 = 82 ;
            int to_be_fired_OF_RC_CB_up_2 = 83 ;
            int already_standby_OF_RC_CB_up_2 = 84 ;
            int already_required_OF_RC_CB_up_2 = 85 ;
            int required_OF_RO_CB_up_1 = 86 ;
            int already_S_OF_RO_CB_up_1 = 87 ;
            int S_OF_RO_CB_up_1 = 88 ;
            int relevant_evt_OF_RO_CB_up_1 = 89 ;
            int failI_OF_RO_CB_up_1 = 90 ;
            int to_be_fired_OF_RO_CB_up_1 = 91 ;
            int already_standby_OF_RO_CB_up_1 = 92 ;
            int already_required_OF_RO_CB_up_1 = 93 ;
            int required_OF_RO_CB_up_2 = 94 ;
            int already_S_OF_RO_CB_up_2 = 95 ;
            int S_OF_RO_CB_up_2 = 96 ;
            int relevant_evt_OF_RO_CB_up_2 = 97 ;
            int failI_OF_RO_CB_up_2 = 98 ;
            int to_be_fired_OF_RO_CB_up_2 = 99 ;
            int already_standby_OF_RO_CB_up_2 = 100 ;
            int already_required_OF_RO_CB_up_2 = 101 ;
            int required_OF_RS_dies = 102 ;
            int already_S_OF_RS_dies = 103 ;
            int S_OF_RS_dies = 104 ;
            int relevant_evt_OF_RS_dies = 105 ;
            int failI_OF_RS_dies = 106 ;
            int to_be_fired_OF_RS_dies = 107 ;
            int already_standby_OF_RS_dies = 108 ;
            int already_required_OF_RS_dies = 109 ;
            int required_OF_Transfo1 = 110 ;
            int already_S_OF_Transfo1 = 111 ;
            int S_OF_Transfo1 = 112 ;
            int relevant_evt_OF_Transfo1 = 113 ;
            int failF_OF_Transfo1 = 114 ;
            int required_OF_Transfo2 = 115 ;
            int already_S_OF_Transfo2 = 116 ;
            int S_OF_Transfo2 = 117 ;
            int relevant_evt_OF_Transfo2 = 118 ;
            int failF_OF_Transfo2 = 119 ;
            int required_OF_UE_1 = 120 ;
            int already_S_OF_UE_1 = 121 ;
            int S_OF_UE_1 = 122 ;
            int relevant_evt_OF_UE_1 = 123 ;
            int required_OF_dies_generator = 124 ;
            int already_S_OF_dies_generator = 125 ;
            int S_OF_dies_generator = 126 ;
            int relevant_evt_OF_dies_generator = 127 ;
            int failF_OF_dies_generator = 128 ;




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