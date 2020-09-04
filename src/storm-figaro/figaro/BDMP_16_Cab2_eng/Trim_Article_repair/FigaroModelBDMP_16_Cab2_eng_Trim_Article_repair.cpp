#include <iostream>
#include "FigaroModelBDMP_16_Cab2_eng_Trim_Article_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::init()
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
	REINITIALISATION_OF_required_OF_C1 = true;
	boolState[already_S_OF_C1] = false;
	REINITIALISATION_OF_S_OF_C1 = false;
	REINITIALISATION_OF_relevant_evt_OF_C1 = false;
	boolState[failF_OF_C1] = false;
	REINITIALISATION_OF_required_OF_D1 = true;
	boolState[already_S_OF_D1] = false;
	REINITIALISATION_OF_S_OF_D1 = false;
	REINITIALISATION_OF_relevant_evt_OF_D1 = false;
	boolState[failF_OF_D1] = false;
	REINITIALISATION_OF_required_OF_E1 = true;
	boolState[already_S_OF_E1] = false;
	REINITIALISATION_OF_S_OF_E1 = false;
	REINITIALISATION_OF_relevant_evt_OF_E1 = false;
	boolState[failF_OF_E1] = false;
	REINITIALISATION_OF_required_OF_OR_1 = true;
	boolState[already_S_OF_OR_1] = false;
	REINITIALISATION_OF_S_OF_OR_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_1 = false;
	REINITIALISATION_OF_required_OF_OR_2 = true;
	boolState[already_S_OF_OR_2] = false;
	REINITIALISATION_OF_S_OF_OR_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_2 = false;
	REINITIALISATION_OF_required_OF_OR_3 = true;
	boolState[already_S_OF_OR_3] = false;
	REINITIALISATION_OF_S_OF_OR_3 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_3 = false;
	REINITIALISATION_OF_required_OF_OR_4 = true;
	boolState[already_S_OF_OR_4] = false;
	REINITIALISATION_OF_S_OF_OR_4 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_4 = false;
	REINITIALISATION_OF_required_OF_SF_A = true;
	boolState[already_S_OF_SF_A] = false;
	REINITIALISATION_OF_S_OF_SF_A = false;
	REINITIALISATION_OF_relevant_evt_OF_SF_A = false;
	boolState[failF_OF_SF_A] = false;
	boolState[failS_OF_SF_A] = false;
	REINITIALISATION_OF_required_OF_SF_B = true;
	boolState[already_S_OF_SF_B] = false;
	REINITIALISATION_OF_S_OF_SF_B = false;
	REINITIALISATION_OF_relevant_evt_OF_SF_B = false;
	boolState[failF_OF_SF_B] = false;
	boolState[failS_OF_SF_B] = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_C1 = false;
	FIRE_xx11_OF_C1 = false;
	FIRE_xx10_OF_D1 = false;
	FIRE_xx11_OF_D1 = false;
	FIRE_xx10_OF_E1 = false;
	FIRE_xx11_OF_E1 = false;
	FIRE_xx17_OF_SF_A = false;
	FIRE_xx18_OF_SF_A = false;
	FIRE_xx19_OF_SF_A = false;
	FIRE_xx17_OF_SF_B = false;
	FIRE_xx18_OF_SF_B = false;
	FIRE_xx19_OF_SF_B = false;

}

