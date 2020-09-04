#include <iostream>
#include "FigaroModelBDMP_13_Share1_No_trim_No_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_AND_1 = true;
	boolState[already_S_OF_AND_1] = false;
	REINITIALISATION_OF_S_OF_AND_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_1 = false;
	REINITIALISATION_OF_required_OF_FailureOf_A = true;
	boolState[already_S_OF_FailureOf_A] = false;
	REINITIALISATION_OF_S_OF_FailureOf_A = false;
	REINITIALISATION_OF_relevant_evt_OF_FailureOf_A = false;
	boolState[failF_OF_FailureOf_A] = false;
	REINITIALISATION_OF_required_OF_FailureOf_B = true;
	boolState[already_S_OF_FailureOf_B] = false;
	REINITIALISATION_OF_S_OF_FailureOf_B = false;
	REINITIALISATION_OF_relevant_evt_OF_FailureOf_B = false;
	boolState[failF_OF_FailureOf_B] = false;
	REINITIALISATION_OF_required_OF_FailureOf_S = true;
	boolState[already_S_OF_FailureOf_S] = false;
	REINITIALISATION_OF_S_OF_FailureOf_S = false;
	REINITIALISATION_OF_relevant_evt_OF_FailureOf_S = false;
	boolState[failF_OF_FailureOf_S] = false;
	REINITIALISATION_OF_required_OF_FunctionOf_A_lost = true;
	boolState[already_S_OF_FunctionOf_A_lost] = false;
	REINITIALISATION_OF_S_OF_FunctionOf_A_lost = false;
	REINITIALISATION_OF_relevant_evt_OF_FunctionOf_A_lost = false;
	REINITIALISATION_OF_required_OF_OR_3 = true;
	boolState[already_S_OF_OR_3] = false;
	REINITIALISATION_OF_S_OF_OR_3 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_3 = false;
	REINITIALISATION_OF_required_OF_OnDemandFailureOf_S = true;
	boolState[already_S_OF_OnDemandFailureOf_S] = false;
	REINITIALISATION_OF_S_OF_OnDemandFailureOf_S = false;
	REINITIALISATION_OF_relevant_evt_OF_OnDemandFailureOf_S = false;
	boolState[failI_OF_OnDemandFailureOf_S] = false;
	REINITIALISATION_OF_to_be_fired_OF_OnDemandFailureOf_S = false;
	boolState[already_standby_OF_OnDemandFailureOf_S] = false;
	boolState[already_required_OF_OnDemandFailureOf_S] = false;
	REINITIALISATION_OF_required_OF_S_unavailable = true;
	boolState[already_S_OF_S_unavailable] = false;
	REINITIALISATION_OF_S_OF_S_unavailable = false;
	REINITIALISATION_OF_relevant_evt_OF_S_unavailable = false;
	REINITIALISATION_OF_required_OF_THEN_1 = true;
	boolState[already_S_OF_THEN_1] = false;
	REINITIALISATION_OF_S_OF_THEN_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_THEN_1 = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_FailureOf_A = false;
	FIRE_xx10_OF_FailureOf_B = false;
	FIRE_xx10_OF_FailureOf_S = false;
	FIRE_xx23_OF_OnDemandFailureOf_S_INS_3 = false;
	FIRE_xx23_OF_OnDemandFailureOf_S_INS_4 = false;

}

