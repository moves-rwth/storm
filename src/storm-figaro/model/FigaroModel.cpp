

#include <iostream>

#include "FigaroModel.h"


using namespace std;





namespace storm{
    namespace figaro{
        /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
        storm::figaro::FigaroProgram::FigaroProgram()
        {
        for(int i=0; i < numBoolState; i++)
            boolState[i]=0;
        
        }
        
        void storm::figaro::FigaroProgram::init()
        {
        cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;
        
        REINITIALISATION_OF_capacity_OF_Backup_1 = 100;
        REINITIALISATION_OF_null_production_OF_Backup_1 = false;
        enumState[state_OF_Backup_1] = standby;
        intState[rank_OF_Backup_1] = 0;
        REINITIALISATION_OF_dequeue_OF_Backup_1 = false;
        boolState[fail_OF_Backup_1] = false;
        REINITIALISATION_OF_capacity_OF_Block_1 = 100;
        REINITIALISATION_OF_null_production_OF_Block_1 = false;
        enumState[state_OF_Block_1] = working;
        intState[rank_OF_Block_1] = 0;
        REINITIALISATION_OF_dequeue_OF_Block_1 = false;
        boolState[fail_OF_Block_1] = false;
        REINITIALISATION_OF_capacity_OF_Block_2 = 100;
        REINITIALISATION_OF_null_production_OF_Block_2 = false;
        enumState[state_OF_Block_2] = working;
        intState[rank_OF_Block_2] = 0;
        REINITIALISATION_OF_dequeue_OF_Block_2 = false;
        boolState[fail_OF_Block_2] = false;
        REINITIALISATION_OF_capacity_OF_Block_3 = 100;
        REINITIALISATION_OF_null_production_OF_Block_3 = false;
        enumState[state_OF_Block_3] = working;
        intState[rank_OF_Block_3] = 0;
        REINITIALISATION_OF_dequeue_OF_Block_3 = false;
        boolState[fail_OF_Block_3] = false;
        REINITIALISATION_OF_capacity_OF_Block_4 = 100;
        REINITIALISATION_OF_null_production_OF_Block_4 = false;
        enumState[state_OF_Block_4] = working;
        intState[rank_OF_Block_4] = 0;
        REINITIALISATION_OF_dequeue_OF_Block_4 = false;
        boolState[fail_OF_Block_4] = false;
        REINITIALISATION_OF_capacity_OF_SS1 = 100;
        REINITIALISATION_OF_null_production_OF_SS1 = false;
        REINITIALISATION_OF_potential_capacity_OF_SS1 = 100;
        REINITIALISATION_OF_capacity_OF_SS2 = 100;
        REINITIALISATION_OF_null_production_OF_SS2 = false;
        REINITIALISATION_OF_potential_capacity_OF_SS2 = 100;
        REINITIALISATION_OF_capacity_OF_min_1 = 100;
        REINITIALISATION_OF_null_production_OF_min_1 = false;
        REINITIALISATION_OF_max_rank_OF_rep_1 = 0;
        boolState[free_OF_rep_1] = true;
        boolState[dequeue_OF_rep_1] = false;
        
        /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
        FIRE_fail_in_op_OF_Backup_1 = false;
        FIRE_xx1_OF_Backup_1 = false;
        FIRE_xx2_OF_Backup_1_INS_2 = false;
        FIRE_xx2_OF_Backup_1_INS_3 = false;
        FIRE_fail_in_op_OF_Block_1 = false;
        FIRE_xx1_OF_Block_1 = false;
        FIRE_fail_in_op_OF_Block_2 = false;
        FIRE_xx1_OF_Block_2 = false;
        FIRE_fail_in_op_OF_Block_3 = false;
        FIRE_xx1_OF_Block_3 = false;
        FIRE_fail_in_op_OF_Block_4 = false;
        FIRE_xx1_OF_Block_4 = false;
        
        }
        
        void storm::figaro::FigaroProgram::saveCurrentState()
        {
            // cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
        backupBoolState = boolState ;
        backupFloatState = floatState ;
        backupIntState = intState ;
        backupEnumState = enumState ;
        }
        
        int storm::figaro::FigaroProgram::compareStates()
        {
            // cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;
        
        return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState);
        }
        