void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::printState()
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
	cout << "Attribute :  boolState[required_OF_C1] | Value : " << boolState[required_OF_C1] << endl;
	cout << "Attribute :  boolState[already_S_OF_C1] | Value : " << boolState[already_S_OF_C1] << endl;
	cout << "Attribute :  boolState[S_OF_C1] | Value : " << boolState[S_OF_C1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_C1] | Value : " << boolState[relevant_evt_OF_C1] << endl;
	cout << "Attribute :  boolState[failF_OF_C1] | Value : " << boolState[failF_OF_C1] << endl;
	cout << "Attribute :  boolState[required_OF_D1] | Value : " << boolState[required_OF_D1] << endl;
	cout << "Attribute :  boolState[already_S_OF_D1] | Value : " << boolState[already_S_OF_D1] << endl;
	cout << "Attribute :  boolState[S_OF_D1] | Value : " << boolState[S_OF_D1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_D1] | Value : " << boolState[relevant_evt_OF_D1] << endl;
	cout << "Attribute :  boolState[failF_OF_D1] | Value : " << boolState[failF_OF_D1] << endl;
	cout << "Attribute :  boolState[required_OF_E1] | Value : " << boolState[required_OF_E1] << endl;
	cout << "Attribute :  boolState[already_S_OF_E1] | Value : " << boolState[already_S_OF_E1] << endl;
	cout << "Attribute :  boolState[S_OF_E1] | Value : " << boolState[S_OF_E1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_E1] | Value : " << boolState[relevant_evt_OF_E1] << endl;
	cout << "Attribute :  boolState[failF_OF_E1] | Value : " << boolState[failF_OF_E1] << endl;
	cout << "Attribute :  boolState[required_OF_OR_1] | Value : " << boolState[required_OF_OR_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_1] | Value : " << boolState[already_S_OF_OR_1] << endl;
	cout << "Attribute :  boolState[S_OF_OR_1] | Value : " << boolState[S_OF_OR_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_1] | Value : " << boolState[relevant_evt_OF_OR_1] << endl;
	cout << "Attribute :  boolState[required_OF_OR_2] | Value : " << boolState[required_OF_OR_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_2] | Value : " << boolState[already_S_OF_OR_2] << endl;
	cout << "Attribute :  boolState[S_OF_OR_2] | Value : " << boolState[S_OF_OR_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_2] | Value : " << boolState[relevant_evt_OF_OR_2] << endl;
	cout << "Attribute :  boolState[required_OF_OR_3] | Value : " << boolState[required_OF_OR_3] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_3] | Value : " << boolState[already_S_OF_OR_3] << endl;
	cout << "Attribute :  boolState[S_OF_OR_3] | Value : " << boolState[S_OF_OR_3] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_3] | Value : " << boolState[relevant_evt_OF_OR_3] << endl;
	cout << "Attribute :  boolState[required_OF_OR_4] | Value : " << boolState[required_OF_OR_4] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_4] | Value : " << boolState[already_S_OF_OR_4] << endl;
	cout << "Attribute :  boolState[S_OF_OR_4] | Value : " << boolState[S_OF_OR_4] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_4] | Value : " << boolState[relevant_evt_OF_OR_4] << endl;
	cout << "Attribute :  boolState[required_OF_SF_A] | Value : " << boolState[required_OF_SF_A] << endl;
	cout << "Attribute :  boolState[already_S_OF_SF_A] | Value : " << boolState[already_S_OF_SF_A] << endl;
	cout << "Attribute :  boolState[S_OF_SF_A] | Value : " << boolState[S_OF_SF_A] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_SF_A] | Value : " << boolState[relevant_evt_OF_SF_A] << endl;
	cout << "Attribute :  boolState[failF_OF_SF_A] | Value : " << boolState[failF_OF_SF_A] << endl;
	cout << "Attribute :  boolState[failS_OF_SF_A] | Value : " << boolState[failS_OF_SF_A] << endl;
	cout << "Attribute :  boolState[required_OF_SF_B] | Value : " << boolState[required_OF_SF_B] << endl;
	cout << "Attribute :  boolState[already_S_OF_SF_B] | Value : " << boolState[already_S_OF_SF_B] << endl;
	cout << "Attribute :  boolState[S_OF_SF_B] | Value : " << boolState[S_OF_SF_B] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_SF_B] | Value : " << boolState[relevant_evt_OF_SF_B] << endl;
	cout << "Attribute :  boolState[failF_OF_SF_B] | Value : " << boolState[failF_OF_SF_B] << endl;
	cout << "Attribute :  boolState[failS_OF_SF_B] | Value : " << boolState[failS_OF_SF_B] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::figaromodelhasinstransitions()
{
	return false;
}
void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_AND_2] = REINITIALISATION_OF_required_OF_AND_2;
	boolState[S_OF_AND_2] = REINITIALISATION_OF_S_OF_AND_2;
	boolState[relevant_evt_OF_AND_2] = REINITIALISATION_OF_relevant_evt_OF_AND_2;
	boolState[required_OF_C1] = REINITIALISATION_OF_required_OF_C1;
	boolState[S_OF_C1] = REINITIALISATION_OF_S_OF_C1;
	boolState[relevant_evt_OF_C1] = REINITIALISATION_OF_relevant_evt_OF_C1;
	boolState[required_OF_D1] = REINITIALISATION_OF_required_OF_D1;
	boolState[S_OF_D1] = REINITIALISATION_OF_S_OF_D1;
	boolState[relevant_evt_OF_D1] = REINITIALISATION_OF_relevant_evt_OF_D1;
	boolState[required_OF_E1] = REINITIALISATION_OF_required_OF_E1;
	boolState[S_OF_E1] = REINITIALISATION_OF_S_OF_E1;
	boolState[relevant_evt_OF_E1] = REINITIALISATION_OF_relevant_evt_OF_E1;
	boolState[required_OF_OR_1] = REINITIALISATION_OF_required_OF_OR_1;
	boolState[S_OF_OR_1] = REINITIALISATION_OF_S_OF_OR_1;
	boolState[relevant_evt_OF_OR_1] = REINITIALISATION_OF_relevant_evt_OF_OR_1;
	boolState[required_OF_OR_2] = REINITIALISATION_OF_required_OF_OR_2;
	boolState[S_OF_OR_2] = REINITIALISATION_OF_S_OF_OR_2;
	boolState[relevant_evt_OF_OR_2] = REINITIALISATION_OF_relevant_evt_OF_OR_2;
	boolState[required_OF_OR_3] = REINITIALISATION_OF_required_OF_OR_3;
	boolState[S_OF_OR_3] = REINITIALISATION_OF_S_OF_OR_3;
	boolState[relevant_evt_OF_OR_3] = REINITIALISATION_OF_relevant_evt_OF_OR_3;
	boolState[required_OF_OR_4] = REINITIALISATION_OF_required_OF_OR_4;
	boolState[S_OF_OR_4] = REINITIALISATION_OF_S_OF_OR_4;
	boolState[relevant_evt_OF_OR_4] = REINITIALISATION_OF_relevant_evt_OF_OR_4;
	boolState[required_OF_SF_A] = REINITIALISATION_OF_required_OF_SF_A;
	boolState[S_OF_SF_A] = REINITIALISATION_OF_S_OF_SF_A;
	boolState[relevant_evt_OF_SF_A] = REINITIALISATION_OF_relevant_evt_OF_SF_A;
	boolState[required_OF_SF_B] = REINITIALISATION_OF_required_OF_SF_B;
	boolState[S_OF_SF_B] = REINITIALISATION_OF_S_OF_SF_B;
	boolState[relevant_evt_OF_SF_B] = REINITIALISATION_OF_relevant_evt_OF_SF_B;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
}

