#include <iostream>
#include "FigaroModelBDMP_15_Trim_Article_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_A1 = true;
	boolState[already_S_OF_A1] = false;
	REINITIALISATION_OF_S_OF_A1 = false;
	REINITIALISATION_OF_relevant_evt_OF_A1 = false;
	boolState[failF_OF_A1] = false;
	REINITIALISATION_OF_required_OF_A1_lost = true;
	boolState[already_S_OF_A1_lost] = false;
	REINITIALISATION_OF_S_OF_A1_lost = false;
	REINITIALISATION_OF_relevant_evt_OF_A1_lost = false;
	REINITIALISATION_OF_required_OF_A2 = true;
	boolState[already_S_OF_A2] = false;
	REINITIALISATION_OF_S_OF_A2 = false;
	REINITIALISATION_OF_relevant_evt_OF_A2 = false;
	boolState[failF_OF_A2] = false;
	REINITIALISATION_OF_required_OF_A2_lost = true;
	boolState[already_S_OF_A2_lost] = false;
	REINITIALISATION_OF_S_OF_A2_lost = false;
	REINITIALISATION_OF_relevant_evt_OF_A2_lost = false;
	REINITIALISATION_OF_required_OF_AND_1 = true;
	boolState[already_S_OF_AND_1] = false;
	REINITIALISATION_OF_S_OF_AND_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_1 = false;
	REINITIALISATION_OF_required_OF_A_lost = true;
	boolState[already_S_OF_A_lost] = false;
	REINITIALISATION_OF_S_OF_A_lost = false;
	REINITIALISATION_OF_relevant_evt_OF_A_lost = false;
	REINITIALISATION_OF_required_OF_B1 = true;
	boolState[already_S_OF_B1] = false;
	REINITIALISATION_OF_S_OF_B1 = false;
	REINITIALISATION_OF_relevant_evt_OF_B1 = false;
	boolState[failF_OF_B1] = false;
	REINITIALISATION_OF_required_OF_B1_lost = true;
	boolState[already_S_OF_B1_lost] = false;
	REINITIALISATION_OF_S_OF_B1_lost = false;
	REINITIALISATION_OF_relevant_evt_OF_B1_lost = false;
	REINITIALISATION_OF_required_OF_B2 = true;
	boolState[already_S_OF_B2] = false;
	REINITIALISATION_OF_S_OF_B2 = false;
	REINITIALISATION_OF_relevant_evt_OF_B2 = false;
	boolState[failF_OF_B2] = false;
	REINITIALISATION_OF_required_OF_B2_lost = true;
	boolState[already_S_OF_B2_lost] = false;
	REINITIALISATION_OF_S_OF_B2_lost = false;
	REINITIALISATION_OF_relevant_evt_OF_B2_lost = false;
	REINITIALISATION_OF_required_OF_B_lost = true;
	boolState[already_S_OF_B_lost] = false;
	REINITIALISATION_OF_S_OF_B_lost = false;
	REINITIALISATION_OF_relevant_evt_OF_B_lost = false;
	REINITIALISATION_OF_required_OF_K1 = true;
	boolState[already_S_OF_K1] = false;
	REINITIALISATION_OF_S_OF_K1 = false;
	REINITIALISATION_OF_relevant_evt_OF_K1 = false;
	boolState[failF_OF_K1] = false;
	REINITIALISATION_OF_required_OF_K2 = true;
	boolState[already_S_OF_K2] = false;
	REINITIALISATION_OF_S_OF_K2 = false;
	REINITIALISATION_OF_relevant_evt_OF_K2 = false;
	boolState[failF_OF_K2] = false;
	REINITIALISATION_OF_required_OF_K3 = true;
	boolState[already_S_OF_K3] = false;
	REINITIALISATION_OF_S_OF_K3 = false;
	REINITIALISATION_OF_relevant_evt_OF_K3 = false;
	boolState[failF_OF_K3] = false;
	REINITIALISATION_OF_required_OF_OR_1 = true;
	boolState[already_S_OF_OR_1] = false;
	REINITIALISATION_OF_S_OF_OR_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_1 = false;
	REINITIALISATION_OF_required_OF_OR_2 = true;
	boolState[already_S_OF_OR_2] = false;
	REINITIALISATION_OF_S_OF_OR_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_2 = false;
	REINITIALISATION_OF_required_OF_SF_1 = true;
	boolState[already_S_OF_SF_1] = false;
	REINITIALISATION_OF_S_OF_SF_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_SF_1 = false;
	boolState[failF_OF_SF_1] = false;
	boolState[failS_OF_SF_1] = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_A1 = false;
	FIRE_xx11_OF_A1 = false;
	FIRE_xx10_OF_A2 = false;
	FIRE_xx11_OF_A2 = false;
	FIRE_xx10_OF_B1 = false;
	FIRE_xx11_OF_B1 = false;
	FIRE_xx10_OF_B2 = false;
	FIRE_xx11_OF_B2 = false;
	FIRE_xx10_OF_K1 = false;
	FIRE_xx11_OF_K1 = false;
	FIRE_xx10_OF_K2 = false;
	FIRE_xx11_OF_K2 = false;
	FIRE_xx10_OF_K3 = false;
	FIRE_xx11_OF_K3 = false;
	FIRE_xx17_OF_SF_1 = false;
	FIRE_xx18_OF_SF_1 = false;
	FIRE_xx19_OF_SF_1 = false;

}

