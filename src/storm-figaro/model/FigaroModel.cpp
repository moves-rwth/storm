

#include <iostream>

#include "FigaroModel.h"


using namespace std;





namespace storm{
    namespace figaro{
        /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
//        storm::figaro::FigaroProgram1::FigaroProgram1()
//        {
//            for(int i=0; i < numBoolState; i++)
//                boolState[i]=0;
//
//        }

        void storm::figaro::FigaroProgram1::init()
        {
            cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

            floatState[calculated_lambda_OF_Arrival] = 0;
            intState[mark_OF_Cashdesk] = 0;
            intState[mark_OF_First_waiting_area] = 0;
            intState[mark_OF_Pump_1] = 0;
            intState[mark_OF_Pump_2] = 0;
            intState[mark_OF_Pump_3] = 0;
            intState[mark_OF_Second_waiting_area] = 0;
            floatState[calculated_lambda_OF_end_of_payment] = 0;
            floatState[calculated_lambda_OF_tank_is_full_1] = 0;
            floatState[calculated_lambda_OF_tank_is_full_2] = 0;
            floatState[calculated_lambda_OF_tank_is_full_3] = 0;


            /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
            FIRE_xx2_OF_Arrival = false;
            FIRE_xx2_OF_end_of_payment = false;
            FIRE_xx2_OF_tank_is_full_1 = false;
            FIRE_xx2_OF_tank_is_full_2 = false;
            FIRE_xx2_OF_tank_is_full_3 = false;
            boolFailureState[mark_OF_Second_waiting_area_iseq_3] = (intState[mark_OF_Second_waiting_area] == 3);

        }

        void storm::figaro::FigaroProgram1::saveCurrentState()
        {
            // cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
            backupBoolState = boolState ;
            backupFloatState = floatState ;
            backupIntState = intState ;
            backupEnumState = enumState ;
        }

        int storm::figaro::FigaroProgram1::compareStates()
        {
            // cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

            return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState);
        }

        void storm::figaro::FigaroProgram1::printState()
        {
            cout <<"\n==================== Print of the current state :  ====================" << endl;

            cout << "Attribute :  floatState[calculated_lambda_OF_Arrival] | Value : " << floatState[calculated_lambda_OF_Arrival] << endl;
            cout << "Attribute :  intState[mark_OF_Cashdesk] | Value : " << intState[mark_OF_Cashdesk] << endl;
            cout << "Attribute :  intState[mark_OF_First_waiting_area] | Value : " << intState[mark_OF_First_waiting_area] << endl;
            cout << "Attribute :  intState[mark_OF_Pump_1] | Value : " << intState[mark_OF_Pump_1] << endl;
            cout << "Attribute :  intState[mark_OF_Pump_2] | Value : " << intState[mark_OF_Pump_2] << endl;
            cout << "Attribute :  intState[mark_OF_Pump_3] | Value : " << intState[mark_OF_Pump_3] << endl;
            cout << "Attribute :  intState[mark_OF_Second_waiting_area] | Value : " << intState[mark_OF_Second_waiting_area] << endl;
            cout << "Attribute :  floatState[calculated_lambda_OF_end_of_payment] | Value : " << floatState[calculated_lambda_OF_end_of_payment] << endl;
            cout << "Attribute :  floatState[calculated_lambda_OF_tank_is_full_1] | Value : " << floatState[calculated_lambda_OF_tank_is_full_1] << endl;
            cout << "Attribute :  floatState[calculated_lambda_OF_tank_is_full_2] | Value : " << floatState[calculated_lambda_OF_tank_is_full_2] << endl;
            cout << "Attribute :  floatState[calculated_lambda_OF_tank_is_full_3] | Value : " << floatState[calculated_lambda_OF_tank_is_full_3] << endl;
        }

        bool storm::figaro::FigaroProgram1::figaromodelhasinstransitions()
        {
            return false;
        }

        void storm::figaro::FigaroProgram1::doReinitialisations()
        {
        }

