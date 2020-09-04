
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
        class FigaroProgram_BDMP_21_Remote_Access_Server_Security_No_trim_No_repair: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram_BDMP_21_Remote_Access_Server_Security_No_trim_No_repair(): FigaroProgram(//            std::map<std::string, size_t> mFigaroboolelementindex =
                    {  
            	{"required_OF_Authentication_with_password" , 0},
            	{"already_S_OF_Authentication_with_password" , 1},
            	{"S_OF_Authentication_with_password" , 2},
            	{"relevant_evt_OF_Authentication_with_password" , 3},
            	{"required_OF_Bruteforce" , 4},
            	{"already_S_OF_Bruteforce" , 5},
            	{"S_OF_Bruteforce" , 6},
            	{"relevant_evt_OF_Bruteforce" , 7},
            	{"failF_OF_Bruteforce" , 8},
            	{"required_OF_Exploit_vulnerability" , 9},
            	{"already_S_OF_Exploit_vulnerability" , 10},
            	{"S_OF_Exploit_vulnerability" , 11},
            	{"relevant_evt_OF_Exploit_vulnerability" , 12},
            	{"failF_OF_Exploit_vulnerability" , 13},
            	{"required_OF_Find_vulnerability" , 14},
            	{"already_S_OF_Find_vulnerability" , 15},
            	{"S_OF_Find_vulnerability" , 16},
            	{"relevant_evt_OF_Find_vulnerability" , 17},
            	{"failF_OF_Find_vulnerability" , 18},
            	{"required_OF_Logged_into_the_RAS" , 19},
            	{"already_S_OF_Logged_into_the_RAS" , 20},
            	{"S_OF_Logged_into_the_RAS" , 21},
            	{"relevant_evt_OF_Logged_into_the_RAS" , 22},
            	{"required_OF_RAS_access_granted" , 23},
            	{"already_S_OF_RAS_access_granted" , 24},
            	{"S_OF_RAS_access_granted" , 25},
            	{"relevant_evt_OF_RAS_access_granted" , 26},
            	{"required_OF_RAS_ownership" , 27},
            	{"already_S_OF_RAS_ownership" , 28},
            	{"S_OF_RAS_ownership" , 29},
            	{"relevant_evt_OF_RAS_ownership" , 30},
            	{"required_OF_Social_engineering" , 31},
            	{"already_S_OF_Social_engineering" , 32},
            	{"S_OF_Social_engineering" , 33},
            	{"relevant_evt_OF_Social_engineering" , 34},
            	{"failF_OF_Social_engineering" , 35},
            	{"required_OF_Vulnerability_found_and_exploited" , 36},
            	{"already_S_OF_Vulnerability_found_and_exploited" , 37},
            	{"S_OF_Vulnerability_found_and_exploited" , 38},
            	{"relevant_evt_OF_Vulnerability_found_and_exploited" , 39},
            	{"required_OF_Wardialing" , 40},
            	{"already_S_OF_Wardialing" , 41},
            	{"S_OF_Wardialing" , 42},
            	{"relevant_evt_OF_Wardialing" , 43},
            	{"failF_OF_Wardialing" , 44}},

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
                    45 ,
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
            bool REINITIALISATION_OF_required_OF_Authentication_with_password ;
            bool REINITIALISATION_OF_S_OF_Authentication_with_password ;
            bool REINITIALISATION_OF_relevant_evt_OF_Authentication_with_password ;
            bool REINITIALISATION_OF_required_OF_Bruteforce ;
            bool REINITIALISATION_OF_S_OF_Bruteforce ;
            bool REINITIALISATION_OF_relevant_evt_OF_Bruteforce ;
            bool REINITIALISATION_OF_required_OF_Exploit_vulnerability ;
            bool REINITIALISATION_OF_S_OF_Exploit_vulnerability ;
            bool REINITIALISATION_OF_relevant_evt_OF_Exploit_vulnerability ;
            bool REINITIALISATION_OF_required_OF_Find_vulnerability ;
            bool REINITIALISATION_OF_S_OF_Find_vulnerability ;
            bool REINITIALISATION_OF_relevant_evt_OF_Find_vulnerability ;
            bool REINITIALISATION_OF_required_OF_Logged_into_the_RAS ;
            bool REINITIALISATION_OF_S_OF_Logged_into_the_RAS ;
            bool REINITIALISATION_OF_relevant_evt_OF_Logged_into_the_RAS ;
            bool REINITIALISATION_OF_required_OF_RAS_access_granted ;
            bool REINITIALISATION_OF_S_OF_RAS_access_granted ;
            bool REINITIALISATION_OF_relevant_evt_OF_RAS_access_granted ;
            bool REINITIALISATION_OF_required_OF_RAS_ownership ;
            bool REINITIALISATION_OF_S_OF_RAS_ownership ;
            bool REINITIALISATION_OF_relevant_evt_OF_RAS_ownership ;
            bool REINITIALISATION_OF_required_OF_Social_engineering ;
            bool REINITIALISATION_OF_S_OF_Social_engineering ;
            bool REINITIALISATION_OF_relevant_evt_OF_Social_engineering ;
            bool REINITIALISATION_OF_required_OF_Vulnerability_found_and_exploited ;
            bool REINITIALISATION_OF_S_OF_Vulnerability_found_and_exploited ;
            bool REINITIALISATION_OF_relevant_evt_OF_Vulnerability_found_and_exploited ;
            bool REINITIALISATION_OF_required_OF_Wardialing ;
            bool REINITIALISATION_OF_S_OF_Wardialing ;
            bool REINITIALISATION_OF_relevant_evt_OF_Wardialing ;
            
		/* ---------- DECLARATION OF CONSTANTS ------------ */
			double const mu_OF_Social_engineering = 0.1;
			std::string const trigger_kind_OF_t_3 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_Exploit_vulnerability = false;
			std::string const calculate_required_OF_Find_vulnerability = "fn_fathers_and_trig";
			bool const to_be_taken_into_account_OF_RAS_ownership = true;
			bool const force_relevant_events_OF_Vulnerability_found_and_exploited = false;
			std::string const trigger_kind_OF_t_2 = "fn_fathers_and_trig";
			bool const failF_FROZEN_OF_Bruteforce = false;
			double const lambda_OF_Exploit_vulnerability = 0.0001;
			double const mu_OF_Find_vulnerability = 0.1;
			bool const Standard_OF___ARBRE__EIRM = false;
			bool const no_trim_OF___ARBRE__EIRM = true;
			bool const force_relevant_events_OF_RAS_access_granted = false;
			bool const repairable_system_OF_OPTIONS = false;
			bool const Weakpassword_OF___ARBRE__EIRM = false;
			double const lambda_OF_Bruteforce = 0.0001;
			std::string const calculate_required_OF_Vulnerability_found_and_exploited = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_RAS_ownership = true;
			bool const Default_OF___ARBRE__EIRM = true;
			bool const force_relevant_events_OF_Exploit_vulnerability = false;
			std::string const calculate_required_OF_RAS_access_granted = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_Authentication_with_password = false;
			bool const failF_FROZEN_OF_Wardialing = false;
			bool const force_relevant_events_OF_Bruteforce = false;
			std::string const calculate_required_OF_Exploit_vulnerability = "fn_fathers_and_trig";
			double const mu_OF_Exploit_vulnerability = 0.1;
			bool const failF_FROZEN_OF_Social_engineering = false;
			std::string const calculate_required_OF_Authentication_with_password = "fn_fathers_and_trig";
			double const lambda_OF_Social_engineering = 0.0001;
			double const lambda_OF_Wardialing = 0.0001;
			std::string const trimming_option_OF_OPTIONS = "maximum";
			bool const failF_FROZEN_OF_Find_vulnerability = false;
			std::string const calculate_required_OF_Bruteforce = "fn_fathers_and_trig";
			double const mu_OF_Bruteforce = 0.1;
			bool const Trim_article_OF___ARBRE__EIRM = false;
			std::string const calculate_required_OF_RAS_ownership = "always_true";
			double const lambda_OF_Find_vulnerability = 0.0001;
			bool const force_relevant_events_OF_Logged_into_the_RAS = false;
			bool const force_relevant_events_OF_Social_engineering = false;
			double const mu_OF_Wardialing = 0.1;
			bool const force_relevant_events_OF_Wardialing = false;
			bool const no_repair_OF___ARBRE__EIRM = true;
			std::string const calculate_required_OF_Logged_into_the_RAS = "fn_fathers_and_trig";
			std::string const calculate_required_OF_Social_engineering = "fn_fathers_and_trig";
			std::string const calculate_required_OF_Wardialing = "fn_fathers_and_trig";
			bool const force_relevant_events_OF_Find_vulnerability = false;
			bool const trimming_OF_OPTIONS = false;
		
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx10_OF_Bruteforce;
            bool FIRE_xx10_OF_Exploit_vulnerability;
            bool FIRE_xx10_OF_Find_vulnerability;
            bool FIRE_xx10_OF_Social_engineering;
            bool FIRE_xx10_OF_Wardialing;

            int required_OF_Authentication_with_password = 0 ;
            int already_S_OF_Authentication_with_password = 1 ;
            int S_OF_Authentication_with_password = 2 ;
            int relevant_evt_OF_Authentication_with_password = 3 ;
            int required_OF_Bruteforce = 4 ;
            int already_S_OF_Bruteforce = 5 ;
            int S_OF_Bruteforce = 6 ;
            int relevant_evt_OF_Bruteforce = 7 ;
            int failF_OF_Bruteforce = 8 ;
            int required_OF_Exploit_vulnerability = 9 ;
            int already_S_OF_Exploit_vulnerability = 10 ;
            int S_OF_Exploit_vulnerability = 11 ;
            int relevant_evt_OF_Exploit_vulnerability = 12 ;
            int failF_OF_Exploit_vulnerability = 13 ;
            int required_OF_Find_vulnerability = 14 ;
            int already_S_OF_Find_vulnerability = 15 ;
            int S_OF_Find_vulnerability = 16 ;
            int relevant_evt_OF_Find_vulnerability = 17 ;
            int failF_OF_Find_vulnerability = 18 ;
            int required_OF_Logged_into_the_RAS = 19 ;
            int already_S_OF_Logged_into_the_RAS = 20 ;
            int S_OF_Logged_into_the_RAS = 21 ;
            int relevant_evt_OF_Logged_into_the_RAS = 22 ;
            int required_OF_RAS_access_granted = 23 ;
            int already_S_OF_RAS_access_granted = 24 ;
            int S_OF_RAS_access_granted = 25 ;
            int relevant_evt_OF_RAS_access_granted = 26 ;
            int required_OF_RAS_ownership = 27 ;
            int already_S_OF_RAS_ownership = 28 ;
            int S_OF_RAS_ownership = 29 ;
            int relevant_evt_OF_RAS_ownership = 30 ;
            int required_OF_Social_engineering = 31 ;
            int already_S_OF_Social_engineering = 32 ;
            int S_OF_Social_engineering = 33 ;
            int relevant_evt_OF_Social_engineering = 34 ;
            int failF_OF_Social_engineering = 35 ;
            int required_OF_Vulnerability_found_and_exploited = 36 ;
            int already_S_OF_Vulnerability_found_and_exploited = 37 ;
            int S_OF_Vulnerability_found_and_exploited = 38 ;
            int relevant_evt_OF_Vulnerability_found_and_exploited = 39 ;
            int required_OF_Wardialing = 40 ;
            int already_S_OF_Wardialing = 41 ;
            int S_OF_Wardialing = 42 ;
            int relevant_evt_OF_Wardialing = 43 ;
            int failF_OF_Wardialing = 44 ;




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