void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_AND_1] | Value : " << boolState[required_OF_AND_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_1] | Value : " << boolState[already_S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[S_OF_AND_1] | Value : " << boolState[S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_1] | Value : " << boolState[relevant_evt_OF_AND_1] << endl;
	cout << "Attribute :  boolState[required_OF_FailureOf_A] | Value : " << boolState[required_OF_FailureOf_A] << endl;
	cout << "Attribute :  boolState[already_S_OF_FailureOf_A] | Value : " << boolState[already_S_OF_FailureOf_A] << endl;
	cout << "Attribute :  boolState[S_OF_FailureOf_A] | Value : " << boolState[S_OF_FailureOf_A] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_FailureOf_A] | Value : " << boolState[relevant_evt_OF_FailureOf_A] << endl;
	cout << "Attribute :  boolState[failF_OF_FailureOf_A] | Value : " << boolState[failF_OF_FailureOf_A] << endl;
	cout << "Attribute :  boolState[required_OF_FailureOf_B] | Value : " << boolState[required_OF_FailureOf_B] << endl;
	cout << "Attribute :  boolState[already_S_OF_FailureOf_B] | Value : " << boolState[already_S_OF_FailureOf_B] << endl;
	cout << "Attribute :  boolState[S_OF_FailureOf_B] | Value : " << boolState[S_OF_FailureOf_B] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_FailureOf_B] | Value : " << boolState[relevant_evt_OF_FailureOf_B] << endl;
	cout << "Attribute :  boolState[failF_OF_FailureOf_B] | Value : " << boolState[failF_OF_FailureOf_B] << endl;
	cout << "Attribute :  boolState[required_OF_FailureOf_S] | Value : " << boolState[required_OF_FailureOf_S] << endl;
	cout << "Attribute :  boolState[already_S_OF_FailureOf_S] | Value : " << boolState[already_S_OF_FailureOf_S] << endl;
	cout << "Attribute :  boolState[S_OF_FailureOf_S] | Value : " << boolState[S_OF_FailureOf_S] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_FailureOf_S] | Value : " << boolState[relevant_evt_OF_FailureOf_S] << endl;
	cout << "Attribute :  boolState[failF_OF_FailureOf_S] | Value : " << boolState[failF_OF_FailureOf_S] << endl;
	cout << "Attribute :  boolState[required_OF_FunctionOf_A_lost] | Value : " << boolState[required_OF_FunctionOf_A_lost] << endl;
	cout << "Attribute :  boolState[already_S_OF_FunctionOf_A_lost] | Value : " << boolState[already_S_OF_FunctionOf_A_lost] << endl;
	cout << "Attribute :  boolState[S_OF_FunctionOf_A_lost] | Value : " << boolState[S_OF_FunctionOf_A_lost] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_FunctionOf_A_lost] | Value : " << boolState[relevant_evt_OF_FunctionOf_A_lost] << endl;
	cout << "Attribute :  boolState[required_OF_OR_3] | Value : " << boolState[required_OF_OR_3] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_3] | Value : " << boolState[already_S_OF_OR_3] << endl;
	cout << "Attribute :  boolState[S_OF_OR_3] | Value : " << boolState[S_OF_OR_3] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_3] | Value : " << boolState[relevant_evt_OF_OR_3] << endl;
	cout << "Attribute :  boolState[required_OF_OnDemandFailureOf_S] | Value : " << boolState[required_OF_OnDemandFailureOf_S] << endl;
	cout << "Attribute :  boolState[already_S_OF_OnDemandFailureOf_S] | Value : " << boolState[already_S_OF_OnDemandFailureOf_S] << endl;
	cout << "Attribute :  boolState[S_OF_OnDemandFailureOf_S] | Value : " << boolState[S_OF_OnDemandFailureOf_S] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OnDemandFailureOf_S] | Value : " << boolState[relevant_evt_OF_OnDemandFailureOf_S] << endl;
	cout << "Attribute :  boolState[failI_OF_OnDemandFailureOf_S] | Value : " << boolState[failI_OF_OnDemandFailureOf_S] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_OnDemandFailureOf_S] | Value : " << boolState[to_be_fired_OF_OnDemandFailureOf_S] << endl;
	cout << "Attribute :  boolState[already_standby_OF_OnDemandFailureOf_S] | Value : " << boolState[already_standby_OF_OnDemandFailureOf_S] << endl;
	cout << "Attribute :  boolState[already_required_OF_OnDemandFailureOf_S] | Value : " << boolState[already_required_OF_OnDemandFailureOf_S] << endl;
	cout << "Attribute :  boolState[required_OF_S_unavailable] | Value : " << boolState[required_OF_S_unavailable] << endl;
	cout << "Attribute :  boolState[already_S_OF_S_unavailable] | Value : " << boolState[already_S_OF_S_unavailable] << endl;
	cout << "Attribute :  boolState[S_OF_S_unavailable] | Value : " << boolState[S_OF_S_unavailable] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_S_unavailable] | Value : " << boolState[relevant_evt_OF_S_unavailable] << endl;
	cout << "Attribute :  boolState[required_OF_THEN_1] | Value : " << boolState[required_OF_THEN_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_THEN_1] | Value : " << boolState[already_S_OF_THEN_1] << endl;
	cout << "Attribute :  boolState[S_OF_THEN_1] | Value : " << boolState[S_OF_THEN_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_THEN_1] | Value : " << boolState[relevant_evt_OF_THEN_1] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_FailureOf_A] = REINITIALISATION_OF_required_OF_FailureOf_A;
	boolState[S_OF_FailureOf_A] = REINITIALISATION_OF_S_OF_FailureOf_A;
	boolState[relevant_evt_OF_FailureOf_A] = REINITIALISATION_OF_relevant_evt_OF_FailureOf_A;
	boolState[required_OF_FailureOf_B] = REINITIALISATION_OF_required_OF_FailureOf_B;
	boolState[S_OF_FailureOf_B] = REINITIALISATION_OF_S_OF_FailureOf_B;
	boolState[relevant_evt_OF_FailureOf_B] = REINITIALISATION_OF_relevant_evt_OF_FailureOf_B;
	boolState[required_OF_FailureOf_S] = REINITIALISATION_OF_required_OF_FailureOf_S;
	boolState[S_OF_FailureOf_S] = REINITIALISATION_OF_S_OF_FailureOf_S;
	boolState[relevant_evt_OF_FailureOf_S] = REINITIALISATION_OF_relevant_evt_OF_FailureOf_S;
	boolState[required_OF_FunctionOf_A_lost] = REINITIALISATION_OF_required_OF_FunctionOf_A_lost;
	boolState[S_OF_FunctionOf_A_lost] = REINITIALISATION_OF_S_OF_FunctionOf_A_lost;
	boolState[relevant_evt_OF_FunctionOf_A_lost] = REINITIALISATION_OF_relevant_evt_OF_FunctionOf_A_lost;
	boolState[required_OF_OR_3] = REINITIALISATION_OF_required_OF_OR_3;
	boolState[S_OF_OR_3] = REINITIALISATION_OF_S_OF_OR_3;
	boolState[relevant_evt_OF_OR_3] = REINITIALISATION_OF_relevant_evt_OF_OR_3;
	boolState[required_OF_OnDemandFailureOf_S] = REINITIALISATION_OF_required_OF_OnDemandFailureOf_S;
	boolState[S_OF_OnDemandFailureOf_S] = REINITIALISATION_OF_S_OF_OnDemandFailureOf_S;
	boolState[relevant_evt_OF_OnDemandFailureOf_S] = REINITIALISATION_OF_relevant_evt_OF_OnDemandFailureOf_S;
	boolState[to_be_fired_OF_OnDemandFailureOf_S] = REINITIALISATION_OF_to_be_fired_OF_OnDemandFailureOf_S;
	boolState[required_OF_S_unavailable] = REINITIALISATION_OF_required_OF_S_unavailable;
	boolState[S_OF_S_unavailable] = REINITIALISATION_OF_S_OF_S_unavailable;
	boolState[relevant_evt_OF_S_unavailable] = REINITIALISATION_OF_relevant_evt_OF_S_unavailable;
	boolState[required_OF_THEN_1] = REINITIALISATION_OF_required_OF_THEN_1;
	boolState[S_OF_THEN_1] = REINITIALISATION_OF_S_OF_THEN_1;
	boolState[relevant_evt_OF_THEN_1] = REINITIALISATION_OF_relevant_evt_OF_THEN_1;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
}

