#include <iostream>
#include "FigaroModelfigaro_RBD.h"

        using namespace std;





        namespace storm{
            namespace figaro{
                /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
        //        storm::figaro::FigaroProgram_"+program_name+"::FigaroProgram_"+program_name+"()
        //        {
        //        for(int i=0; i < numBoolState; i++)
        //            boolState[i]=0;
        //
        //        }
        
void storm::figaro::FigaroProgram_figaro_RBD::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_not_linked_OF_b1 = false;
	boolState[fail_in_operation_OF_b1] = false;
	boolState[fail_to_start_OF_b1] = false;
	REINITIALISATION_OF_function_ok_OF_b1 = false;
	REINITIALISATION_OF_function_known_OF_b1 = false;
	enumState[state_OF_b1] = in_operation;
	REINITIALISATION_OF_not_linked_OF_b2 = false;
	boolState[fail_in_operation_OF_b2] = false;
	boolState[fail_to_start_OF_b2] = false;
	REINITIALISATION_OF_function_ok_OF_b2 = false;
	REINITIALISATION_OF_function_known_OF_b2 = false;
	enumState[state_OF_b2] = in_operation;
	REINITIALISATION_OF_not_linked_OF_b3 = false;
	boolState[fail_in_operation_OF_b3] = false;
	boolState[fail_to_start_OF_b3] = false;
	REINITIALISATION_OF_function_ok_OF_b3 = false;
	REINITIALISATION_OF_function_known_OF_b3 = false;
	enumState[state_OF_b3] = in_operation;
	REINITIALISATION_OF_not_linked_OF_b4 = false;
	boolState[fail_in_operation_OF_b4] = false;
	boolState[fail_to_start_OF_b4] = false;
	REINITIALISATION_OF_function_ok_OF_b4 = false;
	REINITIALISATION_OF_function_known_OF_b4 = false;
	enumState[state_OF_b4] = in_operation;
	REINITIALISATION_OF_not_linked_OF_b5 = false;
	boolState[fail_in_operation_OF_b5] = false;
	boolState[fail_to_start_OF_b5] = false;
	REINITIALISATION_OF_function_ok_OF_b5 = false;
	REINITIALISATION_OF_function_known_OF_b5 = false;
	enumState[state_OF_b5] = in_operation;
	REINITIALISATION_OF_not_linked_OF_de_1 = false;
	REINITIALISATION_OF_not_linked_OF_ds_1 = false;
	REINITIALISATION_OF_not_linked_OF_gate_2_4 = false;
	REINITIALISATION_OF_not_linked_OF_n_1 = false;
	REINITIALISATION_OF_not_linked_OF_n_2 = false;
	intState[nb_avail_repairmen_OF_rep3_4] = 1;
	intState[nb_avail_repairmen_OF_rep_1] = 1;
	intState[nb_avail_repairmen_OF_rep_2] = 1;
	intState[nb_avail_repairmen_OF_rep_5] = 1;
	REINITIALISATION_OF_not_linked_OF_sb1 = false;
	boolState[fail_in_operation_OF_sb1] = false;
	boolState[fail_to_start_OF_sb1] = false;
	REINITIALISATION_OF_function_ok_OF_sb1 = false;
	REINITIALISATION_OF_function_known_OF_sb1 = false;
	enumState[state_OF_sb1] = standby;
	REINITIALISATION_OF_not_linked_OF_sb2 = false;
	boolState[fail_in_operation_OF_sb2] = false;
	boolState[fail_to_start_OF_sb2] = false;
	REINITIALISATION_OF_function_ok_OF_sb2 = false;
	REINITIALISATION_OF_function_known_OF_sb2 = false;
	enumState[state_OF_sb2] = standby;
	REINITIALISATION_OF_not_linked_OF_sb3 = false;
	boolState[fail_in_operation_OF_sb3] = false;
	boolState[fail_to_start_OF_sb3] = false;
	REINITIALISATION_OF_function_ok_OF_sb3 = false;
	REINITIALISATION_OF_function_known_OF_sb3 = false;
	enumState[state_OF_sb3] = standby;
	REINITIALISATION_OF_not_linked_OF_sb4 = false;
	boolState[fail_in_operation_OF_sb4] = false;
	boolState[fail_to_start_OF_sb4] = false;
	REINITIALISATION_OF_function_ok_OF_sb4 = false;
	REINITIALISATION_OF_function_known_OF_sb4 = false;
	enumState[state_OF_sb4] = standby;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_occ_fail_to_start_OF_b1_INS_0 = false;
	FIRE_occ_fail_to_start_OF_b1_INS_1 = false;
	FIRE_occ_fail_in_operation_OF_b1 = false;
	FIRE_occ_repair_OF_b1 = false;
	FIRE_occ_fail_to_start_OF_b2_INS_4 = false;
	FIRE_occ_fail_to_start_OF_b2_INS_5 = false;
	FIRE_occ_fail_in_operation_OF_b2 = false;
	FIRE_occ_repair_OF_b2 = false;
	FIRE_occ_fail_to_start_OF_b3_INS_8 = false;
	FIRE_occ_fail_to_start_OF_b3_INS_9 = false;
	FIRE_occ_fail_in_operation_OF_b3 = false;
	FIRE_occ_repair_OF_b3 = false;
	FIRE_occ_fail_to_start_OF_b4_INS_12 = false;
	FIRE_occ_fail_to_start_OF_b4_INS_13 = false;
	FIRE_occ_fail_in_operation_OF_b4 = false;
	FIRE_occ_repair_OF_b4 = false;
	FIRE_occ_fail_to_start_OF_b5_INS_16 = false;
	FIRE_occ_fail_to_start_OF_b5_INS_17 = false;
	FIRE_occ_fail_in_operation_OF_b5 = false;
	FIRE_occ_repair_OF_b5 = false;
	FIRE_occ_fail_to_start_OF_sb1_INS_20 = false;
	FIRE_occ_fail_to_start_OF_sb1_INS_21 = false;
	FIRE_occ_fail_in_operation_OF_sb1 = false;
	FIRE_occ_repair_OF_sb1 = false;
	FIRE_occ_fail_to_start_OF_sb2_INS_24 = false;
	FIRE_occ_fail_to_start_OF_sb2_INS_25 = false;
	FIRE_occ_fail_in_operation_OF_sb2 = false;
	FIRE_occ_repair_OF_sb2 = false;
	FIRE_occ_fail_to_start_OF_sb3_INS_28 = false;
	FIRE_occ_fail_to_start_OF_sb3_INS_29 = false;
	FIRE_occ_fail_in_operation_OF_sb3 = false;
	FIRE_occ_repair_OF_sb3 = false;
	FIRE_occ_fail_to_start_OF_sb4_INS_32 = false;
	FIRE_occ_fail_to_start_OF_sb4_INS_33 = false;
	FIRE_occ_fail_in_operation_OF_sb4 = false;
	FIRE_occ_repair_OF_sb4 = false;

}

void storm::figaro::FigaroProgram_figaro_RBD::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_figaro_RBD::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_figaro_RBD::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[not_linked_OF_b1] | Value : " << boolState[not_linked_OF_b1] << endl;
	cout << "Attribute :  boolState[fail_in_operation_OF_b1] | Value : " << boolState[fail_in_operation_OF_b1] << endl;
	cout << "Attribute :  boolState[fail_to_start_OF_b1] | Value : " << boolState[fail_to_start_OF_b1] << endl;
	cout << "Attribute :  boolState[function_ok_OF_b1] | Value : " << boolState[function_ok_OF_b1] << endl;
	cout << "Attribute :  boolState[function_known_OF_b1] | Value : " << boolState[function_known_OF_b1] << endl;
	cout << "Attribute :  enumState[state_OF_b1] | Value : " << enumState[state_OF_b1] << endl;
	cout << "Attribute :  boolState[not_linked_OF_b2] | Value : " << boolState[not_linked_OF_b2] << endl;
	cout << "Attribute :  boolState[fail_in_operation_OF_b2] | Value : " << boolState[fail_in_operation_OF_b2] << endl;
	cout << "Attribute :  boolState[fail_to_start_OF_b2] | Value : " << boolState[fail_to_start_OF_b2] << endl;
	cout << "Attribute :  boolState[function_ok_OF_b2] | Value : " << boolState[function_ok_OF_b2] << endl;
	cout << "Attribute :  boolState[function_known_OF_b2] | Value : " << boolState[function_known_OF_b2] << endl;
	cout << "Attribute :  enumState[state_OF_b2] | Value : " << enumState[state_OF_b2] << endl;
	cout << "Attribute :  boolState[not_linked_OF_b3] | Value : " << boolState[not_linked_OF_b3] << endl;
	cout << "Attribute :  boolState[fail_in_operation_OF_b3] | Value : " << boolState[fail_in_operation_OF_b3] << endl;
	cout << "Attribute :  boolState[fail_to_start_OF_b3] | Value : " << boolState[fail_to_start_OF_b3] << endl;
	cout << "Attribute :  boolState[function_ok_OF_b3] | Value : " << boolState[function_ok_OF_b3] << endl;
	cout << "Attribute :  boolState[function_known_OF_b3] | Value : " << boolState[function_known_OF_b3] << endl;
	cout << "Attribute :  enumState[state_OF_b3] | Value : " << enumState[state_OF_b3] << endl;
	cout << "Attribute :  boolState[not_linked_OF_b4] | Value : " << boolState[not_linked_OF_b4] << endl;
	cout << "Attribute :  boolState[fail_in_operation_OF_b4] | Value : " << boolState[fail_in_operation_OF_b4] << endl;
	cout << "Attribute :  boolState[fail_to_start_OF_b4] | Value : " << boolState[fail_to_start_OF_b4] << endl;
	cout << "Attribute :  boolState[function_ok_OF_b4] | Value : " << boolState[function_ok_OF_b4] << endl;
	cout << "Attribute :  boolState[function_known_OF_b4] | Value : " << boolState[function_known_OF_b4] << endl;
	cout << "Attribute :  enumState[state_OF_b4] | Value : " << enumState[state_OF_b4] << endl;
	cout << "Attribute :  boolState[not_linked_OF_b5] | Value : " << boolState[not_linked_OF_b5] << endl;
	cout << "Attribute :  boolState[fail_in_operation_OF_b5] | Value : " << boolState[fail_in_operation_OF_b5] << endl;
	cout << "Attribute :  boolState[fail_to_start_OF_b5] | Value : " << boolState[fail_to_start_OF_b5] << endl;
	cout << "Attribute :  boolState[function_ok_OF_b5] | Value : " << boolState[function_ok_OF_b5] << endl;
	cout << "Attribute :  boolState[function_known_OF_b5] | Value : " << boolState[function_known_OF_b5] << endl;
	cout << "Attribute :  enumState[state_OF_b5] | Value : " << enumState[state_OF_b5] << endl;
	cout << "Attribute :  boolState[not_linked_OF_de_1] | Value : " << boolState[not_linked_OF_de_1] << endl;
	cout << "Attribute :  boolState[not_linked_OF_ds_1] | Value : " << boolState[not_linked_OF_ds_1] << endl;
	cout << "Attribute :  boolState[not_linked_OF_gate_2_4] | Value : " << boolState[not_linked_OF_gate_2_4] << endl;
	cout << "Attribute :  boolState[not_linked_OF_n_1] | Value : " << boolState[not_linked_OF_n_1] << endl;
	cout << "Attribute :  boolState[not_linked_OF_n_2] | Value : " << boolState[not_linked_OF_n_2] << endl;
	cout << "Attribute :  intState[nb_avail_repairmen_OF_rep3_4] | Value : " << intState[nb_avail_repairmen_OF_rep3_4] << endl;
	cout << "Attribute :  intState[nb_avail_repairmen_OF_rep_1] | Value : " << intState[nb_avail_repairmen_OF_rep_1] << endl;
	cout << "Attribute :  intState[nb_avail_repairmen_OF_rep_2] | Value : " << intState[nb_avail_repairmen_OF_rep_2] << endl;
	cout << "Attribute :  intState[nb_avail_repairmen_OF_rep_5] | Value : " << intState[nb_avail_repairmen_OF_rep_5] << endl;
	cout << "Attribute :  boolState[not_linked_OF_sb1] | Value : " << boolState[not_linked_OF_sb1] << endl;
	cout << "Attribute :  boolState[fail_in_operation_OF_sb1] | Value : " << boolState[fail_in_operation_OF_sb1] << endl;
	cout << "Attribute :  boolState[fail_to_start_OF_sb1] | Value : " << boolState[fail_to_start_OF_sb1] << endl;
	cout << "Attribute :  boolState[function_ok_OF_sb1] | Value : " << boolState[function_ok_OF_sb1] << endl;
	cout << "Attribute :  boolState[function_known_OF_sb1] | Value : " << boolState[function_known_OF_sb1] << endl;
	cout << "Attribute :  enumState[state_OF_sb1] | Value : " << enumState[state_OF_sb1] << endl;
	cout << "Attribute :  boolState[not_linked_OF_sb2] | Value : " << boolState[not_linked_OF_sb2] << endl;
	cout << "Attribute :  boolState[fail_in_operation_OF_sb2] | Value : " << boolState[fail_in_operation_OF_sb2] << endl;
	cout << "Attribute :  boolState[fail_to_start_OF_sb2] | Value : " << boolState[fail_to_start_OF_sb2] << endl;
	cout << "Attribute :  boolState[function_ok_OF_sb2] | Value : " << boolState[function_ok_OF_sb2] << endl;
	cout << "Attribute :  boolState[function_known_OF_sb2] | Value : " << boolState[function_known_OF_sb2] << endl;
	cout << "Attribute :  enumState[state_OF_sb2] | Value : " << enumState[state_OF_sb2] << endl;
	cout << "Attribute :  boolState[not_linked_OF_sb3] | Value : " << boolState[not_linked_OF_sb3] << endl;
	cout << "Attribute :  boolState[fail_in_operation_OF_sb3] | Value : " << boolState[fail_in_operation_OF_sb3] << endl;
	cout << "Attribute :  boolState[fail_to_start_OF_sb3] | Value : " << boolState[fail_to_start_OF_sb3] << endl;
	cout << "Attribute :  boolState[function_ok_OF_sb3] | Value : " << boolState[function_ok_OF_sb3] << endl;
	cout << "Attribute :  boolState[function_known_OF_sb3] | Value : " << boolState[function_known_OF_sb3] << endl;
	cout << "Attribute :  enumState[state_OF_sb3] | Value : " << enumState[state_OF_sb3] << endl;
	cout << "Attribute :  boolState[not_linked_OF_sb4] | Value : " << boolState[not_linked_OF_sb4] << endl;
	cout << "Attribute :  boolState[fail_in_operation_OF_sb4] | Value : " << boolState[fail_in_operation_OF_sb4] << endl;
	cout << "Attribute :  boolState[fail_to_start_OF_sb4] | Value : " << boolState[fail_to_start_OF_sb4] << endl;
	cout << "Attribute :  boolState[function_ok_OF_sb4] | Value : " << boolState[function_ok_OF_sb4] << endl;
	cout << "Attribute :  boolState[function_known_OF_sb4] | Value : " << boolState[function_known_OF_sb4] << endl;
	cout << "Attribute :  enumState[state_OF_sb4] | Value : " << enumState[state_OF_sb4] << endl;
}

