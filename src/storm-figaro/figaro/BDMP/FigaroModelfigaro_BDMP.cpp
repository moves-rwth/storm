#include <iostream>
#include "FigaroModelfigaro_BDMP.h"

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
        
void storm::figaro::FigaroProgram_figaro_BDMP::init()
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
	REINITIALISATION_OF_required_OF_A_and_B_unavailable = true;
	boolState[already_S_OF_A_and_B_unavailable] = false;
	REINITIALISATION_OF_S_OF_A_and_B_unavailable = false;
	REINITIALISATION_OF_relevant_evt_OF_A_and_B_unavailable = false;
	REINITIALISATION_OF_required_OF_A_or_B_isolated = true;
	boolState[already_S_OF_A_or_B_isolated] = false;
	REINITIALISATION_OF_S_OF_A_or_B_isolated = false;
	REINITIALISATION_OF_relevant_evt_OF_A_or_B_isolated = false;
	REINITIALISATION_OF_required_OF_A_unavailable = true;
	boolState[already_S_OF_A_unavailable] = false;
	REINITIALISATION_OF_S_OF_A_unavailable = false;
	REINITIALISATION_OF_relevant_evt_OF_A_unavailable = false;
	REINITIALISATION_OF_required_OF_B_unavailable = true;
	boolState[already_S_OF_B_unavailable] = false;
	REINITIALISATION_OF_S_OF_B_unavailable = false;
	REINITIALISATION_OF_relevant_evt_OF_B_unavailable = false;
	REINITIALISATION_OF_required_OF_FailureOfA = true;
	boolState[already_S_OF_FailureOfA] = false;
	REINITIALISATION_OF_S_OF_FailureOfA = false;
	REINITIALISATION_OF_relevant_evt_OF_FailureOfA = false;
	boolState[failF_OF_FailureOfA] = false;
	REINITIALISATION_OF_required_OF_FailureOfB = true;
	boolState[already_S_OF_FailureOfB] = false;
	REINITIALISATION_OF_S_OF_FailureOfB = false;
	REINITIALISATION_OF_relevant_evt_OF_FailureOfB = false;
	boolState[failF_OF_FailureOfB] = false;
	REINITIALISATION_OF_required_OF_IO_K1 = true;
	boolState[already_S_OF_IO_K1] = false;
	REINITIALISATION_OF_S_OF_IO_K1 = false;
	REINITIALISATION_OF_relevant_evt_OF_IO_K1 = false;
	boolState[failF_OF_IO_K1] = false;
	REINITIALISATION_OF_required_OF_IO_K2 = true;
	boolState[already_S_OF_IO_K2] = false;
	REINITIALISATION_OF_S_OF_IO_K2 = false;
	REINITIALISATION_OF_relevant_evt_OF_IO_K2 = false;
	boolState[failF_OF_IO_K2] = false;
	REINITIALISATION_OF_required_OF_IO_K3 = true;
	boolState[already_S_OF_IO_K3] = false;
	REINITIALISATION_OF_S_OF_IO_K3 = false;
	REINITIALISATION_OF_relevant_evt_OF_IO_K3 = false;
	boolState[failF_OF_IO_K3] = false;
	REINITIALISATION_OF_required_OF_IO_K4 = true;
	boolState[already_S_OF_IO_K4] = false;
	REINITIALISATION_OF_S_OF_IO_K4 = false;
	REINITIALISATION_OF_relevant_evt_OF_IO_K4 = false;
	boolState[failF_OF_IO_K4] = false;
	REINITIALISATION_OF_required_OF_IO_K5 = true;
	boolState[already_S_OF_IO_K5] = false;
	REINITIALISATION_OF_S_OF_IO_K5 = false;
	REINITIALISATION_OF_relevant_evt_OF_IO_K5 = false;
	boolState[failF_OF_IO_K5] = false;
	REINITIALISATION_OF_required_OF_RC_K5 = true;
	boolState[already_S_OF_RC_K5] = false;
	REINITIALISATION_OF_S_OF_RC_K5 = false;
	REINITIALISATION_OF_relevant_evt_OF_RC_K5 = false;
	boolState[failI_OF_RC_K5] = false;
	REINITIALISATION_OF_to_be_fired_OF_RC_K5 = false;
	boolState[already_standby_OF_RC_K5] = false;
	boolState[already_required_OF_RC_K5] = false;
	REINITIALISATION_OF_required_OF_RO_K1 = true;
	boolState[already_S_OF_RO_K1] = false;
	REINITIALISATION_OF_S_OF_RO_K1 = false;
	REINITIALISATION_OF_relevant_evt_OF_RO_K1 = false;
	boolState[failI_OF_RO_K1] = false;
	REINITIALISATION_OF_to_be_fired_OF_RO_K1 = false;
	boolState[already_standby_OF_RO_K1] = false;
	boolState[already_required_OF_RO_K1] = false;
	REINITIALISATION_OF_required_OF_RO_K2 = true;
	boolState[already_S_OF_RO_K2] = false;
	REINITIALISATION_OF_S_OF_RO_K2 = false;
	REINITIALISATION_OF_relevant_evt_OF_RO_K2 = false;
	boolState[failI_OF_RO_K2] = false;
	REINITIALISATION_OF_to_be_fired_OF_RO_K2 = false;
	boolState[already_standby_OF_RO_K2] = false;
	boolState[already_required_OF_RO_K2] = false;
	REINITIALISATION_OF_required_OF_RO_K3 = true;
	boolState[already_S_OF_RO_K3] = false;
	REINITIALISATION_OF_S_OF_RO_K3 = false;
	REINITIALISATION_OF_relevant_evt_OF_RO_K3 = false;
	boolState[failI_OF_RO_K3] = false;
	REINITIALISATION_OF_to_be_fired_OF_RO_K3 = false;
	boolState[already_standby_OF_RO_K3] = false;
	boolState[already_required_OF_RO_K3] = false;
	REINITIALISATION_OF_required_OF_RO_K4 = true;
	boolState[already_S_OF_RO_K4] = false;
	REINITIALISATION_OF_S_OF_RO_K4 = false;
	REINITIALISATION_OF_relevant_evt_OF_RO_K4 = false;
	boolState[failI_OF_RO_K4] = false;
	REINITIALISATION_OF_to_be_fired_OF_RO_K4 = false;
	boolState[already_standby_OF_RO_K4] = false;
	boolState[already_required_OF_RO_K4] = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;
	REINITIALISATION_OF_required_OF_due_to_A = true;
	boolState[already_S_OF_due_to_A] = false;
	REINITIALISATION_OF_S_OF_due_to_A = false;
	REINITIALISATION_OF_relevant_evt_OF_due_to_A = false;
	REINITIALISATION_OF_required_OF_due_to_B = true;
	boolState[already_S_OF_due_to_B] = false;
	REINITIALISATION_OF_S_OF_due_to_B = false;
	REINITIALISATION_OF_relevant_evt_OF_due_to_B = false;
	REINITIALISATION_OF_required_OF_failure_on_phase_change = true;
	boolState[already_S_OF_failure_on_phase_change] = false;
	REINITIALISATION_OF_S_OF_failure_on_phase_change = false;
	REINITIALISATION_OF_relevant_evt_OF_failure_on_phase_change = false;
	REINITIALISATION_OF_required_OF_impossible_to_isolate_A = true;
	boolState[already_S_OF_impossible_to_isolate_A] = false;
	REINITIALISATION_OF_S_OF_impossible_to_isolate_A = false;
	REINITIALISATION_OF_relevant_evt_OF_impossible_to_isolate_A = false;
	REINITIALISATION_OF_required_OF_phase_1 = true;
	boolState[already_S_OF_phase_1] = false;
	REINITIALISATION_OF_S_OF_phase_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_phase_1 = false;
	boolState[in_progress_OF_phase_1] = true;
	boolState[already_required_OF_phase_1] = false;
	boolState[start_phase_OF_phase_1] = false;
	REINITIALISATION_OF_required_OF_impossible_to_isolate_B = true;
	boolState[already_S_OF_impossible_to_isolate_B] = false;
	REINITIALISATION_OF_S_OF_impossible_to_isolate_B = false;
	REINITIALISATION_OF_relevant_evt_OF_impossible_to_isolate_B = false;
	REINITIALISATION_OF_required_OF_phase_2 = true;
	boolState[already_S_OF_phase_2] = false;
	REINITIALISATION_OF_S_OF_phase_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_phase_2 = false;
	boolState[in_progress_OF_phase_2] = false;
	boolState[already_required_OF_phase_2] = false;
	boolState[start_phase_OF_phase_2] = false;
	REINITIALISATION_OF_required_OF_short_circuit = true;
	boolState[already_S_OF_short_circuit] = false;
	REINITIALISATION_OF_S_OF_short_circuit = false;
	REINITIALISATION_OF_relevant_evt_OF_short_circuit = false;
	REINITIALISATION_OF_required_OF_system_failure = true;
	boolState[already_S_OF_system_failure] = false;
	REINITIALISATION_OF_S_OF_system_failure = false;
	REINITIALISATION_OF_relevant_evt_OF_system_failure = false;
	REINITIALISATION_OF_required_OF_system_failure_in_phase_1 = true;
	boolState[already_S_OF_system_failure_in_phase_1] = false;
	REINITIALISATION_OF_S_OF_system_failure_in_phase_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_system_failure_in_phase_1 = false;
	REINITIALISATION_OF_required_OF_system_failure_in_phase_2 = true;
	boolState[already_S_OF_system_failure_in_phase_2] = false;
	REINITIALISATION_OF_S_OF_system_failure_in_phase_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_system_failure_in_phase_2 = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_FailureOfA = false;
	FIRE_xx10_OF_FailureOfB = false;
	FIRE_xx10_OF_IO_K1 = false;
	FIRE_xx10_OF_IO_K2 = false;
	FIRE_xx10_OF_IO_K3 = false;
	FIRE_xx10_OF_IO_K4 = false;
	FIRE_xx10_OF_IO_K5 = false;
	FIRE_xx23_OF_RC_K5_INS_7 = false;
	FIRE_xx23_OF_RC_K5_INS_8 = false;
	FIRE_xx23_OF_RO_K1_INS_9 = false;
	FIRE_xx23_OF_RO_K1_INS_10 = false;
	FIRE_xx23_OF_RO_K2_INS_11 = false;
	FIRE_xx23_OF_RO_K2_INS_12 = false;
	FIRE_xx23_OF_RO_K3_INS_13 = false;
	FIRE_xx23_OF_RO_K3_INS_14 = false;
	FIRE_xx23_OF_RO_K4_INS_15 = false;
	FIRE_xx23_OF_RO_K4_INS_16 = false;
	FIRE_xx43_a_OF_phase_1 = false;
	FIRE_xx47_OF_phase_1_INS_18 = false;
	FIRE_xx43_a_OF_phase_2 = false;
	FIRE_xx47_OF_phase_2_INS_20 = false;

}

