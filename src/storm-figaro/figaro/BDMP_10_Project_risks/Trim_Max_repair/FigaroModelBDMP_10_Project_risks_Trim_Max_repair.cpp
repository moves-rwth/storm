#include <iostream>
#include "FigaroModelBDMP_10_Project_risks_Trim_Max_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_AND_1 = true;
	boolState[already_S_OF_AND_1] = false;
	REINITIALISATION_OF_S_OF_AND_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_1 = false;
	REINITIALISATION_OF_required_OF_AND_1_1 = true;
	boolState[already_S_OF_AND_1_1] = false;
	REINITIALISATION_OF_S_OF_AND_1_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_1_1 = false;
	REINITIALISATION_OF_required_OF_F_1 = true;
	boolState[already_S_OF_F_1] = false;
	REINITIALISATION_OF_S_OF_F_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_F_1 = false;
	boolState[failF_OF_F_1] = false;
	REINITIALISATION_OF_required_OF_F_2 = true;
	boolState[already_S_OF_F_2] = false;
	REINITIALISATION_OF_S_OF_F_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_F_2 = false;
	boolState[failF_OF_F_2] = false;
	REINITIALISATION_OF_required_OF_F_3 = true;
	boolState[already_S_OF_F_3] = false;
	REINITIALISATION_OF_S_OF_F_3 = false;
	REINITIALISATION_OF_relevant_evt_OF_F_3 = false;
	boolState[failF_OF_F_3] = false;
	REINITIALISATION_OF_required_OF_F_3_1 = true;
	boolState[already_S_OF_F_3_1] = false;
	REINITIALISATION_OF_S_OF_F_3_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_F_3_1 = false;
	boolState[failF_OF_F_3_1] = false;
	REINITIALISATION_OF_required_OF_F_4 = true;
	boolState[already_S_OF_F_4] = false;
	REINITIALISATION_OF_S_OF_F_4 = false;
	REINITIALISATION_OF_relevant_evt_OF_F_4 = false;
	boolState[failF_OF_F_4] = false;
	REINITIALISATION_OF_required_OF_I_1 = true;
	boolState[already_S_OF_I_1] = false;
	REINITIALISATION_OF_S_OF_I_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_I_1 = false;
	boolState[failI_OF_I_1] = false;
	REINITIALISATION_OF_to_be_fired_OF_I_1 = false;
	boolState[already_standby_OF_I_1] = false;
	boolState[already_required_OF_I_1] = false;
	REINITIALISATION_OF_required_OF_Major_risk = true;
	boolState[already_S_OF_Major_risk] = false;
	REINITIALISATION_OF_S_OF_Major_risk = false;
	REINITIALISATION_OF_relevant_evt_OF_Major_risk = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_F_1 = false;
	FIRE_xx11_OF_F_1 = false;
	FIRE_xx10_OF_F_2 = false;
	FIRE_xx11_OF_F_2 = false;
	FIRE_xx10_OF_F_3 = false;
	FIRE_xx11_OF_F_3 = false;
	FIRE_xx10_OF_F_3_1 = false;
	FIRE_xx11_OF_F_3_1 = false;
	FIRE_xx10_OF_F_4 = false;
	FIRE_xx11_OF_F_4 = false;
	FIRE_xx23_OF_I_1_INS_10 = false;
	FIRE_xx23_OF_I_1_INS_11 = false;
	FIRE_xx24_OF_I_1 = false;

}

