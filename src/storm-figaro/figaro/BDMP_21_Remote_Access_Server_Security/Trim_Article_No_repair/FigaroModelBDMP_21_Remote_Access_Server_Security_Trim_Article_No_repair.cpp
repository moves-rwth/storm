#include <iostream>
#include "FigaroModelBDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_Authentication_with_password = true;
	boolState[already_S_OF_Authentication_with_password] = false;
	REINITIALISATION_OF_S_OF_Authentication_with_password = false;
	REINITIALISATION_OF_relevant_evt_OF_Authentication_with_password = false;
	REINITIALISATION_OF_required_OF_Bruteforce = true;
	boolState[already_S_OF_Bruteforce] = false;
	REINITIALISATION_OF_S_OF_Bruteforce = false;
	REINITIALISATION_OF_relevant_evt_OF_Bruteforce = false;
	boolState[failF_OF_Bruteforce] = false;
	REINITIALISATION_OF_required_OF_Exploit_vulnerability = true;
	boolState[already_S_OF_Exploit_vulnerability] = false;
	REINITIALISATION_OF_S_OF_Exploit_vulnerability = false;
	REINITIALISATION_OF_relevant_evt_OF_Exploit_vulnerability = false;
	boolState[failF_OF_Exploit_vulnerability] = false;
	REINITIALISATION_OF_required_OF_Find_vulnerability = true;
	boolState[already_S_OF_Find_vulnerability] = false;
	REINITIALISATION_OF_S_OF_Find_vulnerability = false;
	REINITIALISATION_OF_relevant_evt_OF_Find_vulnerability = false;
	boolState[failF_OF_Find_vulnerability] = false;
	REINITIALISATION_OF_required_OF_Logged_into_the_RAS = true;
	boolState[already_S_OF_Logged_into_the_RAS] = false;
	REINITIALISATION_OF_S_OF_Logged_into_the_RAS = false;
	REINITIALISATION_OF_relevant_evt_OF_Logged_into_the_RAS = false;
	REINITIALISATION_OF_required_OF_RAS_access_granted = true;
	boolState[already_S_OF_RAS_access_granted] = false;
	REINITIALISATION_OF_S_OF_RAS_access_granted = false;
	REINITIALISATION_OF_relevant_evt_OF_RAS_access_granted = false;
	REINITIALISATION_OF_required_OF_RAS_ownership = true;
	boolState[already_S_OF_RAS_ownership] = false;
	REINITIALISATION_OF_S_OF_RAS_ownership = false;
	REINITIALISATION_OF_relevant_evt_OF_RAS_ownership = false;
	REINITIALISATION_OF_required_OF_Social_engineering = true;
	boolState[already_S_OF_Social_engineering] = false;
	REINITIALISATION_OF_S_OF_Social_engineering = false;
	REINITIALISATION_OF_relevant_evt_OF_Social_engineering = false;
	boolState[failF_OF_Social_engineering] = false;
	REINITIALISATION_OF_required_OF_Vulnerability_found_and_exploited = true;
	boolState[already_S_OF_Vulnerability_found_and_exploited] = false;
	REINITIALISATION_OF_S_OF_Vulnerability_found_and_exploited = false;
	REINITIALISATION_OF_relevant_evt_OF_Vulnerability_found_and_exploited = false;
	REINITIALISATION_OF_required_OF_Wardialing = true;
	boolState[already_S_OF_Wardialing] = false;
	REINITIALISATION_OF_S_OF_Wardialing = false;
	REINITIALISATION_OF_relevant_evt_OF_Wardialing = false;
	boolState[failF_OF_Wardialing] = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_Bruteforce = false;
	FIRE_xx10_OF_Exploit_vulnerability = false;
	FIRE_xx10_OF_Find_vulnerability = false;
	FIRE_xx10_OF_Social_engineering = false;
	FIRE_xx10_OF_Wardialing = false;

}

