#include <iostream>
#include "FigaroModelBDMP_02_3trains_standby_redundancy_Trim_Article_No_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_AND_1 = true;
	boolState[already_S_OF_AND_1] = false;
	REINITIALISATION_OF_S_OF_AND_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_1 = false;
	REINITIALISATION_OF_required_OF_AND_2 = true;
	boolState[already_S_OF_AND_2] = false;
	REINITIALISATION_OF_S_OF_AND_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_2 = false;
	REINITIALISATION_OF_required_OF_A_lost = true;
	boolState[already_S_OF_A_lost] = false;
	REINITIALISATION_OF_S_OF_A_lost = false;
	REINITIALISATION_OF_relevant_evt_OF_A_lost = false;
	REINITIALISATION_OF_required_OF_A_op = true;
	boolState[already_S_OF_A_op] = false;
	REINITIALISATION_OF_S_OF_A_op = false;
	REINITIALISATION_OF_relevant_evt_OF_A_op = false;
	boolState[failF_OF_A_op] = false;
	REINITIALISATION_OF_required_OF_A_start = true;
	boolState[already_S_OF_A_start] = false;
	REINITIALISATION_OF_S_OF_A_start = false;
	REINITIALISATION_OF_relevant_evt_OF_A_start = false;
	boolState[failI_OF_A_start] = false;
	REINITIALISATION_OF_to_be_fired_OF_A_start = false;
	boolState[already_standby_OF_A_start] = false;
	boolState[already_required_OF_A_start] = false;
	REINITIALISATION_OF_required_OF_B_op = true;
	boolState[already_S_OF_B_op] = false;
	REINITIALISATION_OF_S_OF_B_op = false;
	REINITIALISATION_OF_relevant_evt_OF_B_op = false;
	boolState[failF_OF_B_op] = false;
	REINITIALISATION_OF_required_OF_C_fail = true;
	boolState[already_S_OF_C_fail] = false;
	REINITIALISATION_OF_S_OF_C_fail = false;
	REINITIALISATION_OF_relevant_evt_OF_C_fail = false;
	boolState[failF_OF_C_fail] = false;
	REINITIALISATION_OF_required_OF_C_start = true;
	boolState[already_S_OF_C_start] = false;
	REINITIALISATION_OF_S_OF_C_start = false;
	REINITIALISATION_OF_relevant_evt_OF_C_start = false;
	boolState[failI_OF_C_start] = false;
	REINITIALISATION_OF_to_be_fired_OF_C_start = false;
	boolState[already_standby_OF_C_start] = false;
	boolState[already_required_OF_C_start] = false;
	REINITIALISATION_OF_required_OF_OR_1 = true;
	boolState[already_S_OF_OR_1] = false;
	REINITIALISATION_OF_S_OF_OR_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_1 = false;
	REINITIALISATION_OF_required_OF_OR_2 = true;
	boolState[already_S_OF_OR_2] = false;
	REINITIALISATION_OF_S_OF_OR_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_2 = false;
	REINITIALISATION_OF_required_OF_P_op = true;
	boolState[already_S_OF_P_op] = false;
	REINITIALISATION_OF_S_OF_P_op = false;
	REINITIALISATION_OF_relevant_evt_OF_P_op = false;
	boolState[failF_OF_P_op] = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_A_op = false;
	FIRE_xx23_OF_A_start_INS_1 = false;
	FIRE_xx23_OF_A_start_INS_2 = false;
	FIRE_xx10_OF_B_op = false;
	FIRE_xx10_OF_C_fail = false;
	FIRE_xx23_OF_C_start_INS_5 = false;
	FIRE_xx23_OF_C_start_INS_6 = false;
	FIRE_xx10_OF_P_op = false;

}