void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_A1] | Value : " << boolState[required_OF_A1] << endl;
	cout << "Attribute :  boolState[already_S_OF_A1] | Value : " << boolState[already_S_OF_A1] << endl;
	cout << "Attribute :  boolState[S_OF_A1] | Value : " << boolState[S_OF_A1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A1] | Value : " << boolState[relevant_evt_OF_A1] << endl;
	cout << "Attribute :  boolState[failF_OF_A1] | Value : " << boolState[failF_OF_A1] << endl;
	cout << "Attribute :  boolState[required_OF_A1_lost] | Value : " << boolState[required_OF_A1_lost] << endl;
	cout << "Attribute :  boolState[already_S_OF_A1_lost] | Value : " << boolState[already_S_OF_A1_lost] << endl;
	cout << "Attribute :  boolState[S_OF_A1_lost] | Value : " << boolState[S_OF_A1_lost] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A1_lost] | Value : " << boolState[relevant_evt_OF_A1_lost] << endl;
	cout << "Attribute :  boolState[required_OF_A2] | Value : " << boolState[required_OF_A2] << endl;
	cout << "Attribute :  boolState[already_S_OF_A2] | Value : " << boolState[already_S_OF_A2] << endl;
	cout << "Attribute :  boolState[S_OF_A2] | Value : " << boolState[S_OF_A2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A2] | Value : " << boolState[relevant_evt_OF_A2] << endl;
	cout << "Attribute :  boolState[failF_OF_A2] | Value : " << boolState[failF_OF_A2] << endl;
	cout << "Attribute :  boolState[required_OF_A2_lost] | Value : " << boolState[required_OF_A2_lost] << endl;
	cout << "Attribute :  boolState[already_S_OF_A2_lost] | Value : " << boolState[already_S_OF_A2_lost] << endl;
	cout << "Attribute :  boolState[S_OF_A2_lost] | Value : " << boolState[S_OF_A2_lost] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A2_lost] | Value : " << boolState[relevant_evt_OF_A2_lost] << endl;
	cout << "Attribute :  boolState[required_OF_AND_1] | Value : " << boolState[required_OF_AND_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_1] | Value : " << boolState[already_S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[S_OF_AND_1] | Value : " << boolState[S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_1] | Value : " << boolState[relevant_evt_OF_AND_1] << endl;
	cout << "Attribute :  boolState[required_OF_A_lost] | Value : " << boolState[required_OF_A_lost] << endl;
	cout << "Attribute :  boolState[already_S_OF_A_lost] | Value : " << boolState[already_S_OF_A_lost] << endl;
	cout << "Attribute :  boolState[S_OF_A_lost] | Value : " << boolState[S_OF_A_lost] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A_lost] | Value : " << boolState[relevant_evt_OF_A_lost] << endl;
	cout << "Attribute :  boolState[required_OF_B1] | Value : " << boolState[required_OF_B1] << endl;
	cout << "Attribute :  boolState[already_S_OF_B1] | Value : " << boolState[already_S_OF_B1] << endl;
	cout << "Attribute :  boolState[S_OF_B1] | Value : " << boolState[S_OF_B1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_B1] | Value : " << boolState[relevant_evt_OF_B1] << endl;
	cout << "Attribute :  boolState[failF_OF_B1] | Value : " << boolState[failF_OF_B1] << endl;
	cout << "Attribute :  boolState[required_OF_B1_lost] | Value : " << boolState[required_OF_B1_lost] << endl;
	cout << "Attribute :  boolState[already_S_OF_B1_lost] | Value : " << boolState[already_S_OF_B1_lost] << endl;
	cout << "Attribute :  boolState[S_OF_B1_lost] | Value : " << boolState[S_OF_B1_lost] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_B1_lost] | Value : " << boolState[relevant_evt_OF_B1_lost] << endl;
	cout << "Attribute :  boolState[required_OF_B2] | Value : " << boolState[required_OF_B2] << endl;
	cout << "Attribute :  boolState[already_S_OF_B2] | Value : " << boolState[already_S_OF_B2] << endl;
	cout << "Attribute :  boolState[S_OF_B2] | Value : " << boolState[S_OF_B2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_B2] | Value : " << boolState[relevant_evt_OF_B2] << endl;
	cout << "Attribute :  boolState[failF_OF_B2] | Value : " << boolState[failF_OF_B2] << endl;
	cout << "Attribute :  boolState[required_OF_B2_lost] | Value : " << boolState[required_OF_B2_lost] << endl;
	cout << "Attribute :  boolState[already_S_OF_B2_lost] | Value : " << boolState[already_S_OF_B2_lost] << endl;
	cout << "Attribute :  boolState[S_OF_B2_lost] | Value : " << boolState[S_OF_B2_lost] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_B2_lost] | Value : " << boolState[relevant_evt_OF_B2_lost] << endl;
	cout << "Attribute :  boolState[required_OF_B_lost] | Value : " << boolState[required_OF_B_lost] << endl;
	cout << "Attribute :  boolState[already_S_OF_B_lost] | Value : " << boolState[already_S_OF_B_lost] << endl;
	cout << "Attribute :  boolState[S_OF_B_lost] | Value : " << boolState[S_OF_B_lost] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_B_lost] | Value : " << boolState[relevant_evt_OF_B_lost] << endl;
	cout << "Attribute :  boolState[required_OF_K1] | Value : " << boolState[required_OF_K1] << endl;
	cout << "Attribute :  boolState[already_S_OF_K1] | Value : " << boolState[already_S_OF_K1] << endl;
	cout << "Attribute :  boolState[S_OF_K1] | Value : " << boolState[S_OF_K1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_K1] | Value : " << boolState[relevant_evt_OF_K1] << endl;
	cout << "Attribute :  boolState[failF_OF_K1] | Value : " << boolState[failF_OF_K1] << endl;
	cout << "Attribute :  boolState[required_OF_K2] | Value : " << boolState[required_OF_K2] << endl;
	cout << "Attribute :  boolState[already_S_OF_K2] | Value : " << boolState[already_S_OF_K2] << endl;
	cout << "Attribute :  boolState[S_OF_K2] | Value : " << boolState[S_OF_K2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_K2] | Value : " << boolState[relevant_evt_OF_K2] << endl;
	cout << "Attribute :  boolState[failF_OF_K2] | Value : " << boolState[failF_OF_K2] << endl;
	cout << "Attribute :  boolState[required_OF_K3] | Value : " << boolState[required_OF_K3] << endl;
	cout << "Attribute :  boolState[already_S_OF_K3] | Value : " << boolState[already_S_OF_K3] << endl;
	cout << "Attribute :  boolState[S_OF_K3] | Value : " << boolState[S_OF_K3] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_K3] | Value : " << boolState[relevant_evt_OF_K3] << endl;
	cout << "Attribute :  boolState[failF_OF_K3] | Value : " << boolState[failF_OF_K3] << endl;
	cout << "Attribute :  boolState[required_OF_OR_1] | Value : " << boolState[required_OF_OR_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_1] | Value : " << boolState[already_S_OF_OR_1] << endl;
	cout << "Attribute :  boolState[S_OF_OR_1] | Value : " << boolState[S_OF_OR_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_1] | Value : " << boolState[relevant_evt_OF_OR_1] << endl;
	cout << "Attribute :  boolState[required_OF_OR_2] | Value : " << boolState[required_OF_OR_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_2] | Value : " << boolState[already_S_OF_OR_2] << endl;
	cout << "Attribute :  boolState[S_OF_OR_2] | Value : " << boolState[S_OF_OR_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_2] | Value : " << boolState[relevant_evt_OF_OR_2] << endl;
	cout << "Attribute :  boolState[required_OF_SF_1] | Value : " << boolState[required_OF_SF_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_SF_1] | Value : " << boolState[already_S_OF_SF_1] << endl;
	cout << "Attribute :  boolState[S_OF_SF_1] | Value : " << boolState[S_OF_SF_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_SF_1] | Value : " << boolState[relevant_evt_OF_SF_1] << endl;
	cout << "Attribute :  boolState[failF_OF_SF_1] | Value : " << boolState[failF_OF_SF_1] << endl;
	cout << "Attribute :  boolState[failS_OF_SF_1] | Value : " << boolState[failS_OF_SF_1] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::figaromodelhasinstransitions()
{
	return false;
}
void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::doReinitialisations()
{
	boolState[required_OF_A1] = REINITIALISATION_OF_required_OF_A1;
	boolState[S_OF_A1] = REINITIALISATION_OF_S_OF_A1;
	boolState[relevant_evt_OF_A1] = REINITIALISATION_OF_relevant_evt_OF_A1;
	boolState[required_OF_A1_lost] = REINITIALISATION_OF_required_OF_A1_lost;
	boolState[S_OF_A1_lost] = REINITIALISATION_OF_S_OF_A1_lost;
	boolState[relevant_evt_OF_A1_lost] = REINITIALISATION_OF_relevant_evt_OF_A1_lost;
	boolState[required_OF_A2] = REINITIALISATION_OF_required_OF_A2;
	boolState[S_OF_A2] = REINITIALISATION_OF_S_OF_A2;
	boolState[relevant_evt_OF_A2] = REINITIALISATION_OF_relevant_evt_OF_A2;
	boolState[required_OF_A2_lost] = REINITIALISATION_OF_required_OF_A2_lost;
	boolState[S_OF_A2_lost] = REINITIALISATION_OF_S_OF_A2_lost;
	boolState[relevant_evt_OF_A2_lost] = REINITIALISATION_OF_relevant_evt_OF_A2_lost;
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_A_lost] = REINITIALISATION_OF_required_OF_A_lost;
	boolState[S_OF_A_lost] = REINITIALISATION_OF_S_OF_A_lost;
	boolState[relevant_evt_OF_A_lost] = REINITIALISATION_OF_relevant_evt_OF_A_lost;
	boolState[required_OF_B1] = REINITIALISATION_OF_required_OF_B1;
	boolState[S_OF_B1] = REINITIALISATION_OF_S_OF_B1;
	boolState[relevant_evt_OF_B1] = REINITIALISATION_OF_relevant_evt_OF_B1;
	boolState[required_OF_B1_lost] = REINITIALISATION_OF_required_OF_B1_lost;
	boolState[S_OF_B1_lost] = REINITIALISATION_OF_S_OF_B1_lost;
	boolState[relevant_evt_OF_B1_lost] = REINITIALISATION_OF_relevant_evt_OF_B1_lost;
	boolState[required_OF_B2] = REINITIALISATION_OF_required_OF_B2;
	boolState[S_OF_B2] = REINITIALISATION_OF_S_OF_B2;
	boolState[relevant_evt_OF_B2] = REINITIALISATION_OF_relevant_evt_OF_B2;
	boolState[required_OF_B2_lost] = REINITIALISATION_OF_required_OF_B2_lost;
	boolState[S_OF_B2_lost] = REINITIALISATION_OF_S_OF_B2_lost;
	boolState[relevant_evt_OF_B2_lost] = REINITIALISATION_OF_relevant_evt_OF_B2_lost;
	boolState[required_OF_B_lost] = REINITIALISATION_OF_required_OF_B_lost;
	boolState[S_OF_B_lost] = REINITIALISATION_OF_S_OF_B_lost;
	boolState[relevant_evt_OF_B_lost] = REINITIALISATION_OF_relevant_evt_OF_B_lost;
	boolState[required_OF_K1] = REINITIALISATION_OF_required_OF_K1;
	boolState[S_OF_K1] = REINITIALISATION_OF_S_OF_K1;
	boolState[relevant_evt_OF_K1] = REINITIALISATION_OF_relevant_evt_OF_K1;
	boolState[required_OF_K2] = REINITIALISATION_OF_required_OF_K2;
	boolState[S_OF_K2] = REINITIALISATION_OF_S_OF_K2;
	boolState[relevant_evt_OF_K2] = REINITIALISATION_OF_relevant_evt_OF_K2;
	boolState[required_OF_K3] = REINITIALISATION_OF_required_OF_K3;
	boolState[S_OF_K3] = REINITIALISATION_OF_S_OF_K3;
	boolState[relevant_evt_OF_K3] = REINITIALISATION_OF_relevant_evt_OF_K3;
	boolState[required_OF_OR_1] = REINITIALISATION_OF_required_OF_OR_1;
	boolState[S_OF_OR_1] = REINITIALISATION_OF_S_OF_OR_1;
	boolState[relevant_evt_OF_OR_1] = REINITIALISATION_OF_relevant_evt_OF_OR_1;
	boolState[required_OF_OR_2] = REINITIALISATION_OF_required_OF_OR_2;
	boolState[S_OF_OR_2] = REINITIALISATION_OF_S_OF_OR_2;
	boolState[relevant_evt_OF_OR_2] = REINITIALISATION_OF_relevant_evt_OF_OR_2;
	boolState[required_OF_SF_1] = REINITIALISATION_OF_required_OF_SF_1;
	boolState[S_OF_SF_1] = REINITIALISATION_OF_S_OF_SF_1;
	boolState[relevant_evt_OF_SF_1] = REINITIALISATION_OF_relevant_evt_OF_SF_1;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
}

