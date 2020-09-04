#include <iostream>
#include "FigaroModelBDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_A_ND_by_ship = true;
	boolState[already_S_OF_A_ND_by_ship] = false;
	REINITIALISATION_OF_S_OF_A_ND_by_ship = false;
	REINITIALISATION_OF_relevant_evt_OF_A_ND_by_ship = false;
	REINITIALISATION_OF_required_OF_A_ND_by_ship_1 = true;
	boolState[already_S_OF_A_ND_by_ship_1] = false;
	REINITIALISATION_OF_S_OF_A_ND_by_ship_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_A_ND_by_ship_1 = false;
	REINITIALISATION_OF_required_OF_B_ND_by_platform = true;
	boolState[already_S_OF_B_ND_by_platform] = false;
	REINITIALISATION_OF_S_OF_B_ND_by_platform = false;
	REINITIALISATION_OF_relevant_evt_OF_B_ND_by_platform = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;
	REINITIALISATION_OF_required_OF_collision = true;
	boolState[already_S_OF_collision] = false;
	REINITIALISATION_OF_S_OF_collision = false;
	REINITIALISATION_OF_relevant_evt_OF_collision = false;
	REINITIALISATION_OF_required_OF_course_not_changed = true;
	boolState[already_S_OF_course_not_changed] = false;
	REINITIALISATION_OF_S_OF_course_not_changed = false;
	REINITIALISATION_OF_relevant_evt_OF_course_not_changed = false;
	boolState[failI_OF_course_not_changed] = false;
	REINITIALISATION_OF_to_be_fired_OF_course_not_changed = false;
	boolState[already_standby_OF_course_not_changed] = false;
	boolState[already_required_OF_course_not_changed] = false;
	REINITIALISATION_OF_required_OF_non_detection = true;
	boolState[already_S_OF_non_detection] = false;
	REINITIALISATION_OF_S_OF_non_detection = false;
	REINITIALISATION_OF_relevant_evt_OF_non_detection = false;
	REINITIALISATION_OF_required_OF_radar = true;
	boolState[already_S_OF_radar] = false;
	REINITIALISATION_OF_S_OF_radar = false;
	REINITIALISATION_OF_relevant_evt_OF_radar = false;
	boolState[failI_OF_radar] = false;
	REINITIALISATION_OF_to_be_fired_OF_radar = false;
	boolState[already_standby_OF_radar] = false;
	boolState[already_required_OF_radar] = false;
	REINITIALISATION_OF_required_OF_radar_1 = true;
	boolState[already_S_OF_radar_1] = false;
	REINITIALISATION_OF_S_OF_radar_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_radar_1 = false;
	boolState[failI_OF_radar_1] = false;
	REINITIALISATION_OF_to_be_fired_OF_radar_1 = false;
	boolState[already_standby_OF_radar_1] = false;
	boolState[already_required_OF_radar_1] = false;
	REINITIALISATION_OF_required_OF_ship_on_collision_course = true;
	boolState[already_S_OF_ship_on_collision_course] = false;
	REINITIALISATION_OF_S_OF_ship_on_collision_course = false;
	REINITIALISATION_OF_relevant_evt_OF_ship_on_collision_course = false;
	boolState[failF_OF_ship_on_collision_course] = false;
	REINITIALISATION_OF_required_OF_visual = true;
	boolState[already_S_OF_visual] = false;
	REINITIALISATION_OF_S_OF_visual = false;
	REINITIALISATION_OF_relevant_evt_OF_visual = false;
	boolState[failI_OF_visual] = false;
	REINITIALISATION_OF_to_be_fired_OF_visual = false;
	boolState[already_standby_OF_visual] = false;
	boolState[already_required_OF_visual] = false;
	REINITIALISATION_OF_required_OF_visual_1 = true;
	boolState[already_S_OF_visual_1] = false;
	REINITIALISATION_OF_S_OF_visual_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_visual_1 = false;
	boolState[failI_OF_visual_1] = false;
	REINITIALISATION_OF_to_be_fired_OF_visual_1 = false;
	boolState[already_standby_OF_visual_1] = false;
	boolState[already_required_OF_visual_1] = false;
	REINITIALISATION_OF_required_OF_warning = true;
	boolState[already_S_OF_warning] = false;
	REINITIALISATION_OF_S_OF_warning = false;
	REINITIALISATION_OF_relevant_evt_OF_warning = false;
	boolState[failI_OF_warning] = false;
	REINITIALISATION_OF_to_be_fired_OF_warning = false;
	boolState[already_standby_OF_warning] = false;
	boolState[already_required_OF_warning] = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx23_OF_course_not_changed_INS_0 = false;
	FIRE_xx23_OF_course_not_changed_INS_1 = false;
	FIRE_xx23_OF_radar_INS_2 = false;
	FIRE_xx23_OF_radar_INS_3 = false;
	FIRE_xx23_OF_radar_1_INS_4 = false;
	FIRE_xx23_OF_radar_1_INS_5 = false;
	FIRE_xx10_OF_ship_on_collision_course = false;
	FIRE_xx23_OF_visual_INS_7 = false;
	FIRE_xx23_OF_visual_INS_8 = false;
	FIRE_xx23_OF_visual_1_INS_9 = false;
	FIRE_xx23_OF_visual_1_INS_10 = false;
	FIRE_xx23_OF_warning_INS_11 = false;
	FIRE_xx23_OF_warning_INS_12 = false;

}

