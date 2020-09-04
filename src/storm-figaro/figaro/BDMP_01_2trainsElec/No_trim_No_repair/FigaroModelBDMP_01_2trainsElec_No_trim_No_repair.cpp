#include <iostream>
#include "FigaroModelBDMP_01_2trainsElec_No_trim_No_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_AND_1 = true;
	boolState[already_S_OF_AND_1] = false;
	REINITIALISATION_OF_S_OF_AND_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_1 = false;
	REINITIALISATION_OF_required_OF_CB1_IO = true;
	boolState[already_S_OF_CB1_IO] = false;
	REINITIALISATION_OF_S_OF_CB1_IO = false;
	REINITIALISATION_OF_relevant_evt_OF_CB1_IO = false;
	boolState[failF_OF_CB1_IO] = false;
	REINITIALISATION_OF_required_OF_CB1_RO = true;
	boolState[already_S_OF_CB1_RO] = false;
	REINITIALISATION_OF_S_OF_CB1_RO = false;
	REINITIALISATION_OF_relevant_evt_OF_CB1_RO = false;
	boolState[failI_OF_CB1_RO] = false;
	REINITIALISATION_OF_to_be_fired_OF_CB1_RO = false;
	boolState[already_standby_OF_CB1_RO] = false;
	boolState[already_required_OF_CB1_RO] = false;
	REINITIALISATION_OF_required_OF_CB2_RC = true;
	boolState[already_S_OF_CB2_RC] = false;
	REINITIALISATION_OF_S_OF_CB2_RC = false;
	REINITIALISATION_OF_relevant_evt_OF_CB2_RC = false;
	boolState[failI_OF_CB2_RC] = false;
	REINITIALISATION_OF_to_be_fired_OF_CB2_RC = false;
	boolState[already_standby_OF_CB2_RC] = false;
	boolState[already_required_OF_CB2_RC] = false;
	REINITIALISATION_OF_required_OF_Dies_gen = true;
	boolState[already_S_OF_Dies_gen] = false;
	REINITIALISATION_OF_S_OF_Dies_gen = false;
	REINITIALISATION_OF_relevant_evt_OF_Dies_gen = false;
	boolState[failF_OF_Dies_gen] = false;
	REINITIALISATION_OF_required_OF_Dies_gen_RS = true;
	boolState[already_S_OF_Dies_gen_RS] = false;
	REINITIALISATION_OF_S_OF_Dies_gen_RS = false;
	REINITIALISATION_OF_relevant_evt_OF_Dies_gen_RS = false;
	boolState[failI_OF_Dies_gen_RS] = false;
	REINITIALISATION_OF_to_be_fired_OF_Dies_gen_RS = false;
	boolState[already_standby_OF_Dies_gen_RS] = false;
	boolState[already_required_OF_Dies_gen_RS] = false;
	REINITIALISATION_OF_required_OF_Grid = true;
	boolState[already_S_OF_Grid] = false;
	REINITIALISATION_OF_S_OF_Grid = false;
	REINITIALISATION_OF_relevant_evt_OF_Grid = false;
	boolState[failF_OF_Grid] = false;
	REINITIALISATION_OF_required_OF_LossOfDieselLine = true;
	boolState[already_S_OF_LossOfDieselLine] = false;
	REINITIALISATION_OF_S_OF_LossOfDieselLine = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfDieselLine = false;
	REINITIALISATION_OF_required_OF_LossOfLine_1 = true;
	boolState[already_S_OF_LossOfLine_1] = false;
	REINITIALISATION_OF_S_OF_LossOfLine_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfLine_1 = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_CB1_IO = false;
	FIRE_xx23_OF_CB1_RO_INS_1 = false;
	FIRE_xx23_OF_CB1_RO_INS_2 = false;
	FIRE_xx23_OF_CB2_RC_INS_3 = false;
	FIRE_xx23_OF_CB2_RC_INS_4 = false;
	FIRE_xx10_OF_Dies_gen = false;
	FIRE_xx23_OF_Dies_gen_RS_INS_6 = false;
	FIRE_xx23_OF_Dies_gen_RS_INS_7 = false;
	FIRE_xx10_OF_Grid = false;

}

