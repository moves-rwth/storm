#include <iostream>
#include "FigaroModelBDMP_09_Phases_Trim_Article_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::init()
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
	REINITIALISATION_OF_required_OF_AND_3 = true;
	boolState[already_S_OF_AND_3] = false;
	REINITIALISATION_OF_S_OF_AND_3 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_3 = false;
	REINITIALISATION_OF_required_OF_Cpt_A = true;
	boolState[already_S_OF_Cpt_A] = false;
	REINITIALISATION_OF_S_OF_Cpt_A = false;
	REINITIALISATION_OF_relevant_evt_OF_Cpt_A = false;
	boolState[failF_OF_Cpt_A] = false;
	REINITIALISATION_OF_required_OF_Cpt_B = true;
	boolState[already_S_OF_Cpt_B] = false;
	REINITIALISATION_OF_S_OF_Cpt_B = false;
	REINITIALISATION_OF_relevant_evt_OF_Cpt_B = false;
	boolState[failF_OF_Cpt_B] = false;
	REINITIALISATION_OF_required_OF_Cpt_C = true;
	boolState[already_S_OF_Cpt_C] = false;
	REINITIALISATION_OF_S_OF_Cpt_C = false;
	REINITIALISATION_OF_relevant_evt_OF_Cpt_C = false;
	boolState[failF_OF_Cpt_C] = false;
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
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;
	REINITIALISATION_OF_required_OF_phase_1 = true;
	boolState[already_S_OF_phase_1] = false;
	REINITIALISATION_OF_S_OF_phase_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_phase_1 = false;
	boolState[in_progress_OF_phase_1] = false;
	boolState[already_required_OF_phase_1] = false;
	boolState[start_phase_OF_phase_1] = false;
	REINITIALISATION_OF_required_OF_phase_2 = true;
	boolState[already_S_OF_phase_2] = false;
	REINITIALISATION_OF_S_OF_phase_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_phase_2 = false;
	boolState[in_progress_OF_phase_2] = false;
	boolState[already_required_OF_phase_2] = false;
	boolState[start_phase_OF_phase_2] = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_Cpt_A = false;
	FIRE_xx11_OF_Cpt_A = false;
	FIRE_xx10_OF_Cpt_B = false;
	FIRE_xx11_OF_Cpt_B = false;
	FIRE_xx10_OF_Cpt_C = false;
	FIRE_xx11_OF_Cpt_C = false;
	FIRE_xx43_a_OF_phase_1 = false;
	FIRE_xx47_OF_phase_1_INS_7 = false;
	FIRE_xx43_a_OF_phase_2 = false;
	FIRE_xx47_OF_phase_2_INS_9 = false;

}

