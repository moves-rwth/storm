//
//    #pragma once
//    #include <array>
//    #include <map>
//    #include <vector>
//    #include <sstream>
//    namespace storm{
//        namespace figaro{
//            class FigaroProgram{
//            public:
//                FigaroProgram();
//
//
//                std::map<std::string, size_t> mFigaroelementindex = {
//            	{"required_OF_AND_1" , 0},
//            	{"already_S_OF_AND_1" , 1},
//            	{"S_OF_AND_1" , 2},
//            	{"relevant_evt_OF_AND_1" , 3},
//            	{"required_OF_CB1_IO" , 4},
//            	{"already_S_OF_CB1_IO" , 5},
//            	{"S_OF_CB1_IO" , 6},
//            	{"relevant_evt_OF_CB1_IO" , 7},
//            	{"failF_OF_CB1_IO" , 8},
//            	{"required_OF_CB1_RO" , 9},
//            	{"already_S_OF_CB1_RO" , 10},
//            	{"S_OF_CB1_RO" , 11},
//            	{"relevant_evt_OF_CB1_RO" , 12},
//            	{"failI_OF_CB1_RO" , 13},
//            	{"to_be_fired_OF_CB1_RO" , 14},
//            	{"already_standby_OF_CB1_RO" , 15},
//            	{"already_required_OF_CB1_RO" , 16},
//            	{"required_OF_CB2_RC" , 17},
//            	{"already_S_OF_CB2_RC" , 18},
//            	{"S_OF_CB2_RC" , 19},
//            	{"relevant_evt_OF_CB2_RC" , 20},
//            	{"failI_OF_CB2_RC" , 21},
//            	{"to_be_fired_OF_CB2_RC" , 22},
//            	{"already_standby_OF_CB2_RC" , 23},
//            	{"already_required_OF_CB2_RC" , 24},
//            	{"required_OF_Dies_gen" , 25},
//            	{"already_S_OF_Dies_gen" , 26},
//            	{"S_OF_Dies_gen" , 27},
//            	{"relevant_evt_OF_Dies_gen" , 28},
//            	{"failF_OF_Dies_gen" , 29},
//            	{"required_OF_Dies_gen_RS" , 30},
//            	{"already_S_OF_Dies_gen_RS" , 31},
//            	{"S_OF_Dies_gen_RS" , 32},
//            	{"relevant_evt_OF_Dies_gen_RS" , 33},
//            	{"failI_OF_Dies_gen_RS" , 34},
//            	{"to_be_fired_OF_Dies_gen_RS" , 35},
//            	{"already_standby_OF_Dies_gen_RS" , 36},
//            	{"already_required_OF_Dies_gen_RS" , 37},
//            	{"required_OF_Grid" , 38},
//            	{"already_S_OF_Grid" , 39},
//            	{"S_OF_Grid" , 40},
//            	{"relevant_evt_OF_Grid" , 41},
//            	{"failF_OF_Grid" , 42},
//            	{"required_OF_LossOfDieselLine" , 43},
//            	{"already_S_OF_LossOfDieselLine" , 44},
//            	{"S_OF_LossOfDieselLine" , 45},
//            	{"relevant_evt_OF_LossOfDieselLine" , 46},
//            	{"required_OF_LossOfLine_1" , 47},
//            	{"already_S_OF_LossOfLine_1" , 48},
//            	{"S_OF_LossOfLine_1" , 49},
//            	{"relevant_evt_OF_LossOfLine_1" , 50},
//            	{"required_OF_UE_1" , 51},
//            	{"already_S_OF_UE_1" , 52},
//            	{"S_OF_UE_1" , 53},
//            	{"relevant_evt_OF_UE_1" , 54}};
//
//            std::map<std::string, size_t> mFigaroelementfailureindex = {
//            	{"S_OF_AND_1" , 2},
//            	{"S_OF_CB1_IO" , 6},
//            	{"S_OF_CB1_RO" , 11},
//            	{"S_OF_CB2_RC" , 19},
//            	{"S_OF_Dies_gen" , 27},
//            	{"S_OF_Dies_gen_RS" , 32},
//            	{"S_OF_Grid" , 40},
//            	{"S_OF_LossOfDieselLine" , 45},
//            	{"S_OF_LossOfLine_1" , 49},
//            	{"S_OF_UE_1" , 53}};
//
//            std::string const topevent="S_OF_UE_1";
//            static int const numBoolState = 55 ;
//            std::array<bool, numBoolState> boolState;
//            std::array<bool, numBoolState> backupBoolState;
//            bool ins_transition_found = false;
//
//            bool REINITIALISATION_OF_required_OF_AND_1 ;
//            bool REINITIALISATION_OF_S_OF_AND_1 ;
//            bool REINITIALISATION_OF_relevant_evt_OF_AND_1 ;
//            bool REINITIALISATION_OF_required_OF_CB1_IO ;
//            bool REINITIALISATION_OF_S_OF_CB1_IO ;
//            bool REINITIALISATION_OF_relevant_evt_OF_CB1_IO ;
//            bool REINITIALISATION_OF_required_OF_CB1_RO ;
//            bool REINITIALISATION_OF_S_OF_CB1_RO ;
//            bool REINITIALISATION_OF_relevant_evt_OF_CB1_RO ;
//            bool REINITIALISATION_OF_to_be_fired_OF_CB1_RO ;
//            bool REINITIALISATION_OF_required_OF_CB2_RC ;
//            bool REINITIALISATION_OF_S_OF_CB2_RC ;
//            bool REINITIALISATION_OF_relevant_evt_OF_CB2_RC ;
//            bool REINITIALISATION_OF_to_be_fired_OF_CB2_RC ;
//            bool REINITIALISATION_OF_required_OF_Dies_gen ;
//            bool REINITIALISATION_OF_S_OF_Dies_gen ;
//            bool REINITIALISATION_OF_relevant_evt_OF_Dies_gen ;
//            bool REINITIALISATION_OF_required_OF_Dies_gen_RS ;
//            bool REINITIALISATION_OF_S_OF_Dies_gen_RS ;
//            bool REINITIALISATION_OF_relevant_evt_OF_Dies_gen_RS ;
//            bool REINITIALISATION_OF_to_be_fired_OF_Dies_gen_RS ;
//            bool REINITIALISATION_OF_required_OF_Grid ;
//            bool REINITIALISATION_OF_S_OF_Grid ;
//            bool REINITIALISATION_OF_relevant_evt_OF_Grid ;
//            bool REINITIALISATION_OF_required_OF_LossOfDieselLine ;
//            bool REINITIALISATION_OF_S_OF_LossOfDieselLine ;
//            bool REINITIALISATION_OF_relevant_evt_OF_LossOfDieselLine ;
//            bool REINITIALISATION_OF_required_OF_LossOfLine_1 ;
//            bool REINITIALISATION_OF_S_OF_LossOfLine_1 ;
//            bool REINITIALISATION_OF_relevant_evt_OF_LossOfLine_1 ;
//            bool REINITIALISATION_OF_required_OF_UE_1 ;
//            bool REINITIALISATION_OF_S_OF_UE_1 ;
//            bool REINITIALISATION_OF_relevant_evt_OF_UE_1 ;
//
///* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
//            bool FIRE_xx10_OF_CB1_IO;
//            bool FIRE_xx23_OF_CB1_RO_INS_1;
//            bool FIRE_xx23_OF_CB1_RO_INS_2;
//            bool FIRE_xx23_OF_CB2_RC_INS_3;
//            bool FIRE_xx23_OF_CB2_RC_INS_4;
//            bool FIRE_xx10_OF_Dies_gen;
//            bool FIRE_xx23_OF_Dies_gen_RS_INS_6;
//            bool FIRE_xx23_OF_Dies_gen_RS_INS_7;
//            bool FIRE_xx10_OF_Grid;
//
//            int required_OF_AND_1 = 0 ;
//            int already_S_OF_AND_1 = 1 ;
//            int S_OF_AND_1 = 2 ;
//            int relevant_evt_OF_AND_1 = 3 ;
//            int required_OF_CB1_IO = 4 ;
//            int already_S_OF_CB1_IO = 5 ;
//            int S_OF_CB1_IO = 6 ;
//            int relevant_evt_OF_CB1_IO = 7 ;
//            int failF_OF_CB1_IO = 8 ;
//            int required_OF_CB1_RO = 9 ;
//            int already_S_OF_CB1_RO = 10 ;
//            int S_OF_CB1_RO = 11 ;
//            int relevant_evt_OF_CB1_RO = 12 ;
//            int failI_OF_CB1_RO = 13 ;
//            int to_be_fired_OF_CB1_RO = 14 ;
//            int already_standby_OF_CB1_RO = 15 ;
//            int already_required_OF_CB1_RO = 16 ;
//            int required_OF_CB2_RC = 17 ;
//            int already_S_OF_CB2_RC = 18 ;
//            int S_OF_CB2_RC = 19 ;
//            int relevant_evt_OF_CB2_RC = 20 ;
//            int failI_OF_CB2_RC = 21 ;
//            int to_be_fired_OF_CB2_RC = 22 ;
//            int already_standby_OF_CB2_RC = 23 ;
//            int already_required_OF_CB2_RC = 24 ;
//            int required_OF_Dies_gen = 25 ;
//            int already_S_OF_Dies_gen = 26 ;
//            int S_OF_Dies_gen = 27 ;
//            int relevant_evt_OF_Dies_gen = 28 ;
//            int failF_OF_Dies_gen = 29 ;
//            int required_OF_Dies_gen_RS = 30 ;
//            int already_S_OF_Dies_gen_RS = 31 ;
//            int S_OF_Dies_gen_RS = 32 ;
//            int relevant_evt_OF_Dies_gen_RS = 33 ;
//            int failI_OF_Dies_gen_RS = 34 ;
//            int to_be_fired_OF_Dies_gen_RS = 35 ;
//            int already_standby_OF_Dies_gen_RS = 36 ;
//            int already_required_OF_Dies_gen_RS = 37 ;
//            int required_OF_Grid = 38 ;
//            int already_S_OF_Grid = 39 ;
//            int S_OF_Grid = 40 ;
//            int relevant_evt_OF_Grid = 41 ;
//            int failF_OF_Grid = 42 ;
//            int required_OF_LossOfDieselLine = 43 ;
//            int already_S_OF_LossOfDieselLine = 44 ;
//            int S_OF_LossOfDieselLine = 45 ;
//            int relevant_evt_OF_LossOfDieselLine = 46 ;
//            int required_OF_LossOfLine_1 = 47 ;
//            int already_S_OF_LossOfLine_1 = 48 ;
//            int S_OF_LossOfLine_1 = 49 ;
//            int relevant_evt_OF_LossOfLine_1 = 50 ;
//            int required_OF_UE_1 = 51 ;
//            int already_S_OF_UE_1 = 52 ;
//            int S_OF_UE_1 = 53 ;
//            int relevant_evt_OF_UE_1 = 54 ;
//
//
//            /* ---------- DECLARATION OF FUNCTIONS ------------ */
//            void init();
//            void saveCurrentState();
//            void printState();
//            void fireOccurrence(int numFire);
//            std::vector<std::tuple<int, double, std::string, int>> showFireableOccurrences();
//            void runOnceInteractionStep_initialization();
//            void runOnceInteractionStep_propagate_effect_S();
//            void runOnceInteractionStep_propagate_effect_required();
//            void runOnceInteractionStep_propagate_leaves();
//            void runOnceInteractionStep_tops();
//            int compareStates();
//            void doReinitialisations();
//            void runInteractions();
//            void printstatetuple();
//            void fireinsttransitiongroup(std::string);
//            int_fast64_t stateSize() const;
//            bool figaromodelhasinstransitions();
//            };
//        }
//    }
