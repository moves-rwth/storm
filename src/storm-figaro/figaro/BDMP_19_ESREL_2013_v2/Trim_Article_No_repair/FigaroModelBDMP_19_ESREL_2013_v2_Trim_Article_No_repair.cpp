#include <iostream>
#include "FigaroModelBDMP_19_ESREL_2013_v2_Trim_Article_No_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;
	REINITIALISATION_OF_required_OF_collision_avoided = true;
	boolState[already_S_OF_collision_avoided] = false;
	REINITIALISATION_OF_S_OF_collision_avoided = false;
	REINITIALISATION_OF_relevant_evt_OF_collision_avoided = false;
	REINITIALISATION_OF_required_OF_course_changed = true;
	boolState[already_S_OF_course_changed] = false;
	REINITIALISATION_OF_S_OF_course_changed = false;
	REINITIALISATION_OF_relevant_evt_OF_course_changed = false;
	boolState[failF_OF_course_changed] = false;
	REINITIALISATION_OF_required_OF_detection = true;
	boolState[already_S_OF_detection] = false;
	REINITIALISATION_OF_S_OF_detection = false;
	REINITIALISATION_OF_relevant_evt_OF_detection = false;
	REINITIALISATION_OF_required_OF_detection_by_platform = true;
	boolState[already_S_OF_detection_by_platform] = false;
	REINITIALISATION_OF_S_OF_detection_by_platform = false;
	REINITIALISATION_OF_relevant_evt_OF_detection_by_platform = false;
	REINITIALISATION_OF_required_OF_detection_by_platform_1 = true;
	boolState[already_S_OF_detection_by_platform_1] = false;
	REINITIALISATION_OF_S_OF_detection_by_platform_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_detection_by_platform_1 = false;
	REINITIALISATION_OF_required_OF_detection_by_ship = true;
	boolState[already_S_OF_detection_by_ship] = false;
	REINITIALISATION_OF_S_OF_detection_by_ship = false;
	REINITIALISATION_OF_relevant_evt_OF_detection_by_ship = false;
	REINITIALISATION_OF_required_OF_radar = true;
	boolState[already_S_OF_radar] = false;
	REINITIALISATION_OF_S_OF_radar = false;
	REINITIALISATION_OF_relevant_evt_OF_radar = false;
	boolState[failF_OF_radar] = false;
	REINITIALISATION_OF_required_OF_radar_1 = true;
	boolState[already_S_OF_radar_1] = false;
	REINITIALISATION_OF_S_OF_radar_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_radar_1 = false;
	boolState[failF_OF_radar_1] = false;
	REINITIALISATION_OF_required_OF_radar_and_visual = true;
	boolState[already_S_OF_radar_and_visual] = false;
	REINITIALISATION_OF_S_OF_radar_and_visual = false;
	REINITIALISATION_OF_relevant_evt_OF_radar_and_visual = false;
	boolState[in_progress_OF_radar_and_visual] = false;
	boolState[already_required_OF_radar_and_visual] = false;
	boolState[start_phase_OF_radar_and_visual] = false;
	REINITIALISATION_OF_required_OF_radar_only = true;
	boolState[already_S_OF_radar_only] = false;
	REINITIALISATION_OF_S_OF_radar_only = false;
	REINITIALISATION_OF_relevant_evt_OF_radar_only = false;
	boolState[in_progress_OF_radar_only] = false;
	boolState[already_required_OF_radar_only] = false;
	boolState[start_phase_OF_radar_only] = false;
	REINITIALISATION_OF_required_OF_too_late = true;
	boolState[already_S_OF_too_late] = false;
	REINITIALISATION_OF_S_OF_too_late = false;
	REINITIALISATION_OF_relevant_evt_OF_too_late = false;
	boolState[in_progress_OF_too_late] = false;
	boolState[already_required_OF_too_late] = false;
	boolState[start_phase_OF_too_late] = false;
	REINITIALISATION_OF_required_OF_visual = true;
	boolState[already_S_OF_visual] = false;
	REINITIALISATION_OF_S_OF_visual = false;
	REINITIALISATION_OF_relevant_evt_OF_visual = false;
	boolState[failF_OF_visual] = false;
	REINITIALISATION_OF_required_OF_visual_1 = true;
	boolState[already_S_OF_visual_1] = false;
	REINITIALISATION_OF_S_OF_visual_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_visual_1 = false;
	boolState[failF_OF_visual_1] = false;
	REINITIALISATION_OF_required_OF_warning = true;
	boolState[already_S_OF_warning] = false;
	REINITIALISATION_OF_S_OF_warning = false;
	REINITIALISATION_OF_relevant_evt_OF_warning = false;
	boolState[failF_OF_warning] = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_course_changed = false;
	FIRE_xx11_OF_course_changed = false;
	FIRE_xx10_OF_radar = false;
	FIRE_xx11_OF_radar = false;
	FIRE_xx10_OF_radar_1 = false;
	FIRE_xx11_OF_radar_1 = false;
	FIRE_xx43_a_OF_radar_and_visual = false;
	FIRE_xx47_OF_radar_and_visual_INS_7 = false;
	FIRE_xx43_a_OF_radar_only = false;
	FIRE_xx47_OF_radar_only_INS_9 = false;
	FIRE_xx43_a_OF_too_late = false;
	FIRE_xx47_OF_too_late_INS_11 = false;
	FIRE_xx10_OF_visual = false;
	FIRE_xx11_OF_visual = false;
	FIRE_xx10_OF_visual_1 = false;
	FIRE_xx11_OF_visual_1 = false;
	FIRE_xx10_OF_warning = false;
	FIRE_xx11_OF_warning = false;

}

