#include <iostream>
#include "FigaroModelBDMP_07Excl1_Trim_Article_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::init()
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
	REINITIALISATION_OF_required_OF_OR_1 = true;
	boolState[already_S_OF_OR_1] = false;
	REINITIALISATION_OF_S_OF_OR_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_OR_1 = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;
	REINITIALISATION_OF_required_OF_cptA = true;
	boolState[already_S_OF_cptA] = false;
	REINITIALISATION_OF_S_OF_cptA = false;
	REINITIALISATION_OF_relevant_evt_OF_cptA = false;
	boolState[failF_OF_cptA] = false;
	REINITIALISATION_OF_required_OF_cptB = true;
	boolState[already_S_OF_cptB] = false;
	REINITIALISATION_OF_S_OF_cptB = false;
	REINITIALISATION_OF_relevant_evt_OF_cptB = false;
	boolState[failF_OF_cptB] = false;
	REINITIALISATION_OF_required_OF_cptC = true;
	boolState[already_S_OF_cptC] = false;
	REINITIALISATION_OF_S_OF_cptC = false;
	REINITIALISATION_OF_relevant_evt_OF_cptC = false;
	boolState[failF_OF_cptC] = false;
	REINITIALISATION_OF_required_OF_cptD = true;
	boolState[already_S_OF_cptD] = false;
	REINITIALISATION_OF_S_OF_cptD = false;
	REINITIALISATION_OF_relevant_evt_OF_cptD = false;
	boolState[failF_OF_cptD] = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_cptA = false;
	FIRE_xx11_OF_cptA = false;
	FIRE_xx10_OF_cptB = false;
	FIRE_xx11_OF_cptB = false;
	FIRE_xx10_OF_cptC = false;
	FIRE_xx11_OF_cptC = false;
	FIRE_xx10_OF_cptD = false;
	FIRE_xx11_OF_cptD = false;

}

void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::printState()
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
	cout << "Attribute :  boolState[required_OF_OR_1] | Value : " << boolState[required_OF_OR_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_OR_1] | Value : " << boolState[already_S_OF_OR_1] << endl;
	cout << "Attribute :  boolState[S_OF_OR_1] | Value : " << boolState[S_OF_OR_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_OR_1] | Value : " << boolState[relevant_evt_OF_OR_1] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
	cout << "Attribute :  boolState[required_OF_cptA] | Value : " << boolState[required_OF_cptA] << endl;
	cout << "Attribute :  boolState[already_S_OF_cptA] | Value : " << boolState[already_S_OF_cptA] << endl;
	cout << "Attribute :  boolState[S_OF_cptA] | Value : " << boolState[S_OF_cptA] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_cptA] | Value : " << boolState[relevant_evt_OF_cptA] << endl;
	cout << "Attribute :  boolState[failF_OF_cptA] | Value : " << boolState[failF_OF_cptA] << endl;
	cout << "Attribute :  boolState[required_OF_cptB] | Value : " << boolState[required_OF_cptB] << endl;
	cout << "Attribute :  boolState[already_S_OF_cptB] | Value : " << boolState[already_S_OF_cptB] << endl;
	cout << "Attribute :  boolState[S_OF_cptB] | Value : " << boolState[S_OF_cptB] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_cptB] | Value : " << boolState[relevant_evt_OF_cptB] << endl;
	cout << "Attribute :  boolState[failF_OF_cptB] | Value : " << boolState[failF_OF_cptB] << endl;
	cout << "Attribute :  boolState[required_OF_cptC] | Value : " << boolState[required_OF_cptC] << endl;
	cout << "Attribute :  boolState[already_S_OF_cptC] | Value : " << boolState[already_S_OF_cptC] << endl;
	cout << "Attribute :  boolState[S_OF_cptC] | Value : " << boolState[S_OF_cptC] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_cptC] | Value : " << boolState[relevant_evt_OF_cptC] << endl;
	cout << "Attribute :  boolState[failF_OF_cptC] | Value : " << boolState[failF_OF_cptC] << endl;
	cout << "Attribute :  boolState[required_OF_cptD] | Value : " << boolState[required_OF_cptD] << endl;
	cout << "Attribute :  boolState[already_S_OF_cptD] | Value : " << boolState[already_S_OF_cptD] << endl;
	cout << "Attribute :  boolState[S_OF_cptD] | Value : " << boolState[S_OF_cptD] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_cptD] | Value : " << boolState[relevant_evt_OF_cptD] << endl;
	cout << "Attribute :  boolState[failF_OF_cptD] | Value : " << boolState[failF_OF_cptD] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::figaromodelhasinstransitions()
{
	return false;
}
void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_AND_2] = REINITIALISATION_OF_required_OF_AND_2;
	boolState[S_OF_AND_2] = REINITIALISATION_OF_S_OF_AND_2;
	boolState[relevant_evt_OF_AND_2] = REINITIALISATION_OF_relevant_evt_OF_AND_2;
	boolState[required_OF_OR_1] = REINITIALISATION_OF_required_OF_OR_1;
	boolState[S_OF_OR_1] = REINITIALISATION_OF_S_OF_OR_1;
	boolState[relevant_evt_OF_OR_1] = REINITIALISATION_OF_relevant_evt_OF_OR_1;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
	boolState[required_OF_cptA] = REINITIALISATION_OF_required_OF_cptA;
	boolState[S_OF_cptA] = REINITIALISATION_OF_S_OF_cptA;
	boolState[relevant_evt_OF_cptA] = REINITIALISATION_OF_relevant_evt_OF_cptA;
	boolState[required_OF_cptB] = REINITIALISATION_OF_required_OF_cptB;
	boolState[S_OF_cptB] = REINITIALISATION_OF_S_OF_cptB;
	boolState[relevant_evt_OF_cptB] = REINITIALISATION_OF_relevant_evt_OF_cptB;
	boolState[required_OF_cptC] = REINITIALISATION_OF_required_OF_cptC;
	boolState[S_OF_cptC] = REINITIALISATION_OF_S_OF_cptC;
	boolState[relevant_evt_OF_cptC] = REINITIALISATION_OF_relevant_evt_OF_cptC;
	boolState[required_OF_cptD] = REINITIALISATION_OF_required_OF_cptD;
	boolState[S_OF_cptD] = REINITIALISATION_OF_S_OF_cptD;
	boolState[relevant_evt_OF_cptD] = REINITIALISATION_OF_relevant_evt_OF_cptD;
}