void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::printState()
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
	cout << "Attribute :  boolState[required_OF_AND_3] | Value : " << boolState[required_OF_AND_3] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_3] | Value : " << boolState[already_S_OF_AND_3] << endl;
	cout << "Attribute :  boolState[S_OF_AND_3] | Value : " << boolState[S_OF_AND_3] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_3] | Value : " << boolState[relevant_evt_OF_AND_3] << endl;
	cout << "Attribute :  boolState[required_OF_Cpt_A] | Value : " << boolState[required_OF_Cpt_A] << endl;
	cout << "Attribute :  boolState[already_S_OF_Cpt_A] | Value : " << boolState[already_S_OF_Cpt_A] << endl;
	cout << "Attribute :  boolState[S_OF_Cpt_A] | Value : " << boolState[S_OF_Cpt_A] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Cpt_A] | Value : " << boolState[relevant_evt_OF_Cpt_A] << endl;
	cout << "Attribute :  boolState[failF_OF_Cpt_A] | Value : " << boolState[failF_OF_Cpt_A] << endl;
	cout << "Attribute :  boolState[required_OF_Cpt_B] | Value : " << boolState[required_OF_Cpt_B] << endl;
	cout << "Attribute :  boolState[already_S_OF_Cpt_B] | Value : " << boolState[already_S_OF_Cpt_B] << endl;
	cout << "Attribute :  boolState[S_OF_Cpt_B] | Value : " << boolState[S_OF_Cpt_B] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Cpt_B] | Value : " << boolState[relevant_evt_OF_Cpt_B] << endl;
	cout << "Attribute :  boolState[failF_OF_Cpt_B] | Value : " << boolState[failF_OF_Cpt_B] << endl;
	cout << "Attribute :  boolState[required_OF_Cpt_C] | Value : " << boolState[required_OF_Cpt_C] << endl;
	cout << "Attribute :  boolState[already_S_OF_Cpt_C] | Value : " << boolState[already_S_OF_Cpt_C] << endl;
	cout << "Attribute :  boolState[S_OF_Cpt_C] | Value : " << boolState[S_OF_Cpt_C] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Cpt_C] | Value : " << boolState[relevant_evt_OF_Cpt_C] << endl;
	cout << "Attribute :  boolState[failF_OF_Cpt_C] | Value : " << boolState[failF_OF_Cpt_C] << endl;
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
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
	cout << "Attribute :  boolState[required_OF_phase_1] | Value : " << boolState[required_OF_phase_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_phase_1] | Value : " << boolState[already_S_OF_phase_1] << endl;
	cout << "Attribute :  boolState[S_OF_phase_1] | Value : " << boolState[S_OF_phase_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_phase_1] | Value : " << boolState[relevant_evt_OF_phase_1] << endl;
	cout << "Attribute :  boolState[in_progress_OF_phase_1] | Value : " << boolState[in_progress_OF_phase_1] << endl;
	cout << "Attribute :  boolState[already_required_OF_phase_1] | Value : " << boolState[already_required_OF_phase_1] << endl;
	cout << "Attribute :  boolState[start_phase_OF_phase_1] | Value : " << boolState[start_phase_OF_phase_1] << endl;
	cout << "Attribute :  boolState[required_OF_phase_2] | Value : " << boolState[required_OF_phase_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_phase_2] | Value : " << boolState[already_S_OF_phase_2] << endl;
	cout << "Attribute :  boolState[S_OF_phase_2] | Value : " << boolState[S_OF_phase_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_phase_2] | Value : " << boolState[relevant_evt_OF_phase_2] << endl;
	cout << "Attribute :  boolState[in_progress_OF_phase_2] | Value : " << boolState[in_progress_OF_phase_2] << endl;
	cout << "Attribute :  boolState[already_required_OF_phase_2] | Value : " << boolState[already_required_OF_phase_2] << endl;
	cout << "Attribute :  boolState[start_phase_OF_phase_2] | Value : " << boolState[start_phase_OF_phase_2] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_AND_2] = REINITIALISATION_OF_required_OF_AND_2;
	boolState[S_OF_AND_2] = REINITIALISATION_OF_S_OF_AND_2;
	boolState[relevant_evt_OF_AND_2] = REINITIALISATION_OF_relevant_evt_OF_AND_2;
	boolState[required_OF_AND_3] = REINITIALISATION_OF_required_OF_AND_3;
	boolState[S_OF_AND_3] = REINITIALISATION_OF_S_OF_AND_3;
	boolState[relevant_evt_OF_AND_3] = REINITIALISATION_OF_relevant_evt_OF_AND_3;
	boolState[required_OF_Cpt_A] = REINITIALISATION_OF_required_OF_Cpt_A;
	boolState[S_OF_Cpt_A] = REINITIALISATION_OF_S_OF_Cpt_A;
	boolState[relevant_evt_OF_Cpt_A] = REINITIALISATION_OF_relevant_evt_OF_Cpt_A;
	boolState[required_OF_Cpt_B] = REINITIALISATION_OF_required_OF_Cpt_B;
	boolState[S_OF_Cpt_B] = REINITIALISATION_OF_S_OF_Cpt_B;
	boolState[relevant_evt_OF_Cpt_B] = REINITIALISATION_OF_relevant_evt_OF_Cpt_B;
	boolState[required_OF_Cpt_C] = REINITIALISATION_OF_required_OF_Cpt_C;
	boolState[S_OF_Cpt_C] = REINITIALISATION_OF_S_OF_Cpt_C;
	boolState[relevant_evt_OF_Cpt_C] = REINITIALISATION_OF_relevant_evt_OF_Cpt_C;
	boolState[required_OF_OR_1] = REINITIALISATION_OF_required_OF_OR_1;
	boolState[S_OF_OR_1] = REINITIALISATION_OF_S_OF_OR_1;
	boolState[relevant_evt_OF_OR_1] = REINITIALISATION_OF_relevant_evt_OF_OR_1;
	boolState[required_OF_OR_2] = REINITIALISATION_OF_required_OF_OR_2;
	boolState[S_OF_OR_2] = REINITIALISATION_OF_S_OF_OR_2;
	boolState[relevant_evt_OF_OR_2] = REINITIALISATION_OF_relevant_evt_OF_OR_2;
	boolState[required_OF_OR_3] = REINITIALISATION_OF_required_OF_OR_3;
	boolState[S_OF_OR_3] = REINITIALISATION_OF_S_OF_OR_3;
	boolState[relevant_evt_OF_OR_3] = REINITIALISATION_OF_relevant_evt_OF_OR_3;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
	boolState[required_OF_phase_1] = REINITIALISATION_OF_required_OF_phase_1;
	boolState[S_OF_phase_1] = REINITIALISATION_OF_S_OF_phase_1;
	boolState[relevant_evt_OF_phase_1] = REINITIALISATION_OF_relevant_evt_OF_phase_1;
	boolState[required_OF_phase_2] = REINITIALISATION_OF_required_OF_phase_2;
	boolState[S_OF_phase_2] = REINITIALISATION_OF_S_OF_phase_2;
	boolState[relevant_evt_OF_phase_2] = REINITIALISATION_OF_relevant_evt_OF_phase_2;
}