void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
	cout << "Attribute :  boolState[required_OF_collision_avoided] | Value : " << boolState[required_OF_collision_avoided] << endl;
	cout << "Attribute :  boolState[already_S_OF_collision_avoided] | Value : " << boolState[already_S_OF_collision_avoided] << endl;
	cout << "Attribute :  boolState[S_OF_collision_avoided] | Value : " << boolState[S_OF_collision_avoided] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_collision_avoided] | Value : " << boolState[relevant_evt_OF_collision_avoided] << endl;
	cout << "Attribute :  boolState[required_OF_course_changed] | Value : " << boolState[required_OF_course_changed] << endl;
	cout << "Attribute :  boolState[already_S_OF_course_changed] | Value : " << boolState[already_S_OF_course_changed] << endl;
	cout << "Attribute :  boolState[S_OF_course_changed] | Value : " << boolState[S_OF_course_changed] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_course_changed] | Value : " << boolState[relevant_evt_OF_course_changed] << endl;
	cout << "Attribute :  boolState[failF_OF_course_changed] | Value : " << boolState[failF_OF_course_changed] << endl;
	cout << "Attribute :  boolState[required_OF_detection] | Value : " << boolState[required_OF_detection] << endl;
	cout << "Attribute :  boolState[already_S_OF_detection] | Value : " << boolState[already_S_OF_detection] << endl;
	cout << "Attribute :  boolState[S_OF_detection] | Value : " << boolState[S_OF_detection] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_detection] | Value : " << boolState[relevant_evt_OF_detection] << endl;
	cout << "Attribute :  boolState[required_OF_detection_by_platform] | Value : " << boolState[required_OF_detection_by_platform] << endl;
	cout << "Attribute :  boolState[already_S_OF_detection_by_platform] | Value : " << boolState[already_S_OF_detection_by_platform] << endl;
	cout << "Attribute :  boolState[S_OF_detection_by_platform] | Value : " << boolState[S_OF_detection_by_platform] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_detection_by_platform] | Value : " << boolState[relevant_evt_OF_detection_by_platform] << endl;
	cout << "Attribute :  boolState[required_OF_detection_by_platform_1] | Value : " << boolState[required_OF_detection_by_platform_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_detection_by_platform_1] | Value : " << boolState[already_S_OF_detection_by_platform_1] << endl;
	cout << "Attribute :  boolState[S_OF_detection_by_platform_1] | Value : " << boolState[S_OF_detection_by_platform_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_detection_by_platform_1] | Value : " << boolState[relevant_evt_OF_detection_by_platform_1] << endl;
	cout << "Attribute :  boolState[required_OF_detection_by_ship] | Value : " << boolState[required_OF_detection_by_ship] << endl;
	cout << "Attribute :  boolState[already_S_OF_detection_by_ship] | Value : " << boolState[already_S_OF_detection_by_ship] << endl;
	cout << "Attribute :  boolState[S_OF_detection_by_ship] | Value : " << boolState[S_OF_detection_by_ship] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_detection_by_ship] | Value : " << boolState[relevant_evt_OF_detection_by_ship] << endl;
	cout << "Attribute :  boolState[required_OF_radar] | Value : " << boolState[required_OF_radar] << endl;
	cout << "Attribute :  boolState[already_S_OF_radar] | Value : " << boolState[already_S_OF_radar] << endl;
	cout << "Attribute :  boolState[S_OF_radar] | Value : " << boolState[S_OF_radar] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_radar] | Value : " << boolState[relevant_evt_OF_radar] << endl;
	cout << "Attribute :  boolState[failF_OF_radar] | Value : " << boolState[failF_OF_radar] << endl;
	cout << "Attribute :  boolState[required_OF_radar_1] | Value : " << boolState[required_OF_radar_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_radar_1] | Value : " << boolState[already_S_OF_radar_1] << endl;
	cout << "Attribute :  boolState[S_OF_radar_1] | Value : " << boolState[S_OF_radar_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_radar_1] | Value : " << boolState[relevant_evt_OF_radar_1] << endl;
	cout << "Attribute :  boolState[failF_OF_radar_1] | Value : " << boolState[failF_OF_radar_1] << endl;
	cout << "Attribute :  boolState[required_OF_radar_and_visual] | Value : " << boolState[required_OF_radar_and_visual] << endl;
	cout << "Attribute :  boolState[already_S_OF_radar_and_visual] | Value : " << boolState[already_S_OF_radar_and_visual] << endl;
	cout << "Attribute :  boolState[S_OF_radar_and_visual] | Value : " << boolState[S_OF_radar_and_visual] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_radar_and_visual] | Value : " << boolState[relevant_evt_OF_radar_and_visual] << endl;
	cout << "Attribute :  boolState[in_progress_OF_radar_and_visual] | Value : " << boolState[in_progress_OF_radar_and_visual] << endl;
	cout << "Attribute :  boolState[already_required_OF_radar_and_visual] | Value : " << boolState[already_required_OF_radar_and_visual] << endl;
	cout << "Attribute :  boolState[start_phase_OF_radar_and_visual] | Value : " << boolState[start_phase_OF_radar_and_visual] << endl;
	cout << "Attribute :  boolState[required_OF_radar_only] | Value : " << boolState[required_OF_radar_only] << endl;
	cout << "Attribute :  boolState[already_S_OF_radar_only] | Value : " << boolState[already_S_OF_radar_only] << endl;
	cout << "Attribute :  boolState[S_OF_radar_only] | Value : " << boolState[S_OF_radar_only] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_radar_only] | Value : " << boolState[relevant_evt_OF_radar_only] << endl;
	cout << "Attribute :  boolState[in_progress_OF_radar_only] | Value : " << boolState[in_progress_OF_radar_only] << endl;
	cout << "Attribute :  boolState[already_required_OF_radar_only] | Value : " << boolState[already_required_OF_radar_only] << endl;
	cout << "Attribute :  boolState[start_phase_OF_radar_only] | Value : " << boolState[start_phase_OF_radar_only] << endl;
	cout << "Attribute :  boolState[required_OF_too_late] | Value : " << boolState[required_OF_too_late] << endl;
	cout << "Attribute :  boolState[already_S_OF_too_late] | Value : " << boolState[already_S_OF_too_late] << endl;
	cout << "Attribute :  boolState[S_OF_too_late] | Value : " << boolState[S_OF_too_late] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_too_late] | Value : " << boolState[relevant_evt_OF_too_late] << endl;
	cout << "Attribute :  boolState[in_progress_OF_too_late] | Value : " << boolState[in_progress_OF_too_late] << endl;
	cout << "Attribute :  boolState[already_required_OF_too_late] | Value : " << boolState[already_required_OF_too_late] << endl;
	cout << "Attribute :  boolState[start_phase_OF_too_late] | Value : " << boolState[start_phase_OF_too_late] << endl;
	cout << "Attribute :  boolState[required_OF_visual] | Value : " << boolState[required_OF_visual] << endl;
	cout << "Attribute :  boolState[already_S_OF_visual] | Value : " << boolState[already_S_OF_visual] << endl;
	cout << "Attribute :  boolState[S_OF_visual] | Value : " << boolState[S_OF_visual] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_visual] | Value : " << boolState[relevant_evt_OF_visual] << endl;
	cout << "Attribute :  boolState[failF_OF_visual] | Value : " << boolState[failF_OF_visual] << endl;
	cout << "Attribute :  boolState[required_OF_visual_1] | Value : " << boolState[required_OF_visual_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_visual_1] | Value : " << boolState[already_S_OF_visual_1] << endl;
	cout << "Attribute :  boolState[S_OF_visual_1] | Value : " << boolState[S_OF_visual_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_visual_1] | Value : " << boolState[relevant_evt_OF_visual_1] << endl;
	cout << "Attribute :  boolState[failF_OF_visual_1] | Value : " << boolState[failF_OF_visual_1] << endl;
	cout << "Attribute :  boolState[required_OF_warning] | Value : " << boolState[required_OF_warning] << endl;
	cout << "Attribute :  boolState[already_S_OF_warning] | Value : " << boolState[already_S_OF_warning] << endl;
	cout << "Attribute :  boolState[S_OF_warning] | Value : " << boolState[S_OF_warning] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_warning] | Value : " << boolState[relevant_evt_OF_warning] << endl;
	cout << "Attribute :  boolState[failF_OF_warning] | Value : " << boolState[failF_OF_warning] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::doReinitialisations()
{
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
	boolState[required_OF_collision_avoided] = REINITIALISATION_OF_required_OF_collision_avoided;
	boolState[S_OF_collision_avoided] = REINITIALISATION_OF_S_OF_collision_avoided;
	boolState[relevant_evt_OF_collision_avoided] = REINITIALISATION_OF_relevant_evt_OF_collision_avoided;
	boolState[required_OF_course_changed] = REINITIALISATION_OF_required_OF_course_changed;
	boolState[S_OF_course_changed] = REINITIALISATION_OF_S_OF_course_changed;
	boolState[relevant_evt_OF_course_changed] = REINITIALISATION_OF_relevant_evt_OF_course_changed;
	boolState[required_OF_detection] = REINITIALISATION_OF_required_OF_detection;
	boolState[S_OF_detection] = REINITIALISATION_OF_S_OF_detection;
	boolState[relevant_evt_OF_detection] = REINITIALISATION_OF_relevant_evt_OF_detection;
	boolState[required_OF_detection_by_platform] = REINITIALISATION_OF_required_OF_detection_by_platform;
	boolState[S_OF_detection_by_platform] = REINITIALISATION_OF_S_OF_detection_by_platform;
	boolState[relevant_evt_OF_detection_by_platform] = REINITIALISATION_OF_relevant_evt_OF_detection_by_platform;
	boolState[required_OF_detection_by_platform_1] = REINITIALISATION_OF_required_OF_detection_by_platform_1;
	boolState[S_OF_detection_by_platform_1] = REINITIALISATION_OF_S_OF_detection_by_platform_1;
	boolState[relevant_evt_OF_detection_by_platform_1] = REINITIALISATION_OF_relevant_evt_OF_detection_by_platform_1;
	boolState[required_OF_detection_by_ship] = REINITIALISATION_OF_required_OF_detection_by_ship;
	boolState[S_OF_detection_by_ship] = REINITIALISATION_OF_S_OF_detection_by_ship;
	boolState[relevant_evt_OF_detection_by_ship] = REINITIALISATION_OF_relevant_evt_OF_detection_by_ship;
	boolState[required_OF_radar] = REINITIALISATION_OF_required_OF_radar;
	boolState[S_OF_radar] = REINITIALISATION_OF_S_OF_radar;
	boolState[relevant_evt_OF_radar] = REINITIALISATION_OF_relevant_evt_OF_radar;
	boolState[required_OF_radar_1] = REINITIALISATION_OF_required_OF_radar_1;
	boolState[S_OF_radar_1] = REINITIALISATION_OF_S_OF_radar_1;
	boolState[relevant_evt_OF_radar_1] = REINITIALISATION_OF_relevant_evt_OF_radar_1;
	boolState[required_OF_radar_and_visual] = REINITIALISATION_OF_required_OF_radar_and_visual;
	boolState[S_OF_radar_and_visual] = REINITIALISATION_OF_S_OF_radar_and_visual;
	boolState[relevant_evt_OF_radar_and_visual] = REINITIALISATION_OF_relevant_evt_OF_radar_and_visual;
	boolState[required_OF_radar_only] = REINITIALISATION_OF_required_OF_radar_only;
	boolState[S_OF_radar_only] = REINITIALISATION_OF_S_OF_radar_only;
	boolState[relevant_evt_OF_radar_only] = REINITIALISATION_OF_relevant_evt_OF_radar_only;
	boolState[required_OF_too_late] = REINITIALISATION_OF_required_OF_too_late;
	boolState[S_OF_too_late] = REINITIALISATION_OF_S_OF_too_late;
	boolState[relevant_evt_OF_too_late] = REINITIALISATION_OF_relevant_evt_OF_too_late;
	boolState[required_OF_visual] = REINITIALISATION_OF_required_OF_visual;
	boolState[S_OF_visual] = REINITIALISATION_OF_S_OF_visual;
	boolState[relevant_evt_OF_visual] = REINITIALISATION_OF_relevant_evt_OF_visual;
	boolState[required_OF_visual_1] = REINITIALISATION_OF_required_OF_visual_1;
	boolState[S_OF_visual_1] = REINITIALISATION_OF_S_OF_visual_1;
	boolState[relevant_evt_OF_visual_1] = REINITIALISATION_OF_relevant_evt_OF_visual_1;
	boolState[required_OF_warning] = REINITIALISATION_OF_required_OF_warning;
	boolState[S_OF_warning] = REINITIALISATION_OF_S_OF_warning;
	boolState[relevant_evt_OF_warning] = REINITIALISATION_OF_relevant_evt_OF_warning;
}

