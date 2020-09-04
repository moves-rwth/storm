
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
        class FigaroProgram_figaro_RBD: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_figaro_RBD(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"not_linked_OF_b1" , 0},
            	{"fail_in_operation_OF_b1" , 1},
            	{"fail_to_start_OF_b1" , 2},
            	{"function_ok_OF_b1" , 3},
            	{"function_known_OF_b1" , 4},
            	{"not_linked_OF_b2" , 5},
            	{"fail_in_operation_OF_b2" , 6},
            	{"fail_to_start_OF_b2" , 7},
            	{"function_ok_OF_b2" , 8},
            	{"function_known_OF_b2" , 9},
            	{"not_linked_OF_b3" , 10},
            	{"fail_in_operation_OF_b3" , 11},
            	{"fail_to_start_OF_b3" , 12},
            	{"function_ok_OF_b3" , 13},
            	{"function_known_OF_b3" , 14},
            	{"not_linked_OF_b4" , 15},
            	{"fail_in_operation_OF_b4" , 16},
            	{"fail_to_start_OF_b4" , 17},
            	{"function_ok_OF_b4" , 18},
            	{"function_known_OF_b4" , 19},
            	{"not_linked_OF_b5" , 20},
            	{"fail_in_operation_OF_b5" , 21},
            	{"fail_to_start_OF_b5" , 22},
            	{"function_ok_OF_b5" , 23},
            	{"function_known_OF_b5" , 24},
            	{"not_linked_OF_de_1" , 25},
            	{"not_linked_OF_ds_1" , 26},
            	{"not_linked_OF_gate_2_4" , 27},
            	{"not_linked_OF_n_1" , 28},
            	{"not_linked_OF_n_2" , 29},
            	{"not_linked_OF_sb1" , 30},
            	{"fail_in_operation_OF_sb1" , 31},
            	{"fail_to_start_OF_sb1" , 32},
            	{"function_ok_OF_sb1" , 33},
            	{"function_known_OF_sb1" , 34},
            	{"not_linked_OF_sb2" , 35},
            	{"fail_in_operation_OF_sb2" , 36},
            	{"fail_to_start_OF_sb2" , 37},
            	{"function_ok_OF_sb2" , 38},
            	{"function_known_OF_sb2" , 39},
            	{"not_linked_OF_sb3" , 40},
            	{"fail_in_operation_OF_sb3" , 41},
            	{"fail_to_start_OF_sb3" , 42},
            	{"function_ok_OF_sb3" , 43},
            	{"function_known_OF_sb3" , 44},
            	{"not_linked_OF_sb4" , 45},
            	{"fail_in_operation_OF_sb4" , 46},
            	{"fail_to_start_OF_sb4" , 47},
            	{"function_ok_OF_sb4" , 48},
            	{"function_known_OF_sb4" , 49}},

//            std::map<std::string, size_t> mFigaroelementfailureindex =
                    {  { "exp0",0}},

//            std::map<std::string, size_t> mFigarofloatelementindex =
                     { },

//            std::map<std::string, size_t> mFigarointelementindex =
                     {  
            	{"nb_avail_repairmen_OF_rep3_4" , 0},
            	{"nb_avail_repairmen_OF_rep_1" , 1},
            	{"nb_avail_repairmen_OF_rep_2" , 2},
            	{"nb_avail_repairmen_OF_rep_5" , 3}},

//            std::map<std::string, size_t> mFigaroenumelementindex =
                     {  
            	{"state_OF_b1" , 0},
            	{"state_OF_b2" , 1},
            	{"state_OF_b3" , 2},
            	{"state_OF_b4" , 3},
            	{"state_OF_b5" , 4},
            	{"state_OF_sb1" , 5},
            	{"state_OF_sb2" , 6},
            	{"state_OF_sb3" , 7},
            	{"state_OF_sb4" , 8}},

//            std::map<std::string, size_t> failure_variable_names =
                    {  "exp0"},

//            std::set<std::string> enum_variables_names =
                     {  
            	"state_OF_b1" ,
            	"state_OF_b2" ,
            	"state_OF_b3" ,
            	"state_OF_b4" ,
            	"state_OF_b5" ,
            	"state_OF_sb1" ,
            	"state_OF_sb2" ,
            	"state_OF_sb3" ,
            	"state_OF_sb4" },

//            std::set<std::string> float_variables_names =
                     { },


//            std::string const topevent=
                    "exp0",
//            static int const numBoolState = 
                    50 ,
//             numBoolFailureState = 
                    1 ,
//            static int const numFloatState = 
                    0 ,
//            static int const numIntState = 
                    4 ,
//            static int const numEnumState = 
                    9 ,
//            bool ins_transition_found = 
                     false){} 

            /* ---------- CODING ENUMERATED VARIABLES STATES ------------ */
            enum enum_status {  standby = 0, start_demand = 1, in_operation = 2, waiting_for_repair = 3, under_repair = 4};