void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_AND_1] | Value : " << boolState[required_OF_AND_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_1] | Value : " << boolState[already_S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[S_OF_AND_1] | Value : " << boolState[S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_1] | Value : " << boolState[relevant_evt_OF_AND_1] << endl;
	cout << "Attribute :  boolState[required_OF_AND_1_1] | Value : " << boolState[required_OF_AND_1_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_1_1] | Value : " << boolState[already_S_OF_AND_1_1] << endl;
	cout << "Attribute :  boolState[S_OF_AND_1_1] | Value : " << boolState[S_OF_AND_1_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_1_1] | Value : " << boolState[relevant_evt_OF_AND_1_1] << endl;
	cout << "Attribute :  boolState[required_OF_F_1] | Value : " << boolState[required_OF_F_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_F_1] | Value : " << boolState[already_S_OF_F_1] << endl;
	cout << "Attribute :  boolState[S_OF_F_1] | Value : " << boolState[S_OF_F_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_F_1] | Value : " << boolState[relevant_evt_OF_F_1] << endl;
	cout << "Attribute :  boolState[failF_OF_F_1] | Value : " << boolState[failF_OF_F_1] << endl;
	cout << "Attribute :  boolState[required_OF_F_2] | Value : " << boolState[required_OF_F_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_F_2] | Value : " << boolState[already_S_OF_F_2] << endl;
	cout << "Attribute :  boolState[S_OF_F_2] | Value : " << boolState[S_OF_F_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_F_2] | Value : " << boolState[relevant_evt_OF_F_2] << endl;
	cout << "Attribute :  boolState[failF_OF_F_2] | Value : " << boolState[failF_OF_F_2] << endl;
	cout << "Attribute :  boolState[required_OF_F_3] | Value : " << boolState[required_OF_F_3] << endl;
	cout << "Attribute :  boolState[already_S_OF_F_3] | Value : " << boolState[already_S_OF_F_3] << endl;
	cout << "Attribute :  boolState[S_OF_F_3] | Value : " << boolState[S_OF_F_3] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_F_3] | Value : " << boolState[relevant_evt_OF_F_3] << endl;
	cout << "Attribute :  boolState[failF_OF_F_3] | Value : " << boolState[failF_OF_F_3] << endl;
	cout << "Attribute :  boolState[required_OF_F_3_1] | Value : " << boolState[required_OF_F_3_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_F_3_1] | Value : " << boolState[already_S_OF_F_3_1] << endl;
	cout << "Attribute :  boolState[S_OF_F_3_1] | Value : " << boolState[S_OF_F_3_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_F_3_1] | Value : " << boolState[relevant_evt_OF_F_3_1] << endl;
	cout << "Attribute :  boolState[failF_OF_F_3_1] | Value : " << boolState[failF_OF_F_3_1] << endl;
	cout << "Attribute :  boolState[required_OF_F_4] | Value : " << boolState[required_OF_F_4] << endl;
	cout << "Attribute :  boolState[already_S_OF_F_4] | Value : " << boolState[already_S_OF_F_4] << endl;
	cout << "Attribute :  boolState[S_OF_F_4] | Value : " << boolState[S_OF_F_4] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_F_4] | Value : " << boolState[relevant_evt_OF_F_4] << endl;
	cout << "Attribute :  boolState[failF_OF_F_4] | Value : " << boolState[failF_OF_F_4] << endl;
	cout << "Attribute :  boolState[required_OF_I_1] | Value : " << boolState[required_OF_I_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_I_1] | Value : " << boolState[already_S_OF_I_1] << endl;
	cout << "Attribute :  boolState[S_OF_I_1] | Value : " << boolState[S_OF_I_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_I_1] | Value : " << boolState[relevant_evt_OF_I_1] << endl;
	cout << "Attribute :  boolState[failI_OF_I_1] | Value : " << boolState[failI_OF_I_1] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_I_1] | Value : " << boolState[to_be_fired_OF_I_1] << endl;
	cout << "Attribute :  boolState[already_standby_OF_I_1] | Value : " << boolState[already_standby_OF_I_1] << endl;
	cout << "Attribute :  boolState[already_required_OF_I_1] | Value : " << boolState[already_required_OF_I_1] << endl;
	cout << "Attribute :  boolState[required_OF_Major_risk] | Value : " << boolState[required_OF_Major_risk] << endl;
	cout << "Attribute :  boolState[already_S_OF_Major_risk] | Value : " << boolState[already_S_OF_Major_risk] << endl;
	cout << "Attribute :  boolState[S_OF_Major_risk] | Value : " << boolState[S_OF_Major_risk] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Major_risk] | Value : " << boolState[relevant_evt_OF_Major_risk] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_AND_1_1] = REINITIALISATION_OF_required_OF_AND_1_1;
	boolState[S_OF_AND_1_1] = REINITIALISATION_OF_S_OF_AND_1_1;
	boolState[relevant_evt_OF_AND_1_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1_1;
	boolState[required_OF_F_1] = REINITIALISATION_OF_required_OF_F_1;
	boolState[S_OF_F_1] = REINITIALISATION_OF_S_OF_F_1;
	boolState[relevant_evt_OF_F_1] = REINITIALISATION_OF_relevant_evt_OF_F_1;
	boolState[required_OF_F_2] = REINITIALISATION_OF_required_OF_F_2;
	boolState[S_OF_F_2] = REINITIALISATION_OF_S_OF_F_2;
	boolState[relevant_evt_OF_F_2] = REINITIALISATION_OF_relevant_evt_OF_F_2;
	boolState[required_OF_F_3] = REINITIALISATION_OF_required_OF_F_3;
	boolState[S_OF_F_3] = REINITIALISATION_OF_S_OF_F_3;
	boolState[relevant_evt_OF_F_3] = REINITIALISATION_OF_relevant_evt_OF_F_3;
	boolState[required_OF_F_3_1] = REINITIALISATION_OF_required_OF_F_3_1;
	boolState[S_OF_F_3_1] = REINITIALISATION_OF_S_OF_F_3_1;
	boolState[relevant_evt_OF_F_3_1] = REINITIALISATION_OF_relevant_evt_OF_F_3_1;
	boolState[required_OF_F_4] = REINITIALISATION_OF_required_OF_F_4;
	boolState[S_OF_F_4] = REINITIALISATION_OF_S_OF_F_4;
	boolState[relevant_evt_OF_F_4] = REINITIALISATION_OF_relevant_evt_OF_F_4;
	boolState[required_OF_I_1] = REINITIALISATION_OF_required_OF_I_1;
	boolState[S_OF_I_1] = REINITIALISATION_OF_S_OF_I_1;
	boolState[relevant_evt_OF_I_1] = REINITIALISATION_OF_relevant_evt_OF_I_1;
	boolState[to_be_fired_OF_I_1] = REINITIALISATION_OF_to_be_fired_OF_I_1;
	boolState[required_OF_Major_risk] = REINITIALISATION_OF_required_OF_Major_risk;
	boolState[S_OF_Major_risk] = REINITIALISATION_OF_S_OF_Major_risk;
	boolState[relevant_evt_OF_Major_risk] = REINITIALISATION_OF_relevant_evt_OF_Major_risk;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
}