void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_A1 = true;
	}

	if (numFire == 1)
	{
		FIRE_xx11_OF_A1 = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_A2 = true;
	}

	if (numFire == 3)
	{
		FIRE_xx11_OF_A2 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_B1 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx11_OF_B1 = true;
	}

	if (numFire == 6)
	{
		FIRE_xx10_OF_B2 = true;
	}

	if (numFire == 7)
	{
		FIRE_xx11_OF_B2 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx10_OF_K1 = true;
	}

	if (numFire == 9)
	{
		FIRE_xx11_OF_K1 = true;
	}

	if (numFire == 10)
	{
		FIRE_xx10_OF_K2 = true;
	}

	if (numFire == 11)
	{
		FIRE_xx11_OF_K2 = true;
	}

	if (numFire == 12)
	{
		FIRE_xx10_OF_K3 = true;
	}

	if (numFire == 13)
	{
		FIRE_xx11_OF_K3 = true;
	}

	if (numFire == 14)
	{
		FIRE_xx17_OF_SF_1 = true;
	}

	if (numFire == 15)
	{
		FIRE_xx18_OF_SF_1 = true;
	}

	if (numFire == 16)
	{
		FIRE_xx19_OF_SF_1 = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_A1
	if ((boolState[failF_OF_A1] == false) && (boolState[required_OF_A1] && boolState[relevant_evt_OF_A1])) 
	{
		 
		if (FIRE_xx10_OF_A1)
		{
			boolState[failF_OF_A1]  =  true;
			FIRE_xx10_OF_A1 = false;
		}
	}

	// Occurrence xx11_OF_A1
	if (boolState[failF_OF_A1] == true) 
	{
		 
		if (FIRE_xx11_OF_A1)
		{
			boolState[failF_OF_A1]  =  false;
			FIRE_xx11_OF_A1 = false;
		}
	}

	// Occurrence xx10_OF_A2
	if ((boolState[failF_OF_A2] == false) && (boolState[required_OF_A2] && boolState[relevant_evt_OF_A2])) 
	{
		 
		if (FIRE_xx10_OF_A2)
		{
			boolState[failF_OF_A2]  =  true;
			FIRE_xx10_OF_A2 = false;
		}
	}

	// Occurrence xx11_OF_A2
	if (boolState[failF_OF_A2] == true) 
	{
		 
		if (FIRE_xx11_OF_A2)
		{
			boolState[failF_OF_A2]  =  false;
			FIRE_xx11_OF_A2 = false;
		}
	}

	// Occurrence xx10_OF_B1
	if ((boolState[failF_OF_B1] == false) && (boolState[required_OF_B1] && boolState[relevant_evt_OF_B1])) 
	{
		 
		if (FIRE_xx10_OF_B1)
		{
			boolState[failF_OF_B1]  =  true;
			FIRE_xx10_OF_B1 = false;
		}
	}

	// Occurrence xx11_OF_B1
	if (boolState[failF_OF_B1] == true) 
	{
		 
		if (FIRE_xx11_OF_B1)
		{
			boolState[failF_OF_B1]  =  false;
			FIRE_xx11_OF_B1 = false;
		}
	}

	// Occurrence xx10_OF_B2
	if ((boolState[failF_OF_B2] == false) && (boolState[required_OF_B2] && boolState[relevant_evt_OF_B2])) 
	{
		 
		if (FIRE_xx10_OF_B2)
		{
			boolState[failF_OF_B2]  =  true;
			FIRE_xx10_OF_B2 = false;
		}
	}

	// Occurrence xx11_OF_B2
	if (boolState[failF_OF_B2] == true) 
	{
		 
		if (FIRE_xx11_OF_B2)
		{
			boolState[failF_OF_B2]  =  false;
			FIRE_xx11_OF_B2 = false;
		}
	}

	// Occurrence xx10_OF_K1
	if ((boolState[failF_OF_K1] == false) && (boolState[required_OF_K1] && boolState[relevant_evt_OF_K1])) 
	{
		 
		if (FIRE_xx10_OF_K1)
		{
			boolState[failF_OF_K1]  =  true;
			FIRE_xx10_OF_K1 = false;
		}
	}

	// Occurrence xx11_OF_K1
	if (boolState[failF_OF_K1] == true) 
	{
		 
		if (FIRE_xx11_OF_K1)
		{
			boolState[failF_OF_K1]  =  false;
			FIRE_xx11_OF_K1 = false;
		}
	}

	// Occurrence xx10_OF_K2
	if ((boolState[failF_OF_K2] == false) && (boolState[required_OF_K2] && boolState[relevant_evt_OF_K2])) 
	{
		 
		if (FIRE_xx10_OF_K2)
		{
			boolState[failF_OF_K2]  =  true;
			FIRE_xx10_OF_K2 = false;
		}
	}

	// Occurrence xx11_OF_K2
	if (boolState[failF_OF_K2] == true) 
	{
		 
		if (FIRE_xx11_OF_K2)
		{
			boolState[failF_OF_K2]  =  false;
			FIRE_xx11_OF_K2 = false;
		}
	}

	// Occurrence xx10_OF_K3
	if ((boolState[failF_OF_K3] == false) && (boolState[required_OF_K3] && boolState[relevant_evt_OF_K3])) 
	{
		 
		if (FIRE_xx10_OF_K3)
		{
			boolState[failF_OF_K3]  =  true;
			FIRE_xx10_OF_K3 = false;
		}
	}

	// Occurrence xx11_OF_K3
	if (boolState[failF_OF_K3] == true) 
	{
		 
		if (FIRE_xx11_OF_K3)
		{
			boolState[failF_OF_K3]  =  false;
			FIRE_xx11_OF_K3 = false;
		}
	}

	// Occurrence xx17_OF_SF_1
	if ((boolState[failF_OF_SF_1] == false) && ((boolState[required_OF_SF_1] &&  boolState[relevant_evt_OF_SF_1]) && ( !boolState[failS_OF_SF_1]))) 
	{
		 
		if (FIRE_xx17_OF_SF_1)
		{
			boolState[failF_OF_SF_1]  =  true;
			FIRE_xx17_OF_SF_1 = false;
		}
	}

	// Occurrence xx18_OF_SF_1
	if ((boolState[failS_OF_SF_1] == false) && ((( !boolState[required_OF_SF_1]) &&  boolState[relevant_evt_OF_SF_1]) && ( !boolState[failF_OF_SF_1]))) 
	{
		 
		if (FIRE_xx18_OF_SF_1)
		{
			boolState[failS_OF_SF_1]  =  true;
			FIRE_xx18_OF_SF_1 = false;
		}
	}

	// Occurrence xx19_OF_SF_1
	if ((boolState[failS_OF_SF_1] == true) || (boolState[failF_OF_SF_1] == true)) 
	{
		 
		if (FIRE_xx19_OF_SF_1)
		{
			boolState[failS_OF_SF_1]  =  false;
			boolState[failF_OF_SF_1]  =  false;
			FIRE_xx19_OF_SF_1 = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::showFireableOccurrences()
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
     
	if ((boolState[failF_OF_A1] == false) && (boolState[required_OF_A1] && boolState[relevant_evt_OF_A1]))
	{
		cout << "0 : xx10_OF_A1 : FAULT failF  LABEL \"failure in operation A1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_A1]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_A1] == true)
	{
		cout << "1 : xx11_OF_A1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_A1]  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_A2] == false) && (boolState[required_OF_A2] && boolState[relevant_evt_OF_A2]))
	{
		cout << "2 : xx10_OF_A2 : FAULT failF  LABEL \"failure in operation A2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_A2]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_A2] == true)
	{
		cout << "3 : xx11_OF_A2 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_A2]  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_B1] == false) && (boolState[required_OF_B1] && boolState[relevant_evt_OF_B1]))
	{
		cout << "4 : xx10_OF_B1 : FAULT failF  LABEL \"failure in operation B1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_B1]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_B1] == true)
	{
		cout << "5 : xx11_OF_B1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_B1]  =  FALSE" << endl;
		list.push_back(make_tuple(5, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_B2] == false) && (boolState[required_OF_B2] && boolState[relevant_evt_OF_B2]))
	{
		cout << "6 : xx10_OF_B2 : FAULT failF  LABEL \"failure in operation B2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_B2]  =  TRUE" << endl;
		list.push_back(make_tuple(6, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_B2] == true)
	{
		cout << "7 : xx11_OF_B2 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_B2]  =  FALSE" << endl;
		list.push_back(make_tuple(7, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_K1] == false) && (boolState[required_OF_K1] && boolState[relevant_evt_OF_K1]))
	{
		cout << "8 : xx10_OF_K1 : FAULT failF  LABEL \"failure in operation K1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_K1]  =  TRUE" << endl;
		list.push_back(make_tuple(8, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_K1] == true)
	{
		cout << "9 : xx11_OF_K1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_K1]  =  FALSE" << endl;
		list.push_back(make_tuple(9, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_K2] == false) && (boolState[required_OF_K2] && boolState[relevant_evt_OF_K2]))
	{
		cout << "10 : xx10_OF_K2 : FAULT failF  LABEL \"failure in operation K2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_K2]  =  TRUE" << endl;
		list.push_back(make_tuple(10, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_K2] == true)
	{
		cout << "11 : xx11_OF_K2 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_K2]  =  FALSE" << endl;
		list.push_back(make_tuple(11, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_K3] == false) && (boolState[required_OF_K3] && boolState[relevant_evt_OF_K3]))
	{
		cout << "12 : xx10_OF_K3 : FAULT failF  LABEL \"failure in operation K3\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_K3]  =  TRUE" << endl;
		list.push_back(make_tuple(12, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_K3] == true)
	{
		cout << "13 : xx11_OF_K3 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_K3]  =  FALSE" << endl;
		list.push_back(make_tuple(13, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_SF_1] == false) && ((boolState[required_OF_SF_1] && boolState[relevant_evt_OF_SF_1]) && ( !boolState[failS_OF_SF_1])))
	{
		cout << "14 : xx17_OF_SF_1 : FAULT failF  LABEL \"failure in operation SF_1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_SF_1]  =  TRUE" << endl;
		list.push_back(make_tuple(14, 0.0001, "EXP", 0));
	}
	if ((boolState[failS_OF_SF_1] == false) && ((( !boolState[required_OF_SF_1]) && boolState[relevant_evt_OF_SF_1]) && ( !boolState[failF_OF_SF_1])))
	{
		cout << "15 : xx18_OF_SF_1 : FAULT failS  LABEL \"standby failure SF_1\"  DIST EXP (1e-05)  INDUCING boolState[failS_OF_SF_1]  =  TRUE" << endl;
		list.push_back(make_tuple(15, 1e-05, "EXP", 0));
	}
	if ((boolState[failS_OF_SF_1] == true) || (boolState[failF_OF_SF_1] == true))
	{
		cout << "16 : xx19_OF_SF_1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failS_OF_SF_1]  =  FALSE,failF_OF_SF_1  =  FALSE" << endl;
		list.push_back(make_tuple(16, 0.1, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_A1] == true )
	{
		boolState[S_OF_A1]  =  true;
	}

	if (boolState[failF_OF_A2] == true )
	{
		boolState[S_OF_A2]  =  true;
	}

	if (boolState[failF_OF_B1] == true )
	{
		boolState[S_OF_B1]  =  true;
	}

	if (boolState[failF_OF_B2] == true )
	{
		boolState[S_OF_B2]  =  true;
	}

	if (boolState[failF_OF_K1] == true )
	{
		boolState[S_OF_K1]  =  true;
	}

	if (boolState[failF_OF_K2] == true )
	{
		boolState[S_OF_K2]  =  true;
	}

	if (boolState[failF_OF_K3] == true )
	{
		boolState[S_OF_K3]  =  true;
	}

	if ((boolState[failS_OF_SF_1] == true) || (boolState[failF_OF_SF_1] == true) )
	{
		boolState[S_OF_SF_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_A1] || boolState[S_OF_K1] )
	{
		boolState[S_OF_A1_lost]  =  true;
	}

	if (boolState[S_OF_A2] || boolState[S_OF_K2] )
	{
		boolState[S_OF_A2_lost]  =  true;
	}

	if (boolState[S_OF_A_lost] && boolState[S_OF_B_lost] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if ((boolState[S_OF_A1_lost] && boolState[S_OF_A2_lost]) && boolState[S_OF_OR_1] )
	{
		boolState[S_OF_A_lost]  =  true;
	}

	if (boolState[S_OF_B1] || boolState[S_OF_K2] )
	{
		boolState[S_OF_B1_lost]  =  true;
	}

	if (boolState[S_OF_B2] || boolState[S_OF_K1] )
	{
		boolState[S_OF_B2_lost]  =  true;
	}

	if ((boolState[S_OF_B1_lost] && boolState[S_OF_B2_lost]) && boolState[S_OF_OR_1] )
	{
		boolState[S_OF_B_lost]  =  true;
	}

	if (boolState[S_OF_K3] || boolState[S_OF_SF_1] )
	{
		boolState[S_OF_OR_1]  =  true;
	}

	if (boolState[S_OF_A2_lost] || boolState[S_OF_B2_lost] )
	{
		boolState[S_OF_OR_2]  =  true;
	}

	if (boolState[S_OF_AND_1] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_A1_lost] )
	{
		boolState[required_OF_A1]  =  false;
	}

	if ((boolState[relevant_evt_OF_A1_lost] && ( !boolState[S_OF_A1_lost])) || ( !boolState[S_OF_A2]) )
	{
		boolState[relevant_evt_OF_A1]  =  true;
	}

	if ( !boolState[required_OF_A_lost] )
	{
		boolState[required_OF_A1_lost]  =  false;
	}

	if (boolState[relevant_evt_OF_A_lost] && ( !boolState[S_OF_A_lost]) )
	{
		boolState[relevant_evt_OF_A1_lost]  =  true;
	}

	if (( !boolState[required_OF_A2_lost]) || ( !boolState[S_OF_A1]) )
	{
		boolState[required_OF_A2]  =  false;
	}

	if (boolState[relevant_evt_OF_A2_lost] && ( !boolState[S_OF_A2_lost]) )
	{
		boolState[relevant_evt_OF_A2]  =  true;
	}

	if (( !boolState[required_OF_A_lost]) && ( !boolState[required_OF_OR_2]) )
	{
		boolState[required_OF_A2_lost]  =  false;
	}

	if ((boolState[relevant_evt_OF_A_lost] && ( !boolState[S_OF_A_lost])) || (  boolState[relevant_evt_OF_OR_2] && ( !boolState[S_OF_OR_2])) )
	{
		boolState[relevant_evt_OF_A2_lost]  =  true;
	}

	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_AND_1]  =  true;
	}

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_A_lost]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_A_lost]  =  true;
	}

	if ( !boolState[required_OF_B1_lost] )
	{
		boolState[required_OF_B1]  =  false;
	}

	if ((boolState[relevant_evt_OF_B1_lost] && ( !boolState[S_OF_B1_lost])) || ( !boolState[S_OF_B2]) )
	{
		boolState[relevant_evt_OF_B1]  =  true;
	}

	if ( !boolState[required_OF_B_lost] )
	{
		boolState[required_OF_B1_lost]  =  false;
	}

	if (boolState[relevant_evt_OF_B_lost] && ( !boolState[S_OF_B_lost]) )
	{
		boolState[relevant_evt_OF_B1_lost]  =  true;
	}

	if (( !boolState[required_OF_B2_lost]) || ( !boolState[S_OF_B1]) )
	{
		boolState[required_OF_B2]  =  false;
	}

	if (boolState[relevant_evt_OF_B2_lost] && ( !boolState[S_OF_B2_lost]) )
	{
		boolState[relevant_evt_OF_B2]  =  true;
	}

	if (( !boolState[required_OF_B_lost]) && ( !boolState[required_OF_OR_2]) )
	{
		boolState[required_OF_B2_lost]  =  false;
	}

	if ((boolState[relevant_evt_OF_B_lost] && ( !boolState[S_OF_B_lost])) || (  boolState[relevant_evt_OF_OR_2] && ( !boolState[S_OF_OR_2])) )
	{
		boolState[relevant_evt_OF_B2_lost]  =  true;
	}

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_B_lost]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_B_lost]  =  true;
	}

	if (( !boolState[required_OF_A1_lost]) && ( !boolState[required_OF_B2_lost]) )
	{
		boolState[required_OF_K1]  =  false;
	}

	if ((boolState[relevant_evt_OF_A1_lost] && ( !boolState[S_OF_A1_lost])) || (  boolState[relevant_evt_OF_B2_lost] && ( !boolState[S_OF_B2_lost])) )
	{
		boolState[relevant_evt_OF_K1]  =  true;
	}

	if (( !boolState[required_OF_A2_lost]) && ( !boolState[required_OF_B1_lost]) )
	{
		boolState[required_OF_K2]  =  false;
	}

	if ((boolState[relevant_evt_OF_A2_lost] && ( !boolState[S_OF_A2_lost])) || (  boolState[relevant_evt_OF_B1_lost] && ( !boolState[S_OF_B1_lost])) )
	{
		boolState[relevant_evt_OF_K2]  =  true;
	}

	if ( !boolState[required_OF_OR_1] )
	{
		boolState[required_OF_K3]  =  false;
	}

	if (boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1]) )
	{
		boolState[relevant_evt_OF_K3]  =  true;
	}

	if (( !boolState[required_OF_A_lost]) && ( !boolState[required_OF_B_lost]) )
	{
		boolState[required_OF_OR_1]  =  false;
	}

	if ((boolState[relevant_evt_OF_A_lost] && ( !boolState[S_OF_A_lost])) || (  boolState[relevant_evt_OF_B_lost] && ( !boolState[S_OF_B_lost])) )
	{
		boolState[relevant_evt_OF_OR_1]  =  true;
	}

	if ( !boolState[S_OF_SF_1] )
	{
		boolState[relevant_evt_OF_OR_2]  =  true;
	}

	if (( !boolState[required_OF_OR_1]) || ( !boolState[S_OF_OR_2]) )
	{
		boolState[required_OF_SF_1]  =  false;
	}

	if (boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1]) )
	{
		boolState[relevant_evt_OF_SF_1]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

}


void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_A1]  =  boolState[S_OF_A1]  ;



	boolState[already_S_OF_A1_lost]  =  boolState[S_OF_A1_lost]  ;



	boolState[already_S_OF_A2]  =  boolState[S_OF_A2]  ;



	boolState[already_S_OF_A2_lost]  =  boolState[S_OF_A2_lost]  ;



	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_A_lost]  =  boolState[S_OF_A_lost]  ;



	boolState[already_S_OF_B1]  =  boolState[S_OF_B1]  ;



	boolState[already_S_OF_B1_lost]  =  boolState[S_OF_B1_lost]  ;



	boolState[already_S_OF_B2]  =  boolState[S_OF_B2]  ;



	boolState[already_S_OF_B2_lost]  =  boolState[S_OF_B2_lost]  ;



	boolState[already_S_OF_B_lost]  =  boolState[S_OF_B_lost]  ;



	boolState[already_S_OF_K1]  =  boolState[S_OF_K1]  ;



	boolState[already_S_OF_K2]  =  boolState[S_OF_K2]  ;



	boolState[already_S_OF_K3]  =  boolState[S_OF_K3]  ;



	boolState[already_S_OF_OR_1]  =  boolState[S_OF_OR_1]  ;



	boolState[already_S_OF_OR_2]  =  boolState[S_OF_OR_2]  ;



	boolState[already_S_OF_SF_1]  =  boolState[S_OF_SF_1]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;

}

void
storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::runInteractions() {
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
         
        // ------------------- Handling of FailureState element --------------------------------
    
	boolFailureState[exp0] = ( boolState[S_OF_UE_1] );
        cout << endl;
    }void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_15_Trim_Article_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    