void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_FailureOf_A = true;
	}

	if (numFire == 1)
	{
		FIRE_xx10_OF_FailureOf_B = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_FailureOf_S = true;
	}

	if (numFire == 3)
	{
		FIRE_xx23_OF_OnDemandFailureOf_S_INS_3 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx23_OF_OnDemandFailureOf_S_INS_4 = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_FailureOf_A
	if ((boolState[failF_OF_FailureOf_A] == false) && boolState[required_OF_FailureOf_A]) 
	{
		 
		if (FIRE_xx10_OF_FailureOf_A)
		{
			boolState[failF_OF_FailureOf_A]  =  true;
			FIRE_xx10_OF_FailureOf_A = false;
		}
	}

	// Occurrence xx10_OF_FailureOf_B
	if ((boolState[failF_OF_FailureOf_B] == false) && boolState[required_OF_FailureOf_B]) 
	{
		 
		if (FIRE_xx10_OF_FailureOf_B)
		{
			boolState[failF_OF_FailureOf_B]  =  true;
			FIRE_xx10_OF_FailureOf_B = false;
		}
	}

	// Occurrence xx10_OF_FailureOf_S
	if ((boolState[failF_OF_FailureOf_S] == false) && boolState[required_OF_FailureOf_S]) 
	{
		 
		if (FIRE_xx10_OF_FailureOf_S)
		{
			boolState[failF_OF_FailureOf_S]  =  true;
			FIRE_xx10_OF_FailureOf_S = false;
		}
	}

	// Occurrence xx23_OF_OnDemandFailureOf_S

	if ((boolState[failI_OF_OnDemandFailureOf_S] == false) && boolState[to_be_fired_OF_OnDemandFailureOf_S]) 
	{
	
		
		if (FIRE_xx23_OF_OnDemandFailureOf_S_INS_3) 
		{
			boolState[failI_OF_OnDemandFailureOf_S]  =  true;
			boolState[already_standby_OF_OnDemandFailureOf_S]  =  false;
			boolState[already_required_OF_OnDemandFailureOf_S]  =  false;
			FIRE_xx23_OF_OnDemandFailureOf_S_INS_3 = false;
		}
	
	}
	if ((boolState[failI_OF_OnDemandFailureOf_S] == false) && boolState[to_be_fired_OF_OnDemandFailureOf_S]) 
	{
	
		
		if (FIRE_xx23_OF_OnDemandFailureOf_S_INS_4) 
		{
			boolState[already_standby_OF_OnDemandFailureOf_S]  =  false;
			boolState[already_required_OF_OnDemandFailureOf_S]  =  false;
			FIRE_xx23_OF_OnDemandFailureOf_S_INS_4 = false;
		}
	
	}
}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if ((boolState[failI_OF_OnDemandFailureOf_S] == false) && boolState[to_be_fired_OF_OnDemandFailureOf_S])
	{
		cout << "3 :  INS_SUB_COUNT (1) |FAULT | failI  LABEL \"instantaneous failure OnDemandFailureOf_S\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_OnDemandFailureOf_S]  =  TRUE,already_standby_OF_OnDemandFailureOf_S  =  FALSE,already_required_OF_OnDemandFailureOf_S  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.0001, "INS", 1));
	}
	if ((boolState[failI_OF_OnDemandFailureOf_S] == false) && boolState[to_be_fired_OF_OnDemandFailureOf_S])
	{
		cout << "4 :  INS_SUB_COUNT (1) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_OnDemandFailureOf_S]  =  FALSE,already_required_OF_OnDemandFailureOf_S  =  FALSE" << endl;
		list.push_back(make_tuple(4, 0.9999, "INS", 1));
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
     
	if ((boolState[failF_OF_FailureOf_A] == false) && boolState[required_OF_FailureOf_A])
	{
		cout << "0 : xx10_OF_FailureOf_A : FAULT failF  LABEL \"failure in operation FailureOf_A\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_FailureOf_A]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_FailureOf_B] == false) && boolState[required_OF_FailureOf_B])
	{
		cout << "1 : xx10_OF_FailureOf_B : FAULT failF  LABEL \"failure in operation FailureOf_B\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_FailureOf_B]  =  TRUE" << endl;
		list.push_back(make_tuple(1, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_FailureOf_S] == false) && boolState[required_OF_FailureOf_S])
	{
		cout << "2 : xx10_OF_FailureOf_S : FAULT failF  LABEL \"failure in operation FailureOf_S\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_FailureOf_S]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_FailureOf_A] == true )
	{
		boolState[S_OF_FailureOf_A]  =  true;
	}

	if (boolState[failF_OF_FailureOf_B] == true )
	{
		boolState[S_OF_FailureOf_B]  =  true;
	}

	if (boolState[failF_OF_FailureOf_S] == true )
	{
		boolState[S_OF_FailureOf_S]  =  true;
	}

	if (boolState[failI_OF_OnDemandFailureOf_S] == true )
	{
		boolState[S_OF_OnDemandFailureOf_S]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_FailureOf_A] && boolState[S_OF_S_unavailable] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (boolState[S_OF_AND_1] || boolState[S_OF_THEN_1] )
	{
		boolState[S_OF_FunctionOf_A_lost]  =  true;
	}

	if (boolState[S_OF_FailureOf_A] || boolState[S_OF_FailureOf_B] )
	{
		boolState[S_OF_OR_3]  =  true;
	}

	if (boolState[S_OF_FailureOf_S] || boolState[S_OF_OnDemandFailureOf_S] )
	{
		boolState[S_OF_S_unavailable]  =  true;
	}

	if (((boolState[S_OF_FailureOf_A] && boolState[S_OF_FailureOf_B]) && boolState[already_S_OF_FailureOf_B]) && ( !boolState[already_S_OF_FailureOf_A]) )
	{
		boolState[S_OF_THEN_1]  =  true;
	}

	if (boolState[already_S_OF_THEN_1] && (boolState[S_OF_FailureOf_B] && boolState[S_OF_FailureOf_A]) )
	{
		boolState[S_OF_THEN_1]  =  true;
	}

	if (boolState[S_OF_FunctionOf_A_lost] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_FunctionOf_A_lost] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if (boolState[relevant_evt_OF_FunctionOf_A_lost] && ( !boolState[S_OF_FunctionOf_A_lost]) )
	{
		boolState[relevant_evt_OF_AND_1]  =  true;
	}

	if ((( !boolState[required_OF_THEN_1]) && ( !boolState[required_OF_OR_3])) && (   !boolState[required_OF_AND_1]) )
	{
		boolState[required_OF_FailureOf_A]  =  false;
	}

	if (((boolState[relevant_evt_OF_THEN_1] && ( !boolState[S_OF_THEN_1])) || (  boolState[relevant_evt_OF_OR_3] && ( !boolState[S_OF_OR_3]))) || (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1])) )
	{
		boolState[relevant_evt_OF_FailureOf_A]  =  true;
	}

	if (( !boolState[required_OF_OR_3]) && ( !boolState[required_OF_THEN_1]) )
	{
		boolState[required_OF_FailureOf_B]  =  false;
	}

	if ((boolState[relevant_evt_OF_OR_3] && ( !boolState[S_OF_OR_3])) || (boolState[relevant_evt_OF_THEN_1] && ( !boolState[S_OF_THEN_1])) )
	{
		boolState[relevant_evt_OF_FailureOf_B]  =  true;
	}

	if ( !boolState[required_OF_S_unavailable] )
	{
		boolState[required_OF_FailureOf_S]  =  false;
	}

	if (boolState[relevant_evt_OF_S_unavailable] && ( !boolState[S_OF_S_unavailable]) )
	{
		boolState[relevant_evt_OF_FailureOf_S]  =  true;
	}

	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_FunctionOf_A_lost]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_FunctionOf_A_lost]  =  true;
	}

	if (boolState[relevant_evt_OF_S_unavailable] && ( !boolState[S_OF_S_unavailable]) )
	{
		boolState[relevant_evt_OF_OR_3]  =  true;
	}

	if ( !boolState[required_OF_S_unavailable] )
	{
		boolState[required_OF_OnDemandFailureOf_S]  =  false;
	}

	if (boolState[relevant_evt_OF_S_unavailable] && ( !boolState[S_OF_S_unavailable]) )
	{
		boolState[relevant_evt_OF_OnDemandFailureOf_S]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_OR_3]) )
	{
		boolState[required_OF_S_unavailable]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_S_unavailable]  =  true;
	}

	if ( !boolState[required_OF_FunctionOf_A_lost] )
	{
		boolState[required_OF_THEN_1]  =  false;
	}

	if (boolState[relevant_evt_OF_FunctionOf_A_lost] && ( !boolState[S_OF_FunctionOf_A_lost]) )
	{
		boolState[relevant_evt_OF_THEN_1]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

}


