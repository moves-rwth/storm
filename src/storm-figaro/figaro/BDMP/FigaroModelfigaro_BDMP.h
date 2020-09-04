
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
        class FigaroProgram_figaro_BDMP: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_figaro_BDMP(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_AND_1" , 0},
            	{"already_S_OF_AND_1" , 1},
            	{"S_OF_AND_1" , 2},
            	{"relevant_evt_OF_AND_1" , 3},
            	{"required_OF_AND_2" , 4},
            	{"already_S_OF_AND_2" , 5},
            	{"S_OF_AND_2" , 6},
            	{"relevant_evt_OF_AND_2" , 7},
            	{"required_OF_A_and_B_unavailable" , 8},
            	{"already_S_OF_A_and_B_unavailable" , 9},
            	{"S_OF_A_and_B_unavailable" , 10},
            	{"relevant_evt_OF_A_and_B_unavailable" , 11},
            	{"required_OF_A_or_B_isolated" , 12},
            	{"already_S_OF_A_or_B_isolated" , 13},
            	{"S_OF_A_or_B_isolated" , 14},
            	{"relevant_evt_OF_A_or_B_isolated" , 15},
            	{"required_OF_A_unavailable" , 16},
            	{"already_S_OF_A_unavailable" , 17},
            	{"S_OF_A_unavailable" , 18},
            	{"relevant_evt_OF_A_unavailable" , 19},
            	{"required_OF_B_unavailable" , 20},
            	{"already_S_OF_B_unavailable" , 21},
            	{"S_OF_B_unavailable" , 22},
            	{"relevant_evt_OF_B_unavailable" , 23},
            	{"required_OF_FailureOfA" , 24},
            	{"already_S_OF_FailureOfA" , 25},
            	{"S_OF_FailureOfA" , 26},
            	{"relevant_evt_OF_FailureOfA" , 27},
            	{"failF_OF_FailureOfA" , 28},
            	{"required_OF_FailureOfB" , 29},
            	{"already_S_OF_FailureOfB" , 30},
            	{"S_OF_FailureOfB" , 31},
            	{"relevant_evt_OF_FailureOfB" , 32},
            	{"failF_OF_FailureOfB" , 33},
            	{"required_OF_IO_K1" , 34},
            	{"already_S_OF_IO_K1" , 35},
            	{"S_OF_IO_K1" , 36},
            	{"relevant_evt_OF_IO_K1" , 37},
            	{"failF_OF_IO_K1" , 38},
            	{"required_OF_IO_K2" , 39},
            	{"already_S_OF_IO_K2" , 40},
            	{"S_OF_IO_K2" , 41},
            	{"relevant_evt_OF_IO_K2" , 42},
            	{"failF_OF_IO_K2" , 43},
            	{"required_OF_IO_K3" , 44},
            	{"already_S_OF_IO_K3" , 45},
            	{"S_OF_IO_K3" , 46},
            	{"relevant_evt_OF_IO_K3" , 47},
            	{"failF_OF_IO_K3" , 48},
            	{"required_OF_IO_K4" , 49},
            	{"already_S_OF_IO_K4" , 50},
            	{"S_OF_IO_K4" , 51},
            	{"relevant_evt_OF_IO_K4" , 52},
            	{"failF_OF_IO_K4" , 53},
            	{"required_OF_IO_K5" , 54},
            	{"already_S_OF_IO_K5" , 55},
            	{"S_OF_IO_K5" , 56},
            	{"relevant_evt_OF_IO_K5" , 57},
            	{"failF_OF_IO_K5" , 58},
            	{"required_OF_RC_K5" , 59},
            	{"already_S_OF_RC_K5" , 60},
            	{"S_OF_RC_K5" , 61},
            	{"relevant_evt_OF_RC_K5" , 62},
            	{"failI_OF_RC_K5" , 63},
            	{"to_be_fired_OF_RC_K5" , 64},
            	{"already_standby_OF_RC_K5" , 65},
            	{"already_required_OF_RC_K5" , 66},
            	{"required_OF_RO_K1" , 67},
            	{"already_S_OF_RO_K1" , 68},
            	{"S_OF_RO_K1" , 69},
            	{"relevant_evt_OF_RO_K1" , 70},
            	{"failI_OF_RO_K1" , 71},
            	{"to_be_fired_OF_RO_K1" , 72},
            	{"already_standby_OF_RO_K1" , 73},
            	{"already_required_OF_RO_K1" , 74},
            	{"required_OF_RO_K2" , 75},
            	{"already_S_OF_RO_K2" , 76},
            	{"S_OF_RO_K2" , 77},
            	{"relevant_evt_OF_RO_K2" , 78},
            	{"failI_OF_RO_K2" , 79},
            	{"to_be_fired_OF_RO_K2" , 80},
            	{"already_standby_OF_RO_K2" , 81},
            	{"already_required_OF_RO_K2" , 82},
            	{"required_OF_RO_K3" , 83},
            	{"already_S_OF_RO_K3" , 84},
            	{"S_OF_RO_K3" , 85},
            	{"relevant_evt_OF_RO_K3" , 86},
            	{"failI_OF_RO_K3" , 87},
            	{"to_be_fired_OF_RO_K3" , 88},
            	{"already_standby_OF_RO_K3" , 89},
            	{"already_required_OF_RO_K3" , 90},
            	{"required_OF_RO_K4" , 91},
            	{"already_S_OF_RO_K4" , 92},
            	{"S_OF_RO_K4" , 93},
            	{"relevant_evt_OF_RO_K4" , 94},
            	{"failI_OF_RO_K4" , 95},
            	{"to_be_fired_OF_RO_K4" , 96},
            	{"already_standby_OF_RO_K4" , 97},
            	{"already_required_OF_RO_K4" , 98},
            	{"required_OF_UE_1" , 99},
            	{"already_S_OF_UE_1" , 100},
            	{"S_OF_UE_1" , 101},
            	{"relevant_evt_OF_UE_1" , 102},
            	{"required_OF_due_to_A" , 103},
            	{"already_S_OF_due_to_A" , 104},
            	{"S_OF_due_to_A" , 105},
            	{"relevant_evt_OF_due_to_A" , 106},
            	{"required_OF_due_to_B" , 107},
            	{"already_S_OF_due_to_B" , 108},
            	{"S_OF_due_to_B" , 109},
            	{"relevant_evt_OF_due_to_B" , 110},
            	{"required_OF_failure_on_phase_change" , 111},
            	{"already_S_OF_failure_on_phase_change" , 112},
            	{"S_OF_failure_on_phase_change" , 113},
            	{"relevant_evt_OF_failure_on_phase_change" , 114},
            	{"required_OF_impossible_to_isolate_A" , 115},
            	{"already_S_OF_impossible_to_isolate_A" , 116},
            	{"S_OF_impossible_to_isolate_A" , 117},
            	{"relevant_evt_OF_impossible_to_isolate_A" , 118},
            	{"required_OF_phase_1" , 119},
            	{"already_S_OF_phase_1" , 120},
            	{"S_OF_phase_1" , 121},
            	{"relevant_evt_OF_phase_1" , 122},
            	{"in_progress_OF_phase_1" , 123},
            	{"already_required_OF_phase_1" , 124},
            	{"start_phase_OF_phase_1" , 125},
            	{"required_OF_impossible_to_isolate_B" , 126},
            	{"already_S_OF_impossible_to_isolate_B" , 127},
            	{"S_OF_impossible_to_isolate_B" , 128},
            	{"relevant_evt_OF_impossible_to_isolate_B" , 129},
            	{"required_OF_phase_2" , 130},
            	{"already_S_OF_phase_2" , 131},
            	{"S_OF_phase_2" , 132},
            	{"relevant_evt_OF_phase_2" , 133},
            	{"in_progress_OF_phase_2" , 134},
            	{"already_required_OF_phase_2" , 135},
            	{"start_phase_OF_phase_2" , 136},
            	{"required_OF_short_circuit" , 137},
            	{"already_S_OF_short_circuit" , 138},
            	{"S_OF_short_circuit" , 139},
            	{"relevant_evt_OF_short_circuit" , 140},
            	{"required_OF_system_failure" , 141},
            	{"already_S_OF_system_failure" , 142},
            	{"S_OF_system_failure" , 143},
            	{"relevant_evt_OF_system_failure" , 144},
            	{"required_OF_system_failure_in_phase_1" , 145},
            	{"already_S_OF_system_failure_in_phase_1" , 146},
            	{"S_OF_system_failure_in_phase_1" , 147},
            	{"relevant_evt_OF_system_failure_in_phase_1" , 148},
            	{"required_OF_system_failure_in_phase_2" , 149},
            	{"already_S_OF_system_failure_in_phase_2" , 150},
            	{"S_OF_system_failure_in_phase_2" , 151},
            	{"relevant_evt_OF_system_failure_in_phase_2" , 152}},

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
                    153 ,
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
            bool REINITIALISATION_OF_required_OF_A_and_B_unavailable ;
            bool REINITIALISATION_OF_S_OF_A_and_B_unavailable ;
            bool REINITIALISATION_OF_relevant_evt_OF_A_and_B_unavailable ;
            bool REINITIALISATION_OF_required_OF_A_or_B_isolated ;
            bool REINITIALISATION_OF_S_OF_A_or_B_isolated ;
            bool REINITIALISATION_OF_relevant_evt_OF_A_or_B_isolated ;
            bool REINITIALISATION_OF_required_OF_A_unavailable ;
            bool REINITIALISATION_OF_S_OF_A_unavailable ;
            bool REINITIALISATION_OF_relevant_evt_OF_A_unavailable ;
            bool REINITIALISATION_OF_required_OF_B_unavailable ;
            bool REINITIALISATION_OF_S_OF_B_unavailable ;
            bool REINITIALISATION_OF_relevant_evt_OF_B_unavailable ;
            bool REINITIALISATION_OF_required_OF_FailureOfA ;
            bool REINITIALISATION_OF_S_OF_FailureOfA ;
            bool REINITIALISATION_OF_relevant_evt_OF_FailureOfA ;
            bool REINITIALISATION_OF_required_OF_FailureOfB ;
            bool REINITIALISATION_OF_S_OF_FailureOfB ;
            bool REINITIALISATION_OF_relevant_evt_OF_FailureOfB ;
            bool REINITIALISATION_OF_required_OF_IO_K1 ;
            bool REINITIALISATION_OF_S_OF_IO_K1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_IO_K1 ;
            bool REINITIALISATION_OF_required_OF_IO_K2 ;
            bool REINITIALISATION_OF_S_OF_IO_K2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_IO_K2 ;
            bool REINITIALISATION_OF_required_OF_IO_K3 ;
            bool REINITIALISATION_OF_S_OF_IO_K3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_IO_K3 ;
            bool REINITIALISATION_OF_required_OF_IO_K4 ;
            bool REINITIALISATION_OF_S_OF_IO_K4 ;
            bool REINITIALISATION_OF_relevant_evt_OF_IO_K4 ;
            bool REINITIALISATION_OF_required_OF_IO_K5 ;
            bool REINITIALISATION_OF_S_OF_IO_K5 ;
            bool REINITIALISATION_OF_relevant_evt_OF_IO_K5 ;
            bool REINITIALISATION_OF_required_OF_RC_K5 ;
            bool REINITIALISATION_OF_S_OF_RC_K5 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RC_K5 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RC_K5 ;
            bool REINITIALISATION_OF_required_OF_RO_K1 ;
            bool REINITIALISATION_OF_S_OF_RO_K1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_K1 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RO_K1 ;
            bool REINITIALISATION_OF_required_OF_RO_K2 ;
            bool REINITIALISATION_OF_S_OF_RO_K2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_K2 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RO_K2 ;
            bool REINITIALISATION_OF_required_OF_RO_K3 ;
            bool REINITIALISATION_OF_S_OF_RO_K3 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_K3 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RO_K3 ;
            bool REINITIALISATION_OF_required_OF_RO_K4 ;
            bool REINITIALISATION_OF_S_OF_RO_K4 ;
            bool REINITIALISATION_OF_relevant_evt_OF_RO_K4 ;
            bool REINITIALISATION_OF_to_be_fired_OF_RO_K4 ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            bool REINITIALISATION_OF_required_OF_due_to_A ;
            bool REINITIALISATION_OF_S_OF_due_to_A ;
            bool REINITIALISATION_OF_relevant_evt_OF_due_to_A ;
            bool REINITIALISATION_OF_required_OF_due_to_B ;
            bool REINITIALISATION_OF_S_OF_due_to_B ;
            bool REINITIALISATION_OF_relevant_evt_OF_due_to_B ;
            bool REINITIALISATION_OF_required_OF_failure_on_phase_change ;
            bool REINITIALISATION_OF_S_OF_failure_on_phase_change ;
            bool REINITIALISATION_OF_relevant_evt_OF_failure_on_phase_change ;
            bool REINITIALISATION_OF_required_OF_impossible_to_isolate_A ;
            bool REINITIALISATION_OF_S_OF_impossible_to_isolate_A ;
            bool REINITIALISATION_OF_relevant_evt_OF_impossible_to_isolate_A ;
            bool REINITIALISATION_OF_required_OF_phase_1 ;
            bool REINITIALISATION_OF_S_OF_phase_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_phase_1 ;
            bool REINITIALISATION_OF_required_OF_impossible_to_isolate_B ;
            bool REINITIALISATION_OF_S_OF_impossible_to_isolate_B ;
            bool REINITIALISATION_OF_relevant_evt_OF_impossible_to_isolate_B ;
            bool REINITIALISATION_OF_required_OF_phase_2 ;
            bool REINITIALISATION_OF_S_OF_phase_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_phase_2 ;
            bool REINITIALISATION_OF_required_OF_short_circuit ;
            bool REINITIALISATION_OF_S_OF_short_circuit ;
            bool REINITIALISATION_OF_relevant_evt_OF_short_circuit ;
            bool REINITIALISATION_OF_required_OF_system_failure ;
            bool REINITIALISATION_OF_S_OF_system_failure ;
            bool REINITIALISATION_OF_relevant_evt_OF_system_failure ;
            bool REINITIALISATION_OF_required_OF_system_failure_in_phase_1 ;
            bool REINITIALISATION_OF_S_OF_system_failure_in_phase_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_system_failure_in_phase_1 ;
            bool REINITIALISATION_OF_required_OF_system_failure_in_phase_2 ;
            bool REINITIALISATION_OF_S_OF_system_failure_in_phase_2 ;
            bool REINITIALISATION_OF_relevant_evt_OF_system_failure_in_phase_2 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			double const mu_OF_IO_K3 = 0.1;
			double const mu_OF_RC_K5 = 0.1;
			double const gamma_OF_RO_K4 = 0.05;
			std::string const calculate_required_OF_due_to_A = "fn_fathers_and_trig";
			std::string const calculate_required_OF_IO_K5 = "fn_fathers_and_trig";
			double const lambda_OF_IO_K1 = 0.0001;
			std::string const calculate_required_OF_RO_K4 = "fn_fathers_and_trig";
			std::string const when_to_check_OF_RO_K2 = "not_req_to_req";
			bool const force_relevant_events_OF_RO_K3 = false;
			bool const force_relevant_events_OF_phase_2 = false;
			std::string const calculate_required_OF_A_and_B_unavailable = "fn_fathers_and_trig";
			std::string const Dist_kind_OF_phase_1 = "exponential";
			bool const force_relevant_events_OF_system_failure_in_phase_1 = false;
			bool const Profil1_OF___ARBRE__EIRM = true;
			double const gamma_OF_RO_K1 = 0.05;
			bool const force_relevant_events_OF_short_circuit = false;
			std::string const calculate_required_OF_RO_K1 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_FailureOfA = false;
			std::string const when_to_check_OF_RC_K5 = "not_req_to_req";
			double const lambda_OF_FailureOfB = 0.0001;
			std::string const calculate_required_OF_B_unavailable = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_IO_K3 = false;
			bool const force_relevant_events_OF_UE_1 = true;
			std::string const calculate_required_OF_failure_on_phase_change = "fn_fathers_and_trig";
			double const gamma_OF_RO_K2 = 0.05;
			double const mu_OF_IO_K4 = 0.1;
			double const lambda_OF_IO_K5 = 0.0001;
			bool const force_relevant_events_OF_system_failure_in_phase_2 = false;
			std::string const calculate_required_OF_AND_2 = "fn_fathers_and_trig";
			double const mu_OF_FailureOfA = 0.1;
			std::string const calculate_required_OF_RC_K5 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_A_or_B_isolated = false;
			bool const T100_50_G01_OF___ARBRE__EIRM = false;
			double const mu_OF_IO_K2 = 0.1;
			bool const failF_FROZEN_OF_IO_K4 = false;
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_phase_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_due_to_B = false;
			std::string const trigger_kind_OF_t_8 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_IO_K1 = false;
			std::string const calculate_required_OF_system_failure = "fn_fathers_and_trig";
			double const mu_OF_RO_K3 = 0.1;
			std::string const Dist_kind_OF_phase_2 = "exponential";
			bool const force_relevant_events_OF_impossible_to_isolate_B = false;
			bool const failI_FROZEN_OF_RC_K5 = false;
			bool const failF_FROZEN_OF_IO_K2 = false;
			std::string const calculate_required_OF_IO_K4 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_RO_K2 = "fn_fathers_and_trig";
			bool const failI_FROZEN_OF_RO_K4 = false;
			bool const force_relevant_events_OF_FailureOfB = false;
			std::string const when_to_check_OF_RO_K3 = "not_req_to_req";
			std::string const calculate_required_OF_FailureOfA = "fn_fathers_and_trig";
			double const mu_OF_FailureOfB = 0.1;
			std::string const calculate_required_OF_A_unavailable = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_due_to_A = false;
			std::string const calculate_required_OF_impossible_to_isolate_A = "fn_fathers_and_trig";
			bool const failI_FROZEN_OF_RO_K1 = false;
			std::string const calculate_required_OF_IO_K2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_RO_K4 = false;
			bool const force_relevant_events_OF_IO_K5 = false;
			bool const T100_50_G005_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_IO_K3 = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			double const mu_OF_IO_K5 = 0.1;
			bool const trimming_OF_OPTIONS = true;
			bool const force_relevant_events_OF_A_and_B_unavailable = false;
			double const lambda_OF_IO_K4 = 0.0001;
			bool const repairable_system_OF_OPTIONS = false;
			bool const failI_FROZEN_OF_RO_K2 = false;
			bool const force_relevant_events_OF_RO_K1 = false;
			double const gamma_OF_RO_K3 = 0.05;
			std::string const calculate_required_OF_RO_K3 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_FailureOfB = false;
			std::string const calculate_required_OF_phase_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_B_unavailable = false;
			bool const force_relevant_events_OF_failure_on_phase_change = false;
			std::string const calculate_required_OF_system_failure_in_phase_1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_short_circuit = "fn_fathers_and_trig";
			double const lambda_OF_FailureOfA = 0.0001;
			double const mu_OF_IO_K1 = 0.1;
			double const lambda_OF_IO_K2 = 0.0001;
			bool const failF_FROZEN_OF_IO_K5 = false;
			std::string const calculate_required_OF_UE_1 = "fn_fathers_and_trig";
			double const lambda_OF_IO_K3 = 0.0001;
			bool const force_relevant_events_OF_AND_2 = false;
			bool const force_relevant_events_OF_RC_K5 = false;
			double const duration_OF_phase_1 = 1000;
			std::string const trigger_kind_OF_t_7 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_IO_K1 = false;
			bool const force_relevant_events_OF_AND_1 = false;
			std::string const calculate_required_OF_system_failure_in_phase_2 = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_10 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_phase_1 = false;
			std::string const calculate_required_OF_A_or_B_isolated = "fn_fathers_and_trig";
			double const mu_OF_RO_K4 = 0.1;
			bool const force_relevant_events_OF_system_failure = false;
			bool const force_relevant_events_OF_IO_K4 = false;
			double const duration_OF_phase_2 = 500;
			double const mu_OF_RO_K1 = 0.1;
			std::string const calculate_required_OF_due_to_B = "fn_fathers_and_trig";
			std::string const calculate_required_OF_IO_K1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_RO_K2 = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
			std::string const when_to_check_OF_RO_K4 = "not_req_to_req";
			std::string const trigger_kind_OF_t_9 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_impossible_to_isolate_B = "fn_fathers_and_trig";
			double const mu_OF_RO_K2 = 0.1;
			bool const force_relevant_events_OF_FailureOfA = false;
			bool const force_relevant_events_OF_A_unavailable = false;
			bool const force_relevant_events_OF_impossible_to_isolate_A = false;
			bool const force_relevant_events_OF_IO_K2 = false;
			std::string const calculate_required_OF_FailureOfB = "fn_fathers_and_trig";
			std::string const when_to_check_OF_RO_K1 = "not_req_to_req";
			bool const force_relevant_events_OF_IO_K3 = false;
			double const gamma_OF_RC_K5 = 0.05;
			bool const failI_FROZEN_OF_RO_K3 = false;
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_FailureOfA;
            bool FIRE_xx10_OF_FailureOfB;
            bool FIRE_xx10_OF_IO_K1;
            bool FIRE_xx10_OF_IO_K2;
            bool FIRE_xx10_OF_IO_K3;
            bool FIRE_xx10_OF_IO_K4;
            bool FIRE_xx10_OF_IO_K5;
            bool FIRE_xx23_OF_RC_K5_INS_7;
            bool FIRE_xx23_OF_RC_K5_INS_8;
            bool FIRE_xx23_OF_RO_K1_INS_9;
            bool FIRE_xx23_OF_RO_K1_INS_10;
            bool FIRE_xx23_OF_RO_K2_INS_11;
            bool FIRE_xx23_OF_RO_K2_INS_12;
            bool FIRE_xx23_OF_RO_K3_INS_13;
            bool FIRE_xx23_OF_RO_K3_INS_14;
            bool FIRE_xx23_OF_RO_K4_INS_15;
            bool FIRE_xx23_OF_RO_K4_INS_16;
            bool FIRE_xx43_a_OF_phase_1;
            bool FIRE_xx47_OF_phase_1_INS_18;
            bool FIRE_xx43_a_OF_phase_2;
            bool FIRE_xx47_OF_phase_2_INS_20;

            int required_OF_AND_1 = 0 ;
            int already_S_OF_AND_1 = 1 ;
            int S_OF_AND_1 = 2 ;
            int relevant_evt_OF_AND_1 = 3 ;
            int required_OF_AND_2 = 4 ;
            int already_S_OF_AND_2 = 5 ;
            int S_OF_AND_2 = 6 ;
            int relevant_evt_OF_AND_2 = 7 ;
            int required_OF_A_and_B_unavailable = 8 ;
            int already_S_OF_A_and_B_unavailable = 9 ;
            int S_OF_A_and_B_unavailable = 10 ;
            int relevant_evt_OF_A_and_B_unavailable = 11 ;
            int required_OF_A_or_B_isolated = 12 ;
            int already_S_OF_A_or_B_isolated = 13 ;
            int S_OF_A_or_B_isolated = 14 ;
            int relevant_evt_OF_A_or_B_isolated = 15 ;
            int required_OF_A_unavailable = 16 ;
            int already_S_OF_A_unavailable = 17 ;
            int S_OF_A_unavailable = 18 ;
            int relevant_evt_OF_A_unavailable = 19 ;
            int required_OF_B_unavailable = 20 ;
            int already_S_OF_B_unavailable = 21 ;
            int S_OF_B_unavailable = 22 ;
            int relevant_evt_OF_B_unavailable = 23 ;
            int required_OF_FailureOfA = 24 ;
            int already_S_OF_FailureOfA = 25 ;
            int S_OF_FailureOfA = 26 ;
            int relevant_evt_OF_FailureOfA = 27 ;
            int failF_OF_FailureOfA = 28 ;
            int required_OF_FailureOfB = 29 ;
            int already_S_OF_FailureOfB = 30 ;
            int S_OF_FailureOfB = 31 ;
            int relevant_evt_OF_FailureOfB = 32 ;
            int failF_OF_FailureOfB = 33 ;
            int required_OF_IO_K1 = 34 ;
            int already_S_OF_IO_K1 = 35 ;
            int S_OF_IO_K1 = 36 ;
            int relevant_evt_OF_IO_K1 = 37 ;
            int failF_OF_IO_K1 = 38 ;
            int required_OF_IO_K2 = 39 ;
            int already_S_OF_IO_K2 = 40 ;
            int S_OF_IO_K2 = 41 ;
            int relevant_evt_OF_IO_K2 = 42 ;
            int failF_OF_IO_K2 = 43 ;
            int required_OF_IO_K3 = 44 ;
            int already_S_OF_IO_K3 = 45 ;
            int S_OF_IO_K3 = 46 ;
            int relevant_evt_OF_IO_K3 = 47 ;
            int failF_OF_IO_K3 = 48 ;
            int required_OF_IO_K4 = 49 ;
            int already_S_OF_IO_K4 = 50 ;
            int S_OF_IO_K4 = 51 ;
            int relevant_evt_OF_IO_K4 = 52 ;
            int failF_OF_IO_K4 = 53 ;
            int required_OF_IO_K5 = 54 ;
            int already_S_OF_IO_K5 = 55 ;
            int S_OF_IO_K5 = 56 ;
            int relevant_evt_OF_IO_K5 = 57 ;
            int failF_OF_IO_K5 = 58 ;
            int required_OF_RC_K5 = 59 ;
            int already_S_OF_RC_K5 = 60 ;
            int S_OF_RC_K5 = 61 ;
            int relevant_evt_OF_RC_K5 = 62 ;
            int failI_OF_RC_K5 = 63 ;
            int to_be_fired_OF_RC_K5 = 64 ;
            int already_standby_OF_RC_K5 = 65 ;
            int already_required_OF_RC_K5 = 66 ;
            int required_OF_RO_K1 = 67 ;
            int already_S_OF_RO_K1 = 68 ;
            int S_OF_RO_K1 = 69 ;
            int relevant_evt_OF_RO_K1 = 70 ;
            int failI_OF_RO_K1 = 71 ;
            int to_be_fired_OF_RO_K1 = 72 ;
            int already_standby_OF_RO_K1 = 73 ;
            int already_required_OF_RO_K1 = 74 ;
            int required_OF_RO_K2 = 75 ;
            int already_S_OF_RO_K2 = 76 ;
            int S_OF_RO_K2 = 77 ;
            int relevant_evt_OF_RO_K2 = 78 ;
            int failI_OF_RO_K2 = 79 ;
            int to_be_fired_OF_RO_K2 = 80 ;
            int already_standby_OF_RO_K2 = 81 ;
            int already_required_OF_RO_K2 = 82 ;
            int required_OF_RO_K3 = 83 ;
            int already_S_OF_RO_K3 = 84 ;
            int S_OF_RO_K3 = 85 ;
            int relevant_evt_OF_RO_K3 = 86 ;
            int failI_OF_RO_K3 = 87 ;
            int to_be_fired_OF_RO_K3 = 88 ;
            int already_standby_OF_RO_K3 = 89 ;
            int already_required_OF_RO_K3 = 90 ;
            int required_OF_RO_K4 = 91 ;
            int already_S_OF_RO_K4 = 92 ;
            int S_OF_RO_K4 = 93 ;
            int relevant_evt_OF_RO_K4 = 94 ;
            int failI_OF_RO_K4 = 95 ;
            int to_be_fired_OF_RO_K4 = 96 ;
            int already_standby_OF_RO_K4 = 97 ;
            int already_required_OF_RO_K4 = 98 ;
            int required_OF_UE_1 = 99 ;
            int already_S_OF_UE_1 = 100 ;
            int S_OF_UE_1 = 101 ;
            int relevant_evt_OF_UE_1 = 102 ;
            int required_OF_due_to_A = 103 ;
            int already_S_OF_due_to_A = 104 ;
            int S_OF_due_to_A = 105 ;
            int relevant_evt_OF_due_to_A = 106 ;
            int required_OF_due_to_B = 107 ;
            int already_S_OF_due_to_B = 108 ;
            int S_OF_due_to_B = 109 ;
            int relevant_evt_OF_due_to_B = 110 ;
            int required_OF_failure_on_phase_change = 111 ;
            int already_S_OF_failure_on_phase_change = 112 ;
            int S_OF_failure_on_phase_change = 113 ;
            int relevant_evt_OF_failure_on_phase_change = 114 ;
            int required_OF_impossible_to_isolate_A = 115 ;
            int already_S_OF_impossible_to_isolate_A = 116 ;
            int S_OF_impossible_to_isolate_A = 117 ;
            int relevant_evt_OF_impossible_to_isolate_A = 118 ;
            int required_OF_phase_1 = 119 ;
            int already_S_OF_phase_1 = 120 ;
            int S_OF_phase_1 = 121 ;
            int relevant_evt_OF_phase_1 = 122 ;
            int in_progress_OF_phase_1 = 123 ;
            int already_required_OF_phase_1 = 124 ;
            int start_phase_OF_phase_1 = 125 ;
            int required_OF_impossible_to_isolate_B = 126 ;
            int already_S_OF_impossible_to_isolate_B = 127 ;
            int S_OF_impossible_to_isolate_B = 128 ;
            int relevant_evt_OF_impossible_to_isolate_B = 129 ;
            int required_OF_phase_2 = 130 ;
            int already_S_OF_phase_2 = 131 ;
            int S_OF_phase_2 = 132 ;
            int relevant_evt_OF_phase_2 = 133 ;
            int in_progress_OF_phase_2 = 134 ;
            int already_required_OF_phase_2 = 135 ;
            int start_phase_OF_phase_2 = 136 ;
            int required_OF_short_circuit = 137 ;
            int already_S_OF_short_circuit = 138 ;
            int S_OF_short_circuit = 139 ;
            int relevant_evt_OF_short_circuit = 140 ;
            int required_OF_system_failure = 141 ;
            int already_S_OF_system_failure = 142 ;
            int S_OF_system_failure = 143 ;
            int relevant_evt_OF_system_failure = 144 ;
            int required_OF_system_failure_in_phase_1 = 145 ;
            int already_S_OF_system_failure_in_phase_1 = 146 ;
            int S_OF_system_failure_in_phase_1 = 147 ;
            int relevant_evt_OF_system_failure_in_phase_1 = 148 ;
            int required_OF_system_failure_in_phase_2 = 149 ;
            int already_S_OF_system_failure_in_phase_2 = 150 ;
            int S_OF_system_failure_in_phase_2 = 151 ;
            int relevant_evt_OF_system_failure_in_phase_2 = 152 ;




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