void storm::figaro::FigaroProgram_figaro_BDMP::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_figaro_BDMP::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_figaro_BDMP::printState()
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
	cout << "Attribute :  boolState[required_OF_A_and_B_unavailable] | Value : " << boolState[required_OF_A_and_B_unavailable] << endl;
	cout << "Attribute :  boolState[already_S_OF_A_and_B_unavailable] | Value : " << boolState[already_S_OF_A_and_B_unavailable] << endl;
	cout << "Attribute :  boolState[S_OF_A_and_B_unavailable] | Value : " << boolState[S_OF_A_and_B_unavailable] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A_and_B_unavailable] | Value : " << boolState[relevant_evt_OF_A_and_B_unavailable] << endl;
	cout << "Attribute :  boolState[required_OF_A_or_B_isolated] | Value : " << boolState[required_OF_A_or_B_isolated] << endl;
	cout << "Attribute :  boolState[already_S_OF_A_or_B_isolated] | Value : " << boolState[already_S_OF_A_or_B_isolated] << endl;
	cout << "Attribute :  boolState[S_OF_A_or_B_isolated] | Value : " << boolState[S_OF_A_or_B_isolated] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A_or_B_isolated] | Value : " << boolState[relevant_evt_OF_A_or_B_isolated] << endl;
	cout << "Attribute :  boolState[required_OF_A_unavailable] | Value : " << boolState[required_OF_A_unavailable] << endl;
	cout << "Attribute :  boolState[already_S_OF_A_unavailable] | Value : " << boolState[already_S_OF_A_unavailable] << endl;
	cout << "Attribute :  boolState[S_OF_A_unavailable] | Value : " << boolState[S_OF_A_unavailable] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_A_unavailable] | Value : " << boolState[relevant_evt_OF_A_unavailable] << endl;
	cout << "Attribute :  boolState[required_OF_B_unavailable] | Value : " << boolState[required_OF_B_unavailable] << endl;
	cout << "Attribute :  boolState[already_S_OF_B_unavailable] | Value : " << boolState[already_S_OF_B_unavailable] << endl;
	cout << "Attribute :  boolState[S_OF_B_unavailable] | Value : " << boolState[S_OF_B_unavailable] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_B_unavailable] | Value : " << boolState[relevant_evt_OF_B_unavailable] << endl;
	cout << "Attribute :  boolState[required_OF_FailureOfA] | Value : " << boolState[required_OF_FailureOfA] << endl;
	cout << "Attribute :  boolState[already_S_OF_FailureOfA] | Value : " << boolState[already_S_OF_FailureOfA] << endl;
	cout << "Attribute :  boolState[S_OF_FailureOfA] | Value : " << boolState[S_OF_FailureOfA] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_FailureOfA] | Value : " << boolState[relevant_evt_OF_FailureOfA] << endl;
	cout << "Attribute :  boolState[failF_OF_FailureOfA] | Value : " << boolState[failF_OF_FailureOfA] << endl;
	cout << "Attribute :  boolState[required_OF_FailureOfB] | Value : " << boolState[required_OF_FailureOfB] << endl;
	cout << "Attribute :  boolState[already_S_OF_FailureOfB] | Value : " << boolState[already_S_OF_FailureOfB] << endl;
	cout << "Attribute :  boolState[S_OF_FailureOfB] | Value : " << boolState[S_OF_FailureOfB] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_FailureOfB] | Value : " << boolState[relevant_evt_OF_FailureOfB] << endl;
	cout << "Attribute :  boolState[failF_OF_FailureOfB] | Value : " << boolState[failF_OF_FailureOfB] << endl;
	cout << "Attribute :  boolState[required_OF_IO_K1] | Value : " << boolState[required_OF_IO_K1] << endl;
	cout << "Attribute :  boolState[already_S_OF_IO_K1] | Value : " << boolState[already_S_OF_IO_K1] << endl;
	cout << "Attribute :  boolState[S_OF_IO_K1] | Value : " << boolState[S_OF_IO_K1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_IO_K1] | Value : " << boolState[relevant_evt_OF_IO_K1] << endl;
	cout << "Attribute :  boolState[failF_OF_IO_K1] | Value : " << boolState[failF_OF_IO_K1] << endl;
	cout << "Attribute :  boolState[required_OF_IO_K2] | Value : " << boolState[required_OF_IO_K2] << endl;
	cout << "Attribute :  boolState[already_S_OF_IO_K2] | Value : " << boolState[already_S_OF_IO_K2] << endl;
	cout << "Attribute :  boolState[S_OF_IO_K2] | Value : " << boolState[S_OF_IO_K2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_IO_K2] | Value : " << boolState[relevant_evt_OF_IO_K2] << endl;
	cout << "Attribute :  boolState[failF_OF_IO_K2] | Value : " << boolState[failF_OF_IO_K2] << endl;
	cout << "Attribute :  boolState[required_OF_IO_K3] | Value : " << boolState[required_OF_IO_K3] << endl;
	cout << "Attribute :  boolState[already_S_OF_IO_K3] | Value : " << boolState[already_S_OF_IO_K3] << endl;
	cout << "Attribute :  boolState[S_OF_IO_K3] | Value : " << boolState[S_OF_IO_K3] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_IO_K3] | Value : " << boolState[relevant_evt_OF_IO_K3] << endl;
	cout << "Attribute :  boolState[failF_OF_IO_K3] | Value : " << boolState[failF_OF_IO_K3] << endl;
	cout << "Attribute :  boolState[required_OF_IO_K4] | Value : " << boolState[required_OF_IO_K4] << endl;
	cout << "Attribute :  boolState[already_S_OF_IO_K4] | Value : " << boolState[already_S_OF_IO_K4] << endl;
	cout << "Attribute :  boolState[S_OF_IO_K4] | Value : " << boolState[S_OF_IO_K4] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_IO_K4] | Value : " << boolState[relevant_evt_OF_IO_K4] << endl;
	cout << "Attribute :  boolState[failF_OF_IO_K4] | Value : " << boolState[failF_OF_IO_K4] << endl;
	cout << "Attribute :  boolState[required_OF_IO_K5] | Value : " << boolState[required_OF_IO_K5] << endl;
	cout << "Attribute :  boolState[already_S_OF_IO_K5] | Value : " << boolState[already_S_OF_IO_K5] << endl;
	cout << "Attribute :  boolState[S_OF_IO_K5] | Value : " << boolState[S_OF_IO_K5] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_IO_K5] | Value : " << boolState[relevant_evt_OF_IO_K5] << endl;
	cout << "Attribute :  boolState[failF_OF_IO_K5] | Value : " << boolState[failF_OF_IO_K5] << endl;
	cout << "Attribute :  boolState[required_OF_RC_K5] | Value : " << boolState[required_OF_RC_K5] << endl;
	cout << "Attribute :  boolState[already_S_OF_RC_K5] | Value : " << boolState[already_S_OF_RC_K5] << endl;
	cout << "Attribute :  boolState[S_OF_RC_K5] | Value : " << boolState[S_OF_RC_K5] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RC_K5] | Value : " << boolState[relevant_evt_OF_RC_K5] << endl;
	cout << "Attribute :  boolState[failI_OF_RC_K5] | Value : " << boolState[failI_OF_RC_K5] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RC_K5] | Value : " << boolState[to_be_fired_OF_RC_K5] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RC_K5] | Value : " << boolState[already_standby_OF_RC_K5] << endl;
	cout << "Attribute :  boolState[already_required_OF_RC_K5] | Value : " << boolState[already_required_OF_RC_K5] << endl;
	cout << "Attribute :  boolState[required_OF_RO_K1] | Value : " << boolState[required_OF_RO_K1] << endl;
	cout << "Attribute :  boolState[already_S_OF_RO_K1] | Value : " << boolState[already_S_OF_RO_K1] << endl;
	cout << "Attribute :  boolState[S_OF_RO_K1] | Value : " << boolState[S_OF_RO_K1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RO_K1] | Value : " << boolState[relevant_evt_OF_RO_K1] << endl;
	cout << "Attribute :  boolState[failI_OF_RO_K1] | Value : " << boolState[failI_OF_RO_K1] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RO_K1] | Value : " << boolState[to_be_fired_OF_RO_K1] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RO_K1] | Value : " << boolState[already_standby_OF_RO_K1] << endl;
	cout << "Attribute :  boolState[already_required_OF_RO_K1] | Value : " << boolState[already_required_OF_RO_K1] << endl;
	cout << "Attribute :  boolState[required_OF_RO_K2] | Value : " << boolState[required_OF_RO_K2] << endl;
	cout << "Attribute :  boolState[already_S_OF_RO_K2] | Value : " << boolState[already_S_OF_RO_K2] << endl;
	cout << "Attribute :  boolState[S_OF_RO_K2] | Value : " << boolState[S_OF_RO_K2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RO_K2] | Value : " << boolState[relevant_evt_OF_RO_K2] << endl;
	cout << "Attribute :  boolState[failI_OF_RO_K2] | Value : " << boolState[failI_OF_RO_K2] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RO_K2] | Value : " << boolState[to_be_fired_OF_RO_K2] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RO_K2] | Value : " << boolState[already_standby_OF_RO_K2] << endl;
	cout << "Attribute :  boolState[already_required_OF_RO_K2] | Value : " << boolState[already_required_OF_RO_K2] << endl;
	cout << "Attribute :  boolState[required_OF_RO_K3] | Value : " << boolState[required_OF_RO_K3] << endl;
	cout << "Attribute :  boolState[already_S_OF_RO_K3] | Value : " << boolState[already_S_OF_RO_K3] << endl;
	cout << "Attribute :  boolState[S_OF_RO_K3] | Value : " << boolState[S_OF_RO_K3] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RO_K3] | Value : " << boolState[relevant_evt_OF_RO_K3] << endl;
	cout << "Attribute :  boolState[failI_OF_RO_K3] | Value : " << boolState[failI_OF_RO_K3] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RO_K3] | Value : " << boolState[to_be_fired_OF_RO_K3] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RO_K3] | Value : " << boolState[already_standby_OF_RO_K3] << endl;
	cout << "Attribute :  boolState[already_required_OF_RO_K3] | Value : " << boolState[already_required_OF_RO_K3] << endl;
	cout << "Attribute :  boolState[required_OF_RO_K4] | Value : " << boolState[required_OF_RO_K4] << endl;
	cout << "Attribute :  boolState[already_S_OF_RO_K4] | Value : " << boolState[already_S_OF_RO_K4] << endl;
	cout << "Attribute :  boolState[S_OF_RO_K4] | Value : " << boolState[S_OF_RO_K4] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RO_K4] | Value : " << boolState[relevant_evt_OF_RO_K4] << endl;
	cout << "Attribute :  boolState[failI_OF_RO_K4] | Value : " << boolState[failI_OF_RO_K4] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RO_K4] | Value : " << boolState[to_be_fired_OF_RO_K4] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RO_K4] | Value : " << boolState[already_standby_OF_RO_K4] << endl;
	cout << "Attribute :  boolState[already_required_OF_RO_K4] | Value : " << boolState[already_required_OF_RO_K4] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
	cout << "Attribute :  boolState[required_OF_due_to_A] | Value : " << boolState[required_OF_due_to_A] << endl;
	cout << "Attribute :  boolState[already_S_OF_due_to_A] | Value : " << boolState[already_S_OF_due_to_A] << endl;
	cout << "Attribute :  boolState[S_OF_due_to_A] | Value : " << boolState[S_OF_due_to_A] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_due_to_A] | Value : " << boolState[relevant_evt_OF_due_to_A] << endl;
	cout << "Attribute :  boolState[required_OF_due_to_B] | Value : " << boolState[required_OF_due_to_B] << endl;
	cout << "Attribute :  boolState[already_S_OF_due_to_B] | Value : " << boolState[already_S_OF_due_to_B] << endl;
	cout << "Attribute :  boolState[S_OF_due_to_B] | Value : " << boolState[S_OF_due_to_B] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_due_to_B] | Value : " << boolState[relevant_evt_OF_due_to_B] << endl;
	cout << "Attribute :  boolState[required_OF_failure_on_phase_change] | Value : " << boolState[required_OF_failure_on_phase_change] << endl;
	cout << "Attribute :  boolState[already_S_OF_failure_on_phase_change] | Value : " << boolState[already_S_OF_failure_on_phase_change] << endl;
	cout << "Attribute :  boolState[S_OF_failure_on_phase_change] | Value : " << boolState[S_OF_failure_on_phase_change] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_failure_on_phase_change] | Value : " << boolState[relevant_evt_OF_failure_on_phase_change] << endl;
	cout << "Attribute :  boolState[required_OF_impossible_to_isolate_A] | Value : " << boolState[required_OF_impossible_to_isolate_A] << endl;
	cout << "Attribute :  boolState[already_S_OF_impossible_to_isolate_A] | Value : " << boolState[already_S_OF_impossible_to_isolate_A] << endl;
	cout << "Attribute :  boolState[S_OF_impossible_to_isolate_A] | Value : " << boolState[S_OF_impossible_to_isolate_A] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_impossible_to_isolate_A] | Value : " << boolState[relevant_evt_OF_impossible_to_isolate_A] << endl;
	cout << "Attribute :  boolState[required_OF_phase_1] | Value : " << boolState[required_OF_phase_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_phase_1] | Value : " << boolState[already_S_OF_phase_1] << endl;
	cout << "Attribute :  boolState[S_OF_phase_1] | Value : " << boolState[S_OF_phase_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_phase_1] | Value : " << boolState[relevant_evt_OF_phase_1] << endl;
	cout << "Attribute :  boolState[in_progress_OF_phase_1] | Value : " << boolState[in_progress_OF_phase_1] << endl;
	cout << "Attribute :  boolState[already_required_OF_phase_1] | Value : " << boolState[already_required_OF_phase_1] << endl;
	cout << "Attribute :  boolState[start_phase_OF_phase_1] | Value : " << boolState[start_phase_OF_phase_1] << endl;
	cout << "Attribute :  boolState[required_OF_impossible_to_isolate_B] | Value : " << boolState[required_OF_impossible_to_isolate_B] << endl;
	cout << "Attribute :  boolState[already_S_OF_impossible_to_isolate_B] | Value : " << boolState[already_S_OF_impossible_to_isolate_B] << endl;
	cout << "Attribute :  boolState[S_OF_impossible_to_isolate_B] | Value : " << boolState[S_OF_impossible_to_isolate_B] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_impossible_to_isolate_B] | Value : " << boolState[relevant_evt_OF_impossible_to_isolate_B] << endl;
	cout << "Attribute :  boolState[required_OF_phase_2] | Value : " << boolState[required_OF_phase_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_phase_2] | Value : " << boolState[already_S_OF_phase_2] << endl;
	cout << "Attribute :  boolState[S_OF_phase_2] | Value : " << boolState[S_OF_phase_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_phase_2] | Value : " << boolState[relevant_evt_OF_phase_2] << endl;
	cout << "Attribute :  boolState[in_progress_OF_phase_2] | Value : " << boolState[in_progress_OF_phase_2] << endl;
	cout << "Attribute :  boolState[already_required_OF_phase_2] | Value : " << boolState[already_required_OF_phase_2] << endl;
	cout << "Attribute :  boolState[start_phase_OF_phase_2] | Value : " << boolState[start_phase_OF_phase_2] << endl;
	cout << "Attribute :  boolState[required_OF_short_circuit] | Value : " << boolState[required_OF_short_circuit] << endl;
	cout << "Attribute :  boolState[already_S_OF_short_circuit] | Value : " << boolState[already_S_OF_short_circuit] << endl;
	cout << "Attribute :  boolState[S_OF_short_circuit] | Value : " << boolState[S_OF_short_circuit] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_short_circuit] | Value : " << boolState[relevant_evt_OF_short_circuit] << endl;
	cout << "Attribute :  boolState[required_OF_system_failure] | Value : " << boolState[required_OF_system_failure] << endl;
	cout << "Attribute :  boolState[already_S_OF_system_failure] | Value : " << boolState[already_S_OF_system_failure] << endl;
	cout << "Attribute :  boolState[S_OF_system_failure] | Value : " << boolState[S_OF_system_failure] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_system_failure] | Value : " << boolState[relevant_evt_OF_system_failure] << endl;
	cout << "Attribute :  boolState[required_OF_system_failure_in_phase_1] | Value : " << boolState[required_OF_system_failure_in_phase_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_system_failure_in_phase_1] | Value : " << boolState[already_S_OF_system_failure_in_phase_1] << endl;
	cout << "Attribute :  boolState[S_OF_system_failure_in_phase_1] | Value : " << boolState[S_OF_system_failure_in_phase_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_system_failure_in_phase_1] | Value : " << boolState[relevant_evt_OF_system_failure_in_phase_1] << endl;
	cout << "Attribute :  boolState[required_OF_system_failure_in_phase_2] | Value : " << boolState[required_OF_system_failure_in_phase_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_system_failure_in_phase_2] | Value : " << boolState[already_S_OF_system_failure_in_phase_2] << endl;
	cout << "Attribute :  boolState[S_OF_system_failure_in_phase_2] | Value : " << boolState[S_OF_system_failure_in_phase_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_system_failure_in_phase_2] | Value : " << boolState[relevant_evt_OF_system_failure_in_phase_2] << endl;
}

