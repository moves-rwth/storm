#include <iostream>
#include "FigaroModelfigaro_Telecom.h"

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
        
void storm::figaro::FigaroProgram_figaro_Telecom::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	intState[nb_failures_OF_Failure_counter] = 0;
	boolState[fail_OF_Node_1] = false;
	REINITIALISATION_OF_connected_OF_Node_1 = false;
	boolState[fail_OF_Node_2] = false;
	REINITIALISATION_OF_connected_OF_Node_2 = false;
	boolState[interruption_OF_ud_1] = false;
	boolState[interruption_OF_bidir_3] = false;
	boolState[fail_OF_Node_6] = false;
	REINITIALISATION_OF_connected_OF_Node_6 = false;
	boolState[fail_OF_Node_8] = false;
	REINITIALISATION_OF_connected_OF_Node_8 = false;
	boolState[fail_OF_Source] = false;
	REINITIALISATION_OF_connected_OF_Source = false;
	boolState[fail_OF_Target] = false;
	REINITIALISATION_OF_connected_OF_Target = false;
	boolState[interruption_OF_ud_3] = false;
	boolState[interruption_OF_bidir_7] = false;
	boolState[interruption_OF_bidir_12] = false;
	boolState[interruption_OF_bidir_2] = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx1_OF_Node_1 = false;
	FIRE_xx2_OF_Node_1 = false;
	FIRE_xx1_OF_Node_2 = false;
	FIRE_xx2_OF_Node_2 = false;
	FIRE_xx3_OF_ud_1 = false;
	FIRE_xx4_OF_ud_1 = false;
	FIRE_xx3_OF_bidir_3 = false;
	FIRE_xx4_OF_bidir_3 = false;
	FIRE_xx1_OF_Node_6 = false;
	FIRE_xx2_OF_Node_6 = false;
	FIRE_xx1_OF_Node_8 = false;
	FIRE_xx2_OF_Node_8 = false;
	FIRE_xx1_OF_Source = false;
	FIRE_xx2_OF_Source = false;
	FIRE_xx1_OF_Target = false;
	FIRE_xx2_OF_Target = false;
	FIRE_xx3_OF_ud_3 = false;
	FIRE_xx4_OF_ud_3 = false;
	FIRE_xx3_OF_bidir_7 = false;
	FIRE_xx4_OF_bidir_7 = false;
	FIRE_xx3_OF_bidir_12 = false;
	FIRE_xx4_OF_bidir_12 = false;
	FIRE_xx3_OF_bidir_2 = false;
	FIRE_xx4_OF_bidir_2 = false;

}

void storm::figaro::FigaroProgram_figaro_Telecom::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_figaro_Telecom::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_figaro_Telecom::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  intState[nb_failures_OF_Failure_counter] | Value : " << intState[nb_failures_OF_Failure_counter] << endl;
	cout << "Attribute :  boolState[fail_OF_Node_1] | Value : " << boolState[fail_OF_Node_1] << endl;
	cout << "Attribute :  boolState[connected_OF_Node_1] | Value : " << boolState[connected_OF_Node_1] << endl;
	cout << "Attribute :  boolState[fail_OF_Node_2] | Value : " << boolState[fail_OF_Node_2] << endl;
	cout << "Attribute :  boolState[connected_OF_Node_2] | Value : " << boolState[connected_OF_Node_2] << endl;
	cout << "Attribute :  boolState[interruption_OF_ud_1] | Value : " << boolState[interruption_OF_ud_1] << endl;
	cout << "Attribute :  boolState[interruption_OF_bidir_3] | Value : " << boolState[interruption_OF_bidir_3] << endl;
	cout << "Attribute :  boolState[fail_OF_Node_6] | Value : " << boolState[fail_OF_Node_6] << endl;
	cout << "Attribute :  boolState[connected_OF_Node_6] | Value : " << boolState[connected_OF_Node_6] << endl;
	cout << "Attribute :  boolState[fail_OF_Node_8] | Value : " << boolState[fail_OF_Node_8] << endl;
	cout << "Attribute :  boolState[connected_OF_Node_8] | Value : " << boolState[connected_OF_Node_8] << endl;
	cout << "Attribute :  boolState[fail_OF_Source] | Value : " << boolState[fail_OF_Source] << endl;
	cout << "Attribute :  boolState[connected_OF_Source] | Value : " << boolState[connected_OF_Source] << endl;
	cout << "Attribute :  boolState[fail_OF_Target] | Value : " << boolState[fail_OF_Target] << endl;
	cout << "Attribute :  boolState[connected_OF_Target] | Value : " << boolState[connected_OF_Target] << endl;
	cout << "Attribute :  boolState[interruption_OF_ud_3] | Value : " << boolState[interruption_OF_ud_3] << endl;
	cout << "Attribute :  boolState[interruption_OF_bidir_7] | Value : " << boolState[interruption_OF_bidir_7] << endl;
	cout << "Attribute :  boolState[interruption_OF_bidir_12] | Value : " << boolState[interruption_OF_bidir_12] << endl;
	cout << "Attribute :  boolState[interruption_OF_bidir_2] | Value : " << boolState[interruption_OF_bidir_2] << endl;
}

