
#pragma once
#include <array>
#include <map>
#include <vector>
#include <sstream>
#include<math.h>
#include <set>

#include "FigaroModelTemplate.h"

namespace storm{
    namespace figaro{
    class FigaroProgram1: public storm::figaro::FigaroProgram{
        public:
        FigaroProgram1(): FigaroProgram(
            {
                {"fail_OF_Node_1" , 0},
                {"connected_OF_Node_1" , 1},
                {"fail_OF_Node_2" , 2},
                {"connected_OF_Node_2" , 3},
                {"interruption_OF_ud_1" , 4},
                {"interruption_OF_bidir_3" , 5},
                {"fail_OF_Node_6" , 6},
                {"connected_OF_Node_6" , 7},
                {"fail_OF_Node_8" , 8},
                {"connected_OF_Node_8" , 9},
                {"fail_OF_Source" , 10},
                {"connected_OF_Source" , 11},
                {"fail_OF_Target" , 12},
                {"connected_OF_Target" , 13},
                {"interruption_OF_ud_3" , 14},
                {"interruption_OF_bidir_7" , 15},
                {"interruption_OF_bidir_12" , 16},
                {"interruption_OF_bidir_2" , 17},
                {"not_connected_OF_Target", 18}
            },
            
             {{"not_connected_OF_Target", 18}},
             { },
            
            {
                {"nb_failures_OF_Failure_counter" , 0}},
            
            { },
            
             { },
            
            { },
            "not_connected_OF_Target",
            19 ,
            0 ,
            1 ,
            0 ,
            false
        ){ }

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


            bool REINITIALISATION_OF_connected_OF_Node_1 ;
            bool REINITIALISATION_OF_connected_OF_Node_2 ;
            bool REINITIALISATION_OF_connected_OF_Node_6 ;
            bool REINITIALISATION_OF_connected_OF_Node_8 ;
            bool REINITIALISATION_OF_connected_OF_Source ;
            bool REINITIALISATION_OF_connected_OF_Target ;
            
            /* ---------- DECLARATION OF CONSTANTS ------------ */
            double const link_mu_OF_ud_3 = 1;
            double const link_lambda_OF_bidir_7 = 1e-05;
            std::string const function_OF_Node_8 = "intermediate";
            double const mu_OF_Source = 0.1;
            double const link_mu_OF_bidir_12 = 1;
            double const mu_OF_Node_6 = 0.1;
            double const mu_OF_Node_8 = 0.1;
            double const lambda_OF_Node_2 = 1e-05;
            double const link_lambda_OF_bidir_3 = 1e-05;
            double const lambda_OF_Target = 1e-05;
            double const link_mu_OF_bidir_7 = 1;
            bool const Profil1_OF___ARBRE__EIRM = true;
            std::string const function_OF_Node_2 = "intermediate";
            double const link_lambda_OF_ud_1 = 1e-05;
            double const link_mu_OF_bidir_3 = 1;
            double const link_mu_OF_ud_1 = 1;
            double const mu_OF_Node_2 = 0.1;
            double const mu_OF_Target = 0.1;
            double const lambda_OF_Node_1 = 1e-05;
            double const lambda_OF_Source = 1e-05;
            double const lambda_OF_Node_6 = 1e-05;
            double const link_lambda_OF_bidir_2 = 1e-05;
            std::string const function_OF_Target = "target";
            double const link_lambda_OF_ud_3 = 1e-05;
            std::string const function_OF_Node_1 = "intermediate";
            double const lambda_OF_Node_8 = 1e-05;
            std::string const function_OF_Node_6 = "intermediate";
            double const link_lambda_OF_bidir_12 = 1e-05;
            double const link_mu_OF_bidir_2 = 1;
            double const mu_OF_Node_1 = 0.1;
            std::string const function_OF_Source = "source";
            
            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            bool FIRE_xx1_OF_Node_1;
            bool FIRE_xx2_OF_Node_1;
            bool FIRE_xx1_OF_Node_2;
            bool FIRE_xx2_OF_Node_2;
            bool FIRE_xx3_OF_ud_1;
            bool FIRE_xx4_OF_ud_1;
            bool FIRE_xx3_OF_bidir_3;
            bool FIRE_xx4_OF_bidir_3;
            bool FIRE_xx1_OF_Node_6;
            bool FIRE_xx2_OF_Node_6;
            bool FIRE_xx1_OF_Node_8;
            bool FIRE_xx2_OF_Node_8;
            bool FIRE_xx1_OF_Source;
            bool FIRE_xx2_OF_Source;
            bool FIRE_xx1_OF_Target;
            bool FIRE_xx2_OF_Target;
            bool FIRE_xx3_OF_ud_3;
            bool FIRE_xx4_OF_ud_3;
            bool FIRE_xx3_OF_bidir_7;
            bool FIRE_xx4_OF_bidir_7;
            bool FIRE_xx3_OF_bidir_12;
            bool FIRE_xx4_OF_bidir_12;
            bool FIRE_xx3_OF_bidir_2;
            bool FIRE_xx4_OF_bidir_2;
            
            int fail_OF_Node_1 = 0 ;
            int connected_OF_Node_1 = 1 ;
            int fail_OF_Node_2 = 2 ;
            int connected_OF_Node_2 = 3 ;
            int interruption_OF_ud_1 = 4 ;
            int interruption_OF_bidir_3 = 5 ;
            int fail_OF_Node_6 = 6 ;
            int connected_OF_Node_6 = 7 ;
            int fail_OF_Node_8 = 8 ;
            int connected_OF_Node_8 = 9 ;
            int fail_OF_Source = 10 ;
            int connected_OF_Source = 11 ;
            int fail_OF_Target = 12 ;
            int connected_OF_Target = 13 ;
            int interruption_OF_ud_3 = 14 ;
            int interruption_OF_bidir_7 = 15 ;
            int interruption_OF_bidir_12 = 16 ;
            int interruption_OF_bidir_2 = 17 ;
            int not_connected_OF_Target = 18;
            
            
            int nb_failures_OF_Failure_counter = 0 ;
            
            
            
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

