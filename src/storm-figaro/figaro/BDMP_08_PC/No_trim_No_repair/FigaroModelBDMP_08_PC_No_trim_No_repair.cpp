#include <iostream>
#include "FigaroModelBDMP_08_PC_No_trim_No_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_AND_1 = true;
	boolState[already_S_OF_AND_1] = false;
	REINITIALISATION_OF_S_OF_AND_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_1 = false;
	REINITIALISATION_OF_required_OF_Cpu = true;
	boolState[already_S_OF_Cpu] = false;
	REINITIALISATION_OF_S_OF_Cpu = false;
	REINITIALISATION_OF_relevant_evt_OF_Cpu = false;
	boolState[failF_OF_Cpu] = false;
	boolState[failS_OF_Cpu] = false;
	REINITIALISATION_OF_required_OF_DisplayUnit = true;
	boolState[already_S_OF_DisplayUnit] = false;
	REINITIALISATION_OF_S_OF_DisplayUnit = false;
	REINITIALISATION_OF_relevant_evt_OF_DisplayUnit = false;
	boolState[failF_OF_DisplayUnit] = false;
	REINITIALISATION_OF_required_OF_Fan = true;
	boolState[already_S_OF_Fan] = false;
	REINITIALISATION_OF_S_OF_Fan = false;
	REINITIALISATION_OF_relevant_evt_OF_Fan = false;
	boolState[failF_OF_Fan] = false;
	REINITIALISATION_OF_required_OF_FloppyDrive = true;
	boolState[already_S_OF_FloppyDrive] = false;
	REINITIALISATION_OF_S_OF_FloppyDrive = false;
	REINITIALISATION_OF_relevant_evt_OF_FloppyDrive = false;
	boolState[failF_OF_FloppyDrive] = false;
	REINITIALISATION_OF_required_OF_FloppyDriveInUse = true;
	boolState[already_S_OF_FloppyDriveInUse] = false;
	REINITIALISATION_OF_S_OF_FloppyDriveInUse = false;
	REINITIALISATION_OF_relevant_evt_OF_FloppyDriveInUse = false;
	boolState[failF_OF_FloppyDriveInUse] = false;
	REINITIALISATION_OF_required_OF_HardDrive = true;
	boolState[already_S_OF_HardDrive] = false;
	REINITIALISATION_OF_S_OF_HardDrive = false;
	REINITIALISATION_OF_relevant_evt_OF_HardDrive = false;
	boolState[failF_OF_HardDrive] = false;
	REINITIALISATION_OF_required_OF_LossOfCooling = true;
	boolState[already_S_OF_LossOfCooling] = false;
	REINITIALISATION_OF_S_OF_LossOfCooling = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfCooling = false;
	REINITIALISATION_OF_required_OF_ManipulationError = true;
	boolState[already_S_OF_ManipulationError] = false;
	REINITIALISATION_OF_S_OF_ManipulationError = false;
	REINITIALISATION_OF_relevant_evt_OF_ManipulationError = false;
	boolState[failI_OF_ManipulationError] = false;
	REINITIALISATION_OF_to_be_fired_OF_ManipulationError = false;
	boolState[already_standby_OF_ManipulationError] = false;
	boolState[already_required_OF_ManipulationError] = false;
	REINITIALISATION_OF_required_OF_Memory = true;
	boolState[already_S_OF_Memory] = false;
	REINITIALISATION_OF_S_OF_Memory = false;
	REINITIALISATION_OF_relevant_evt_OF_Memory = false;
	boolState[failF_OF_Memory] = false;
	REINITIALISATION_OF_required_OF_OR_1 = true;
	boolState[already_S_OF_OR_1] = false;
	REINITIALISATION_OF_S_OF_OR_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_1 = false;
	REINITIALISATION_OF_required_OF_PC_down = true;
	boolState[already_S_OF_PC_down] = false;
	REINITIALISATION_OF_S_OF_PC_down = false;
	REINITIALISATION_OF_relevant_evt_OF_PC_down = false;
	REINITIALISATION_OF_required_OF_PowerSupply = true;
	boolState[already_S_OF_PowerSupply] = false;
	REINITIALISATION_OF_S_OF_PowerSupply = false;
	REINITIALISATION_OF_relevant_evt_OF_PowerSupply = false;
	boolState[failF_OF_PowerSupply] = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;
	REINITIALISATION_OF_required_OF_Windows = true;
	boolState[already_S_OF_Windows] = false;
	REINITIALISATION_OF_S_OF_Windows = false;
	REINITIALISATION_OF_relevant_evt_OF_Windows = false;
	boolState[failF_OF_Windows] = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx17_OF_Cpu = false;
	FIRE_xx18_OF_Cpu = false;
	FIRE_xx10_OF_DisplayUnit = false;
	FIRE_xx10_OF_Fan = false;
	FIRE_xx10_OF_FloppyDrive = false;
	FIRE_xx10_OF_FloppyDriveInUse = false;
	FIRE_xx10_OF_HardDrive = false;
	FIRE_xx23_OF_ManipulationError_INS_7 = false;
	FIRE_xx23_OF_ManipulationError_INS_8 = false;
	FIRE_xx10_OF_Memory = false;
	FIRE_xx10_OF_PowerSupply = false;
	FIRE_xx10_OF_Windows = false;

}