void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_AND_1] | Value : " << boolState[required_OF_AND_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_1] | Value : " << boolState[already_S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[S_OF_AND_1] | Value : " << boolState[S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_1] | Value : " << boolState[relevant_evt_OF_AND_1] << endl;
	cout << "Attribute :  boolState[required_OF_CB1_IO] | Value : " << boolState[required_OF_CB1_IO] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB1_IO] | Value : " << boolState[already_S_OF_CB1_IO] << endl;
	cout << "Attribute :  boolState[S_OF_CB1_IO] | Value : " << boolState[S_OF_CB1_IO] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB1_IO] | Value : " << boolState[relevant_evt_OF_CB1_IO] << endl;
	cout << "Attribute :  boolState[failF_OF_CB1_IO] | Value : " << boolState[failF_OF_CB1_IO] << endl;
	cout << "Attribute :  boolState[required_OF_CB1_RO] | Value : " << boolState[required_OF_CB1_RO] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB1_RO] | Value : " << boolState[already_S_OF_CB1_RO] << endl;
	cout << "Attribute :  boolState[S_OF_CB1_RO] | Value : " << boolState[S_OF_CB1_RO] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB1_RO] | Value : " << boolState[relevant_evt_OF_CB1_RO] << endl;
	cout << "Attribute :  boolState[failI_OF_CB1_RO] | Value : " << boolState[failI_OF_CB1_RO] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_CB1_RO] | Value : " << boolState[to_be_fired_OF_CB1_RO] << endl;
	cout << "Attribute :  boolState[already_standby_OF_CB1_RO] | Value : " << boolState[already_standby_OF_CB1_RO] << endl;
	cout << "Attribute :  boolState[already_required_OF_CB1_RO] | Value : " << boolState[already_required_OF_CB1_RO] << endl;
	cout << "Attribute :  boolState[required_OF_CB2_RC] | Value : " << boolState[required_OF_CB2_RC] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB2_RC] | Value : " << boolState[already_S_OF_CB2_RC] << endl;
	cout << "Attribute :  boolState[S_OF_CB2_RC] | Value : " << boolState[S_OF_CB2_RC] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB2_RC] | Value : " << boolState[relevant_evt_OF_CB2_RC] << endl;
	cout << "Attribute :  boolState[failI_OF_CB2_RC] | Value : " << boolState[failI_OF_CB2_RC] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_CB2_RC] | Value : " << boolState[to_be_fired_OF_CB2_RC] << endl;
	cout << "Attribute :  boolState[already_standby_OF_CB2_RC] | Value : " << boolState[already_standby_OF_CB2_RC] << endl;
	cout << "Attribute :  boolState[already_required_OF_CB2_RC] | Value : " << boolState[already_required_OF_CB2_RC] << endl;
	cout << "Attribute :  boolState[required_OF_Dies_gen] | Value : " << boolState[required_OF_Dies_gen] << endl;
	cout << "Attribute :  boolState[already_S_OF_Dies_gen] | Value : " << boolState[already_S_OF_Dies_gen] << endl;
	cout << "Attribute :  boolState[S_OF_Dies_gen] | Value : " << boolState[S_OF_Dies_gen] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Dies_gen] | Value : " << boolState[relevant_evt_OF_Dies_gen] << endl;
	cout << "Attribute :  boolState[failF_OF_Dies_gen] | Value : " << boolState[failF_OF_Dies_gen] << endl;
	cout << "Attribute :  boolState[required_OF_Dies_gen_RS] | Value : " << boolState[required_OF_Dies_gen_RS] << endl;
	cout << "Attribute :  boolState[already_S_OF_Dies_gen_RS] | Value : " << boolState[already_S_OF_Dies_gen_RS] << endl;
	cout << "Attribute :  boolState[S_OF_Dies_gen_RS] | Value : " << boolState[S_OF_Dies_gen_RS] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Dies_gen_RS] | Value : " << boolState[relevant_evt_OF_Dies_gen_RS] << endl;
	cout << "Attribute :  boolState[failI_OF_Dies_gen_RS] | Value : " << boolState[failI_OF_Dies_gen_RS] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_Dies_gen_RS] | Value : " << boolState[to_be_fired_OF_Dies_gen_RS] << endl;
	cout << "Attribute :  boolState[already_standby_OF_Dies_gen_RS] | Value : " << boolState[already_standby_OF_Dies_gen_RS] << endl;
	cout << "Attribute :  boolState[already_required_OF_Dies_gen_RS] | Value : " << boolState[already_required_OF_Dies_gen_RS] << endl;
	cout << "Attribute :  boolState[required_OF_Grid] | Value : " << boolState[required_OF_Grid] << endl;
	cout << "Attribute :  boolState[already_S_OF_Grid] | Value : " << boolState[already_S_OF_Grid] << endl;
	cout << "Attribute :  boolState[S_OF_Grid] | Value : " << boolState[S_OF_Grid] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Grid] | Value : " << boolState[relevant_evt_OF_Grid] << endl;
	cout << "Attribute :  boolState[failF_OF_Grid] | Value : " << boolState[failF_OF_Grid] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfDieselLine] | Value : " << boolState[required_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfDieselLine] | Value : " << boolState[already_S_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfDieselLine] | Value : " << boolState[S_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfDieselLine] | Value : " << boolState[relevant_evt_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfLine_1] | Value : " << boolState[required_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfLine_1] | Value : " << boolState[already_S_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfLine_1] | Value : " << boolState[S_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfLine_1] | Value : " << boolState[relevant_evt_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_CB1_IO] = REINITIALISATION_OF_required_OF_CB1_IO;
	boolState[S_OF_CB1_IO] = REINITIALISATION_OF_S_OF_CB1_IO;
	boolState[relevant_evt_OF_CB1_IO] = REINITIALISATION_OF_relevant_evt_OF_CB1_IO;
	boolState[required_OF_CB1_RO] = REINITIALISATION_OF_required_OF_CB1_RO;
	boolState[S_OF_CB1_RO] = REINITIALISATION_OF_S_OF_CB1_RO;
	boolState[relevant_evt_OF_CB1_RO] = REINITIALISATION_OF_relevant_evt_OF_CB1_RO;
	boolState[to_be_fired_OF_CB1_RO] = REINITIALISATION_OF_to_be_fired_OF_CB1_RO;
	boolState[required_OF_CB2_RC] = REINITIALISATION_OF_required_OF_CB2_RC;
	boolState[S_OF_CB2_RC] = REINITIALISATION_OF_S_OF_CB2_RC;
	boolState[relevant_evt_OF_CB2_RC] = REINITIALISATION_OF_relevant_evt_OF_CB2_RC;
	boolState[to_be_fired_OF_CB2_RC] = REINITIALISATION_OF_to_be_fired_OF_CB2_RC;
	boolState[required_OF_Dies_gen] = REINITIALISATION_OF_required_OF_Dies_gen;
	boolState[S_OF_Dies_gen] = REINITIALISATION_OF_S_OF_Dies_gen;
	boolState[relevant_evt_OF_Dies_gen] = REINITIALISATION_OF_relevant_evt_OF_Dies_gen;
	boolState[required_OF_Dies_gen_RS] = REINITIALISATION_OF_required_OF_Dies_gen_RS;
	boolState[S_OF_Dies_gen_RS] = REINITIALISATION_OF_S_OF_Dies_gen_RS;
	boolState[relevant_evt_OF_Dies_gen_RS] = REINITIALISATION_OF_relevant_evt_OF_Dies_gen_RS;
	boolState[to_be_fired_OF_Dies_gen_RS] = REINITIALISATION_OF_to_be_fired_OF_Dies_gen_RS;
	boolState[required_OF_Grid] = REINITIALISATION_OF_required_OF_Grid;
	boolState[S_OF_Grid] = REINITIALISATION_OF_S_OF_Grid;
	boolState[relevant_evt_OF_Grid] = REINITIALISATION_OF_relevant_evt_OF_Grid;
	boolState[required_OF_LossOfDieselLine] = REINITIALISATION_OF_required_OF_LossOfDieselLine;
	boolState[S_OF_LossOfDieselLine] = REINITIALISATION_OF_S_OF_LossOfDieselLine;
	boolState[relevant_evt_OF_LossOfDieselLine] = REINITIALISATION_OF_relevant_evt_OF_LossOfDieselLine;
	boolState[required_OF_LossOfLine_1] = REINITIALISATION_OF_required_OF_LossOfLine_1;
	boolState[S_OF_LossOfLine_1] = REINITIALISATION_OF_S_OF_LossOfLine_1;
	boolState[relevant_evt_OF_LossOfLine_1] = REINITIALISATION_OF_relevant_evt_OF_LossOfLine_1;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
}