void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_AND_1] | Value : " << boolState[required_OF_AND_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_1] | Value : " << boolState[already_S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[S_OF_AND_1] | Value : " << boolState[S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_1] | Value : " << boolState[relevant_evt_OF_AND_1] << endl;
	cout << "Attribute :  boolState[required_OF_AND_2] | Value : " << boolState[required_OF_AND_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_2] | Value : " << boolState[already_S_OF_AND_2] << endl;
	cout << "Attribute :  boolState[S_OF_AND_2] | Value : " << boolState[S_OF_AND_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_2] | Value : " << boolState[relevant_evt_OF_AND_2] << endl;
	cout << "Attribute :  boolState[required_OF_A_lost] | Value : " << boolState[required_OF_A_lost] << endl;
	cout << "Attribute :  boolState[already_S_OF_A_lost] | Value : " << boolState[already_S_OF_A_lost] << endl;
	cout << "Attribute :  boolState[S_OF_A_lost] | Value : " << boolState[S_OF_A_lost] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A_lost] | Value : " << boolState[relevant_evt_OF_A_lost] << endl;
	cout << "Attribute :  boolState[required_OF_A_op] | Value : " << boolState[required_OF_A_op] << endl;
	cout << "Attribute :  boolState[already_S_OF_A_op] | Value : " << boolState[already_S_OF_A_op] << endl;
	cout << "Attribute :  boolState[S_OF_A_op] | Value : " << boolState[S_OF_A_op] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A_op] | Value : " << boolState[relevant_evt_OF_A_op] << endl;
	cout << "Attribute :  boolState[failF_OF_A_op] | Value : " << boolState[failF_OF_A_op] << endl;
	cout << "Attribute :  boolState[required_OF_A_start] | Value : " << boolState[required_OF_A_start] << endl;
	cout << "Attribute :  boolState[already_S_OF_A_start] | Value : " << boolState[already_S_OF_A_start] << endl;
	cout << "Attribute :  boolState[S_OF_A_start] | Value : " << boolState[S_OF_A_start] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A_start] | Value : " << boolState[relevant_evt_OF_A_start] << endl;
	cout << "Attribute :  boolState[failI_OF_A_start] | Value : " << boolState[failI_OF_A_start] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_A_start] | Value : " << boolState[to_be_fired_OF_A_start] << endl;
	cout << "Attribute :  boolState[already_standby_OF_A_start] | Value : " << boolState[already_standby_OF_A_start] << endl;
	cout << "Attribute :  boolState[already_required_OF_A_start] | Value : " << boolState[already_required_OF_A_start] << endl;
	cout << "Attribute :  boolState[required_OF_B_op] | Value : " << boolState[required_OF_B_op] << endl;
	cout << "Attribute :  boolState[already_S_OF_B_op] | Value : " << boolState[already_S_OF_B_op] << endl;
	cout << "Attribute :  boolState[S_OF_B_op] | Value : " << boolState[S_OF_B_op] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_B_op] | Value : " << boolState[relevant_evt_OF_B_op] << endl;
	cout << "Attribute :  boolState[failF_OF_B_op] | Value : " << boolState[failF_OF_B_op] << endl;
	cout << "Attribute :  boolState[required_OF_C_fail] | Value : " << boolState[required_OF_C_fail] << endl;
	cout << "Attribute :  boolState[already_S_OF_C_fail] | Value : " << boolState[already_S_OF_C_fail] << endl;
	cout << "Attribute :  boolState[S_OF_C_fail] | Value : " << boolState[S_OF_C_fail] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_C_fail] | Value : " << boolState[relevant_evt_OF_C_fail] << endl;
	cout << "Attribute :  boolState[failF_OF_C_fail] | Value : " << boolState[failF_OF_C_fail] << endl;
	cout << "Attribute :  boolState[required_OF_C_start] | Value : " << boolState[required_OF_C_start] << endl;
	cout << "Attribute :  boolState[already_S_OF_C_start] | Value : " << boolState[already_S_OF_C_start] << endl;
	cout << "Attribute :  boolState[S_OF_C_start] | Value : " << boolState[S_OF_C_start] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_C_start] | Value : " << boolState[relevant_evt_OF_C_start] << endl;
	cout << "Attribute :  boolState[failI_OF_C_start] | Value : " << boolState[failI_OF_C_start] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_C_start] | Value : " << boolState[to_be_fired_OF_C_start] << endl;
	cout << "Attribute :  boolState[already_standby_OF_C_start] | Value : " << boolState[already_standby_OF_C_start] << endl;
	cout << "Attribute :  boolState[already_required_OF_C_start] | Value : " << boolState[already_required_OF_C_start] << endl;
	cout << "Attribute :  boolState[required_OF_OR_1] | Value : " << boolState[required_OF_OR_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_1] | Value : " << boolState[already_S_OF_OR_1] << endl;
	cout << "Attribute :  boolState[S_OF_OR_1] | Value : " << boolState[S_OF_OR_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_1] | Value : " << boolState[relevant_evt_OF_OR_1] << endl;
	cout << "Attribute :  boolState[required_OF_OR_2] | Value : " << boolState[required_OF_OR_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_2] | Value : " << boolState[already_S_OF_OR_2] << endl;
	cout << "Attribute :  boolState[S_OF_OR_2] | Value : " << boolState[S_OF_OR_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_2] | Value : " << boolState[relevant_evt_OF_OR_2] << endl;
	cout << "Attribute :  boolState[required_OF_P_op] | Value : " << boolState[required_OF_P_op] << endl;
	cout << "Attribute :  boolState[already_S_OF_P_op] | Value : " << boolState[already_S_OF_P_op] << endl;
	cout << "Attribute :  boolState[S_OF_P_op] | Value : " << boolState[S_OF_P_op] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_P_op] | Value : " << boolState[relevant_evt_OF_P_op] << endl;
	cout << "Attribute :  boolState[failF_OF_P_op] | Value : " << boolState[failF_OF_P_op] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_AND_2] = REINITIALISATION_OF_required_OF_AND_2;
	boolState[S_OF_AND_2] = REINITIALISATION_OF_S_OF_AND_2;
	boolState[relevant_evt_OF_AND_2] = REINITIALISATION_OF_relevant_evt_OF_AND_2;
	boolState[required_OF_A_lost] = REINITIALISATION_OF_required_OF_A_lost;
	boolState[S_OF_A_lost] = REINITIALISATION_OF_S_OF_A_lost;
	boolState[relevant_evt_OF_A_lost] = REINITIALISATION_OF_relevant_evt_OF_A_lost;
	boolState[required_OF_A_op] = REINITIALISATION_OF_required_OF_A_op;
	boolState[S_OF_A_op] = REINITIALISATION_OF_S_OF_A_op;
	boolState[relevant_evt_OF_A_op] = REINITIALISATION_OF_relevant_evt_OF_A_op;
	boolState[required_OF_A_start] = REINITIALISATION_OF_required_OF_A_start;
	boolState[S_OF_A_start] = REINITIALISATION_OF_S_OF_A_start;
	boolState[relevant_evt_OF_A_start] = REINITIALISATION_OF_relevant_evt_OF_A_start;
	boolState[to_be_fired_OF_A_start] = REINITIALISATION_OF_to_be_fired_OF_A_start;
	boolState[required_OF_B_op] = REINITIALISATION_OF_required_OF_B_op;
	boolState[S_OF_B_op] = REINITIALISATION_OF_S_OF_B_op;
	boolState[relevant_evt_OF_B_op] = REINITIALISATION_OF_relevant_evt_OF_B_op;
	boolState[required_OF_C_fail] = REINITIALISATION_OF_required_OF_C_fail;
	boolState[S_OF_C_fail] = REINITIALISATION_OF_S_OF_C_fail;
	boolState[relevant_evt_OF_C_fail] = REINITIALISATION_OF_relevant_evt_OF_C_fail;
	boolState[required_OF_C_start] = REINITIALISATION_OF_required_OF_C_start;
	boolState[S_OF_C_start] = REINITIALISATION_OF_S_OF_C_start;
	boolState[relevant_evt_OF_C_start] = REINITIALISATION_OF_relevant_evt_OF_C_start;
	boolState[to_be_fired_OF_C_start] = REINITIALISATION_OF_to_be_fired_OF_C_start;
	boolState[required_OF_OR_1] = REINITIALISATION_OF_required_OF_OR_1;
	boolState[S_OF_OR_1] = REINITIALISATION_OF_S_OF_OR_1;
	boolState[relevant_evt_OF_OR_1] = REINITIALISATION_OF_relevant_evt_OF_OR_1;
	boolState[required_OF_OR_2] = REINITIALISATION_OF_required_OF_OR_2;
	boolState[S_OF_OR_2] = REINITIALISATION_OF_S_OF_OR_2;
	boolState[relevant_evt_OF_OR_2] = REINITIALISATION_OF_relevant_evt_OF_OR_2;
	boolState[required_OF_P_op] = REINITIALISATION_OF_required_OF_P_op;
	boolState[S_OF_P_op] = REINITIALISATION_OF_S_OF_P_op;
	boolState[relevant_evt_OF_P_op] = REINITIALISATION_OF_relevant_evt_OF_P_op;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
}