void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_FailureOf_A]  =  boolState[S_OF_FailureOf_A]  ;



	boolState[already_S_OF_FailureOf_B]  =  boolState[S_OF_FailureOf_B]  ;



	boolState[already_S_OF_FailureOf_S]  =  boolState[S_OF_FailureOf_S]  ;



	boolState[already_S_OF_FunctionOf_A_lost]  =  boolState[S_OF_FunctionOf_A_lost]  ;



	boolState[already_S_OF_OR_3]  =  boolState[S_OF_OR_3]  ;



	boolState[already_S_OF_OnDemandFailureOf_S]  =  boolState[S_OF_OnDemandFailureOf_S]  ;

	if (( !boolState[required_OF_OnDemandFailureOf_S]) && (( !  boolState[already_standby_OF_OnDemandFailureOf_S]) && ( !  boolState[already_required_OF_OnDemandFailureOf_S])) )
	{
		boolState[already_standby_OF_OnDemandFailureOf_S]  =  true;
	}



	boolState[already_S_OF_S_unavailable]  =  boolState[S_OF_S_unavailable]  ;



	boolState[already_S_OF_THEN_1]  =  boolState[S_OF_THEN_1]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;

}


void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::runOnceInteractionStep_tops()
{
	if (boolState[required_OF_OnDemandFailureOf_S] && boolState[already_standby_OF_OnDemandFailureOf_S] )
	{
		boolState[to_be_fired_OF_OnDemandFailureOf_S]  =  true;
	}

}

void
storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_13_Share1_No_trim_No_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    