        void storm::figaro::FigaroProgram::printState()
        {
        cout <<"\n==================== Print of the current state :  ====================" << endl;
        
        cout << "Attribute :  floatState[capacity_OF_Backup_1] | Value : " << floatState[capacity_OF_Backup_1] << endl;
        cout << "Attribute :  boolState[null_production_OF_Backup_1] | Value : " << boolState[null_production_OF_Backup_1] << endl;
        cout << "Attribute :  enumState[state_OF_Backup_1] | Value : " << enumState[state_OF_Backup_1] << endl;
        cout << "Attribute :  intState[rank_OF_Backup_1] | Value : " << intState[rank_OF_Backup_1] << endl;
        cout << "Attribute :  boolState[dequeue_OF_Backup_1] | Value : " << boolState[dequeue_OF_Backup_1] << endl;
        cout << "Attribute :  boolState[fail_OF_Backup_1] | Value : " << boolState[fail_OF_Backup_1] << endl;
        cout << "Attribute :  floatState[capacity_OF_Block_1] | Value : " << floatState[capacity_OF_Block_1] << endl;
        cout << "Attribute :  boolState[null_production_OF_Block_1] | Value : " << boolState[null_production_OF_Block_1] << endl;
        cout << "Attribute :  enumState[state_OF_Block_1] | Value : " << enumState[state_OF_Block_1] << endl;
        cout << "Attribute :  intState[rank_OF_Block_1] | Value : " << intState[rank_OF_Block_1] << endl;
        cout << "Attribute :  boolState[dequeue_OF_Block_1] | Value : " << boolState[dequeue_OF_Block_1] << endl;
        cout << "Attribute :  boolState[fail_OF_Block_1] | Value : " << boolState[fail_OF_Block_1] << endl;
        cout << "Attribute :  floatState[capacity_OF_Block_2] | Value : " << floatState[capacity_OF_Block_2] << endl;
        cout << "Attribute :  boolState[null_production_OF_Block_2] | Value : " << boolState[null_production_OF_Block_2] << endl;
        cout << "Attribute :  enumState[state_OF_Block_2] | Value : " << enumState[state_OF_Block_2] << endl;
        cout << "Attribute :  intState[rank_OF_Block_2] | Value : " << intState[rank_OF_Block_2] << endl;
        cout << "Attribute :  boolState[dequeue_OF_Block_2] | Value : " << boolState[dequeue_OF_Block_2] << endl;
        cout << "Attribute :  boolState[fail_OF_Block_2] | Value : " << boolState[fail_OF_Block_2] << endl;
        cout << "Attribute :  floatState[capacity_OF_Block_3] | Value : " << floatState[capacity_OF_Block_3] << endl;
        cout << "Attribute :  boolState[null_production_OF_Block_3] | Value : " << boolState[null_production_OF_Block_3] << endl;
        cout << "Attribute :  enumState[state_OF_Block_3] | Value : " << enumState[state_OF_Block_3] << endl;
        cout << "Attribute :  intState[rank_OF_Block_3] | Value : " << intState[rank_OF_Block_3] << endl;
        cout << "Attribute :  boolState[dequeue_OF_Block_3] | Value : " << boolState[dequeue_OF_Block_3] << endl;
        cout << "Attribute :  boolState[fail_OF_Block_3] | Value : " << boolState[fail_OF_Block_3] << endl;
        cout << "Attribute :  floatState[capacity_OF_Block_4] | Value : " << floatState[capacity_OF_Block_4] << endl;
        cout << "Attribute :  boolState[null_production_OF_Block_4] | Value : " << boolState[null_production_OF_Block_4] << endl;
        cout << "Attribute :  enumState[state_OF_Block_4] | Value : " << enumState[state_OF_Block_4] << endl;
        cout << "Attribute :  intState[rank_OF_Block_4] | Value : " << intState[rank_OF_Block_4] << endl;
        cout << "Attribute :  boolState[dequeue_OF_Block_4] | Value : " << boolState[dequeue_OF_Block_4] << endl;
        cout << "Attribute :  boolState[fail_OF_Block_4] | Value : " << boolState[fail_OF_Block_4] << endl;
        cout << "Attribute :  floatState[capacity_OF_SS1] | Value : " << floatState[capacity_OF_SS1] << endl;
        cout << "Attribute :  boolState[null_production_OF_SS1] | Value : " << boolState[null_production_OF_SS1] << endl;
        cout << "Attribute :  floatState[potential_capacity_OF_SS1] | Value : " << floatState[potential_capacity_OF_SS1] << endl;
        cout << "Attribute :  floatState[capacity_OF_SS2] | Value : " << floatState[capacity_OF_SS2] << endl;
        cout << "Attribute :  boolState[null_production_OF_SS2] | Value : " << boolState[null_production_OF_SS2] << endl;
        cout << "Attribute :  floatState[potential_capacity_OF_SS2] | Value : " << floatState[potential_capacity_OF_SS2] << endl;
        cout << "Attribute :  floatState[capacity_OF_min_1] | Value : " << floatState[capacity_OF_min_1] << endl;
        cout << "Attribute :  boolState[null_production_OF_min_1] | Value : " << boolState[null_production_OF_min_1] << endl;
        cout << "Attribute :  intState[max_rank_OF_rep_1] | Value : " << intState[max_rank_OF_rep_1] << endl;
        cout << "Attribute :  boolState[free_OF_rep_1] | Value : " << boolState[free_OF_rep_1] << endl;
        cout << "Attribute :  boolState[dequeue_OF_rep_1] | Value : " << boolState[dequeue_OF_rep_1] << endl;
        }
        
