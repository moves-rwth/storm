
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
        class FigaroProgram_figaro_Miniplant: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_figaro_Miniplant(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"null_production_OF_Backup_1" , 0},
            	{"dequeue_OF_Backup_1" , 1},
            	{"fail_OF_Backup_1" , 2},
            	{"null_production_OF_Block_1" , 3},
            	{"dequeue_OF_Block_1" , 4},
            	{"fail_OF_Block_1" , 5},
            	{"null_production_OF_Block_2" , 6},
            	{"dequeue_OF_Block_2" , 7},
            	{"fail_OF_Block_2" , 8},
            	{"null_production_OF_Block_3" , 9},
            	{"dequeue_OF_Block_3" , 10},
            	{"fail_OF_Block_3" , 11},
            	{"null_production_OF_Block_4" , 12},
            	{"dequeue_OF_Block_4" , 13},
            	{"fail_OF_Block_4" , 14},
            	{"null_production_OF_SS1" , 15},
            	{"null_production_OF_SS2" , 16},
            	{"null_production_OF_min_1" , 17},
            	{"free_OF_rep_1" , 18},
            	{"dequeue_OF_rep_1" , 19}},

//            std::map<std::string, size_t> mFigaroelementfailureindex =
                    {  { "exp0",0}},

//            std::map<std::string, size_t> mFigarofloatelementindex =
                     {  
            	{"capacity_OF_Backup_1" , 0},
            	{"capacity_OF_Block_1" , 1},
            	{"capacity_OF_Block_2" , 2},
            	{"capacity_OF_Block_3" , 3},
            	{"capacity_OF_Block_4" , 4},
            	{"capacity_OF_SS1" , 5},
            	{"potential_capacity_OF_SS1" , 6},
            	{"capacity_OF_SS2" , 7},
            	{"potential_capacity_OF_SS2" , 8},
            	{"capacity_OF_min_1" , 9}},

//            std::map<std::string, size_t> mFigarointelementindex =
                     {  
            	{"rank_OF_Backup_1" , 0},
            	{"rank_OF_Block_1" , 1},
            	{"rank_OF_Block_2" , 2},
            	{"rank_OF_Block_3" , 3},
            	{"rank_OF_Block_4" , 4},
            	{"max_rank_OF_rep_1" , 5}},

//            std::map<std::string, size_t> mFigaroenumelementindex =
                     {  
            	{"state_OF_Backup_1" , 0},
            	{"state_OF_Block_1" , 1},
            	{"state_OF_Block_2" , 2},
            	{"state_OF_Block_3" , 3},
            	{"state_OF_Block_4" , 4}},

//            std::map<std::string, size_t> failure_variable_names =
                    {  "exp0"},

//            std::set<std::string> enum_variables_names =
                     {  
            	"state_OF_Backup_1" ,
            	"state_OF_Block_1" ,
            	"state_OF_Block_2" ,
            	"state_OF_Block_3" ,
            	"state_OF_Block_4" },

//            std::set<std::string> float_variables_names =
                     {  
            	"capacity_OF_Backup_1" ,
            	"capacity_OF_Block_1" ,
            	"capacity_OF_Block_2" ,
            	"capacity_OF_Block_3" ,
            	"capacity_OF_Block_4" ,
            	"capacity_OF_SS1" ,
            	"potential_capacity_OF_SS1" ,
            	"capacity_OF_SS2" ,
            	"potential_capacity_OF_SS2" ,
            	"capacity_OF_min_1" },


//            std::string const topevent=
                    "exp0",
//            static int const numBoolState = 
                    20 ,
//             numBoolFailureState = 
                    1 ,
//            static int const numFloatState = 
                    10 ,
//            static int const numIntState = 
                    6 ,
//            static int const numEnumState = 
                    5 ,
//            bool ins_transition_found = 
                     false){} 

            /* ---------- CODING ENUMERATED VARIABLES STATES ------------ */
            enum enum_status {  working = 0, standby = 1, under_repair = 2, waiting_for_rep = 3, required = 4};