void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_AND_1] | Value : " << boolState[required_OF_AND_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_1] | Value : " << boolState[already_S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[S_OF_AND_1] | Value : " << boolState[S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_1] | Value : " << boolState[relevant_evt_OF_AND_1] << endl;
	cout << "Attribute :  boolState[required_OF_Cpu] | Value : " << boolState[required_OF_Cpu] << endl;
	cout << "Attribute :  boolState[already_S_OF_Cpu] | Value : " << boolState[already_S_OF_Cpu] << endl;
	cout << "Attribute :  boolState[S_OF_Cpu] | Value : " << boolState[S_OF_Cpu] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Cpu] | Value : " << boolState[relevant_evt_OF_Cpu] << endl;
	cout << "Attribute :  boolState[failF_OF_Cpu] | Value : " << boolState[failF_OF_Cpu] << endl;
	cout << "Attribute :  boolState[failS_OF_Cpu] | Value : " << boolState[failS_OF_Cpu] << endl;
	cout << "Attribute :  boolState[required_OF_DisplayUnit] | Value : " << boolState[required_OF_DisplayUnit] << endl;
	cout << "Attribute :  boolState[already_S_OF_DisplayUnit] | Value : " << boolState[already_S_OF_DisplayUnit] << endl;
	cout << "Attribute :  boolState[S_OF_DisplayUnit] | Value : " << boolState[S_OF_DisplayUnit] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_DisplayUnit] | Value : " << boolState[relevant_evt_OF_DisplayUnit] << endl;
	cout << "Attribute :  boolState[failF_OF_DisplayUnit] | Value : " << boolState[failF_OF_DisplayUnit] << endl;
	cout << "Attribute :  boolState[required_OF_Fan] | Value : " << boolState[required_OF_Fan] << endl;
	cout << "Attribute :  boolState[already_S_OF_Fan] | Value : " << boolState[already_S_OF_Fan] << endl;
	cout << "Attribute :  boolState[S_OF_Fan] | Value : " << boolState[S_OF_Fan] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Fan] | Value : " << boolState[relevant_evt_OF_Fan] << endl;
	cout << "Attribute :  boolState[failF_OF_Fan] | Value : " << boolState[failF_OF_Fan] << endl;
	cout << "Attribute :  boolState[required_OF_FloppyDrive] | Value : " << boolState[required_OF_FloppyDrive] << endl;
	cout << "Attribute :  boolState[already_S_OF_FloppyDrive] | Value : " << boolState[already_S_OF_FloppyDrive] << endl;
	cout << "Attribute :  boolState[S_OF_FloppyDrive] | Value : " << boolState[S_OF_FloppyDrive] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_FloppyDrive] | Value : " << boolState[relevant_evt_OF_FloppyDrive] << endl;
	cout << "Attribute :  boolState[failF_OF_FloppyDrive] | Value : " << boolState[failF_OF_FloppyDrive] << endl;
	cout << "Attribute :  boolState[required_OF_FloppyDriveInUse] | Value : " << boolState[required_OF_FloppyDriveInUse] << endl;
	cout << "Attribute :  boolState[already_S_OF_FloppyDriveInUse] | Value : " << boolState[already_S_OF_FloppyDriveInUse] << endl;
	cout << "Attribute :  boolState[S_OF_FloppyDriveInUse] | Value : " << boolState[S_OF_FloppyDriveInUse] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_FloppyDriveInUse] | Value : " << boolState[relevant_evt_OF_FloppyDriveInUse] << endl;
	cout << "Attribute :  boolState[failF_OF_FloppyDriveInUse] | Value : " << boolState[failF_OF_FloppyDriveInUse] << endl;
	cout << "Attribute :  boolState[required_OF_HardDrive] | Value : " << boolState[required_OF_HardDrive] << endl;
	cout << "Attribute :  boolState[already_S_OF_HardDrive] | Value : " << boolState[already_S_OF_HardDrive] << endl;
	cout << "Attribute :  boolState[S_OF_HardDrive] | Value : " << boolState[S_OF_HardDrive] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_HardDrive] | Value : " << boolState[relevant_evt_OF_HardDrive] << endl;
	cout << "Attribute :  boolState[failF_OF_HardDrive] | Value : " << boolState[failF_OF_HardDrive] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfCooling] | Value : " << boolState[required_OF_LossOfCooling] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfCooling] | Value : " << boolState[already_S_OF_LossOfCooling] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfCooling] | Value : " << boolState[S_OF_LossOfCooling] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfCooling] | Value : " << boolState[relevant_evt_OF_LossOfCooling] << endl;
	cout << "Attribute :  boolState[required_OF_ManipulationError] | Value : " << boolState[required_OF_ManipulationError] << endl;
	cout << "Attribute :  boolState[already_S_OF_ManipulationError] | Value : " << boolState[already_S_OF_ManipulationError] << endl;
	cout << "Attribute :  boolState[S_OF_ManipulationError] | Value : " << boolState[S_OF_ManipulationError] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_ManipulationError] | Value : " << boolState[relevant_evt_OF_ManipulationError] << endl;
	cout << "Attribute :  boolState[failI_OF_ManipulationError] | Value : " << boolState[failI_OF_ManipulationError] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_ManipulationError] | Value : " << boolState[to_be_fired_OF_ManipulationError] << endl;
	cout << "Attribute :  boolState[already_standby_OF_ManipulationError] | Value : " << boolState[already_standby_OF_ManipulationError] << endl;
	cout << "Attribute :  boolState[already_required_OF_ManipulationError] | Value : " << boolState[already_required_OF_ManipulationError] << endl;
	cout << "Attribute :  boolState[required_OF_Memory] | Value : " << boolState[required_OF_Memory] << endl;
	cout << "Attribute :  boolState[already_S_OF_Memory] | Value : " << boolState[already_S_OF_Memory] << endl;
	cout << "Attribute :  boolState[S_OF_Memory] | Value : " << boolState[S_OF_Memory] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Memory] | Value : " << boolState[relevant_evt_OF_Memory] << endl;
	cout << "Attribute :  boolState[failF_OF_Memory] | Value : " << boolState[failF_OF_Memory] << endl;
	cout << "Attribute :  boolState[required_OF_OR_1] | Value : " << boolState[required_OF_OR_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_1] | Value : " << boolState[already_S_OF_OR_1] << endl;
	cout << "Attribute :  boolState[S_OF_OR_1] | Value : " << boolState[S_OF_OR_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_1] | Value : " << boolState[relevant_evt_OF_OR_1] << endl;
	cout << "Attribute :  boolState[required_OF_PC_down] | Value : " << boolState[required_OF_PC_down] << endl;
	cout << "Attribute :  boolState[already_S_OF_PC_down] | Value : " << boolState[already_S_OF_PC_down] << endl;
	cout << "Attribute :  boolState[S_OF_PC_down] | Value : " << boolState[S_OF_PC_down] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_PC_down] | Value : " << boolState[relevant_evt_OF_PC_down] << endl;
	cout << "Attribute :  boolState[required_OF_PowerSupply] | Value : " << boolState[required_OF_PowerSupply] << endl;
	cout << "Attribute :  boolState[already_S_OF_PowerSupply] | Value : " << boolState[already_S_OF_PowerSupply] << endl;
	cout << "Attribute :  boolState[S_OF_PowerSupply] | Value : " << boolState[S_OF_PowerSupply] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_PowerSupply] | Value : " << boolState[relevant_evt_OF_PowerSupply] << endl;
	cout << "Attribute :  boolState[failF_OF_PowerSupply] | Value : " << boolState[failF_OF_PowerSupply] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
	cout << "Attribute :  boolState[required_OF_Windows] | Value : " << boolState[required_OF_Windows] << endl;
	cout << "Attribute :  boolState[already_S_OF_Windows] | Value : " << boolState[already_S_OF_Windows] << endl;
	cout << "Attribute :  boolState[S_OF_Windows] | Value : " << boolState[S_OF_Windows] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Windows] | Value : " << boolState[relevant_evt_OF_Windows] << endl;
	cout << "Attribute :  boolState[failF_OF_Windows] | Value : " << boolState[failF_OF_Windows] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_Cpu] = REINITIALISATION_OF_required_OF_Cpu;
	boolState[S_OF_Cpu] = REINITIALISATION_OF_S_OF_Cpu;
	boolState[relevant_evt_OF_Cpu] = REINITIALISATION_OF_relevant_evt_OF_Cpu;
	boolState[required_OF_DisplayUnit] = REINITIALISATION_OF_required_OF_DisplayUnit;
	boolState[S_OF_DisplayUnit] = REINITIALISATION_OF_S_OF_DisplayUnit;
	boolState[relevant_evt_OF_DisplayUnit] = REINITIALISATION_OF_relevant_evt_OF_DisplayUnit;
	boolState[required_OF_Fan] = REINITIALISATION_OF_required_OF_Fan;
	boolState[S_OF_Fan] = REINITIALISATION_OF_S_OF_Fan;
	boolState[relevant_evt_OF_Fan] = REINITIALISATION_OF_relevant_evt_OF_Fan;
	boolState[required_OF_FloppyDrive] = REINITIALISATION_OF_required_OF_FloppyDrive;
	boolState[S_OF_FloppyDrive] = REINITIALISATION_OF_S_OF_FloppyDrive;
	boolState[relevant_evt_OF_FloppyDrive] = REINITIALISATION_OF_relevant_evt_OF_FloppyDrive;
	boolState[required_OF_FloppyDriveInUse] = REINITIALISATION_OF_required_OF_FloppyDriveInUse;
	boolState[S_OF_FloppyDriveInUse] = REINITIALISATION_OF_S_OF_FloppyDriveInUse;
	boolState[relevant_evt_OF_FloppyDriveInUse] = REINITIALISATION_OF_relevant_evt_OF_FloppyDriveInUse;
	boolState[required_OF_HardDrive] = REINITIALISATION_OF_required_OF_HardDrive;
	boolState[S_OF_HardDrive] = REINITIALISATION_OF_S_OF_HardDrive;
	boolState[relevant_evt_OF_HardDrive] = REINITIALISATION_OF_relevant_evt_OF_HardDrive;
	boolState[required_OF_LossOfCooling] = REINITIALISATION_OF_required_OF_LossOfCooling;
	boolState[S_OF_LossOfCooling] = REINITIALISATION_OF_S_OF_LossOfCooling;
	boolState[relevant_evt_OF_LossOfCooling] = REINITIALISATION_OF_relevant_evt_OF_LossOfCooling;
	boolState[required_OF_ManipulationError] = REINITIALISATION_OF_required_OF_ManipulationError;
	boolState[S_OF_ManipulationError] = REINITIALISATION_OF_S_OF_ManipulationError;
	boolState[relevant_evt_OF_ManipulationError] = REINITIALISATION_OF_relevant_evt_OF_ManipulationError;
	boolState[to_be_fired_OF_ManipulationError] = REINITIALISATION_OF_to_be_fired_OF_ManipulationError;
	boolState[required_OF_Memory] = REINITIALISATION_OF_required_OF_Memory;
	boolState[S_OF_Memory] = REINITIALISATION_OF_S_OF_Memory;
	boolState[relevant_evt_OF_Memory] = REINITIALISATION_OF_relevant_evt_OF_Memory;
	boolState[required_OF_OR_1] = REINITIALISATION_OF_required_OF_OR_1;
	boolState[S_OF_OR_1] = REINITIALISATION_OF_S_OF_OR_1;
	boolState[relevant_evt_OF_OR_1] = REINITIALISATION_OF_relevant_evt_OF_OR_1;
	boolState[required_OF_PC_down] = REINITIALISATION_OF_required_OF_PC_down;
	boolState[S_OF_PC_down] = REINITIALISATION_OF_S_OF_PC_down;
	boolState[relevant_evt_OF_PC_down] = REINITIALISATION_OF_relevant_evt_OF_PC_down;
	boolState[required_OF_PowerSupply] = REINITIALISATION_OF_required_OF_PowerSupply;
	boolState[S_OF_PowerSupply] = REINITIALISATION_OF_S_OF_PowerSupply;
	boolState[relevant_evt_OF_PowerSupply] = REINITIALISATION_OF_relevant_evt_OF_PowerSupply;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
	boolState[required_OF_Windows] = REINITIALISATION_OF_required_OF_Windows;
	boolState[S_OF_Windows] = REINITIALISATION_OF_S_OF_Windows;
	boolState[relevant_evt_OF_Windows] = REINITIALISATION_OF_relevant_evt_OF_Windows;
}