void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_A_op = true;
	}

	if (numFire == 1)
	{
		FIRE_xx23_OF_A_start_INS_1 = true;
	}

	if (numFire == 2)
	{
		FIRE_xx23_OF_A_start_INS_2 = true;
	}

	if (numFire == 3)
	{
		FIRE_xx10_OF_B_op = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_C_fail = true;
	}

	if (numFire == 5)
	{
		FIRE_xx23_OF_C_start_INS_5 = true;
	}

	if (numFire == 6)
	{
		FIRE_xx23_OF_C_start_INS_6 = true;
	}

	if (numFire == 7)
	{
		FIRE_xx10_OF_P_op = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_A_op
	if ((boolState[failF_OF_A_op] == false) && (boolState[required_OF_A_op] &&  boolState[relevant_evt_OF_A_op])) 
	{
		 
		if (FIRE_xx10_OF_A_op)
		{
			boolState[failF_OF_A_op]  =  true;
			FIRE_xx10_OF_A_op = false;
		}
	}

	// Occurrence xx23_OF_A_start

	if ((boolState[failI_OF_A_start] == false) && (boolState[to_be_fired_OF_A_start] &&
	boolState[relevant_evt_OF_A_start])) 
	{
	
		
		if (FIRE_xx23_OF_A_start_INS_1) 
		{
			boolState[failI_OF_A_start]  =  true;
			boolState[already_standby_OF_A_start]  =  false;
			boolState[already_required_OF_A_start]  =  false;
			FIRE_xx23_OF_A_start_INS_1 = false;
		}
	
	}
	if ((boolState[failI_OF_A_start] == false) && (boolState[to_be_fired_OF_A_start] &&
	boolState[relevant_evt_OF_A_start])) 
	{
	
		
		if (FIRE_xx23_OF_A_start_INS_2) 
		{
			boolState[already_standby_OF_A_start]  =  false;
			boolState[already_required_OF_A_start]  =  false;
			FIRE_xx23_OF_A_start_INS_2 = false;
		}
	
	}
	// Occurrence xx10_OF_B_op
	if ((boolState[failF_OF_B_op] == false) && (boolState[required_OF_B_op] &&  boolState[relevant_evt_OF_B_op])) 
	{
		 
		if (FIRE_xx10_OF_B_op)
		{
			boolState[failF_OF_B_op]  =  true;
			FIRE_xx10_OF_B_op = false;
		}
	}

	// Occurrence xx10_OF_C_fail
	if ((boolState[failF_OF_C_fail] == false) && (boolState[required_OF_C_fail] &&  boolState[relevant_evt_OF_C_fail])) 
	{
		 
		if (FIRE_xx10_OF_C_fail)
		{
			boolState[failF_OF_C_fail]  =  true;
			FIRE_xx10_OF_C_fail = false;
		}
	}

	// Occurrence xx23_OF_C_start

	if ((boolState[failI_OF_C_start] == false) && (boolState[to_be_fired_OF_C_start] &&
	boolState[relevant_evt_OF_C_start])) 
	{
	
		
		if (FIRE_xx23_OF_C_start_INS_5) 
		{
			boolState[failI_OF_C_start]  =  true;
			boolState[already_standby_OF_C_start]  =  false;
			boolState[already_required_OF_C_start]  =  false;
			FIRE_xx23_OF_C_start_INS_5 = false;
		}
	
	}
	if ((boolState[failI_OF_C_start] == false) && (boolState[to_be_fired_OF_C_start] &&
	boolState[relevant_evt_OF_C_start])) 
	{
	
		
		if (FIRE_xx23_OF_C_start_INS_6) 
		{
			boolState[already_standby_OF_C_start]  =  false;
			boolState[already_required_OF_C_start]  =  false;
			FIRE_xx23_OF_C_start_INS_6 = false;
		}
	
	}
	// Occurrence xx10_OF_P_op
	if ((boolState[failF_OF_P_op] == false) && (boolState[required_OF_P_op] &&  boolState[relevant_evt_OF_P_op])) 
	{
		 
		if (FIRE_xx10_OF_P_op)
		{
			boolState[failF_OF_P_op]  =  true;
			FIRE_xx10_OF_P_op = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if ((boolState[failI_OF_A_start] == false) && (boolState[to_be_fired_OF_A_start] &&	boolState[relevant_evt_OF_A_start]))
	{
		cout << "1 :  INS_SUB_COUNT (1) |FAULT | failI  LABEL \"instantaneous failure A_start\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_A_start]  =  TRUE,already_standby_OF_A_start  =  FALSE,already_required_OF_A_start  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.0001, "INS", 1));
	}
	if ((boolState[failI_OF_A_start] == false) && (boolState[to_be_fired_OF_A_start] &&	boolState[relevant_evt_OF_A_start]))
	{
		cout << "2 :  INS_SUB_COUNT (1) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_A_start]  =  FALSE,already_required_OF_A_start  =  FALSE" << endl;
		list.push_back(make_tuple(2, 0.9999, "INS", 1));
	}
	if ((boolState[failI_OF_C_start] == false) && (boolState[to_be_fired_OF_C_start] &&	boolState[relevant_evt_OF_C_start]))
	{
		cout << "5 :  INS_SUB_COUNT (2) |FAULT | failI  LABEL \"instantaneous failure C_start\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_C_start]  =  TRUE,already_standby_OF_C_start  =  FALSE,already_required_OF_C_start  =  FALSE" << endl;
		list.push_back(make_tuple(5, 0.0001, "INS", 2));
	}
	if ((boolState[failI_OF_C_start] == false) && (boolState[to_be_fired_OF_C_start] &&	boolState[relevant_evt_OF_C_start]))
	{
		cout << "6 :  INS_SUB_COUNT (2) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_C_start]  =  FALSE,already_required_OF_C_start  =  FALSE" << endl;
		list.push_back(make_tuple(6, 0.9999, "INS", 2));
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
     
	if ((boolState[failF_OF_A_op] == false) && (boolState[required_OF_A_op] && boolState[relevant_evt_OF_A_op]))
	{
		cout << "0 : xx10_OF_A_op : FAULT failF  LABEL \"failure in operation A_op\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_A_op]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_B_op] == false) && (boolState[required_OF_B_op] && boolState[relevant_evt_OF_B_op]))
	{
		cout << "3 : xx10_OF_B_op : FAULT failF  LABEL \"failure in operation B_op\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_B_op]  =  TRUE" << endl;
		list.push_back(make_tuple(3, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_C_fail] == false) && (boolState[required_OF_C_fail] && boolState[relevant_evt_OF_C_fail]))
	{
		cout << "4 : xx10_OF_C_fail : FAULT failF  LABEL \"failure in operation C_fail\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_C_fail]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_P_op] == false) && (boolState[required_OF_P_op] && boolState[relevant_evt_OF_P_op]))
	{
		cout << "7 : xx10_OF_P_op : FAULT failF  LABEL \"failure in operation P_op\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_P_op]  =  TRUE" << endl;
		list.push_back(make_tuple(7, 0.0001, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_A_op] == true )
	{
		boolState[S_OF_A_op]  =  true;
	}

	if (boolState[failI_OF_A_start] == true )
	{
		boolState[S_OF_A_start]  =  true;
	}

	if (boolState[failF_OF_B_op] == true )
	{
		boolState[S_OF_B_op]  =  true;
	}

	if (boolState[failF_OF_C_fail] == true )
	{
		boolState[S_OF_C_fail]  =  true;
	}

	if (boolState[failI_OF_C_start] == true )
	{
		boolState[S_OF_C_start]  =  true;
	}

	if (boolState[failF_OF_P_op] == true )
	{
		boolState[S_OF_P_op]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_OR_1] && boolState[S_OF_P_op] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (boolState[S_OF_A_lost] && boolState[S_OF_B_op] )
	{
		boolState[S_OF_AND_2]  =  true;
	}

	if (boolState[S_OF_A_op] || boolState[S_OF_A_start] )
	{
		boolState[S_OF_A_lost]  =  true;
	}

	if (boolState[S_OF_AND_2] || boolState[S_OF_C_start] )
	{
		boolState[S_OF_OR_1]  =  true;
	}

	if (boolState[S_OF_C_fail] || boolState[S_OF_P_op] )
	{
		boolState[S_OF_OR_2]  =  true;
	}

	if (boolState[S_OF_AND_1] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_AND_1]  =  true;
	}

	if ( !boolState[required_OF_OR_1] )
	{
		boolState[required_OF_AND_2]  =  false;
	}

	if (boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1]) )
	{
		boolState[relevant_evt_OF_AND_2]  =  true;
	}

	if ( !boolState[required_OF_AND_2] )
	{
		boolState[required_OF_A_lost]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_2] && ( !boolState[S_OF_AND_2]) )
	{
		boolState[relevant_evt_OF_A_lost]  =  true;
	}

	if ( !boolState[required_OF_A_lost] )
	{
		boolState[required_OF_A_op]  =  false;
	}

	if (boolState[relevant_evt_OF_A_lost] && ( !boolState[S_OF_A_lost]) )
	{
		boolState[relevant_evt_OF_A_op]  =  true;
	}

	if ( !boolState[required_OF_A_lost] )
	{
		boolState[required_OF_A_start]  =  false;
	}

	if (boolState[relevant_evt_OF_A_lost] && ( !boolState[S_OF_A_lost]) )
	{
		boolState[relevant_evt_OF_A_start]  =  true;
	}

	if ( !boolState[required_OF_AND_2] )
	{
		boolState[required_OF_B_op]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_2] && ( !boolState[S_OF_AND_2]) )
	{
		boolState[relevant_evt_OF_B_op]  =  true;
	}

	if (( !boolState[required_OF_OR_2]) || ( !boolState[S_OF_P_op]) )
	{
		boolState[required_OF_C_fail]  =  false;
	}

	if ((boolState[relevant_evt_OF_OR_2] && ( !boolState[S_OF_OR_2])) || ( !boolState[S_OF_C_start]) )
	{
		boolState[relevant_evt_OF_C_fail]  =  true;
	}

	if (( !boolState[required_OF_OR_1]) || ( !boolState[S_OF_C_fail]) )
	{
		boolState[required_OF_C_start]  =  false;
	}

	if (boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1]) )
	{
		boolState[relevant_evt_OF_C_start]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_OR_2]) )
	{
		boolState[required_OF_OR_1]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_OR_1]  =  true;
	}

	if ( !boolState[S_OF_OR_1] )
	{
		boolState[relevant_evt_OF_OR_2]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) && ( !boolState[required_OF_OR_2]) )
	{
		boolState[required_OF_P_op]  =  false;
	}

	if (((boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1])) || (  boolState[relevant_evt_OF_OR_2] && ( !boolState[S_OF_OR_2]))) || ( !boolState[S_OF_C_fail]) )
	{
		boolState[relevant_evt_OF_P_op]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

}