        bool storm::figaro::FigaroProgram::figaromodelhasinstransitions()
        {
        return true;
        }
        
        void storm::figaro::FigaroProgram::doReinitialisations()
        {
        floatState[capacity_OF_Backup_1] = REINITIALISATION_OF_capacity_OF_Backup_1;
        boolState[null_production_OF_Backup_1] = REINITIALISATION_OF_null_production_OF_Backup_1;
        boolState[dequeue_OF_Backup_1] = REINITIALISATION_OF_dequeue_OF_Backup_1;
        floatState[capacity_OF_Block_1] = REINITIALISATION_OF_capacity_OF_Block_1;
        boolState[null_production_OF_Block_1] = REINITIALISATION_OF_null_production_OF_Block_1;
        boolState[dequeue_OF_Block_1] = REINITIALISATION_OF_dequeue_OF_Block_1;
        floatState[capacity_OF_Block_2] = REINITIALISATION_OF_capacity_OF_Block_2;
        boolState[null_production_OF_Block_2] = REINITIALISATION_OF_null_production_OF_Block_2;
        boolState[dequeue_OF_Block_2] = REINITIALISATION_OF_dequeue_OF_Block_2;
        floatState[capacity_OF_Block_3] = REINITIALISATION_OF_capacity_OF_Block_3;
        boolState[null_production_OF_Block_3] = REINITIALISATION_OF_null_production_OF_Block_3;
        boolState[dequeue_OF_Block_3] = REINITIALISATION_OF_dequeue_OF_Block_3;
        floatState[capacity_OF_Block_4] = REINITIALISATION_OF_capacity_OF_Block_4;
        boolState[null_production_OF_Block_4] = REINITIALISATION_OF_null_production_OF_Block_4;
        boolState[dequeue_OF_Block_4] = REINITIALISATION_OF_dequeue_OF_Block_4;
        floatState[capacity_OF_SS1] = REINITIALISATION_OF_capacity_OF_SS1;
        boolState[null_production_OF_SS1] = REINITIALISATION_OF_null_production_OF_SS1;
        floatState[potential_capacity_OF_SS1] = REINITIALISATION_OF_potential_capacity_OF_SS1;
        floatState[capacity_OF_SS2] = REINITIALISATION_OF_capacity_OF_SS2;
        boolState[null_production_OF_SS2] = REINITIALISATION_OF_null_production_OF_SS2;
        floatState[potential_capacity_OF_SS2] = REINITIALISATION_OF_potential_capacity_OF_SS2;
        floatState[capacity_OF_min_1] = REINITIALISATION_OF_capacity_OF_min_1;
        boolState[null_production_OF_min_1] = REINITIALISATION_OF_null_production_OF_min_1;
        intState[max_rank_OF_rep_1] = REINITIALISATION_OF_max_rank_OF_rep_1;
        }
        