void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_CB1_IO = true;
	}

	if (numFire == 1)
	{
		FIRE_xx23_OF_CB1_RO_INS_1 = true;
	}

	if (numFire == 2)
	{
		FIRE_xx23_OF_CB1_RO_INS_2 = true;
	}

	if (numFire == 3)
	{
		FIRE_xx23_OF_CB2_RC_INS_3 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx23_OF_CB2_RC_INS_4 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx10_OF_Dies_gen = true;
	}

	if (numFire == 6)
	{
		FIRE_xx23_OF_Dies_gen_RS_INS_6 = true;
	}

	if (numFire == 7)
	{
		FIRE_xx23_OF_Dies_gen_RS_INS_7 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx10_OF_Grid = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_CB1_IO
	if ((boolState[failF_OF_CB1_IO] == false) && boolState[required_OF_CB1_IO]) 
	{
		 
		if (FIRE_xx10_OF_CB1_IO)
		{
			boolState[failF_OF_CB1_IO]  =  true;
			FIRE_xx10_OF_CB1_IO = false;
		}
	}

	// Occurrence xx23_OF_CB1_RO

	if ((boolState[failI_OF_CB1_RO] == false) && boolState[to_be_fired_OF_CB1_RO]) 
	{
	
		
		if (FIRE_xx23_OF_CB1_RO_INS_1) 
		{
			boolState[failI_OF_CB1_RO]  =  true;
			boolState[already_standby_OF_CB1_RO]  =  false;
			boolState[already_required_OF_CB1_RO]  =  false;
			FIRE_xx23_OF_CB1_RO_INS_1 = false;
		}
	
	}
	if ((boolState[failI_OF_CB1_RO] == false) && boolState[to_be_fired_OF_CB1_RO]) 
	{
	
		
		if (FIRE_xx23_OF_CB1_RO_INS_2) 
		{
			boolState[already_standby_OF_CB1_RO]  =  false;
			boolState[already_required_OF_CB1_RO]  =  false;
			FIRE_xx23_OF_CB1_RO_INS_2 = false;
		}
	
	}
	// Occurrence xx23_OF_CB2_RC

	if ((boolState[failI_OF_CB2_RC] == false) && boolState[to_be_fired_OF_CB2_RC]) 
	{
	
		
		if (FIRE_xx23_OF_CB2_RC_INS_3) 
		{
			boolState[failI_OF_CB2_RC]  =  true;
			boolState[already_standby_OF_CB2_RC]  =  false;
			boolState[already_required_OF_CB2_RC]  =  false;
			FIRE_xx23_OF_CB2_RC_INS_3 = false;
		}
	
	}
	if ((boolState[failI_OF_CB2_RC] == false) && boolState[to_be_fired_OF_CB2_RC]) 
	{
	
		
		if (FIRE_xx23_OF_CB2_RC_INS_4) 
		{
			boolState[already_standby_OF_CB2_RC]  =  false;
			boolState[already_required_OF_CB2_RC]  =  false;
			FIRE_xx23_OF_CB2_RC_INS_4 = false;
		}
	
	}
	// Occurrence xx10_OF_Dies_gen
	if ((boolState[failF_OF_Dies_gen] == false) && boolState[required_OF_Dies_gen]) 
	{
		 
		if (FIRE_xx10_OF_Dies_gen)
		{
			boolState[failF_OF_Dies_gen]  =  true;
			FIRE_xx10_OF_Dies_gen = false;
		}
	}

	// Occurrence xx23_OF_Dies_gen_RS

	if ((boolState[failI_OF_Dies_gen_RS] == false) && boolState[to_be_fired_OF_Dies_gen_RS]) 
	{
	
		
		if (FIRE_xx23_OF_Dies_gen_RS_INS_6) 
		{
			boolState[failI_OF_Dies_gen_RS]  =  true;
			boolState[already_standby_OF_Dies_gen_RS]  =  false;
			boolState[already_required_OF_Dies_gen_RS]  =  false;
			FIRE_xx23_OF_Dies_gen_RS_INS_6 = false;
		}
	
	}
	if ((boolState[failI_OF_Dies_gen_RS] == false) && boolState[to_be_fired_OF_Dies_gen_RS]) 
	{
	
		
		if (FIRE_xx23_OF_Dies_gen_RS_INS_7) 
		{
			boolState[already_standby_OF_Dies_gen_RS]  =  false;
			boolState[already_required_OF_Dies_gen_RS]  =  false;
			FIRE_xx23_OF_Dies_gen_RS_INS_7 = false;
		}
	
	}
	// Occurrence xx10_OF_Grid
	if ((boolState[failF_OF_Grid] == false) && boolState[required_OF_Grid]) 
	{
		 
		if (FIRE_xx10_OF_Grid)
		{
			boolState[failF_OF_Grid]  =  true;
			FIRE_xx10_OF_Grid = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if ((boolState[failI_OF_CB1_RO] == false) && boolState[to_be_fired_OF_CB1_RO])
	{
		cout << "1 :  INS_SUB_COUNT (1) |FAULT | failI  LABEL \"instantaneous failure CB1_RO\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_CB1_RO]  =  TRUE,already_standby_OF_CB1_RO  =  FALSE,already_required_OF_CB1_RO  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.0001, "INS", 1));
	}
	if ((boolState[failI_OF_CB1_RO] == false) && boolState[to_be_fired_OF_CB1_RO])
	{
		cout << "2 :  INS_SUB_COUNT (1) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_CB1_RO]  =  FALSE,already_required_OF_CB1_RO  =  FALSE" << endl;
		list.push_back(make_tuple(2, 0.9999, "INS", 1));
	}
	if ((boolState[failI_OF_CB2_RC] == false) && boolState[to_be_fired_OF_CB2_RC])
	{
		cout << "3 :  INS_SUB_COUNT (2) |FAULT | failI  LABEL \"instantaneous failure CB2_RC\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_CB2_RC]  =  TRUE,already_standby_OF_CB2_RC  =  FALSE,already_required_OF_CB2_RC  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.0001, "INS", 2));
	}
	if ((boolState[failI_OF_CB2_RC] == false) && boolState[to_be_fired_OF_CB2_RC])
	{
		cout << "4 :  INS_SUB_COUNT (2) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_CB2_RC]  =  FALSE,already_required_OF_CB2_RC  =  FALSE" << endl;
		list.push_back(make_tuple(4, 0.9999, "INS", 2));
	}
	if ((boolState[failI_OF_Dies_gen_RS] == false) && boolState[to_be_fired_OF_Dies_gen_RS])
	{
		cout << "6 :  INS_SUB_COUNT (3) |FAULT | failI  LABEL \"instantaneous failure Dies_gen_RS\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_Dies_gen_RS]  =  TRUE,already_standby_OF_Dies_gen_RS  =  FALSE,already_required_OF_Dies_gen_RS  =  FALSE" << endl;
		list.push_back(make_tuple(6, 0.0001, "INS", 3));
	}
	if ((boolState[failI_OF_Dies_gen_RS] == false) && boolState[to_be_fired_OF_Dies_gen_RS])
	{
		cout << "7 :  INS_SUB_COUNT (3) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_Dies_gen_RS]  =  FALSE,already_required_OF_Dies_gen_RS  =  FALSE" << endl;
		list.push_back(make_tuple(7, 0.9999, "INS", 3));
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
     
	if ((boolState[failF_OF_CB1_IO] == false) && boolState[required_OF_CB1_IO])
	{
		cout << "0 : xx10_OF_CB1_IO : FAULT failF  LABEL \"failure in operation CB1_IO\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB1_IO]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Dies_gen] == false) && boolState[required_OF_Dies_gen])
	{
		cout << "5 : xx10_OF_Dies_gen : FAULT failF  LABEL \"failure in operation Dies_gen\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Dies_gen]  =  TRUE" << endl;
		list.push_back(make_tuple(5, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Grid] == false) && boolState[required_OF_Grid])
	{
		cout << "8 : xx10_OF_Grid : FAULT failF  LABEL \"failure in operation Grid\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Grid]  =  TRUE" << endl;
		list.push_back(make_tuple(8, 0.0001, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_CB1_IO] == true )
	{
		boolState[S_OF_CB1_IO]  =  true;
	}

	if (boolState[failI_OF_CB1_RO] == true )
	{
		boolState[S_OF_CB1_RO]  =  true;
	}

	if (boolState[failI_OF_CB2_RC] == true )
	{
		boolState[S_OF_CB2_RC]  =  true;
	}

	if (boolState[failF_OF_Dies_gen] == true )
	{
		boolState[S_OF_Dies_gen]  =  true;
	}

	if (boolState[failI_OF_Dies_gen_RS] == true )
	{
		boolState[S_OF_Dies_gen_RS]  =  true;
	}

	if (boolState[failF_OF_Grid] == true )
	{
		boolState[S_OF_Grid]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_LossOfDieselLine] && boolState[S_OF_LossOfLine_1] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (((boolState[S_OF_CB1_RO] || boolState[S_OF_CB2_RC]) || boolState[S_OF_Dies_gen_RS]) || boolState[S_OF_Dies_gen] )
	{
		boolState[S_OF_LossOfDieselLine]  =  true;
	}

	if (boolState[S_OF_CB1_IO] || boolState[S_OF_Grid] )
	{
		boolState[S_OF_LossOfLine_1]  =  true;
	}

	if (boolState[S_OF_AND_1] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_AND_1]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine_1] )
	{
		boolState[required_OF_CB1_IO]  =  false;
	}

	if ((boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1])) || (boolState[relevant_evt_OF_CB1_RO] && ( !boolState[S_OF_CB1_RO])) )
	{
		boolState[relevant_evt_OF_CB1_IO]  =  true;
	}

	if (( !boolState[required_OF_LossOfDieselLine]) || ( !boolState[S_OF_CB1_IO]) )
	{
		boolState[required_OF_CB1_RO]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_CB1_RO]  =  true;
	}

	if ( !boolState[required_OF_LossOfDieselLine] )
	{
		boolState[required_OF_CB2_RC]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_CB2_RC]  =  true;
	}

	if ( !boolState[required_OF_LossOfDieselLine] )
	{
		boolState[required_OF_Dies_gen]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_Dies_gen]  =  true;
	}

	if ( !boolState[required_OF_LossOfDieselLine] )
	{
		boolState[required_OF_Dies_gen_RS]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_Dies_gen_RS]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine_1] )
	{
		boolState[required_OF_Grid]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1]) )
	{
		boolState[relevant_evt_OF_Grid]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_LossOfLine_1]) )
	{
		boolState[required_OF_LossOfDieselLine]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_LossOfDieselLine]  =  true;
	}

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_LossOfLine_1]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1])) || (  boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine])) )
	{
		boolState[relevant_evt_OF_LossOfLine_1]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

}