void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_A_ND_by_ship] | Value : " << boolState[required_OF_A_ND_by_ship] << endl;
	cout << "Attribute :  boolState[already_S_OF_A_ND_by_ship] | Value : " << boolState[already_S_OF_A_ND_by_ship] << endl;
	cout << "Attribute :  boolState[S_OF_A_ND_by_ship] | Value : " << boolState[S_OF_A_ND_by_ship] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A_ND_by_ship] | Value : " << boolState[relevant_evt_OF_A_ND_by_ship] << endl;
	cout << "Attribute :  boolState[required_OF_A_ND_by_ship_1] | Value : " << boolState[required_OF_A_ND_by_ship_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_A_ND_by_ship_1] | Value : " << boolState[already_S_OF_A_ND_by_ship_1] << endl;
	cout << "Attribute :  boolState[S_OF_A_ND_by_ship_1] | Value : " << boolState[S_OF_A_ND_by_ship_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A_ND_by_ship_1] | Value : " << boolState[relevant_evt_OF_A_ND_by_ship_1] << endl;
	cout << "Attribute :  boolState[required_OF_B_ND_by_platform] | Value : " << boolState[required_OF_B_ND_by_platform] << endl;
	cout << "Attribute :  boolState[already_S_OF_B_ND_by_platform] | Value : " << boolState[already_S_OF_B_ND_by_platform] << endl;
	cout << "Attribute :  boolState[S_OF_B_ND_by_platform] | Value : " << boolState[S_OF_B_ND_by_platform] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_B_ND_by_platform] | Value : " << boolState[relevant_evt_OF_B_ND_by_platform] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
	cout << "Attribute :  boolState[required_OF_collision] | Value : " << boolState[required_OF_collision] << endl;
	cout << "Attribute :  boolState[already_S_OF_collision] | Value : " << boolState[already_S_OF_collision] << endl;
	cout << "Attribute :  boolState[S_OF_collision] | Value : " << boolState[S_OF_collision] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_collision] | Value : " << boolState[relevant_evt_OF_collision] << endl;
	cout << "Attribute :  boolState[required_OF_course_not_changed] | Value : " << boolState[required_OF_course_not_changed] << endl;
	cout << "Attribute :  boolState[already_S_OF_course_not_changed] | Value : " << boolState[already_S_OF_course_not_changed] << endl;
	cout << "Attribute :  boolState[S_OF_course_not_changed] | Value : " << boolState[S_OF_course_not_changed] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_course_not_changed] | Value : " << boolState[relevant_evt_OF_course_not_changed] << endl;
	cout << "Attribute :  boolState[failI_OF_course_not_changed] | Value : " << boolState[failI_OF_course_not_changed] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_course_not_changed] | Value : " << boolState[to_be_fired_OF_course_not_changed] << endl;
	cout << "Attribute :  boolState[already_standby_OF_course_not_changed] | Value : " << boolState[already_standby_OF_course_not_changed] << endl;
	cout << "Attribute :  boolState[already_required_OF_course_not_changed] | Value : " << boolState[already_required_OF_course_not_changed] << endl;
	cout << "Attribute :  boolState[required_OF_non_detection] | Value : " << boolState[required_OF_non_detection] << endl;
	cout << "Attribute :  boolState[already_S_OF_non_detection] | Value : " << boolState[already_S_OF_non_detection] << endl;
	cout << "Attribute :  boolState[S_OF_non_detection] | Value : " << boolState[S_OF_non_detection] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_non_detection] | Value : " << boolState[relevant_evt_OF_non_detection] << endl;
	cout << "Attribute :  boolState[required_OF_radar] | Value : " << boolState[required_OF_radar] << endl;
	cout << "Attribute :  boolState[already_S_OF_radar] | Value : " << boolState[already_S_OF_radar] << endl;
	cout << "Attribute :  boolState[S_OF_radar] | Value : " << boolState[S_OF_radar] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_radar] | Value : " << boolState[relevant_evt_OF_radar] << endl;
	cout << "Attribute :  boolState[failI_OF_radar] | Value : " << boolState[failI_OF_radar] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_radar] | Value : " << boolState[to_be_fired_OF_radar] << endl;
	cout << "Attribute :  boolState[already_standby_OF_radar] | Value : " << boolState[already_standby_OF_radar] << endl;
	cout << "Attribute :  boolState[already_required_OF_radar] | Value : " << boolState[already_required_OF_radar] << endl;
	cout << "Attribute :  boolState[required_OF_radar_1] | Value : " << boolState[required_OF_radar_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_radar_1] | Value : " << boolState[already_S_OF_radar_1] << endl;
	cout << "Attribute :  boolState[S_OF_radar_1] | Value : " << boolState[S_OF_radar_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_radar_1] | Value : " << boolState[relevant_evt_OF_radar_1] << endl;
	cout << "Attribute :  boolState[failI_OF_radar_1] | Value : " << boolState[failI_OF_radar_1] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_radar_1] | Value : " << boolState[to_be_fired_OF_radar_1] << endl;
	cout << "Attribute :  boolState[already_standby_OF_radar_1] | Value : " << boolState[already_standby_OF_radar_1] << endl;
	cout << "Attribute :  boolState[already_required_OF_radar_1] | Value : " << boolState[already_required_OF_radar_1] << endl;
	cout << "Attribute :  boolState[required_OF_ship_on_collision_course] | Value : " << boolState[required_OF_ship_on_collision_course] << endl;
	cout << "Attribute :  boolState[already_S_OF_ship_on_collision_course] | Value : " << boolState[already_S_OF_ship_on_collision_course] << endl;
	cout << "Attribute :  boolState[S_OF_ship_on_collision_course] | Value : " << boolState[S_OF_ship_on_collision_course] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_ship_on_collision_course] | Value : " << boolState[relevant_evt_OF_ship_on_collision_course] << endl;
	cout << "Attribute :  boolState[failF_OF_ship_on_collision_course] | Value : " << boolState[failF_OF_ship_on_collision_course] << endl;
	cout << "Attribute :  boolState[required_OF_visual] | Value : " << boolState[required_OF_visual] << endl;
	cout << "Attribute :  boolState[already_S_OF_visual] | Value : " << boolState[already_S_OF_visual] << endl;
	cout << "Attribute :  boolState[S_OF_visual] | Value : " << boolState[S_OF_visual] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_visual] | Value : " << boolState[relevant_evt_OF_visual] << endl;
	cout << "Attribute :  boolState[failI_OF_visual] | Value : " << boolState[failI_OF_visual] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_visual] | Value : " << boolState[to_be_fired_OF_visual] << endl;
	cout << "Attribute :  boolState[already_standby_OF_visual] | Value : " << boolState[already_standby_OF_visual] << endl;
	cout << "Attribute :  boolState[already_required_OF_visual] | Value : " << boolState[already_required_OF_visual] << endl;
	cout << "Attribute :  boolState[required_OF_visual_1] | Value : " << boolState[required_OF_visual_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_visual_1] | Value : " << boolState[already_S_OF_visual_1] << endl;
	cout << "Attribute :  boolState[S_OF_visual_1] | Value : " << boolState[S_OF_visual_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_visual_1] | Value : " << boolState[relevant_evt_OF_visual_1] << endl;
	cout << "Attribute :  boolState[failI_OF_visual_1] | Value : " << boolState[failI_OF_visual_1] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_visual_1] | Value : " << boolState[to_be_fired_OF_visual_1] << endl;
	cout << "Attribute :  boolState[already_standby_OF_visual_1] | Value : " << boolState[already_standby_OF_visual_1] << endl;
	cout << "Attribute :  boolState[already_required_OF_visual_1] | Value : " << boolState[already_required_OF_visual_1] << endl;
	cout << "Attribute :  boolState[required_OF_warning] | Value : " << boolState[required_OF_warning] << endl;
	cout << "Attribute :  boolState[already_S_OF_warning] | Value : " << boolState[already_S_OF_warning] << endl;
	cout << "Attribute :  boolState[S_OF_warning] | Value : " << boolState[S_OF_warning] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_warning] | Value : " << boolState[relevant_evt_OF_warning] << endl;
	cout << "Attribute :  boolState[failI_OF_warning] | Value : " << boolState[failI_OF_warning] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_warning] | Value : " << boolState[to_be_fired_OF_warning] << endl;
	cout << "Attribute :  boolState[already_standby_OF_warning] | Value : " << boolState[already_standby_OF_warning] << endl;
	cout << "Attribute :  boolState[already_required_OF_warning] | Value : " << boolState[already_required_OF_warning] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::doReinitialisations()
{
	boolState[required_OF_A_ND_by_ship] = REINITIALISATION_OF_required_OF_A_ND_by_ship;
	boolState[S_OF_A_ND_by_ship] = REINITIALISATION_OF_S_OF_A_ND_by_ship;
	boolState[relevant_evt_OF_A_ND_by_ship] = REINITIALISATION_OF_relevant_evt_OF_A_ND_by_ship;
	boolState[required_OF_A_ND_by_ship_1] = REINITIALISATION_OF_required_OF_A_ND_by_ship_1;
	boolState[S_OF_A_ND_by_ship_1] = REINITIALISATION_OF_S_OF_A_ND_by_ship_1;
	boolState[relevant_evt_OF_A_ND_by_ship_1] = REINITIALISATION_OF_relevant_evt_OF_A_ND_by_ship_1;
	boolState[required_OF_B_ND_by_platform] = REINITIALISATION_OF_required_OF_B_ND_by_platform;
	boolState[S_OF_B_ND_by_platform] = REINITIALISATION_OF_S_OF_B_ND_by_platform;
	boolState[relevant_evt_OF_B_ND_by_platform] = REINITIALISATION_OF_relevant_evt_OF_B_ND_by_platform;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
	boolState[required_OF_collision] = REINITIALISATION_OF_required_OF_collision;
	boolState[S_OF_collision] = REINITIALISATION_OF_S_OF_collision;
	boolState[relevant_evt_OF_collision] = REINITIALISATION_OF_relevant_evt_OF_collision;
	boolState[required_OF_course_not_changed] = REINITIALISATION_OF_required_OF_course_not_changed;
	boolState[S_OF_course_not_changed] = REINITIALISATION_OF_S_OF_course_not_changed;
	boolState[relevant_evt_OF_course_not_changed] = REINITIALISATION_OF_relevant_evt_OF_course_not_changed;
	boolState[to_be_fired_OF_course_not_changed] = REINITIALISATION_OF_to_be_fired_OF_course_not_changed;
	boolState[required_OF_non_detection] = REINITIALISATION_OF_required_OF_non_detection;
	boolState[S_OF_non_detection] = REINITIALISATION_OF_S_OF_non_detection;
	boolState[relevant_evt_OF_non_detection] = REINITIALISATION_OF_relevant_evt_OF_non_detection;
	boolState[required_OF_radar] = REINITIALISATION_OF_required_OF_radar;
	boolState[S_OF_radar] = REINITIALISATION_OF_S_OF_radar;
	boolState[relevant_evt_OF_radar] = REINITIALISATION_OF_relevant_evt_OF_radar;
	boolState[to_be_fired_OF_radar] = REINITIALISATION_OF_to_be_fired_OF_radar;
	boolState[required_OF_radar_1] = REINITIALISATION_OF_required_OF_radar_1;
	boolState[S_OF_radar_1] = REINITIALISATION_OF_S_OF_radar_1;
	boolState[relevant_evt_OF_radar_1] = REINITIALISATION_OF_relevant_evt_OF_radar_1;
	boolState[to_be_fired_OF_radar_1] = REINITIALISATION_OF_to_be_fired_OF_radar_1;
	boolState[required_OF_ship_on_collision_course] = REINITIALISATION_OF_required_OF_ship_on_collision_course;
	boolState[S_OF_ship_on_collision_course] = REINITIALISATION_OF_S_OF_ship_on_collision_course;
	boolState[relevant_evt_OF_ship_on_collision_course] = REINITIALISATION_OF_relevant_evt_OF_ship_on_collision_course;
	boolState[required_OF_visual] = REINITIALISATION_OF_required_OF_visual;
	boolState[S_OF_visual] = REINITIALISATION_OF_S_OF_visual;
	boolState[relevant_evt_OF_visual] = REINITIALISATION_OF_relevant_evt_OF_visual;
	boolState[to_be_fired_OF_visual] = REINITIALISATION_OF_to_be_fired_OF_visual;
	boolState[required_OF_visual_1] = REINITIALISATION_OF_required_OF_visual_1;
	boolState[S_OF_visual_1] = REINITIALISATION_OF_S_OF_visual_1;
	boolState[relevant_evt_OF_visual_1] = REINITIALISATION_OF_relevant_evt_OF_visual_1;
	boolState[to_be_fired_OF_visual_1] = REINITIALISATION_OF_to_be_fired_OF_visual_1;
	boolState[required_OF_warning] = REINITIALISATION_OF_required_OF_warning;
	boolState[S_OF_warning] = REINITIALISATION_OF_S_OF_warning;
	boolState[relevant_evt_OF_warning] = REINITIALISATION_OF_relevant_evt_OF_warning;
	boolState[to_be_fired_OF_warning] = REINITIALISATION_OF_to_be_fired_OF_warning;
}