void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_Cpt_A = true;
	}

	if (numFire == 1)
	{
		FIRE_xx11_OF_Cpt_A = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_Cpt_B = true;
	}

	if (numFire == 3)
	{
		FIRE_xx11_OF_Cpt_B = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_Cpt_C = true;
	}

	if (numFire == 5)
	{
		FIRE_xx11_OF_Cpt_C = true;
	}

	if (numFire == 6)
	{
		FIRE_xx43_a_OF_phase_1 = true;
	}

	if (numFire == 7)
	{
		FIRE_xx47_OF_phase_1_INS_7 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx43_a_OF_phase_2 = true;
	}

	if (numFire == 9)
	{
		FIRE_xx47_OF_phase_2_INS_9 = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_Cpt_A
	if ((boolState[failF_OF_Cpt_A] == false) && (boolState[required_OF_Cpt_A] &&  boolState[relevant_evt_OF_Cpt_A])) 
	{
		 
		if (FIRE_xx10_OF_Cpt_A)
		{
			boolState[failF_OF_Cpt_A]  =  true;
			FIRE_xx10_OF_Cpt_A = false;
		}
	}

	// Occurrence xx11_OF_Cpt_A
	if (boolState[failF_OF_Cpt_A] == true) 
	{
		 
		if (FIRE_xx11_OF_Cpt_A)
		{
			boolState[failF_OF_Cpt_A]  =  false;
			FIRE_xx11_OF_Cpt_A = false;
		}
	}

	// Occurrence xx10_OF_Cpt_B
	if ((boolState[failF_OF_Cpt_B] == false) && (boolState[required_OF_Cpt_B] &&  boolState[relevant_evt_OF_Cpt_B])) 
	{
		 
		if (FIRE_xx10_OF_Cpt_B)
		{
			boolState[failF_OF_Cpt_B]  =  true;
			FIRE_xx10_OF_Cpt_B = false;
		}
	}

	// Occurrence xx11_OF_Cpt_B
	if (boolState[failF_OF_Cpt_B] == true) 
	{
		 
		if (FIRE_xx11_OF_Cpt_B)
		{
			boolState[failF_OF_Cpt_B]  =  false;
			FIRE_xx11_OF_Cpt_B = false;
		}
	}

	// Occurrence xx10_OF_Cpt_C
	if ((boolState[failF_OF_Cpt_C] == false) && (boolState[required_OF_Cpt_C] &&  boolState[relevant_evt_OF_Cpt_C])) 
	{
		 
		if (FIRE_xx10_OF_Cpt_C)
		{
			boolState[failF_OF_Cpt_C]  =  true;
			FIRE_xx10_OF_Cpt_C = false;
		}
	}

	// Occurrence xx11_OF_Cpt_C
	if (boolState[failF_OF_Cpt_C] == true) 
	{
		 
		if (FIRE_xx11_OF_Cpt_C)
		{
			boolState[failF_OF_Cpt_C]  =  false;
			FIRE_xx11_OF_Cpt_C = false;
		}
	}

	// Occurrence xx43_a_OF_phase_1
	if (boolState[in_progress_OF_phase_1]) 
	{
		 
		if (FIRE_xx43_a_OF_phase_1)
		{
			boolState[in_progress_OF_phase_1]  =  false;
			FIRE_xx43_a_OF_phase_1 = false;
		}
	}

	// Occurrence xx47_OF_phase_1

	if (boolState[start_phase_OF_phase_1]) 
	{
	
		
		if (FIRE_xx47_OF_phase_1_INS_7) 
		{
			boolState[in_progress_OF_phase_1]  =  true;
			boolState[start_phase_OF_phase_1]  =  false;
			FIRE_xx47_OF_phase_1_INS_7 = false;
		}
	
	}
	// Occurrence xx43_a_OF_phase_2
	if (boolState[in_progress_OF_phase_2]) 
	{
		 
		if (FIRE_xx43_a_OF_phase_2)
		{
			boolState[in_progress_OF_phase_2]  =  false;
			FIRE_xx43_a_OF_phase_2 = false;
		}
	}

	// Occurrence xx47_OF_phase_2

	if (boolState[start_phase_OF_phase_2]) 
	{
	
		
		if (FIRE_xx47_OF_phase_2_INS_9) 
		{
			boolState[in_progress_OF_phase_2]  =  true;
			boolState[start_phase_OF_phase_2]  =  false;
			FIRE_xx47_OF_phase_2_INS_9 = false;
		}
	
	}
}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if (boolState[start_phase_OF_phase_1])
	{
		cout << "7 :  INS_SUB_COUNT (1) |TRANSITION | start  LABEL \"start of phase phase_1\" | DIST INS (1) | INDUCING boolState[in_progress_OF_phase_1]  =  TRUE,start_phase_OF_phase_1  =  FALSE" << endl;
		list.push_back(make_tuple(7, 1, "INS", 1));
	}
	if (boolState[start_phase_OF_phase_2])
	{
		cout << "9 :  INS_SUB_COUNT (2) |TRANSITION | start  LABEL \"start of phase phase_2\" | DIST INS (1) | INDUCING boolState[in_progress_OF_phase_2]  =  TRUE,start_phase_OF_phase_2  =  FALSE" << endl;
		list.push_back(make_tuple(9, 1, "INS", 2));
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
     
	if ((boolState[failF_OF_Cpt_A] == false) && (boolState[required_OF_Cpt_A] && boolState[relevant_evt_OF_Cpt_A]))
	{
		cout << "0 : xx10_OF_Cpt_A : FAULT failF  LABEL \"failure in operation Cpt_A\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Cpt_A]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_Cpt_A] == true)
	{
		cout << "1 : xx11_OF_Cpt_A : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_Cpt_A]  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_Cpt_B] == false) && (boolState[required_OF_Cpt_B] && boolState[relevant_evt_OF_Cpt_B]))
	{
		cout << "2 : xx10_OF_Cpt_B : FAULT failF  LABEL \"failure in operation Cpt_B\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Cpt_B]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_Cpt_B] == true)
	{
		cout << "3 : xx11_OF_Cpt_B : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_Cpt_B]  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_Cpt_C] == false) && (boolState[required_OF_Cpt_C] && boolState[relevant_evt_OF_Cpt_C]))
	{
		cout << "4 : xx10_OF_Cpt_C : FAULT failF  LABEL \"failure in operation Cpt_C\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Cpt_C]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_Cpt_C] == true)
	{
		cout << "5 : xx11_OF_Cpt_C : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_Cpt_C]  =  FALSE" << endl;
		list.push_back(make_tuple(5, 0.1, "EXP", 0));
	}
	if (boolState[in_progress_OF_phase_1])
	{
		cout << "6 : xx43_a_OF_phase_1 : TRANSITION end  LABEL \"End of phase phase_1\"  DIST EXP (0.1)  INDUCING boolState[in_progress_OF_phase_1]  =  FALSE" << endl;
		list.push_back(make_tuple(6, 0.1, "EXP", 0));
	}
	if (boolState[in_progress_OF_phase_2])
	{
		cout << "8 : xx43_a_OF_phase_2 : TRANSITION end  LABEL \"End of phase phase_2\"  DIST EXP (0.1)  INDUCING boolState[in_progress_OF_phase_2]  =  FALSE" << endl;
		list.push_back(make_tuple(8, 0.1, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_Cpt_A] == true )
	{
		boolState[S_OF_Cpt_A]  =  true;
	}

	if (boolState[failF_OF_Cpt_B] == true )
	{
		boolState[S_OF_Cpt_B]  =  true;
	}

	if (boolState[failF_OF_Cpt_C] == true )
	{
		boolState[S_OF_Cpt_C]  =  true;
	}



	boolState[S_OF_phase_1]  =  boolState[in_progress_OF_phase_1]  ;



	boolState[S_OF_phase_2]  =  boolState[in_progress_OF_phase_2]  ;

}


void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_AND_3] && boolState[S_OF_phase_1] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (boolState[S_OF_OR_3] && boolState[S_OF_phase_2] )
	{
		boolState[S_OF_AND_2]  =  true;
	}

	if ((boolState[S_OF_Cpt_A] && boolState[S_OF_Cpt_B]) && boolState[S_OF_Cpt_C] )
	{
		boolState[S_OF_AND_3]  =  true;
	}

	if (boolState[S_OF_Cpt_A] || boolState[S_OF_Cpt_B] )
	{
		boolState[S_OF_OR_1]  =  true;
	}

	if (boolState[S_OF_AND_1] || boolState[S_OF_AND_2] )
	{
		boolState[S_OF_OR_2]  =  true;
	}

	if ((boolState[S_OF_Cpt_A] || boolState[S_OF_Cpt_B]) || boolState[S_OF_Cpt_C] )
	{
		boolState[S_OF_OR_3]  =  true;
	}

	if (boolState[S_OF_OR_2] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_OR_2] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if (boolState[relevant_evt_OF_OR_2] && ( !boolState[S_OF_OR_2]) )
	{
		boolState[relevant_evt_OF_AND_1]  =  true;
	}

	if ( !boolState[required_OF_OR_2] )
	{
		boolState[required_OF_AND_2]  =  false;
	}

	if (boolState[relevant_evt_OF_OR_2] && ( !boolState[S_OF_OR_2]) )
	{
		boolState[relevant_evt_OF_AND_2]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_phase_1]) )
	{
		boolState[required_OF_AND_3]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_AND_3]  =  true;
	}

	if ((( !boolState[required_OF_AND_3]) && ( !boolState[required_OF_OR_1])) && (   !boolState[required_OF_OR_3]) )
	{
		boolState[required_OF_Cpt_A]  =  false;
	}

	if (((boolState[relevant_evt_OF_AND_3] && ( !boolState[S_OF_AND_3])) || (  boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1]))) || (boolState[relevant_evt_OF_OR_3] && ( !boolState[S_OF_OR_3])) )
	{
		boolState[relevant_evt_OF_Cpt_A]  =  true;
	}

	if ((( !boolState[required_OF_AND_3]) && ( !boolState[required_OF_OR_1])) && (   !boolState[required_OF_OR_3]) )
	{
		boolState[required_OF_Cpt_B]  =  false;
	}

	if (((boolState[relevant_evt_OF_AND_3] && ( !boolState[S_OF_AND_3])) || (  boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1]))) || (boolState[relevant_evt_OF_OR_3] && ( !boolState[S_OF_OR_3])) )
	{
		boolState[relevant_evt_OF_Cpt_B]  =  true;
	}

	if ((( !boolState[required_OF_AND_3]) && ( !boolState[required_OF_OR_3])) || ( !  boolState[S_OF_OR_1]) )
	{
		boolState[required_OF_Cpt_C]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_3] && ( !boolState[S_OF_AND_3])) || (  boolState[relevant_evt_OF_OR_3] && ( !boolState[S_OF_OR_3])) )
	{
		boolState[relevant_evt_OF_Cpt_C]  =  true;
	}

	if ( !boolState[S_OF_Cpt_C] )
	{
		boolState[relevant_evt_OF_OR_1]  =  true;
	}

	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_OR_2]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_OR_2]  =  true;
	}

	if (( !boolState[required_OF_AND_2]) || ( !boolState[S_OF_phase_2]) )
	{
		boolState[required_OF_OR_3]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_2] && ( !boolState[S_OF_AND_2]) )
	{
		boolState[relevant_evt_OF_OR_3]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_phase_1]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1])) || (( !boolState[S_OF_AND_3]) || ( !boolState[S_OF_phase_2])) )
	{
		boolState[relevant_evt_OF_phase_1]  =  true;
	}

	if (( !boolState[required_OF_AND_2]) || ( !boolState[S_OF_phase_1]) )
	{
		boolState[required_OF_phase_2]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_2] && ( !boolState[S_OF_AND_2])) || ( !boolState[S_OF_OR_3]) )
	{
		boolState[relevant_evt_OF_phase_2]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_AND_2]  =  boolState[S_OF_AND_2]  ;



	boolState[already_S_OF_AND_3]  =  boolState[S_OF_AND_3]  ;



	boolState[already_S_OF_Cpt_A]  =  boolState[S_OF_Cpt_A]  ;



	boolState[already_S_OF_Cpt_B]  =  boolState[S_OF_Cpt_B]  ;



	boolState[already_S_OF_Cpt_C]  =  boolState[S_OF_Cpt_C]  ;



	boolState[already_S_OF_OR_1]  =  boolState[S_OF_OR_1]  ;



	boolState[already_S_OF_OR_2]  =  boolState[S_OF_OR_2]  ;



	boolState[already_S_OF_OR_3]  =  boolState[S_OF_OR_3]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;



	boolState[already_S_OF_phase_1]  =  boolState[S_OF_phase_1]  ;

	if ((( !boolState[in_progress_OF_phase_1]) && ( !boolState[required_OF_phase_1])) && boolState[already_required_OF_phase_1] )
	{
		boolState[start_phase_OF_phase_1]  =  true;
	}



	boolState[already_S_OF_phase_2]  =  boolState[S_OF_phase_2]  ;

	if ((( !boolState[in_progress_OF_phase_2]) && ( !boolState[required_OF_phase_2])) && boolState[already_required_OF_phase_2] )
	{
		boolState[start_phase_OF_phase_2]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::runOnceInteractionStep_tops()
{


	boolState[already_required_OF_phase_1]  =  boolState[required_OF_phase_1]  ;



	boolState[already_required_OF_phase_2]  =  boolState[required_OF_phase_2]  ;

}

void
storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_09_Phases_Trim_Article_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    