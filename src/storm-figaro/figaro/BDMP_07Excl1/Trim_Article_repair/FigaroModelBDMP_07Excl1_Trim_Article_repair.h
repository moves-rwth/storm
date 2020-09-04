
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
        class FigaroProgram_BDMP_07Excl1_Trim_Article_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_07Excl1_Trim_Article_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_AND_1" , 0},
            	{"already_S_OF_AND_1" , 1},
            	{"S_OF_AND_1" , 2},
            	{"relevant_evt_OF_AND_1" , 3},
            	{"required_OF_AND_2" , 4},
            	{"already_S_OF_AND_2" , 5},
            	{"S_OF_AND_2" , 6},
            	{"relevant_evt_OF_AND_2" , 7},
            	{"required_OF_OR_1" , 8},
            	{"already_S_OF_OR_1" , 9},
            	{"S_OF_OR_1" , 10},
            	{"relevant_evt_OF_OR_1" , 11},
            	{"required_OF_UE_1" , 12},
            	{"already_S_OF_UE_1" , 13},
            	{"S_OF_UE_1" , 14},
            	{"relevant_evt_OF_UE_1" , 15},
            	{"required_OF_cptA" , 16},
            	{"already_S_OF_cptA" , 17},
            	{"S_OF_cptA" , 18},
            	{"relevant_evt_OF_cptA" , 19},
            	{"failF_OF_cptA" , 20},
            	{"required_OF_cptB" , 21},
            	{"already_S_OF_cptB" , 22},
            	{"S_OF_cptB" , 23},
            	{"relevant_evt_OF_cptB" , 24},
            	{"failF_OF_cptB" , 25},
            	{"required_OF_cptC" , 26},
            	{"already_S_OF_cptC" , 27},
            	{"S_OF_cptC" , 28},
            	{"relevant_evt_OF_cptC" , 29},
            	{"failF_OF_cptC" , 30},
            	{"required_OF_cptD" , 31},
            	{"already_S_OF_cptD" , 32},
            	{"S_OF_cptD" , 33},
            	{"relevant_evt_OF_cptD" , 34},
            	{"failF_OF_cptD" , 35}},

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
                    36 ,
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
            bool REINITIALISATION_OF_required_OF_OR_1 ;
            bool REINITIALISATION_OF_S_OF_OR_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_OR_1 ;
            bool REINITIALISATION_OF_required_OF_UE_1 ;
            bool REINITIALISATION_OF_S_OF_UE_1 ;
            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
            bool REINITIALISATION_OF_required_OF_cptA ;
            bool REINITIALISATION_OF_S_OF_cptA ;
            bool REINITIALISATION_OF_relevant_evt_OF_cptA ;
            bool REINITIALISATION_OF_required_OF_cptB ;
            bool REINITIALISATION_OF_S_OF_cptB ;
            bool REINITIALISATION_OF_relevant_evt_OF_cptB ;
            bool REINITIALISATION_OF_required_OF_cptC ;
            bool REINITIALISATION_OF_S_OF_cptC ;
            bool REINITIALISATION_OF_relevant_evt_OF_cptC ;
            bool REINITIALISATION_OF_required_OF_cptD ;
            bool REINITIALISATION_OF_S_OF_cptD ;
            bool REINITIALISATION_OF_relevant_evt_OF_cptD ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			std::string const calculate_required_OF_cptA = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_cptB = false;
			bool const failF_FROZEN_OF_cptC = false;
			bool const repairable_system_OF_OPTIONS = true;
			double const mu_OF_cptC = 0.1;
			double const lambda_OF_cptD = 0.0001;
			std::string const trigger_kind_OF_t_4 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_AND_1 = false;
			std::string const calculate_required_OF_cptB = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_cptD = false;
			double const mu_OF_cptD = 0.1;
			std::string const calculate_required_OF_AND_1 = "fn_fathers_and_trig";
			std::string const trigger_kind_OF_t_3 = "fn_fathers_and_trig";
			bool const No_repair_OF___ARBRE__EIRM = false;
			std::string const trimming_option_OF_OPTIONS = "according_to_article";
			std::string const trigger_kind_OF_t_1 = "fn_fathers_and_trig";
			bool const Default_OF___ARBRE__EIRM = true;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			std::string const calculate_required_OF_UE_1 = "always_true";
			bool const force_relevant_events_OF_cptC = false;
			std::string const calculate_required_OF_cptC = "fn_fathers_and_trig";
			bool const Trim_article_OF___ARBRE__EIRM = true;
			bool const force_relevant_events_OF_AND_2 = false;
			double const lambda_OF_cptA = 0.0001;
			bool const No_trim_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_AND_2 = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_cptD = false;
			bool const to_be_taken_into_account_OF_UE_1 = true;
			std::string const calculate_required_OF_cptD = "fn_fathers_and_trig";
			double const lambda_OF_cptB = 0.0001;
			bool const failF_FROZEN_OF_cptA = false;
			double const mu_OF_cptA = 0.1;
			bool const force_relevant_events_OF_OR_1 = false;
			bool const failF_FROZEN_OF_cptB = false;
			bool const force_relevant_events_OF_UE_1 = true;
			double const mu_OF_cptB = 0.1;
			bool const trimming_OF_OPTIONS = true;
			std::string const calculate_required_OF_OR_1 = "fn_fathers_and_trig";
			double const lambda_OF_cptC = 0.0001;
			bool const Profil1_OF___ARBRE__EIRM = false;
			bool const force_relevant_events_OF_cptA = false;
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_cptA;
            bool FIRE_xx11_OF_cptA;
            bool FIRE_xx10_OF_cptB;
            bool FIRE_xx11_OF_cptB;
            bool FIRE_xx10_OF_cptC;
            bool FIRE_xx11_OF_cptC;
            bool FIRE_xx10_OF_cptD;
            bool FIRE_xx11_OF_cptD;

            int required_OF_AND_1 = 0 ;
            int already_S_OF_AND_1 = 1 ;
            int S_OF_AND_1 = 2 ;
            int relevant_evt_OF_AND_1 = 3 ;
            int required_OF_AND_2 = 4 ;
            int already_S_OF_AND_2 = 5 ;
            int S_OF_AND_2 = 6 ;
            int relevant_evt_OF_AND_2 = 7 ;
            int required_OF_OR_1 = 8 ;
            int already_S_OF_OR_1 = 9 ;
            int S_OF_OR_1 = 10 ;
            int relevant_evt_OF_OR_1 = 11 ;
            int required_OF_UE_1 = 12 ;
            int already_S_OF_UE_1 = 13 ;
            int S_OF_UE_1 = 14 ;
            int relevant_evt_OF_UE_1 = 15 ;
            int required_OF_cptA = 16 ;
            int already_S_OF_cptA = 17 ;
            int S_OF_cptA = 18 ;
            int relevant_evt_OF_cptA = 19 ;
            int failF_OF_cptA = 20 ;
            int required_OF_cptB = 21 ;
            int already_S_OF_cptB = 22 ;
            int S_OF_cptB = 23 ;
            int relevant_evt_OF_cptB = 24 ;
            int failF_OF_cptB = 25 ;
            int required_OF_cptC = 26 ;
            int already_S_OF_cptC = 27 ;
            int S_OF_cptC = 28 ;
            int relevant_evt_OF_cptC = 29 ;
            int failF_OF_cptC = 30 ;
            int required_OF_cptD = 31 ;
            int already_S_OF_cptD = 32 ;
            int S_OF_cptD = 33 ;
            int relevant_evt_OF_cptD = 34 ;
            int failF_OF_cptD = 35 ;




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