        void storm::figaro::FigaroProgram1::fireOccurrence(int numFire)
        {
            cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

            if (numFire == 0)
            {
                FIRE_xx2_OF_Arrival = true;
            }

            if (numFire == 1)
            {
                FIRE_xx2_OF_end_of_payment = true;
            }

            if (numFire == 2)
            {
                FIRE_xx2_OF_tank_is_full_1 = true;
            }

            if (numFire == 3)
            {
                FIRE_xx2_OF_tank_is_full_2 = true;
            }

            if (numFire == 4)
            {
                FIRE_xx2_OF_tank_is_full_3 = true;
            }

            /* ---------- DECLARATION OF OCCURRENCE RULES------------ */

            // Occurrence xx2_OF_Arrival
            if (intState[mark_OF_First_waiting_area] < 3)
            {

                if (FIRE_xx2_OF_Arrival)
                {
                    intState[mark_OF_First_waiting_area]  =  (intState[mark_OF_First_waiting_area] + 1);
                    FIRE_xx2_OF_Arrival = false;
                }
            }

            // Occurrence xx2_OF_end_of_payment
            if (intState[mark_OF_Cashdesk] >= 1)
            {

                if (FIRE_xx2_OF_end_of_payment)
                {
                    intState[mark_OF_Cashdesk]  =  (intState[mark_OF_Cashdesk] - 1);
                    FIRE_xx2_OF_end_of_payment = false;
                }
            }

            // Occurrence xx2_OF_tank_is_full_1
            if ((intState[mark_OF_Pump_1] >= 1) && (intState[mark_OF_Second_waiting_area] < 3))
            {

                if (FIRE_xx2_OF_tank_is_full_1)
                {
                    intState[mark_OF_Second_waiting_area]  =  (intState[mark_OF_Second_waiting_area] + 1);
                    intState[mark_OF_Pump_1]  =  (intState[mark_OF_Pump_1] - 1);
                    FIRE_xx2_OF_tank_is_full_1 = false;
                }
            }

            // Occurrence xx2_OF_tank_is_full_2
            if ((intState[mark_OF_Pump_2] >= 1) && (intState[mark_OF_Second_waiting_area] < 3))
            {

                if (FIRE_xx2_OF_tank_is_full_2)
                {
                    intState[mark_OF_Second_waiting_area]  =  (intState[mark_OF_Second_waiting_area] + 1);
                    intState[mark_OF_Pump_2]  =  (intState[mark_OF_Pump_2] - 1);
                    FIRE_xx2_OF_tank_is_full_2 = false;
                }
            }

            // Occurrence xx2_OF_tank_is_full_3
            if ((intState[mark_OF_Pump_3] >= 1) && (intState[mark_OF_Second_waiting_area] < 3))
            {

                if (FIRE_xx2_OF_tank_is_full_3)
                {
                    intState[mark_OF_Second_waiting_area]  =  (intState[mark_OF_Second_waiting_area] + 1);
                    intState[mark_OF_Pump_3]  =  (intState[mark_OF_Pump_3] - 1);
                    FIRE_xx2_OF_tank_is_full_3 = false;
                }
            }
            boolFailureState[mark_OF_Second_waiting_area_iseq_3] = intState[mark_OF_Second_waiting_area] == 3;
std::cout<<"interaction f ired\n";
        }

