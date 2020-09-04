#include <iostream>
#include "FigaroModelBDMP_05_Trim_Max_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_AND_1 = true;
	boolState[already_S_OF_AND_1] = false;
	REINITIALISATION_OF_S_OF_AND_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_1 = false;
	REINITIALISATION_OF_required_OF_CB_dies = true;
	boolState[already_S_OF_CB_dies] = false;
	REINITIALISATION_OF_S_OF_CB_dies = false;
	REINITIALISATION_OF_relevant_evt_OF_CB_dies = false;
	boolState[failF_OF_CB_dies] = false;
	REINITIALISATION_OF_required_OF_CB_dw_1 = true;
	boolState[already_S_OF_CB_dw_1] = false;
	REINITIALISATION_OF_S_OF_CB_dw_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_CB_dw_1 = false;
	boolState[failF_OF_CB_dw_1] = false;
	REINITIALISATION_OF_required_OF_CB_dw_2 = true;
	boolState[already_S_OF_CB_dw_2] = false;
	REINITIALISATION_OF_S_OF_CB_dw_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_CB_dw_2 = false;
	boolState[failF_OF_CB_dw_2] = false;
	REINITIALISATION_OF_required_OF_CB_up_1 = true;
	boolState[already_S_OF_CB_up_1] = false;
	REINITIALISATION_OF_S_OF_CB_up_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_CB_up_1 = false;
	boolState[failF_OF_CB_up_1] = false;
	REINITIALISATION_OF_required_OF_CB_up_2 = true;
	boolState[already_S_OF_CB_up_2] = false;
	REINITIALISATION_OF_S_OF_CB_up_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_CB_up_2 = false;
	boolState[failF_OF_CB_up_2] = false;
	REINITIALISATION_OF_required_OF_GRID = true;
	boolState[already_S_OF_GRID] = false;
	REINITIALISATION_OF_S_OF_GRID = false;
	REINITIALISATION_OF_relevant_evt_OF_GRID = false;
	boolState[failF_OF_GRID] = false;
	REINITIALISATION_OF_required_OF_LossOfAllBackups = true;
	boolState[already_S_OF_LossOfAllBackups] = false;
	REINITIALISATION_OF_S_OF_LossOfAllBackups = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfAllBackups = false;
	REINITIALISATION_OF_required_OF_LossOfDieselLine = true;
	boolState[already_S_OF_LossOfDieselLine] = false;
	REINITIALISATION_OF_S_OF_LossOfDieselLine = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfDieselLine = false;
	REINITIALISATION_OF_required_OF_LossOfLine2 = true;
	boolState[already_S_OF_LossOfLine2] = false;
	REINITIALISATION_OF_S_OF_LossOfLine2 = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfLine2 = false;
	REINITIALISATION_OF_required_OF_LossOfLine_1 = true;
	boolState[already_S_OF_LossOfLine_1] = false;
	REINITIALISATION_OF_S_OF_LossOfLine_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfLine_1 = false;
	REINITIALISATION_OF_required_OF_Transfo1 = true;
	boolState[already_S_OF_Transfo1] = false;
	REINITIALISATION_OF_S_OF_Transfo1 = false;
	REINITIALISATION_OF_relevant_evt_OF_Transfo1 = false;
	boolState[failF_OF_Transfo1] = false;
	REINITIALISATION_OF_required_OF_Transfo2 = true;
	boolState[already_S_OF_Transfo2] = false;
	REINITIALISATION_OF_S_OF_Transfo2 = false;
	REINITIALISATION_OF_relevant_evt_OF_Transfo2 = false;
	boolState[failF_OF_Transfo2] = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;
	REINITIALISATION_OF_required_OF_dies_generator = true;
	boolState[already_S_OF_dies_generator] = false;
	REINITIALISATION_OF_S_OF_dies_generator = false;
	REINITIALISATION_OF_relevant_evt_OF_dies_generator = false;
	boolState[failF_OF_dies_generator] = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_CB_dies = false;
	FIRE_xx11_OF_CB_dies = false;
	FIRE_xx10_OF_CB_dw_1 = false;
	FIRE_xx11_OF_CB_dw_1 = false;
	FIRE_xx10_OF_CB_dw_2 = false;
	FIRE_xx11_OF_CB_dw_2 = false;
	FIRE_xx10_OF_CB_up_1 = false;
	FIRE_xx11_OF_CB_up_1 = false;
	FIRE_xx10_OF_CB_up_2 = false;
	FIRE_xx11_OF_CB_up_2 = false;
	FIRE_xx10_OF_GRID = false;
	FIRE_xx11_OF_GRID = false;
	FIRE_xx10_OF_Transfo1 = false;
	FIRE_xx11_OF_Transfo1 = false;
	FIRE_xx10_OF_Transfo2 = false;
	FIRE_xx11_OF_Transfo2 = false;
	FIRE_xx10_OF_dies_generator = false;
	FIRE_xx11_OF_dies_generator = false;

}