void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_cptA = true;
	}

	if (numFire == 1)
	{
		FIRE_xx11_OF_cptA = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_cptB = true;
	}

	if (numFire == 3)
	{
		FIRE_xx11_OF_cptB = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_cptC = true;
	}

	if (numFire == 5)
	{
		FIRE_xx11_OF_cptC = true;
	}

	if (numFire == 6)
	{
		FIRE_xx10_OF_cptD = true;
	}

	if (numFire == 7)
	{
		FIRE_xx11_OF_cptD = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_cptA
	if ((boolState[failF_OF_cptA] == false) && (boolState[required_OF_cptA] &&  boolState[relevant_evt_OF_cptA])) 
	{
		 
		if (FIRE_xx10_OF_cptA)
		{
			boolState[failF_OF_cptA]  =  true;
			FIRE_xx10_OF_cptA = false;
		}
	}

	// Occurrence xx11_OF_cptA
	if (boolState[failF_OF_cptA] == true) 
	{
		 
		if (FIRE_xx11_OF_cptA)
		{
			boolState[failF_OF_cptA]  =  false;
			FIRE_xx11_OF_cptA = false;
		}
	}

	// Occurrence xx10_OF_cptB
	if ((boolState[failF_OF_cptB] == false) && (boolState[required_OF_cptB] &&  boolState[relevant_evt_OF_cptB])) 
	{
		 
		if (FIRE_xx10_OF_cptB)
		{
			boolState[failF_OF_cptB]  =  true;
			FIRE_xx10_OF_cptB = false;
		}
	}

	// Occurrence xx11_OF_cptB
	if (boolState[failF_OF_cptB] == true) 
	{
		 
		if (FIRE_xx11_OF_cptB)
		{
			boolState[failF_OF_cptB]  =  false;
			FIRE_xx11_OF_cptB = false;
		}
	}

	// Occurrence xx10_OF_cptC
	if ((boolState[failF_OF_cptC] == false) && (boolState[required_OF_cptC] &&  boolState[relevant_evt_OF_cptC])) 
	{
		 
		if (FIRE_xx10_OF_cptC)
		{
			boolState[failF_OF_cptC]  =  true;
			FIRE_xx10_OF_cptC = false;
		}
	}

	// Occurrence xx11_OF_cptC
	if (boolState[failF_OF_cptC] == true) 
	{
		 
		if (FIRE_xx11_OF_cptC)
		{
			boolState[failF_OF_cptC]  =  false;
			FIRE_xx11_OF_cptC = false;
		}
	}

	// Occurrence xx10_OF_cptD
	if ((boolState[failF_OF_cptD] == false) && (boolState[required_OF_cptD] &&  boolState[relevant_evt_OF_cptD])) 
	{
		 
		if (FIRE_xx10_OF_cptD)
		{
			boolState[failF_OF_cptD]  =  true;
			FIRE_xx10_OF_cptD = false;
		}
	}

	// Occurrence xx11_OF_cptD
	if (boolState[failF_OF_cptD] == true) 
	{
		 
		if (FIRE_xx11_OF_cptD)
		{
			boolState[failF_OF_cptD]  =  false;
			FIRE_xx11_OF_cptD = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::showFireableOccurrences()
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
     
	if ((boolState[failF_OF_cptA] == false) && (boolState[required_OF_cptA] && boolState[relevant_evt_OF_cptA]))
	{
		cout << "0 : xx10_OF_cptA : FAULT failF  LABEL \"failure in operation cptA\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_cptA]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_cptA] == true)
	{
		cout << "1 : xx11_OF_cptA : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_cptA]  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_cptB] == false) && (boolState[required_OF_cptB] && boolState[relevant_evt_OF_cptB]))
	{
		cout << "2 : xx10_OF_cptB : FAULT failF  LABEL \"failure in operation cptB\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_cptB]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_cptB] == true)
	{
		cout << "3 : xx11_OF_cptB : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_cptB]  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_cptC] == false) && (boolState[required_OF_cptC] && boolState[relevant_evt_OF_cptC]))
	{
		cout << "4 : xx10_OF_cptC : FAULT failF  LABEL \"failure in operation cptC\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_cptC]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_cptC] == true)
	{
		cout << "5 : xx11_OF_cptC : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_cptC]  =  FALSE" << endl;
		list.push_back(make_tuple(5, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_cptD] == false) && (boolState[required_OF_cptD] && boolState[relevant_evt_OF_cptD]))
	{
		cout << "6 : xx10_OF_cptD : FAULT failF  LABEL \"failure in operation cptD\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_cptD]  =  TRUE" << endl;
		list.push_back(make_tuple(6, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_cptD] == true)
	{
		cout << "7 : xx11_OF_cptD : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_cptD]  =  FALSE" << endl;
		list.push_back(make_tuple(7, 0.1, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_cptA] == true )
	{
		boolState[S_OF_cptA]  =  true;
	}

	if (boolState[failF_OF_cptB] == true )
	{
		boolState[S_OF_cptB]  =  true;
	}

	if (boolState[failF_OF_cptC] == true )
	{
		boolState[S_OF_cptC]  =  true;
	}

	if (boolState[failF_OF_cptD] == true )
	{
		boolState[S_OF_cptD]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_cptA] && boolState[S_OF_cptC] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (boolState[S_OF_cptB] && boolState[S_OF_cptD] )
	{
		boolState[S_OF_AND_2]  =  true;
	}

	if (boolState[S_OF_AND_1] || boolState[S_OF_AND_2] )
	{
		boolState[S_OF_OR_1]  =  true;
	}

	if (boolState[S_OF_OR_1] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_OR_1] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if (boolState[relevant_evt_OF_OR_1] && ( !boolState[S_OF_OR_1]) )
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

	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_OR_1]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_OR_1]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_cptB]) )
	{
		boolState[required_OF_cptA]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1])) || (( !boolState[S_OF_cptB]) || ( !boolState[S_OF_cptC])) )
	{
		boolState[relevant_evt_OF_cptA]  =  true;
	}

	if (( !boolState[required_OF_AND_2]) || ( !boolState[S_OF_cptA]) )
	{
		boolState[required_OF_cptB]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_2] && ( !boolState[S_OF_AND_2])) || (( !boolState[S_OF_cptA]) || ( !boolState[S_OF_cptD])) )
	{
		boolState[relevant_evt_OF_cptB]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_cptA]) )
	{
		boolState[required_OF_cptC]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_cptC]  =  true;
	}

	if (( !boolState[required_OF_AND_2]) || ( !boolState[S_OF_cptB]) )
	{
		boolState[required_OF_cptD]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_2] && ( !boolState[S_OF_AND_2]) )
	{
		boolState[relevant_evt_OF_cptD]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_AND_2]  =  boolState[S_OF_AND_2]  ;



	boolState[already_S_OF_OR_1]  =  boolState[S_OF_OR_1]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;



	boolState[already_S_OF_cptA]  =  boolState[S_OF_cptA]  ;



	boolState[already_S_OF_cptB]  =  boolState[S_OF_cptB]  ;



	boolState[already_S_OF_cptC]  =  boolState[S_OF_cptC]  ;



	boolState[already_S_OF_cptD]  =  boolState[S_OF_cptD]  ;

}

void
storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_07Excl1_Trim_Article_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    