bool storm::figaro::FigaroProgram_figaro_BDMP::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_figaro_BDMP::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_AND_2] = REINITIALISATION_OF_required_OF_AND_2;
	boolState[S_OF_AND_2] = REINITIALISATION_OF_S_OF_AND_2;
	boolState[relevant_evt_OF_AND_2] = REINITIALISATION_OF_relevant_evt_OF_AND_2;
	boolState[required_OF_A_and_B_unavailable] = REINITIALISATION_OF_required_OF_A_and_B_unavailable;
	boolState[S_OF_A_and_B_unavailable] = REINITIALISATION_OF_S_OF_A_and_B_unavailable;
	boolState[relevant_evt_OF_A_and_B_unavailable] = REINITIALISATION_OF_relevant_evt_OF_A_and_B_unavailable;
	boolState[required_OF_A_or_B_isolated] = REINITIALISATION_OF_required_OF_A_or_B_isolated;
	boolState[S_OF_A_or_B_isolated] = REINITIALISATION_OF_S_OF_A_or_B_isolated;
	boolState[relevant_evt_OF_A_or_B_isolated] = REINITIALISATION_OF_relevant_evt_OF_A_or_B_isolated;
	boolState[required_OF_A_unavailable] = REINITIALISATION_OF_required_OF_A_unavailable;
	boolState[S_OF_A_unavailable] = REINITIALISATION_OF_S_OF_A_unavailable;
	boolState[relevant_evt_OF_A_unavailable] = REINITIALISATION_OF_relevant_evt_OF_A_unavailable;
	boolState[required_OF_B_unavailable] = REINITIALISATION_OF_required_OF_B_unavailable;
	boolState[S_OF_B_unavailable] = REINITIALISATION_OF_S_OF_B_unavailable;
	boolState[relevant_evt_OF_B_unavailable] = REINITIALISATION_OF_relevant_evt_OF_B_unavailable;
	boolState[required_OF_FailureOfA] = REINITIALISATION_OF_required_OF_FailureOfA;
	boolState[S_OF_FailureOfA] = REINITIALISATION_OF_S_OF_FailureOfA;
	boolState[relevant_evt_OF_FailureOfA] = REINITIALISATION_OF_relevant_evt_OF_FailureOfA;
	boolState[required_OF_FailureOfB] = REINITIALISATION_OF_required_OF_FailureOfB;
	boolState[S_OF_FailureOfB] = REINITIALISATION_OF_S_OF_FailureOfB;
	boolState[relevant_evt_OF_FailureOfB] = REINITIALISATION_OF_relevant_evt_OF_FailureOfB;
	boolState[required_OF_IO_K1] = REINITIALISATION_OF_required_OF_IO_K1;
	boolState[S_OF_IO_K1] = REINITIALISATION_OF_S_OF_IO_K1;
	boolState[relevant_evt_OF_IO_K1] = REINITIALISATION_OF_relevant_evt_OF_IO_K1;
	boolState[required_OF_IO_K2] = REINITIALISATION_OF_required_OF_IO_K2;
	boolState[S_OF_IO_K2] = REINITIALISATION_OF_S_OF_IO_K2;
	boolState[relevant_evt_OF_IO_K2] = REINITIALISATION_OF_relevant_evt_OF_IO_K2;
	boolState[required_OF_IO_K3] = REINITIALISATION_OF_required_OF_IO_K3;
	boolState[S_OF_IO_K3] = REINITIALISATION_OF_S_OF_IO_K3;
	boolState[relevant_evt_OF_IO_K3] = REINITIALISATION_OF_relevant_evt_OF_IO_K3;
	boolState[required_OF_IO_K4] = REINITIALISATION_OF_required_OF_IO_K4;
	boolState[S_OF_IO_K4] = REINITIALISATION_OF_S_OF_IO_K4;
	boolState[relevant_evt_OF_IO_K4] = REINITIALISATION_OF_relevant_evt_OF_IO_K4;
	boolState[required_OF_IO_K5] = REINITIALISATION_OF_required_OF_IO_K5;
	boolState[S_OF_IO_K5] = REINITIALISATION_OF_S_OF_IO_K5;
	boolState[relevant_evt_OF_IO_K5] = REINITIALISATION_OF_relevant_evt_OF_IO_K5;
	boolState[required_OF_RC_K5] = REINITIALISATION_OF_required_OF_RC_K5;
	boolState[S_OF_RC_K5] = REINITIALISATION_OF_S_OF_RC_K5;
	boolState[relevant_evt_OF_RC_K5] = REINITIALISATION_OF_relevant_evt_OF_RC_K5;
	boolState[to_be_fired_OF_RC_K5] = REINITIALISATION_OF_to_be_fired_OF_RC_K5;
	boolState[required_OF_RO_K1] = REINITIALISATION_OF_required_OF_RO_K1;
	boolState[S_OF_RO_K1] = REINITIALISATION_OF_S_OF_RO_K1;
	boolState[relevant_evt_OF_RO_K1] = REINITIALISATION_OF_relevant_evt_OF_RO_K1;
	boolState[to_be_fired_OF_RO_K1] = REINITIALISATION_OF_to_be_fired_OF_RO_K1;
	boolState[required_OF_RO_K2] = REINITIALISATION_OF_required_OF_RO_K2;
	boolState[S_OF_RO_K2] = REINITIALISATION_OF_S_OF_RO_K2;
	boolState[relevant_evt_OF_RO_K2] = REINITIALISATION_OF_relevant_evt_OF_RO_K2;
	boolState[to_be_fired_OF_RO_K2] = REINITIALISATION_OF_to_be_fired_OF_RO_K2;
	boolState[required_OF_RO_K3] = REINITIALISATION_OF_required_OF_RO_K3;
	boolState[S_OF_RO_K3] = REINITIALISATION_OF_S_OF_RO_K3;
	boolState[relevant_evt_OF_RO_K3] = REINITIALISATION_OF_relevant_evt_OF_RO_K3;
	boolState[to_be_fired_OF_RO_K3] = REINITIALISATION_OF_to_be_fired_OF_RO_K3;
	boolState[required_OF_RO_K4] = REINITIALISATION_OF_required_OF_RO_K4;
	boolState[S_OF_RO_K4] = REINITIALISATION_OF_S_OF_RO_K4;
	boolState[relevant_evt_OF_RO_K4] = REINITIALISATION_OF_relevant_evt_OF_RO_K4;
	boolState[to_be_fired_OF_RO_K4] = REINITIALISATION_OF_to_be_fired_OF_RO_K4;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
	boolState[required_OF_due_to_A] = REINITIALISATION_OF_required_OF_due_to_A;
	boolState[S_OF_due_to_A] = REINITIALISATION_OF_S_OF_due_to_A;
	boolState[relevant_evt_OF_due_to_A] = REINITIALISATION_OF_relevant_evt_OF_due_to_A;
	boolState[required_OF_due_to_B] = REINITIALISATION_OF_required_OF_due_to_B;
	boolState[S_OF_due_to_B] = REINITIALISATION_OF_S_OF_due_to_B;
	boolState[relevant_evt_OF_due_to_B] = REINITIALISATION_OF_relevant_evt_OF_due_to_B;
	boolState[required_OF_failure_on_phase_change] = REINITIALISATION_OF_required_OF_failure_on_phase_change;
	boolState[S_OF_failure_on_phase_change] = REINITIALISATION_OF_S_OF_failure_on_phase_change;
	boolState[relevant_evt_OF_failure_on_phase_change] = REINITIALISATION_OF_relevant_evt_OF_failure_on_phase_change;
	boolState[required_OF_impossible_to_isolate_A] = REINITIALISATION_OF_required_OF_impossible_to_isolate_A;
	boolState[S_OF_impossible_to_isolate_A] = REINITIALISATION_OF_S_OF_impossible_to_isolate_A;
	boolState[relevant_evt_OF_impossible_to_isolate_A] = REINITIALISATION_OF_relevant_evt_OF_impossible_to_isolate_A;
	boolState[required_OF_phase_1] = REINITIALISATION_OF_required_OF_phase_1;
	boolState[S_OF_phase_1] = REINITIALISATION_OF_S_OF_phase_1;
	boolState[relevant_evt_OF_phase_1] = REINITIALISATION_OF_relevant_evt_OF_phase_1;
	boolState[required_OF_impossible_to_isolate_B] = REINITIALISATION_OF_required_OF_impossible_to_isolate_B;
	boolState[S_OF_impossible_to_isolate_B] = REINITIALISATION_OF_S_OF_impossible_to_isolate_B;
	boolState[relevant_evt_OF_impossible_to_isolate_B] = REINITIALISATION_OF_relevant_evt_OF_impossible_to_isolate_B;
	boolState[required_OF_phase_2] = REINITIALISATION_OF_required_OF_phase_2;
	boolState[S_OF_phase_2] = REINITIALISATION_OF_S_OF_phase_2;
	boolState[relevant_evt_OF_phase_2] = REINITIALISATION_OF_relevant_evt_OF_phase_2;
	boolState[required_OF_short_circuit] = REINITIALISATION_OF_required_OF_short_circuit;
	boolState[S_OF_short_circuit] = REINITIALISATION_OF_S_OF_short_circuit;
	boolState[relevant_evt_OF_short_circuit] = REINITIALISATION_OF_relevant_evt_OF_short_circuit;
	boolState[required_OF_system_failure] = REINITIALISATION_OF_required_OF_system_failure;
	boolState[S_OF_system_failure] = REINITIALISATION_OF_S_OF_system_failure;
	boolState[relevant_evt_OF_system_failure] = REINITIALISATION_OF_relevant_evt_OF_system_failure;
	boolState[required_OF_system_failure_in_phase_1] = REINITIALISATION_OF_required_OF_system_failure_in_phase_1;
	boolState[S_OF_system_failure_in_phase_1] = REINITIALISATION_OF_S_OF_system_failure_in_phase_1;
	boolState[relevant_evt_OF_system_failure_in_phase_1] = REINITIALISATION_OF_relevant_evt_OF_system_failure_in_phase_1;
	boolState[required_OF_system_failure_in_phase_2] = REINITIALISATION_OF_required_OF_system_failure_in_phase_2;
	boolState[S_OF_system_failure_in_phase_2] = REINITIALISATION_OF_S_OF_system_failure_in_phase_2;
	boolState[relevant_evt_OF_system_failure_in_phase_2] = REINITIALISATION_OF_relevant_evt_OF_system_failure_in_phase_2;
}