        void storm::figaro::FigaroProgram::fireOccurrence(int numFire)
        {
        cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;
        
        if (numFire == 0)
            {
            FIRE_fail_in_op_OF_Backup_1 = true;
            }
        
        if (numFire == 1)
            {
            FIRE_xx1_OF_Backup_1 = true;
            }
        
        if (numFire == 2)
            {
            FIRE_xx2_OF_Backup_1_INS_2 = true;
            }
        
        if (numFire == 3)
            {
            FIRE_xx2_OF_Backup_1_INS_3 = true;
            }
        
        if (numFire == 4)
            {
            FIRE_fail_in_op_OF_Block_1 = true;
            }
        
        if (numFire == 5)
            {
            FIRE_xx1_OF_Block_1 = true;
            }
        
        if (numFire == 6)
            {
            FIRE_fail_in_op_OF_Block_2 = true;
            }
        
        if (numFire == 7)
            {
            FIRE_xx1_OF_Block_2 = true;
            }
        
        if (numFire == 8)
            {
            FIRE_fail_in_op_OF_Block_3 = true;
            }
        
        if (numFire == 9)
            {
            FIRE_xx1_OF_Block_3 = true;
            }
        
        if (numFire == 10)
            {
            FIRE_fail_in_op_OF_Block_4 = true;
            }
        
        if (numFire == 11)
            {
            FIRE_xx1_OF_Block_4 = true;
            }
        
        /* ---------- DECLARATION OF OCCURRENCE RULES------------ */
        
            // Occurrence fail_in_op_OF_Backup_1
        if ((boolState[fail_OF_Backup_1] == false) && (enumState[state_OF_Backup_1] == working))
            {
            
            if (FIRE_fail_in_op_OF_Backup_1)
                {
                boolState[fail_OF_Backup_1]  =  true;
                enumState[state_OF_Backup_1]  =  waiting_for_rep;
                intState[rank_OF_Backup_1]  =  (intState[max_rank_OF_rep_1] + 1);
                FIRE_fail_in_op_OF_Backup_1 = false;
                }
            }
        
            // Occurrence xx1_OF_Backup_1
        if ((boolState[fail_OF_Backup_1] == true) && (enumState[state_OF_Backup_1] == under_repair))
            {
            
            if (FIRE_xx1_OF_Backup_1)
                {
                boolState[fail_OF_Backup_1]  =  false;
                enumState[state_OF_Backup_1]  =  working;
                boolState[free_OF_rep_1]  =  true;
                boolState[dequeue_OF_rep_1]  =  true;
                FIRE_xx1_OF_Backup_1 = false;
                }
            }
        
            // Occurrence xx2_OF_Backup_1
        
        if ((boolState[fail_OF_Backup_1] == false) && (enumState[state_OF_Backup_1] == required))
            {
            
            
            if (FIRE_xx2_OF_Backup_1_INS_2)
                {
                boolState[fail_OF_Backup_1]  =  true;
                enumState[state_OF_Backup_1]  =  waiting_for_rep;
                intState[rank_OF_Backup_1]  =  (intState[max_rank_OF_rep_1] + 1);
                FIRE_xx2_OF_Backup_1_INS_2 = false;
                }
            
            }
        if ((boolState[fail_OF_Backup_1] == false) && (enumState[state_OF_Backup_1] == required))
            {
            
            
            if (FIRE_xx2_OF_Backup_1_INS_3)
                {
                enumState[state_OF_Backup_1]  =  working;
                FIRE_xx2_OF_Backup_1_INS_3 = false;
                }
            
            }
            // Occurrence fail_in_op_OF_Block_1
        if ((boolState[fail_OF_Block_1] == false) && (enumState[state_OF_Block_1] == working))
            {
            
            if (FIRE_fail_in_op_OF_Block_1)
                {
                boolState[fail_OF_Block_1]  =  true;
                enumState[state_OF_Block_1]  =  waiting_for_rep;
                intState[rank_OF_Block_1]  =  (intState[max_rank_OF_rep_1] + 1);
                FIRE_fail_in_op_OF_Block_1 = false;
                }
            }
        
            // Occurrence xx1_OF_Block_1
        if ((boolState[fail_OF_Block_1] == true) && (enumState[state_OF_Block_1] == under_repair))
            {
            
            if (FIRE_xx1_OF_Block_1)
                {
                boolState[fail_OF_Block_1]  =  false;
                enumState[state_OF_Block_1]  =  working;
                boolState[free_OF_rep_1]  =  true;
                boolState[dequeue_OF_rep_1]  =  true;
                FIRE_xx1_OF_Block_1 = false;
                }
            }
        
            // Occurrence fail_in_op_OF_Block_2
        if ((boolState[fail_OF_Block_2] == false) && (enumState[state_OF_Block_2] == working))
            {
            
            if (FIRE_fail_in_op_OF_Block_2)
                {
                boolState[fail_OF_Block_2]  =  true;
                enumState[state_OF_Block_2]  =  waiting_for_rep;
                intState[rank_OF_Block_2]  =  (intState[max_rank_OF_rep_1] + 1);
                FIRE_fail_in_op_OF_Block_2 = false;
                }
            }
        
            // Occurrence xx1_OF_Block_2
        if ((boolState[fail_OF_Block_2] == true) && (enumState[state_OF_Block_2] == under_repair))
            {
            
            if (FIRE_xx1_OF_Block_2)
                {
                boolState[fail_OF_Block_2]  =  false;
                enumState[state_OF_Block_2]  =  working;
                boolState[free_OF_rep_1]  =  true;
                boolState[dequeue_OF_rep_1]  =  true;
                FIRE_xx1_OF_Block_2 = false;
                }
            }
        
            // Occurrence fail_in_op_OF_Block_3
        if ((boolState[fail_OF_Block_3] == false) && (enumState[state_OF_Block_3] == working))
            {
            
            if (FIRE_fail_in_op_OF_Block_3)
                {
                boolState[fail_OF_Block_3]  =  true;
                enumState[state_OF_Block_3]  =  waiting_for_rep;
                intState[rank_OF_Block_3]  =  (intState[max_rank_OF_rep_1] + 1);
                FIRE_fail_in_op_OF_Block_3 = false;
                }
            }
        
            // Occurrence xx1_OF_Block_3
        if ((boolState[fail_OF_Block_3] == true) && (enumState[state_OF_Block_3] == under_repair))
            {
            
            if (FIRE_xx1_OF_Block_3)
                {
                boolState[fail_OF_Block_3]  =  false;
                enumState[state_OF_Block_3]  =  working;
                boolState[free_OF_rep_1]  =  true;
                boolState[dequeue_OF_rep_1]  =  true;
                FIRE_xx1_OF_Block_3 = false;
                }
            }
        
            // Occurrence fail_in_op_OF_Block_4
        if ((boolState[fail_OF_Block_4] == false) && (enumState[state_OF_Block_4] == working))
            {
            
            if (FIRE_fail_in_op_OF_Block_4)
                {
                boolState[fail_OF_Block_4]  =  true;
                enumState[state_OF_Block_4]  =  waiting_for_rep;
                intState[rank_OF_Block_4]  =  (intState[max_rank_OF_rep_1] + 1);
                FIRE_fail_in_op_OF_Block_4 = false;
                }
            }
        
            // Occurrence xx1_OF_Block_4
        if ((boolState[fail_OF_Block_4] == true) && (enumState[state_OF_Block_4] == under_repair))
            {
            
            if (FIRE_xx1_OF_Block_4)
                {
                boolState[fail_OF_Block_4]  =  false;
                enumState[state_OF_Block_4]  =  working;
                boolState[free_OF_rep_1]  =  true;
                boolState[dequeue_OF_rep_1]  =  true;
                FIRE_xx1_OF_Block_4 = false;
                }
            }
        
        }
        