//            std::array<bool, numBoolState> boolState;
//            std::array<bool, numBoolState> backupBoolState;
//            std::array<float, numFloatState> floatState;
//            std::array<float, numFloatState> backupFloatState;
//            std::array<int, numIntState> intState;
//            std::array<int, numIntState> backupIntState;
//            std::array<int, numEnumState> enumState;
//            std::array<int, numEnumState> backupEnumState;
            bool REINITIALISATION_OF_not_linked_OF_b1 ;
            bool REINITIALISATION_OF_function_ok_OF_b1 ;
            bool REINITIALISATION_OF_function_known_OF_b1 ;
            bool REINITIALISATION_OF_not_linked_OF_b2 ;
            bool REINITIALISATION_OF_function_ok_OF_b2 ;
            bool REINITIALISATION_OF_function_known_OF_b2 ;
            bool REINITIALISATION_OF_not_linked_OF_b3 ;
            bool REINITIALISATION_OF_function_ok_OF_b3 ;
            bool REINITIALISATION_OF_function_known_OF_b3 ;
            bool REINITIALISATION_OF_not_linked_OF_b4 ;
            bool REINITIALISATION_OF_function_ok_OF_b4 ;
            bool REINITIALISATION_OF_function_known_OF_b4 ;
            bool REINITIALISATION_OF_not_linked_OF_b5 ;
            bool REINITIALISATION_OF_function_ok_OF_b5 ;
            bool REINITIALISATION_OF_function_known_OF_b5 ;
            bool REINITIALISATION_OF_not_linked_OF_de_1 ;
            bool REINITIALISATION_OF_not_linked_OF_ds_1 ;
            bool REINITIALISATION_OF_not_linked_OF_gate_2_4 ;
            bool REINITIALISATION_OF_not_linked_OF_n_1 ;
            bool REINITIALISATION_OF_not_linked_OF_n_2 ;
            bool REINITIALISATION_OF_not_linked_OF_sb1 ;
            bool REINITIALISATION_OF_function_ok_OF_sb1 ;
            bool REINITIALISATION_OF_function_known_OF_sb1 ;
            bool REINITIALISATION_OF_not_linked_OF_sb2 ;
            bool REINITIALISATION_OF_function_ok_OF_sb2 ;
            bool REINITIALISATION_OF_function_known_OF_sb2 ;
            bool REINITIALISATION_OF_not_linked_OF_sb3 ;
            bool REINITIALISATION_OF_function_ok_OF_sb3 ;
            bool REINITIALISATION_OF_function_known_OF_sb3 ;
            bool REINITIALISATION_OF_not_linked_OF_sb4 ;
            bool REINITIALISATION_OF_function_ok_OF_sb4 ;
            bool REINITIALISATION_OF_function_known_OF_sb4 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			double const lambda_OF_b2 = 2e-06;
			int const min_nb_for_failure_OF_gate_2_4 = 3;
			double const gamma_OF_b1 = 0.001;
			double const mu_OF_b5 = 1;
			double const mu_OF_sb2 = 0.02;
			double const gamma_OF_sb1 = 0.01;
			double const gamma_OF_sb3 = 0.02;
			double const mu_OF_b3 = 0.01;
			double const lambda_OF_sb2 = 0.0001;
			double const lambda_OF_b1 = 1e-06;
			double const gamma_OF_b2 = 0.001;
			double const lambda_OF_sb4 = 0.001;
			double const mu_OF_sb4 = 0.02;
			double const lambda_OF_b3 = 5e-05;
			double const lambda_OF_sb3 = 0.001;
			double const gamma_OF_sb2 = 0.01;
			double const gamma_OF_b4 = 0.001;
			bool const Profil1_OF___ARBRE__EIRM = true;
			double const gamma_OF_sb4 = 0.02;
			double const mu_OF_b1 = 0.002;
			double const mu_OF_sb1 = 0.02;
			double const gamma_OF_b5 = 0.001;
			double const lambda_OF_b4 = 1e-05;
			double const gamma_OF_b3 = 0.001;
			int const min_nb_for_success_OF_gate_2_4 = 2;
			double const lambda_OF_sb1 = 0.0001;
			double const mu_OF_b2 = 0.005;
			double const lambda_OF_b5 = 0.002;
			double const mu_OF_b4 = 0.02;
			double const mu_OF_sb3 = 0.02;
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_occ_fail_to_start_OF_b1_INS_0;
            bool FIRE_occ_fail_to_start_OF_b1_INS_1;
            bool FIRE_occ_fail_in_operation_OF_b1;
            bool FIRE_occ_repair_OF_b1;
            bool FIRE_occ_fail_to_start_OF_b2_INS_4;
            bool FIRE_occ_fail_to_start_OF_b2_INS_5;
            bool FIRE_occ_fail_in_operation_OF_b2;
            bool FIRE_occ_repair_OF_b2;
            bool FIRE_occ_fail_to_start_OF_b3_INS_8;
            bool FIRE_occ_fail_to_start_OF_b3_INS_9;
            bool FIRE_occ_fail_in_operation_OF_b3;
            bool FIRE_occ_repair_OF_b3;
            bool FIRE_occ_fail_to_start_OF_b4_INS_12;
            bool FIRE_occ_fail_to_start_OF_b4_INS_13;
            bool FIRE_occ_fail_in_operation_OF_b4;
            bool FIRE_occ_repair_OF_b4;
            bool FIRE_occ_fail_to_start_OF_b5_INS_16;
            bool FIRE_occ_fail_to_start_OF_b5_INS_17;
            bool FIRE_occ_fail_in_operation_OF_b5;
            bool FIRE_occ_repair_OF_b5;
            bool FIRE_occ_fail_to_start_OF_sb1_INS_20;
            bool FIRE_occ_fail_to_start_OF_sb1_INS_21;
            bool FIRE_occ_fail_in_operation_OF_sb1;
            bool FIRE_occ_repair_OF_sb1;
            bool FIRE_occ_fail_to_start_OF_sb2_INS_24;
            bool FIRE_occ_fail_to_start_OF_sb2_INS_25;
            bool FIRE_occ_fail_in_operation_OF_sb2;
            bool FIRE_occ_repair_OF_sb2;
            bool FIRE_occ_fail_to_start_OF_sb3_INS_28;
            bool FIRE_occ_fail_to_start_OF_sb3_INS_29;
            bool FIRE_occ_fail_in_operation_OF_sb3;
            bool FIRE_occ_repair_OF_sb3;
            bool FIRE_occ_fail_to_start_OF_sb4_INS_32;
            bool FIRE_occ_fail_to_start_OF_sb4_INS_33;
            bool FIRE_occ_fail_in_operation_OF_sb4;
            bool FIRE_occ_repair_OF_sb4;

            int not_linked_OF_b1 = 0 ;
            int fail_in_operation_OF_b1 = 1 ;
            int fail_to_start_OF_b1 = 2 ;
            int function_ok_OF_b1 = 3 ;
            int function_known_OF_b1 = 4 ;
            int not_linked_OF_b2 = 5 ;
            int fail_in_operation_OF_b2 = 6 ;
            int fail_to_start_OF_b2 = 7 ;
            int function_ok_OF_b2 = 8 ;
            int function_known_OF_b2 = 9 ;
            int not_linked_OF_b3 = 10 ;
            int fail_in_operation_OF_b3 = 11 ;
            int fail_to_start_OF_b3 = 12 ;
            int function_ok_OF_b3 = 13 ;
            int function_known_OF_b3 = 14 ;
            int not_linked_OF_b4 = 15 ;
            int fail_in_operation_OF_b4 = 16 ;
            int fail_to_start_OF_b4 = 17 ;
            int function_ok_OF_b4 = 18 ;
            int function_known_OF_b4 = 19 ;
            int not_linked_OF_b5 = 20 ;
            int fail_in_operation_OF_b5 = 21 ;
            int fail_to_start_OF_b5 = 22 ;
            int function_ok_OF_b5 = 23 ;
            int function_known_OF_b5 = 24 ;
            int not_linked_OF_de_1 = 25 ;
            int not_linked_OF_ds_1 = 26 ;
            int not_linked_OF_gate_2_4 = 27 ;
            int not_linked_OF_n_1 = 28 ;
            int not_linked_OF_n_2 = 29 ;
            int not_linked_OF_sb1 = 30 ;
            int fail_in_operation_OF_sb1 = 31 ;
            int fail_to_start_OF_sb1 = 32 ;
            int function_ok_OF_sb1 = 33 ;
            int function_known_OF_sb1 = 34 ;
            int not_linked_OF_sb2 = 35 ;
            int fail_in_operation_OF_sb2 = 36 ;
            int fail_to_start_OF_sb2 = 37 ;
            int function_ok_OF_sb2 = 38 ;
            int function_known_OF_sb2 = 39 ;
            int not_linked_OF_sb3 = 40 ;
            int fail_in_operation_OF_sb3 = 41 ;
            int fail_to_start_OF_sb3 = 42 ;
            int function_ok_OF_sb3 = 43 ;
            int function_known_OF_sb3 = 44 ;
            int not_linked_OF_sb4 = 45 ;
            int fail_in_operation_OF_sb4 = 46 ;
            int fail_to_start_OF_sb4 = 47 ;
            int function_ok_OF_sb4 = 48 ;
            int function_known_OF_sb4 = 49 ;


            int nb_avail_repairmen_OF_rep3_4 = 0 ;
            int nb_avail_repairmen_OF_rep_1 = 1 ;
            int nb_avail_repairmen_OF_rep_2 = 2 ;
            int nb_avail_repairmen_OF_rep_5 = 3 ;

            int state_OF_b1 = 0 ;
            int state_OF_b2 = 1 ;
            int state_OF_b3 = 2 ;
            int state_OF_b4 = 3 ;
            int state_OF_b5 = 4 ;
            int state_OF_sb1 = 5 ;
            int state_OF_sb2 = 6 ;
            int state_OF_sb3 = 7 ;
            int state_OF_sb4 = 8 ;

            int exp0 = 0 ;


            /* ---------- DECLARATION OF FUNCTIONS ------------ */
            void init();
            void saveCurrentState();
            void printState();
            void fireOccurrence(int numFire);
            std::vector<std::tuple<int, double, std::string, int>> showFireableOccurrences();
            void runOnceInteractionStep_default_step();
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