void storm::figaro::FigaroProgram_figaro_BDMP::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_FailureOfA = true;
	}

	if (numFire == 1)
	{
		FIRE_xx10_OF_FailureOfB = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_IO_K1 = true;
	}

	if (numFire == 3)
	{
		FIRE_xx10_OF_IO_K2 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_IO_K3 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx10_OF_IO_K4 = true;
	}

	if (numFire == 6)
	{
		FIRE_xx10_OF_IO_K5 = true;
	}

	if (numFire == 7)
	{
		FIRE_xx23_OF_RC_K5_INS_7 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx23_OF_RC_K5_INS_8 = true;
	}

	if (numFire == 9)
	{
		FIRE_xx23_OF_RO_K1_INS_9 = true;
	}

	if (numFire == 10)
	{
		FIRE_xx23_OF_RO_K1_INS_10 = true;
	}

	if (numFire == 11)
	{
		FIRE_xx23_OF_RO_K2_INS_11 = true;
	}

	if (numFire == 12)
	{
		FIRE_xx23_OF_RO_K2_INS_12 = true;
	}

	if (numFire == 13)
	{
		FIRE_xx23_OF_RO_K3_INS_13 = true;
	}

	if (numFire == 14)
	{
		FIRE_xx23_OF_RO_K3_INS_14 = true;
	}

	if (numFire == 15)
	{
		FIRE_xx23_OF_RO_K4_INS_15 = true;
	}

	if (numFire == 16)
	{
		FIRE_xx23_OF_RO_K4_INS_16 = true;
	}

	if (numFire == 17)
	{
		FIRE_xx43_a_OF_phase_1 = true;
	}

	if (numFire == 18)
	{
		FIRE_xx47_OF_phase_1_INS_18 = true;
	}

	if (numFire == 19)
	{
		FIRE_xx43_a_OF_phase_2 = true;
	}

	if (numFire == 20)
	{
		FIRE_xx47_OF_phase_2_INS_20 = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_FailureOfA
	if ((boolState[failF_OF_FailureOfA] == false) && (boolState[required_OF_FailureOfA]
&& boolState[relevant_evt_OF_FailureOfA])) 
	{
		 
		if (FIRE_xx10_OF_FailureOfA)
		{
			boolState[failF_OF_FailureOfA]  =  true;
			FIRE_xx10_OF_FailureOfA = false;
		}
	}

	// Occurrence xx10_OF_FailureOfB
	if ((boolState[failF_OF_FailureOfB] == false) && (boolState[required_OF_FailureOfB]
&& boolState[relevant_evt_OF_FailureOfB])) 
	{
		 
		if (FIRE_xx10_OF_FailureOfB)
		{
			boolState[failF_OF_FailureOfB]  =  true;
			FIRE_xx10_OF_FailureOfB = false;
		}
	}

	// Occurrence xx10_OF_IO_K1
	if ((boolState[failF_OF_IO_K1] == false) && (boolState[required_OF_IO_K1] &&  boolState[relevant_evt_OF_IO_K1])) 
	{
		 
		if (FIRE_xx10_OF_IO_K1)
		{
			boolState[failF_OF_IO_K1]  =  true;
			FIRE_xx10_OF_IO_K1 = false;
		}
	}

	// Occurrence xx10_OF_IO_K2
	if ((boolState[failF_OF_IO_K2] == false) && (boolState[required_OF_IO_K2] &&  boolState[relevant_evt_OF_IO_K2])) 
	{
		 
		if (FIRE_xx10_OF_IO_K2)
		{
			boolState[failF_OF_IO_K2]  =  true;
			FIRE_xx10_OF_IO_K2 = false;
		}
	}

	// Occurrence xx10_OF_IO_K3
	if ((boolState[failF_OF_IO_K3] == false) && (boolState[required_OF_IO_K3] &&  boolState[relevant_evt_OF_IO_K3])) 
	{
		 
		if (FIRE_xx10_OF_IO_K3)
		{
			boolState[failF_OF_IO_K3]  =  true;
			FIRE_xx10_OF_IO_K3 = false;
		}
	}

	// Occurrence xx10_OF_IO_K4
	if ((boolState[failF_OF_IO_K4] == false) && (boolState[required_OF_IO_K4] &&  boolState[relevant_evt_OF_IO_K4])) 
	{
		 
		if (FIRE_xx10_OF_IO_K4)
		{
			boolState[failF_OF_IO_K4]  =  true;
			FIRE_xx10_OF_IO_K4 = false;
		}
	}

	// Occurrence xx10_OF_IO_K5
	if ((boolState[failF_OF_IO_K5] == false) && (boolState[required_OF_IO_K5] &&  boolState[relevant_evt_OF_IO_K5])) 
	{
		 
		if (FIRE_xx10_OF_IO_K5)
		{
			boolState[failF_OF_IO_K5]  =  true;
			FIRE_xx10_OF_IO_K5 = false;
		}
	}

	// Occurrence xx23_OF_RC_K5

	if ((boolState[failI_OF_RC_K5] == false) && ((boolState[to_be_fired_OF_RC_K5] && ((
	 !boolState[to_be_fired_OF_RO_K1]) && ( !boolState[to_be_fired_OF_RO_K4])))
	&& boolState[relevant_evt_OF_RC_K5])) 
	{
	
		
		if (FIRE_xx23_OF_RC_K5_INS_7) 
		{
			boolState[failI_OF_RC_K5]  =  true;
			boolState[already_standby_OF_RC_K5]  =  false;
			boolState[already_required_OF_RC_K5]  =  false;
			FIRE_xx23_OF_RC_K5_INS_7 = false;
		}
	
	}
	if ((boolState[failI_OF_RC_K5] == false) && ((boolState[to_be_fired_OF_RC_K5] && ((
	 !boolState[to_be_fired_OF_RO_K1]) && ( !boolState[to_be_fired_OF_RO_K4])))
	&& boolState[relevant_evt_OF_RC_K5])) 
	{
	
		
		if (FIRE_xx23_OF_RC_K5_INS_8) 
		{
			boolState[already_standby_OF_RC_K5]  =  false;
			boolState[already_required_OF_RC_K5]  =  false;
			FIRE_xx23_OF_RC_K5_INS_8 = false;
		}
	
	}
	// Occurrence xx23_OF_RO_K1

	if ((boolState[failI_OF_RO_K1] == false) && (boolState[to_be_fired_OF_RO_K1] &&
	boolState[relevant_evt_OF_RO_K1])) 
	{
	
		
		if (FIRE_xx23_OF_RO_K1_INS_9) 
		{
			boolState[failI_OF_RO_K1]  =  true;
			boolState[already_standby_OF_RO_K1]  =  false;
			boolState[already_required_OF_RO_K1]  =  false;
			FIRE_xx23_OF_RO_K1_INS_9 = false;
		}
	
	}
	if ((boolState[failI_OF_RO_K1] == false) && (boolState[to_be_fired_OF_RO_K1] &&
	boolState[relevant_evt_OF_RO_K1])) 
	{
	
		
		if (FIRE_xx23_OF_RO_K1_INS_10) 
		{
			boolState[already_standby_OF_RO_K1]  =  false;
			boolState[already_required_OF_RO_K1]  =  false;
			FIRE_xx23_OF_RO_K1_INS_10 = false;
		}
	
	}
	// Occurrence xx23_OF_RO_K2

	if ((boolState[failI_OF_RO_K2] == false) && (boolState[to_be_fired_OF_RO_K2] &&
	boolState[relevant_evt_OF_RO_K2])) 
	{
	
		
		if (FIRE_xx23_OF_RO_K2_INS_11) 
		{
			boolState[failI_OF_RO_K2]  =  true;
			boolState[already_standby_OF_RO_K2]  =  false;
			boolState[already_required_OF_RO_K2]  =  false;
			FIRE_xx23_OF_RO_K2_INS_11 = false;
		}
	
	}
	if ((boolState[failI_OF_RO_K2] == false) && (boolState[to_be_fired_OF_RO_K2] &&
	boolState[relevant_evt_OF_RO_K2])) 
	{
	
		
		if (FIRE_xx23_OF_RO_K2_INS_12) 
		{
			boolState[already_standby_OF_RO_K2]  =  false;
			boolState[already_required_OF_RO_K2]  =  false;
			FIRE_xx23_OF_RO_K2_INS_12 = false;
		}
	
	}
	// Occurrence xx23_OF_RO_K3

	if ((boolState[failI_OF_RO_K3] == false) && (boolState[to_be_fired_OF_RO_K3] &&
	boolState[relevant_evt_OF_RO_K3])) 
	{
	
		
		if (FIRE_xx23_OF_RO_K3_INS_13) 
		{
			boolState[failI_OF_RO_K3]  =  true;
			boolState[already_standby_OF_RO_K3]  =  false;
			boolState[already_required_OF_RO_K3]  =  false;
			FIRE_xx23_OF_RO_K3_INS_13 = false;
		}
	
	}
	if ((boolState[failI_OF_RO_K3] == false) && (boolState[to_be_fired_OF_RO_K3] &&
	boolState[relevant_evt_OF_RO_K3])) 
	{
	
		
		if (FIRE_xx23_OF_RO_K3_INS_14) 
		{
			boolState[already_standby_OF_RO_K3]  =  false;
			boolState[already_required_OF_RO_K3]  =  false;
			FIRE_xx23_OF_RO_K3_INS_14 = false;
		}
	
	}
	// Occurrence xx23_OF_RO_K4

	if ((boolState[failI_OF_RO_K4] == false) && (boolState[to_be_fired_OF_RO_K4] &&
	boolState[relevant_evt_OF_RO_K4])) 
	{
	
		
		if (FIRE_xx23_OF_RO_K4_INS_15) 
		{
			boolState[failI_OF_RO_K4]  =  true;
			boolState[already_standby_OF_RO_K4]  =  false;
			boolState[already_required_OF_RO_K4]  =  false;
			FIRE_xx23_OF_RO_K4_INS_15 = false;
		}
	
	}
	if ((boolState[failI_OF_RO_K4] == false) && (boolState[to_be_fired_OF_RO_K4] &&
	boolState[relevant_evt_OF_RO_K4])) 
	{
	
		
		if (FIRE_xx23_OF_RO_K4_INS_16) 
		{
			boolState[already_standby_OF_RO_K4]  =  false;
			boolState[already_required_OF_RO_K4]  =  false;
			FIRE_xx23_OF_RO_K4_INS_16 = false;
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
	
		
		if (FIRE_xx47_OF_phase_1_INS_18) 
		{
			boolState[in_progress_OF_phase_1]  =  true;
			boolState[start_phase_OF_phase_1]  =  false;
			FIRE_xx47_OF_phase_1_INS_18 = false;
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
	
		
		if (FIRE_xx47_OF_phase_2_INS_20) 
		{
			boolState[in_progress_OF_phase_2]  =  true;
			boolState[start_phase_OF_phase_2]  =  false;
			FIRE_xx47_OF_phase_2_INS_20 = false;
		}
	
	}
}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_figaro_BDMP::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if ((boolState[failI_OF_RC_K5] == false) && ((boolState[to_be_fired_OF_RC_K5] && ((	 !boolState[to_be_fired_OF_RO_K1]) && ( !boolState[to_be_fired_OF_RO_K4])))	&& boolState[relevant_evt_OF_RC_K5]))
	{
		cout << "7 :  INS_SUB_COUNT (1) |FAULT | failI  LABEL \"instantaneous failure RC_K5\" | DIST INS (0.05) | INDUCING boolState[failI_OF_RC_K5]  =  TRUE,already_standby_OF_RC_K5  =  FALSE,already_required_OF_RC_K5  =  FALSE" << endl;
		list.push_back(make_tuple(7, 0.05, "INS", 1));
	}
	if ((boolState[failI_OF_RC_K5] == false) && ((boolState[to_be_fired_OF_RC_K5] && ((	 !boolState[to_be_fired_OF_RO_K1]) && ( !boolState[to_be_fired_OF_RO_K4])))	&& boolState[relevant_evt_OF_RC_K5]))
	{
		cout << "8 :  INS_SUB_COUNT (1) |TRANSITION | good | DIST INS (0.95) | INDUCING boolState[already_standby_OF_RC_K5]  =  FALSE,already_required_OF_RC_K5  =  FALSE" << endl;
		list.push_back(make_tuple(8, 0.95, "INS", 1));
	}
	if ((boolState[failI_OF_RO_K1] == false) && (boolState[to_be_fired_OF_RO_K1] &&	boolState[relevant_evt_OF_RO_K1]))
	{
		cout << "9 :  INS_SUB_COUNT (2) |FAULT | failI  LABEL \"instantaneous failure RO_K1\" | DIST INS (0.05) | INDUCING boolState[failI_OF_RO_K1]  =  TRUE,already_standby_OF_RO_K1  =  FALSE,already_required_OF_RO_K1  =  FALSE" << endl;
		list.push_back(make_tuple(9, 0.05, "INS", 2));
	}
	if ((boolState[failI_OF_RO_K1] == false) && (boolState[to_be_fired_OF_RO_K1] &&	boolState[relevant_evt_OF_RO_K1]))
	{
		cout << "10 :  INS_SUB_COUNT (2) |TRANSITION | good | DIST INS (0.95) | INDUCING boolState[already_standby_OF_RO_K1]  =  FALSE,already_required_OF_RO_K1  =  FALSE" << endl;
		list.push_back(make_tuple(10, 0.95, "INS", 2));
	}
	if ((boolState[failI_OF_RO_K2] == false) && (boolState[to_be_fired_OF_RO_K2] &&	boolState[relevant_evt_OF_RO_K2]))
	{
		cout << "11 :  INS_SUB_COUNT (3) |FAULT | failI  LABEL \"instantaneous failure RO_K2\" | DIST INS (0.05) | INDUCING boolState[failI_OF_RO_K2]  =  TRUE,already_standby_OF_RO_K2  =  FALSE,already_required_OF_RO_K2  =  FALSE" << endl;
		list.push_back(make_tuple(11, 0.05, "INS", 3));
	}
	if ((boolState[failI_OF_RO_K2] == false) && (boolState[to_be_fired_OF_RO_K2] &&	boolState[relevant_evt_OF_RO_K2]))
	{
		cout << "12 :  INS_SUB_COUNT (3) |TRANSITION | good | DIST INS (0.95) | INDUCING boolState[already_standby_OF_RO_K2]  =  FALSE,already_required_OF_RO_K2  =  FALSE" << endl;
		list.push_back(make_tuple(12, 0.95, "INS", 3));
	}
	if ((boolState[failI_OF_RO_K3] == false) && (boolState[to_be_fired_OF_RO_K3] &&	boolState[relevant_evt_OF_RO_K3]))
	{
		cout << "13 :  INS_SUB_COUNT (4) |FAULT | failI  LABEL \"instantaneous failure RO_K3\" | DIST INS (0.05) | INDUCING boolState[failI_OF_RO_K3]  =  TRUE,already_standby_OF_RO_K3  =  FALSE,already_required_OF_RO_K3  =  FALSE" << endl;
		list.push_back(make_tuple(13, 0.05, "INS", 4));
	}
	if ((boolState[failI_OF_RO_K3] == false) && (boolState[to_be_fired_OF_RO_K3] &&	boolState[relevant_evt_OF_RO_K3]))
	{
		cout << "14 :  INS_SUB_COUNT (4) |TRANSITION | good | DIST INS (0.95) | INDUCING boolState[already_standby_OF_RO_K3]  =  FALSE,already_required_OF_RO_K3  =  FALSE" << endl;
		list.push_back(make_tuple(14, 0.95, "INS", 4));
	}
	if ((boolState[failI_OF_RO_K4] == false) && (boolState[to_be_fired_OF_RO_K4] &&	boolState[relevant_evt_OF_RO_K4]))
	{
		cout << "15 :  INS_SUB_COUNT (5) |FAULT | failI  LABEL \"instantaneous failure RO_K4\" | DIST INS (0.05) | INDUCING boolState[failI_OF_RO_K4]  =  TRUE,already_standby_OF_RO_K4  =  FALSE,already_required_OF_RO_K4  =  FALSE" << endl;
		list.push_back(make_tuple(15, 0.05, "INS", 5));
	}
	if ((boolState[failI_OF_RO_K4] == false) && (boolState[to_be_fired_OF_RO_K4] &&	boolState[relevant_evt_OF_RO_K4]))
	{
		cout << "16 :  INS_SUB_COUNT (5) |TRANSITION | good | DIST INS (0.95) | INDUCING boolState[already_standby_OF_RO_K4]  =  FALSE,already_required_OF_RO_K4  =  FALSE" << endl;
		list.push_back(make_tuple(16, 0.95, "INS", 5));
	}
	if (boolState[start_phase_OF_phase_1])
	{
		cout << "18 :  INS_SUB_COUNT (6) |TRANSITION | start  LABEL \"start of phase phase_1\" | DIST INS (1) | INDUCING boolState[in_progress_OF_phase_1]  =  TRUE,start_phase_OF_phase_1  =  FALSE" << endl;
		list.push_back(make_tuple(18, 1, "INS", 6));
	}
	if (boolState[start_phase_OF_phase_2])
	{
		cout << "20 :  INS_SUB_COUNT (7) |TRANSITION | start  LABEL \"start of phase phase_2\" | DIST INS (1) | INDUCING boolState[in_progress_OF_phase_2]  =  TRUE,start_phase_OF_phase_2  =  FALSE" << endl;
		list.push_back(make_tuple(20, 1, "INS", 7));
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
     
	if ((boolState[failF_OF_FailureOfA] == false) && (boolState[required_OF_FailureOfA] && boolState[relevant_evt_OF_FailureOfA]))
	{
		cout << "0 : xx10_OF_FailureOfA : FAULT failF  LABEL \"failure in operation FailureOfA\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_FailureOfA]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_FailureOfB] == false) && (boolState[required_OF_FailureOfB] && boolState[relevant_evt_OF_FailureOfB]))
	{
		cout << "1 : xx10_OF_FailureOfB : FAULT failF  LABEL \"failure in operation FailureOfB\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_FailureOfB]  =  TRUE" << endl;
		list.push_back(make_tuple(1, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_IO_K1] == false) && (boolState[required_OF_IO_K1] && boolState[relevant_evt_OF_IO_K1]))
	{
		cout << "2 : xx10_OF_IO_K1 : FAULT failF  LABEL \"failure in operation IO_K1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_IO_K1]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_IO_K2] == false) && (boolState[required_OF_IO_K2] && boolState[relevant_evt_OF_IO_K2]))
	{
		cout << "3 : xx10_OF_IO_K2 : FAULT failF  LABEL \"failure in operation IO_K2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_IO_K2]  =  TRUE" << endl;
		list.push_back(make_tuple(3, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_IO_K3] == false) && (boolState[required_OF_IO_K3] && boolState[relevant_evt_OF_IO_K3]))
	{
		cout << "4 : xx10_OF_IO_K3 : FAULT failF  LABEL \"failure in operation IO_K3\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_IO_K3]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_IO_K4] == false) && (boolState[required_OF_IO_K4] && boolState[relevant_evt_OF_IO_K4]))
	{
		cout << "5 : xx10_OF_IO_K4 : FAULT failF  LABEL \"failure in operation IO_K4\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_IO_K4]  =  TRUE" << endl;
		list.push_back(make_tuple(5, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_IO_K5] == false) && (boolState[required_OF_IO_K5] && boolState[relevant_evt_OF_IO_K5]))
	{
		cout << "6 : xx10_OF_IO_K5 : FAULT failF  LABEL \"failure in operation IO_K5\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_IO_K5]  =  TRUE" << endl;
		list.push_back(make_tuple(6, 0.0001, "EXP", 0));
	}
	if (boolState[in_progress_OF_phase_1])
	{
		cout << "17 : xx43_a_OF_phase_1 : TRANSITION end  LABEL \"End of phase phase_1\"  DIST EXP (0.001)  INDUCING boolState[in_progress_OF_phase_1]  =  FALSE" << endl;
		list.push_back(make_tuple(17, 0.001, "EXP", 0));
	}
	if (boolState[in_progress_OF_phase_2])
	{
		cout << "19 : xx43_a_OF_phase_2 : TRANSITION end  LABEL \"End of phase phase_2\"  DIST EXP (0.002)  INDUCING boolState[in_progress_OF_phase_2]  =  FALSE" << endl;
		list.push_back(make_tuple(19, 0.002, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_figaro_BDMP::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_FailureOfA] == true )
	{
		boolState[S_OF_FailureOfA]  =  true;
	}

	if (boolState[failF_OF_FailureOfB] == true )
	{
		boolState[S_OF_FailureOfB]  =  true;
	}

	if (boolState[failF_OF_IO_K1] == true )
	{
		boolState[S_OF_IO_K1]  =  true;
	}

	if (boolState[failF_OF_IO_K2] == true )
	{
		boolState[S_OF_IO_K2]  =  true;
	}

	if (boolState[failF_OF_IO_K3] == true )
	{
		boolState[S_OF_IO_K3]  =  true;
	}

	if (boolState[failF_OF_IO_K4] == true )
	{
		boolState[S_OF_IO_K4]  =  true;
	}

	if (boolState[failF_OF_IO_K5] == true )
	{
		boolState[S_OF_IO_K5]  =  true;
	}

	if (boolState[failI_OF_RC_K5] == true )
	{
		boolState[S_OF_RC_K5]  =  true;
	}

	if (boolState[failI_OF_RO_K1] == true )
	{
		boolState[S_OF_RO_K1]  =  true;
	}

	if (boolState[failI_OF_RO_K2] == true )
	{
		boolState[S_OF_RO_K2]  =  true;
	}

	if (boolState[failI_OF_RO_K3] == true )
	{
		boolState[S_OF_RO_K3]  =  true;
	}

	if (boolState[failI_OF_RO_K4] == true )
	{
		boolState[S_OF_RO_K4]  =  true;
	}



	boolState[S_OF_phase_1]  =  boolState[in_progress_OF_phase_1]  ;



	boolState[S_OF_phase_2]  =  boolState[in_progress_OF_phase_2]  ;

}


void storm::figaro::FigaroProgram_figaro_BDMP::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_phase_1] && boolState[S_OF_system_failure_in_phase_1] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (boolState[S_OF_phase_2] && boolState[S_OF_system_failure_in_phase_2] )
	{
		boolState[S_OF_AND_2]  =  true;
	}

	if (boolState[S_OF_A_unavailable] && boolState[S_OF_B_unavailable] )
	{
		boolState[S_OF_A_and_B_unavailable]  =  true;
	}

	if ((boolState[S_OF_IO_K2] || boolState[S_OF_IO_K3]) || boolState[S_OF_IO_K5] )
	{
		boolState[S_OF_A_or_B_isolated]  =  true;
	}

	if ((boolState[S_OF_FailureOfA] || boolState[S_OF_IO_K2]) || boolState[S_OF_IO_K4] )
	{
		boolState[S_OF_A_unavailable]  =  true;
	}

	if ((boolState[S_OF_FailureOfB] || boolState[S_OF_IO_K1]) || boolState[S_OF_IO_K3] )
	{
		boolState[S_OF_B_unavailable]  =  true;
	}

	if (boolState[S_OF_system_failure] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

	if (boolState[S_OF_FailureOfA] && boolState[S_OF_impossible_to_isolate_A] )
	{
		boolState[S_OF_due_to_A]  =  true;
	}

	if (boolState[S_OF_FailureOfB] && boolState[S_OF_impossible_to_isolate_B] )
	{
		boolState[S_OF_due_to_B]  =  true;
	}

	if ((boolState[S_OF_RC_K5] || boolState[S_OF_RO_K1]) || boolState[S_OF_RO_K4] )
	{
		boolState[S_OF_failure_on_phase_change]  =  true;
	}

	if (boolState[S_OF_RO_K2] && boolState[S_OF_RO_K4] )
	{
		boolState[S_OF_impossible_to_isolate_A]  =  true;
	}

	if (boolState[S_OF_RO_K1] && boolState[S_OF_RO_K3] )
	{
		boolState[S_OF_impossible_to_isolate_B]  =  true;
	}

	if (boolState[S_OF_due_to_A] || boolState[S_OF_due_to_B] )
	{
		boolState[S_OF_short_circuit]  =  true;
	}

	if (boolState[S_OF_AND_1] || boolState[S_OF_AND_2] )
	{
		boolState[S_OF_system_failure]  =  true;
	}

	if (boolState[S_OF_A_and_B_unavailable] || boolState[S_OF_short_circuit] )
	{
		boolState[S_OF_system_failure_in_phase_1]  =  true;
	}

	if (((boolState[S_OF_A_or_B_isolated] || boolState[S_OF_FailureOfA]) || boolState[S_OF_FailureOfB]) || boolState[S_OF_failure_on_phase_change] )
	{
		boolState[S_OF_system_failure_in_phase_2]  =  true;
	}

}


void storm::figaro::FigaroProgram_figaro_BDMP::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_system_failure] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if (boolState[relevant_evt_OF_system_failure] && ( !boolState[S_OF_system_failure]) )
	{
		boolState[relevant_evt_OF_AND_1]  =  true;
	}

	if ( !boolState[required_OF_system_failure] )
	{
		boolState[required_OF_AND_2]  =  false;
	}

	if (boolState[relevant_evt_OF_system_failure] && ( !boolState[S_OF_system_failure]) )
	{
		boolState[relevant_evt_OF_AND_2]  =  true;
	}

	if ( !boolState[required_OF_system_failure_in_phase_1] )
	{
		boolState[required_OF_A_and_B_unavailable]  =  false;
	}

	if (boolState[relevant_evt_OF_system_failure_in_phase_1] && ( !boolState[S_OF_system_failure_in_phase_1]) )
	{
		boolState[relevant_evt_OF_A_and_B_unavailable]  =  true;
	}

	if ( !boolState[required_OF_system_failure_in_phase_2] )
	{
		boolState[required_OF_A_or_B_isolated]  =  false;
	}

	if (boolState[relevant_evt_OF_system_failure_in_phase_2] && ( !boolState[S_OF_system_failure_in_phase_2]) )
	{
		boolState[relevant_evt_OF_A_or_B_isolated]  =  true;
	}

	if ( !boolState[required_OF_A_and_B_unavailable] )
	{
		boolState[required_OF_A_unavailable]  =  false;
	}

	if (boolState[relevant_evt_OF_A_and_B_unavailable] && ( !boolState[S_OF_A_and_B_unavailable]) )
	{
		boolState[relevant_evt_OF_A_unavailable]  =  true;
	}

	if ( !boolState[required_OF_A_and_B_unavailable] )
	{
		boolState[required_OF_B_unavailable]  =  false;
	}

	if (boolState[relevant_evt_OF_A_and_B_unavailable] && ( !boolState[S_OF_A_and_B_unavailable]) )
	{
		boolState[relevant_evt_OF_B_unavailable]  =  true;
	}

	if ((( !boolState[required_OF_A_unavailable]) && ( !boolState[required_OF_due_to_A])) && ( !boolState[required_OF_system_failure_in_phase_2]) )
	{
		boolState[required_OF_FailureOfA]  =  false;
	}

	if ((((boolState[relevant_evt_OF_A_unavailable] && ( !boolState[S_OF_A_unavailable])  ) || (boolState[relevant_evt_OF_due_to_A] && ( !boolState[S_OF_due_to_A]))) || (  boolState[relevant_evt_OF_system_failure_in_phase_2] && ( !boolState[S_OF_system_failure_in_phase_2]))) || (boolState[relevant_evt_OF_impossible_to_isolate_A] && ( !boolState[S_OF_impossible_to_isolate_A])) )
	{
		boolState[relevant_evt_OF_FailureOfA]  =  true;
	}

	if ((( !boolState[required_OF_B_unavailable]) && ( !boolState[required_OF_due_to_B])) && ( !boolState[required_OF_system_failure_in_phase_2]) )
	{
		boolState[required_OF_FailureOfB]  =  false;
	}

	if ((((boolState[relevant_evt_OF_B_unavailable] && ( !boolState[S_OF_B_unavailable])  ) || (boolState[relevant_evt_OF_due_to_B] && ( !boolState[S_OF_due_to_B]))) || (  boolState[relevant_evt_OF_system_failure_in_phase_2] && ( !boolState[S_OF_system_failure_in_phase_2]))) || (boolState[relevant_evt_OF_impossible_to_isolate_B] && ( !boolState[S_OF_impossible_to_isolate_B])) )
	{
		boolState[relevant_evt_OF_FailureOfB]  =  true;
	}

	if ( !boolState[required_OF_B_unavailable] )
	{
		boolState[required_OF_IO_K1]  =  false;
	}

	if (boolState[relevant_evt_OF_B_unavailable] && ( !boolState[S_OF_B_unavailable]) )
	{
		boolState[relevant_evt_OF_IO_K1]  =  true;
	}

	if (( !boolState[required_OF_A_or_B_isolated]) && ( !boolState[required_OF_A_unavailable]) )
	{
		boolState[required_OF_IO_K2]  =  false;
	}

	if ((boolState[relevant_evt_OF_A_or_B_isolated] && ( !boolState[S_OF_A_or_B_isolated])) || (boolState[relevant_evt_OF_A_unavailable] && ( !  boolState[S_OF_A_unavailable])) )
	{
		boolState[relevant_evt_OF_IO_K2]  =  true;
	}

	if (( !boolState[required_OF_A_or_B_isolated]) && ( !boolState[required_OF_B_unavailable]) )
	{
		boolState[required_OF_IO_K3]  =  false;
	}

	if ((boolState[relevant_evt_OF_A_or_B_isolated] && ( !boolState[S_OF_A_or_B_isolated])) || (boolState[relevant_evt_OF_B_unavailable] && ( !  boolState[S_OF_B_unavailable])) )
	{
		boolState[relevant_evt_OF_IO_K3]  =  true;
	}

	if ( !boolState[required_OF_A_unavailable] )
	{
		boolState[required_OF_IO_K4]  =  false;
	}

	if (boolState[relevant_evt_OF_A_unavailable] && ( !boolState[S_OF_A_unavailable]) )
	{
		boolState[relevant_evt_OF_IO_K4]  =  true;
	}

	if ( !boolState[required_OF_A_or_B_isolated] )
	{
		boolState[required_OF_IO_K5]  =  false;
	}

	if (boolState[relevant_evt_OF_A_or_B_isolated] && ( !boolState[S_OF_A_or_B_isolated]) )
	{
		boolState[relevant_evt_OF_IO_K5]  =  true;
	}

	if ( !boolState[required_OF_failure_on_phase_change] )
	{
		boolState[required_OF_RC_K5]  =  false;
	}

	if (boolState[relevant_evt_OF_failure_on_phase_change] && ( !boolState[S_OF_failure_on_phase_change]) )
	{
		boolState[relevant_evt_OF_RC_K5]  =  true;
	}

	if (( !boolState[required_OF_failure_on_phase_change]) && ( !boolState[required_OF_impossible_to_isolate_B]) )
	{
		boolState[required_OF_RO_K1]  =  false;
	}

	if ((boolState[relevant_evt_OF_failure_on_phase_change] && ( !boolState[S_OF_failure_on_phase_change])) || (boolState[relevant_evt_OF_impossible_to_isolate_B] && ( !boolState[S_OF_impossible_to_isolate_B])) )
	{
		boolState[relevant_evt_OF_RO_K1]  =  true;
	}

	if ( !boolState[required_OF_impossible_to_isolate_A] )
	{
		boolState[required_OF_RO_K2]  =  false;
	}

	if (boolState[relevant_evt_OF_impossible_to_isolate_A] && ( !boolState[S_OF_impossible_to_isolate_A]) )
	{
		boolState[relevant_evt_OF_RO_K2]  =  true;
	}

	if ( !boolState[required_OF_impossible_to_isolate_B] )
	{
		boolState[required_OF_RO_K3]  =  false;
	}

	if (boolState[relevant_evt_OF_impossible_to_isolate_B] && ( !boolState[S_OF_impossible_to_isolate_B]) )
	{
		boolState[relevant_evt_OF_RO_K3]  =  true;
	}

	if (( !boolState[required_OF_failure_on_phase_change]) && ( !boolState[required_OF_impossible_to_isolate_A]) )
	{
		boolState[required_OF_RO_K4]  =  false;
	}

	if ((boolState[relevant_evt_OF_failure_on_phase_change] && ( !boolState[S_OF_failure_on_phase_change])) || (boolState[relevant_evt_OF_impossible_to_isolate_A] && ( !boolState[S_OF_impossible_to_isolate_A])) )
	{
		boolState[relevant_evt_OF_RO_K4]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

	if ( !boolState[required_OF_short_circuit] )
	{
		boolState[required_OF_due_to_A]  =  false;
	}

	if (boolState[relevant_evt_OF_short_circuit] && ( !boolState[S_OF_short_circuit]) )
	{
		boolState[relevant_evt_OF_due_to_A]  =  true;
	}

	if ( !boolState[required_OF_short_circuit] )
	{
		boolState[required_OF_due_to_B]  =  false;
	}

	if (boolState[relevant_evt_OF_short_circuit] && ( !boolState[S_OF_short_circuit]) )
	{
		boolState[relevant_evt_OF_due_to_B]  =  true;
	}

	if ( !boolState[required_OF_system_failure_in_phase_2] )
	{
		boolState[required_OF_failure_on_phase_change]  =  false;
	}

	if (boolState[relevant_evt_OF_system_failure_in_phase_2] && ( !boolState[S_OF_system_failure_in_phase_2]) )
	{
		boolState[relevant_evt_OF_failure_on_phase_change]  =  true;
	}

	if (( !boolState[required_OF_due_to_A]) || ( !boolState[S_OF_FailureOfA]) )
	{
		boolState[required_OF_impossible_to_isolate_A]  =  false;
	}

	if (boolState[relevant_evt_OF_due_to_A] && ( !boolState[S_OF_due_to_A]) )
	{
		boolState[relevant_evt_OF_impossible_to_isolate_A]  =  true;
	}

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_phase_1]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1])) || ((  boolState[relevant_evt_OF_phase_2] && ( !boolState[S_OF_phase_2])) || (  boolState[relevant_evt_OF_system_failure_in_phase_1] && ( !boolState[S_OF_system_failure_in_phase_1]))) )
	{
		boolState[relevant_evt_OF_phase_1]  =  true;
	}

	if (( !boolState[required_OF_due_to_B]) || ( !boolState[S_OF_FailureOfB]) )
	{
		boolState[required_OF_impossible_to_isolate_B]  =  false;
	}

	if (boolState[relevant_evt_OF_due_to_B] && ( !boolState[S_OF_due_to_B]) )
	{
		boolState[relevant_evt_OF_impossible_to_isolate_B]  =  true;
	}

	if (( !boolState[required_OF_AND_2]) || ( !boolState[S_OF_phase_1]) )
	{
		boolState[required_OF_phase_2]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_2] && ( !boolState[S_OF_AND_2])) || (  boolState[relevant_evt_OF_system_failure_in_phase_2] && ( !boolState[S_OF_system_failure_in_phase_2])) )
	{
		boolState[relevant_evt_OF_phase_2]  =  true;
	}

	if ( !boolState[required_OF_system_failure_in_phase_1] )
	{
		boolState[required_OF_short_circuit]  =  false;
	}

	if (boolState[relevant_evt_OF_system_failure_in_phase_1] && ( !boolState[S_OF_system_failure_in_phase_1]) )
	{
		boolState[relevant_evt_OF_short_circuit]  =  true;
	}

	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_system_failure]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_system_failure]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_phase_1]) )
	{
		boolState[required_OF_system_failure_in_phase_1]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_system_failure_in_phase_1]  =  true;
	}

	if (( !boolState[required_OF_AND_2]) || ( !boolState[S_OF_phase_2]) )
	{
		boolState[required_OF_system_failure_in_phase_2]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_2] && ( !boolState[S_OF_AND_2]) )
	{
		boolState[relevant_evt_OF_system_failure_in_phase_2]  =  true;
	}

}