bool storm::figaro::FigaroProgram_figaro_RBD::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_figaro_RBD::doReinitialisations()
{
	boolState[not_linked_OF_b1] = REINITIALISATION_OF_not_linked_OF_b1;
	boolState[function_ok_OF_b1] = REINITIALISATION_OF_function_ok_OF_b1;
	boolState[function_known_OF_b1] = REINITIALISATION_OF_function_known_OF_b1;
	boolState[not_linked_OF_b2] = REINITIALISATION_OF_not_linked_OF_b2;
	boolState[function_ok_OF_b2] = REINITIALISATION_OF_function_ok_OF_b2;
	boolState[function_known_OF_b2] = REINITIALISATION_OF_function_known_OF_b2;
	boolState[not_linked_OF_b3] = REINITIALISATION_OF_not_linked_OF_b3;
	boolState[function_ok_OF_b3] = REINITIALISATION_OF_function_ok_OF_b3;
	boolState[function_known_OF_b3] = REINITIALISATION_OF_function_known_OF_b3;
	boolState[not_linked_OF_b4] = REINITIALISATION_OF_not_linked_OF_b4;
	boolState[function_ok_OF_b4] = REINITIALISATION_OF_function_ok_OF_b4;
	boolState[function_known_OF_b4] = REINITIALISATION_OF_function_known_OF_b4;
	boolState[not_linked_OF_b5] = REINITIALISATION_OF_not_linked_OF_b5;
	boolState[function_ok_OF_b5] = REINITIALISATION_OF_function_ok_OF_b5;
	boolState[function_known_OF_b5] = REINITIALISATION_OF_function_known_OF_b5;
	boolState[not_linked_OF_de_1] = REINITIALISATION_OF_not_linked_OF_de_1;
	boolState[not_linked_OF_ds_1] = REINITIALISATION_OF_not_linked_OF_ds_1;
	boolState[not_linked_OF_gate_2_4] = REINITIALISATION_OF_not_linked_OF_gate_2_4;
	boolState[not_linked_OF_n_1] = REINITIALISATION_OF_not_linked_OF_n_1;
	boolState[not_linked_OF_n_2] = REINITIALISATION_OF_not_linked_OF_n_2;
	boolState[not_linked_OF_sb1] = REINITIALISATION_OF_not_linked_OF_sb1;
	boolState[function_ok_OF_sb1] = REINITIALISATION_OF_function_ok_OF_sb1;
	boolState[function_known_OF_sb1] = REINITIALISATION_OF_function_known_OF_sb1;
	boolState[not_linked_OF_sb2] = REINITIALISATION_OF_not_linked_OF_sb2;
	boolState[function_ok_OF_sb2] = REINITIALISATION_OF_function_ok_OF_sb2;
	boolState[function_known_OF_sb2] = REINITIALISATION_OF_function_known_OF_sb2;
	boolState[not_linked_OF_sb3] = REINITIALISATION_OF_not_linked_OF_sb3;
	boolState[function_ok_OF_sb3] = REINITIALISATION_OF_function_ok_OF_sb3;
	boolState[function_known_OF_sb3] = REINITIALISATION_OF_function_known_OF_sb3;
	boolState[not_linked_OF_sb4] = REINITIALISATION_OF_not_linked_OF_sb4;
	boolState[function_ok_OF_sb4] = REINITIALISATION_OF_function_ok_OF_sb4;
	boolState[function_known_OF_sb4] = REINITIALISATION_OF_function_known_OF_sb4;
}