void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_AND_2]  =  boolState[S_OF_AND_2]  ;



	boolState[already_S_OF_A_lost]  =  boolState[S_OF_A_lost]  ;



	boolState[already_S_OF_A_op]  =  boolState[S_OF_A_op]  ;



	boolState[already_S_OF_A_start]  =  boolState[S_OF_A_start]  ;

	if (( !boolState[required_OF_A_start]) && (( !boolState[already_standby_OF_A_start]) && ( !boolState[already_required_OF_A_start])) )
	{
		boolState[already_standby_OF_A_start]  =  true;
	}



	boolState[already_S_OF_B_op]  =  boolState[S_OF_B_op]  ;



	boolState[already_S_OF_C_fail]  =  boolState[S_OF_C_fail]  ;



	boolState[already_S_OF_C_start]  =  boolState[S_OF_C_start]  ;

	if (( !boolState[required_OF_C_start]) && (( !boolState[already_standby_OF_C_start]) && ( !boolState[already_required_OF_C_start])) )
	{
		boolState[already_standby_OF_C_start]  =  true;
	}



	boolState[already_S_OF_OR_1]  =  boolState[S_OF_OR_1]  ;



	boolState[already_S_OF_OR_2]  =  boolState[S_OF_OR_2]  ;



	boolState[already_S_OF_P_op]  =  boolState[S_OF_P_op]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;

}


void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::runOnceInteractionStep_tops()
{
	if (boolState[required_OF_A_start] && boolState[already_standby_OF_A_start] )
	{
		boolState[to_be_fired_OF_A_start]  =  true;
	}

	if (boolState[required_OF_C_start] && boolState[already_standby_OF_C_start] )
	{
		boolState[to_be_fired_OF_C_start]  =  true;
	}

}

void
storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_02_3trains_standby_redundancy_Trim_Article_No_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    