void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx23_OF_course_not_changed_INS_0 = true;
	}

	if (numFire == 1)
	{
		FIRE_xx23_OF_course_not_changed_INS_1 = true;
	}

	if (numFire == 2)
	{
		FIRE_xx23_OF_radar_INS_2 = true;
	}

	if (numFire == 3)
	{
		FIRE_xx23_OF_radar_INS_3 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx23_OF_radar_1_INS_4 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx23_OF_radar_1_INS_5 = true;
	}

	if (numFire == 6)
	{
		FIRE_xx10_OF_ship_on_collision_course = true;
	}

	if (numFire == 7)
	{
		FIRE_xx23_OF_visual_INS_7 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx23_OF_visual_INS_8 = true;
	}

	if (numFire == 9)
	{
		FIRE_xx23_OF_visual_1_INS_9 = true;
	}

	if (numFire == 10)
	{
		FIRE_xx23_OF_visual_1_INS_10 = true;
	}

	if (numFire == 11)
	{
		FIRE_xx23_OF_warning_INS_11 = true;
	}

	if (numFire == 12)
	{
		FIRE_xx23_OF_warning_INS_12 = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx23_OF_course_not_changed

	if ((boolState[failI_OF_course_not_changed] == false) && boolState[to_be_fired_OF_course_not_changed]) 
	{
	
		
		if (FIRE_xx23_OF_course_not_changed_INS_0) 
		{
			boolState[failI_OF_course_not_changed]  =  true;
			boolState[already_standby_OF_course_not_changed]  =  false;
			boolState[already_required_OF_course_not_changed]  =  false;
			FIRE_xx23_OF_course_not_changed_INS_0 = false;
		}
	
	}
	if ((boolState[failI_OF_course_not_changed] == false) && boolState[to_be_fired_OF_course_not_changed]) 
	{
	
		
		if (FIRE_xx23_OF_course_not_changed_INS_1) 
		{
			boolState[already_standby_OF_course_not_changed]  =  false;
			boolState[already_required_OF_course_not_changed]  =  false;
			FIRE_xx23_OF_course_not_changed_INS_1 = false;
		}
	
	}
	// Occurrence xx23_OF_radar

	if ((boolState[failI_OF_radar] == false) && boolState[to_be_fired_OF_radar]) 
	{
	
		
		if (FIRE_xx23_OF_radar_INS_2) 
		{
			boolState[failI_OF_radar]  =  true;
			boolState[already_standby_OF_radar]  =  false;
			boolState[already_required_OF_radar]  =  false;
			FIRE_xx23_OF_radar_INS_2 = false;
		}
	
	}
	if ((boolState[failI_OF_radar] == false) && boolState[to_be_fired_OF_radar]) 
	{
	
		
		if (FIRE_xx23_OF_radar_INS_3) 
		{
			boolState[already_standby_OF_radar]  =  false;
			boolState[already_required_OF_radar]  =  false;
			FIRE_xx23_OF_radar_INS_3 = false;
		}
	
	}
	// Occurrence xx23_OF_radar_1

	if ((boolState[failI_OF_radar_1] == false) && boolState[to_be_fired_OF_radar_1]) 
	{
	
		
		if (FIRE_xx23_OF_radar_1_INS_4) 
		{
			boolState[failI_OF_radar_1]  =  true;
			boolState[already_standby_OF_radar_1]  =  false;
			boolState[already_required_OF_radar_1]  =  false;
			FIRE_xx23_OF_radar_1_INS_4 = false;
		}
	
	}
	if ((boolState[failI_OF_radar_1] == false) && boolState[to_be_fired_OF_radar_1]) 
	{
	
		
		if (FIRE_xx23_OF_radar_1_INS_5) 
		{
			boolState[already_standby_OF_radar_1]  =  false;
			boolState[already_required_OF_radar_1]  =  false;
			FIRE_xx23_OF_radar_1_INS_5 = false;
		}
	
	}
	// Occurrence xx10_OF_ship_on_collision_course
	if ((boolState[failF_OF_ship_on_collision_course] == false) && boolState[required_OF_ship_on_collision_course]) 
	{
		 
		if (FIRE_xx10_OF_ship_on_collision_course)
		{
			boolState[failF_OF_ship_on_collision_course]  =  true;
			FIRE_xx10_OF_ship_on_collision_course = false;
		}
	}

	// Occurrence xx23_OF_visual

	if ((boolState[failI_OF_visual] == false) && boolState[to_be_fired_OF_visual]) 
	{
	
		
		if (FIRE_xx23_OF_visual_INS_7) 
		{
			boolState[failI_OF_visual]  =  true;
			boolState[already_standby_OF_visual]  =  false;
			boolState[already_required_OF_visual]  =  false;
			FIRE_xx23_OF_visual_INS_7 = false;
		}
	
	}
	if ((boolState[failI_OF_visual] == false) && boolState[to_be_fired_OF_visual]) 
	{
	
		
		if (FIRE_xx23_OF_visual_INS_8) 
		{
			boolState[already_standby_OF_visual]  =  false;
			boolState[already_required_OF_visual]  =  false;
			FIRE_xx23_OF_visual_INS_8 = false;
		}
	
	}
	// Occurrence xx23_OF_visual_1

	if ((boolState[failI_OF_visual_1] == false) && boolState[to_be_fired_OF_visual_1]) 
	{
	
		
		if (FIRE_xx23_OF_visual_1_INS_9) 
		{
			boolState[failI_OF_visual_1]  =  true;
			boolState[already_standby_OF_visual_1]  =  false;
			boolState[already_required_OF_visual_1]  =  false;
			FIRE_xx23_OF_visual_1_INS_9 = false;
		}
	
	}
	if ((boolState[failI_OF_visual_1] == false) && boolState[to_be_fired_OF_visual_1]) 
	{
	
		
		if (FIRE_xx23_OF_visual_1_INS_10) 
		{
			boolState[already_standby_OF_visual_1]  =  false;
			boolState[already_required_OF_visual_1]  =  false;
			FIRE_xx23_OF_visual_1_INS_10 = false;
		}
	
	}
	// Occurrence xx23_OF_warning

	if ((boolState[failI_OF_warning] == false) && boolState[to_be_fired_OF_warning]) 
	{
	
		
		if (FIRE_xx23_OF_warning_INS_11) 
		{
			boolState[failI_OF_warning]  =  true;
			boolState[already_standby_OF_warning]  =  false;
			boolState[already_required_OF_warning]  =  false;
			FIRE_xx23_OF_warning_INS_11 = false;
		}
	
	}
	if ((boolState[failI_OF_warning] == false) && boolState[to_be_fired_OF_warning]) 
	{
	
		
		if (FIRE_xx23_OF_warning_INS_12) 
		{
			boolState[already_standby_OF_warning]  =  false;
			boolState[already_required_OF_warning]  =  false;
			FIRE_xx23_OF_warning_INS_12 = false;
		}
	
	}
}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if ((boolState[failI_OF_course_not_changed] == false) && boolState[to_be_fired_OF_course_not_changed])
	{
		cout << "0 :  INS_SUB_COUNT (1) |FAULT | failI  LABEL \"instantaneous failure course_not_changed\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_course_not_changed]  =  TRUE,already_standby_OF_course_not_changed  =  FALSE,already_required_OF_course_not_changed  =  FALSE" << endl;
		list.push_back(make_tuple(0, 0.0001, "INS", 1));
	}
	if ((boolState[failI_OF_course_not_changed] == false) && boolState[to_be_fired_OF_course_not_changed])
	{
		cout << "1 :  INS_SUB_COUNT (1) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_course_not_changed]  =  FALSE,already_required_OF_course_not_changed  =  FALSE" << endl;
		list.push_back(make_tuple(1, 0.9999, "INS", 1));
	}
	if ((boolState[failI_OF_radar] == false) && boolState[to_be_fired_OF_radar])
	{
		cout << "2 :  INS_SUB_COUNT (2) |FAULT | failI  LABEL \"instantaneous failure radar\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_radar]  =  TRUE,already_standby_OF_radar  =  FALSE,already_required_OF_radar  =  FALSE" << endl;
		list.push_back(make_tuple(2, 0.0001, "INS", 2));
	}
	if ((boolState[failI_OF_radar] == false) && boolState[to_be_fired_OF_radar])
	{
		cout << "3 :  INS_SUB_COUNT (2) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_radar]  =  FALSE,already_required_OF_radar  =  FALSE" << endl;
		list.push_back(make_tuple(3, 0.9999, "INS", 2));
	}
	if ((boolState[failI_OF_radar_1] == false) && boolState[to_be_fired_OF_radar_1])
	{
		cout << "4 :  INS_SUB_COUNT (3) |FAULT | failI  LABEL \"instantaneous failure radar_1\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_radar_1]  =  TRUE,already_standby_OF_radar_1  =  FALSE,already_required_OF_radar_1  =  FALSE" << endl;
		list.push_back(make_tuple(4, 0.0001, "INS", 3));
	}
	if ((boolState[failI_OF_radar_1] == false) && boolState[to_be_fired_OF_radar_1])
	{
		cout << "5 :  INS_SUB_COUNT (3) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_radar_1]  =  FALSE,already_required_OF_radar_1  =  FALSE" << endl;
		list.push_back(make_tuple(5, 0.9999, "INS", 3));
	}
	if ((boolState[failI_OF_visual] == false) && boolState[to_be_fired_OF_visual])
	{
		cout << "7 :  INS_SUB_COUNT (4) |FAULT | failI  LABEL \"instantaneous failure visual\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_visual]  =  TRUE,already_standby_OF_visual  =  FALSE,already_required_OF_visual  =  FALSE" << endl;
		list.push_back(make_tuple(7, 0.0001, "INS", 4));
	}
	if ((boolState[failI_OF_visual] == false) && boolState[to_be_fired_OF_visual])
	{
		cout << "8 :  INS_SUB_COUNT (4) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_visual]  =  FALSE,already_required_OF_visual  =  FALSE" << endl;
		list.push_back(make_tuple(8, 0.9999, "INS", 4));
	}
	if ((boolState[failI_OF_visual_1] == false) && boolState[to_be_fired_OF_visual_1])
	{
		cout << "9 :  INS_SUB_COUNT (5) |FAULT | failI  LABEL \"instantaneous failure visual_1\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_visual_1]  =  TRUE,already_standby_OF_visual_1  =  FALSE,already_required_OF_visual_1  =  FALSE" << endl;
		list.push_back(make_tuple(9, 0.0001, "INS", 5));
	}
	if ((boolState[failI_OF_visual_1] == false) && boolState[to_be_fired_OF_visual_1])
	{
		cout << "10 :  INS_SUB_COUNT (5) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_visual_1]  =  FALSE,already_required_OF_visual_1  =  FALSE" << endl;
		list.push_back(make_tuple(10, 0.9999, "INS", 5));
	}
	if ((boolState[failI_OF_warning] == false) && boolState[to_be_fired_OF_warning])
	{
		cout << "11 :  INS_SUB_COUNT (6) |FAULT | failI  LABEL \"instantaneous failure warning\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_warning]  =  TRUE,already_standby_OF_warning  =  FALSE,already_required_OF_warning  =  FALSE" << endl;
		list.push_back(make_tuple(11, 0.0001, "INS", 6));
	}
	if ((boolState[failI_OF_warning] == false) && boolState[to_be_fired_OF_warning])
	{
		cout << "12 :  INS_SUB_COUNT (6) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_warning]  =  FALSE,already_required_OF_warning  =  FALSE" << endl;
		list.push_back(make_tuple(12, 0.9999, "INS", 6));
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
     
	if ((boolState[failF_OF_ship_on_collision_course] == false) && boolState[required_OF_ship_on_collision_course])
	{
		cout << "6 : xx10_OF_ship_on_collision_course : FAULT failF  LABEL \"failure in operation ship_on_collision_course\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_ship_on_collision_course]  =  TRUE" << endl;
		list.push_back(make_tuple(6, 0.0001, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failI_OF_course_not_changed] == true )
	{
		boolState[S_OF_course_not_changed]  =  true;
	}

	if (boolState[failI_OF_radar] == true )
	{
		boolState[S_OF_radar]  =  true;
	}

	if (boolState[failI_OF_radar_1] == true )
	{
		boolState[S_OF_radar_1]  =  true;
	}

	if (boolState[failF_OF_ship_on_collision_course] == true )
	{
		boolState[S_OF_ship_on_collision_course]  =  true;
	}

	if (boolState[failI_OF_visual] == true )
	{
		boolState[S_OF_visual]  =  true;
	}

	if (boolState[failI_OF_visual_1] == true )
	{
		boolState[S_OF_visual_1]  =  true;
	}

	if (boolState[failI_OF_warning] == true )
	{
		boolState[S_OF_warning]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_radar] && boolState[S_OF_visual] )
	{
		boolState[S_OF_A_ND_by_ship]  =  true;
	}

	if (boolState[S_OF_radar_1] && boolState[S_OF_visual_1] )
	{
		boolState[S_OF_A_ND_by_ship_1]  =  true;
	}

	if (boolState[S_OF_A_ND_by_ship_1] || boolState[S_OF_warning] )
	{
		boolState[S_OF_B_ND_by_platform]  =  true;
	}

	if (boolState[S_OF_collision] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

	if (boolState[S_OF_course_not_changed] || boolState[S_OF_non_detection] )
	{
		boolState[S_OF_collision]  =  true;
	}

	if (boolState[S_OF_A_ND_by_ship] && boolState[S_OF_B_ND_by_platform] )
	{
		boolState[S_OF_non_detection]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_non_detection] )
	{
		boolState[required_OF_A_ND_by_ship]  =  false;
	}

	if (boolState[relevant_evt_OF_non_detection] && ( !boolState[S_OF_non_detection]) )
	{
		boolState[relevant_evt_OF_A_ND_by_ship]  =  true;
	}

	if ( !boolState[required_OF_B_ND_by_platform] )
	{
		boolState[required_OF_A_ND_by_ship_1]  =  false;
	}

	if (boolState[relevant_evt_OF_B_ND_by_platform] && ( !boolState[S_OF_B_ND_by_platform]) )
	{
		boolState[relevant_evt_OF_A_ND_by_ship_1]  =  true;
	}

	if ( !boolState[required_OF_non_detection] )
	{
		boolState[required_OF_B_ND_by_platform]  =  false;
	}

	if (boolState[relevant_evt_OF_non_detection] && ( !boolState[S_OF_non_detection]) )
	{
		boolState[relevant_evt_OF_B_ND_by_platform]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

	if (( !boolState[required_OF_UE_1]) || ( !boolState[S_OF_ship_on_collision_course]) )
	{
		boolState[required_OF_collision]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_collision]  =  true;
	}

	if ( !boolState[required_OF_collision] )
	{
		boolState[required_OF_course_not_changed]  =  false;
	}

	if (boolState[relevant_evt_OF_collision] && ( !boolState[S_OF_collision]) )
	{
		boolState[relevant_evt_OF_course_not_changed]  =  true;
	}

	if ( !boolState[required_OF_collision] )
	{
		boolState[required_OF_non_detection]  =  false;
	}

	if (boolState[relevant_evt_OF_collision] && ( !boolState[S_OF_collision]) )
	{
		boolState[relevant_evt_OF_non_detection]  =  true;
	}

	if ( !boolState[required_OF_A_ND_by_ship] )
	{
		boolState[required_OF_radar]  =  false;
	}

	if (boolState[relevant_evt_OF_A_ND_by_ship] && ( !boolState[S_OF_A_ND_by_ship]) )
	{
		boolState[relevant_evt_OF_radar]  =  true;
	}

	if ( !boolState[required_OF_A_ND_by_ship_1] )
	{
		boolState[required_OF_radar_1]  =  false;
	}

	if (boolState[relevant_evt_OF_A_ND_by_ship_1] && ( !boolState[S_OF_A_ND_by_ship_1]) )
	{
		boolState[relevant_evt_OF_radar_1]  =  true;
	}

	if (boolState[relevant_evt_OF_collision] && ( !boolState[S_OF_collision]) )
	{
		boolState[relevant_evt_OF_ship_on_collision_course]  =  true;
	}

	if ( !boolState[required_OF_A_ND_by_ship] )
	{
		boolState[required_OF_visual]  =  false;
	}

	if (boolState[relevant_evt_OF_A_ND_by_ship] && ( !boolState[S_OF_A_ND_by_ship]) )
	{
		boolState[relevant_evt_OF_visual]  =  true;
	}

	if ( !boolState[required_OF_A_ND_by_ship_1] )
	{
		boolState[required_OF_visual_1]  =  false;
	}

	if (boolState[relevant_evt_OF_A_ND_by_ship_1] && ( !boolState[S_OF_A_ND_by_ship_1]) )
	{
		boolState[relevant_evt_OF_visual_1]  =  true;
	}

	if ( !boolState[required_OF_B_ND_by_platform] )
	{
		boolState[required_OF_warning]  =  false;
	}

	if (boolState[relevant_evt_OF_B_ND_by_platform] && ( !boolState[S_OF_B_ND_by_platform]) )
	{
		boolState[relevant_evt_OF_warning]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_A_ND_by_ship]  =  boolState[S_OF_A_ND_by_ship]  ;



	boolState[already_S_OF_A_ND_by_ship_1]  =  boolState[S_OF_A_ND_by_ship_1]  ;



	boolState[already_S_OF_B_ND_by_platform]  =  boolState[S_OF_B_ND_by_platform]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;



	boolState[already_S_OF_collision]  =  boolState[S_OF_collision]  ;



	boolState[already_S_OF_course_not_changed]  =  boolState[S_OF_course_not_changed]  ;

	if (( !boolState[required_OF_course_not_changed]) && (( !  boolState[already_standby_OF_course_not_changed]) && ( !  boolState[already_required_OF_course_not_changed])) )
	{
		boolState[already_standby_OF_course_not_changed]  =  true;
	}



	boolState[already_S_OF_non_detection]  =  boolState[S_OF_non_detection]  ;



	boolState[already_S_OF_radar]  =  boolState[S_OF_radar]  ;

	if (( !boolState[required_OF_radar]) && (( !boolState[already_standby_OF_radar]) && ( !boolState[already_required_OF_radar])) )
	{
		boolState[already_standby_OF_radar]  =  true;
	}



	boolState[already_S_OF_radar_1]  =  boolState[S_OF_radar_1]  ;

	if (( !boolState[required_OF_radar_1]) && (( !boolState[already_standby_OF_radar_1]) && ( !boolState[already_required_OF_radar_1])) )
	{
		boolState[already_standby_OF_radar_1]  =  true;
	}



	boolState[already_S_OF_ship_on_collision_course]  =  boolState[S_OF_ship_on_collision_course]  ;



	boolState[already_S_OF_visual]  =  boolState[S_OF_visual]  ;

	if (( !boolState[required_OF_visual]) && (( !boolState[already_standby_OF_visual])   && ( !boolState[already_required_OF_visual])) )
	{
		boolState[already_standby_OF_visual]  =  true;
	}



	boolState[already_S_OF_visual_1]  =  boolState[S_OF_visual_1]  ;

	if (( !boolState[required_OF_visual_1]) && (( !boolState[already_standby_OF_visual_1]) && ( !boolState[already_required_OF_visual_1])) )
	{
		boolState[already_standby_OF_visual_1]  =  true;
	}



	boolState[already_S_OF_warning]  =  boolState[S_OF_warning]  ;

	if (( !boolState[required_OF_warning]) && (( !boolState[already_standby_OF_warning]) && ( !boolState[already_required_OF_warning])) )
	{
		boolState[already_standby_OF_warning]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::runOnceInteractionStep_tops()
{
	if (boolState[required_OF_course_not_changed] && boolState[already_standby_OF_course_not_changed] )
	{
		boolState[to_be_fired_OF_course_not_changed]  =  true;
	}

	if (boolState[required_OF_radar] && boolState[already_standby_OF_radar] )
	{
		boolState[to_be_fired_OF_radar]  =  true;
	}

	if (boolState[required_OF_radar_1] && boolState[already_standby_OF_radar_1] )
	{
		boolState[to_be_fired_OF_radar_1]  =  true;
	}

	if (boolState[required_OF_visual] && boolState[already_standby_OF_visual] )
	{
		boolState[to_be_fired_OF_visual]  =  true;
	}

	if (boolState[required_OF_visual_1] && boolState[already_standby_OF_visual_1] )
	{
		boolState[to_be_fired_OF_visual_1]  =  true;
	}

	if (boolState[required_OF_warning] && boolState[already_standby_OF_warning] )
	{
		boolState[to_be_fired_OF_warning]  =  true;
	}

}

void
storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_18_ESREL_2013_Event_trees_and_Petri_nets_No_trim_No_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    