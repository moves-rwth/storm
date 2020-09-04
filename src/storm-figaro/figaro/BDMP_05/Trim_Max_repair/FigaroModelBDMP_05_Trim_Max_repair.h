
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
        class FigaroProgram_BDMP_05_Trim_Max_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_05_Trim_Max_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
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
            	{"required_OF_Transfo1" , 50},
            	{"already_S_OF_Transfo1" , 51},
            	{"S_OF_Transfo1" , 52},
            	{"relevant_evt_OF_Transfo1" , 53},
            	{"failF_OF_Transfo1" , 54},
            	{"required_OF_Transfo2" , 55},
            	{"already_S_OF_Transfo2" , 56},
            	{"S_OF_Transfo2" , 57},
            	{"relevant_evt_OF_Transfo2" , 58},
            	{"failF_OF_Transfo2" , 59},
            	{"required_OF_UE_1" , 60},
            	{"already_S_OF_UE_1" , 61},
            	{"S_OF_UE_1" , 62},
            	{"relevant_evt_OF_UE_1" , 63},
            	{"required_OF_dies_generator" , 64},
            	{"already_S_OF_dies_generator" , 65},
            	{"S_OF_dies_generator" , 66},
            	{"relevant_evt_OF_dies_generator" , 67},
            	{"failF_OF_dies_generator" , 68}},

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
                    69 ,
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
			double const lambda_OF_dies_generator = 0.0001;
			std::string const calculate_required_OF_CB_dies = "fn_fathers_and_trig";
			double const mu_OF_CB_up_2 = 0.1;
			bool const failF_FROZEN_OF_CB_up_1 = false;
			bool const force_relevant_events_OF_Transfo1 = false;
			bool const failF_FROZEN_OF_Transfo2 = false;
			bool const force_relevant_events_OF_AND_1 = false;
			double const mu_OF_CB_dies = 0.1;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const No_repair_OF___ARBRE__EIRM = false;
			bool const trimming_OF_OPTIONS = true;
			double const lambda_OF_CB_up_1 = 0.0001;
			std::string const calculate_required_OF_Transfo1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_LossOfLine2 = false;
			double const lambda_OF_Transfo2 = 0.0001;
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_LossOfDieselLine = false;
			double const mu_OF_Transfo1 = 0.1;
			bool const failF_FROZEN_OF_CB_dw_1 = false;
			bool const force_relevant_events_OF_LossOfLine_1 = false;
			bool const force_relevant_events_OF_dies_generator = false;
			bool const repairable_system_OF_OPTIONS = true;
			bool const No_trim_OF___ARBRE__EIRM = false;
			bool const Profil1_OF___ARBRE__EIRM = false;
			bool const failF_FROZEN_OF_CB_dw_2 = false;
			std::string const calculate_required_OF_LossOfLine2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_GRID = false;
			bool const Default_OF___ARBRE__EIRM = true;
			std::string const calculate_required_OF_dies_generator = "fn_fathers_and_trig";
			double const lambda_OF_CB_dw_1 = 0.0001;
			std::string const calculate_required_OF_LossOfDieselLine = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_CB_up_1 = false;
			std::string const calculate_required_OF_LossOfLine_1 = "fn_fathers_and_trig";
			double const lambda_OF_CB_dw_2 = 0.0001;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			bool const force_relevant_events_OF_Transfo2 = false;
			double const mu_OF_dies_generator = 0.1;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			double const lambda_OF_GRID = 0.0001;
			std::string const calculate_required_OF_CB_up_1 = "fn_fathers_and_trig";
			double const mu_OF_CB_up_1 = 0.1;
			bool const failF_FROZEN_OF_CB_up_2 = false;
			std::string const calculate_required_OF_Transfo2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_UE_1 = true;
			bool const force_relevant_events_OF_CB_dw_1 = false;
			bool const failF_FROZEN_OF_CB_dies = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
			bool const force_relevant_events_OF_CB_dw_2 = false;
			double const lambda_OF_CB_up_2 = 0.0001;
			double const mu_OF_Transfo2 = 0.1;
			double const lambda_OF_CB_dies = 0.0001;
			bool const force_relevant_events_OF_GRID = false;
			bool const failF_FROZEN_OF_Transfo1 = false;
			bool const Trim_article_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_CB_dw_1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_CB_dw_2 = "fn_fathers_and_trig";
			double const mu_OF_CB_dw_1 = 0.1;
			double const lambda_OF_Transfo1 = 0.0001;
			double const mu_OF_CB_dw_2 = 0.1;
			bool const force_relevant_events_OF_CB_up_2 = false;
			std::string const calculate_required_OF_GRID = "fn_fathers_and_trig";
			double const mu_OF_GRID = 0.1;
			bool const force_relevant_events_OF_LossOfAllBackups = false;
			bool const failF_FROZEN_OF_dies_generator = false;
			bool const force_relevant_events_OF_CB_dies = false;
			std::string const calculate_required_OF_CB_up_2 = "fn_fathers_and_trig";
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
            bool FIRE_xx10_OF_Transfo1;
            bool FIRE_xx11_OF_Transfo1;
            bool FIRE_xx10_OF_Transfo2;
            bool FIRE_xx11_OF_Transfo2;
            bool FIRE_xx10_OF_dies_generator;
            bool FIRE_xx11_OF_dies_generator;

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
            int required_OF_Transfo1 = 50 ;
            int already_S_OF_Transfo1 = 51 ;
            int S_OF_Transfo1 = 52 ;
            int relevant_evt_OF_Transfo1 = 53 ;
            int failF_OF_Transfo1 = 54 ;
            int required_OF_Transfo2 = 55 ;
            int already_S_OF_Transfo2 = 56 ;
            int S_OF_Transfo2 = 57 ;
            int relevant_evt_OF_Transfo2 = 58 ;
            int failF_OF_Transfo2 = 59 ;
            int required_OF_UE_1 = 60 ;
            int already_S_OF_UE_1 = 61 ;
            int S_OF_UE_1 = 62 ;
            int relevant_evt_OF_UE_1 = 63 ;
            int required_OF_dies_generator = 64 ;
            int already_S_OF_dies_generator = 65 ;
            int S_OF_dies_generator = 66 ;
            int relevant_evt_OF_dies_generator = 67 ;
            int failF_OF_dies_generator = 68 ;




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