bool storm::figaro::FigaroProgram_figaro_Telecom::figaromodelhasinstransitions()
{
	return false;
}
void storm::figaro::FigaroProgram_figaro_Telecom::doReinitialisations()
{
	boolState[connected_OF_Node_1] = REINITIALISATION_OF_connected_OF_Node_1;
	boolState[connected_OF_Node_2] = REINITIALISATION_OF_connected_OF_Node_2;
	boolState[connected_OF_Node_6] = REINITIALISATION_OF_connected_OF_Node_6;
	boolState[connected_OF_Node_8] = REINITIALISATION_OF_connected_OF_Node_8;
	boolState[connected_OF_Source] = REINITIALISATION_OF_connected_OF_Source;
	boolState[connected_OF_Target] = REINITIALISATION_OF_connected_OF_Target;
}

void storm::figaro::FigaroProgram_figaro_Telecom::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx1_OF_Node_1 = true;
	}

	if (numFire == 1)
	{
		FIRE_xx2_OF_Node_1 = true;
	}

	if (numFire == 2)
	{
		FIRE_xx1_OF_Node_2 = true;
	}

	if (numFire == 3)
	{
		FIRE_xx2_OF_Node_2 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx3_OF_ud_1 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx4_OF_ud_1 = true;
	}

	if (numFire == 6)
	{
		FIRE_xx3_OF_bidir_3 = true;
	}

	if (numFire == 7)
	{
		FIRE_xx4_OF_bidir_3 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx1_OF_Node_6 = true;
	}

	if (numFire == 9)
	{
		FIRE_xx2_OF_Node_6 = true;
	}

	if (numFire == 10)
	{
		FIRE_xx1_OF_Node_8 = true;
	}

	if (numFire == 11)
	{
		FIRE_xx2_OF_Node_8 = true;
	}

	if (numFire == 12)
	{
		FIRE_xx1_OF_Source = true;
	}

	if (numFire == 13)
	{
		FIRE_xx2_OF_Source = true;
	}

	if (numFire == 14)
	{
		FIRE_xx1_OF_Target = true;
	}

	if (numFire == 15)
	{
		FIRE_xx2_OF_Target = true;
	}

	if (numFire == 16)
	{
		FIRE_xx3_OF_ud_3 = true;
	}

	if (numFire == 17)
	{
		FIRE_xx4_OF_ud_3 = true;
	}

	if (numFire == 18)
	{
		FIRE_xx3_OF_bidir_7 = true;
	}

	if (numFire == 19)
	{
		FIRE_xx4_OF_bidir_7 = true;
	}

	if (numFire == 20)
	{
		FIRE_xx3_OF_bidir_12 = true;
	}

	if (numFire == 21)
	{
		FIRE_xx4_OF_bidir_12 = true;
	}

	if (numFire == 22)
	{
		FIRE_xx3_OF_bidir_2 = true;
	}

	if (numFire == 23)
	{
		FIRE_xx4_OF_bidir_2 = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx1_OF_Node_1
	if (boolState[fail_OF_Node_1] == false) 
	{
		 
		if (FIRE_xx1_OF_Node_1)
		{
			boolState[fail_OF_Node_1]  =  true;
			FIRE_xx1_OF_Node_1 = false;
		}
	}

	// Occurrence xx2_OF_Node_1
	if (boolState[fail_OF_Node_1] == true) 
	{
		 
		if (FIRE_xx2_OF_Node_1)
		{
			boolState[fail_OF_Node_1]  =  false;
			FIRE_xx2_OF_Node_1 = false;
		}
	}

	// Occurrence xx1_OF_Node_2
	if (boolState[fail_OF_Node_2] == false) 
	{
		 
		if (FIRE_xx1_OF_Node_2)
		{
			boolState[fail_OF_Node_2]  =  true;
			FIRE_xx1_OF_Node_2 = false;
		}
	}

	// Occurrence xx2_OF_Node_2
	if (boolState[fail_OF_Node_2] == true) 
	{
		 
		if (FIRE_xx2_OF_Node_2)
		{
			boolState[fail_OF_Node_2]  =  false;
			FIRE_xx2_OF_Node_2 = false;
		}
	}

	// Occurrence xx3_OF_ud_1
	if (boolState[interruption_OF_ud_1] == false) 
	{
		 
		if (FIRE_xx3_OF_ud_1)
		{
			boolState[interruption_OF_ud_1]  =  true;
			FIRE_xx3_OF_ud_1 = false;
		}
	}

	// Occurrence xx4_OF_ud_1
	if (boolState[interruption_OF_ud_1] == true) 
	{
		 
		if (FIRE_xx4_OF_ud_1)
		{
			boolState[interruption_OF_ud_1]  =  false;
			FIRE_xx4_OF_ud_1 = false;
		}
	}

	// Occurrence xx3_OF_bidir_3
	if (boolState[interruption_OF_bidir_3] == false) 
	{
		 
		if (FIRE_xx3_OF_bidir_3)
		{
			boolState[interruption_OF_bidir_3]  =  true;
			FIRE_xx3_OF_bidir_3 = false;
		}
	}

	// Occurrence xx4_OF_bidir_3
	if (boolState[interruption_OF_bidir_3] == true) 
	{
		 
		if (FIRE_xx4_OF_bidir_3)
		{
			boolState[interruption_OF_bidir_3]  =  false;
			FIRE_xx4_OF_bidir_3 = false;
		}
	}

	// Occurrence xx1_OF_Node_6
	if (boolState[fail_OF_Node_6] == false) 
	{
		 
		if (FIRE_xx1_OF_Node_6)
		{
			boolState[fail_OF_Node_6]  =  true;
			FIRE_xx1_OF_Node_6 = false;
		}
	}

	// Occurrence xx2_OF_Node_6
	if (boolState[fail_OF_Node_6] == true) 
	{
		 
		if (FIRE_xx2_OF_Node_6)
		{
			boolState[fail_OF_Node_6]  =  false;
			FIRE_xx2_OF_Node_6 = false;
		}
	}

	// Occurrence xx1_OF_Node_8
	if (boolState[fail_OF_Node_8] == false) 
	{
		 
		if (FIRE_xx1_OF_Node_8)
		{
			boolState[fail_OF_Node_8]  =  true;
			FIRE_xx1_OF_Node_8 = false;
		}
	}

	// Occurrence xx2_OF_Node_8
	if (boolState[fail_OF_Node_8] == true) 
	{
		 
		if (FIRE_xx2_OF_Node_8)
		{
			boolState[fail_OF_Node_8]  =  false;
			FIRE_xx2_OF_Node_8 = false;
		}
	}

	// Occurrence xx1_OF_Source
	if (boolState[fail_OF_Source] == false) 
	{
		 
		if (FIRE_xx1_OF_Source)
		{
			boolState[fail_OF_Source]  =  true;
			FIRE_xx1_OF_Source = false;
		}
	}

	// Occurrence xx2_OF_Source
	if (boolState[fail_OF_Source] == true) 
	{
		 
		if (FIRE_xx2_OF_Source)
		{
			boolState[fail_OF_Source]  =  false;
			FIRE_xx2_OF_Source = false;
		}
	}

	// Occurrence xx1_OF_Target
	if (boolState[fail_OF_Target] == false) 
	{
		 
		if (FIRE_xx1_OF_Target)
		{
			boolState[fail_OF_Target]  =  true;
			FIRE_xx1_OF_Target = false;
		}
	}

	// Occurrence xx2_OF_Target
	if (boolState[fail_OF_Target] == true) 
	{
		 
		if (FIRE_xx2_OF_Target)
		{
			boolState[fail_OF_Target]  =  false;
			FIRE_xx2_OF_Target = false;
		}
	}

	// Occurrence xx3_OF_ud_3
	if (boolState[interruption_OF_ud_3] == false) 
	{
		 
		if (FIRE_xx3_OF_ud_3)
		{
			boolState[interruption_OF_ud_3]  =  true;
			FIRE_xx3_OF_ud_3 = false;
		}
	}

	// Occurrence xx4_OF_ud_3
	if (boolState[interruption_OF_ud_3] == true) 
	{
		 
		if (FIRE_xx4_OF_ud_3)
		{
			boolState[interruption_OF_ud_3]  =  false;
			FIRE_xx4_OF_ud_3 = false;
		}
	}

	// Occurrence xx3_OF_bidir_7
	if (boolState[interruption_OF_bidir_7] == false) 
	{
		 
		if (FIRE_xx3_OF_bidir_7)
		{
			boolState[interruption_OF_bidir_7]  =  true;
			FIRE_xx3_OF_bidir_7 = false;
		}
	}

	// Occurrence xx4_OF_bidir_7
	if (boolState[interruption_OF_bidir_7] == true) 
	{
		 
		if (FIRE_xx4_OF_bidir_7)
		{
			boolState[interruption_OF_bidir_7]  =  false;
			FIRE_xx4_OF_bidir_7 = false;
		}
	}

	// Occurrence xx3_OF_bidir_12
	if (boolState[interruption_OF_bidir_12] == false) 
	{
		 
		if (FIRE_xx3_OF_bidir_12)
		{
			boolState[interruption_OF_bidir_12]  =  true;
			FIRE_xx3_OF_bidir_12 = false;
		}
	}

	// Occurrence xx4_OF_bidir_12
	if (boolState[interruption_OF_bidir_12] == true) 
	{
		 
		if (FIRE_xx4_OF_bidir_12)
		{
			boolState[interruption_OF_bidir_12]  =  false;
			FIRE_xx4_OF_bidir_12 = false;
		}
	}

	// Occurrence xx3_OF_bidir_2
	if (boolState[interruption_OF_bidir_2] == false) 
	{
		 
		if (FIRE_xx3_OF_bidir_2)
		{
			boolState[interruption_OF_bidir_2]  =  true;
			FIRE_xx3_OF_bidir_2 = false;
		}
	}

	// Occurrence xx4_OF_bidir_2
	if (boolState[interruption_OF_bidir_2] == true) 
	{
		 
		if (FIRE_xx4_OF_bidir_2)
		{
			boolState[interruption_OF_bidir_2]  =  false;
			FIRE_xx4_OF_bidir_2 = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_figaro_Telecom::showFireableOccurrences()
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
     
	if (boolState[fail_OF_Node_1] == false)
	{
		cout << "0 : xx1_OF_Node_1 : FAULT fail  DIST EXP (1e-05)  INDUCING boolState[fail_OF_Node_1]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 1e-05, "EXP", 0));
	}
	if (boolState[fail_OF_Node_1] == true)
	{
		cout << "1 : xx2_OF_Node_1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[fail_OF_Node_1]  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.1, "EXP", 0));
	}
	if (boolState[fail_OF_Node_2] == false)
	{
		cout << "2 : xx1_OF_Node_2 : FAULT fail  DIST EXP (1e-05)  INDUCING boolState[fail_OF_Node_2]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 1e-05, "EXP", 0));
	}
	if (boolState[fail_OF_Node_2] == true)
	{
		cout << "3 : xx2_OF_Node_2 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[fail_OF_Node_2]  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.1, "EXP", 0));
	}
	if (boolState[interruption_OF_ud_1] == false)
	{
		cout << "4 : xx3_OF_ud_1 : FAULT interruption  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_ud_1]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 1e-05, "EXP", 0));
	}
	if (boolState[interruption_OF_ud_1] == true)
	{
		cout << "5 : xx4_OF_ud_1 : REPAIR rep  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_ud_1]  =  FALSE" << endl;
		list.push_back(make_tuple(5, 1e-05, "EXP", 0));
	}
	if (boolState[interruption_OF_bidir_3] == false)
	{
		cout << "6 : xx3_OF_bidir_3 : FAULT interruption  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_bidir_3]  =  TRUE" << endl;
		list.push_back(make_tuple(6, 1e-05, "EXP", 0));
	}
	if (boolState[interruption_OF_bidir_3] == true)
	{
		cout << "7 : xx4_OF_bidir_3 : REPAIR rep  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_bidir_3]  =  FALSE" << endl;
		list.push_back(make_tuple(7, 1e-05, "EXP", 0));
	}
	if (boolState[fail_OF_Node_6] == false)
	{
		cout << "8 : xx1_OF_Node_6 : FAULT fail  DIST EXP (1e-05)  INDUCING boolState[fail_OF_Node_6]  =  TRUE" << endl;
		list.push_back(make_tuple(8, 1e-05, "EXP", 0));
	}
	if (boolState[fail_OF_Node_6] == true)
	{
		cout << "9 : xx2_OF_Node_6 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[fail_OF_Node_6]  =  FALSE" << endl;
		list.push_back(make_tuple(9, 0.1, "EXP", 0));
	}
	if (boolState[fail_OF_Node_8] == false)
	{
		cout << "10 : xx1_OF_Node_8 : FAULT fail  DIST EXP (1e-05)  INDUCING boolState[fail_OF_Node_8]  =  TRUE" << endl;
		list.push_back(make_tuple(10, 1e-05, "EXP", 0));
	}
	if (boolState[fail_OF_Node_8] == true)
	{
		cout << "11 : xx2_OF_Node_8 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[fail_OF_Node_8]  =  FALSE" << endl;
		list.push_back(make_tuple(11, 0.1, "EXP", 0));
	}
	if (boolState[fail_OF_Source] == false)
	{
		cout << "12 : xx1_OF_Source : FAULT fail  DIST EXP (1e-05)  INDUCING boolState[fail_OF_Source]  =  TRUE" << endl;
		list.push_back(make_tuple(12, 1e-05, "EXP", 0));
	}
	if (boolState[fail_OF_Source] == true)
	{
		cout << "13 : xx2_OF_Source : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[fail_OF_Source]  =  FALSE" << endl;
		list.push_back(make_tuple(13, 0.1, "EXP", 0));
	}
	if (boolState[fail_OF_Target] == false)
	{
		cout << "14 : xx1_OF_Target : FAULT fail  DIST EXP (1e-05)  INDUCING boolState[fail_OF_Target]  =  TRUE" << endl;
		list.push_back(make_tuple(14, 1e-05, "EXP", 0));
	}
	if (boolState[fail_OF_Target] == true)
	{
		cout << "15 : xx2_OF_Target : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[fail_OF_Target]  =  FALSE" << endl;
		list.push_back(make_tuple(15, 0.1, "EXP", 0));
	}
	if (boolState[interruption_OF_ud_3] == false)
	{
		cout << "16 : xx3_OF_ud_3 : FAULT interruption  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_ud_3]  =  TRUE" << endl;
		list.push_back(make_tuple(16, 1e-05, "EXP", 0));
	}
	if (boolState[interruption_OF_ud_3] == true)
	{
		cout << "17 : xx4_OF_ud_3 : REPAIR rep  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_ud_3]  =  FALSE" << endl;
		list.push_back(make_tuple(17, 1e-05, "EXP", 0));
	}
	if (boolState[interruption_OF_bidir_7] == false)
	{
		cout << "18 : xx3_OF_bidir_7 : FAULT interruption  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_bidir_7]  =  TRUE" << endl;
		list.push_back(make_tuple(18, 1e-05, "EXP", 0));
	}
	if (boolState[interruption_OF_bidir_7] == true)
	{
		cout << "19 : xx4_OF_bidir_7 : REPAIR rep  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_bidir_7]  =  FALSE" << endl;
		list.push_back(make_tuple(19, 1e-05, "EXP", 0));
	}
	if (boolState[interruption_OF_bidir_12] == false)
	{
		cout << "20 : xx3_OF_bidir_12 : FAULT interruption  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_bidir_12]  =  TRUE" << endl;
		list.push_back(make_tuple(20, 1e-05, "EXP", 0));
	}
	if (boolState[interruption_OF_bidir_12] == true)
	{
		cout << "21 : xx4_OF_bidir_12 : REPAIR rep  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_bidir_12]  =  FALSE" << endl;
		list.push_back(make_tuple(21, 1e-05, "EXP", 0));
	}
	if (boolState[interruption_OF_bidir_2] == false)
	{
		cout << "22 : xx3_OF_bidir_2 : FAULT interruption  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_bidir_2]  =  TRUE" << endl;
		list.push_back(make_tuple(22, 1e-05, "EXP", 0));
	}
	if (boolState[interruption_OF_bidir_2] == true)
	{
		cout << "23 : xx4_OF_bidir_2 : REPAIR rep  DIST EXP (1e-05)  INDUCING boolState[interruption_OF_bidir_2]  =  FALSE" << endl;
		list.push_back(make_tuple(23, 1e-05, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_figaro_Telecom::runOnceInteractionStep_default_step()
{


	intState[nb_failures_OF_Failure_counter]  =  (((((( boolState[fail_OF_Node_1]  +    boolState[fail_OF_Node_2])  +   boolState[fail_OF_Node_6])  +   boolState[fail_OF_Node_8])  +    boolState[fail_OF_Source])  +   boolState[fail_OF_Target])  +  ((((( boolState[interruption_OF_ud_1]  +   boolState[interruption_OF_bidir_3])  +   boolState[interruption_OF_ud_3])  +    boolState[interruption_OF_bidir_7])  +   boolState[interruption_OF_bidir_12])  +    boolState[interruption_OF_bidir_2]))  ;

	if (((boolState[interruption_OF_ud_1] == false) && boolState[connected_OF_Source]) && (boolState[fail_OF_Node_1] == false) )
	{
		boolState[connected_OF_Node_1]  =  true;
	}

	if (((boolState[interruption_OF_bidir_3] == false) && ((boolState[fail_OF_Node_2] ==  false) && (boolState[fail_OF_Node_8] == false))) && (boolState[connected_OF_Node_2] || boolState[connected_OF_Node_8]) )
	{
		boolState[connected_OF_Node_2]  =  true;
		  boolState[connected_OF_Node_8]  =  true;
	}

	if (boolState[fail_OF_Source] == false )
	{
		boolState[connected_OF_Source]  =  true;
	}

	if (((boolState[interruption_OF_ud_3] == false) && boolState[connected_OF_Node_1]) && (boolState[fail_OF_Node_6] == false) )
	{
		boolState[connected_OF_Node_6]  =  true;
	}

	if (((boolState[interruption_OF_bidir_7] == false) && ((boolState[fail_OF_Node_8] ==  false) && (boolState[fail_OF_Target] == false))) && (boolState[connected_OF_Node_8] || boolState[connected_OF_Target]) )
	{
		boolState[connected_OF_Node_8]  =  true;
		  boolState[connected_OF_Target]  =  true;
	}

	if (((boolState[interruption_OF_bidir_12] == false) && ((boolState[fail_OF_Node_6] ==  false) && (boolState[fail_OF_Target] == false))) && (boolState[connected_OF_Node_6] || boolState[connected_OF_Target]) )
	{
		boolState[connected_OF_Node_6]  =  true;
		  boolState[connected_OF_Target]  =  true;
	}

	if (((boolState[interruption_OF_bidir_2] == false) && ((boolState[fail_OF_Node_2] ==  false) && (boolState[fail_OF_Source] == false))) && (boolState[connected_OF_Node_2] || boolState[connected_OF_Source]) )
	{
		boolState[connected_OF_Node_2]  =  true;
		  boolState[connected_OF_Source]  =  true;
	}

}

void
storm::figaro::FigaroProgram_figaro_Telecom::runInteractions() {
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
    
	boolFailureState[exp0] = (  !boolState[connected_OF_Target] );
        cout << endl;
    }void storm::figaro::FigaroProgram_figaro_Telecom::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_figaro_Telecom::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_figaro_Telecom::fireinsttransitiongroup(std::string user_input_ins)

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
    