void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_F_1 = true;
	}

	if (numFire == 1)
	{
		FIRE_xx11_OF_F_1 = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_F_2 = true;
	}

	if (numFire == 3)
	{
		FIRE_xx11_OF_F_2 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_F_3 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx11_OF_F_3 = true;
	}

	if (numFire == 6)
	{
		FIRE_xx10_OF_F_3_1 = true;
	}

	if (numFire == 7)
	{
		FIRE_xx11_OF_F_3_1 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx10_OF_F_4 = true;
	}

	if (numFire == 9)
	{
		FIRE_xx11_OF_F_4 = true;
	}

	if (numFire == 10)
	{
		FIRE_xx23_OF_I_1_INS_10 = true;
	}

	if (numFire == 11)
	{
		FIRE_xx23_OF_I_1_INS_11 = true;
	}

	if (numFire == 12)
	{
		FIRE_xx24_OF_I_1 = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_F_1
	if ((boolState[failF_OF_F_1] == false) && (boolState[required_OF_F_1] && boolState[relevant_evt_OF_F_1])) 
	{
		 
		if (FIRE_xx10_OF_F_1)
		{
			boolState[failF_OF_F_1]  =  true;
			FIRE_xx10_OF_F_1 = false;
		}
	}

	// Occurrence xx11_OF_F_1
	if (boolState[failF_OF_F_1] == true) 
	{
		 
		if (FIRE_xx11_OF_F_1)
		{
			boolState[failF_OF_F_1]  =  false;
			FIRE_xx11_OF_F_1 = false;
		}
	}

	// Occurrence xx10_OF_F_2
	if ((boolState[failF_OF_F_2] == false) && (boolState[required_OF_F_2] && boolState[relevant_evt_OF_F_2])) 
	{
		 
		if (FIRE_xx10_OF_F_2)
		{
			boolState[failF_OF_F_2]  =  true;
			FIRE_xx10_OF_F_2 = false;
		}
	}

	// Occurrence xx11_OF_F_2
	if (boolState[failF_OF_F_2] == true) 
	{
		 
		if (FIRE_xx11_OF_F_2)
		{
			boolState[failF_OF_F_2]  =  false;
			FIRE_xx11_OF_F_2 = false;
		}
	}

	// Occurrence xx10_OF_F_3
	if ((boolState[failF_OF_F_3] == false) && (boolState[required_OF_F_3] && boolState[relevant_evt_OF_F_3])) 
	{
		 
		if (FIRE_xx10_OF_F_3)
		{
			boolState[failF_OF_F_3]  =  true;
			FIRE_xx10_OF_F_3 = false;
		}
	}

	// Occurrence xx11_OF_F_3
	if (boolState[failF_OF_F_3] == true) 
	{
		 
		if (FIRE_xx11_OF_F_3)
		{
			boolState[failF_OF_F_3]  =  false;
			FIRE_xx11_OF_F_3 = false;
		}
	}

	// Occurrence xx10_OF_F_3_1
	if ((boolState[failF_OF_F_3_1] == false) && (boolState[required_OF_F_3_1] &&  boolState[relevant_evt_OF_F_3_1])) 
	{
		 
		if (FIRE_xx10_OF_F_3_1)
		{
			boolState[failF_OF_F_3_1]  =  true;
			FIRE_xx10_OF_F_3_1 = false;
		}
	}

	// Occurrence xx11_OF_F_3_1
	if (boolState[failF_OF_F_3_1] == true) 
	{
		 
		if (FIRE_xx11_OF_F_3_1)
		{
			boolState[failF_OF_F_3_1]  =  false;
			FIRE_xx11_OF_F_3_1 = false;
		}
	}

	// Occurrence xx10_OF_F_4
	if ((boolState[failF_OF_F_4] == false) && (boolState[required_OF_F_4] && boolState[relevant_evt_OF_F_4])) 
	{
		 
		if (FIRE_xx10_OF_F_4)
		{
			boolState[failF_OF_F_4]  =  true;
			FIRE_xx10_OF_F_4 = false;
		}
	}

	// Occurrence xx11_OF_F_4
	if (boolState[failF_OF_F_4] == true) 
	{
		 
		if (FIRE_xx11_OF_F_4)
		{
			boolState[failF_OF_F_4]  =  false;
			FIRE_xx11_OF_F_4 = false;
		}
	}

	// Occurrence xx23_OF_I_1

	if ((boolState[failI_OF_I_1] == false) && (boolState[to_be_fired_OF_I_1] &&
	boolState[relevant_evt_OF_I_1])) 
	{
	
		
		if (FIRE_xx23_OF_I_1_INS_10) 
		{
			boolState[failI_OF_I_1]  =  true;
			boolState[already_standby_OF_I_1]  =  false;
			boolState[already_required_OF_I_1]  =  false;
			FIRE_xx23_OF_I_1_INS_10 = false;
		}
	
	}
	if ((boolState[failI_OF_I_1] == false) && (boolState[to_be_fired_OF_I_1] &&
	boolState[relevant_evt_OF_I_1])) 
	{
	
		
		if (FIRE_xx23_OF_I_1_INS_11) 
		{
			boolState[already_standby_OF_I_1]  =  false;
			boolState[already_required_OF_I_1]  =  false;
			FIRE_xx23_OF_I_1_INS_11 = false;
		}
	
	}
	// Occurrence xx24_OF_I_1
	if (boolState[failI_OF_I_1] == true) 
	{
		 
		if (FIRE_xx24_OF_I_1)
		{
			boolState[failI_OF_I_1]  =  false;
			FIRE_xx24_OF_I_1 = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if ((boolState[failI_OF_I_1] == false) && (boolState[to_be_fired_OF_I_1] &&	boolState[relevant_evt_OF_I_1]))
	{
		cout << "10 :  INS_SUB_COUNT (1) |FAULT | failI  LABEL \"instantaneous failure I_1\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_I_1]  =  TRUE,already_standby_OF_I_1  =  FALSE,already_required_OF_I_1  =  FALSE" << endl;
		list.push_back(make_tuple(10, 0.0001, "INS", 1));
	}
	if ((boolState[failI_OF_I_1] == false) && (boolState[to_be_fired_OF_I_1] &&	boolState[relevant_evt_OF_I_1]))
	{
		cout << "11 :  INS_SUB_COUNT (1) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_I_1]  =  FALSE,already_required_OF_I_1  =  FALSE" << endl;
		list.push_back(make_tuple(11, 0.9999, "INS", 1));
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
     
	if ((boolState[failF_OF_F_1] == false) && (boolState[required_OF_F_1] && boolState[relevant_evt_OF_F_1]))
	{
		cout << "0 : xx10_OF_F_1 : FAULT failF  LABEL \"failure in operation F_1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_F_1]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_F_1] == true)
	{
		cout << "1 : xx11_OF_F_1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_F_1]  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_F_2] == false) && (boolState[required_OF_F_2] && boolState[relevant_evt_OF_F_2]))
	{
		cout << "2 : xx10_OF_F_2 : FAULT failF  LABEL \"failure in operation F_2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_F_2]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_F_2] == true)
	{
		cout << "3 : xx11_OF_F_2 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_F_2]  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_F_3] == false) && (boolState[required_OF_F_3] && boolState[relevant_evt_OF_F_3]))
	{
		cout << "4 : xx10_OF_F_3 : FAULT failF  LABEL \"failure in operation F_3\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_F_3]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_F_3] == true)
	{
		cout << "5 : xx11_OF_F_3 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_F_3]  =  FALSE" << endl;
		list.push_back(make_tuple(5, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_F_3_1] == false) && (boolState[required_OF_F_3_1] && boolState[relevant_evt_OF_F_3_1]))
	{
		cout << "6 : xx10_OF_F_3_1 : FAULT failF  LABEL \"failure in operation F_3_1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_F_3_1]  =  TRUE" << endl;
		list.push_back(make_tuple(6, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_F_3_1] == true)
	{
		cout << "7 : xx11_OF_F_3_1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_F_3_1]  =  FALSE" << endl;
		list.push_back(make_tuple(7, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_F_4] == false) && (boolState[required_OF_F_4] && boolState[relevant_evt_OF_F_4]))
	{
		cout << "8 : xx10_OF_F_4 : FAULT failF  LABEL \"failure in operation F_4\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_F_4]  =  TRUE" << endl;
		list.push_back(make_tuple(8, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_F_4] == true)
	{
		cout << "9 : xx11_OF_F_4 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_F_4]  =  FALSE" << endl;
		list.push_back(make_tuple(9, 0.1, "EXP", 0));
	}
	if (boolState[failI_OF_I_1] == true)
	{
		cout << "12 : xx24_OF_I_1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failI_OF_I_1]  =  FALSE" << endl;
		list.push_back(make_tuple(12, 0.1, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_F_1] == true )
	{
		boolState[S_OF_F_1]  =  true;
	}

	if (boolState[failF_OF_F_2] == true )
	{
		boolState[S_OF_F_2]  =  true;
	}

	if (boolState[failF_OF_F_3] == true )
	{
		boolState[S_OF_F_3]  =  true;
	}

	if (boolState[failF_OF_F_3_1] == true )
	{
		boolState[S_OF_F_3_1]  =  true;
	}

	if (boolState[failF_OF_F_4] == true )
	{
		boolState[S_OF_F_4]  =  true;
	}

	if (boolState[failI_OF_I_1] == true )
	{
		boolState[S_OF_I_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_F_3] && boolState[S_OF_F_4] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (boolState[S_OF_F_3_1] && boolState[S_OF_I_1] )
	{
		boolState[S_OF_AND_1_1]  =  true;
	}

	if (((boolState[S_OF_AND_1] || boolState[S_OF_AND_1_1]) || boolState[S_OF_F_1]) || boolState[S_OF_F_2] )
	{
		boolState[S_OF_Major_risk]  =  true;
	}

	if (boolState[S_OF_Major_risk] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_Major_risk] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if ((boolState[relevant_evt_OF_Major_risk] && ( !boolState[S_OF_Major_risk])) || (  boolState[relevant_evt_OF_AND_1_1] && ( !boolState[S_OF_AND_1_1])) )
	{
		boolState[relevant_evt_OF_AND_1]  =  true;
	}

	if (( !boolState[required_OF_Major_risk]) || ( !boolState[S_OF_AND_1]) )
	{
		boolState[required_OF_AND_1_1]  =  false;
	}

	if (boolState[relevant_evt_OF_Major_risk] && ( !boolState[S_OF_Major_risk]) )
	{
		boolState[relevant_evt_OF_AND_1_1]  =  true;
	}

	if ( !boolState[required_OF_Major_risk] )
	{
		boolState[required_OF_F_1]  =  false;
	}

	if (boolState[relevant_evt_OF_Major_risk] && ( !boolState[S_OF_Major_risk]) )
	{
		boolState[relevant_evt_OF_F_1]  =  true;
	}

	if ( !boolState[required_OF_Major_risk] )
	{
		boolState[required_OF_F_2]  =  false;
	}

	if (boolState[relevant_evt_OF_Major_risk] && ( !boolState[S_OF_Major_risk]) )
	{
		boolState[relevant_evt_OF_F_2]  =  true;
	}

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_F_3]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1])) || (  boolState[relevant_evt_OF_F_4] && ( !boolState[S_OF_F_4])) )
	{
		boolState[relevant_evt_OF_F_3]  =  true;
	}

	if ( !boolState[required_OF_AND_1_1] )
	{
		boolState[required_OF_F_3_1]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_1_1] && ( !boolState[S_OF_AND_1_1])) || (  boolState[relevant_evt_OF_I_1] && ( !boolState[S_OF_I_1])) )
	{
		boolState[relevant_evt_OF_F_3_1]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_F_3]) )
	{
		boolState[required_OF_F_4]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_F_4]  =  true;
	}

	if (( !boolState[required_OF_AND_1_1]) || ( !boolState[S_OF_F_3_1]) )
	{
		boolState[required_OF_I_1]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1_1] && ( !boolState[S_OF_AND_1_1]) )
	{
		boolState[relevant_evt_OF_I_1]  =  true;
	}

	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_Major_risk]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_Major_risk]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

}


void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_AND_1_1]  =  boolState[S_OF_AND_1_1]  ;



	boolState[already_S_OF_F_1]  =  boolState[S_OF_F_1]  ;



	boolState[already_S_OF_F_2]  =  boolState[S_OF_F_2]  ;



	boolState[already_S_OF_F_3]  =  boolState[S_OF_F_3]  ;



	boolState[already_S_OF_F_3_1]  =  boolState[S_OF_F_3_1]  ;



	boolState[already_S_OF_F_4]  =  boolState[S_OF_F_4]  ;



	boolState[already_S_OF_I_1]  =  boolState[S_OF_I_1]  ;

	if (( !boolState[required_OF_I_1]) && (( !boolState[already_standby_OF_I_1]) && (   !boolState[already_required_OF_I_1])) )
	{
		boolState[already_standby_OF_I_1]  =  true;
	}



	boolState[already_S_OF_Major_risk]  =  boolState[S_OF_Major_risk]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;

}


void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::runOnceInteractionStep_tops()
{
	if (boolState[required_OF_I_1] && boolState[already_standby_OF_I_1] )
	{
		boolState[to_be_fired_OF_I_1]  =  true;
	}

}

void
storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_10_Project_risks_Trim_Max_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    