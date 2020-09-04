
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
        class FigaroProgram_BDMP_01_2trainsElec_Trim_Max_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_01_2trainsElec_Trim_Max_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_AND_1" , 0},
            	{"already_S_OF_AND_1" , 1},
            	{"S_OF_AND_1" , 2},
            	{"relevant_evt_OF_AND_1" , 3},
            	{"required_OF_CB1_IO" , 4},
            	{"already_S_OF_CB1_IO" , 5},
            	{"S_OF_CB1_IO" , 6},
            	{"relevant_evt_OF_CB1_IO" , 7},
            	{"failF_OF_CB1_IO" , 8},
            	{"required_OF_CB1_RO" , 9},
            	{"already_S_OF_CB1_RO" , 10},
            	{"S_OF_CB1_RO" , 11},
            	{"relevant_evt_OF_CB1_RO" , 12},
            	{"failI_OF_CB1_RO" , 13},
            	{"to_be_fired_OF_CB1_RO" , 14},
            	{"already_standby_OF_CB1_RO" , 15},
            	{"already_required_OF_CB1_RO" , 16},
            	{"required_OF_CB2_RC" , 17},
            	{"already_S_OF_CB2_RC" , 18},
            	{"S_OF_CB2_RC" , 19},
            	{"relevant_evt_OF_CB2_RC" , 20},
            	{"failI_OF_CB2_RC" , 21},
            	{"to_be_fired_OF_CB2_RC" , 22},
            	{"already_standby_OF_CB2_RC" , 23},
            	{"already_required_OF_CB2_RC" , 24},
            	{"required_OF_Dies_gen" , 25},
            	{"already_S_OF_Dies_gen" , 26},
            	{"S_OF_Dies_gen" , 27},
            	{"relevant_evt_OF_Dies_gen" , 28},
            	{"failF_OF_Dies_gen" , 29},
            	{"required_OF_Dies_gen_RS" , 30},
            	{"already_S_OF_Dies_gen_RS" , 31},
            	{"S_OF_Dies_gen_RS" , 32},
            	{"relevant_evt_OF_Dies_gen_RS" , 33},
            	{"failI_OF_Dies_gen_RS" , 34},
            	{"to_be_fired_OF_Dies_gen_RS" , 35},
            	{"already_standby_OF_Dies_gen_RS" , 36},
            	{"already_required_OF_Dies_gen_RS" , 37},
            	{"required_OF_Grid" , 38},
            	{"already_S_OF_Grid" , 39},
            	{"S_OF_Grid" , 40},
            	{"relevant_evt_OF_Grid" , 41},
            	{"failF_OF_Grid" , 42},
            	{"required_OF_LossOfDieselLine" , 43},
            	{"already_S_OF_LossOfDieselLine" , 44},
            	{"S_OF_LossOfDieselLine" , 45},
            	{"relevant_evt_OF_LossOfDieselLine" , 46},
            	{"required_OF_LossOfLine_1" , 47},
            	{"already_S_OF_LossOfLine_1" , 48},
            	{"S_OF_LossOfLine_1" , 49},
            	{"relevant_evt_OF_LossOfLine_1" , 50},
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
            bool REINITIALISATION_OF_required_OF_CB1_IO ;
            bool REINITIALISATION_OF_S_OF_CB1_IO ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB1_IO ;
            bool REINITIALISATION_OF_required_OF_CB1_RO ;
            bool REINITIALISATION_OF_S_OF_CB1_RO ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB1_RO ;
            bool REINITIALISATION_OF_to_be_fired_OF_CB1_RO ;
            bool REINITIALISATION_OF_required_OF_CB2_RC ;
            bool REINITIALISATION_OF_S_OF_CB2_RC ;
            bool REINITIALISATION_OF_relevant_evt_OF_CB2_RC ;
            bool REINITIALISATION_OF_to_be_fired_OF_CB2_RC ;
            bool REINITIALISATION_OF_required_OF_Dies_gen ;
            bool REINITIALISATION_OF_S_OF_Dies_gen ;
            bool REINITIALISATION_OF_relevant_evt_OF_Dies_gen ;
            bool REINITIALISATION_OF_required_OF_Dies_gen_RS ;
            bool REINITIALISATION_OF_S_OF_Dies_gen_RS ;
            bool REINITIALISATION_OF_relevant_evt_OF_Dies_gen_RS ;
            bool REINITIALISATION_OF_to_be_fired_OF_Dies_gen_RS ;
            bool REINITIALISATION_OF_required_OF_Grid ;
            bool REINITIALISATION_OF_S_OF_Grid ;
            bool REINITIALISATION_OF_relevant_evt_OF_Grid ;
            bool REINITIALISATION_OF_required_OF_LossOfDieselLine ;
            bool REINITIALISATION_OF_S_OF_LossOfDieselLine ;
            bool REINITIALISATION_OF_relevant_evt_OF_LossOfDieselLine ;
            bool REINITIALISATION_OF_required_OF_LossOfLine_1 ;
            bool REINITIALISATION_OF_S_OF_LossOfLine_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_LossOfLine_1 ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			double const mu_OF_CB1_RO = 0.1;
			bool const force_relevant_events_OF_Grid = false;
			bool const repairable_system_OF_OPTIONS = true;
			double const mu_OF_CB1_IO = 0.1;
			bool const failF_FROZEN_OF_CB1_IO = false;
			bool const force_relevant_events_OF_Dies_gen_RS = false;
			std::string const calculate_required_OF_Grid = "fn_fathers_and_trig";
			double const lambda_OF_Dies_gen = 0.0001;
			bool const force_relevant_events_OF_AND_1 = false;
			std::string const calculate_required_OF_Dies_gen_RS = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_CB2_RC = false;
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_Dies_gen = false;
			double const mu_OF_Dies_gen = 0.1;
			double const gamma_OF_Dies_gen_RS = 0.0001;
			bool const No_repair_OF___ARBRE__EIRM = false;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_CB2_RC = "fn_fathers_and_trig";
			double const gamma_OF_CB2_RC = 0.0001;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_CB1_IO = false;
			bool const force_relevant_events_OF_CB1_RO = false;
			std::string const calculate_required_OF_UE_1 = "always_true";
			std::string const calculate_required_OF_CB1_RO = "fn_fathers_and_trig";
			std::string const calculate_required_OF_CB1_IO = "fn_fathers_and_trig";
			double const gamma_OF_CB1_RO = 0.0001;
			bool const failI_FROZEN_OF_Dies_gen_RS = false;
			bool const force_relevant_events_OF_LossOfDieselLine = false;
			bool const No_trim_OF___ARBRE__EIRM = false;
			double const lambda_OF_Grid = 0.0001;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			std::string const calculate_required_OF_LossOfDieselLine = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_Dies_gen = false;
			bool const default_OF___ARBRE__EIRM = true;
			bool const failI_FROZEN_OF_CB2_RC = false;
			std::string const calculate_required_OF_Dies_gen = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_Grid = false;
			double const mu_OF_Grid = 0.1;
			bool const failI_FROZEN_OF_CB1_RO = false;
			std::string const when_to_check_OF_Dies_gen_RS = "not_req_to_req";
			bool const force_relevant_events_OF_UE_1 = true;
			bool const trimming_OF_OPTIONS = true;
			std::string const when_to_check_OF_CB2_RC = "not_req_to_req";
			double const mu_OF_Dies_gen_RS = 0.1;
			bool const Trim_article_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_LossOfLine_1 = false;
			bool const Profil1_OF___ARBRE__EIRM = false;
			double const lambda_OF_CB1_IO = 0.0001;
			std::string const when_to_check_OF_CB1_RO = "not_req_to_req";
			double const mu_OF_CB2_RC = 0.1;
			std::string const calculate_required_OF_LossOfLine_1 = "fn_fathers_and_trig";
			std::string const trimming_option_OF_OPTIONS = "maximum";
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_CB1_IO;
            bool FIRE_xx11_OF_CB1_IO;
            bool FIRE_xx23_OF_CB1_RO_INS_2;
            bool FIRE_xx23_OF_CB1_RO_INS_3;
            bool FIRE_xx24_OF_CB1_RO;
            bool FIRE_xx23_OF_CB2_RC_INS_5;
            bool FIRE_xx23_OF_CB2_RC_INS_6;
            bool FIRE_xx24_OF_CB2_RC;
            bool FIRE_xx10_OF_Dies_gen;
            bool FIRE_xx11_OF_Dies_gen;
            bool FIRE_xx23_OF_Dies_gen_RS_INS_10;
            bool FIRE_xx23_OF_Dies_gen_RS_INS_11;
            bool FIRE_xx24_OF_Dies_gen_RS;
            bool FIRE_xx10_OF_Grid;
            bool FIRE_xx11_OF_Grid;

            int required_OF_AND_1 = 0 ;
            int already_S_OF_AND_1 = 1 ;
            int S_OF_AND_1 = 2 ;
            int relevant_evt_OF_AND_1 = 3 ;
            int required_OF_CB1_IO = 4 ;
            int already_S_OF_CB1_IO = 5 ;
            int S_OF_CB1_IO = 6 ;
            int relevant_evt_OF_CB1_IO = 7 ;
            int failF_OF_CB1_IO = 8 ;
            int required_OF_CB1_RO = 9 ;
            int already_S_OF_CB1_RO = 10 ;
            int S_OF_CB1_RO = 11 ;
            int relevant_evt_OF_CB1_RO = 12 ;
            int failI_OF_CB1_RO = 13 ;
            int to_be_fired_OF_CB1_RO = 14 ;
            int already_standby_OF_CB1_RO = 15 ;
            int already_required_OF_CB1_RO = 16 ;
            int required_OF_CB2_RC = 17 ;
            int already_S_OF_CB2_RC = 18 ;
            int S_OF_CB2_RC = 19 ;
            int relevant_evt_OF_CB2_RC = 20 ;
            int failI_OF_CB2_RC = 21 ;
            int to_be_fired_OF_CB2_RC = 22 ;
            int already_standby_OF_CB2_RC = 23 ;
            int already_required_OF_CB2_RC = 24 ;
            int required_OF_Dies_gen = 25 ;
            int already_S_OF_Dies_gen = 26 ;
            int S_OF_Dies_gen = 27 ;
            int relevant_evt_OF_Dies_gen = 28 ;
            int failF_OF_Dies_gen = 29 ;
            int required_OF_Dies_gen_RS = 30 ;
            int already_S_OF_Dies_gen_RS = 31 ;
            int S_OF_Dies_gen_RS = 32 ;
            int relevant_evt_OF_Dies_gen_RS = 33 ;
            int failI_OF_Dies_gen_RS = 34 ;
            int to_be_fired_OF_Dies_gen_RS = 35 ;
            int already_standby_OF_Dies_gen_RS = 36 ;
            int already_required_OF_Dies_gen_RS = 37 ;
            int required_OF_Grid = 38 ;
            int already_S_OF_Grid = 39 ;
            int S_OF_Grid = 40 ;
            int relevant_evt_OF_Grid = 41 ;
            int failF_OF_Grid = 42 ;
            int required_OF_LossOfDieselLine = 43 ;
            int already_S_OF_LossOfDieselLine = 44 ;
            int S_OF_LossOfDieselLine = 45 ;
            int relevant_evt_OF_LossOfDieselLine = 46 ;
            int required_OF_LossOfLine_1 = 47 ;
            int already_S_OF_LossOfLine_1 = 48 ;
            int S_OF_LossOfLine_1 = 49 ;
            int relevant_evt_OF_LossOfLine_1 = 50 ;
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