void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_AND_1] | Value : " << boolState[required_OF_AND_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_1] | Value : " << boolState[already_S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[S_OF_AND_1] | Value : " << boolState[S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_1] | Value : " << boolState[relevant_evt_OF_AND_1] << endl;
	cout << "Attribute :  boolState[required_OF_CB_dies] | Value : " << boolState[required_OF_CB_dies] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB_dies] | Value : " << boolState[already_S_OF_CB_dies] << endl;
	cout << "Attribute :  boolState[S_OF_CB_dies] | Value : " << boolState[S_OF_CB_dies] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB_dies] | Value : " << boolState[relevant_evt_OF_CB_dies] << endl;
	cout << "Attribute :  boolState[failF_OF_CB_dies] | Value : " << boolState[failF_OF_CB_dies] << endl;
	cout << "Attribute :  boolState[required_OF_CB_dw_1] | Value : " << boolState[required_OF_CB_dw_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB_dw_1] | Value : " << boolState[already_S_OF_CB_dw_1] << endl;
	cout << "Attribute :  boolState[S_OF_CB_dw_1] | Value : " << boolState[S_OF_CB_dw_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB_dw_1] | Value : " << boolState[relevant_evt_OF_CB_dw_1] << endl;
	cout << "Attribute :  boolState[failF_OF_CB_dw_1] | Value : " << boolState[failF_OF_CB_dw_1] << endl;
	cout << "Attribute :  boolState[required_OF_CB_dw_2] | Value : " << boolState[required_OF_CB_dw_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB_dw_2] | Value : " << boolState[already_S_OF_CB_dw_2] << endl;
	cout << "Attribute :  boolState[S_OF_CB_dw_2] | Value : " << boolState[S_OF_CB_dw_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB_dw_2] | Value : " << boolState[relevant_evt_OF_CB_dw_2] << endl;
	cout << "Attribute :  boolState[failF_OF_CB_dw_2] | Value : " << boolState[failF_OF_CB_dw_2] << endl;
	cout << "Attribute :  boolState[required_OF_CB_up_1] | Value : " << boolState[required_OF_CB_up_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB_up_1] | Value : " << boolState[already_S_OF_CB_up_1] << endl;
	cout << "Attribute :  boolState[S_OF_CB_up_1] | Value : " << boolState[S_OF_CB_up_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB_up_1] | Value : " << boolState[relevant_evt_OF_CB_up_1] << endl;
	cout << "Attribute :  boolState[failF_OF_CB_up_1] | Value : " << boolState[failF_OF_CB_up_1] << endl;
	cout << "Attribute :  boolState[required_OF_CB_up_2] | Value : " << boolState[required_OF_CB_up_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB_up_2] | Value : " << boolState[already_S_OF_CB_up_2] << endl;
	cout << "Attribute :  boolState[S_OF_CB_up_2] | Value : " << boolState[S_OF_CB_up_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB_up_2] | Value : " << boolState[relevant_evt_OF_CB_up_2] << endl;
	cout << "Attribute :  boolState[failF_OF_CB_up_2] | Value : " << boolState[failF_OF_CB_up_2] << endl;
	cout << "Attribute :  boolState[required_OF_GRID] | Value : " << boolState[required_OF_GRID] << endl;
	cout << "Attribute :  boolState[already_S_OF_GRID] | Value : " << boolState[already_S_OF_GRID] << endl;
	cout << "Attribute :  boolState[S_OF_GRID] | Value : " << boolState[S_OF_GRID] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_GRID] | Value : " << boolState[relevant_evt_OF_GRID] << endl;
	cout << "Attribute :  boolState[failF_OF_GRID] | Value : " << boolState[failF_OF_GRID] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfAllBackups] | Value : " << boolState[required_OF_LossOfAllBackups] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfAllBackups] | Value : " << boolState[already_S_OF_LossOfAllBackups] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfAllBackups] | Value : " << boolState[S_OF_LossOfAllBackups] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfAllBackups] | Value : " << boolState[relevant_evt_OF_LossOfAllBackups] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfDieselLine] | Value : " << boolState[required_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfDieselLine] | Value : " << boolState[already_S_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfDieselLine] | Value : " << boolState[S_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfDieselLine] | Value : " << boolState[relevant_evt_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfLine2] | Value : " << boolState[required_OF_LossOfLine2] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfLine2] | Value : " << boolState[already_S_OF_LossOfLine2] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfLine2] | Value : " << boolState[S_OF_LossOfLine2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfLine2] | Value : " << boolState[relevant_evt_OF_LossOfLine2] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfLine_1] | Value : " << boolState[required_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfLine_1] | Value : " << boolState[already_S_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfLine_1] | Value : " << boolState[S_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfLine_1] | Value : " << boolState[relevant_evt_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[required_OF_Transfo1] | Value : " << boolState[required_OF_Transfo1] << endl;
	cout << "Attribute :  boolState[already_S_OF_Transfo1] | Value : " << boolState[already_S_OF_Transfo1] << endl;
	cout << "Attribute :  boolState[S_OF_Transfo1] | Value : " << boolState[S_OF_Transfo1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Transfo1] | Value : " << boolState[relevant_evt_OF_Transfo1] << endl;
	cout << "Attribute :  boolState[failF_OF_Transfo1] | Value : " << boolState[failF_OF_Transfo1] << endl;
	cout << "Attribute :  boolState[required_OF_Transfo2] | Value : " << boolState[required_OF_Transfo2] << endl;
	cout << "Attribute :  boolState[already_S_OF_Transfo2] | Value : " << boolState[already_S_OF_Transfo2] << endl;
	cout << "Attribute :  boolState[S_OF_Transfo2] | Value : " << boolState[S_OF_Transfo2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Transfo2] | Value : " << boolState[relevant_evt_OF_Transfo2] << endl;
	cout << "Attribute :  boolState[failF_OF_Transfo2] | Value : " << boolState[failF_OF_Transfo2] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
	cout << "Attribute :  boolState[required_OF_dies_generator] | Value : " << boolState[required_OF_dies_generator] << endl;
	cout << "Attribute :  boolState[already_S_OF_dies_generator] | Value : " << boolState[already_S_OF_dies_generator] << endl;
	cout << "Attribute :  boolState[S_OF_dies_generator] | Value : " << boolState[S_OF_dies_generator] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_dies_generator] | Value : " << boolState[relevant_evt_OF_dies_generator] << endl;
	cout << "Attribute :  boolState[failF_OF_dies_generator] | Value : " << boolState[failF_OF_dies_generator] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::figaromodelhasinstransitions()
{
	return false;
}
void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_CB_dies] = REINITIALISATION_OF_required_OF_CB_dies;
	boolState[S_OF_CB_dies] = REINITIALISATION_OF_S_OF_CB_dies;
	boolState[relevant_evt_OF_CB_dies] = REINITIALISATION_OF_relevant_evt_OF_CB_dies;
	boolState[required_OF_CB_dw_1] = REINITIALISATION_OF_required_OF_CB_dw_1;
	boolState[S_OF_CB_dw_1] = REINITIALISATION_OF_S_OF_CB_dw_1;
	boolState[relevant_evt_OF_CB_dw_1] = REINITIALISATION_OF_relevant_evt_OF_CB_dw_1;
	boolState[required_OF_CB_dw_2] = REINITIALISATION_OF_required_OF_CB_dw_2;
	boolState[S_OF_CB_dw_2] = REINITIALISATION_OF_S_OF_CB_dw_2;
	boolState[relevant_evt_OF_CB_dw_2] = REINITIALISATION_OF_relevant_evt_OF_CB_dw_2;
	boolState[required_OF_CB_up_1] = REINITIALISATION_OF_required_OF_CB_up_1;
	boolState[S_OF_CB_up_1] = REINITIALISATION_OF_S_OF_CB_up_1;
	boolState[relevant_evt_OF_CB_up_1] = REINITIALISATION_OF_relevant_evt_OF_CB_up_1;
	boolState[required_OF_CB_up_2] = REINITIALISATION_OF_required_OF_CB_up_2;
	boolState[S_OF_CB_up_2] = REINITIALISATION_OF_S_OF_CB_up_2;
	boolState[relevant_evt_OF_CB_up_2] = REINITIALISATION_OF_relevant_evt_OF_CB_up_2;
	boolState[required_OF_GRID] = REINITIALISATION_OF_required_OF_GRID;
	boolState[S_OF_GRID] = REINITIALISATION_OF_S_OF_GRID;
	boolState[relevant_evt_OF_GRID] = REINITIALISATION_OF_relevant_evt_OF_GRID;
	boolState[required_OF_LossOfAllBackups] = REINITIALISATION_OF_required_OF_LossOfAllBackups;
	boolState[S_OF_LossOfAllBackups] = REINITIALISATION_OF_S_OF_LossOfAllBackups;
	boolState[relevant_evt_OF_LossOfAllBackups] = REINITIALISATION_OF_relevant_evt_OF_LossOfAllBackups;
	boolState[required_OF_LossOfDieselLine] = REINITIALISATION_OF_required_OF_LossOfDieselLine;
	boolState[S_OF_LossOfDieselLine] = REINITIALISATION_OF_S_OF_LossOfDieselLine;
	boolState[relevant_evt_OF_LossOfDieselLine] = REINITIALISATION_OF_relevant_evt_OF_LossOfDieselLine;
	boolState[required_OF_LossOfLine2] = REINITIALISATION_OF_required_OF_LossOfLine2;
	boolState[S_OF_LossOfLine2] = REINITIALISATION_OF_S_OF_LossOfLine2;
	boolState[relevant_evt_OF_LossOfLine2] = REINITIALISATION_OF_relevant_evt_OF_LossOfLine2;
	boolState[required_OF_LossOfLine_1] = REINITIALISATION_OF_required_OF_LossOfLine_1;
	boolState[S_OF_LossOfLine_1] = REINITIALISATION_OF_S_OF_LossOfLine_1;
	boolState[relevant_evt_OF_LossOfLine_1] = REINITIALISATION_OF_relevant_evt_OF_LossOfLine_1;
	boolState[required_OF_Transfo1] = REINITIALISATION_OF_required_OF_Transfo1;
	boolState[S_OF_Transfo1] = REINITIALISATION_OF_S_OF_Transfo1;
	boolState[relevant_evt_OF_Transfo1] = REINITIALISATION_OF_relevant_evt_OF_Transfo1;
	boolState[required_OF_Transfo2] = REINITIALISATION_OF_required_OF_Transfo2;
	boolState[S_OF_Transfo2] = REINITIALISATION_OF_S_OF_Transfo2;
	boolState[relevant_evt_OF_Transfo2] = REINITIALISATION_OF_relevant_evt_OF_Transfo2;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
	boolState[required_OF_dies_generator] = REINITIALISATION_OF_required_OF_dies_generator;
	boolState[S_OF_dies_generator] = REINITIALISATION_OF_S_OF_dies_generator;
	boolState[relevant_evt_OF_dies_generator] = REINITIALISATION_OF_relevant_evt_OF_dies_generator;
}