void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_C1 = true;
	}

	if (numFire == 1)
	{
		FIRE_xx11_OF_C1 = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_D1 = true;
	}

	if (numFire == 3)
	{
		FIRE_xx11_OF_D1 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_E1 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx11_OF_E1 = true;
	}

	if (numFire == 6)
	{
		FIRE_xx17_OF_SF_A = true;
	}

	if (numFire == 7)
	{
		FIRE_xx18_OF_SF_A = true;
	}

	if (numFire == 8)
	{
		FIRE_xx19_OF_SF_A = true;
	}

	if (numFire == 9)
	{
		FIRE_xx17_OF_SF_B = true;
	}

	if (numFire == 10)
	{
		FIRE_xx18_OF_SF_B = true;
	}

	if (numFire == 11)
	{
		FIRE_xx19_OF_SF_B = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_C1
	if ((boolState[failF_OF_C1] == false) && (boolState[required_OF_C1] && boolState[relevant_evt_OF_C1])) 
	{
		 
		if (FIRE_xx10_OF_C1)
		{
			boolState[failF_OF_C1]  =  true;
			FIRE_xx10_OF_C1 = false;
		}
	}

	// Occurrence xx11_OF_C1
	if (boolState[failF_OF_C1] == true) 
	{
		 
		if (FIRE_xx11_OF_C1)
		{
			boolState[failF_OF_C1]  =  false;
			FIRE_xx11_OF_C1 = false;
		}
	}

	// Occurrence xx10_OF_D1
	if ((boolState[failF_OF_D1] == false) && (boolState[required_OF_D1] && boolState[relevant_evt_OF_D1])) 
	{
		 
		if (FIRE_xx10_OF_D1)
		{
			boolState[failF_OF_D1]  =  true;
			FIRE_xx10_OF_D1 = false;
		}
	}

	// Occurrence xx11_OF_D1
	if (boolState[failF_OF_D1] == true) 
	{
		 
		if (FIRE_xx11_OF_D1)
		{
			boolState[failF_OF_D1]  =  false;
			FIRE_xx11_OF_D1 = false;
		}
	}

	// Occurrence xx10_OF_E1
	if ((boolState[failF_OF_E1] == false) && (boolState[required_OF_E1] && boolState[relevant_evt_OF_E1])) 
	{
		 
		if (FIRE_xx10_OF_E1)
		{
			boolState[failF_OF_E1]  =  true;
			FIRE_xx10_OF_E1 = false;
		}
	}

	// Occurrence xx11_OF_E1
	if (boolState[failF_OF_E1] == true) 
	{
		 
		if (FIRE_xx11_OF_E1)
		{
			boolState[failF_OF_E1]  =  false;
			FIRE_xx11_OF_E1 = false;
		}
	}

	// Occurrence xx17_OF_SF_A
	if ((boolState[failF_OF_SF_A] == false) && ((boolState[required_OF_SF_A] &&  boolState[relevant_evt_OF_SF_A]) && ( !boolState[failS_OF_SF_A]))) 
	{
		 
		if (FIRE_xx17_OF_SF_A)
		{
			boolState[failF_OF_SF_A]  =  true;
			FIRE_xx17_OF_SF_A = false;
		}
	}

	// Occurrence xx18_OF_SF_A
	if ((boolState[failS_OF_SF_A] == false) && ((( !boolState[required_OF_SF_A]) &&  boolState[relevant_evt_OF_SF_A]) && ( !boolState[failF_OF_SF_A]))) 
	{
		 
		if (FIRE_xx18_OF_SF_A)
		{
			boolState[failS_OF_SF_A]  =  true;
			FIRE_xx18_OF_SF_A = false;
		}
	}

	// Occurrence xx19_OF_SF_A
	if ((boolState[failS_OF_SF_A] == true) || (boolState[failF_OF_SF_A] == true)) 
	{
		 
		if (FIRE_xx19_OF_SF_A)
		{
			boolState[failS_OF_SF_A]  =  false;
			boolState[failF_OF_SF_A]  =  false;
			FIRE_xx19_OF_SF_A = false;
		}
	}

	// Occurrence xx17_OF_SF_B
	if ((boolState[failF_OF_SF_B] == false) && ((boolState[required_OF_SF_B] &&  boolState[relevant_evt_OF_SF_B]) && ( !boolState[failS_OF_SF_B]))) 
	{
		 
		if (FIRE_xx17_OF_SF_B)
		{
			boolState[failF_OF_SF_B]  =  true;
			FIRE_xx17_OF_SF_B = false;
		}
	}

	// Occurrence xx18_OF_SF_B
	if ((boolState[failS_OF_SF_B] == false) && ((( !boolState[required_OF_SF_B]) &&  boolState[relevant_evt_OF_SF_B]) && ( !boolState[failF_OF_SF_B]))) 
	{
		 
		if (FIRE_xx18_OF_SF_B)
		{
			boolState[failS_OF_SF_B]  =  true;
			FIRE_xx18_OF_SF_B = false;
		}
	}

	// Occurrence xx19_OF_SF_B
	if ((boolState[failS_OF_SF_B] == true) || (boolState[failF_OF_SF_B] == true)) 
	{
		 
		if (FIRE_xx19_OF_SF_B)
		{
			boolState[failS_OF_SF_B]  =  false;
			boolState[failF_OF_SF_B]  =  false;
			FIRE_xx19_OF_SF_B = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::showFireableOccurrences()
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
     
	if ((boolState[failF_OF_C1] == false) && (boolState[required_OF_C1] && boolState[relevant_evt_OF_C1]))
	{
		cout << "0 : xx10_OF_C1 : FAULT failF  LABEL \"failure in operation C1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_C1]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_C1] == true)
	{
		cout << "1 : xx11_OF_C1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_C1]  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_D1] == false) && (boolState[required_OF_D1] && boolState[relevant_evt_OF_D1]))
	{
		cout << "2 : xx10_OF_D1 : FAULT failF  LABEL \"failure in operation D1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_D1]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_D1] == true)
	{
		cout << "3 : xx11_OF_D1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_D1]  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_E1] == false) && (boolState[required_OF_E1] && boolState[relevant_evt_OF_E1]))
	{
		cout << "4 : xx10_OF_E1 : FAULT failF  LABEL \"failure in operation E1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_E1]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_E1] == true)
	{
		cout << "5 : xx11_OF_E1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_E1]  =  FALSE" << endl;
		list.push_back(make_tuple(5, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_SF_A] == false) && ((boolState[required_OF_SF_A] && boolState[relevant_evt_OF_SF_A]) && ( !boolState[failS_OF_SF_A])))
	{
		cout << "6 : xx17_OF_SF_A : FAULT failF  LABEL \"failure in operation SF_A\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_SF_A]  =  TRUE" << endl;
		list.push_back(make_tuple(6, 0.0001, "EXP", 0));
	}
	if ((boolState[failS_OF_SF_A] == false) && ((( !boolState[required_OF_SF_A]) && boolState[relevant_evt_OF_SF_A]) && ( !boolState[failF_OF_SF_A])))
	{
		cout << "7 : xx18_OF_SF_A : FAULT failS  LABEL \"standby failure SF_A\"  DIST EXP (1e-05)  INDUCING boolState[failS_OF_SF_A]  =  TRUE" << endl;
		list.push_back(make_tuple(7, 1e-05, "EXP", 0));
	}
	if ((boolState[failS_OF_SF_A] == true) || (boolState[failF_OF_SF_A] == true))
	{
		cout << "8 : xx19_OF_SF_A : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failS_OF_SF_A]  =  FALSE,failF_OF_SF_A  =  FALSE" << endl;
		list.push_back(make_tuple(8, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_SF_B] == false) && ((boolState[required_OF_SF_B] && boolState[relevant_evt_OF_SF_B]) && ( !boolState[failS_OF_SF_B])))
	{
		cout << "9 : xx17_OF_SF_B : FAULT failF  LABEL \"failure in operation SF_B\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_SF_B]  =  TRUE" << endl;
		list.push_back(make_tuple(9, 0.0001, "EXP", 0));
	}
	if ((boolState[failS_OF_SF_B] == false) && ((( !boolState[required_OF_SF_B]) && boolState[relevant_evt_OF_SF_B]) && ( !boolState[failF_OF_SF_B])))
	{
		cout << "10 : xx18_OF_SF_B : FAULT failS  LABEL \"standby failure SF_B\"  DIST EXP (1e-05)  INDUCING boolState[failS_OF_SF_B]  =  TRUE" << endl;
		list.push_back(make_tuple(10, 1e-05, "EXP", 0));
	}
	if ((boolState[failS_OF_SF_B] == true) || (boolState[failF_OF_SF_B] == true))
	{
		cout << "11 : xx19_OF_SF_B : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failS_OF_SF_B]  =  FALSE,failF_OF_SF_B  =  FALSE" << endl;
		list.push_back(make_tuple(11, 0.1, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_C1] == true )
	{
		boolState[S_OF_C1]  =  true;
	}

	if (boolState[failF_OF_D1] == true )
	{
		boolState[S_OF_D1]  =  true;
	}

	if (boolState[failF_OF_E1] == true )
	{
		boolState[S_OF_E1]  =  true;
	}

	if ((boolState[failS_OF_SF_A] == true) || (boolState[failF_OF_SF_A] == true) )
	{
		boolState[S_OF_SF_A]  =  true;
	}

	if ((boolState[failS_OF_SF_B] == true) || (boolState[failF_OF_SF_B] == true) )
	{
		boolState[S_OF_SF_B]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_OR_1] && boolState[S_OF_OR_2] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (boolState[S_OF_OR_3] && boolState[S_OF_SF_A] )
	{
		boolState[S_OF_AND_2]  =  true;
	}

	if (boolState[S_OF_AND_2] || boolState[S_OF_E1] )
	{
		boolState[S_OF_OR_1]  =  true;
	}

	if (boolState[S_OF_C1] || boolState[S_OF_SF_B] )
	{
		boolState[S_OF_OR_2]  =  true;
	}

	if (boolState[S_OF_C1] || boolState[S_OF_D1] )
	{
		boolState[S_OF_OR_3]  =  true;
	}

	if (boolState[S_OF_D1] || boolState[S_OF_E1] )
	{
		boolState[S_OF_OR_4]  =  true;
	}

	if (boolState[S_OF_AND_1] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::runOnceInteractionStep_propagate_effect_required()
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

	if (( !boolState[required_OF_OR_2]) && ( !boolState[required_OF_OR_3]) )
	{
		boolState[required_OF_C1]  =  false;
	}

	if ((boolState[relevant_evt_OF_OR_2] && ( !boolState[S_OF_OR_2])) || (boolState[relevant_evt_OF_OR_3] && ( !boolState[S_OF_OR_3])) )
	{
		boolState[relevant_evt_OF_C1]  =  true;
	}

	if (( !boolState[required_OF_OR_3]) && ( !boolState[required_OF_OR_4]) )
	{
		boolState[required_OF_D1]  =  false;
	}

	if ((boolState[relevant_evt_OF_OR_3] && ( !boolState[S_OF_OR_3])) || (boolState[relevant_evt_OF_OR_4] && ( !boolState[S_OF_OR_4])) )
	{
		boolState[relevant_evt_OF_D1]  =  true;
	}

	if (( !boolState[required_OF_OR_1]) && ( !boolState[required_OF_OR_4]) )
	{
		boolState[required_OF_E1]  =  false;
	}

	if ((boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1])) || (boolState[relevant_evt_OF_OR_4] && ( !boolState[S_OF_OR_4])) )
	{
		boolState[relevant_evt_OF_E1]  =  true;
	}

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_OR_1]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_OR_1]  =  true;
	}

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_OR_2]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_OR_2]  =  true;
	}

	if ( !boolState[required_OF_AND_2] )
	{
		boolState[required_OF_OR_3]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_2] && ( !boolState[S_OF_AND_2])) || ( !boolState[S_OF_SF_A]) )
	{
		boolState[relevant_evt_OF_OR_3]  =  true;
	}

	if ( !boolState[S_OF_SF_B] )
	{
		boolState[relevant_evt_OF_OR_4]  =  true;
	}

	if (( !boolState[required_OF_AND_2]) || ( !boolState[S_OF_OR_3]) )
	{
		boolState[required_OF_SF_A]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_2] && ( !boolState[S_OF_AND_2]) )
	{
		boolState[relevant_evt_OF_SF_A]  =  true;
	}

	if (( !boolState[required_OF_OR_2]) || ( !boolState[S_OF_OR_4]) )
	{
		boolState[required_OF_SF_B]  =  false;
	}

	if (boolState[relevant_evt_OF_OR_2] && ( !boolState[S_OF_OR_2]) )
	{
		boolState[relevant_evt_OF_SF_B]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

}


void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_AND_2]  =  boolState[S_OF_AND_2]  ;



	boolState[already_S_OF_C1]  =  boolState[S_OF_C1]  ;



	boolState[already_S_OF_D1]  =  boolState[S_OF_D1]  ;



	boolState[already_S_OF_E1]  =  boolState[S_OF_E1]  ;



	boolState[already_S_OF_OR_1]  =  boolState[S_OF_OR_1]  ;



	boolState[already_S_OF_OR_2]  =  boolState[S_OF_OR_2]  ;



	boolState[already_S_OF_OR_3]  =  boolState[S_OF_OR_3]  ;



	boolState[already_S_OF_OR_4]  =  boolState[S_OF_OR_4]  ;



	boolState[already_S_OF_SF_A]  =  boolState[S_OF_SF_A]  ;



	boolState[already_S_OF_SF_B]  =  boolState[S_OF_SF_B]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;

}

void
storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_16_Cab2_eng_Trim_Article_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    