void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx17_OF_Cpu = true;
	}

	if (numFire == 1)
	{
		FIRE_xx18_OF_Cpu = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_DisplayUnit = true;
	}

	if (numFire == 3)
	{
		FIRE_xx10_OF_Fan = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_FloppyDrive = true;
	}

	if (numFire == 5)
	{
		FIRE_xx10_OF_FloppyDriveInUse = true;
	}

	if (numFire == 6)
	{
		FIRE_xx10_OF_HardDrive = true;
	}

	if (numFire == 7)
	{
		FIRE_xx23_OF_ManipulationError_INS_7 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx23_OF_ManipulationError_INS_8 = true;
	}

	if (numFire == 9)
	{
		FIRE_xx10_OF_Memory = true;
	}

	if (numFire == 10)
	{
		FIRE_xx10_OF_PowerSupply = true;
	}

	if (numFire == 11)
	{
		FIRE_xx10_OF_Windows = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx17_OF_Cpu
	if ((boolState[failF_OF_Cpu] == false) && (boolState[required_OF_Cpu] && ( !boolState[failS_OF_Cpu]))) 
	{
		 
		if (FIRE_xx17_OF_Cpu)
		{
			boolState[failF_OF_Cpu]  =  true;
			FIRE_xx17_OF_Cpu = false;
		}
	}

	// Occurrence xx18_OF_Cpu
	if ((boolState[failS_OF_Cpu] == false) && (( !boolState[required_OF_Cpu]) && ( !boolState[failF_OF_Cpu]))) 
	{
		 
		if (FIRE_xx18_OF_Cpu)
		{
			boolState[failS_OF_Cpu]  =  true;
			FIRE_xx18_OF_Cpu = false;
		}
	}

	// Occurrence xx10_OF_DisplayUnit
	if ((boolState[failF_OF_DisplayUnit] == false) && boolState[required_OF_DisplayUnit]) 
	{
		 
		if (FIRE_xx10_OF_DisplayUnit)
		{
			boolState[failF_OF_DisplayUnit]  =  true;
			FIRE_xx10_OF_DisplayUnit = false;
		}
	}

	// Occurrence xx10_OF_Fan
	if ((boolState[failF_OF_Fan] == false) && boolState[required_OF_Fan]) 
	{
		 
		if (FIRE_xx10_OF_Fan)
		{
			boolState[failF_OF_Fan]  =  true;
			FIRE_xx10_OF_Fan = false;
		}
	}

	// Occurrence xx10_OF_FloppyDrive
	if ((boolState[failF_OF_FloppyDrive] == false) && boolState[required_OF_FloppyDrive]) 
	{
		 
		if (FIRE_xx10_OF_FloppyDrive)
		{
			boolState[failF_OF_FloppyDrive]  =  true;
			FIRE_xx10_OF_FloppyDrive = false;
		}
	}

	// Occurrence xx10_OF_FloppyDriveInUse
	if ((boolState[failF_OF_FloppyDriveInUse] == false) && boolState[required_OF_FloppyDriveInUse]) 
	{
		 
		if (FIRE_xx10_OF_FloppyDriveInUse)
		{
			boolState[failF_OF_FloppyDriveInUse]  =  true;
			FIRE_xx10_OF_FloppyDriveInUse = false;
		}
	}

	// Occurrence xx10_OF_HardDrive
	if ((boolState[failF_OF_HardDrive] == false) && boolState[required_OF_HardDrive]) 
	{
		 
		if (FIRE_xx10_OF_HardDrive)
		{
			boolState[failF_OF_HardDrive]  =  true;
			FIRE_xx10_OF_HardDrive = false;
		}
	}

	// Occurrence xx23_OF_ManipulationError

	if ((boolState[failI_OF_ManipulationError] == false) && boolState[to_be_fired_OF_ManipulationError]) 
	{
	
		
		if (FIRE_xx23_OF_ManipulationError_INS_7) 
		{
			boolState[failI_OF_ManipulationError]  =  true;
			boolState[already_standby_OF_ManipulationError]  =  false;
			boolState[already_required_OF_ManipulationError]  =  false;
			FIRE_xx23_OF_ManipulationError_INS_7 = false;
		}
	
	}
	if ((boolState[failI_OF_ManipulationError] == false) && boolState[to_be_fired_OF_ManipulationError]) 
	{
	
		
		if (FIRE_xx23_OF_ManipulationError_INS_8) 
		{
			boolState[already_standby_OF_ManipulationError]  =  false;
			boolState[already_required_OF_ManipulationError]  =  false;
			FIRE_xx23_OF_ManipulationError_INS_8 = false;
		}
	
	}
	// Occurrence xx10_OF_Memory
	if ((boolState[failF_OF_Memory] == false) && boolState[required_OF_Memory]) 
	{
		 
		if (FIRE_xx10_OF_Memory)
		{
			boolState[failF_OF_Memory]  =  true;
			FIRE_xx10_OF_Memory = false;
		}
	}

	// Occurrence xx10_OF_PowerSupply
	if ((boolState[failF_OF_PowerSupply] == false) && boolState[required_OF_PowerSupply]) 
	{
		 
		if (FIRE_xx10_OF_PowerSupply)
		{
			boolState[failF_OF_PowerSupply]  =  true;
			FIRE_xx10_OF_PowerSupply = false;
		}
	}

	// Occurrence xx10_OF_Windows
	if ((boolState[failF_OF_Windows] == false) && boolState[required_OF_Windows]) 
	{
		 
		if (FIRE_xx10_OF_Windows)
		{
			boolState[failF_OF_Windows]  =  true;
			FIRE_xx10_OF_Windows = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if ((boolState[failI_OF_ManipulationError] == false) && boolState[to_be_fired_OF_ManipulationError])
	{
		cout << "7 :  INS_SUB_COUNT (1) |FAULT | failI  LABEL \"instantaneous failure ManipulationError\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_ManipulationError]  =  TRUE,already_standby_OF_ManipulationError  =  FALSE,already_required_OF_ManipulationError  =  FALSE" << endl;
		list.push_back(make_tuple(7, 0.0001, "INS", 1));
	}
	if ((boolState[failI_OF_ManipulationError] == false) && boolState[to_be_fired_OF_ManipulationError])
	{
		cout << "8 :  INS_SUB_COUNT (1) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_ManipulationError]  =  FALSE,already_required_OF_ManipulationError  =  FALSE" << endl;
		list.push_back(make_tuple(8, 0.9999, "INS", 1));
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
     
	if ((boolState[failF_OF_Cpu] == false) && (boolState[required_OF_Cpu] && ( !boolState[failS_OF_Cpu])))
	{
		cout << "0 : xx17_OF_Cpu : FAULT failF  LABEL \"failure in operation Cpu\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Cpu]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if ((boolState[failS_OF_Cpu] == false) && (( !boolState[required_OF_Cpu]) && ( !boolState[failF_OF_Cpu])))
	{
		cout << "1 : xx18_OF_Cpu : FAULT failS  LABEL \"standby failure Cpu\"  DIST EXP (1e-05)  INDUCING boolState[failS_OF_Cpu]  =  TRUE" << endl;
		list.push_back(make_tuple(1, 1e-05, "EXP", 0));
	}
	if ((boolState[failF_OF_DisplayUnit] == false) && boolState[required_OF_DisplayUnit])
	{
		cout << "2 : xx10_OF_DisplayUnit : FAULT failF  LABEL \"failure in operation DisplayUnit\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_DisplayUnit]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Fan] == false) && boolState[required_OF_Fan])
	{
		cout << "3 : xx10_OF_Fan : FAULT failF  LABEL \"failure in operation Fan\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Fan]  =  TRUE" << endl;
		list.push_back(make_tuple(3, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_FloppyDrive] == false) && boolState[required_OF_FloppyDrive])
	{
		cout << "4 : xx10_OF_FloppyDrive : FAULT failF  LABEL \"failure in operation FloppyDrive\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_FloppyDrive]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_FloppyDriveInUse] == false) && boolState[required_OF_FloppyDriveInUse])
	{
		cout << "5 : xx10_OF_FloppyDriveInUse : FAULT failF  LABEL \"failure in operation FloppyDriveInUse\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_FloppyDriveInUse]  =  TRUE" << endl;
		list.push_back(make_tuple(5, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_HardDrive] == false) && boolState[required_OF_HardDrive])
	{
		cout << "6 : xx10_OF_HardDrive : FAULT failF  LABEL \"failure in operation HardDrive\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_HardDrive]  =  TRUE" << endl;
		list.push_back(make_tuple(6, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Memory] == false) && boolState[required_OF_Memory])
	{
		cout << "9 : xx10_OF_Memory : FAULT failF  LABEL \"failure in operation Memory\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Memory]  =  TRUE" << endl;
		list.push_back(make_tuple(9, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_PowerSupply] == false) && boolState[required_OF_PowerSupply])
	{
		cout << "10 : xx10_OF_PowerSupply : FAULT failF  LABEL \"failure in operation PowerSupply\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_PowerSupply]  =  TRUE" << endl;
		list.push_back(make_tuple(10, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Windows] == false) && boolState[required_OF_Windows])
	{
		cout << "11 : xx10_OF_Windows : FAULT failF  LABEL \"failure in operation Windows\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Windows]  =  TRUE" << endl;
		list.push_back(make_tuple(11, 0.0001, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::runOnceInteractionStep_initialization()
{
	if ((boolState[failS_OF_Cpu] == true) || (boolState[failF_OF_Cpu] == true) )
	{
		boolState[S_OF_Cpu]  =  true;
	}

	if (boolState[failF_OF_DisplayUnit] == true )
	{
		boolState[S_OF_DisplayUnit]  =  true;
	}

	if (boolState[failF_OF_Fan] == true )
	{
		boolState[S_OF_Fan]  =  true;
	}

	if (boolState[failF_OF_FloppyDrive] == true )
	{
		boolState[S_OF_FloppyDrive]  =  true;
	}

	if (boolState[failF_OF_FloppyDriveInUse] == true )
	{
		boolState[S_OF_FloppyDriveInUse]  =  true;
	}

	if (boolState[failF_OF_HardDrive] == true )
	{
		boolState[S_OF_HardDrive]  =  true;
	}

	if (boolState[failI_OF_ManipulationError] == true )
	{
		boolState[S_OF_ManipulationError]  =  true;
	}

	if (boolState[failF_OF_Memory] == true )
	{
		boolState[S_OF_Memory]  =  true;
	}

	if (boolState[failF_OF_PowerSupply] == true )
	{
		boolState[S_OF_PowerSupply]  =  true;
	}

	if (boolState[failF_OF_Windows] == true )
	{
		boolState[S_OF_Windows]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_OR_1] && boolState[S_OF_FloppyDriveInUse] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (boolState[S_OF_Fan] || boolState[S_OF_PowerSupply] )
	{
		boolState[S_OF_LossOfCooling]  =  true;
	}

	if (boolState[S_OF_ManipulationError] || boolState[S_OF_FloppyDrive] )
	{
		boolState[S_OF_OR_1]  =  true;
	}

	if ((((((boolState[S_OF_PowerSupply] || boolState[S_OF_AND_1]) || boolState[S_OF_Memory]) || boolState[S_OF_HardDrive]) || boolState[S_OF_DisplayUnit]) || boolState[S_OF_Windows]) || boolState[S_OF_Cpu] )
	{
		boolState[S_OF_PC_down]  =  true;
	}

	if (boolState[S_OF_PC_down] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_PC_down] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if (boolState[relevant_evt_OF_PC_down] && ( !boolState[S_OF_PC_down]) )
	{
		boolState[relevant_evt_OF_AND_1]  =  true;
	}

	if (( !boolState[required_OF_PC_down]) || ( !boolState[S_OF_LossOfCooling]) )
	{
		boolState[required_OF_Cpu]  =  false;
	}

	if (boolState[relevant_evt_OF_PC_down] && ( !boolState[S_OF_PC_down]) )
	{
		boolState[relevant_evt_OF_Cpu]  =  true;
	}

	if ( !boolState[required_OF_PC_down] )
	{
		boolState[required_OF_DisplayUnit]  =  false;
	}

	if (boolState[relevant_evt_OF_PC_down] && ( !boolState[S_OF_PC_down]) )
	{
		boolState[relevant_evt_OF_DisplayUnit]  =  true;
	}

	if ( !boolState[required_OF_LossOfCooling] )
	{
		boolState[required_OF_Fan]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfCooling] && ( !boolState[S_OF_LossOfCooling]) )
	{
		boolState[relevant_evt_OF_Fan]  =  true;
	}

	if ( !boolState[required_OF_OR_1] )
	{
		boolState[required_OF_FloppyDrive]  =  false;
	}

	if (boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1]) )
	{
		boolState[relevant_evt_OF_FloppyDrive]  =  true;
	}

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_FloppyDriveInUse]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1])) || (  boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1])) )
	{
		boolState[relevant_evt_OF_FloppyDriveInUse]  =  true;
	}

	if ( !boolState[required_OF_PC_down] )
	{
		boolState[required_OF_HardDrive]  =  false;
	}

	if (boolState[relevant_evt_OF_PC_down] && ( !boolState[S_OF_PC_down]) )
	{
		boolState[relevant_evt_OF_HardDrive]  =  true;
	}

	if (boolState[relevant_evt_OF_Cpu] && ( !boolState[S_OF_Cpu]) )
	{
		boolState[relevant_evt_OF_LossOfCooling]  =  true;
	}

	if ( !boolState[required_OF_OR_1] )
	{
		boolState[required_OF_ManipulationError]  =  false;
	}

	if (boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1]) )
	{
		boolState[relevant_evt_OF_ManipulationError]  =  true;
	}

	if ( !boolState[required_OF_PC_down] )
	{
		boolState[required_OF_Memory]  =  false;
	}

	if (boolState[relevant_evt_OF_PC_down] && ( !boolState[S_OF_PC_down]) )
	{
		boolState[relevant_evt_OF_Memory]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_FloppyDriveInUse]) )
	{
		boolState[required_OF_OR_1]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_OR_1]  =  true;
	}

	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_PC_down]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_PC_down]  =  true;
	}

	if (( !boolState[required_OF_LossOfCooling]) && ( !boolState[required_OF_PC_down]) )
	{
		boolState[required_OF_PowerSupply]  =  false;
	}

	if ((boolState[relevant_evt_OF_LossOfCooling] && ( !boolState[S_OF_LossOfCooling])) || (boolState[relevant_evt_OF_PC_down] && ( !boolState[S_OF_PC_down])) )
	{
		boolState[relevant_evt_OF_PowerSupply]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

	if ( !boolState[required_OF_PC_down] )
	{
		boolState[required_OF_Windows]  =  false;
	}

	if (boolState[relevant_evt_OF_PC_down] && ( !boolState[S_OF_PC_down]) )
	{
		boolState[relevant_evt_OF_Windows]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_Cpu]  =  boolState[S_OF_Cpu]  ;



	boolState[already_S_OF_DisplayUnit]  =  boolState[S_OF_DisplayUnit]  ;



	boolState[already_S_OF_Fan]  =  boolState[S_OF_Fan]  ;



	boolState[already_S_OF_FloppyDrive]  =  boolState[S_OF_FloppyDrive]  ;



	boolState[already_S_OF_FloppyDriveInUse]  =  boolState[S_OF_FloppyDriveInUse]  ;



	boolState[already_S_OF_HardDrive]  =  boolState[S_OF_HardDrive]  ;



	boolState[already_S_OF_LossOfCooling]  =  boolState[S_OF_LossOfCooling]  ;



	boolState[already_S_OF_ManipulationError]  =  boolState[S_OF_ManipulationError]  ;

	if (( !boolState[required_OF_ManipulationError]) && (( !  boolState[already_standby_OF_ManipulationError]) && ( !  boolState[already_required_OF_ManipulationError])) )
	{
		boolState[already_standby_OF_ManipulationError]  =  true;
	}



	boolState[already_S_OF_Memory]  =  boolState[S_OF_Memory]  ;



	boolState[already_S_OF_OR_1]  =  boolState[S_OF_OR_1]  ;



	boolState[already_S_OF_PC_down]  =  boolState[S_OF_PC_down]  ;



	boolState[already_S_OF_PowerSupply]  =  boolState[S_OF_PowerSupply]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;



	boolState[already_S_OF_Windows]  =  boolState[S_OF_Windows]  ;

}


void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::runOnceInteractionStep_tops()
{
	if (boolState[required_OF_ManipulationError] && boolState[already_standby_OF_ManipulationError] )
	{
		boolState[to_be_fired_OF_ManipulationError]  =  true;
	}

}

void
storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_08_PC_No_trim_No_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    