        std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram::showFireableOccurrences()
        {
        std::vector<std::tuple<int, double, std::string, int>> list = {};
        cout <<"\n==================== List of fireable occurrences :  ====================" << endl;
        
        if ((boolState[fail_OF_Backup_1] == false) && (enumState[state_OF_Backup_1] == required))
            {
            cout << "2 :  INS_SUB_COUNT (1) |FAULT | fail | DIST INS (gamma_OF_Backup_1) | INDUCING boolState[fail_OF_Backup_1]  =  TRUE,state_OF_Backup_1  =  waiting_for_rep,rank_OF_Backup_1  =  (intState[max_rank_OF_rep_1] + 1)" << endl;
            list.push_back(make_tuple(2, gamma_OF_Backup_1, "INS", 1));
            }
        if ((boolState[fail_OF_Backup_1] == false) && (enumState[state_OF_Backup_1] == required))
            {
            cout << "3 :  INS_SUB_COUNT (1) |TRANSITION | startup | DIST INS (1 - gamma_OF_Backup_1) | INDUCING enumState[state_OF_Backup_1]  =  working" << endl;
            list.push_back(make_tuple(3, 1 - gamma_OF_Backup_1, "INS", 1));
            }
        if (list.size() > 0)
            {
            ins_transition_found = true;
            return list;
            }
        else
            {
            ins_transition_found = false;
            }
        
        if ((boolState[fail_OF_Backup_1] == false) && (enumState[state_OF_Backup_1] == working))
            {
            cout << "0 : fail_in_op_OF_Backup_1 : FAULT fail  DIST EXP (lambda_OF_Backup_1)  INDUCING boolState[fail_OF_Backup_1]  =  TRUE,state_OF_Backup_1  =  'waiting_for_rep',rank_OF_Backup_1  =  (intState[max_rank_OF_rep_1] + 1)" << endl;
            list.push_back(make_tuple(0, lambda_OF_Backup_1, "EXP", 0));
            }
        if ((boolState[fail_OF_Backup_1] == true) && (enumState[state_OF_Backup_1] == under_repair))
            {
            cout << "1 : xx1_OF_Backup_1 : REPAIR rep  DIST EXP (mu_OF_Backup_1)  INDUCING boolState[fail_OF_Backup_1]  =  FALSE,state_OF_Backup_1  =  'working',free_OF_rep_1  =  TRUE,dequeue_OF_rep_1  =  TRUE" << endl;
            list.push_back(make_tuple(1, mu_OF_Backup_1, "EXP", 0));
            }
        if ((boolState[fail_OF_Block_1] == false) && (enumState[state_OF_Block_1] == working))
            {
            cout << "4 : fail_in_op_OF_Block_1 : FAULT fail  DIST EXP (lambda_OF_Block_1)  INDUCING boolState[fail_OF_Block_1]  =  TRUE,state_OF_Block_1  =  'waiting_for_rep',rank_OF_Block_1  =  (intState[max_rank_OF_rep_1] + 1)" << endl;
            list.push_back(make_tuple(4, lambda_OF_Block_1, "EXP", 0));
            }
        if ((boolState[fail_OF_Block_1] == true) && (enumState[state_OF_Block_1] == under_repair))
            {
            cout << "5 : xx1_OF_Block_1 : REPAIR rep  DIST EXP (mu_OF_Block_1)  INDUCING boolState[fail_OF_Block_1]  =  FALSE,state_OF_Block_1  =  'working',free_OF_rep_1  =  TRUE,dequeue_OF_rep_1  =  TRUE" << endl;
            list.push_back(make_tuple(5, mu_OF_Block_1, "EXP", 0));
            }
        if ((boolState[fail_OF_Block_2] == false) && (enumState[state_OF_Block_2] == working))
            {
            cout << "6 : fail_in_op_OF_Block_2 : FAULT fail  DIST EXP (lambda_OF_Block_2)  INDUCING boolState[fail_OF_Block_2]  =  TRUE,state_OF_Block_2  =  'waiting_for_rep',rank_OF_Block_2  =  (intState[max_rank_OF_rep_1] + 1)" << endl;
            list.push_back(make_tuple(6, lambda_OF_Block_2, "EXP", 0));
            }
        if ((boolState[fail_OF_Block_2] == true) && (enumState[state_OF_Block_2] == under_repair))
            {
            cout << "7 : xx1_OF_Block_2 : REPAIR rep  DIST EXP (mu_OF_Block_2)  INDUCING boolState[fail_OF_Block_2]  =  FALSE,state_OF_Block_2  =  'working',free_OF_rep_1  =  TRUE,dequeue_OF_rep_1  =  TRUE" << endl;
            list.push_back(make_tuple(7, mu_OF_Block_2, "EXP", 0));
            }
        if ((boolState[fail_OF_Block_3] == false) && (enumState[state_OF_Block_3] == working))
            {
            cout << "8 : fail_in_op_OF_Block_3 : FAULT fail  DIST EXP (lambda_OF_Block_3)  INDUCING boolState[fail_OF_Block_3]  =  TRUE,state_OF_Block_3  =  'waiting_for_rep',rank_OF_Block_3  =  (intState[max_rank_OF_rep_1] + 1)" << endl;
            list.push_back(make_tuple(8, lambda_OF_Block_3, "EXP", 0));
            }
        if ((boolState[fail_OF_Block_3] == true) && (enumState[state_OF_Block_3] == under_repair))
            {
            cout << "9 : xx1_OF_Block_3 : REPAIR rep  DIST EXP (mu_OF_Block_3)  INDUCING boolState[fail_OF_Block_3]  =  FALSE,state_OF_Block_3  =  'working',free_OF_rep_1  =  TRUE,dequeue_OF_rep_1  =  TRUE" << endl;
            list.push_back(make_tuple(9, mu_OF_Block_3, "EXP", 0));
            }
        if ((boolState[fail_OF_Block_4] == false) && (enumState[state_OF_Block_4] == working))
            {
            cout << "10 : fail_in_op_OF_Block_4 : FAULT fail  DIST EXP (lambda_OF_Block_4)  INDUCING boolState[fail_OF_Block_4]  =  TRUE,state_OF_Block_4  =  'waiting_for_rep',rank_OF_Block_4  =  (intState[max_rank_OF_rep_1] + 1)" << endl;
            list.push_back(make_tuple(10, lambda_OF_Block_4, "EXP", 0));
            }
        if ((boolState[fail_OF_Block_4] == true) && (enumState[state_OF_Block_4] == under_repair))
            {
            cout << "11 : xx1_OF_Block_4 : REPAIR rep  DIST EXP (mu_OF_Block_4)  INDUCING boolState[fail_OF_Block_4]  =  FALSE,state_OF_Block_4  =  'working',free_OF_rep_1  =  TRUE,dequeue_OF_rep_1  =  TRUE" << endl;
            list.push_back(make_tuple(11, mu_OF_Block_4, "EXP", 0));
            }
        return list;
        }
        
        
        void storm::figaro::FigaroProgram::runOnceInteractionStep_default_step()
        {
        if (enumState[state_OF_Backup_1] == working )
            {
            floatState[capacity_OF_Backup_1]  =  nominal_capacity_OF_Backup_1;
            } else { floatState[capacity_OF_Backup_1]  =   0; }
        
        
        if ( !(boolState[fail_OF_Backup_1] == false) )
            {
            boolState[null_production_OF_Backup_1]  =  true;
            }
        
        if (boolState[dequeue_OF_Backup_1] && (intState[rank_OF_Backup_1] > 0) )
            {
            boolState[dequeue_OF_Backup_1]  =  false;
            intState[rank_OF_Backup_1]  =  (intState[rank_OF_Backup_1] - 1);
            }
        
        if (((enumState[state_OF_Backup_1] == working) || (enumState[state_OF_Backup_1] ==  required)) && (boolState[fail_OF_Block_4] == false) )
            {
            enumState[state_OF_Backup_1]  =  standby;
            }
        
        if ((enumState[state_OF_Backup_1] == standby) && (boolState[fail_OF_Block_4] == true) )
            {
            enumState[state_OF_Backup_1]  =  required;
            }
        
        if (boolState[fail_OF_Block_1] == false )
            {
            floatState[capacity_OF_Block_1]  =  nominal_capacity_OF_Block_1;
            } else { floatState[capacity_OF_Block_1]  =   0; }
        
        
        if ( !(boolState[fail_OF_Block_1] == false) )
            {
            boolState[null_production_OF_Block_1]  =  true;
            }
        
        if (boolState[dequeue_OF_Block_1] && (intState[rank_OF_Block_1] > 0) )
            {
            boolState[dequeue_OF_Block_1]  =  false;
            intState[rank_OF_Block_1]  =  (intState[rank_OF_Block_1] - 1);
            }
        
        if (boolState[fail_OF_Block_2] == false )
            {
            floatState[capacity_OF_Block_2]  =  nominal_capacity_OF_Block_2;
            } else { floatState[capacity_OF_Block_2]  =   0; }
        
        
        if ( !(boolState[fail_OF_Block_2] == false) )
            {
            boolState[null_production_OF_Block_2]  =  true;
            }
        
        if (boolState[dequeue_OF_Block_2] && (intState[rank_OF_Block_2] > 0) )
            {
            boolState[dequeue_OF_Block_2]  =  false;
            intState[rank_OF_Block_2]  =  (intState[rank_OF_Block_2] - 1);
            }
        
        if (boolState[fail_OF_Block_3] == false )
            {
            floatState[capacity_OF_Block_3]  =  nominal_capacity_OF_Block_3;
            } else { floatState[capacity_OF_Block_3]  =   0; }
        
        
        if ( !(boolState[fail_OF_Block_3] == false) )
            {
            boolState[null_production_OF_Block_3]  =  true;
            }
        
        if (boolState[dequeue_OF_Block_3] && (intState[rank_OF_Block_3] > 0) )
            {
            boolState[dequeue_OF_Block_3]  =  false;
            intState[rank_OF_Block_3]  =  (intState[rank_OF_Block_3] - 1);
            }
        
        if (boolState[fail_OF_Block_4] == false )
            {
            floatState[capacity_OF_Block_4]  =  nominal_capacity_OF_Block_4;
            } else { floatState[capacity_OF_Block_4]  =   0; }
        
        
        if ( !(boolState[fail_OF_Block_4] == false) )
            {
            boolState[null_production_OF_Block_4]  =  true;
            }
        
        if (boolState[dequeue_OF_Block_4] && (intState[rank_OF_Block_4] > 0) )
            {
            boolState[dequeue_OF_Block_4]  =  false;
            intState[rank_OF_Block_4]  =  (intState[rank_OF_Block_4] - 1);
            }
        
        
        
        floatState[potential_capacity_OF_SS1]  =  fmin(floatState[capacity_OF_Block_1]  +   floatState[capacity_OF_Block_2],  100)  ;
        
        if (floatState[potential_capacity_OF_SS1] < functioning_threshold_OF_SS1 )
            {
            floatState[capacity_OF_SS1]  =   0;
            } else { floatState[capacity_OF_SS1]  =  floatState[potential_capacity_OF_SS1]; }
        
        
        if (boolState[null_production_OF_Block_1] && boolState[null_production_OF_Block_2] )
            {
            boolState[null_production_OF_SS1]  =  true;
            }
        
        
        
        floatState[potential_capacity_OF_SS2]  =  fmin((floatState[capacity_OF_Backup_1]  +   floatState[capacity_OF_Block_3])  +  floatState[capacity_OF_Block_4],  100)  ;
        
        if (floatState[potential_capacity_OF_SS2] < functioning_threshold_OF_SS2 )
            {
            floatState[capacity_OF_SS2]  =   0;
            } else { floatState[capacity_OF_SS2]  =  floatState[potential_capacity_OF_SS2]; }
        
        
        if ((boolState[null_production_OF_Backup_1] && boolState[null_production_OF_Block_3]) && boolState[null_production_OF_Block_4] )
            {
            boolState[null_production_OF_SS2]  =  true;
            }
        
        if (floatState[capacity_OF_SS1] < floatState[capacity_OF_min_1] )
            {
            floatState[capacity_OF_min_1]  =  floatState[capacity_OF_SS1];
            }
        
        if (floatState[capacity_OF_SS2] < floatState[capacity_OF_min_1] )
            {
            floatState[capacity_OF_min_1]  =  floatState[capacity_OF_SS2];
            }
        
        if (boolState[null_production_OF_SS1] || boolState[null_production_OF_SS2] )
            {
            boolState[null_production_OF_min_1]  =  true;
            }
        
        if (boolState[dequeue_OF_rep_1] )
            {
            boolState[dequeue_OF_rep_1]  =  false;
            boolState[dequeue_OF_Block_1]  =  true;
            boolState[dequeue_OF_Block_3]  =  true;
            boolState[dequeue_OF_Block_2]  =  true;
            boolState[dequeue_OF_Block_4]  =  true;
            boolState[dequeue_OF_Backup_1]  =  true;
            }
        
        }
        
        
        void storm::figaro::FigaroProgram::runOnceInteractionStep_compute_max_rank()
        {
        
        
        intState[max_rank_OF_rep_1]  =  fmax(intState[max_rank_OF_rep_1], fmax(intState[rank_OF_Block_1], fmax( intState[rank_OF_Block_3], fmax( intState[rank_OF_Block_2], fmax( intState[rank_OF_Block_4],  intState[rank_OF_Backup_1])))))  ;
        
        }
        
        
        void storm::figaro::FigaroProgram::runOnceInteractionStep_rep_management()
        {
        if (((enumState[state_OF_Backup_1] == waiting_for_rep) && ((intState[rank_OF_Backup_1] == 1) || (1 == 0))) && boolState[free_OF_rep_1] )
            {
            enumState[state_OF_Backup_1]  =  under_repair;
            boolState[free_OF_rep_1]  =  false;
            }
        
        if (((enumState[state_OF_Block_1] == waiting_for_rep) && ((intState[rank_OF_Block_1] == 1) || (1 == 0))) && boolState[free_OF_rep_1] )
            {
            enumState[state_OF_Block_1]  =  under_repair;
            boolState[free_OF_rep_1]  =  false;
            }
        
        if (((enumState[state_OF_Block_2] == waiting_for_rep) && ((intState[rank_OF_Block_2] == 1) || (1 == 0))) && boolState[free_OF_rep_1] )
            {
            enumState[state_OF_Block_2]  =  under_repair;
            boolState[free_OF_rep_1]  =  false;
            }
        
        if (((enumState[state_OF_Block_3] == waiting_for_rep) && ((intState[rank_OF_Block_3] == 1) || (1 == 0))) && boolState[free_OF_rep_1] )
            {
            enumState[state_OF_Block_3]  =  under_repair;
            boolState[free_OF_rep_1]  =  false;
            }
        
        if (((enumState[state_OF_Block_4] == waiting_for_rep) && ((intState[rank_OF_Block_4] == 1) || (1 == 0))) && boolState[free_OF_rep_1] )
            {
            enumState[state_OF_Block_4]  =  under_repair;
            boolState[free_OF_rep_1]  =  false;
            }
        
        }
        