void storm::figaro::FigaroProgram_figaro_RBD::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_occ_fail_to_start_OF_b1_INS_0 = true;
	}

	if (numFire == 1)
	{
		FIRE_occ_fail_to_start_OF_b1_INS_1 = true;
	}

	if (numFire == 2)
	{
		FIRE_occ_fail_in_operation_OF_b1 = true;
	}

	if (numFire == 3)
	{
		FIRE_occ_repair_OF_b1 = true;
	}

	if (numFire == 4)
	{
		FIRE_occ_fail_to_start_OF_b2_INS_4 = true;
	}

	if (numFire == 5)
	{
		FIRE_occ_fail_to_start_OF_b2_INS_5 = true;
	}

	if (numFire == 6)
	{
		FIRE_occ_fail_in_operation_OF_b2 = true;
	}

	if (numFire == 7)
	{
		FIRE_occ_repair_OF_b2 = true;
	}

	if (numFire == 8)
	{
		FIRE_occ_fail_to_start_OF_b3_INS_8 = true;
	}

	if (numFire == 9)
	{
		FIRE_occ_fail_to_start_OF_b3_INS_9 = true;
	}

	if (numFire == 10)
	{
		FIRE_occ_fail_in_operation_OF_b3 = true;
	}

	if (numFire == 11)
	{
		FIRE_occ_repair_OF_b3 = true;
	}

	if (numFire == 12)
	{
		FIRE_occ_fail_to_start_OF_b4_INS_12 = true;
	}

	if (numFire == 13)
	{
		FIRE_occ_fail_to_start_OF_b4_INS_13 = true;
	}

	if (numFire == 14)
	{
		FIRE_occ_fail_in_operation_OF_b4 = true;
	}

	if (numFire == 15)
	{
		FIRE_occ_repair_OF_b4 = true;
	}

	if (numFire == 16)
	{
		FIRE_occ_fail_to_start_OF_b5_INS_16 = true;
	}

	if (numFire == 17)
	{
		FIRE_occ_fail_to_start_OF_b5_INS_17 = true;
	}

	if (numFire == 18)
	{
		FIRE_occ_fail_in_operation_OF_b5 = true;
	}

	if (numFire == 19)
	{
		FIRE_occ_repair_OF_b5 = true;
	}

	if (numFire == 20)
	{
		FIRE_occ_fail_to_start_OF_sb1_INS_20 = true;
	}

	if (numFire == 21)
	{
		FIRE_occ_fail_to_start_OF_sb1_INS_21 = true;
	}

	if (numFire == 22)
	{
		FIRE_occ_fail_in_operation_OF_sb1 = true;
	}

	if (numFire == 23)
	{
		FIRE_occ_repair_OF_sb1 = true;
	}

	if (numFire == 24)
	{
		FIRE_occ_fail_to_start_OF_sb2_INS_24 = true;
	}

	if (numFire == 25)
	{
		FIRE_occ_fail_to_start_OF_sb2_INS_25 = true;
	}

	if (numFire == 26)
	{
		FIRE_occ_fail_in_operation_OF_sb2 = true;
	}

	if (numFire == 27)
	{
		FIRE_occ_repair_OF_sb2 = true;
	}

	if (numFire == 28)
	{
		FIRE_occ_fail_to_start_OF_sb3_INS_28 = true;
	}

	if (numFire == 29)
	{
		FIRE_occ_fail_to_start_OF_sb3_INS_29 = true;
	}

	if (numFire == 30)
	{
		FIRE_occ_fail_in_operation_OF_sb3 = true;
	}

	if (numFire == 31)
	{
		FIRE_occ_repair_OF_sb3 = true;
	}

	if (numFire == 32)
	{
		FIRE_occ_fail_to_start_OF_sb4_INS_32 = true;
	}

	if (numFire == 33)
	{
		FIRE_occ_fail_to_start_OF_sb4_INS_33 = true;
	}

	if (numFire == 34)
	{
		FIRE_occ_fail_in_operation_OF_sb4 = true;
	}

	if (numFire == 35)
	{
		FIRE_occ_repair_OF_sb4 = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence occ_fail_to_start_OF_b1

	if ((boolState[fail_to_start_OF_b1] == false) && (enumState[state_OF_b1] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_b1_INS_0) 
		{
			boolState[fail_to_start_OF_b1]  =  true;
			enumState[state_OF_b1]  =  waiting_for_repair;
			FIRE_occ_fail_to_start_OF_b1_INS_0 = false;
		}
	
	}
	if ((boolState[fail_to_start_OF_b1] == false) && (enumState[state_OF_b1] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_b1_INS_1) 
		{
			enumState[state_OF_b1]  =  in_operation;
			FIRE_occ_fail_to_start_OF_b1_INS_1 = false;
		}
	
	}
	// Occurrence occ_fail_in_operation_OF_b1
	if ((boolState[fail_in_operation_OF_b1] == false) && (enumState[state_OF_b1] == in_operation)) 
	{
		 
		if (FIRE_occ_fail_in_operation_OF_b1)
		{
			boolState[fail_in_operation_OF_b1]  =  true;
			enumState[state_OF_b1]  =  waiting_for_repair;
			FIRE_occ_fail_in_operation_OF_b1 = false;
		}
	}

	// Occurrence occ_repair_OF_b1
	if (((boolState[fail_in_operation_OF_b1] == true) || (boolState[fail_to_start_OF_b1] ==
true)) && (enumState[state_OF_b1] == under_repair)) 
	{
		 
		if (FIRE_occ_repair_OF_b1)
		{
			boolState[fail_in_operation_OF_b1]  =  false;
			boolState[fail_to_start_OF_b1]  =  false;
			enumState[state_OF_b1]  =  in_operation;
			intState[nb_avail_repairmen_OF_rep_1]  =  (intState[nb_avail_repairmen_OF_rep_1] + 1);
			FIRE_occ_repair_OF_b1 = false;
		}
	}

	// Occurrence occ_fail_to_start_OF_b2

	if ((boolState[fail_to_start_OF_b2] == false) && (enumState[state_OF_b2] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_b2_INS_4) 
		{
			boolState[fail_to_start_OF_b2]  =  true;
			enumState[state_OF_b2]  =  waiting_for_repair;
			FIRE_occ_fail_to_start_OF_b2_INS_4 = false;
		}
	
	}
	if ((boolState[fail_to_start_OF_b2] == false) && (enumState[state_OF_b2] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_b2_INS_5) 
		{
			enumState[state_OF_b2]  =  in_operation;
			FIRE_occ_fail_to_start_OF_b2_INS_5 = false;
		}
	
	}
	// Occurrence occ_fail_in_operation_OF_b2
	if ((boolState[fail_in_operation_OF_b2] == false) && (enumState[state_OF_b2] == in_operation)) 
	{
		 
		if (FIRE_occ_fail_in_operation_OF_b2)
		{
			boolState[fail_in_operation_OF_b2]  =  true;
			enumState[state_OF_b2]  =  waiting_for_repair;
			FIRE_occ_fail_in_operation_OF_b2 = false;
		}
	}

	// Occurrence occ_repair_OF_b2
	if (((boolState[fail_in_operation_OF_b2] == true) || (boolState[fail_to_start_OF_b2] ==
true)) && (enumState[state_OF_b2] == under_repair)) 
	{
		 
		if (FIRE_occ_repair_OF_b2)
		{
			boolState[fail_in_operation_OF_b2]  =  false;
			boolState[fail_to_start_OF_b2]  =  false;
			enumState[state_OF_b2]  =  in_operation;
			intState[nb_avail_repairmen_OF_rep_2]  =  (intState[nb_avail_repairmen_OF_rep_2] + 1);
			FIRE_occ_repair_OF_b2 = false;
		}
	}

	// Occurrence occ_fail_to_start_OF_b3

	if ((boolState[fail_to_start_OF_b3] == false) && (enumState[state_OF_b3] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_b3_INS_8) 
		{
			boolState[fail_to_start_OF_b3]  =  true;
			enumState[state_OF_b3]  =  waiting_for_repair;
			FIRE_occ_fail_to_start_OF_b3_INS_8 = false;
		}
	
	}
	if ((boolState[fail_to_start_OF_b3] == false) && (enumState[state_OF_b3] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_b3_INS_9) 
		{
			enumState[state_OF_b3]  =  in_operation;
			FIRE_occ_fail_to_start_OF_b3_INS_9 = false;
		}
	
	}
	// Occurrence occ_fail_in_operation_OF_b3
	if ((boolState[fail_in_operation_OF_b3] == false) && (enumState[state_OF_b3] == in_operation)) 
	{
		 
		if (FIRE_occ_fail_in_operation_OF_b3)
		{
			boolState[fail_in_operation_OF_b3]  =  true;
			enumState[state_OF_b3]  =  waiting_for_repair;
			FIRE_occ_fail_in_operation_OF_b3 = false;
		}
	}

	// Occurrence occ_repair_OF_b3
	if (((boolState[fail_in_operation_OF_b3] == true) || (boolState[fail_to_start_OF_b3] ==
true)) && (enumState[state_OF_b3] == under_repair)) 
	{
		 
		if (FIRE_occ_repair_OF_b3)
		{
			boolState[fail_in_operation_OF_b3]  =  false;
			boolState[fail_to_start_OF_b3]  =  false;
			enumState[state_OF_b3]  =  in_operation;
			intState[nb_avail_repairmen_OF_rep3_4]  =  (intState[nb_avail_repairmen_OF_rep3_4] + 1);
			FIRE_occ_repair_OF_b3 = false;
		}
	}

	// Occurrence occ_fail_to_start_OF_b4

	if ((boolState[fail_to_start_OF_b4] == false) && (enumState[state_OF_b4] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_b4_INS_12) 
		{
			boolState[fail_to_start_OF_b4]  =  true;
			enumState[state_OF_b4]  =  waiting_for_repair;
			FIRE_occ_fail_to_start_OF_b4_INS_12 = false;
		}
	
	}
	if ((boolState[fail_to_start_OF_b4] == false) && (enumState[state_OF_b4] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_b4_INS_13) 
		{
			enumState[state_OF_b4]  =  in_operation;
			FIRE_occ_fail_to_start_OF_b4_INS_13 = false;
		}
	
	}
	// Occurrence occ_fail_in_operation_OF_b4
	if ((boolState[fail_in_operation_OF_b4] == false) && (enumState[state_OF_b4] == in_operation)) 
	{
		 
		if (FIRE_occ_fail_in_operation_OF_b4)
		{
			boolState[fail_in_operation_OF_b4]  =  true;
			enumState[state_OF_b4]  =  waiting_for_repair;
			FIRE_occ_fail_in_operation_OF_b4 = false;
		}
	}

	// Occurrence occ_repair_OF_b4
	if (((boolState[fail_in_operation_OF_b4] == true) || (boolState[fail_to_start_OF_b4] ==
true)) && (enumState[state_OF_b4] == under_repair)) 
	{
		 
		if (FIRE_occ_repair_OF_b4)
		{
			boolState[fail_in_operation_OF_b4]  =  false;
			boolState[fail_to_start_OF_b4]  =  false;
			enumState[state_OF_b4]  =  in_operation;
			intState[nb_avail_repairmen_OF_rep3_4]  =  (intState[nb_avail_repairmen_OF_rep3_4] + 1);
			FIRE_occ_repair_OF_b4 = false;
		}
	}

	// Occurrence occ_fail_to_start_OF_b5

	if ((boolState[fail_to_start_OF_b5] == false) && (enumState[state_OF_b5] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_b5_INS_16) 
		{
			boolState[fail_to_start_OF_b5]  =  true;
			enumState[state_OF_b5]  =  waiting_for_repair;
			FIRE_occ_fail_to_start_OF_b5_INS_16 = false;
		}
	
	}
	if ((boolState[fail_to_start_OF_b5] == false) && (enumState[state_OF_b5] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_b5_INS_17) 
		{
			enumState[state_OF_b5]  =  in_operation;
			FIRE_occ_fail_to_start_OF_b5_INS_17 = false;
		}
	
	}
	// Occurrence occ_fail_in_operation_OF_b5
	if ((boolState[fail_in_operation_OF_b5] == false) && (enumState[state_OF_b5] == in_operation)) 
	{
		 
		if (FIRE_occ_fail_in_operation_OF_b5)
		{
			boolState[fail_in_operation_OF_b5]  =  true;
			enumState[state_OF_b5]  =  waiting_for_repair;
			FIRE_occ_fail_in_operation_OF_b5 = false;
		}
	}

	// Occurrence occ_repair_OF_b5
	if (((boolState[fail_in_operation_OF_b5] == true) || (boolState[fail_to_start_OF_b5] ==
true)) && (enumState[state_OF_b5] == under_repair)) 
	{
		 
		if (FIRE_occ_repair_OF_b5)
		{
			boolState[fail_in_operation_OF_b5]  =  false;
			boolState[fail_to_start_OF_b5]  =  false;
			enumState[state_OF_b5]  =  in_operation;
			intState[nb_avail_repairmen_OF_rep_5]  =  (intState[nb_avail_repairmen_OF_rep_5] + 1);
			FIRE_occ_repair_OF_b5 = false;
		}
	}

	// Occurrence occ_fail_to_start_OF_sb1

	if ((boolState[fail_to_start_OF_sb1] == false) && (enumState[state_OF_sb1] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_sb1_INS_20) 
		{
			boolState[fail_to_start_OF_sb1]  =  true;
			enumState[state_OF_sb1]  =  waiting_for_repair;
			FIRE_occ_fail_to_start_OF_sb1_INS_20 = false;
		}
	
	}
	if ((boolState[fail_to_start_OF_sb1] == false) && (enumState[state_OF_sb1] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_sb1_INS_21) 
		{
			enumState[state_OF_sb1]  =  in_operation;
			FIRE_occ_fail_to_start_OF_sb1_INS_21 = false;
		}
	
	}
	// Occurrence occ_fail_in_operation_OF_sb1
	if ((boolState[fail_in_operation_OF_sb1] == false) && (enumState[state_OF_sb1] == in_operation)) 
	{
		 
		if (FIRE_occ_fail_in_operation_OF_sb1)
		{
			boolState[fail_in_operation_OF_sb1]  =  true;
			enumState[state_OF_sb1]  =  waiting_for_repair;
			FIRE_occ_fail_in_operation_OF_sb1 = false;
		}
	}

	// Occurrence occ_repair_OF_sb1
	if (((boolState[fail_in_operation_OF_sb1] == true) || (boolState[fail_to_start_OF_sb1]
== true)) && (enumState[state_OF_sb1] == under_repair)) 
	{
		 
		if (FIRE_occ_repair_OF_sb1)
		{
			boolState[fail_in_operation_OF_sb1]  =  false;
			boolState[fail_to_start_OF_sb1]  =  false;
			enumState[state_OF_sb1]  =  in_operation;
			intState[nb_avail_repairmen_OF_rep_1]  =  (intState[nb_avail_repairmen_OF_rep_1] + 1);
			FIRE_occ_repair_OF_sb1 = false;
		}
	}

	// Occurrence occ_fail_to_start_OF_sb2

	if ((boolState[fail_to_start_OF_sb2] == false) && (enumState[state_OF_sb2] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_sb2_INS_24) 
		{
			boolState[fail_to_start_OF_sb2]  =  true;
			enumState[state_OF_sb2]  =  waiting_for_repair;
			FIRE_occ_fail_to_start_OF_sb2_INS_24 = false;
		}
	
	}
	if ((boolState[fail_to_start_OF_sb2] == false) && (enumState[state_OF_sb2] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_sb2_INS_25) 
		{
			enumState[state_OF_sb2]  =  in_operation;
			FIRE_occ_fail_to_start_OF_sb2_INS_25 = false;
		}
	
	}
	// Occurrence occ_fail_in_operation_OF_sb2
	if ((boolState[fail_in_operation_OF_sb2] == false) && (enumState[state_OF_sb2] == in_operation)) 
	{
		 
		if (FIRE_occ_fail_in_operation_OF_sb2)
		{
			boolState[fail_in_operation_OF_sb2]  =  true;
			enumState[state_OF_sb2]  =  waiting_for_repair;
			FIRE_occ_fail_in_operation_OF_sb2 = false;
		}
	}

	// Occurrence occ_repair_OF_sb2
	if (((boolState[fail_in_operation_OF_sb2] == true) || (boolState[fail_to_start_OF_sb2]
== true)) && (enumState[state_OF_sb2] == under_repair)) 
	{
		 
		if (FIRE_occ_repair_OF_sb2)
		{
			boolState[fail_in_operation_OF_sb2]  =  false;
			boolState[fail_to_start_OF_sb2]  =  false;
			enumState[state_OF_sb2]  =  in_operation;
			intState[nb_avail_repairmen_OF_rep_2]  =  (intState[nb_avail_repairmen_OF_rep_2] + 1);
			FIRE_occ_repair_OF_sb2 = false;
		}
	}

	// Occurrence occ_fail_to_start_OF_sb3

	if ((boolState[fail_to_start_OF_sb3] == false) && (enumState[state_OF_sb3] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_sb3_INS_28) 
		{
			boolState[fail_to_start_OF_sb3]  =  true;
			enumState[state_OF_sb3]  =  waiting_for_repair;
			FIRE_occ_fail_to_start_OF_sb3_INS_28 = false;
		}
	
	}
	if ((boolState[fail_to_start_OF_sb3] == false) && (enumState[state_OF_sb3] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_sb3_INS_29) 
		{
			enumState[state_OF_sb3]  =  in_operation;
			FIRE_occ_fail_to_start_OF_sb3_INS_29 = false;
		}
	
	}
	// Occurrence occ_fail_in_operation_OF_sb3
	if ((boolState[fail_in_operation_OF_sb3] == false) && (enumState[state_OF_sb3] == in_operation)) 
	{
		 
		if (FIRE_occ_fail_in_operation_OF_sb3)
		{
			boolState[fail_in_operation_OF_sb3]  =  true;
			enumState[state_OF_sb3]  =  waiting_for_repair;
			FIRE_occ_fail_in_operation_OF_sb3 = false;
		}
	}

	// Occurrence occ_repair_OF_sb3
	if (((boolState[fail_in_operation_OF_sb3] == true) || (boolState[fail_to_start_OF_sb3]
== true)) && (enumState[state_OF_sb3] == under_repair)) 
	{
		 
		if (FIRE_occ_repair_OF_sb3)
		{
			boolState[fail_in_operation_OF_sb3]  =  false;
			boolState[fail_to_start_OF_sb3]  =  false;
			enumState[state_OF_sb3]  =  in_operation;
			intState[nb_avail_repairmen_OF_rep3_4]  =  (intState[nb_avail_repairmen_OF_rep3_4] + 1);
			FIRE_occ_repair_OF_sb3 = false;
		}
	}

	// Occurrence occ_fail_to_start_OF_sb4

	if ((boolState[fail_to_start_OF_sb4] == false) && (enumState[state_OF_sb4] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_sb4_INS_32) 
		{
			boolState[fail_to_start_OF_sb4]  =  true;
			enumState[state_OF_sb4]  =  waiting_for_repair;
			FIRE_occ_fail_to_start_OF_sb4_INS_32 = false;
		}
	
	}
	if ((boolState[fail_to_start_OF_sb4] == false) && (enumState[state_OF_sb4] == start_demand)) 
	{
	
		
		if (FIRE_occ_fail_to_start_OF_sb4_INS_33) 
		{
			enumState[state_OF_sb4]  =  in_operation;
			FIRE_occ_fail_to_start_OF_sb4_INS_33 = false;
		}
	
	}
	// Occurrence occ_fail_in_operation_OF_sb4
	if ((boolState[fail_in_operation_OF_sb4] == false) && (enumState[state_OF_sb4] == in_operation)) 
	{
		 
		if (FIRE_occ_fail_in_operation_OF_sb4)
		{
			boolState[fail_in_operation_OF_sb4]  =  true;
			enumState[state_OF_sb4]  =  waiting_for_repair;
			FIRE_occ_fail_in_operation_OF_sb4 = false;
		}
	}

	// Occurrence occ_repair_OF_sb4
	if (((boolState[fail_in_operation_OF_sb4] == true) || (boolState[fail_to_start_OF_sb4]
== true)) && (enumState[state_OF_sb4] == under_repair)) 
	{
		 
		if (FIRE_occ_repair_OF_sb4)
		{
			boolState[fail_in_operation_OF_sb4]  =  false;
			boolState[fail_to_start_OF_sb4]  =  false;
			enumState[state_OF_sb4]  =  in_operation;
			intState[nb_avail_repairmen_OF_rep3_4]  =  (intState[nb_avail_repairmen_OF_rep3_4] + 1);
			FIRE_occ_repair_OF_sb4 = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_figaro_RBD::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if ((boolState[fail_to_start_OF_b1] == false) && (enumState[state_OF_b1] == start_demand))
	{
		cout << "0 :  INS_SUB_COUNT (1) |FAULT | fail_to_start  LABEL \"b1 failed to start\" | DIST INS (0.001) | INDUCING boolState[fail_to_start_OF_b1]  =  TRUE,state_OF_b1  =  waiting_for_repair" << endl;
		list.push_back(make_tuple(0, 0.001, "INS", 1));
	}
	if ((boolState[fail_to_start_OF_b1] == false) && (enumState[state_OF_b1] == start_demand))
	{
		cout << "1 :  INS_SUB_COUNT (1) |TRANSITION | start | DIST INS (0.999) | INDUCING enumState[state_OF_b1]  =  in_operation" << endl;
		list.push_back(make_tuple(1, 0.999, "INS", 1));
	}
	if ((boolState[fail_to_start_OF_b2] == false) && (enumState[state_OF_b2] == start_demand))
	{
		cout << "4 :  INS_SUB_COUNT (2) |FAULT | fail_to_start  LABEL \"b2 failed to start\" | DIST INS (0.001) | INDUCING boolState[fail_to_start_OF_b2]  =  TRUE,state_OF_b2  =  waiting_for_repair" << endl;
		list.push_back(make_tuple(4, 0.001, "INS", 2));
	}
	if ((boolState[fail_to_start_OF_b2] == false) && (enumState[state_OF_b2] == start_demand))
	{
		cout << "5 :  INS_SUB_COUNT (2) |TRANSITION | start | DIST INS (0.999) | INDUCING enumState[state_OF_b2]  =  in_operation" << endl;
		list.push_back(make_tuple(5, 0.999, "INS", 2));
	}
	if ((boolState[fail_to_start_OF_b3] == false) && (enumState[state_OF_b3] == start_demand))
	{
		cout << "8 :  INS_SUB_COUNT (3) |FAULT | fail_to_start  LABEL \"b3 failed to start\" | DIST INS (0.001) | INDUCING boolState[fail_to_start_OF_b3]  =  TRUE,state_OF_b3  =  waiting_for_repair" << endl;
		list.push_back(make_tuple(8, 0.001, "INS", 3));
	}
	if ((boolState[fail_to_start_OF_b3] == false) && (enumState[state_OF_b3] == start_demand))
	{
		cout << "9 :  INS_SUB_COUNT (3) |TRANSITION | start | DIST INS (0.999) | INDUCING enumState[state_OF_b3]  =  in_operation" << endl;
		list.push_back(make_tuple(9, 0.999, "INS", 3));
	}
	if ((boolState[fail_to_start_OF_b4] == false) && (enumState[state_OF_b4] == start_demand))
	{
		cout << "12 :  INS_SUB_COUNT (4) |FAULT | fail_to_start  LABEL \"b4 failed to start\" | DIST INS (0.001) | INDUCING boolState[fail_to_start_OF_b4]  =  TRUE,state_OF_b4  =  waiting_for_repair" << endl;
		list.push_back(make_tuple(12, 0.001, "INS", 4));
	}
	if ((boolState[fail_to_start_OF_b4] == false) && (enumState[state_OF_b4] == start_demand))
	{
		cout << "13 :  INS_SUB_COUNT (4) |TRANSITION | start | DIST INS (0.999) | INDUCING enumState[state_OF_b4]  =  in_operation" << endl;
		list.push_back(make_tuple(13, 0.999, "INS", 4));
	}
	if ((boolState[fail_to_start_OF_b5] == false) && (enumState[state_OF_b5] == start_demand))
	{
		cout << "16 :  INS_SUB_COUNT (5) |FAULT | fail_to_start  LABEL \"b5 failed to start\" | DIST INS (0.001) | INDUCING boolState[fail_to_start_OF_b5]  =  TRUE,state_OF_b5  =  waiting_for_repair" << endl;
		list.push_back(make_tuple(16, 0.001, "INS", 5));
	}
	if ((boolState[fail_to_start_OF_b5] == false) && (enumState[state_OF_b5] == start_demand))
	{
		cout << "17 :  INS_SUB_COUNT (5) |TRANSITION | start | DIST INS (0.999) | INDUCING enumState[state_OF_b5]  =  in_operation" << endl;
		list.push_back(make_tuple(17, 0.999, "INS", 5));
	}
	if ((boolState[fail_to_start_OF_sb1] == false) && (enumState[state_OF_sb1] == start_demand))
	{
		cout << "20 :  INS_SUB_COUNT (6) |FAULT | fail_to_start  LABEL \"sb1 failed to start\" | DIST INS (0.01) | INDUCING boolState[fail_to_start_OF_sb1]  =  TRUE,state_OF_sb1  =  waiting_for_repair" << endl;
		list.push_back(make_tuple(20, 0.01, "INS", 6));
	}
	if ((boolState[fail_to_start_OF_sb1] == false) && (enumState[state_OF_sb1] == start_demand))
	{
		cout << "21 :  INS_SUB_COUNT (6) |TRANSITION | start | DIST INS (0.99) | INDUCING enumState[state_OF_sb1]  =  in_operation" << endl;
		list.push_back(make_tuple(21, 0.99, "INS", 6));
	}
	if ((boolState[fail_to_start_OF_sb2] == false) && (enumState[state_OF_sb2] == start_demand))
	{
		cout << "24 :  INS_SUB_COUNT (7) |FAULT | fail_to_start  LABEL \"sb2 failed to start\" | DIST INS (0.01) | INDUCING boolState[fail_to_start_OF_sb2]  =  TRUE,state_OF_sb2  =  waiting_for_repair" << endl;
		list.push_back(make_tuple(24, 0.01, "INS", 7));
	}
	if ((boolState[fail_to_start_OF_sb2] == false) && (enumState[state_OF_sb2] == start_demand))
	{
		cout << "25 :  INS_SUB_COUNT (7) |TRANSITION | start | DIST INS (0.99) | INDUCING enumState[state_OF_sb2]  =  in_operation" << endl;
		list.push_back(make_tuple(25, 0.99, "INS", 7));
	}
	if ((boolState[fail_to_start_OF_sb3] == false) && (enumState[state_OF_sb3] == start_demand))
	{
		cout << "28 :  INS_SUB_COUNT (8) |FAULT | fail_to_start  LABEL \"sb3 failed to start\" | DIST INS (0.02) | INDUCING boolState[fail_to_start_OF_sb3]  =  TRUE,state_OF_sb3  =  waiting_for_repair" << endl;
		list.push_back(make_tuple(28, 0.02, "INS", 8));
	}
	if ((boolState[fail_to_start_OF_sb3] == false) && (enumState[state_OF_sb3] == start_demand))
	{
		cout << "29 :  INS_SUB_COUNT (8) |TRANSITION | start | DIST INS (0.98) | INDUCING enumState[state_OF_sb3]  =  in_operation" << endl;
		list.push_back(make_tuple(29, 0.98, "INS", 8));
	}
	if ((boolState[fail_to_start_OF_sb4] == false) && (enumState[state_OF_sb4] == start_demand))
	{
		cout << "32 :  INS_SUB_COUNT (9) |FAULT | fail_to_start  LABEL \"sb4 failed to start\" | DIST INS (0.02) | INDUCING boolState[fail_to_start_OF_sb4]  =  TRUE,state_OF_sb4  =  waiting_for_repair" << endl;
		list.push_back(make_tuple(32, 0.02, "INS", 9));
	}
	if ((boolState[fail_to_start_OF_sb4] == false) && (enumState[state_OF_sb4] == start_demand))
	{
		cout << "33 :  INS_SUB_COUNT (9) |TRANSITION | start | DIST INS (0.98) | INDUCING enumState[state_OF_sb4]  =  in_operation" << endl;
		list.push_back(make_tuple(33, 0.98, "INS", 9));
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
     
	if ((boolState[fail_in_operation_OF_b1] == false) && (enumState[state_OF_b1] == in_operation))
	{
		cout << "2 : occ_fail_in_operation_OF_b1 : FAULT fail_in_operation  LABEL \"b1 failed in operation\"  DIST EXP (1e-06)  INDUCING boolState[fail_in_operation_OF_b1]  =  TRUE,state_OF_b1  =  'waiting_for_repair'" << endl;
		list.push_back(make_tuple(2, 1e-06, "EXP", 0));
	}
	if (((boolState[fail_in_operation_OF_b1] == true) || (boolState[fail_to_start_OF_b1] ==true)) && (enumState[state_OF_b1] == under_repair))
	{
		cout << "3 : occ_repair_OF_b1 : REPAIR repair  DIST EXP (0.002)  INDUCING boolState[fail_in_operation_OF_b1]  =  FALSE,fail_to_start_OF_b1  =  FALSE,state_OF_b1  =  'in_operation',nb_avail_repairmen_OF_rep_1  =  (intState[nb_avail_repairmen_OF_rep_1] + 1)" << endl;
		list.push_back(make_tuple(3, 0.002, "EXP", 0));
	}
	if ((boolState[fail_in_operation_OF_b2] == false) && (enumState[state_OF_b2] == in_operation))
	{
		cout << "6 : occ_fail_in_operation_OF_b2 : FAULT fail_in_operation  LABEL \"b2 failed in operation\"  DIST EXP (2e-06)  INDUCING boolState[fail_in_operation_OF_b2]  =  TRUE,state_OF_b2  =  'waiting_for_repair'" << endl;
		list.push_back(make_tuple(6, 2e-06, "EXP", 0));
	}
	if (((boolState[fail_in_operation_OF_b2] == true) || (boolState[fail_to_start_OF_b2] ==true)) && (enumState[state_OF_b2] == under_repair))
	{
		cout << "7 : occ_repair_OF_b2 : REPAIR repair  DIST EXP (0.005)  INDUCING boolState[fail_in_operation_OF_b2]  =  FALSE,fail_to_start_OF_b2  =  FALSE,state_OF_b2  =  'in_operation',nb_avail_repairmen_OF_rep_2  =  (intState[nb_avail_repairmen_OF_rep_2] + 1)" << endl;
		list.push_back(make_tuple(7, 0.005, "EXP", 0));
	}
	if ((boolState[fail_in_operation_OF_b3] == false) && (enumState[state_OF_b3] == in_operation))
	{
		cout << "10 : occ_fail_in_operation_OF_b3 : FAULT fail_in_operation  LABEL \"b3 failed in operation\"  DIST EXP (5e-05)  INDUCING boolState[fail_in_operation_OF_b3]  =  TRUE,state_OF_b3  =  'waiting_for_repair'" << endl;
		list.push_back(make_tuple(10, 5e-05, "EXP", 0));
	}
	if (((boolState[fail_in_operation_OF_b3] == true) || (boolState[fail_to_start_OF_b3] ==true)) && (enumState[state_OF_b3] == under_repair))
	{
		cout << "11 : occ_repair_OF_b3 : REPAIR repair  DIST EXP (0.01)  INDUCING boolState[fail_in_operation_OF_b3]  =  FALSE,fail_to_start_OF_b3  =  FALSE,state_OF_b3  =  'in_operation',nb_avail_repairmen_OF_rep3_4  =  (intState[nb_avail_repairmen_OF_rep3_4] + 1)" << endl;
		list.push_back(make_tuple(11, 0.01, "EXP", 0));
	}
	if ((boolState[fail_in_operation_OF_b4] == false) && (enumState[state_OF_b4] == in_operation))
	{
		cout << "14 : occ_fail_in_operation_OF_b4 : FAULT fail_in_operation  LABEL \"b4 failed in operation\"  DIST EXP (1e-05)  INDUCING boolState[fail_in_operation_OF_b4]  =  TRUE,state_OF_b4  =  'waiting_for_repair'" << endl;
		list.push_back(make_tuple(14, 1e-05, "EXP", 0));
	}
	if (((boolState[fail_in_operation_OF_b4] == true) || (boolState[fail_to_start_OF_b4] ==true)) && (enumState[state_OF_b4] == under_repair))
	{
		cout << "15 : occ_repair_OF_b4 : REPAIR repair  DIST EXP (0.02)  INDUCING boolState[fail_in_operation_OF_b4]  =  FALSE,fail_to_start_OF_b4  =  FALSE,state_OF_b4  =  'in_operation',nb_avail_repairmen_OF_rep3_4  =  (intState[nb_avail_repairmen_OF_rep3_4] + 1)" << endl;
		list.push_back(make_tuple(15, 0.02, "EXP", 0));
	}
	if ((boolState[fail_in_operation_OF_b5] == false) && (enumState[state_OF_b5] == in_operation))
	{
		cout << "18 : occ_fail_in_operation_OF_b5 : FAULT fail_in_operation  LABEL \"b5 failed in operation\"  DIST EXP (0.002)  INDUCING boolState[fail_in_operation_OF_b5]  =  TRUE,state_OF_b5  =  'waiting_for_repair'" << endl;
		list.push_back(make_tuple(18, 0.002, "EXP", 0));
	}
	if (((boolState[fail_in_operation_OF_b5] == true) || (boolState[fail_to_start_OF_b5] ==true)) && (enumState[state_OF_b5] == under_repair))
	{
		cout << "19 : occ_repair_OF_b5 : REPAIR repair  DIST EXP (1)  INDUCING boolState[fail_in_operation_OF_b5]  =  FALSE,fail_to_start_OF_b5  =  FALSE,state_OF_b5  =  'in_operation',nb_avail_repairmen_OF_rep_5  =  (intState[nb_avail_repairmen_OF_rep_5] + 1)" << endl;
		list.push_back(make_tuple(19, 1, "EXP", 0));
	}
	if ((boolState[fail_in_operation_OF_sb1] == false) && (enumState[state_OF_sb1] == in_operation))
	{
		cout << "22 : occ_fail_in_operation_OF_sb1 : FAULT fail_in_operation  LABEL \"sb1 failed in operation\"  DIST EXP (0.0001)  INDUCING boolState[fail_in_operation_OF_sb1]  =  TRUE,state_OF_sb1  =  'waiting_for_repair'" << endl;
		list.push_back(make_tuple(22, 0.0001, "EXP", 0));
	}
	if (((boolState[fail_in_operation_OF_sb1] == true) || (boolState[fail_to_start_OF_sb1] == true)) && (enumState[state_OF_sb1] == under_repair))
	{
		cout << "23 : occ_repair_OF_sb1 : REPAIR repair  DIST EXP (0.02)  INDUCING boolState[fail_in_operation_OF_sb1]  =  FALSE,fail_to_start_OF_sb1  =  FALSE,state_OF_sb1  =  'in_operation',nb_avail_repairmen_OF_rep_1  =  (intState[nb_avail_repairmen_OF_rep_1] + 1)" << endl;
		list.push_back(make_tuple(23, 0.02, "EXP", 0));
	}
	if ((boolState[fail_in_operation_OF_sb2] == false) && (enumState[state_OF_sb2] == in_operation))
	{
		cout << "26 : occ_fail_in_operation_OF_sb2 : FAULT fail_in_operation  LABEL \"sb2 failed in operation\"  DIST EXP (0.0001)  INDUCING boolState[fail_in_operation_OF_sb2]  =  TRUE,state_OF_sb2  =  'waiting_for_repair'" << endl;
		list.push_back(make_tuple(26, 0.0001, "EXP", 0));
	}
	if (((boolState[fail_in_operation_OF_sb2] == true) || (boolState[fail_to_start_OF_sb2] == true)) && (enumState[state_OF_sb2] == under_repair))
	{
		cout << "27 : occ_repair_OF_sb2 : REPAIR repair  DIST EXP (0.02)  INDUCING boolState[fail_in_operation_OF_sb2]  =  FALSE,fail_to_start_OF_sb2  =  FALSE,state_OF_sb2  =  'in_operation',nb_avail_repairmen_OF_rep_2  =  (intState[nb_avail_repairmen_OF_rep_2] + 1)" << endl;
		list.push_back(make_tuple(27, 0.02, "EXP", 0));
	}
	if ((boolState[fail_in_operation_OF_sb3] == false) && (enumState[state_OF_sb3] == in_operation))
	{
		cout << "30 : occ_fail_in_operation_OF_sb3 : FAULT fail_in_operation  LABEL \"sb3 failed in operation\"  DIST EXP (0.001)  INDUCING boolState[fail_in_operation_OF_sb3]  =  TRUE,state_OF_sb3  =  'waiting_for_repair'" << endl;
		list.push_back(make_tuple(30, 0.001, "EXP", 0));
	}
	if (((boolState[fail_in_operation_OF_sb3] == true) || (boolState[fail_to_start_OF_sb3] == true)) && (enumState[state_OF_sb3] == under_repair))
	{
		cout << "31 : occ_repair_OF_sb3 : REPAIR repair  DIST EXP (0.02)  INDUCING boolState[fail_in_operation_OF_sb3]  =  FALSE,fail_to_start_OF_sb3  =  FALSE,state_OF_sb3  =  'in_operation',nb_avail_repairmen_OF_rep3_4  =  (intState[nb_avail_repairmen_OF_rep3_4] + 1)" << endl;
		list.push_back(make_tuple(31, 0.02, "EXP", 0));
	}
	if ((boolState[fail_in_operation_OF_sb4] == false) && (enumState[state_OF_sb4] == in_operation))
	{
		cout << "34 : occ_fail_in_operation_OF_sb4 : FAULT fail_in_operation  LABEL \"sb4 failed in operation\"  DIST EXP (0.001)  INDUCING boolState[fail_in_operation_OF_sb4]  =  TRUE,state_OF_sb4  =  'waiting_for_repair'" << endl;
		list.push_back(make_tuple(34, 0.001, "EXP", 0));
	}
	if (((boolState[fail_in_operation_OF_sb4] == true) || (boolState[fail_to_start_OF_sb4] == true)) && (enumState[state_OF_sb4] == under_repair))
	{
		cout << "35 : occ_repair_OF_sb4 : REPAIR repair  DIST EXP (0.02)  INDUCING boolState[fail_in_operation_OF_sb4]  =  FALSE,fail_to_start_OF_sb4  =  FALSE,state_OF_sb4  =  'in_operation',nb_avail_repairmen_OF_rep3_4  =  (intState[nb_avail_repairmen_OF_rep3_4] + 1)" << endl;
		list.push_back(make_tuple(35, 0.02, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_figaro_RBD::runOnceInteractionStep_default_step()
{
	if (((enumState[state_OF_b1] == in_operation) || (enumState[state_OF_b1] ==  start_demand)) && ((boolState[fail_to_start_OF_b1] == false) && (  boolState[fail_in_operation_OF_b1] == false)) )
	{
		boolState[function_ok_OF_b1]  =  true;
		  boolState[function_known_OF_b1]  =  true;
	} else { boolState[function_ok_OF_b1]  =  false;  boolState[function_known_OF_b1]  =  true; }


	if ((enumState[state_OF_b1] == waiting_for_repair) && (intState[nb_avail_repairmen_OF_rep_1] > 0) )
	{
		enumState[state_OF_b1]  =  under_repair;
		  intState[nb_avail_repairmen_OF_rep_1]  =  (intState[nb_avail_repairmen_OF_rep_1]  - 1);
	}

	if (boolState[not_linked_OF_n_2] || ( !boolState[function_ok_OF_b1]) )
	{
		boolState[not_linked_OF_b1]  =  true;
	} else { boolState[not_linked_OF_b1]  =  false; }


	if (((enumState[state_OF_b2] == in_operation) || (enumState[state_OF_b2] ==  start_demand)) && ((boolState[fail_to_start_OF_b2] == false) && (  boolState[fail_in_operation_OF_b2] == false)) )
	{
		boolState[function_ok_OF_b2]  =  true;
		  boolState[function_known_OF_b2]  =  true;
	} else { boolState[function_ok_OF_b2]  =  false;  boolState[function_known_OF_b2]  =  true; }


	if ((enumState[state_OF_b2] == waiting_for_repair) && (intState[nb_avail_repairmen_OF_rep_2] > 0) )
	{
		enumState[state_OF_b2]  =  under_repair;
		  intState[nb_avail_repairmen_OF_rep_2]  =  (intState[nb_avail_repairmen_OF_rep_2]  - 1);
	}

	if (boolState[not_linked_OF_n_2] || ( !boolState[function_ok_OF_b2]) )
	{
		boolState[not_linked_OF_b2]  =  true;
	} else { boolState[not_linked_OF_b2]  =  false; }


	if (((enumState[state_OF_b3] == in_operation) || (enumState[state_OF_b3] ==  start_demand)) && ((boolState[fail_to_start_OF_b3] == false) && (  boolState[fail_in_operation_OF_b3] == false)) )
	{
		boolState[function_ok_OF_b3]  =  true;
		  boolState[function_known_OF_b3]  =  true;
	} else { boolState[function_ok_OF_b3]  =  false;  boolState[function_known_OF_b3]  =  true; }


	if ((enumState[state_OF_b3] == waiting_for_repair) && (intState[nb_avail_repairmen_OF_rep3_4] > 0) )
	{
		enumState[state_OF_b3]  =  under_repair;
		  intState[nb_avail_repairmen_OF_rep3_4]  =  (intState[nb_avail_repairmen_OF_rep3_4] - 1);
	}

	if (boolState[not_linked_OF_ds_1] || ( !boolState[function_ok_OF_b3]) )
	{
		boolState[not_linked_OF_b3]  =  true;
	} else { boolState[not_linked_OF_b3]  =  false; }


	if (((enumState[state_OF_b4] == in_operation) || (enumState[state_OF_b4] ==  start_demand)) && ((boolState[fail_to_start_OF_b4] == false) && (  boolState[fail_in_operation_OF_b4] == false)) )
	{
		boolState[function_ok_OF_b4]  =  true;
		  boolState[function_known_OF_b4]  =  true;
	} else { boolState[function_ok_OF_b4]  =  false;  boolState[function_known_OF_b4]  =  true; }


	if ((enumState[state_OF_b4] == waiting_for_repair) && (intState[nb_avail_repairmen_OF_rep3_4] > 0) )
	{
		enumState[state_OF_b4]  =  under_repair;
		  intState[nb_avail_repairmen_OF_rep3_4]  =  (intState[nb_avail_repairmen_OF_rep3_4] - 1);
	}

	if (boolState[not_linked_OF_n_1] || ( !boolState[function_ok_OF_b4]) )
	{
		boolState[not_linked_OF_b4]  =  true;
	} else { boolState[not_linked_OF_b4]  =  false; }


	if (((enumState[state_OF_b5] == in_operation) || (enumState[state_OF_b5] ==  start_demand)) && ((boolState[fail_to_start_OF_b5] == false) && (  boolState[fail_in_operation_OF_b5] == false)) )
	{
		boolState[function_ok_OF_b5]  =  true;
		  boolState[function_known_OF_b5]  =  true;
	} else { boolState[function_ok_OF_b5]  =  false;  boolState[function_known_OF_b5]  =  true; }


	if ((enumState[state_OF_b5] == waiting_for_repair) && (intState[nb_avail_repairmen_OF_rep_5] > 0) )
	{
		enumState[state_OF_b5]  =  under_repair;
		  intState[nb_avail_repairmen_OF_rep_5]  =  (intState[nb_avail_repairmen_OF_rep_5]  - 1);
	}

	if (boolState[not_linked_OF_ds_1] || ( !boolState[function_ok_OF_b5]) )
	{
		boolState[not_linked_OF_b5]  =  true;
	} else { boolState[not_linked_OF_b5]  =  false; }


	if (boolState[not_linked_OF_gate_2_4] )
	{
		boolState[not_linked_OF_de_1]  =  true;
	} else { boolState[not_linked_OF_de_1]  =  false; }




	boolState[not_linked_OF_ds_1]  =  false  ;

	if ((3 <= (boolState[not_linked_OF_b1] + boolState[not_linked_OF_b2] +  boolState[not_linked_OF_sb1] + boolState[not_linked_OF_sb2])) )
	{
		boolState[not_linked_OF_gate_2_4]  =  true;
	} else { boolState[not_linked_OF_gate_2_4]  =  false; }


	if (boolState[not_linked_OF_b3] && boolState[not_linked_OF_sb3] )
	{
		boolState[not_linked_OF_n_1]  =  true;
	} else { boolState[not_linked_OF_n_1]  =  false; }


	if ((boolState[not_linked_OF_b4] && boolState[not_linked_OF_b5]) && boolState[not_linked_OF_sb4] )
	{
		boolState[not_linked_OF_n_2]  =  true;
	} else { boolState[not_linked_OF_n_2]  =  false; }


	if (((enumState[state_OF_sb1] == in_operation) || (enumState[state_OF_sb1] ==  start_demand)) && ((boolState[fail_to_start_OF_sb1] == false) && (  boolState[fail_in_operation_OF_sb1] == false)) )
	{
		boolState[function_ok_OF_sb1]  =  true;
		  boolState[function_known_OF_sb1]  =  true;
	} else { boolState[function_ok_OF_sb1]  =  false;  boolState[function_known_OF_sb1]  =  true; }


	if ((enumState[state_OF_sb1] == waiting_for_repair) && (  intState[nb_avail_repairmen_OF_rep_1] > 0) )
	{
		enumState[state_OF_sb1]  =  under_repair;
		  intState[nb_avail_repairmen_OF_rep_1]  =  (intState[nb_avail_repairmen_OF_rep_1]  - 1);
	}

	if (boolState[not_linked_OF_n_2] || ( !boolState[function_ok_OF_sb1]) )
	{
		boolState[not_linked_OF_sb1]  =  true;
	} else { boolState[not_linked_OF_sb1]  =  false; }


	if ((enumState[state_OF_sb1] == standby) && (( !boolState[function_ok_OF_b1]) && boolState[function_known_OF_b1]) )
	{
		enumState[state_OF_sb1]  =  start_demand;
	}

	if (((enumState[state_OF_sb1] == in_operation) || (enumState[state_OF_sb1] ==  start_demand)) && (boolState[function_ok_OF_b1] && boolState[function_known_OF_b1]) )
	{
		enumState[state_OF_sb1]  =  standby;
	}

	if (((enumState[state_OF_sb2] == in_operation) || (enumState[state_OF_sb2] ==  start_demand)) && ((boolState[fail_to_start_OF_sb2] == false) && (  boolState[fail_in_operation_OF_sb2] == false)) )
	{
		boolState[function_ok_OF_sb2]  =  true;
		  boolState[function_known_OF_sb2]  =  true;
	} else { boolState[function_ok_OF_sb2]  =  false;  boolState[function_known_OF_sb2]  =  true; }


	if ((enumState[state_OF_sb2] == waiting_for_repair) && (  intState[nb_avail_repairmen_OF_rep_2] > 0) )
	{
		enumState[state_OF_sb2]  =  under_repair;
		  intState[nb_avail_repairmen_OF_rep_2]  =  (intState[nb_avail_repairmen_OF_rep_2]  - 1);
	}

	if (boolState[not_linked_OF_n_2] || ( !boolState[function_ok_OF_sb2]) )
	{
		boolState[not_linked_OF_sb2]  =  true;
	} else { boolState[not_linked_OF_sb2]  =  false; }


	if ((enumState[state_OF_sb2] == standby) && (( !boolState[function_ok_OF_b2]) && boolState[function_known_OF_b2]) )
	{
		enumState[state_OF_sb2]  =  start_demand;
	}

	if (((enumState[state_OF_sb2] == in_operation) || (enumState[state_OF_sb2] ==  start_demand)) && (boolState[function_ok_OF_b2] && boolState[function_known_OF_b2]) )
	{
		enumState[state_OF_sb2]  =  standby;
	}

	if (((enumState[state_OF_sb3] == in_operation) || (enumState[state_OF_sb3] ==  start_demand)) && ((boolState[fail_to_start_OF_sb3] == false) && (  boolState[fail_in_operation_OF_sb3] == false)) )
	{
		boolState[function_ok_OF_sb3]  =  true;
		  boolState[function_known_OF_sb3]  =  true;
	} else { boolState[function_ok_OF_sb3]  =  false;  boolState[function_known_OF_sb3]  =  true; }


	if ((enumState[state_OF_sb3] == waiting_for_repair) && (  intState[nb_avail_repairmen_OF_rep3_4] > 0) )
	{
		enumState[state_OF_sb3]  =  under_repair;
		  intState[nb_avail_repairmen_OF_rep3_4]  =  (intState[nb_avail_repairmen_OF_rep3_4] - 1);
	}

	if (boolState[not_linked_OF_ds_1] || ( !boolState[function_ok_OF_sb3]) )
	{
		boolState[not_linked_OF_sb3]  =  true;
	} else { boolState[not_linked_OF_sb3]  =  false; }


	if ((enumState[state_OF_sb3] == standby) && (( !boolState[function_ok_OF_b3]) && boolState[function_known_OF_b3]) )
	{
		enumState[state_OF_sb3]  =  start_demand;
	}

	if (((enumState[state_OF_sb3] == in_operation) || (enumState[state_OF_sb3] ==  start_demand)) && (boolState[function_ok_OF_b3] && boolState[function_known_OF_b3]) )
	{
		enumState[state_OF_sb3]  =  standby;
	}

	if (((enumState[state_OF_sb4] == in_operation) || (enumState[state_OF_sb4] ==  start_demand)) && ((boolState[fail_to_start_OF_sb4] == false) && (  boolState[fail_in_operation_OF_sb4] == false)) )
	{
		boolState[function_ok_OF_sb4]  =  true;
		  boolState[function_known_OF_sb4]  =  true;
	} else { boolState[function_ok_OF_sb4]  =  false;  boolState[function_known_OF_sb4]  =  true; }


	if ((enumState[state_OF_sb4] == waiting_for_repair) && (  intState[nb_avail_repairmen_OF_rep3_4] > 0) )
	{
		enumState[state_OF_sb4]  =  under_repair;
		  intState[nb_avail_repairmen_OF_rep3_4]  =  (intState[nb_avail_repairmen_OF_rep3_4] - 1);
	}

	if (boolState[not_linked_OF_n_1] || ( !boolState[function_ok_OF_sb4]) )
	{
		boolState[not_linked_OF_sb4]  =  true;
	} else { boolState[not_linked_OF_sb4]  =  false; }


	if ((enumState[state_OF_sb4] == standby) && (( !boolState[function_ok_OF_b4]) && boolState[function_known_OF_b4]) )
	{
		enumState[state_OF_sb4]  =  start_demand;
	}

	if (((enumState[state_OF_sb4] == in_operation) || (enumState[state_OF_sb4] ==  start_demand)) && (boolState[function_ok_OF_b4] && boolState[function_known_OF_b4]) )
	{
		enumState[state_OF_sb4]  =  standby;
	}

}

void
storm::figaro::FigaroProgram_figaro_RBD::runInteractions() {
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
         
        // ------------------- Handling of FailureState element --------------------------------
    
	boolFailureState[exp0] = ( boolState[not_linked_OF_de_1] );
        cout << endl;
    }void storm::figaro::FigaroProgram_figaro_RBD::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_figaro_RBD::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_figaro_RBD::fireinsttransitiongroup(std::string user_input_ins)

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
    