void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_Authentication_with_password] | Value : " << boolState[required_OF_Authentication_with_password] << endl;
	cout << "Attribute :  boolState[already_S_OF_Authentication_with_password] | Value : " << boolState[already_S_OF_Authentication_with_password] << endl;
	cout << "Attribute :  boolState[S_OF_Authentication_with_password] | Value : " << boolState[S_OF_Authentication_with_password] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Authentication_with_password] | Value : " << boolState[relevant_evt_OF_Authentication_with_password] << endl;
	cout << "Attribute :  boolState[required_OF_Bruteforce] | Value : " << boolState[required_OF_Bruteforce] << endl;
	cout << "Attribute :  boolState[already_S_OF_Bruteforce] | Value : " << boolState[already_S_OF_Bruteforce] << endl;
	cout << "Attribute :  boolState[S_OF_Bruteforce] | Value : " << boolState[S_OF_Bruteforce] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Bruteforce] | Value : " << boolState[relevant_evt_OF_Bruteforce] << endl;
	cout << "Attribute :  boolState[failF_OF_Bruteforce] | Value : " << boolState[failF_OF_Bruteforce] << endl;
	cout << "Attribute :  boolState[required_OF_Exploit_vulnerability] | Value : " << boolState[required_OF_Exploit_vulnerability] << endl;
	cout << "Attribute :  boolState[already_S_OF_Exploit_vulnerability] | Value : " << boolState[already_S_OF_Exploit_vulnerability] << endl;
	cout << "Attribute :  boolState[S_OF_Exploit_vulnerability] | Value : " << boolState[S_OF_Exploit_vulnerability] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Exploit_vulnerability] | Value : " << boolState[relevant_evt_OF_Exploit_vulnerability] << endl;
	cout << "Attribute :  boolState[failF_OF_Exploit_vulnerability] | Value : " << boolState[failF_OF_Exploit_vulnerability] << endl;
	cout << "Attribute :  boolState[required_OF_Find_vulnerability] | Value : " << boolState[required_OF_Find_vulnerability] << endl;
	cout << "Attribute :  boolState[already_S_OF_Find_vulnerability] | Value : " << boolState[already_S_OF_Find_vulnerability] << endl;
	cout << "Attribute :  boolState[S_OF_Find_vulnerability] | Value : " << boolState[S_OF_Find_vulnerability] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Find_vulnerability] | Value : " << boolState[relevant_evt_OF_Find_vulnerability] << endl;
	cout << "Attribute :  boolState[failF_OF_Find_vulnerability] | Value : " << boolState[failF_OF_Find_vulnerability] << endl;
	cout << "Attribute :  boolState[required_OF_Logged_into_the_RAS] | Value : " << boolState[required_OF_Logged_into_the_RAS] << endl;
	cout << "Attribute :  boolState[already_S_OF_Logged_into_the_RAS] | Value : " << boolState[already_S_OF_Logged_into_the_RAS] << endl;
	cout << "Attribute :  boolState[S_OF_Logged_into_the_RAS] | Value : " << boolState[S_OF_Logged_into_the_RAS] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Logged_into_the_RAS] | Value : " << boolState[relevant_evt_OF_Logged_into_the_RAS] << endl;
	cout << "Attribute :  boolState[required_OF_RAS_access_granted] | Value : " << boolState[required_OF_RAS_access_granted] << endl;
	cout << "Attribute :  boolState[already_S_OF_RAS_access_granted] | Value : " << boolState[already_S_OF_RAS_access_granted] << endl;
	cout << "Attribute :  boolState[S_OF_RAS_access_granted] | Value : " << boolState[S_OF_RAS_access_granted] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RAS_access_granted] | Value : " << boolState[relevant_evt_OF_RAS_access_granted] << endl;
	cout << "Attribute :  boolState[required_OF_RAS_ownership] | Value : " << boolState[required_OF_RAS_ownership] << endl;
	cout << "Attribute :  boolState[already_S_OF_RAS_ownership] | Value : " << boolState[already_S_OF_RAS_ownership] << endl;
	cout << "Attribute :  boolState[S_OF_RAS_ownership] | Value : " << boolState[S_OF_RAS_ownership] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RAS_ownership] | Value : " << boolState[relevant_evt_OF_RAS_ownership] << endl;
	cout << "Attribute :  boolState[required_OF_Social_engineering] | Value : " << boolState[required_OF_Social_engineering] << endl;
	cout << "Attribute :  boolState[already_S_OF_Social_engineering] | Value : " << boolState[already_S_OF_Social_engineering] << endl;
	cout << "Attribute :  boolState[S_OF_Social_engineering] | Value : " << boolState[S_OF_Social_engineering] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Social_engineering] | Value : " << boolState[relevant_evt_OF_Social_engineering] << endl;
	cout << "Attribute :  boolState[failF_OF_Social_engineering] | Value : " << boolState[failF_OF_Social_engineering] << endl;
	cout << "Attribute :  boolState[required_OF_Vulnerability_found_and_exploited] | Value : " << boolState[required_OF_Vulnerability_found_and_exploited] << endl;
	cout << "Attribute :  boolState[already_S_OF_Vulnerability_found_and_exploited] | Value : " << boolState[already_S_OF_Vulnerability_found_and_exploited] << endl;
	cout << "Attribute :  boolState[S_OF_Vulnerability_found_and_exploited] | Value : " << boolState[S_OF_Vulnerability_found_and_exploited] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Vulnerability_found_and_exploited] | Value : " << boolState[relevant_evt_OF_Vulnerability_found_and_exploited] << endl;
	cout << "Attribute :  boolState[required_OF_Wardialing] | Value : " << boolState[required_OF_Wardialing] << endl;
	cout << "Attribute :  boolState[already_S_OF_Wardialing] | Value : " << boolState[already_S_OF_Wardialing] << endl;
	cout << "Attribute :  boolState[S_OF_Wardialing] | Value : " << boolState[S_OF_Wardialing] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Wardialing] | Value : " << boolState[relevant_evt_OF_Wardialing] << endl;
	cout << "Attribute :  boolState[failF_OF_Wardialing] | Value : " << boolState[failF_OF_Wardialing] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::figaromodelhasinstransitions()
{
	return false;
}
void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::doReinitialisations()
{
	boolState[required_OF_Authentication_with_password] = REINITIALISATION_OF_required_OF_Authentication_with_password;
	boolState[S_OF_Authentication_with_password] = REINITIALISATION_OF_S_OF_Authentication_with_password;
	boolState[relevant_evt_OF_Authentication_with_password] = REINITIALISATION_OF_relevant_evt_OF_Authentication_with_password;
	boolState[required_OF_Bruteforce] = REINITIALISATION_OF_required_OF_Bruteforce;
	boolState[S_OF_Bruteforce] = REINITIALISATION_OF_S_OF_Bruteforce;
	boolState[relevant_evt_OF_Bruteforce] = REINITIALISATION_OF_relevant_evt_OF_Bruteforce;
	boolState[required_OF_Exploit_vulnerability] = REINITIALISATION_OF_required_OF_Exploit_vulnerability;
	boolState[S_OF_Exploit_vulnerability] = REINITIALISATION_OF_S_OF_Exploit_vulnerability;
	boolState[relevant_evt_OF_Exploit_vulnerability] = REINITIALISATION_OF_relevant_evt_OF_Exploit_vulnerability;
	boolState[required_OF_Find_vulnerability] = REINITIALISATION_OF_required_OF_Find_vulnerability;
	boolState[S_OF_Find_vulnerability] = REINITIALISATION_OF_S_OF_Find_vulnerability;
	boolState[relevant_evt_OF_Find_vulnerability] = REINITIALISATION_OF_relevant_evt_OF_Find_vulnerability;
	boolState[required_OF_Logged_into_the_RAS] = REINITIALISATION_OF_required_OF_Logged_into_the_RAS;
	boolState[S_OF_Logged_into_the_RAS] = REINITIALISATION_OF_S_OF_Logged_into_the_RAS;
	boolState[relevant_evt_OF_Logged_into_the_RAS] = REINITIALISATION_OF_relevant_evt_OF_Logged_into_the_RAS;
	boolState[required_OF_RAS_access_granted] = REINITIALISATION_OF_required_OF_RAS_access_granted;
	boolState[S_OF_RAS_access_granted] = REINITIALISATION_OF_S_OF_RAS_access_granted;
	boolState[relevant_evt_OF_RAS_access_granted] = REINITIALISATION_OF_relevant_evt_OF_RAS_access_granted;
	boolState[required_OF_RAS_ownership] = REINITIALISATION_OF_required_OF_RAS_ownership;
	boolState[S_OF_RAS_ownership] = REINITIALISATION_OF_S_OF_RAS_ownership;
	boolState[relevant_evt_OF_RAS_ownership] = REINITIALISATION_OF_relevant_evt_OF_RAS_ownership;
	boolState[required_OF_Social_engineering] = REINITIALISATION_OF_required_OF_Social_engineering;
	boolState[S_OF_Social_engineering] = REINITIALISATION_OF_S_OF_Social_engineering;
	boolState[relevant_evt_OF_Social_engineering] = REINITIALISATION_OF_relevant_evt_OF_Social_engineering;
	boolState[required_OF_Vulnerability_found_and_exploited] = REINITIALISATION_OF_required_OF_Vulnerability_found_and_exploited;
	boolState[S_OF_Vulnerability_found_and_exploited] = REINITIALISATION_OF_S_OF_Vulnerability_found_and_exploited;
	boolState[relevant_evt_OF_Vulnerability_found_and_exploited] = REINITIALISATION_OF_relevant_evt_OF_Vulnerability_found_and_exploited;
	boolState[required_OF_Wardialing] = REINITIALISATION_OF_required_OF_Wardialing;
	boolState[S_OF_Wardialing] = REINITIALISATION_OF_S_OF_Wardialing;
	boolState[relevant_evt_OF_Wardialing] = REINITIALISATION_OF_relevant_evt_OF_Wardialing;
}

