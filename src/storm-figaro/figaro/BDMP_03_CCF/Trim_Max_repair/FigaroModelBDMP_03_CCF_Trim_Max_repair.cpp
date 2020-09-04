#include <iostream>
#include "FigaroModelBDMP_03_CCF_Trim_Max_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_CCF_fail_A = true;
	boolState[already_S_OF_CCF_fail_A] = false;
	REINITIALISATION_OF_S_OF_CCF_fail_A = false;
	REINITIALISATION_OF_relevant_evt_OF_CCF_fail_A = false;
	boolState[failI_OF_CCF_fail_A] = false;
	REINITIALISATION_OF_to_be_fired_OF_CCF_fail_A = false;
	boolState[already_standby_OF_CCF_fail_A] = false;
	boolState[already_required_OF_CCF_fail_A] = false;
	REINITIALISATION_OF_required_OF_CCF_fail_B = true;
	boolState[already_S_OF_CCF_fail_B] = false;
	REINITIALISATION_OF_S_OF_CCF_fail_B = false;
	REINITIALISATION_OF_relevant_evt_OF_CCF_fail_B = false;
	boolState[failI_OF_CCF_fail_B] = false;
	REINITIALISATION_OF_to_be_fired_OF_CCF_fail_B = false;
	boolState[already_standby_OF_CCF_fail_B] = false;
	boolState[already_required_OF_CCF_fail_B] = false;
	REINITIALISATION_OF_required_OF_CCF_fail_C = true;
	boolState[already_S_OF_CCF_fail_C] = false;
	REINITIALISATION_OF_S_OF_CCF_fail_C = false;
	REINITIALISATION_OF_relevant_evt_OF_CCF_fail_C = false;
	boolState[failI_OF_CCF_fail_C] = false;
	REINITIALISATION_OF_to_be_fired_OF_CCF_fail_C = false;
	boolState[already_standby_OF_CCF_fail_C] = false;
	boolState[already_required_OF_CCF_fail_C] = false;
	REINITIALISATION_OF_required_OF_Indep_fail_A = true;
	boolState[already_S_OF_Indep_fail_A] = false;
	REINITIALISATION_OF_S_OF_Indep_fail_A = false;
	REINITIALISATION_OF_relevant_evt_OF_Indep_fail_A = false;
	boolState[failF_OF_Indep_fail_A] = false;
	REINITIALISATION_OF_required_OF_Indep_fail_B = true;
	boolState[already_S_OF_Indep_fail_B] = false;
	REINITIALISATION_OF_S_OF_Indep_fail_B = false;
	REINITIALISATION_OF_relevant_evt_OF_Indep_fail_B = false;
	boolState[failF_OF_Indep_fail_B] = false;
	REINITIALISATION_OF_required_OF_Indep_fail_C = true;
	boolState[already_S_OF_Indep_fail_C] = false;
	REINITIALISATION_OF_S_OF_Indep_fail_C = false;
	REINITIALISATION_OF_relevant_evt_OF_Indep_fail_C = false;
	boolState[failF_OF_Indep_fail_C] = false;
	REINITIALISATION_OF_required_OF_Shock = true;
	boolState[already_S_OF_Shock] = false;
	REINITIALISATION_OF_S_OF_Shock = false;
	REINITIALISATION_OF_relevant_evt_OF_Shock = false;
	boolState[failF_OF_Shock] = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;
	REINITIALISATION_OF_required_OF_loss_of_A = true;
	boolState[already_S_OF_loss_of_A] = false;
	REINITIALISATION_OF_S_OF_loss_of_A = false;
	REINITIALISATION_OF_relevant_evt_OF_loss_of_A = false;
	REINITIALISATION_OF_required_OF_loss_of_B = true;
	boolState[already_S_OF_loss_of_B] = false;
	REINITIALISATION_OF_S_OF_loss_of_B = false;
	REINITIALISATION_OF_relevant_evt_OF_loss_of_B = false;
	REINITIALISATION_OF_required_OF_loss_of_C = true;
	boolState[already_S_OF_loss_of_C] = false;
	REINITIALISATION_OF_S_OF_loss_of_C = false;
	REINITIALISATION_OF_relevant_evt_OF_loss_of_C = false;
	REINITIALISATION_OF_required_OF_system_loss = true;
	boolState[already_S_OF_system_loss] = false;
	REINITIALISATION_OF_S_OF_system_loss = false;
	REINITIALISATION_OF_relevant_evt_OF_system_loss = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx23_OF_CCF_fail_A_INS_0 = false;
	FIRE_xx23_OF_CCF_fail_A_INS_1 = false;
	FIRE_xx24_OF_CCF_fail_A = false;
	FIRE_xx23_OF_CCF_fail_B_INS_3 = false;
	FIRE_xx23_OF_CCF_fail_B_INS_4 = false;
	FIRE_xx24_OF_CCF_fail_B = false;
	FIRE_xx23_OF_CCF_fail_C_INS_6 = false;
	FIRE_xx23_OF_CCF_fail_C_INS_7 = false;
	FIRE_xx24_OF_CCF_fail_C = false;
	FIRE_xx10_OF_Indep_fail_A = false;
	FIRE_xx11_OF_Indep_fail_A = false;
	FIRE_xx10_OF_Indep_fail_B = false;
	FIRE_xx11_OF_Indep_fail_B = false;
	FIRE_xx10_OF_Indep_fail_C = false;
	FIRE_xx11_OF_Indep_fail_C = false;
	FIRE_xx10_OF_Shock = false;
	FIRE_xx11_OF_Shock = false;

}