void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_CB1_IO]  =  boolState[S_OF_CB1_IO]  ;



	boolState[already_S_OF_CB1_RO]  =  boolState[S_OF_CB1_RO]  ;

	if (( !boolState[required_OF_CB1_RO]) && (( !boolState[already_standby_OF_CB1_RO])   && ( !boolState[already_required_OF_CB1_RO])) )
	{
		boolState[already_standby_OF_CB1_RO]  =  true;
	}



	boolState[already_S_OF_CB2_RC]  =  boolState[S_OF_CB2_RC]  ;

	if (( !boolState[required_OF_CB2_RC]) && (( !boolState[already_standby_OF_CB2_RC])   && ( !boolState[already_required_OF_CB2_RC])) )
	{
		boolState[already_standby_OF_CB2_RC]  =  true;
	}



	boolState[already_S_OF_Dies_gen]  =  boolState[S_OF_Dies_gen]  ;



	boolState[already_S_OF_Dies_gen_RS]  =  boolState[S_OF_Dies_gen_RS]  ;

	if (( !boolState[required_OF_Dies_gen_RS]) && (( !boolState[already_standby_OF_Dies_gen_RS]) && ( !boolState[already_required_OF_Dies_gen_RS])) )
	{
		boolState[already_standby_OF_Dies_gen_RS]  =  true;
	}



	boolState[already_S_OF_Grid]  =  boolState[S_OF_Grid]  ;



	boolState[already_S_OF_LossOfDieselLine]  =  boolState[S_OF_LossOfDieselLine]  ;



	boolState[already_S_OF_LossOfLine_1]  =  boolState[S_OF_LossOfLine_1]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;

}


void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::runOnceInteractionStep_tops()
{
	if (boolState[required_OF_CB1_RO] && boolState[already_standby_OF_CB1_RO] )
	{
		boolState[to_be_fired_OF_CB1_RO]  =  true;
	}

	if (boolState[required_OF_CB2_RC] && boolState[already_standby_OF_CB2_RC] )
	{
		boolState[to_be_fired_OF_CB2_RC]  =  true;
	}

	if (boolState[required_OF_Dies_gen_RS] && boolState[already_standby_OF_Dies_gen_RS] )
	{
		boolState[to_be_fired_OF_Dies_gen_RS]  =  true;
	}

}

void
storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_01_2trainsElec_No_trim_No_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    