void storm::figaro::FigaroProgram_figaro_BDMP::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_AND_2]  =  boolState[S_OF_AND_2]  ;



	boolState[already_S_OF_A_and_B_unavailable]  =  boolState[S_OF_A_and_B_unavailable]  ;



	boolState[already_S_OF_A_or_B_isolated]  =  boolState[S_OF_A_or_B_isolated]  ;



	boolState[already_S_OF_A_unavailable]  =  boolState[S_OF_A_unavailable]  ;



	boolState[already_S_OF_B_unavailable]  =  boolState[S_OF_B_unavailable]  ;



	boolState[already_S_OF_FailureOfA]  =  boolState[S_OF_FailureOfA]  ;



	boolState[already_S_OF_FailureOfB]  =  boolState[S_OF_FailureOfB]  ;



	boolState[already_S_OF_IO_K1]  =  boolState[S_OF_IO_K1]  ;



	boolState[already_S_OF_IO_K2]  =  boolState[S_OF_IO_K2]  ;



	boolState[already_S_OF_IO_K3]  =  boolState[S_OF_IO_K3]  ;



	boolState[already_S_OF_IO_K4]  =  boolState[S_OF_IO_K4]  ;



	boolState[already_S_OF_IO_K5]  =  boolState[S_OF_IO_K5]  ;



	boolState[already_S_OF_RC_K5]  =  boolState[S_OF_RC_K5]  ;

	if (( !boolState[required_OF_RC_K5]) && (( !boolState[already_standby_OF_RC_K5]) && ( !boolState[already_required_OF_RC_K5])) )
	{
		boolState[already_standby_OF_RC_K5]  =  true;
	}



	boolState[already_S_OF_RO_K1]  =  boolState[S_OF_RO_K1]  ;

	if (( !boolState[required_OF_RO_K1]) && (( !boolState[already_standby_OF_RO_K1]) && ( !boolState[already_required_OF_RO_K1])) )
	{
		boolState[already_standby_OF_RO_K1]  =  true;
	}



	boolState[already_S_OF_RO_K2]  =  boolState[S_OF_RO_K2]  ;

	if (( !boolState[required_OF_RO_K2]) && (( !boolState[already_standby_OF_RO_K2]) && ( !boolState[already_required_OF_RO_K2])) )
	{
		boolState[already_standby_OF_RO_K2]  =  true;
	}



	boolState[already_S_OF_RO_K3]  =  boolState[S_OF_RO_K3]  ;

	if (( !boolState[required_OF_RO_K3]) && (( !boolState[already_standby_OF_RO_K3]) && ( !boolState[already_required_OF_RO_K3])) )
	{
		boolState[already_standby_OF_RO_K3]  =  true;
	}



	boolState[already_S_OF_RO_K4]  =  boolState[S_OF_RO_K4]  ;

	if (( !boolState[required_OF_RO_K4]) && (( !boolState[already_standby_OF_RO_K4]) && ( !boolState[already_required_OF_RO_K4])) )
	{
		boolState[already_standby_OF_RO_K4]  =  true;
	}



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;



	boolState[already_S_OF_due_to_A]  =  boolState[S_OF_due_to_A]  ;



	boolState[already_S_OF_due_to_B]  =  boolState[S_OF_due_to_B]  ;



	boolState[already_S_OF_failure_on_phase_change]  =  boolState[S_OF_failure_on_phase_change]  ;



	boolState[already_S_OF_impossible_to_isolate_A]  =  boolState[S_OF_impossible_to_isolate_A]  ;



	boolState[already_S_OF_phase_1]  =  boolState[S_OF_phase_1]  ;

	if ((( !boolState[in_progress_OF_phase_1]) && ( !boolState[required_OF_phase_1])) && boolState[already_required_OF_phase_1] )
	{
		boolState[start_phase_OF_phase_1]  =  true;
	}



	boolState[already_S_OF_impossible_to_isolate_B]  =  boolState[S_OF_impossible_to_isolate_B]  ;



	boolState[already_S_OF_phase_2]  =  boolState[S_OF_phase_2]  ;

	if ((( !boolState[in_progress_OF_phase_2]) && ( !boolState[required_OF_phase_2])) && boolState[already_required_OF_phase_2] )
	{
		boolState[start_phase_OF_phase_2]  =  true;
	}



	boolState[already_S_OF_short_circuit]  =  boolState[S_OF_short_circuit]  ;



	boolState[already_S_OF_system_failure]  =  boolState[S_OF_system_failure]  ;



	boolState[already_S_OF_system_failure_in_phase_1]  =  boolState[S_OF_system_failure_in_phase_1]  ;



	boolState[already_S_OF_system_failure_in_phase_2]  =  boolState[S_OF_system_failure_in_phase_2]  ;

}