void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_course_changed = true;
	}

	if (numFire == 1)
	{
		FIRE_xx11_OF_course_changed = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_radar = true;
	}

	if (numFire == 3)
	{
		FIRE_xx11_OF_radar = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_radar_1 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx11_OF_radar_1 = true;
	}

	if (numFire == 6)
	{
		FIRE_xx43_a_OF_radar_and_visual = true;
	}

	if (numFire == 7)
	{
		FIRE_xx47_OF_radar_and_visual_INS_7 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx43_a_OF_radar_only = true;
	}

	if (numFire == 9)
	{
		FIRE_xx47_OF_radar_only_INS_9 = true;
	}

	if (numFire == 10)
	{
		FIRE_xx43_a_OF_too_late = true;
	}

	if (numFire == 11)
	{
		FIRE_xx47_OF_too_late_INS_11 = true;
	}

	if (numFire == 12)
	{
		FIRE_xx10_OF_visual = true;
	}

	if (numFire == 13)
	{
		FIRE_xx11_OF_visual = true;
	}

	if (numFire == 14)
	{
		FIRE_xx10_OF_visual_1 = true;
	}

	if (numFire == 15)
	{
		FIRE_xx11_OF_visual_1 = true;
	}

	if (numFire == 16)
	{
		FIRE_xx10_OF_warning = true;
	}

	if (numFire == 17)
	{
		FIRE_xx11_OF_warning = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_course_changed
	if ((boolState[failF_OF_course_changed] == false) && (boolState[required_OF_course_changed] && boolState[relevant_evt_OF_course_changed])) 
	{
		 
		if (FIRE_xx10_OF_course_changed)
		{
			boolState[failF_OF_course_changed]  =  true;
			FIRE_xx10_OF_course_changed = false;
		}
	}

	// Occurrence xx11_OF_course_changed
	if (boolState[failF_OF_course_changed] == true) 
	{
		 
		if (FIRE_xx11_OF_course_changed)
		{
			boolState[failF_OF_course_changed]  =  false;
			FIRE_xx11_OF_course_changed = false;
		}
	}

	// Occurrence xx10_OF_radar
	if ((boolState[failF_OF_radar] == false) && (boolState[required_OF_radar] &&  boolState[relevant_evt_OF_radar])) 
	{
		 
		if (FIRE_xx10_OF_radar)
		{
			boolState[failF_OF_radar]  =  true;
			FIRE_xx10_OF_radar = false;
		}
	}

	// Occurrence xx11_OF_radar
	if (boolState[failF_OF_radar] == true) 
	{
		 
		if (FIRE_xx11_OF_radar)
		{
			boolState[failF_OF_radar]  =  false;
			FIRE_xx11_OF_radar = false;
		}
	}

	// Occurrence xx10_OF_radar_1
	if ((boolState[failF_OF_radar_1] == false) && (boolState[required_OF_radar_1] &&  boolState[relevant_evt_OF_radar_1])) 
	{
		 
		if (FIRE_xx10_OF_radar_1)
		{
			boolState[failF_OF_radar_1]  =  true;
			FIRE_xx10_OF_radar_1 = false;
		}
	}

	// Occurrence xx11_OF_radar_1
	if (boolState[failF_OF_radar_1] == true) 
	{
		 
		if (FIRE_xx11_OF_radar_1)
		{
			boolState[failF_OF_radar_1]  =  false;
			FIRE_xx11_OF_radar_1 = false;
		}
	}

	// Occurrence xx43_a_OF_radar_and_visual
	if (boolState[in_progress_OF_radar_and_visual]) 
	{
		 
		if (FIRE_xx43_a_OF_radar_and_visual)
		{
			boolState[in_progress_OF_radar_and_visual]  =  false;
			FIRE_xx43_a_OF_radar_and_visual = false;
		}
	}

	// Occurrence xx47_OF_radar_and_visual

	if (boolState[start_phase_OF_radar_and_visual]) 
	{
	
		
		if (FIRE_xx47_OF_radar_and_visual_INS_7) 
		{
			boolState[in_progress_OF_radar_and_visual]  =  true;
			boolState[start_phase_OF_radar_and_visual]  =  false;
			FIRE_xx47_OF_radar_and_visual_INS_7 = false;
		}
	
	}
	// Occurrence xx43_a_OF_radar_only
	if (boolState[in_progress_OF_radar_only]) 
	{
		 
		if (FIRE_xx43_a_OF_radar_only)
		{
			boolState[in_progress_OF_radar_only]  =  false;
			FIRE_xx43_a_OF_radar_only = false;
		}
	}

	// Occurrence xx47_OF_radar_only

	if (boolState[start_phase_OF_radar_only]) 
	{
	
		
		if (FIRE_xx47_OF_radar_only_INS_9) 
		{
			boolState[in_progress_OF_radar_only]  =  true;
			boolState[start_phase_OF_radar_only]  =  false;
			FIRE_xx47_OF_radar_only_INS_9 = false;
		}
	
	}
	// Occurrence xx43_a_OF_too_late
	if (boolState[in_progress_OF_too_late]) 
	{
		 
		if (FIRE_xx43_a_OF_too_late)
		{
			boolState[in_progress_OF_too_late]  =  false;
			FIRE_xx43_a_OF_too_late = false;
		}
	}

	// Occurrence xx47_OF_too_late

	if (boolState[start_phase_OF_too_late]) 
	{
	
		
		if (FIRE_xx47_OF_too_late_INS_11) 
		{
			boolState[in_progress_OF_too_late]  =  true;
			boolState[start_phase_OF_too_late]  =  false;
			FIRE_xx47_OF_too_late_INS_11 = false;
		}
	
	}
	// Occurrence xx10_OF_visual
	if ((boolState[failF_OF_visual] == false) && (boolState[required_OF_visual] &&  boolState[relevant_evt_OF_visual])) 
	{
		 
		if (FIRE_xx10_OF_visual)
		{
			boolState[failF_OF_visual]  =  true;
			FIRE_xx10_OF_visual = false;
		}
	}

	// Occurrence xx11_OF_visual
	if (boolState[failF_OF_visual] == true) 
	{
		 
		if (FIRE_xx11_OF_visual)
		{
			boolState[failF_OF_visual]  =  false;
			FIRE_xx11_OF_visual = false;
		}
	}

	// Occurrence xx10_OF_visual_1
	if ((boolState[failF_OF_visual_1] == false) && (boolState[required_OF_visual_1] &&  boolState[relevant_evt_OF_visual_1])) 
	{
		 
		if (FIRE_xx10_OF_visual_1)
		{
			boolState[failF_OF_visual_1]  =  true;
			FIRE_xx10_OF_visual_1 = false;
		}
	}

	// Occurrence xx11_OF_visual_1
	if (boolState[failF_OF_visual_1] == true) 
	{
		 
		if (FIRE_xx11_OF_visual_1)
		{
			boolState[failF_OF_visual_1]  =  false;
			FIRE_xx11_OF_visual_1 = false;
		}
	}

	// Occurrence xx10_OF_warning
	if ((boolState[failF_OF_warning] == false) && (boolState[required_OF_warning] &&  boolState[relevant_evt_OF_warning])) 
	{
		 
		if (FIRE_xx10_OF_warning)
		{
			boolState[failF_OF_warning]  =  true;
			FIRE_xx10_OF_warning = false;
		}
	}

	// Occurrence xx11_OF_warning
	if (boolState[failF_OF_warning] == true) 
	{
		 
		if (FIRE_xx11_OF_warning)
		{
			boolState[failF_OF_warning]  =  false;
			FIRE_xx11_OF_warning = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if (boolState[start_phase_OF_radar_and_visual])
	{
		cout << "7 :  INS_SUB_COUNT (1) |TRANSITION | start  LABEL \"start of phase radar_and_visual\" | DIST INS (1) | INDUCING boolState[in_progress_OF_radar_and_visual]  =  TRUE,start_phase_OF_radar_and_visual  =  FALSE" << endl;
		list.push_back(make_tuple(7, 1, "INS", 1));
	}
	if (boolState[start_phase_OF_radar_only])
	{
		cout << "9 :  INS_SUB_COUNT (2) |TRANSITION | start  LABEL \"start of phase radar_only\" | DIST INS (1) | INDUCING boolState[in_progress_OF_radar_only]  =  TRUE,start_phase_OF_radar_only  =  FALSE" << endl;
		list.push_back(make_tuple(9, 1, "INS", 2));
	}
	if (boolState[start_phase_OF_too_late])
	{
		cout << "11 :  INS_SUB_COUNT (3) |TRANSITION | start  LABEL \"start of phase too_late\" | DIST INS (1) | INDUCING boolState[in_progress_OF_too_late]  =  TRUE,start_phase_OF_too_late  =  FALSE" << endl;
		list.push_back(make_tuple(11, 1, "INS", 3));
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
     
	if ((boolState[failF_OF_course_changed] == false) && (boolState[required_OF_course_changed] && boolState[relevant_evt_OF_course_changed]))
	{
		cout << "0 : xx10_OF_course_changed : FAULT failF  LABEL \"failure in operation course_changed\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_course_changed]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_course_changed] == true)
	{
		cout << "1 : xx11_OF_course_changed : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_course_changed]  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_radar] == false) && (boolState[required_OF_radar] && boolState[relevant_evt_OF_radar]))
	{
		cout << "2 : xx10_OF_radar : FAULT failF  LABEL \"failure in operation radar\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_radar]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_radar] == true)
	{
		cout << "3 : xx11_OF_radar : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_radar]  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_radar_1] == false) && (boolState[required_OF_radar_1] && boolState[relevant_evt_OF_radar_1]))
	{
		cout << "4 : xx10_OF_radar_1 : FAULT failF  LABEL \"failure in operation radar_1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_radar_1]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_radar_1] == true)
	{
		cout << "5 : xx11_OF_radar_1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_radar_1]  =  FALSE" << endl;
		list.push_back(make_tuple(5, 0.1, "EXP", 0));
	}
	if (boolState[in_progress_OF_radar_and_visual])
	{
		cout << "6 : xx43_a_OF_radar_and_visual : TRANSITION end  LABEL \"End of phase radar_and_visual\"  DIST EXP (0.1)  INDUCING boolState[in_progress_OF_radar_and_visual]  =  FALSE" << endl;
		list.push_back(make_tuple(6, 0.1, "EXP", 0));
	}
	if (boolState[in_progress_OF_radar_only])
	{
		cout << "8 : xx43_a_OF_radar_only : TRANSITION end  LABEL \"End of phase radar_only\"  DIST EXP (0.1)  INDUCING boolState[in_progress_OF_radar_only]  =  FALSE" << endl;
		list.push_back(make_tuple(8, 0.1, "EXP", 0));
	}
	if (boolState[in_progress_OF_too_late])
	{
		cout << "10 : xx43_a_OF_too_late : TRANSITION end  LABEL \"End of phase too_late\"  DIST EXP (0.1)  INDUCING boolState[in_progress_OF_too_late]  =  FALSE" << endl;
		list.push_back(make_tuple(10, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_visual] == false) && (boolState[required_OF_visual] && boolState[relevant_evt_OF_visual]))
	{
		cout << "12 : xx10_OF_visual : FAULT failF  LABEL \"failure in operation visual\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_visual]  =  TRUE" << endl;
		list.push_back(make_tuple(12, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_visual] == true)
	{
		cout << "13 : xx11_OF_visual : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_visual]  =  FALSE" << endl;
		list.push_back(make_tuple(13, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_visual_1] == false) && (boolState[required_OF_visual_1] && boolState[relevant_evt_OF_visual_1]))
	{
		cout << "14 : xx10_OF_visual_1 : FAULT failF  LABEL \"failure in operation visual_1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_visual_1]  =  TRUE" << endl;
		list.push_back(make_tuple(14, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_visual_1] == true)
	{
		cout << "15 : xx11_OF_visual_1 : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_visual_1]  =  FALSE" << endl;
		list.push_back(make_tuple(15, 0.1, "EXP", 0));
	}
	if ((boolState[failF_OF_warning] == false) && (boolState[required_OF_warning] && boolState[relevant_evt_OF_warning]))
	{
		cout << "16 : xx10_OF_warning : FAULT failF  LABEL \"failure in operation warning\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_warning]  =  TRUE" << endl;
		list.push_back(make_tuple(16, 0.0001, "EXP", 0));
	}
	if (boolState[failF_OF_warning] == true)
	{
		cout << "17 : xx11_OF_warning : REPAIR rep  DIST EXP (0.1)  INDUCING boolState[failF_OF_warning]  =  FALSE" << endl;
		list.push_back(make_tuple(17, 0.1, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_course_changed] == true )
	{
		boolState[S_OF_course_changed]  =  true;
	}

	if (boolState[failF_OF_radar] == true )
	{
		boolState[S_OF_radar]  =  true;
	}

	if (boolState[failF_OF_radar_1] == true )
	{
		boolState[S_OF_radar_1]  =  true;
	}



	boolState[S_OF_radar_and_visual]  =  boolState[in_progress_OF_radar_and_visual]  ;



	boolState[S_OF_radar_only]  =  boolState[in_progress_OF_radar_only]  ;



	boolState[S_OF_too_late]  =  boolState[in_progress_OF_too_late]  ;

	if (boolState[failF_OF_visual] == true )
	{
		boolState[S_OF_visual]  =  true;
	}

	if (boolState[failF_OF_visual_1] == true )
	{
		boolState[S_OF_visual_1]  =  true;
	}

	if (boolState[failF_OF_warning] == true )
	{
		boolState[S_OF_warning]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_collision_avoided] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

	if (boolState[S_OF_course_changed] && boolState[S_OF_detection] )
	{
		boolState[S_OF_collision_avoided]  =  true;
	}

	if (boolState[S_OF_detection_by_platform] || boolState[S_OF_detection_by_ship] )
	{
		boolState[S_OF_detection]  =  true;
	}

	if (boolState[S_OF_detection_by_platform_1] && boolState[S_OF_warning] )
	{
		boolState[S_OF_detection_by_platform]  =  true;
	}

	if (boolState[S_OF_radar_1] || boolState[S_OF_visual_1] )
	{
		boolState[S_OF_detection_by_platform_1]  =  true;
	}

	if (boolState[S_OF_radar] || boolState[S_OF_visual] )
	{
		boolState[S_OF_detection_by_ship]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::runOnceInteractionStep_propagate_effect_required()
{


	boolState[relevant_evt_OF_UE_1]  =  true  ;

	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_collision_avoided]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_collision_avoided]  =  true;
	}

	if (( !boolState[required_OF_collision_avoided]) || ( !boolState[S_OF_detection]) )
	{
		boolState[required_OF_course_changed]  =  false;
	}

	if (boolState[relevant_evt_OF_collision_avoided] && ( !boolState[S_OF_collision_avoided]) )
	{
		boolState[relevant_evt_OF_course_changed]  =  true;
	}

	if ( !boolState[required_OF_collision_avoided] )
	{
		boolState[required_OF_detection]  =  false;
	}

	if ((boolState[relevant_evt_OF_collision_avoided] && ( !boolState[S_OF_collision_avoided])) || ( !boolState[S_OF_course_changed]) )
	{
		boolState[relevant_evt_OF_detection]  =  true;
	}

	if ( !boolState[required_OF_detection] )
	{
		boolState[required_OF_detection_by_platform]  =  false;
	}

	if (boolState[relevant_evt_OF_detection] && ( !boolState[S_OF_detection]) )
	{
		boolState[relevant_evt_OF_detection_by_platform]  =  true;
	}

	if ( !boolState[required_OF_detection_by_platform] )
	{
		boolState[required_OF_detection_by_platform_1]  =  false;
	}

	if ((boolState[relevant_evt_OF_detection_by_platform] && ( !boolState[S_OF_detection_by_platform])) || ( !boolState[S_OF_warning]) )
	{
		boolState[relevant_evt_OF_detection_by_platform_1]  =  true;
	}

	if ( !boolState[required_OF_detection] )
	{
		boolState[required_OF_detection_by_ship]  =  false;
	}

	if (boolState[relevant_evt_OF_detection] && ( !boolState[S_OF_detection]) )
	{
		boolState[relevant_evt_OF_detection_by_ship]  =  true;
	}

	if (( !boolState[required_OF_detection_by_ship]) || ( !boolState[S_OF_radar_only]) )
	{
		boolState[required_OF_radar]  =  false;
	}

	if (boolState[relevant_evt_OF_detection_by_ship] && ( !boolState[S_OF_detection_by_ship]) )
	{
		boolState[relevant_evt_OF_radar]  =  true;
	}

	if (( !boolState[required_OF_detection_by_platform_1]) || ( !boolState[S_OF_radar_only]) )
	{
		boolState[required_OF_radar_1]  =  false;
	}

	if (boolState[relevant_evt_OF_detection_by_platform_1] && ( !boolState[S_OF_detection_by_platform_1]) )
	{
		boolState[relevant_evt_OF_radar_1]  =  true;
	}

	if ( !boolState[S_OF_radar_only] )
	{
		boolState[required_OF_radar_and_visual]  =  false;
	}

	if ((( !boolState[S_OF_too_late]) || ( !boolState[S_OF_visual])) || ( !boolState[S_OF_visual_1]) )
	{
		boolState[relevant_evt_OF_radar_and_visual]  =  true;
	}

	if ((( !boolState[S_OF_radar]) || ( !boolState[S_OF_radar_1])) || ( !boolState[S_OF_radar_and_visual]) )
	{
		boolState[relevant_evt_OF_radar_only]  =  true;
	}

	if ( !boolState[S_OF_radar_and_visual] )
	{
		boolState[required_OF_too_late]  =  false;
	}

	if (( !boolState[required_OF_detection_by_ship]) || ( !boolState[S_OF_radar_and_visual]) )
	{
		boolState[required_OF_visual]  =  false;
	}

	if (boolState[relevant_evt_OF_detection_by_ship] && ( !boolState[S_OF_detection_by_ship]) )
	{
		boolState[relevant_evt_OF_visual]  =  true;
	}

	if (( !boolState[required_OF_detection_by_platform_1]) || ( !boolState[S_OF_radar_and_visual]) )
	{
		boolState[required_OF_visual_1]  =  false;
	}

	if (boolState[relevant_evt_OF_detection_by_platform_1] && ( !boolState[S_OF_detection_by_platform_1]) )
	{
		boolState[relevant_evt_OF_visual_1]  =  true;
	}

	if (( !boolState[required_OF_detection_by_platform]) || ( !boolState[S_OF_detection_by_platform_1]) )
	{
		boolState[required_OF_warning]  =  false;
	}

	if (boolState[relevant_evt_OF_detection_by_platform] && ( !boolState[S_OF_detection_by_platform]) )
	{
		boolState[relevant_evt_OF_warning]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;



	boolState[already_S_OF_collision_avoided]  =  boolState[S_OF_collision_avoided]  ;



	boolState[already_S_OF_course_changed]  =  boolState[S_OF_course_changed]  ;



	boolState[already_S_OF_detection]  =  boolState[S_OF_detection]  ;



	boolState[already_S_OF_detection_by_platform]  =  boolState[S_OF_detection_by_platform]  ;



	boolState[already_S_OF_detection_by_platform_1]  =  boolState[S_OF_detection_by_platform_1]  ;



	boolState[already_S_OF_detection_by_ship]  =  boolState[S_OF_detection_by_ship]  ;



	boolState[already_S_OF_radar]  =  boolState[S_OF_radar]  ;



	boolState[already_S_OF_radar_1]  =  boolState[S_OF_radar_1]  ;



	boolState[already_S_OF_radar_and_visual]  =  boolState[S_OF_radar_and_visual]  ;

	if ((( !boolState[in_progress_OF_radar_and_visual]) && ( !boolState[required_OF_radar_and_visual])) && boolState[already_required_OF_radar_and_visual] )
	{
		boolState[start_phase_OF_radar_and_visual]  =  true;
	}



	boolState[already_S_OF_radar_only]  =  boolState[S_OF_radar_only]  ;

	if ((( !boolState[in_progress_OF_radar_only]) && ( !boolState[required_OF_radar_only])) && boolState[already_required_OF_radar_only] )
	{
		boolState[start_phase_OF_radar_only]  =  true;
	}



	boolState[already_S_OF_too_late]  =  boolState[S_OF_too_late]  ;

	if ((( !boolState[in_progress_OF_too_late]) && ( !boolState[required_OF_too_late])  ) && boolState[already_required_OF_too_late] )
	{
		boolState[start_phase_OF_too_late]  =  true;
	}



	boolState[already_S_OF_visual]  =  boolState[S_OF_visual]  ;



	boolState[already_S_OF_visual_1]  =  boolState[S_OF_visual_1]  ;



	boolState[already_S_OF_warning]  =  boolState[S_OF_warning]  ;

}


void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::runOnceInteractionStep_tops()
{


	boolState[already_required_OF_radar_and_visual]  =  boolState[required_OF_radar_and_visual]  ;



	boolState[already_required_OF_radar_only]  =  boolState[required_OF_radar_only]  ;



	boolState[already_required_OF_too_late]  =  boolState[required_OF_too_late]  ;

}

void
storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_19_ESREL_2013_v2_Trim_Article_No_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    