//            std::array<bool, numBoolState> boolState;
//            std::array<bool, numBoolState> backupBoolState;
//            std::array<float, numFloatState> floatState;
//            std::array<float, numFloatState> backupFloatState;
//            std::array<int, numIntState> intState;
//            std::array<int, numIntState> backupIntState;
//            std::array<int, numEnumState> enumState;
//            std::array<int, numEnumState> backupEnumState;
            bool REINITIALISATION_OF_capacity_OF_Backup_1 ;
            bool REINITIALISATION_OF_null_production_OF_Backup_1 ;
            bool REINITIALISATION_OF_dequeue_OF_Backup_1 ;
            bool REINITIALISATION_OF_capacity_OF_Block_1 ;
            bool REINITIALISATION_OF_null_production_OF_Block_1 ;
            bool REINITIALISATION_OF_dequeue_OF_Block_1 ;
            bool REINITIALISATION_OF_capacity_OF_Block_2 ;
            bool REINITIALISATION_OF_null_production_OF_Block_2 ;
            bool REINITIALISATION_OF_dequeue_OF_Block_2 ;
            bool REINITIALISATION_OF_capacity_OF_Block_3 ;
            bool REINITIALISATION_OF_null_production_OF_Block_3 ;
            bool REINITIALISATION_OF_dequeue_OF_Block_3 ;
            bool REINITIALISATION_OF_capacity_OF_Block_4 ;
            bool REINITIALISATION_OF_null_production_OF_Block_4 ;
            bool REINITIALISATION_OF_dequeue_OF_Block_4 ;
            bool REINITIALISATION_OF_capacity_OF_SS1 ;
            bool REINITIALISATION_OF_null_production_OF_SS1 ;
            bool REINITIALISATION_OF_potential_capacity_OF_SS1 ;
            bool REINITIALISATION_OF_capacity_OF_SS2 ;
            bool REINITIALISATION_OF_null_production_OF_SS2 ;
            bool REINITIALISATION_OF_potential_capacity_OF_SS2 ;
            bool REINITIALISATION_OF_capacity_OF_min_1 ;
            bool REINITIALISATION_OF_null_production_OF_min_1 ;
            bool REINITIALISATION_OF_max_rank_OF_rep_1 ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			double const nominal_capacity_OF_Block_1 = 30;
			double const lambda_OF_Block_2 = 0.0001;
			double const lambda_OF_Block_4 = 0.0001;
			double const lambda_OF_Block_1 = 0.0001;
			double const lambda_OF_Backup_1 = 0.0001;
			double const mu_OF_Block_2 = 0.1;
			double const nominal_capacity_OF_Block_3 = 30;
			double const mu_OF_Block_4 = 0.1;
			double const mu_OF_Block_1 = 0.1;
			double const mu_OF_Backup_1 = 0.1;
			bool const Profil1_OF___ARBRE__EIRM = true;
			double const functioning_threshold_OF_SS1 = 0;
			double const nominal_capacity_OF_Block_2 = 70;
			double const functioning_threshold_OF_SS2 = 0;
			double const gamma_OF_Backup_1 = 0.01;
			double const nominal_capacity_OF_Block_4 = 70;
			double const lambda_OF_Block_3 = 0.0001;
			double const nominal_capacity_OF_Backup_1 = 70;
			double const mu_OF_Block_3 = 0.1;
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_fail_in_op_OF_Backup_1;
            bool FIRE_xx1_OF_Backup_1;
            bool FIRE_xx2_OF_Backup_1_INS_2;
            bool FIRE_xx2_OF_Backup_1_INS_3;
            bool FIRE_fail_in_op_OF_Block_1;
            bool FIRE_xx1_OF_Block_1;
            bool FIRE_fail_in_op_OF_Block_2;
            bool FIRE_xx1_OF_Block_2;
            bool FIRE_fail_in_op_OF_Block_3;
            bool FIRE_xx1_OF_Block_3;
            bool FIRE_fail_in_op_OF_Block_4;
            bool FIRE_xx1_OF_Block_4;

            int null_production_OF_Backup_1 = 0 ;
            int dequeue_OF_Backup_1 = 1 ;
            int fail_OF_Backup_1 = 2 ;
            int null_production_OF_Block_1 = 3 ;
            int dequeue_OF_Block_1 = 4 ;
            int fail_OF_Block_1 = 5 ;
            int null_production_OF_Block_2 = 6 ;
            int dequeue_OF_Block_2 = 7 ;
            int fail_OF_Block_2 = 8 ;
            int null_production_OF_Block_3 = 9 ;
            int dequeue_OF_Block_3 = 10 ;
            int fail_OF_Block_3 = 11 ;
            int null_production_OF_Block_4 = 12 ;
            int dequeue_OF_Block_4 = 13 ;
            int fail_OF_Block_4 = 14 ;
            int null_production_OF_SS1 = 15 ;
            int null_production_OF_SS2 = 16 ;
            int null_production_OF_min_1 = 17 ;
            int free_OF_rep_1 = 18 ;
            int dequeue_OF_rep_1 = 19 ;

            int capacity_OF_Backup_1 = 0 ;
            int capacity_OF_Block_1 = 1 ;
            int capacity_OF_Block_2 = 2 ;
            int capacity_OF_Block_3 = 3 ;
            int capacity_OF_Block_4 = 4 ;
            int capacity_OF_SS1 = 5 ;
            int potential_capacity_OF_SS1 = 6 ;
            int capacity_OF_SS2 = 7 ;
            int potential_capacity_OF_SS2 = 8 ;
            int capacity_OF_min_1 = 9 ;

            int rank_OF_Backup_1 = 0 ;
            int rank_OF_Block_1 = 1 ;
            int rank_OF_Block_2 = 2 ;
            int rank_OF_Block_3 = 3 ;
            int rank_OF_Block_4 = 4 ;
            int max_rank_OF_rep_1 = 5 ;

            int state_OF_Backup_1 = 0 ;
            int state_OF_Block_1 = 1 ;
            int state_OF_Block_2 = 2 ;
            int state_OF_Block_3 = 3 ;
            int state_OF_Block_4 = 4 ;

            int exp0 = 0 ;


            /* ---------- DECLARATION OF FUNCTIONS ------------ */
            void init();
            void saveCurrentState();
            void printState();
            void fireOccurrence(int numFire);
            std::vector<std::tuple<int, double, std::string, int>> showFireableOccurrences();
            void runOnceInteractionStep_default_step();
            void runOnceInteractionStep_compute_max_rank();
            void runOnceInteractionStep_rep_management();
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