        std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram1::showFireableOccurrences()
        {
            std::vector<std::tuple<int, double, std::string, int>> list = {};
            cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

            if (list.size() > 0)
            {
                ins_transition_found = true;
                return list;
            }
            else
            {
                ins_transition_found = false;
            }

            if (intState[mark_OF_First_waiting_area] < 3)
            {
                cout << "0 : xx2_OF_Arrival : TRANSITION firing  DIST EXP (floatState[calculated_lambda_OF_Arrival])  INDUCING intState[mark_OF_First_waiting_area]  =  (intState[mark_OF_First_waiting_area] + 1)" << endl;
                list.push_back(make_tuple(0, floatState[calculated_lambda_OF_Arrival], "EXP", 0));
            }
            if (intState[mark_OF_Cashdesk] >= 1)
            {
                cout << "1 : xx2_OF_end_of_payment : TRANSITION firing  DIST EXP (floatState[calculated_lambda_OF_end_of_payment])  INDUCING intState[mark_OF_Cashdesk]  =  (intState[mark_OF_Cashdesk] - 1)" << endl;
                list.push_back(make_tuple(1, floatState[calculated_lambda_OF_end_of_payment], "EXP", 0));
            }
            if ((intState[mark_OF_Pump_1] >= 1) && (intState[mark_OF_Second_waiting_area] < 3))
            {
                cout << "2 : xx2_OF_tank_is_full_1 : TRANSITION firing  DIST EXP (floatState[calculated_lambda_OF_tank_is_full_1])  INDUCING intState[mark_OF_Second_waiting_area]  =  (intState[mark_OF_Second_waiting_area] + 1),mark_OF_Pump_1  =  (intState[mark_OF_Pump_1] - 1)" << endl;
                list.push_back(make_tuple(2, floatState[calculated_lambda_OF_tank_is_full_1], "EXP", 0));
            }
            if ((intState[mark_OF_Pump_2] >= 1) && (intState[mark_OF_Second_waiting_area] < 3))
            {
                cout << "3 : xx2_OF_tank_is_full_2 : TRANSITION firing  DIST EXP (floatState[calculated_lambda_OF_tank_is_full_2])  INDUCING intState[mark_OF_Second_waiting_area]  =  (intState[mark_OF_Second_waiting_area] + 1),mark_OF_Pump_2  =  (intState[mark_OF_Pump_2] - 1)" << endl;
                list.push_back(make_tuple(3, floatState[calculated_lambda_OF_tank_is_full_2], "EXP", 0));
            }
            if ((intState[mark_OF_Pump_3] >= 1) && (intState[mark_OF_Second_waiting_area] < 3))
            {
                cout << "4 : xx2_OF_tank_is_full_3 : TRANSITION firing  DIST EXP (floatState[calculated_lambda_OF_tank_is_full_3])  INDUCING intState[mark_OF_Second_waiting_area]  =  (intState[mark_OF_Second_waiting_area] + 1),mark_OF_Pump_3  =  (intState[mark_OF_Pump_3] - 1)" << endl;
                list.push_back(make_tuple(4, floatState[calculated_lambda_OF_tank_is_full_3], "EXP", 0));
            }
            return list;
        }


        void storm::figaro::FigaroProgram1::runOnceInteractionStep_default_step()
        {
            if ( !false )
            {
                floatState[calculated_lambda_OF_Arrival]  =  0.2;
            }



            floatState[calculated_lambda_OF_end_of_payment]  =  ( intState[mark_OF_Cashdesk]  * 0.1)  ;

            if ( !false )
            {
                floatState[calculated_lambda_OF_tank_is_full_1]  =  0.1;
            }

            if ( !false )
            {
                floatState[calculated_lambda_OF_tank_is_full_2]  =  0.1;
            }

            if ( !false )
            {
                floatState[calculated_lambda_OF_tank_is_full_3]  =  0.1;
            }

            if ((intState[mark_OF_First_waiting_area] >= 1) && (intState[mark_OF_Pump_1] < 1) )
            {
                intState[mark_OF_Pump_1]  =  (intState[mark_OF_Pump_1]  +  1);
                intState[mark_OF_First_waiting_area]  =  (intState[mark_OF_First_waiting_area] -  1);
            }

            if ((intState[mark_OF_First_waiting_area] >= 1) && (intState[mark_OF_Pump_2] < 1) )
            {
                intState[mark_OF_Pump_2]  =  (intState[mark_OF_Pump_2]  +  1);
                intState[mark_OF_First_waiting_area]  =  (intState[mark_OF_First_waiting_area] -  1);
            }

            if ((intState[mark_OF_First_waiting_area] >= 1) && (intState[mark_OF_Pump_3] < 1) )
            {
                intState[mark_OF_Pump_3]  =  (intState[mark_OF_Pump_3]  +  1);
                intState[mark_OF_First_waiting_area]  =  (intState[mark_OF_First_waiting_area] -  1);
            }

            if ((intState[mark_OF_Second_waiting_area] >= 1) && (intState[mark_OF_Cashdesk] < 2) )
            {
                intState[mark_OF_Cashdesk]  =  (intState[mark_OF_Cashdesk]  +  1);
                intState[mark_OF_Second_waiting_area]  =  (intState[mark_OF_Second_waiting_area]  - 1);
            }
            boolFailureState[mark_OF_Second_waiting_area_iseq_3] = (intState[mark_OF_Second_waiting_area] == 3);

        }

        void storm::figaro::FigaroProgram1::runInteractions() {
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

            cout << endl;
        }
        void storm::figaro::FigaroProgram1::printstatetuple(){
            std::cout<<"\n State information: (";
            for (int i=0; i<boolState.size(); i++)
            {
                std::cout<<boolState.at(i);
            }
            std::cout<<")";

        }
        int_fast64_t FigaroProgram1::stateSize() const{
            return numBoolState ;
        }

        void storm::figaro::FigaroProgram1::fireinsttransitiongroup(std::string user_input_ins)
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

