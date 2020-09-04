
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
        class FigaroProgram_BDMP_08_PC_Trim_Max_No_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_08_PC_Trim_Max_No_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_AND_1" , 0},
            	{"already_S_OF_AND_1" , 1},
            	{"S_OF_AND_1" , 2},
            	{"relevant_evt_OF_AND_1" , 3},
            	{"required_OF_Cpu" , 4},
            	{"already_S_OF_Cpu" , 5},
            	{"S_OF_Cpu" , 6},
            	{"relevant_evt_OF_Cpu" , 7},
            	{"failF_OF_Cpu" , 8},
            	{"failS_OF_Cpu" , 9},
            	{"required_OF_DisplayUnit" , 10},
            	{"already_S_OF_DisplayUnit" , 11},
            	{"S_OF_DisplayUnit" , 12},
            	{"relevant_evt_OF_DisplayUnit" , 13},
            	{"failF_OF_DisplayUnit" , 14},
            	{"required_OF_Fan" , 15},
            	{"already_S_OF_Fan" , 16},
            	{"S_OF_Fan" , 17},
            	{"relevant_evt_OF_Fan" , 18},
            	{"failF_OF_Fan" , 19},
            	{"required_OF_FloppyDrive" , 20},
            	{"already_S_OF_FloppyDrive" , 21},
            	{"S_OF_FloppyDrive" , 22},
            	{"relevant_evt_OF_FloppyDrive" , 23},
            	{"failF_OF_FloppyDrive" , 24},
            	{"required_OF_FloppyDriveInUse" , 25},
            	{"already_S_OF_FloppyDriveInUse" , 26},
            	{"S_OF_FloppyDriveInUse" , 27},
            	{"relevant_evt_OF_FloppyDriveInUse" , 28},
            	{"failF_OF_FloppyDriveInUse" , 29},
            	{"required_OF_HardDrive" , 30},
            	{"already_S_OF_HardDrive" , 31},
            	{"S_OF_HardDrive" , 32},
            	{"relevant_evt_OF_HardDrive" , 33},
            	{"failF_OF_HardDrive" , 34},
            	{"required_OF_LossOfCooling" , 35},
            	{"already_S_OF_LossOfCooling" , 36},
            	{"S_OF_LossOfCooling" , 37},
            	{"relevant_evt_OF_LossOfCooling" , 38},
            	{"required_OF_ManipulationError" , 39},
            	{"already_S_OF_ManipulationError" , 40},
            	{"S_OF_ManipulationError" , 41},
            	{"relevant_evt_OF_ManipulationError" , 42},
            	{"failI_OF_ManipulationError" , 43},
            	{"to_be_fired_OF_ManipulationError" , 44},
            	{"already_standby_OF_ManipulationError" , 45},
            	{"already_required_OF_ManipulationError" , 46},
            	{"required_OF_Memory" , 47},
            	{"already_S_OF_Memory" , 48},
            	{"S_OF_Memory" , 49},
            	{"relevant_evt_OF_Memory" , 50},
            	{"failF_OF_Memory" , 51},
            	{"required_OF_OR_1" , 52},
            	{"already_S_OF_OR_1" , 53},
            	{"S_OF_OR_1" , 54},
            	{"relevant_evt_OF_OR_1" , 55},
            	{"required_OF_PC_down" , 56},
            	{"already_S_OF_PC_down" , 57},
            	{"S_OF_PC_down" , 58},
            	{"relevant_evt_OF_PC_down" , 59},
            	{"required_OF_PowerSupply" , 60},
            	{"already_S_OF_PowerSupply" , 61},
            	{"S_OF_PowerSupply" , 62},
            	{"relevant_evt_OF_PowerSupply" , 63},
            	{"failF_OF_PowerSupply" , 64},
            	{"required_OF_UE_1" , 65},
            	{"already_S_OF_UE_1" , 66},
            	{"S_OF_UE_1" , 67},
            	{"relevant_evt_OF_UE_1" , 68},
            	{"required_OF_Windows" , 69},
            	{"already_S_OF_Windows" , 70},
            	{"S_OF_Windows" , 71},
            	{"relevant_evt_OF_Windows" , 72},
            	{"failF_OF_Windows" , 73}},

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
                    74 ,
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
            bool REINITIALISATION_OF_required_OF_Cpu ;
            bool REINITIALISATION_OF_S_OF_Cpu ;
            bool REINITIALISATION_OF_relevant_evt_OF_Cpu ;
            bool REINITIALISATION_OF_required_OF_DisplayUnit ;
            bool REINITIALISATION_OF_S_OF_DisplayUnit ;
            bool REINITIALISATION_OF_relevant_evt_OF_DisplayUnit ;
            bool REINITIALISATION_OF_required_OF_Fan ;
            bool REINITIALISATION_OF_S_OF_Fan ;
            bool REINITIALISATION_OF_relevant_evt_OF_Fan ;
            bool REINITIALISATION_OF_required_OF_FloppyDrive ;
            bool REINITIALISATION_OF_S_OF_FloppyDrive ;
            bool REINITIALISATION_OF_relevant_evt_OF_FloppyDrive ;
            bool REINITIALISATION_OF_required_OF_FloppyDriveInUse ;
            bool REINITIALISATION_OF_S_OF_FloppyDriveInUse ;
            bool REINITIALISATION_OF_relevant_evt_OF_FloppyDriveInUse ;
            bool REINITIALISATION_OF_required_OF_HardDrive ;
            bool REINITIALISATION_OF_S_OF_HardDrive ;
            bool REINITIALISATION_OF_relevant_evt_OF_HardDrive ;
            bool REINITIALISATION_OF_required_OF_LossOfCooling ;
            bool REINITIALISATION_OF_S_OF_LossOfCooling ;
            bool REINITIALISATION_OF_relevant_evt_OF_LossOfCooling ;
            bool REINITIALISATION_OF_required_OF_ManipulationError ;
            bool REINITIALISATION_OF_S_OF_ManipulationError ;
            bool REINITIALISATION_OF_relevant_evt_OF_ManipulationError ;
            bool REINITIALISATION_OF_to_be_fired_OF_ManipulationError ;
            bool REINITIALISATION_OF_required_OF_Memory ;
            bool REINITIALISATION_OF_S_OF_Memory ;
            bool REINITIALISATION_OF_relevant_evt_OF_Memory ;
            bool REINITIALISATION_OF_required_OF_OR_1 ;
            bool REINITIALISATION_OF_S_OF_OR_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_1 ;
            bool REINITIALISATION_OF_required_OF_PC_down ;
            bool REINITIALISATION_OF_S_OF_PC_down ;
            bool REINITIALISATION_OF_relevant_evt_OF_PC_down ;
            bool REINITIALISATION_OF_required_OF_PowerSupply ;
            bool REINITIALISATION_OF_S_OF_PowerSupply ;
            bool REINITIALISATION_OF_relevant_evt_OF_PowerSupply ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            bool REINITIALISATION_OF_required_OF_Windows ;
            bool REINITIALISATION_OF_S_OF_Windows ;
            bool REINITIALISATION_OF_relevant_evt_OF_Windows ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			bool const force_relevant_events_OF_OR_1 = false;
			std::string const calculate_required_OF_PC_down = "fn_fathers_and_trig";
			std::string const calculate_required_OF_PowerSupply = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_DisplayUnit = false;
			bool const force_relevant_events_OF_AND_1 = false;
			double const mu_OF_PowerSupply = 0.1;
			bool const force_relevant_events_OF_Windows = false;
			double const lambda_OF_HardDrive = 0.0001;
			bool const force_relevant_events_OF_FloppyDrive = false;
			bool const failF_FROZEN_OF_FloppyDriveInUse = false;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const No_repair_OF___ARBRE__EIRM = true;
			bool const trimming_OF_OPTIONS = true;
			bool const force_relevant_events_OF_Cpu = false;
			double const lambda_OF_FloppyDriveInUse = 0.0001;
			std::string const calculate_required_OF_OR_1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_DisplayUnit = "fn_fathers_and_trig";
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			std::string const when_to_check_OF_ManipulationError = "not_req_to_req";
			std::string const calculate_required_OF_UE_1 = "always_true";
			bool const repairable_system_OF_OPTIONS = false;
			std::string const calculate_required_OF_FloppyDrive = "fn_fathers_and_trig";
			double const mu_OF_FloppyDrive = 0.1;
			std::string const calculate_required_OF_Windows = "fn_fathers_and_trig";
			double const mu_OF_DisplayUnit = 0.1;
			bool const failI_FROZEN_OF_ManipulationError = false;
			double const mu_OF_Windows = 0.1;
			bool const No_trim_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_LossOfCooling = false;
			std::string const calculate_required_OF_Cpu = "fn_fathers_and_trig";
			double const mu_OF_Cpu = 0.1;
			bool const force_relevant_events_OF_HardDrive = false;
			bool const Profil1_OF___ARBRE__EIRM = false;
			bool const Default_OF___ARBRE__EIRM = true;
			bool const failF_FROZEN_OF_Fan = false;
			double const standby_lambda_OF_Cpu = 1e-05;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			bool const force_relevant_events_OF_FloppyDriveInUse = false;
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_LossOfCooling = "fn_fathers_and_trig";
			std::string const calculate_required_OF_HardDrive = "fn_fathers_and_trig";
			double const lambda_OF_Fan = 0.0001;
			bool const failF_FROZEN_OF_Memory = false;
			double const mu_OF_HardDrive = 0.1;
			std::string const calculate_required_OF_FloppyDriveInUse = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_PowerSupply = false;
			bool const force_relevant_events_OF_UE_1 = true;
			bool const failS_FROZEN_OF_Cpu = false;
			std::string const trimming_option_OF_OPTIONS = "maximum";
			double const mu_OF_FloppyDriveInUse = 0.1;
			bool const force_relevant_events_OF_ManipulationError = false;
			double const lambda_OF_Memory = 0.0001;
			double const lambda_OF_PowerSupply = 0.0001;
			bool const Trim_article_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_Fan = false;
			std::string const calculate_required_OF_ManipulationError = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_Windows = false;
			bool const failF_FROZEN_OF_FloppyDrive = false;
			bool const failF_FROZEN_OF_DisplayUnit = false;
			double const gamma_OF_ManipulationError = 0.0001;
			double const mu_OF_ManipulationError = 0.1;
			bool const failF_FROZEN_OF_Cpu = false;
			bool const force_relevant_events_OF_Memory = false;
			double const lambda_OF_DisplayUnit = 0.0001;
			std::string const calculate_required_OF_Fan = "fn_fathers_and_trig";
			double const mu_OF_Fan = 0.1;
			double const lambda_OF_Windows = 0.0001;
			double const lambda_OF_FloppyDrive = 0.0001;
			bool const force_relevant_events_OF_PC_down = false;
			bool const force_relevant_events_OF_PowerSupply = false;
			double const lambda_OF_Cpu = 0.0001;
			bool const failF_FROZEN_OF_HardDrive = false;
			std::string const calculate_required_OF_Memory = "fn_fathers_and_trig";
			double const mu_OF_Memory = 0.1;
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx17_OF_Cpu;
            bool FIRE_xx18_OF_Cpu;
            bool FIRE_xx10_OF_DisplayUnit;
            bool FIRE_xx10_OF_Fan;
            bool FIRE_xx10_OF_FloppyDrive;
            bool FIRE_xx10_OF_FloppyDriveInUse;
            bool FIRE_xx10_OF_HardDrive;
            bool FIRE_xx23_OF_ManipulationError_INS_7;
            bool FIRE_xx23_OF_ManipulationError_INS_8;
            bool FIRE_xx10_OF_Memory;
            bool FIRE_xx10_OF_PowerSupply;
            bool FIRE_xx10_OF_Windows;

            int required_OF_AND_1 = 0 ;
            int already_S_OF_AND_1 = 1 ;
            int S_OF_AND_1 = 2 ;
            int relevant_evt_OF_AND_1 = 3 ;
            int required_OF_Cpu = 4 ;
            int already_S_OF_Cpu = 5 ;
            int S_OF_Cpu = 6 ;
            int relevant_evt_OF_Cpu = 7 ;
            int failF_OF_Cpu = 8 ;
            int failS_OF_Cpu = 9 ;
            int required_OF_DisplayUnit = 10 ;
            int already_S_OF_DisplayUnit = 11 ;
            int S_OF_DisplayUnit = 12 ;
            int relevant_evt_OF_DisplayUnit = 13 ;
            int failF_OF_DisplayUnit = 14 ;
            int required_OF_Fan = 15 ;
            int already_S_OF_Fan = 16 ;
            int S_OF_Fan = 17 ;
            int relevant_evt_OF_Fan = 18 ;
            int failF_OF_Fan = 19 ;
            int required_OF_FloppyDrive = 20 ;
            int already_S_OF_FloppyDrive = 21 ;
            int S_OF_FloppyDrive = 22 ;
            int relevant_evt_OF_FloppyDrive = 23 ;
            int failF_OF_FloppyDrive = 24 ;
            int required_OF_FloppyDriveInUse = 25 ;
            int already_S_OF_FloppyDriveInUse = 26 ;
            int S_OF_FloppyDriveInUse = 27 ;
            int relevant_evt_OF_FloppyDriveInUse = 28 ;
            int failF_OF_FloppyDriveInUse = 29 ;
            int required_OF_HardDrive = 30 ;
            int already_S_OF_HardDrive = 31 ;
            int S_OF_HardDrive = 32 ;
            int relevant_evt_OF_HardDrive = 33 ;
            int failF_OF_HardDrive = 34 ;
            int required_OF_LossOfCooling = 35 ;
            int already_S_OF_LossOfCooling = 36 ;
            int S_OF_LossOfCooling = 37 ;
            int relevant_evt_OF_LossOfCooling = 38 ;
            int required_OF_ManipulationError = 39 ;
            int already_S_OF_ManipulationError = 40 ;
            int S_OF_ManipulationError = 41 ;
            int relevant_evt_OF_ManipulationError = 42 ;
            int failI_OF_ManipulationError = 43 ;
            int to_be_fired_OF_ManipulationError = 44 ;
            int already_standby_OF_ManipulationError = 45 ;
            int already_required_OF_ManipulationError = 46 ;
            int required_OF_Memory = 47 ;
            int already_S_OF_Memory = 48 ;
            int S_OF_Memory = 49 ;
            int relevant_evt_OF_Memory = 50 ;
            int failF_OF_Memory = 51 ;
            int required_OF_OR_1 = 52 ;
            int already_S_OF_OR_1 = 53 ;
            int S_OF_OR_1 = 54 ;
            int relevant_evt_OF_OR_1 = 55 ;
            int required_OF_PC_down = 56 ;
            int already_S_OF_PC_down = 57 ;
            int S_OF_PC_down = 58 ;
            int relevant_evt_OF_PC_down = 59 ;
            int required_OF_PowerSupply = 60 ;
            int already_S_OF_PowerSupply = 61 ;
            int S_OF_PowerSupply = 62 ;
            int relevant_evt_OF_PowerSupply = 63 ;
            int failF_OF_PowerSupply = 64 ;
            int required_OF_UE_1 = 65 ;
            int already_S_OF_UE_1 = 66 ;
            int S_OF_UE_1 = 67 ;
            int relevant_evt_OF_UE_1 = 68 ;
            int required_OF_Windows = 69 ;
            int already_S_OF_Windows = 70 ;
            int S_OF_Windows = 71 ;
            int relevant_evt_OF_Windows = 72 ;
            int failF_OF_Windows = 73 ;




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