void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_CCF_fail_A] | Value : " << boolState[required_OF_CCF_fail_A] << endl;
	cout << "Attribute :  boolState[already_S_OF_CCF_fail_A] | Value : " << boolState[already_S_OF_CCF_fail_A] << endl;
	cout << "Attribute :  boolState[S_OF_CCF_fail_A] | Value : " << boolState[S_OF_CCF_fail_A] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CCF_fail_A] | Value : " << boolState[relevant_evt_OF_CCF_fail_A] << endl;
	cout << "Attribute :  boolState[failI_OF_CCF_fail_A] | Value : " << boolState[failI_OF_CCF_fail_A] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_CCF_fail_A] | Value : " << boolState[to_be_fired_OF_CCF_fail_A] << endl;
	cout << "Attribute :  boolState[already_standby_OF_CCF_fail_A] | Value : " << boolState[already_standby_OF_CCF_fail_A] << endl;
	cout << "Attribute :  boolState[already_required_OF_CCF_fail_A] | Value : " << boolState[already_required_OF_CCF_fail_A] << endl;
	cout << "Attribute :  boolState[required_OF_CCF_fail_B] | Value : " << boolState[required_OF_CCF_fail_B] << endl;
	cout << "Attribute :  boolState[already_S_OF_CCF_fail_B] | Value : " << boolState[already_S_OF_CCF_fail_B] << endl;
	cout << "Attribute :  boolState[S_OF_CCF_fail_B] | Value : " << boolState[S_OF_CCF_fail_B] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CCF_fail_B] | Value : " << boolState[relevant_evt_OF_CCF_fail_B] << endl;
	cout << "Attribute :  boolState[failI_OF_CCF_fail_B] | Value : " << boolState[failI_OF_CCF_fail_B] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_CCF_fail_B] | Value : " << boolState[to_be_fired_OF_CCF_fail_B] << endl;
	cout << "Attribute :  boolState[already_standby_OF_CCF_fail_B] | Value : " << boolState[already_standby_OF_CCF_fail_B] << endl;
	cout << "Attribute :  boolState[already_required_OF_CCF_fail_B] | Value : " << boolState[already_required_OF_CCF_fail_B] << endl;
	cout << "Attribute :  boolState[required_OF_CCF_fail_C] | Value : " << boolState[required_OF_CCF_fail_C] << endl;
	cout << "Attribute :  boolState[already_S_OF_CCF_fail_C] | Value : " << boolState[already_S_OF_CCF_fail_C] << endl;
	cout << "Attribute :  boolState[S_OF_CCF_fail_C] | Value : " << boolState[S_OF_CCF_fail_C] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CCF_fail_C] | Value : " << boolState[relevant_evt_OF_CCF_fail_C] << endl;
	cout << "Attribute :  boolState[failI_OF_CCF_fail_C] | Value : " << boolState[failI_OF_CCF_fail_C] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_CCF_fail_C] | Value : " << boolState[to_be_fired_OF_CCF_fail_C] << endl;
	cout << "Attribute :  boolState[already_standby_OF_CCF_fail_C] | Value : " << boolState[already_standby_OF_CCF_fail_C] << endl;
	cout << "Attribute :  boolState[already_required_OF_CCF_fail_C] | Value : " << boolState[already_required_OF_CCF_fail_C] << endl;
	cout << "Attribute :  boolState[required_OF_Indep_fail_A] | Value : " << boolState[required_OF_Indep_fail_A] << endl;
	cout << "Attribute :  boolState[already_S_OF_Indep_fail_A] | Value : " << boolState[already_S_OF_Indep_fail_A] << endl;
	cout << "Attribute :  boolState[S_OF_Indep_fail_A] | Value : " << boolState[S_OF_Indep_fail_A] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Indep_fail_A] | Value : " << boolState[relevant_evt_OF_Indep_fail_A] << endl;
	cout << "Attribute :  boolState[failF_OF_Indep_fail_A] | Value : " << boolState[failF_OF_Indep_fail_A] << endl;
	cout << "Attribute :  boolState[required_OF_Indep_fail_B] | Value : " << boolState[required_OF_Indep_fail_B] << endl;
	cout << "Attribute :  boolState[already_S_OF_Indep_fail_B] | Value : " << boolState[already_S_OF_Indep_fail_B] << endl;
	cout << "Attribute :  boolState[S_OF_Indep_fail_B] | Value : " << boolState[S_OF_Indep_fail_B] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Indep_fail_B] | Value : " << boolState[relevant_evt_OF_Indep_fail_B] << endl;
	cout << "Attribute :  boolState[failF_OF_Indep_fail_B] | Value : " << boolState[failF_OF_Indep_fail_B] << endl;
	cout << "Attribute :  boolState[required_OF_Indep_fail_C] | Value : " << boolState[required_OF_Indep_fail_C] << endl;
	cout << "Attribute :  boolState[already_S_OF_Indep_fail_C] | Value : " << boolState[already_S_OF_Indep_fail_C] << endl;
	cout << "Attribute :  boolState[S_OF_Indep_fail_C] | Value : " << boolState[S_OF_Indep_fail_C] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Indep_fail_C] | Value : " << boolState[relevant_evt_OF_Indep_fail_C] << endl;
	cout << "Attribute :  boolState[failF_OF_Indep_fail_C] | Value : " << boolState[failF_OF_Indep_fail_C] << endl;
	cout << "Attribute :  boolState[required_OF_Shock] | Value : " << boolState[required_OF_Shock] << endl;
	cout << "Attribute :  boolState[already_S_OF_Shock] | Value : " << boolState[already_S_OF_Shock] << endl;
	cout << "Attribute :  boolState[S_OF_Shock] | Value : " << boolState[S_OF_Shock] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Shock] | Value : " << boolState[relevant_evt_OF_Shock] << endl;
	cout << "Attribute :  boolState[failF_OF_Shock] | Value : " << boolState[failF_OF_Shock] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
	cout << "Attribute :  boolState[required_OF_loss_of_A] | Value : " << boolState[required_OF_loss_of_A] << endl;
	cout << "Attribute :  boolState[already_S_OF_loss_of_A] | Value : " << boolState[already_S_OF_loss_of_A] << endl;
	cout << "Attribute :  boolState[S_OF_loss_of_A] | Value : " << boolState[S_OF_loss_of_A] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_loss_of_A] | Value : " << boolState[relevant_evt_OF_loss_of_A] << endl;
	cout << "Attribute :  boolState[required_OF_loss_of_B] | Value : " << boolState[required_OF_loss_of_B] << endl;
	cout << "Attribute :  boolState[already_S_OF_loss_of_B] | Value : " << boolState[already_S_OF_loss_of_B] << endl;
	cout << "Attribute :  boolState[S_OF_loss_of_B] | Value : " << boolState[S_OF_loss_of_B] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_loss_of_B] | Value : " << boolState[relevant_evt_OF_loss_of_B] << endl;
	cout << "Attribute :  boolState[required_OF_loss_of_C] | Value : " << boolState[required_OF_loss_of_C] << endl;
	cout << "Attribute :  boolState[already_S_OF_loss_of_C] | Value : " << boolState[already_S_OF_loss_of_C] << endl;
	cout << "Attribute :  boolState[S_OF_loss_of_C] | Value : " << boolState[S_OF_loss_of_C] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_loss_of_C] | Value : " << boolState[relevant_evt_OF_loss_of_C] << endl;
	cout << "Attribute :  boolState[required_OF_system_loss] | Value : " << boolState[required_OF_system_loss] << endl;
	cout << "Attribute :  boolState[already_S_OF_system_loss] | Value : " << boolState[already_S_OF_system_loss] << endl;
	cout << "Attribute :  boolState[S_OF_system_loss] | Value : " << boolState[S_OF_system_loss] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_system_loss] | Value : " << boolState[relevant_evt_OF_system_loss] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::doReinitialisations()
{
	boolState[required_OF_CCF_fail_A] = REINITIALISATION_OF_required_OF_CCF_fail_A;
	boolState[S_OF_CCF_fail_A] = REINITIALISATION_OF_S_OF_CCF_fail_A;
	boolState[relevant_evt_OF_CCF_fail_A] = REINITIALISATION_OF_relevant_evt_OF_CCF_fail_A;
	boolState[to_be_fired_OF_CCF_fail_A] = REINITIALISATION_OF_to_be_fired_OF_CCF_fail_A;
	boolState[required_OF_CCF_fail_B] = REINITIALISATION_OF_required_OF_CCF_fail_B;
	boolState[S_OF_CCF_fail_B] = REINITIALISATION_OF_S_OF_CCF_fail_B;
	boolState[relevant_evt_OF_CCF_fail_B] = REINITIALISATION_OF_relevant_evt_OF_CCF_fail_B;
	boolState[to_be_fired_OF_CCF_fail_B] = REINITIALISATION_OF_to_be_fired_OF_CCF_fail_B;
	boolState[required_OF_CCF_fail_C] = REINITIALISATION_OF_required_OF_CCF_fail_C;
	boolState[S_OF_CCF_fail_C] = REINITIALISATION_OF_S_OF_CCF_fail_C;
	boolState[relevant_evt_OF_CCF_fail_C] = REINITIALISATION_OF_relevant_evt_OF_CCF_fail_C;
	boolState[to_be_fired_OF_CCF_fail_C] = REINITIALISATION_OF_to_be_fired_OF_CCF_fail_C;
	boolState[required_OF_Indep_fail_A] = REINITIALISATION_OF_required_OF_Indep_fail_A;
	boolState[S_OF_Indep_fail_A] = REINITIALISATION_OF_S_OF_Indep_fail_A;
	boolState[relevant_evt_OF_Indep_fail_A] = REINITIALISATION_OF_relevant_evt_OF_Indep_fail_A;
	boolState[required_OF_Indep_fail_B] = REINITIALISATION_OF_required_OF_Indep_fail_B;
	boolState[S_OF_Indep_fail_B] = REINITIALISATION_OF_S_OF_Indep_fail_B;
	boolState[relevant_evt_OF_Indep_fail_B] = REINITIALISATION_OF_relevant_evt_OF_Indep_fail_B;
	boolState[required_OF_Indep_fail_C] = REINITIALISATION_OF_required_OF_Indep_fail_C;
	boolState[S_OF_Indep_fail_C] = REINITIALISATION_OF_S_OF_Indep_fail_C;
	boolState[relevant_evt_OF_Indep_fail_C] = REINITIALISATION_OF_relevant_evt_OF_Indep_fail_C;
	boolState[required_OF_Shock] = REINITIALISATION_OF_required_OF_Shock;
	boolState[S_OF_Shock] = REINITIALISATION_OF_S_OF_Shock;
	boolState[relevant_evt_OF_Shock] = REINITIALISATION_OF_relevant_evt_OF_Shock;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
	boolState[required_OF_loss_of_A] = REINITIALISATION_OF_required_OF_loss_of_A;
	boolState[S_OF_loss_of_A] = REINITIALISATION_OF_S_OF_loss_of_A;
	boolState[relevant_evt_OF_loss_of_A] = REINITIALISATION_OF_relevant_evt_OF_loss_of_A;
	boolState[required_OF_loss_of_B] = REINITIALISATION_OF_required_OF_loss_of_B;
	boolState[S_OF_loss_of_B] = REINITIALISATION_OF_S_OF_loss_of_B;
	boolState[relevant_evt_OF_loss_of_B] = REINITIALISATION_OF_relevant_evt_OF_loss_of_B;
	boolState[required_OF_loss_of_C] = REINITIALISATION_OF_required_OF_loss_of_C;
	boolState[S_OF_loss_of_C] = REINITIALISATION_OF_S_OF_loss_of_C;
	boolState[relevant_evt_OF_loss_of_C] = REINITIALISATION_OF_relevant_evt_OF_loss_of_C;
	boolState[required_OF_system_loss] = REINITIALISATION_OF_required_OF_system_loss;
	boolState[S_OF_system_loss] = REINITIALISATION_OF_S_OF_system_loss;
	boolState[relevant_evt_OF_system_loss] = REINITIALISATION_OF_relevant_evt_OF_system_loss;
}