void storm::figaro::FigaroProgram_figaro_BDMP::runOnceInteractionStep_tops()
{
	if (boolState[required_OF_RC_K5] && boolState[already_standby_OF_RC_K5] )
	{
		boolState[to_be_fired_OF_RC_K5]  =  true;
	}

	if (boolState[required_OF_RO_K1] && boolState[already_standby_OF_RO_K1] )
	{
		boolState[to_be_fired_OF_RO_K1]  =  true;
	}

	if (boolState[required_OF_RO_K2] && boolState[already_standby_OF_RO_K2] )
	{
		boolState[to_be_fired_OF_RO_K2]  =  true;
	}

	if (boolState[required_OF_RO_K3] && boolState[already_standby_OF_RO_K3] )
	{
		boolState[to_be_fired_OF_RO_K3]  =  true;
	}

	if (boolState[required_OF_RO_K4] && boolState[already_standby_OF_RO_K4] )
	{
		boolState[to_be_fired_OF_RO_K4]  =  true;
	}



	boolState[already_required_OF_phase_1]  =  boolState[required_OF_phase_1]  ;



	boolState[already_required_OF_phase_2]  =  boolState[required_OF_phase_2]  ;

}

void
storm::figaro::FigaroProgram_figaro_BDMP::runInteractions() {
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
    }void storm::figaro::FigaroProgram_figaro_BDMP::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_figaro_BDMP::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_figaro_BDMP::fireinsttransitiongroup(std::string user_input_ins)

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
    