void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_Bruteforce = true;
	}

	if (numFire == 1)
	{
		FIRE_xx10_OF_Exploit_vulnerability = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_Find_vulnerability = true;
	}

	if (numFire == 3)
	{
		FIRE_xx10_OF_Social_engineering = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_Wardialing = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_Bruteforce
	if ((boolState[failF_OF_Bruteforce] == false) && (boolState[required_OF_Bruteforce]
&& boolState[relevant_evt_OF_Bruteforce])) 
	{
		 
		if (FIRE_xx10_OF_Bruteforce)
		{
			boolState[failF_OF_Bruteforce]  =  true;
			FIRE_xx10_OF_Bruteforce = false;
		}
	}

	// Occurrence xx10_OF_Exploit_vulnerability
	if ((boolState[failF_OF_Exploit_vulnerability] == false) && (boolState[required_OF_Exploit_vulnerability] && boolState[relevant_evt_OF_Exploit_vulnerability])) 
	{
		 
		if (FIRE_xx10_OF_Exploit_vulnerability)
		{
			boolState[failF_OF_Exploit_vulnerability]  =  true;
			FIRE_xx10_OF_Exploit_vulnerability = false;
		}
	}

	// Occurrence xx10_OF_Find_vulnerability
	if ((boolState[failF_OF_Find_vulnerability] == false) && (boolState[required_OF_Find_vulnerability] && boolState[relevant_evt_OF_Find_vulnerability])) 
	{
		 
		if (FIRE_xx10_OF_Find_vulnerability)
		{
			boolState[failF_OF_Find_vulnerability]  =  true;
			FIRE_xx10_OF_Find_vulnerability = false;
		}
	}

	// Occurrence xx10_OF_Social_engineering
	if ((boolState[failF_OF_Social_engineering] == false) && (boolState[required_OF_Social_engineering] && boolState[relevant_evt_OF_Social_engineering])) 
	{
		 
		if (FIRE_xx10_OF_Social_engineering)
		{
			boolState[failF_OF_Social_engineering]  =  true;
			FIRE_xx10_OF_Social_engineering = false;
		}
	}

	// Occurrence xx10_OF_Wardialing
	if ((boolState[failF_OF_Wardialing] == false) && (boolState[required_OF_Wardialing]
&& boolState[relevant_evt_OF_Wardialing])) 
	{
		 
		if (FIRE_xx10_OF_Wardialing)
		{
			boolState[failF_OF_Wardialing]  =  true;
			FIRE_xx10_OF_Wardialing = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::showFireableOccurrences()
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
     
	if ((boolState[failF_OF_Bruteforce] == false) && (boolState[required_OF_Bruteforce] && boolState[relevant_evt_OF_Bruteforce]))
	{
		cout << "0 : xx10_OF_Bruteforce : FAULT failF  LABEL \"failure in operation Bruteforce\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Bruteforce]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Exploit_vulnerability] == false) && (boolState[required_OF_Exploit_vulnerability] && boolState[relevant_evt_OF_Exploit_vulnerability]))
	{
		cout << "1 : xx10_OF_Exploit_vulnerability : FAULT failF  LABEL \"failure in operation Exploit_vulnerability\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Exploit_vulnerability]  =  TRUE" << endl;
		list.push_back(make_tuple(1, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Find_vulnerability] == false) && (boolState[required_OF_Find_vulnerability] && boolState[relevant_evt_OF_Find_vulnerability]))
	{
		cout << "2 : xx10_OF_Find_vulnerability : FAULT failF  LABEL \"failure in operation Find_vulnerability\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Find_vulnerability]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Social_engineering] == false) && (boolState[required_OF_Social_engineering] && boolState[relevant_evt_OF_Social_engineering]))
	{
		cout << "3 : xx10_OF_Social_engineering : FAULT failF  LABEL \"failure in operation Social_engineering\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Social_engineering]  =  TRUE" << endl;
		list.push_back(make_tuple(3, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Wardialing] == false) && (boolState[required_OF_Wardialing] && boolState[relevant_evt_OF_Wardialing]))
	{
		cout << "4 : xx10_OF_Wardialing : FAULT failF  LABEL \"failure in operation Wardialing\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Wardialing]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_Bruteforce] == true )
	{
		boolState[S_OF_Bruteforce]  =  true;
	}

	if (boolState[failF_OF_Exploit_vulnerability] == true )
	{
		boolState[S_OF_Exploit_vulnerability]  =  true;
	}

	if (boolState[failF_OF_Find_vulnerability] == true )
	{
		boolState[S_OF_Find_vulnerability]  =  true;
	}

	if (boolState[failF_OF_Social_engineering] == true )
	{
		boolState[S_OF_Social_engineering]  =  true;
	}

	if (boolState[failF_OF_Wardialing] == true )
	{
		boolState[S_OF_Wardialing]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_Bruteforce] || boolState[S_OF_Social_engineering] )
	{
		boolState[S_OF_Authentication_with_password]  =  true;
	}

	if (boolState[S_OF_RAS_access_granted] && boolState[S_OF_Wardialing] )
	{
		boolState[S_OF_Logged_into_the_RAS]  =  true;
	}

	if (boolState[S_OF_Authentication_with_password] || boolState[S_OF_Vulnerability_found_and_exploited] )
	{
		boolState[S_OF_RAS_access_granted]  =  true;
	}

	if (boolState[S_OF_Logged_into_the_RAS] )
	{
		boolState[S_OF_RAS_ownership]  =  true;
	}

	if (boolState[S_OF_Exploit_vulnerability] && boolState[S_OF_Find_vulnerability] )
	{
		boolState[S_OF_Vulnerability_found_and_exploited]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_RAS_access_granted] )
	{
		boolState[required_OF_Authentication_with_password]  =  false;
	}

	if (boolState[relevant_evt_OF_RAS_access_granted] && ( !boolState[S_OF_RAS_access_granted]) )
	{
		boolState[relevant_evt_OF_Authentication_with_password]  =  true;
	}

	if ( !boolState[required_OF_Authentication_with_password] )
	{
		boolState[required_OF_Bruteforce]  =  false;
	}

	if (boolState[relevant_evt_OF_Authentication_with_password] && ( !boolState[S_OF_Authentication_with_password]) )
	{
		boolState[relevant_evt_OF_Bruteforce]  =  true;
	}

	if (( !boolState[required_OF_Vulnerability_found_and_exploited]) || ( !  boolState[S_OF_Find_vulnerability]) )
	{
		boolState[required_OF_Exploit_vulnerability]  =  false;
	}

	if (boolState[relevant_evt_OF_Vulnerability_found_and_exploited] && ( !boolState[S_OF_Vulnerability_found_and_exploited]) )
	{
		boolState[relevant_evt_OF_Exploit_vulnerability]  =  true;
	}

	if ( !boolState[required_OF_Vulnerability_found_and_exploited] )
	{
		boolState[required_OF_Find_vulnerability]  =  false;
	}

	if ((boolState[relevant_evt_OF_Vulnerability_found_and_exploited] && ( !  boolState[S_OF_Vulnerability_found_and_exploited])) || ( !boolState[S_OF_Exploit_vulnerability]) )
	{
		boolState[relevant_evt_OF_Find_vulnerability]  =  true;
	}

	if ( !boolState[required_OF_RAS_ownership] )
	{
		boolState[required_OF_Logged_into_the_RAS]  =  false;
	}

	if (boolState[relevant_evt_OF_RAS_ownership] && ( !boolState[S_OF_RAS_ownership]) )
	{
		boolState[relevant_evt_OF_Logged_into_the_RAS]  =  true;
	}

	if (( !boolState[required_OF_Logged_into_the_RAS]) || ( !boolState[S_OF_Wardialing]) )
	{
		boolState[required_OF_RAS_access_granted]  =  false;
	}

	if (boolState[relevant_evt_OF_Logged_into_the_RAS] && ( !boolState[S_OF_Logged_into_the_RAS]) )
	{
		boolState[relevant_evt_OF_RAS_access_granted]  =  true;
	}



	boolState[relevant_evt_OF_RAS_ownership]  =  true  ;

	if ( !boolState[required_OF_Authentication_with_password] )
	{
		boolState[required_OF_Social_engineering]  =  false;
	}

	if (boolState[relevant_evt_OF_Authentication_with_password] && ( !boolState[S_OF_Authentication_with_password]) )
	{
		boolState[relevant_evt_OF_Social_engineering]  =  true;
	}

	if ( !boolState[required_OF_RAS_access_granted] )
	{
		boolState[required_OF_Vulnerability_found_and_exploited]  =  false;
	}

	if (boolState[relevant_evt_OF_RAS_access_granted] && ( !boolState[S_OF_RAS_access_granted]) )
	{
		boolState[relevant_evt_OF_Vulnerability_found_and_exploited]  =  true;
	}

	if ( !boolState[required_OF_Logged_into_the_RAS] )
	{
		boolState[required_OF_Wardialing]  =  false;
	}

	if ((boolState[relevant_evt_OF_Logged_into_the_RAS] && ( !boolState[S_OF_Logged_into_the_RAS])) || ( !boolState[S_OF_RAS_access_granted]) )
	{
		boolState[relevant_evt_OF_Wardialing]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_Authentication_with_password]  =  boolState[S_OF_Authentication_with_password]  ;



	boolState[already_S_OF_Bruteforce]  =  boolState[S_OF_Bruteforce]  ;



	boolState[already_S_OF_Exploit_vulnerability]  =  boolState[S_OF_Exploit_vulnerability]  ;



	boolState[already_S_OF_Find_vulnerability]  =  boolState[S_OF_Find_vulnerability]  ;



	boolState[already_S_OF_Logged_into_the_RAS]  =  boolState[S_OF_Logged_into_the_RAS]  ;



	boolState[already_S_OF_RAS_access_granted]  =  boolState[S_OF_RAS_access_granted]  ;



	boolState[already_S_OF_RAS_ownership]  =  boolState[S_OF_RAS_ownership]  ;



	boolState[already_S_OF_Social_engineering]  =  boolState[S_OF_Social_engineering]  ;



	boolState[already_S_OF_Vulnerability_found_and_exploited]  =  boolState[S_OF_Vulnerability_found_and_exploited]  ;



	boolState[already_S_OF_Wardialing]  =  boolState[S_OF_Wardialing]  ;

}

void
storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::runInteractions() {
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
    
	boolFailureState[exp0] = ( boolState[S_OF_RAS_ownership] );
        cout << endl;
    }void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_21_Remote_Access_Server_Security_Trim_Article_No_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    