void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx23_OF_CCF_fail_A_INS_0 = true;
	}

	if (numFire == 1)
	{
		FIRE_xx23_OF_CCF_fail_A_INS_1 = true;
	}

	if (numFire == 2)
	{
		FIRE_xx24_OF_CCF_fail_A = true;
	}

	if (numFire == 3)
	{
		FIRE_xx23_OF_CCF_fail_B_INS_3 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx23_OF_CCF_fail_B_INS_4 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx24_OF_CCF_fail_B = true;
	}

	if (numFire == 6)
	{
		FIRE_xx23_OF_CCF_fail_C_INS_6 = true;
	}

	if (numFire == 7)
	{
		FIRE_xx23_OF_CCF_fail_C_INS_7 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx24_OF_CCF_fail_C = true;
	}

	if (numFire == 9)
	{
		FIRE_xx10_OF_Indep_fail_A = true;
	}

	if (numFire == 10)
	{
		FIRE_xx11_OF_Indep_fail_A = true;
	}

	if (numFire == 11)
	{
		FIRE_xx10_OF_Indep_fail_B = true;
	}

	if (numFire == 12)
	{
		FIRE_xx11_OF_Indep_fail_B = true;
	}

	if (numFire == 13)
	{
		FIRE_xx10_OF_Indep_fail_C = true;
	}

	if (numFire == 14)
	{
		FIRE_xx11_OF_Indep_fail_C = true;
	}

	if (numFire == 15)
	{
		FIRE_xx10_OF_Shock = true;
	}

	if (numFire == 16)
	{
		FIRE_xx11_OF_Shock = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx23_OF_CCF_fail_A

	if ((boolState[failI_OF_CCF_fail_A] == false) && (boolState[to_be_fired_OF_CCF_fail_A]
	&& boolState[relevant_evt_OF_CCF_fail_A])) 
	{
	
		
		if (FIRE_xx23_OF_CCF_fail_A_INS_0) 
		{
			boolState[failI_OF_CCF_fail_A]  =  true;
			boolState[already_standby_OF_CCF_fail_A]  =  false;
			boolState[already_required_OF_CCF_fail_A]  =  false;
			FIRE_xx23_OF_CCF_fail_A_INS_0 = false;
		}
	
	}
	if ((boolState[failI_OF_CCF_fail_A] == false) && (boolState[to_be_fired_OF_CCF_fail_A]
	&& boolState[relevant_evt_OF_CCF_fail_A])) 
	{
	
		
		if (FIRE_xx23_OF_CCF_fail_A_INS_1) 
		{
			boolState[already_standby_OF_CCF_fail_A]  =  false;
			boolState[already_required_OF_CCF_fail_A]  =  false;
			FIRE_xx23_OF_CCF_fail_A_INS_1 = false;
		}
	
	}
	// Occurrence xx24_OF_CCF_fail_A
	if (boolState[failI_OF_CCF_fail_A] == true) 
	{
		 
		if (FIRE_xx24_OF_CCF_fail_A)
		{
			boolState[failI_OF_CCF_fail_A]  =  false;
			FIRE_xx24_OF_CCF_fail_A = false;
		}
	}

	// Occurrence xx23_OF_CCF_fail_B

	if ((boolState[failI_OF_CCF_fail_B] == false) && (boolState[to_be_fired_OF_CCF_fail_B]
	&& boolState[relevant_evt_OF_CCF_fail_B])) 
	{
	
		
		if (FIRE_xx23_OF_CCF_fail_B_INS_3) 
		{
			boolState[failI_OF_CCF_fail_B]  =  true;
			boolState[already_standby_OF_CCF_fail_B]  =  false;
			boolState[already_required_OF_CCF_fail_B]  =  false;
			FIRE_xx23_OF_CCF_fail_B_INS_3 = false;
		}
	
	}
	if ((boolState[failI_OF_CCF_fail_B] == false) && (boolState[to_be_fired_OF_CCF_fail_B]
	&& boolState[relevant_evt_OF_CCF_fail_B])) 
	{
	
		
		if (FIRE_xx23_OF_CCF_fail_B_INS_4) 
		{
			boolState[already_standby_OF_CCF_fail_B]  =  false;
			boolState[already_required_OF_CCF_fail_B]  =  false;
			FIRE_xx23_OF_CCF_fail_B_INS_4 = false;
		}
	
	}
	// Occurrence xx24_OF_CCF_fail_B
	if (boolState[failI_OF_CCF_fail_B] == true) 
	{
		 
		if (FIRE_xx24_OF_CCF_fail_B)
		{
			boolState[failI_OF_CCF_fail_B]  =  false;
			FIRE_xx24_OF_CCF_fail_B = false;
		}
	}

	// Occurrence xx23_OF_CCF_fail_C

	if ((boolState[failI_OF_CCF_fail_C] == false) && (boolState[to_be_fired_OF_CCF_fail_C]
	&& boolState[relevant_evt_OF_CCF_fail_C])) 
	{
	
		
		if (FIRE_xx23_OF_CCF_fail_C_INS_6) 
		{
			boolState[failI_OF_CCF_fail_C]  =  true;
			boolState[already_standby_OF_CCF_fail_C]  =  false;
			boolState[already_required_OF_CCF_fail_C]  =  false;
			FIRE_xx23_OF_CCF_fail_C_INS_6 = false;
		}
	
	}
	if ((boolState[failI_OF_CCF_fail_C] == false) && (boolState[to_be_fired_OF_CCF_fail_C]
	&& boolState[relevant_evt_OF_CCF_fail_C])) 
	{
	
		
		if (FIRE_xx23_OF_CCF_fail_C_INS_7) 
		{
			boolState[already_standby_OF_CCF_fail_C]  =  false;
			boolState[already_required_OF_CCF_fail_C]  =  false;
			FIRE_xx23_OF_CCF_fail_C_INS_7 = false;
		}
	
	}
	// Occurrence xx24_OF_CCF_fail_C
	if (boolState[failI_OF_CCF_fail_C] == true) 
	{
		 
		if (FIRE_xx24_OF_CCF_fail_C)
		{
			boolState[failI_OF_CCF_fail_C]  =  false;
			FIRE_xx24_OF_CCF_fail_C = false;
		}
	}

	// Occurrence xx10_OF_Indep_fail_A
	if ((boolState[failF_OF_Indep_fail_A] == false) && (boolState[required_OF_Indep_fail_A] && boolState[relevant_evt_OF_Indep_fail_A])) 
	{
		 
		if (FIRE_xx10_OF_Indep_fail_A)
		{
			boolState[failF_OF_Indep_fail_A]  =  true;
			FIRE_xx10_OF_Indep_fail_A = false;
		}
	}

	// Occurrence xx11_OF_Indep_fail_A
	if (boolState[failF_OF_Indep_fail_A] == true) 
	{
		 
		if (FIRE_xx11_OF_Indep_fail_A)
		{
			boolState[failF_OF_Indep_fail_A]  =  false;
			FIRE_xx11_OF_Indep_fail_A = false;
		}
	}

	// Occurrence xx10_OF_Indep_fail_B
	if ((boolState[failF_OF_Indep_fail_B] == false) && (boolState[required_OF_Indep_fail_B] && boolState[relevant_evt_OF_Indep_fail_B])) 
	{
		 
		if (FIRE_xx10_OF_Indep_fail_B)
		{
			boolState[failF_OF_Indep_fail_B]  =  true;
			FIRE_xx10_OF_Indep_fail_B = false;
		}
	}

	// Occurrence xx11_OF_Indep_fail_B
	if (boolState[failF_OF_Indep_fail_B] == true) 
	{
		 
		if (FIRE_xx11_OF_Indep_fail_B)
		{
			boolState[failF_OF_Indep_fail_B]  =  false;
			FIRE_xx11_OF_Indep_fail_B = false;
		}
	}

	// Occurrence xx10_OF_Indep_fail_C
	if ((boolState[failF_OF_Indep_fail_C] == false) && (boolState[required_OF_Indep_fail_C] && boolState[relevant_evt_OF_Indep_fail_C])) 
	{
		 
		if (FIRE_xx10_OF_Indep_fail_C)
		{
			boolState[failF_OF_Indep_fail_C]  =  true;
			FIRE_xx10_OF_Indep_fail_C = false;
		}
	}

	// Occurrence xx11_OF_Indep_fail_C
	if (boolState[failF_OF_Indep_fail_C] == true) 
	{
		 
		if (FIRE_xx11_OF_Indep_fail_C)
		{
			boolState[failF_OF_Indep_fail_C]  =  false;
			FIRE_xx11_OF_Indep_fail_C = false;
		}
	}

	// Occurrence xx10_OF_Shock
	if ((boolState[failF_OF_Shock] == false) && (boolState[required_OF_Shock] &&  boolState[relevant_evt_OF_Shock])) 
	{
		 
		if (FIRE_xx10_OF_Shock)
		{
			boolState[failF_OF_Shock]  =  true;
			FIRE_xx10_OF_Shock = false;
		}
	}

	// Occurrence xx11_OF_Shock
	if (boolState[failF_OF_Shock] == true) 
	{
		 
		if (FIRE_xx11_OF_Shock)
		{
			boolState[failF_OF_Shock]  =  false;
			FIRE_xx11_OF_Shock = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if ((boolState[failI_OF_CCF_fail_A] == false) && (boolState[to_be_fired_OF_CCF_fail_A] && boolState[relevant_evt_OF_CCF_fail_A]))
	{
		cout << "0 :  INS_SUB_COUNT (1) |FAULT | failI  LABEL \"instantaneous failure CCF_fail_A\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_CCF_fail_A]  =  TRUE,already_standby_OF_CCF_fail_A  =  FALSE,already_required_OF_CCF_fail_A  =  FALSE" << endl;
		list.push_back(make_tuple(0, 0.0001, "INS", 1));
	}
	if ((boolState[failI_OF_CCF_fail_A] == false) && (boolState[to_be_fired_OF_CCF_fail_A] && boolState[relevant_evt_OF_CCF_fail_A]))
	{
		cout << "1 :  INS_SUB_COUNT (1) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_CCF_fail_A]  =  FALSE,already_required_OF_CCF_fail_A  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.9999, "INS", 1));
	}
	if ((boolState[failI_OF_CCF_fail_B] == false) && (boolState[to_be_fired_OF_CCF_fail_B] && boolState[relevant_evt_OF_CCF_fail_B]))
	{
		cout << "3 :  INS_SUB_COUNT (2) |FAULT | failI  LABEL \"instantaneous failure CCF_fail_B\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_CCF_fail_B]  =  TRUE,already_standby_OF_CCF_fail_B  =  FALSE,already_required_OF_CCF_fail_B  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.0001, "INS", 2));
	}
	if ((boolState[failI_OF_CCF_fail_B] == false) && (boolState[to_be_fired_OF_CCF_fail_B] && boolState[relevant_evt_OF_CCF_fail_B]))
	{
		cout << "4 :  INS_SUB_COUNT (2) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_CCF_fail_B]  =  FALSE,already_required_OF_CCF_fail_B  =  FALSE" << endl;
		list.push_back(make_tuple(4, 0.9999, "INS", 2));
	}
	if ((boolState[failI_OF_CCF_fail_C] == false) && (boolState[to_be_fired_OF_CCF_fail_C] && boolState[relevant_evt_OF_CCF_fail_C]))
	{
		cout << "6 :  INS_SUB_COUNT (3) |FAULT | failI  LABEL \"instantaneous failure CCF_fail_C\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_CCF_fail_C]  =  TRUE,already_standby_OF_CCF_fail_C  =  FALSE,already_required_OF_CCF_fail_C  =  FALSE" << endl;
		list.push_back(make_tuple(6, 0.0001, "INS", 3));
	}
	if ((boolState[failI_OF_CCF_fail_C] == false) && (boolState[to_be_fired_OF_CCF_fail_C] && boolState[relevant_evt_OF_CCF_fail_C]))
	{
		cout << "7 :  INS_SUB_COUNT (3) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_CCF_fail_C]  =  FALSE,already_required_OF_CCF_fail_C  =  FALSE" << endl;
		list.push_back(make_tuple(7, 0.9999, "INS", 3));
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
     
	if (boolState[failI_OF_CCF_fail_A] == true)
	{
		cout << "2 : xx24_OF_CCF_fail_A : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failI_OF_CCF_fail_A]  =  FALSE" << endl;
		list.push_back(make_tuple(2, 0.1, "EXP", 0));
	}
	if (boolState[failI_OF_CCF_fail_B] == true)
	{
		cout << "5 : xx24_OF_CCF_fail_B : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failI_OF_CCF_fail_B]  =  FALSE" << endl;
		list.push_back(make_tuple(5, 0.1, "EXP", 0));
	}
	if (boolState[failI_OF_CCF_fail_C] == true)
	{
		cout << "8 : xx24_OF_CCF_fail_C : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failI_OF_CCF_fail_C]  =  FALSE" << endl;
		list.push_back(make_tuple(8, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_Indep_fail_A] == false) && (boolState[required_OF_Indep_fail_A] && boolState[relevant_evt_OF_Indep_fail_A]))
	{
		cout << "9 : xx10_OF_Indep_fail_A : FAULT failF  LABEL \"failure in operation Indep_fail_A\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Indep_fail_A]  =  TRUE" << endl;
		list.push_back(make_tuple(9, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_Indep_fail_A] == true)
	{
		cout << "10 : xx11_OF_Indep_fail_A : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_Indep_fail_A]  =  FALSE" << endl;
		list.push_back(make_tuple(10, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_Indep_fail_B] == false) && (boolState[required_OF_Indep_fail_B] && boolState[relevant_evt_OF_Indep_fail_B]))
	{
		cout << "11 : xx10_OF_Indep_fail_B : FAULT failF  LABEL \"failure in operation Indep_fail_B\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Indep_fail_B]  =  TRUE" << endl;
		list.push_back(make_tuple(11, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_Indep_fail_B] == true)
	{
		cout << "12 : xx11_OF_Indep_fail_B : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_Indep_fail_B]  =  FALSE" << endl;
		list.push_back(make_tuple(12, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_Indep_fail_C] == false) && (boolState[required_OF_Indep_fail_C] && boolState[relevant_evt_OF_Indep_fail_C]))
	{
		cout << "13 : xx10_OF_Indep_fail_C : FAULT failF  LABEL \"failure in operation Indep_fail_C\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Indep_fail_C]  =  TRUE" << endl;
		list.push_back(make_tuple(13, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_Indep_fail_C] == true)
	{
		cout << "14 : xx11_OF_Indep_fail_C : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_Indep_fail_C]  =  FALSE" << endl;
		list.push_back(make_tuple(14, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_Shock] == false) && (boolState[required_OF_Shock] && boolState[relevant_evt_OF_Shock]))
	{
		cout << "15 : xx10_OF_Shock : FAULT failF  LABEL \"failure in operation Shock\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Shock]  =  TRUE" << endl;
		list.push_back(make_tuple(15, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_Shock] == true)
	{
		cout << "16 : xx11_OF_Shock : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_Shock]  =  FALSE" << endl;
		list.push_back(make_tuple(16, 0.1, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failI_OF_CCF_fail_A] == true )
	{
		boolState[S_OF_CCF_fail_A]  =  true;
	}

	if (boolState[failI_OF_CCF_fail_B] == true )
	{
		boolState[S_OF_CCF_fail_B]  =  true;
	}

	if (boolState[failI_OF_CCF_fail_C] == true )
	{
		boolState[S_OF_CCF_fail_C]  =  true;
	}

	if (boolState[failF_OF_Indep_fail_A] == true )
	{
		boolState[S_OF_Indep_fail_A]  =  true;
	}

	if (boolState[failF_OF_Indep_fail_B] == true )
	{
		boolState[S_OF_Indep_fail_B]  =  true;
	}

	if (boolState[failF_OF_Indep_fail_C] == true )
	{
		boolState[S_OF_Indep_fail_C]  =  true;
	}

	if (boolState[failF_OF_Shock] == true )
	{
		boolState[S_OF_Shock]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_system_loss] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

	if (boolState[S_OF_Indep_fail_A] || boolState[S_OF_CCF_fail_A] )
	{
		boolState[S_OF_loss_of_A]  =  true;
	}

	if (boolState[S_OF_Indep_fail_B] || boolState[S_OF_CCF_fail_B] )
	{
		boolState[S_OF_loss_of_B]  =  true;
	}

	if (boolState[S_OF_Indep_fail_C] || boolState[S_OF_CCF_fail_C] )
	{
		boolState[S_OF_loss_of_C]  =  true;
	}

	if ((2 <= (boolState[S_OF_loss_of_A] + boolState[S_OF_loss_of_B] + boolState[S_OF_loss_of_C])) )
	{
		boolState[S_OF_system_loss]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::runOnceInteractionStep_propagate_effect_required()
{
	if (( !boolState[required_OF_loss_of_A]) || ( !boolState[S_OF_Shock]) )
	{
		boolState[required_OF_CCF_fail_A]  =  false;
	}

	if (boolState[relevant_evt_OF_loss_of_A] && ( !boolState[S_OF_loss_of_A]) )
	{
		boolState[relevant_evt_OF_CCF_fail_A]  =  true;
	}

	if (( !boolState[required_OF_loss_of_B]) || ( !boolState[S_OF_Shock]) )
	{
		boolState[required_OF_CCF_fail_B]  =  false;
	}

	if (boolState[relevant_evt_OF_loss_of_B] && ( !boolState[S_OF_loss_of_B]) )
	{
		boolState[relevant_evt_OF_CCF_fail_B]  =  true;
	}

	if (( !boolState[required_OF_loss_of_C]) || ( !boolState[S_OF_Shock]) )
	{
		boolState[required_OF_CCF_fail_C]  =  false;
	}

	if (boolState[relevant_evt_OF_loss_of_C] && ( !boolState[S_OF_loss_of_C]) )
	{
		boolState[relevant_evt_OF_CCF_fail_C]  =  true;
	}

	if ( !boolState[required_OF_loss_of_A] )
	{
		boolState[required_OF_Indep_fail_A]  =  false;
	}

	if (boolState[relevant_evt_OF_loss_of_A] && ( !boolState[S_OF_loss_of_A]) )
	{
		boolState[relevant_evt_OF_Indep_fail_A]  =  true;
	}

	if ( !boolState[required_OF_loss_of_B] )
	{
		boolState[required_OF_Indep_fail_B]  =  false;
	}

	if (boolState[relevant_evt_OF_loss_of_B] && ( !boolState[S_OF_loss_of_B]) )
	{
		boolState[relevant_evt_OF_Indep_fail_B]  =  true;
	}

	if ( !boolState[required_OF_loss_of_C] )
	{
		boolState[required_OF_Indep_fail_C]  =  false;
	}

	if (boolState[relevant_evt_OF_loss_of_C] && ( !boolState[S_OF_loss_of_C]) )
	{
		boolState[relevant_evt_OF_Indep_fail_C]  =  true;
	}

	if (((boolState[relevant_evt_OF_CCF_fail_C] && ( !boolState[S_OF_CCF_fail_C])) || (  boolState[relevant_evt_OF_CCF_fail_A] && ( !boolState[S_OF_CCF_fail_A]))) || (  boolState[relevant_evt_OF_CCF_fail_B] && ( !boolState[S_OF_CCF_fail_B])) )
	{
		boolState[relevant_evt_OF_Shock]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

	if ( !boolState[required_OF_system_loss] )
	{
		boolState[required_OF_loss_of_A]  =  false;
	}

	if (boolState[relevant_evt_OF_system_loss] && ( !boolState[S_OF_system_loss]) )
	{
		boolState[relevant_evt_OF_loss_of_A]  =  true;
	}

	if ( !boolState[required_OF_system_loss] )
	{
		boolState[required_OF_loss_of_B]  =  false;
	}

	if (boolState[relevant_evt_OF_system_loss] && ( !boolState[S_OF_system_loss]) )
	{
		boolState[relevant_evt_OF_loss_of_B]  =  true;
	}

	if ( !boolState[required_OF_system_loss] )
	{
		boolState[required_OF_loss_of_C]  =  false;
	}

	if (boolState[relevant_evt_OF_system_loss] && ( !boolState[S_OF_system_loss]) )
	{
		boolState[relevant_evt_OF_loss_of_C]  =  true;
	}

	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_system_loss]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_system_loss]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_CCF_fail_A]  =  boolState[S_OF_CCF_fail_A]  ;

	if (( !boolState[required_OF_CCF_fail_A]) && (( !boolState[already_standby_OF_CCF_fail_A]) && ( !boolState[already_required_OF_CCF_fail_A])) )
	{
		boolState[already_standby_OF_CCF_fail_A]  =  true;
	}



	boolState[already_S_OF_CCF_fail_B]  =  boolState[S_OF_CCF_fail_B]  ;

	if (( !boolState[required_OF_CCF_fail_B]) && (( !boolState[already_standby_OF_CCF_fail_B]) && ( !boolState[already_required_OF_CCF_fail_B])) )
	{
		boolState[already_standby_OF_CCF_fail_B]  =  true;
	}



	boolState[already_S_OF_CCF_fail_C]  =  boolState[S_OF_CCF_fail_C]  ;

	if (( !boolState[required_OF_CCF_fail_C]) && (( !boolState[already_standby_OF_CCF_fail_C]) && ( !boolState[already_required_OF_CCF_fail_C])) )
	{
		boolState[already_standby_OF_CCF_fail_C]  =  true;
	}



	boolState[already_S_OF_Indep_fail_A]  =  boolState[S_OF_Indep_fail_A]  ;



	boolState[already_S_OF_Indep_fail_B]  =  boolState[S_OF_Indep_fail_B]  ;



	boolState[already_S_OF_Indep_fail_C]  =  boolState[S_OF_Indep_fail_C]  ;



	boolState[already_S_OF_Shock]  =  boolState[S_OF_Shock]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;



	boolState[already_S_OF_loss_of_A]  =  boolState[S_OF_loss_of_A]  ;



	boolState[already_S_OF_loss_of_B]  =  boolState[S_OF_loss_of_B]  ;



	boolState[already_S_OF_loss_of_C]  =  boolState[S_OF_loss_of_C]  ;



	boolState[already_S_OF_system_loss]  =  boolState[S_OF_system_loss]  ;

}


void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::runOnceInteractionStep_tops()
{
	if (boolState[required_OF_CCF_fail_A] && boolState[already_standby_OF_CCF_fail_A] )
	{
		boolState[to_be_fired_OF_CCF_fail_A]  =  true;
	}

	if (boolState[required_OF_CCF_fail_B] && boolState[already_standby_OF_CCF_fail_B] )
	{
		boolState[to_be_fired_OF_CCF_fail_B]  =  true;
	}

	if (boolState[required_OF_CCF_fail_C] && boolState[already_standby_OF_CCF_fail_C] )
	{
		boolState[to_be_fired_OF_CCF_fail_C]  =  true;
	}

}

void
storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::runInteractions() {
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
		runOnceInteractionStep_initialization();

		comparator = compareStates();
            counter++;

        } while (comparator > 0 && counter < max_interactions_loop);
        if (comparator <= 0)
        {
            cout << "==> Stabilisation of interactions at loop #" << counter << " for runInteractionStep_initialization() ." << endl;
        }
        else {
            cout << "==> Maximum of interactions loop  reached : #" << counter <<" for runOnceInteractionStep_initialization()." << endl;
        }
         
        counter = 0;
        comparator = 1;
        do
        {
            //cout << counter << endl;
            saveCurrentState();
		runOnceInteractionStep_propagate_effect_S();

		comparator = compareStates();
            counter++;

        } while (comparator > 0 && counter < max_interactions_loop);
        if (comparator <= 0)
        {
            cout << "==> Stabilisation of interactions at loop #" << counter << " for runInteractionStep_propagate_effect_S() ." << endl;
        }
        else {
            cout << "==> Maximum of interactions loop  reached : #" << counter <<" for runOnceInteractionStep_propagate_effect_S()." << endl;
        }
         
        counter = 0;
        comparator = 1;
        do
        {
            //cout << counter << endl;
            saveCurrentState();
		runOnceInteractionStep_propagate_effect_required();

		comparator = compareStates();
            counter++;

        } while (comparator > 0 && counter < max_interactions_loop);
        if (comparator <= 0)
        {
            cout << "==> Stabilisation of interactions at loop #" << counter << " for runInteractionStep_propagate_effect_required() ." << endl;
        }
        else {
            cout << "==> Maximum of interactions loop  reached : #" << counter <<" for runOnceInteractionStep_propagate_effect_required()." << endl;
        }
         
        counter = 0;
        comparator = 1;
        do
        {
            //cout << counter << endl;
            saveCurrentState();
		runOnceInteractionStep_propagate_leaves();

		comparator = compareStates();
            counter++;

        } while (comparator > 0 && counter < max_interactions_loop);
        if (comparator <= 0)
        {
            cout << "==> Stabilisation of interactions at loop #" << counter << " for runInteractionStep_propagate_leaves() ." << endl;
        }
        else {
            cout << "==> Maximum of interactions loop  reached : #" << counter <<" for runOnceInteractionStep_propagate_leaves()." << endl;
        }
         
        counter = 0;
        comparator = 1;
        do
        {
            //cout << counter << endl;
            saveCurrentState();
		runOnceInteractionStep_tops();

		comparator = compareStates();
            counter++;

        } while (comparator > 0 && counter < max_interactions_loop);
        if (comparator <= 0)
        {
            cout << "==> Stabilisation of interactions at loop #" << counter << " for runInteractionStep_tops() ." << endl;
        }
        else {
            cout << "==> Maximum of interactions loop  reached : #" << counter <<" for runOnceInteractionStep_tops()." << endl;
        }
         
        // ------------------- Handling of FailureState element --------------------------------
    
	boolFailureState[exp0] = ( boolState[S_OF_UE_1] );
        cout << endl;
    }void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_03_CCF_Trim_Max_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    