void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_CB_dies = true;
	}

	if (numFire == 1)
	{
		FIRE_xx11_OF_CB_dies = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_CB_dw_1 = true;
	}

	if (numFire == 3)
	{
		FIRE_xx11_OF_CB_dw_1 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_CB_dw_2 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx11_OF_CB_dw_2 = true;
	}

	if (numFire == 6)
	{
		FIRE_xx10_OF_CB_up_1 = true;
	}

	if (numFire == 7)
	{
		FIRE_xx11_OF_CB_up_1 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx10_OF_CB_up_2 = true;
	}

	if (numFire == 9)
	{
		FIRE_xx11_OF_CB_up_2 = true;
	}

	if (numFire == 10)
	{
		FIRE_xx10_OF_GRID = true;
	}

	if (numFire == 11)
	{
		FIRE_xx11_OF_GRID = true;
	}

	if (numFire == 12)
	{
		FIRE_xx10_OF_Transfo1 = true;
	}

	if (numFire == 13)
	{
		FIRE_xx11_OF_Transfo1 = true;
	}

	if (numFire == 14)
	{
		FIRE_xx10_OF_Transfo2 = true;
	}

	if (numFire == 15)
	{
		FIRE_xx11_OF_Transfo2 = true;
	}

	if (numFire == 16)
	{
		FIRE_xx10_OF_dies_generator = true;
	}

	if (numFire == 17)
	{
		FIRE_xx11_OF_dies_generator = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_CB_dies
	if ((boolState[failF_OF_CB_dies] == false) && (boolState[required_OF_CB_dies] &&  boolState[relevant_evt_OF_CB_dies])) 
	{
		 
		if (FIRE_xx10_OF_CB_dies)
		{
			boolState[failF_OF_CB_dies]  =  true;
			FIRE_xx10_OF_CB_dies = false;
		}
	}

	// Occurrence xx11_OF_CB_dies
	if (boolState[failF_OF_CB_dies] == true) 
	{
		 
		if (FIRE_xx11_OF_CB_dies)
		{
			boolState[failF_OF_CB_dies]  =  false;
			FIRE_xx11_OF_CB_dies = false;
		}
	}

	// Occurrence xx10_OF_CB_dw_1
	if ((boolState[failF_OF_CB_dw_1] == false) && (boolState[required_OF_CB_dw_1] &&  boolState[relevant_evt_OF_CB_dw_1])) 
	{
		 
		if (FIRE_xx10_OF_CB_dw_1)
		{
			boolState[failF_OF_CB_dw_1]  =  true;
			FIRE_xx10_OF_CB_dw_1 = false;
		}
	}

	// Occurrence xx11_OF_CB_dw_1
	if (boolState[failF_OF_CB_dw_1] == true) 
	{
		 
		if (FIRE_xx11_OF_CB_dw_1)
		{
			boolState[failF_OF_CB_dw_1]  =  false;
			FIRE_xx11_OF_CB_dw_1 = false;
		}
	}

	// Occurrence xx10_OF_CB_dw_2
	if ((boolState[failF_OF_CB_dw_2] == false) && (boolState[required_OF_CB_dw_2] &&  boolState[relevant_evt_OF_CB_dw_2])) 
	{
		 
		if (FIRE_xx10_OF_CB_dw_2)
		{
			boolState[failF_OF_CB_dw_2]  =  true;
			FIRE_xx10_OF_CB_dw_2 = false;
		}
	}

	// Occurrence xx11_OF_CB_dw_2
	if (boolState[failF_OF_CB_dw_2] == true) 
	{
		 
		if (FIRE_xx11_OF_CB_dw_2)
		{
			boolState[failF_OF_CB_dw_2]  =  false;
			FIRE_xx11_OF_CB_dw_2 = false;
		}
	}

	// Occurrence xx10_OF_CB_up_1
	if ((boolState[failF_OF_CB_up_1] == false) && (boolState[required_OF_CB_up_1] &&  boolState[relevant_evt_OF_CB_up_1])) 
	{
		 
		if (FIRE_xx10_OF_CB_up_1)
		{
			boolState[failF_OF_CB_up_1]  =  true;
			FIRE_xx10_OF_CB_up_1 = false;
		}
	}

	// Occurrence xx11_OF_CB_up_1
	if (boolState[failF_OF_CB_up_1] == true) 
	{
		 
		if (FIRE_xx11_OF_CB_up_1)
		{
			boolState[failF_OF_CB_up_1]  =  false;
			FIRE_xx11_OF_CB_up_1 = false;
		}
	}

	// Occurrence xx10_OF_CB_up_2
	if ((boolState[failF_OF_CB_up_2] == false) && (boolState[required_OF_CB_up_2] &&  boolState[relevant_evt_OF_CB_up_2])) 
	{
		 
		if (FIRE_xx10_OF_CB_up_2)
		{
			boolState[failF_OF_CB_up_2]  =  true;
			FIRE_xx10_OF_CB_up_2 = false;
		}
	}

	// Occurrence xx11_OF_CB_up_2
	if (boolState[failF_OF_CB_up_2] == true) 
	{
		 
		if (FIRE_xx11_OF_CB_up_2)
		{
			boolState[failF_OF_CB_up_2]  =  false;
			FIRE_xx11_OF_CB_up_2 = false;
		}
	}

	// Occurrence xx10_OF_GRID
	if ((boolState[failF_OF_GRID] == false) && (boolState[required_OF_GRID] &&  boolState[relevant_evt_OF_GRID])) 
	{
		 
		if (FIRE_xx10_OF_GRID)
		{
			boolState[failF_OF_GRID]  =  true;
			FIRE_xx10_OF_GRID = false;
		}
	}

	// Occurrence xx11_OF_GRID
	if (boolState[failF_OF_GRID] == true) 
	{
		 
		if (FIRE_xx11_OF_GRID)
		{
			boolState[failF_OF_GRID]  =  false;
			FIRE_xx11_OF_GRID = false;
		}
	}

	// Occurrence xx10_OF_Transfo1
	if ((boolState[failF_OF_Transfo1] == false) && (boolState[required_OF_Transfo1] &&  boolState[relevant_evt_OF_Transfo1])) 
	{
		 
		if (FIRE_xx10_OF_Transfo1)
		{
			boolState[failF_OF_Transfo1]  =  true;
			FIRE_xx10_OF_Transfo1 = false;
		}
	}

	// Occurrence xx11_OF_Transfo1
	if (boolState[failF_OF_Transfo1] == true) 
	{
		 
		if (FIRE_xx11_OF_Transfo1)
		{
			boolState[failF_OF_Transfo1]  =  false;
			FIRE_xx11_OF_Transfo1 = false;
		}
	}

	// Occurrence xx10_OF_Transfo2
	if ((boolState[failF_OF_Transfo2] == false) && (boolState[required_OF_Transfo2] &&  boolState[relevant_evt_OF_Transfo2])) 
	{
		 
		if (FIRE_xx10_OF_Transfo2)
		{
			boolState[failF_OF_Transfo2]  =  true;
			FIRE_xx10_OF_Transfo2 = false;
		}
	}

	// Occurrence xx11_OF_Transfo2
	if (boolState[failF_OF_Transfo2] == true) 
	{
		 
		if (FIRE_xx11_OF_Transfo2)
		{
			boolState[failF_OF_Transfo2]  =  false;
			FIRE_xx11_OF_Transfo2 = false;
		}
	}

	// Occurrence xx10_OF_dies_generator
	if ((boolState[failF_OF_dies_generator] == false) && (boolState[required_OF_dies_generator] && boolState[relevant_evt_OF_dies_generator])) 
	{
		 
		if (FIRE_xx10_OF_dies_generator)
		{
			boolState[failF_OF_dies_generator]  =  true;
			FIRE_xx10_OF_dies_generator = false;
		}
	}

	// Occurrence xx11_OF_dies_generator
	if (boolState[failF_OF_dies_generator] == true) 
	{
		 
		if (FIRE_xx11_OF_dies_generator)
		{
			boolState[failF_OF_dies_generator]  =  false;
			FIRE_xx11_OF_dies_generator = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::showFireableOccurrences()
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
     
	if ((boolState[failF_OF_CB_dies] == false) && (boolState[required_OF_CB_dies] && boolState[relevant_evt_OF_CB_dies]))
	{
		cout << "0 : xx10_OF_CB_dies : FAULT failF  LABEL \"failure in operation CB_dies\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB_dies]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_CB_dies] == true)
	{
		cout << "1 : xx11_OF_CB_dies : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_CB_dies]  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_CB_dw_1] == false) && (boolState[required_OF_CB_dw_1] && boolState[relevant_evt_OF_CB_dw_1]))
	{
		cout << "2 : xx10_OF_CB_dw_1 : FAULT failF  LABEL \"failure in operation CB_dw_1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB_dw_1]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_CB_dw_1] == true)
	{
		cout << "3 : xx11_OF_CB_dw_1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_CB_dw_1]  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_CB_dw_2] == false) && (boolState[required_OF_CB_dw_2] && boolState[relevant_evt_OF_CB_dw_2]))
	{
		cout << "4 : xx10_OF_CB_dw_2 : FAULT failF  LABEL \"failure in operation CB_dw_2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB_dw_2]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_CB_dw_2] == true)
	{
		cout << "5 : xx11_OF_CB_dw_2 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_CB_dw_2]  =  FALSE" << endl;
		list.push_back(make_tuple(5, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_CB_up_1] == false) && (boolState[required_OF_CB_up_1] && boolState[relevant_evt_OF_CB_up_1]))
	{
		cout << "6 : xx10_OF_CB_up_1 : FAULT failF  LABEL \"failure in operation CB_up_1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB_up_1]  =  TRUE" << endl;
		list.push_back(make_tuple(6, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_CB_up_1] == true)
	{
		cout << "7 : xx11_OF_CB_up_1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_CB_up_1]  =  FALSE" << endl;
		list.push_back(make_tuple(7, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_CB_up_2] == false) && (boolState[required_OF_CB_up_2] && boolState[relevant_evt_OF_CB_up_2]))
	{
		cout << "8 : xx10_OF_CB_up_2 : FAULT failF  LABEL \"failure in operation CB_up_2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB_up_2]  =  TRUE" << endl;
		list.push_back(make_tuple(8, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_CB_up_2] == true)
	{
		cout << "9 : xx11_OF_CB_up_2 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_CB_up_2]  =  FALSE" << endl;
		list.push_back(make_tuple(9, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_GRID] == false) && (boolState[required_OF_GRID] && boolState[relevant_evt_OF_GRID]))
	{
		cout << "10 : xx10_OF_GRID : FAULT failF  LABEL \"failure in operation GRID\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_GRID]  =  TRUE" << endl;
		list.push_back(make_tuple(10, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_GRID] == true)
	{
		cout << "11 : xx11_OF_GRID : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_GRID]  =  FALSE" << endl;
		list.push_back(make_tuple(11, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_Transfo1] == false) && (boolState[required_OF_Transfo1] && boolState[relevant_evt_OF_Transfo1]))
	{
		cout << "12 : xx10_OF_Transfo1 : FAULT failF  LABEL \"failure in operation Transfo1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Transfo1]  =  TRUE" << endl;
		list.push_back(make_tuple(12, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_Transfo1] == true)
	{
		cout << "13 : xx11_OF_Transfo1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_Transfo1]  =  FALSE" << endl;
		list.push_back(make_tuple(13, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_Transfo2] == false) && (boolState[required_OF_Transfo2] && boolState[relevant_evt_OF_Transfo2]))
	{
		cout << "14 : xx10_OF_Transfo2 : FAULT failF  LABEL \"failure in operation Transfo2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Transfo2]  =  TRUE" << endl;
		list.push_back(make_tuple(14, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_Transfo2] == true)
	{
		cout << "15 : xx11_OF_Transfo2 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_Transfo2]  =  FALSE" << endl;
		list.push_back(make_tuple(15, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_dies_generator] == false) && (boolState[required_OF_dies_generator] && boolState[relevant_evt_OF_dies_generator]))
	{
		cout << "16 : xx10_OF_dies_generator : FAULT failF  LABEL \"failure in operation dies_generator\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_dies_generator]  =  TRUE" << endl;
		list.push_back(make_tuple(16, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_dies_generator] == true)
	{
		cout << "17 : xx11_OF_dies_generator : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_dies_generator]  =  FALSE" << endl;
		list.push_back(make_tuple(17, 0.1, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_CB_dies] == true )
	{
		boolState[S_OF_CB_dies]  =  true;
	}

	if (boolState[failF_OF_CB_dw_1] == true )
	{
		boolState[S_OF_CB_dw_1]  =  true;
	}

	if (boolState[failF_OF_CB_dw_2] == true )
	{
		boolState[S_OF_CB_dw_2]  =  true;
	}

	if (boolState[failF_OF_CB_up_1] == true )
	{
		boolState[S_OF_CB_up_1]  =  true;
	}

	if (boolState[failF_OF_CB_up_2] == true )
	{
		boolState[S_OF_CB_up_2]  =  true;
	}

	if (boolState[failF_OF_GRID] == true )
	{
		boolState[S_OF_GRID]  =  true;
	}

	if (boolState[failF_OF_Transfo1] == true )
	{
		boolState[S_OF_Transfo1]  =  true;
	}

	if (boolState[failF_OF_Transfo2] == true )
	{
		boolState[S_OF_Transfo2]  =  true;
	}

	if (boolState[failF_OF_dies_generator] == true )
	{
		boolState[S_OF_dies_generator]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_LossOfAllBackups] && boolState[S_OF_LossOfLine_1] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (boolState[S_OF_LossOfDieselLine] && boolState[S_OF_LossOfLine2] )
	{
		boolState[S_OF_LossOfAllBackups]  =  true;
	}

	if (boolState[S_OF_CB_dies] || boolState[S_OF_dies_generator] )
	{
		boolState[S_OF_LossOfDieselLine]  =  true;
	}

	if (((boolState[S_OF_CB_up_2] || boolState[S_OF_CB_dw_2]) || boolState[S_OF_Transfo2]) || boolState[S_OF_GRID] )
	{
		boolState[S_OF_LossOfLine2]  =  true;
	}

	if (((boolState[S_OF_CB_up_1] || boolState[S_OF_GRID]) || boolState[S_OF_Transfo1]) || boolState[S_OF_CB_dw_1] )
	{
		boolState[S_OF_LossOfLine_1]  =  true;
	}

	if (boolState[S_OF_AND_1] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_AND_1]  =  true;
	}

	if ( !boolState[required_OF_LossOfDieselLine] )
	{
		boolState[required_OF_CB_dies]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_CB_dies]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine_1] )
	{
		boolState[required_OF_CB_dw_1]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1]) )
	{
		boolState[relevant_evt_OF_CB_dw_1]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine2] )
	{
		boolState[required_OF_CB_dw_2]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2]) )
	{
		boolState[relevant_evt_OF_CB_dw_2]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine_1] )
	{
		boolState[required_OF_CB_up_1]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1]) )
	{
		boolState[relevant_evt_OF_CB_up_1]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine2] )
	{
		boolState[required_OF_CB_up_2]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2]) )
	{
		boolState[relevant_evt_OF_CB_up_2]  =  true;
	}

	if (( !boolState[required_OF_LossOfLine_1]) && ( !boolState[required_OF_LossOfLine2]) )
	{
		boolState[required_OF_GRID]  =  false;
	}

	if ((boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1])) || (boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2])) )
	{
		boolState[relevant_evt_OF_GRID]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_LossOfLine_1]) )
	{
		boolState[required_OF_LossOfAllBackups]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_LossOfAllBackups]  =  true;
	}

	if (( !boolState[required_OF_LossOfAllBackups]) || ( !boolState[S_OF_LossOfLine2]) )
	{
		boolState[required_OF_LossOfDieselLine]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfAllBackups] && ( !boolState[S_OF_LossOfAllBackups]) )
	{
		boolState[relevant_evt_OF_LossOfDieselLine]  =  true;
	}

	if ( !boolState[required_OF_LossOfAllBackups] )
	{
		boolState[required_OF_LossOfLine2]  =  false;
	}

	if ((boolState[relevant_evt_OF_LossOfAllBackups] && ( !boolState[S_OF_LossOfAllBackups])) || (boolState[relevant_evt_OF_LossOfDieselLine] && (   !boolState[S_OF_LossOfDieselLine])) )
	{
		boolState[relevant_evt_OF_LossOfLine2]  =  true;
	}

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_LossOfLine_1]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1])) || (  boolState[relevant_evt_OF_LossOfAllBackups] && ( !boolState[S_OF_LossOfAllBackups])) )
	{
		boolState[relevant_evt_OF_LossOfLine_1]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine_1] )
	{
		boolState[required_OF_Transfo1]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1]) )
	{
		boolState[relevant_evt_OF_Transfo1]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine2] )
	{
		boolState[required_OF_Transfo2]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2]) )
	{
		boolState[relevant_evt_OF_Transfo2]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

	if ( !boolState[required_OF_LossOfDieselLine] )
	{
		boolState[required_OF_dies_generator]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_dies_generator]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_CB_dies]  =  boolState[S_OF_CB_dies]  ;



	boolState[already_S_OF_CB_dw_1]  =  boolState[S_OF_CB_dw_1]  ;



	boolState[already_S_OF_CB_dw_2]  =  boolState[S_OF_CB_dw_2]  ;



	boolState[already_S_OF_CB_up_1]  =  boolState[S_OF_CB_up_1]  ;



	boolState[already_S_OF_CB_up_2]  =  boolState[S_OF_CB_up_2]  ;



	boolState[already_S_OF_GRID]  =  boolState[S_OF_GRID]  ;



	boolState[already_S_OF_LossOfAllBackups]  =  boolState[S_OF_LossOfAllBackups]  ;



	boolState[already_S_OF_LossOfDieselLine]  =  boolState[S_OF_LossOfDieselLine]  ;



	boolState[already_S_OF_LossOfLine2]  =  boolState[S_OF_LossOfLine2]  ;



	boolState[already_S_OF_LossOfLine_1]  =  boolState[S_OF_LossOfLine_1]  ;



	boolState[already_S_OF_Transfo1]  =  boolState[S_OF_Transfo1]  ;



	boolState[already_S_OF_Transfo2]  =  boolState[S_OF_Transfo2]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;



	boolState[already_S_OF_dies_generator]  =  boolState[S_OF_dies_generator]  ;

}

void
storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_05_Trim_Max_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    