        void storm::figaro::FigaroProgram::runInteractions() {
            int counter = 0;
            int comparator = 1;
            doReinitialisations();
            int max_interactions_loop = 200;
            
            counter = 0;
            comparator = 1;
            do
                {
                    //cout << counter << endl;
                saveCurrentState();
                runOnceInteractionStep_default_step();
                
                comparator = compareStates();
                counter++;
                
                } while (comparator > 0 && counter < max_interactions_loop);
            if (comparator <= 0)
                {
                cout << "==> Stabilisation of interactions at loop #" << counter << " for runInteractionStep_default_step() ." << endl;
                }
            else {
                cout << "==> Maximum of interactions loop  reached : #" << counter <<" for runOnceInteractionStep_default_step()." << endl;
            }
            
            counter = 0;
            comparator = 1;
            do
                {
                    //cout << counter << endl;
                saveCurrentState();
                runOnceInteractionStep_compute_max_rank();
                
                comparator = compareStates();
                counter++;
                
                } while (comparator > 0 && counter < max_interactions_loop);
            if (comparator <= 0)
                {
                cout << "==> Stabilisation of interactions at loop #" << counter << " for runInteractionStep_compute_max_rank() ." << endl;
                }
            else {
                cout << "==> Maximum of interactions loop  reached : #" << counter <<" for runOnceInteractionStep_compute_max_rank()." << endl;
            }
            
            counter = 0;
            comparator = 1;
            do
                {
                    //cout << counter << endl;
                saveCurrentState();
                runOnceInteractionStep_rep_management();
                
                comparator = compareStates();
                counter++;
                
                } while (comparator > 0 && counter < max_interactions_loop);
            if (comparator <= 0)
                {
                cout << "==> Stabilisation of interactions at loop #" << counter << " for runInteractionStep_rep_management() ." << endl;
                }
            else {
                cout << "==> Maximum of interactions loop  reached : #" << counter <<" for runOnceInteractionStep_rep_management()." << endl;
            }
            
            cout << endl;
        }
        void storm::figaro::FigaroProgram::printstatetuple(){
            std::cout<<"\n State information: (";
            for (int i=0; i<boolState.size(); i++)
                {
                std::cout<<boolState.at(i);
                }
            std::cout<<")";
            
        }
        int_fast64_t FigaroProgram::stateSize() const{
            return numBoolState;
        }
        
        void storm::figaro::FigaroProgram::fireinsttransitiongroup(std::string user_input_ins)
        {
        std::vector<int> list_user_inputs = {};
        int user_input = -2;
        stringstream ss(user_input_ins);
        for (int i; ss >> i;) {
            list_user_inputs.push_back(i);
            if (ss.peek() == ',')
                ss.ignore();
        }
        
        for (size_t i = 0; i < list_user_inputs.size(); i++)
            {
            cout << list_user_inputs[i] << endl;
            user_input = list_user_inputs[i];
            if (user_input == -1) {
                break;
            }
            fireOccurrence(user_input);
            }
        }
        
    }
}

