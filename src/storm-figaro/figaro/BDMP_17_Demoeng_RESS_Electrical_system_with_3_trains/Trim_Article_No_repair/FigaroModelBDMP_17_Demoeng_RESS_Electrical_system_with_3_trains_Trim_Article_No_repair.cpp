#include <iostream>
#include "FigaroModelBDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair.h"

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
        
void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::init()
{
	cout <<">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	REINITIALISATION_OF_required_OF_AND_1 = true;
	boolState[already_S_OF_AND_1] = false;
	REINITIALISATION_OF_S_OF_AND_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_AND_1 = false;
	REINITIALISATION_OF_required_OF_CB_dies = true;
	boolState[already_S_OF_CB_dies] = false;
	REINITIALISATION_OF_S_OF_CB_dies = false;
	REINITIALISATION_OF_relevant_evt_OF_CB_dies = false;
	boolState[failF_OF_CB_dies] = false;
	REINITIALISATION_OF_required_OF_CB_dw_1 = true;
	boolState[already_S_OF_CB_dw_1] = false;
	REINITIALISATION_OF_S_OF_CB_dw_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_CB_dw_1 = false;
	boolState[failF_OF_CB_dw_1] = false;
	REINITIALISATION_OF_required_OF_CB_dw_2 = true;
	boolState[already_S_OF_CB_dw_2] = false;
	REINITIALISATION_OF_S_OF_CB_dw_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_CB_dw_2 = false;
	boolState[failF_OF_CB_dw_2] = false;
	REINITIALISATION_OF_required_OF_CB_up_1 = true;
	boolState[already_S_OF_CB_up_1] = false;
	REINITIALISATION_OF_S_OF_CB_up_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_CB_up_1 = false;
	boolState[failF_OF_CB_up_1] = false;
	REINITIALISATION_OF_required_OF_CB_up_2 = true;
	boolState[already_S_OF_CB_up_2] = false;
	REINITIALISATION_OF_S_OF_CB_up_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_CB_up_2 = false;
	boolState[failF_OF_CB_up_2] = false;
	REINITIALISATION_OF_required_OF_GRID = true;
	boolState[already_S_OF_GRID] = false;
	REINITIALISATION_OF_S_OF_GRID = false;
	REINITIALISATION_OF_relevant_evt_OF_GRID = false;
	boolState[failF_OF_GRID] = false;
	REINITIALISATION_OF_required_OF_LossOfAllBackups = true;
	boolState[already_S_OF_LossOfAllBackups] = false;
	REINITIALISATION_OF_S_OF_LossOfAllBackups = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfAllBackups = false;
	REINITIALISATION_OF_required_OF_LossOfDieselLine = true;
	boolState[already_S_OF_LossOfDieselLine] = false;
	REINITIALISATION_OF_S_OF_LossOfDieselLine = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfDieselLine = false;
	REINITIALISATION_OF_required_OF_LossOfLine2 = true;
	boolState[already_S_OF_LossOfLine2] = false;
	REINITIALISATION_OF_S_OF_LossOfLine2 = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfLine2 = false;
	REINITIALISATION_OF_required_OF_LossOfLine_1 = true;
	boolState[already_S_OF_LossOfLine_1] = false;
	REINITIALISATION_OF_S_OF_LossOfLine_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_LossOfLine_1 = false;
	REINITIALISATION_OF_required_OF_Loss_of_GRID = true;
	boolState[already_S_OF_Loss_of_GRID] = false;
	REINITIALISATION_OF_S_OF_Loss_of_GRID = false;
	REINITIALISATION_OF_relevant_evt_OF_Loss_of_GRID = false;
	REINITIALISATION_OF_required_OF_PropagationShortCircuitTransfo1 = true;
	boolState[already_S_OF_PropagationShortCircuitTransfo1] = false;
	REINITIALISATION_OF_S_OF_PropagationShortCircuitTransfo1 = false;
	REINITIALISATION_OF_relevant_evt_OF_PropagationShortCircuitTransfo1 = false;
	REINITIALISATION_OF_required_OF_PropagationShortCircuitTransfo2 = true;
	boolState[already_S_OF_PropagationShortCircuitTransfo2] = false;
	REINITIALISATION_OF_S_OF_PropagationShortCircuitTransfo2 = false;
	REINITIALISATION_OF_relevant_evt_OF_PropagationShortCircuitTransfo2 = false;
	REINITIALISATION_OF_required_OF_RC_CB_dies = true;
	boolState[already_S_OF_RC_CB_dies] = false;
	REINITIALISATION_OF_S_OF_RC_CB_dies = false;
	REINITIALISATION_OF_relevant_evt_OF_RC_CB_dies = false;
	boolState[failI_OF_RC_CB_dies] = false;
	REINITIALISATION_OF_to_be_fired_OF_RC_CB_dies = false;
	boolState[already_standby_OF_RC_CB_dies] = false;
	boolState[already_required_OF_RC_CB_dies] = false;
	REINITIALISATION_OF_required_OF_RC_CB_dw_2 = true;
	boolState[already_S_OF_RC_CB_dw_2] = false;
	REINITIALISATION_OF_S_OF_RC_CB_dw_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_RC_CB_dw_2 = false;
	boolState[failI_OF_RC_CB_dw_2] = false;
	REINITIALISATION_OF_to_be_fired_OF_RC_CB_dw_2 = false;
	boolState[already_standby_OF_RC_CB_dw_2] = false;
	boolState[already_required_OF_RC_CB_dw_2] = false;
	REINITIALISATION_OF_required_OF_RC_CB_up_2 = true;
	boolState[already_S_OF_RC_CB_up_2] = false;
	REINITIALISATION_OF_S_OF_RC_CB_up_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_RC_CB_up_2 = false;
	boolState[failI_OF_RC_CB_up_2] = false;
	REINITIALISATION_OF_to_be_fired_OF_RC_CB_up_2 = false;
	boolState[already_standby_OF_RC_CB_up_2] = false;
	boolState[already_required_OF_RC_CB_up_2] = false;
	REINITIALISATION_OF_required_OF_RO_CB_up_1 = true;
	boolState[already_S_OF_RO_CB_up_1] = false;
	REINITIALISATION_OF_S_OF_RO_CB_up_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_RO_CB_up_1 = false;
	boolState[failI_OF_RO_CB_up_1] = false;
	REINITIALISATION_OF_to_be_fired_OF_RO_CB_up_1 = false;
	boolState[already_standby_OF_RO_CB_up_1] = false;
	boolState[already_required_OF_RO_CB_up_1] = false;
	REINITIALISATION_OF_required_OF_RO_CB_up_2 = true;
	boolState[already_S_OF_RO_CB_up_2] = false;
	REINITIALISATION_OF_S_OF_RO_CB_up_2 = false;
	REINITIALISATION_OF_relevant_evt_OF_RO_CB_up_2 = false;
	boolState[failI_OF_RO_CB_up_2] = false;
	REINITIALISATION_OF_to_be_fired_OF_RO_CB_up_2 = false;
	boolState[already_standby_OF_RO_CB_up_2] = false;
	boolState[already_required_OF_RO_CB_up_2] = false;
	REINITIALISATION_OF_required_OF_RS_dies = true;
	boolState[already_S_OF_RS_dies] = false;
	REINITIALISATION_OF_S_OF_RS_dies = false;
	REINITIALISATION_OF_relevant_evt_OF_RS_dies = false;
	boolState[failI_OF_RS_dies] = false;
	REINITIALISATION_OF_to_be_fired_OF_RS_dies = false;
	boolState[already_standby_OF_RS_dies] = false;
	boolState[already_required_OF_RS_dies] = false;
	REINITIALISATION_OF_required_OF_Transfo1 = true;
	boolState[already_S_OF_Transfo1] = false;
	REINITIALISATION_OF_S_OF_Transfo1 = false;
	REINITIALISATION_OF_relevant_evt_OF_Transfo1 = false;
	boolState[failF_OF_Transfo1] = false;
	REINITIALISATION_OF_required_OF_Transfo2 = true;
	boolState[already_S_OF_Transfo2] = false;
	REINITIALISATION_OF_S_OF_Transfo2 = false;
	REINITIALISATION_OF_relevant_evt_OF_Transfo2 = false;
	boolState[failF_OF_Transfo2] = false;
	REINITIALISATION_OF_required_OF_UE_1 = true;
	boolState[already_S_OF_UE_1] = false;
	REINITIALISATION_OF_S_OF_UE_1 = false;
	REINITIALISATION_OF_relevant_evt_OF_UE_1 = false;
	REINITIALISATION_OF_required_OF_dies_generator = true;
	boolState[already_S_OF_dies_generator] = false;
	REINITIALISATION_OF_S_OF_dies_generator = false;
	REINITIALISATION_OF_relevant_evt_OF_dies_generator = false;
	boolState[failF_OF_dies_generator] = false;

	/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
	FIRE_xx10_OF_CB_dies = false;
	FIRE_xx10_OF_CB_dw_1 = false;
	FIRE_xx10_OF_CB_dw_2 = false;
	FIRE_xx10_OF_CB_up_1 = false;
	FIRE_xx10_OF_CB_up_2 = false;
	FIRE_xx10_OF_GRID = false;
	FIRE_xx23_OF_RC_CB_dies_INS_6 = false;
	FIRE_xx23_OF_RC_CB_dies_INS_7 = false;
	FIRE_xx23_OF_RC_CB_dw_2_INS_8 = false;
	FIRE_xx23_OF_RC_CB_dw_2_INS_9 = false;
	FIRE_xx23_OF_RC_CB_up_2_INS_10 = false;
	FIRE_xx23_OF_RC_CB_up_2_INS_11 = false;
	FIRE_xx23_OF_RO_CB_up_1_INS_12 = false;
	FIRE_xx23_OF_RO_CB_up_1_INS_13 = false;
	FIRE_xx23_OF_RO_CB_up_2_INS_14 = false;
	FIRE_xx23_OF_RO_CB_up_2_INS_15 = false;
	FIRE_xx23_OF_RS_dies_INS_16 = false;
	FIRE_xx23_OF_RS_dies_INS_17 = false;
	FIRE_xx10_OF_Transfo1 = false;
	FIRE_xx10_OF_Transfo2 = false;
	FIRE_xx10_OF_dies_generator = false;

}

void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::saveCurrentState()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<" << endl;
	backupBoolState = boolState ;
	backupFloatState = floatState ;
	backupIntState = intState ;
	backupEnumState = enumState ;
}

int storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::compareStates()
{
	// cout <<">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); 
}

void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::printState()
{
	cout <<"\n==================== Print of the current state :  ====================" << endl;

	cout << "Attribute :  boolState[required_OF_AND_1] | Value : " << boolState[required_OF_AND_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_AND_1] | Value : " << boolState[already_S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[S_OF_AND_1] | Value : " << boolState[S_OF_AND_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_AND_1] | Value : " << boolState[relevant_evt_OF_AND_1] << endl;
	cout << "Attribute :  boolState[required_OF_CB_dies] | Value : " << boolState[required_OF_CB_dies] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB_dies] | Value : " << boolState[already_S_OF_CB_dies] << endl;
	cout << "Attribute :  boolState[S_OF_CB_dies] | Value : " << boolState[S_OF_CB_dies] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB_dies] | Value : " << boolState[relevant_evt_OF_CB_dies] << endl;
	cout << "Attribute :  boolState[failF_OF_CB_dies] | Value : " << boolState[failF_OF_CB_dies] << endl;
	cout << "Attribute :  boolState[required_OF_CB_dw_1] | Value : " << boolState[required_OF_CB_dw_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB_dw_1] | Value : " << boolState[already_S_OF_CB_dw_1] << endl;
	cout << "Attribute :  boolState[S_OF_CB_dw_1] | Value : " << boolState[S_OF_CB_dw_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB_dw_1] | Value : " << boolState[relevant_evt_OF_CB_dw_1] << endl;
	cout << "Attribute :  boolState[failF_OF_CB_dw_1] | Value : " << boolState[failF_OF_CB_dw_1] << endl;
	cout << "Attribute :  boolState[required_OF_CB_dw_2] | Value : " << boolState[required_OF_CB_dw_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB_dw_2] | Value : " << boolState[already_S_OF_CB_dw_2] << endl;
	cout << "Attribute :  boolState[S_OF_CB_dw_2] | Value : " << boolState[S_OF_CB_dw_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB_dw_2] | Value : " << boolState[relevant_evt_OF_CB_dw_2] << endl;
	cout << "Attribute :  boolState[failF_OF_CB_dw_2] | Value : " << boolState[failF_OF_CB_dw_2] << endl;
	cout << "Attribute :  boolState[required_OF_CB_up_1] | Value : " << boolState[required_OF_CB_up_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB_up_1] | Value : " << boolState[already_S_OF_CB_up_1] << endl;
	cout << "Attribute :  boolState[S_OF_CB_up_1] | Value : " << boolState[S_OF_CB_up_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB_up_1] | Value : " << boolState[relevant_evt_OF_CB_up_1] << endl;
	cout << "Attribute :  boolState[failF_OF_CB_up_1] | Value : " << boolState[failF_OF_CB_up_1] << endl;
	cout << "Attribute :  boolState[required_OF_CB_up_2] | Value : " << boolState[required_OF_CB_up_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_CB_up_2] | Value : " << boolState[already_S_OF_CB_up_2] << endl;
	cout << "Attribute :  boolState[S_OF_CB_up_2] | Value : " << boolState[S_OF_CB_up_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_CB_up_2] | Value : " << boolState[relevant_evt_OF_CB_up_2] << endl;
	cout << "Attribute :  boolState[failF_OF_CB_up_2] | Value : " << boolState[failF_OF_CB_up_2] << endl;
	cout << "Attribute :  boolState[required_OF_GRID] | Value : " << boolState[required_OF_GRID] << endl;
	cout << "Attribute :  boolState[already_S_OF_GRID] | Value : " << boolState[already_S_OF_GRID] << endl;
	cout << "Attribute :  boolState[S_OF_GRID] | Value : " << boolState[S_OF_GRID] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_GRID] | Value : " << boolState[relevant_evt_OF_GRID] << endl;
	cout << "Attribute :  boolState[failF_OF_GRID] | Value : " << boolState[failF_OF_GRID] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfAllBackups] | Value : " << boolState[required_OF_LossOfAllBackups] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfAllBackups] | Value : " << boolState[already_S_OF_LossOfAllBackups] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfAllBackups] | Value : " << boolState[S_OF_LossOfAllBackups] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfAllBackups] | Value : " << boolState[relevant_evt_OF_LossOfAllBackups] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfDieselLine] | Value : " << boolState[required_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfDieselLine] | Value : " << boolState[already_S_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfDieselLine] | Value : " << boolState[S_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfDieselLine] | Value : " << boolState[relevant_evt_OF_LossOfDieselLine] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfLine2] | Value : " << boolState[required_OF_LossOfLine2] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfLine2] | Value : " << boolState[already_S_OF_LossOfLine2] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfLine2] | Value : " << boolState[S_OF_LossOfLine2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfLine2] | Value : " << boolState[relevant_evt_OF_LossOfLine2] << endl;
	cout << "Attribute :  boolState[required_OF_LossOfLine_1] | Value : " << boolState[required_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_LossOfLine_1] | Value : " << boolState[already_S_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[S_OF_LossOfLine_1] | Value : " << boolState[S_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_LossOfLine_1] | Value : " << boolState[relevant_evt_OF_LossOfLine_1] << endl;
	cout << "Attribute :  boolState[required_OF_Loss_of_GRID] | Value : " << boolState[required_OF_Loss_of_GRID] << endl;
	cout << "Attribute :  boolState[already_S_OF_Loss_of_GRID] | Value : " << boolState[already_S_OF_Loss_of_GRID] << endl;
	cout << "Attribute :  boolState[S_OF_Loss_of_GRID] | Value : " << boolState[S_OF_Loss_of_GRID] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Loss_of_GRID] | Value : " << boolState[relevant_evt_OF_Loss_of_GRID] << endl;
	cout << "Attribute :  boolState[required_OF_PropagationShortCircuitTransfo1] | Value : " << boolState[required_OF_PropagationShortCircuitTransfo1] << endl;
	cout << "Attribute :  boolState[already_S_OF_PropagationShortCircuitTransfo1] | Value : " << boolState[already_S_OF_PropagationShortCircuitTransfo1] << endl;
	cout << "Attribute :  boolState[S_OF_PropagationShortCircuitTransfo1] | Value : " << boolState[S_OF_PropagationShortCircuitTransfo1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_PropagationShortCircuitTransfo1] | Value : " << boolState[relevant_evt_OF_PropagationShortCircuitTransfo1] << endl;
	cout << "Attribute :  boolState[required_OF_PropagationShortCircuitTransfo2] | Value : " << boolState[required_OF_PropagationShortCircuitTransfo2] << endl;
	cout << "Attribute :  boolState[already_S_OF_PropagationShortCircuitTransfo2] | Value : " << boolState[already_S_OF_PropagationShortCircuitTransfo2] << endl;
	cout << "Attribute :  boolState[S_OF_PropagationShortCircuitTransfo2] | Value : " << boolState[S_OF_PropagationShortCircuitTransfo2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_PropagationShortCircuitTransfo2] | Value : " << boolState[relevant_evt_OF_PropagationShortCircuitTransfo2] << endl;
	cout << "Attribute :  boolState[required_OF_RC_CB_dies] | Value : " << boolState[required_OF_RC_CB_dies] << endl;
	cout << "Attribute :  boolState[already_S_OF_RC_CB_dies] | Value : " << boolState[already_S_OF_RC_CB_dies] << endl;
	cout << "Attribute :  boolState[S_OF_RC_CB_dies] | Value : " << boolState[S_OF_RC_CB_dies] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RC_CB_dies] | Value : " << boolState[relevant_evt_OF_RC_CB_dies] << endl;
	cout << "Attribute :  boolState[failI_OF_RC_CB_dies] | Value : " << boolState[failI_OF_RC_CB_dies] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RC_CB_dies] | Value : " << boolState[to_be_fired_OF_RC_CB_dies] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RC_CB_dies] | Value : " << boolState[already_standby_OF_RC_CB_dies] << endl;
	cout << "Attribute :  boolState[already_required_OF_RC_CB_dies] | Value : " << boolState[already_required_OF_RC_CB_dies] << endl;
	cout << "Attribute :  boolState[required_OF_RC_CB_dw_2] | Value : " << boolState[required_OF_RC_CB_dw_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_RC_CB_dw_2] | Value : " << boolState[already_S_OF_RC_CB_dw_2] << endl;
	cout << "Attribute :  boolState[S_OF_RC_CB_dw_2] | Value : " << boolState[S_OF_RC_CB_dw_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RC_CB_dw_2] | Value : " << boolState[relevant_evt_OF_RC_CB_dw_2] << endl;
	cout << "Attribute :  boolState[failI_OF_RC_CB_dw_2] | Value : " << boolState[failI_OF_RC_CB_dw_2] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RC_CB_dw_2] | Value : " << boolState[to_be_fired_OF_RC_CB_dw_2] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RC_CB_dw_2] | Value : " << boolState[already_standby_OF_RC_CB_dw_2] << endl;
	cout << "Attribute :  boolState[already_required_OF_RC_CB_dw_2] | Value : " << boolState[already_required_OF_RC_CB_dw_2] << endl;
	cout << "Attribute :  boolState[required_OF_RC_CB_up_2] | Value : " << boolState[required_OF_RC_CB_up_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_RC_CB_up_2] | Value : " << boolState[already_S_OF_RC_CB_up_2] << endl;
	cout << "Attribute :  boolState[S_OF_RC_CB_up_2] | Value : " << boolState[S_OF_RC_CB_up_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RC_CB_up_2] | Value : " << boolState[relevant_evt_OF_RC_CB_up_2] << endl;
	cout << "Attribute :  boolState[failI_OF_RC_CB_up_2] | Value : " << boolState[failI_OF_RC_CB_up_2] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RC_CB_up_2] | Value : " << boolState[to_be_fired_OF_RC_CB_up_2] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RC_CB_up_2] | Value : " << boolState[already_standby_OF_RC_CB_up_2] << endl;
	cout << "Attribute :  boolState[already_required_OF_RC_CB_up_2] | Value : " << boolState[already_required_OF_RC_CB_up_2] << endl;
	cout << "Attribute :  boolState[required_OF_RO_CB_up_1] | Value : " << boolState[required_OF_RO_CB_up_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_RO_CB_up_1] | Value : " << boolState[already_S_OF_RO_CB_up_1] << endl;
	cout << "Attribute :  boolState[S_OF_RO_CB_up_1] | Value : " << boolState[S_OF_RO_CB_up_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RO_CB_up_1] | Value : " << boolState[relevant_evt_OF_RO_CB_up_1] << endl;
	cout << "Attribute :  boolState[failI_OF_RO_CB_up_1] | Value : " << boolState[failI_OF_RO_CB_up_1] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RO_CB_up_1] | Value : " << boolState[to_be_fired_OF_RO_CB_up_1] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RO_CB_up_1] | Value : " << boolState[already_standby_OF_RO_CB_up_1] << endl;
	cout << "Attribute :  boolState[already_required_OF_RO_CB_up_1] | Value : " << boolState[already_required_OF_RO_CB_up_1] << endl;
	cout << "Attribute :  boolState[required_OF_RO_CB_up_2] | Value : " << boolState[required_OF_RO_CB_up_2] << endl;
	cout << "Attribute :  boolState[already_S_OF_RO_CB_up_2] | Value : " << boolState[already_S_OF_RO_CB_up_2] << endl;
	cout << "Attribute :  boolState[S_OF_RO_CB_up_2] | Value : " << boolState[S_OF_RO_CB_up_2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RO_CB_up_2] | Value : " << boolState[relevant_evt_OF_RO_CB_up_2] << endl;
	cout << "Attribute :  boolState[failI_OF_RO_CB_up_2] | Value : " << boolState[failI_OF_RO_CB_up_2] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RO_CB_up_2] | Value : " << boolState[to_be_fired_OF_RO_CB_up_2] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RO_CB_up_2] | Value : " << boolState[already_standby_OF_RO_CB_up_2] << endl;
	cout << "Attribute :  boolState[already_required_OF_RO_CB_up_2] | Value : " << boolState[already_required_OF_RO_CB_up_2] << endl;
	cout << "Attribute :  boolState[required_OF_RS_dies] | Value : " << boolState[required_OF_RS_dies] << endl;
	cout << "Attribute :  boolState[already_S_OF_RS_dies] | Value : " << boolState[already_S_OF_RS_dies] << endl;
	cout << "Attribute :  boolState[S_OF_RS_dies] | Value : " << boolState[S_OF_RS_dies] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_RS_dies] | Value : " << boolState[relevant_evt_OF_RS_dies] << endl;
	cout << "Attribute :  boolState[failI_OF_RS_dies] | Value : " << boolState[failI_OF_RS_dies] << endl;
	cout << "Attribute :  boolState[to_be_fired_OF_RS_dies] | Value : " << boolState[to_be_fired_OF_RS_dies] << endl;
	cout << "Attribute :  boolState[already_standby_OF_RS_dies] | Value : " << boolState[already_standby_OF_RS_dies] << endl;
	cout << "Attribute :  boolState[already_required_OF_RS_dies] | Value : " << boolState[already_required_OF_RS_dies] << endl;
	cout << "Attribute :  boolState[required_OF_Transfo1] | Value : " << boolState[required_OF_Transfo1] << endl;
	cout << "Attribute :  boolState[already_S_OF_Transfo1] | Value : " << boolState[already_S_OF_Transfo1] << endl;
	cout << "Attribute :  boolState[S_OF_Transfo1] | Value : " << boolState[S_OF_Transfo1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Transfo1] | Value : " << boolState[relevant_evt_OF_Transfo1] << endl;
	cout << "Attribute :  boolState[failF_OF_Transfo1] | Value : " << boolState[failF_OF_Transfo1] << endl;
	cout << "Attribute :  boolState[required_OF_Transfo2] | Value : " << boolState[required_OF_Transfo2] << endl;
	cout << "Attribute :  boolState[already_S_OF_Transfo2] | Value : " << boolState[already_S_OF_Transfo2] << endl;
	cout << "Attribute :  boolState[S_OF_Transfo2] | Value : " << boolState[S_OF_Transfo2] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_Transfo2] | Value : " << boolState[relevant_evt_OF_Transfo2] << endl;
	cout << "Attribute :  boolState[failF_OF_Transfo2] | Value : " << boolState[failF_OF_Transfo2] << endl;
	cout << "Attribute :  boolState[required_OF_UE_1] | Value : " << boolState[required_OF_UE_1] << endl;
	cout << "Attribute :  boolState[already_S_OF_UE_1] | Value : " << boolState[already_S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[S_OF_UE_1] | Value : " << boolState[S_OF_UE_1] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_UE_1] | Value : " << boolState[relevant_evt_OF_UE_1] << endl;
	cout << "Attribute :  boolState[required_OF_dies_generator] | Value : " << boolState[required_OF_dies_generator] << endl;
	cout << "Attribute :  boolState[already_S_OF_dies_generator] | Value : " << boolState[already_S_OF_dies_generator] << endl;
	cout << "Attribute :  boolState[S_OF_dies_generator] | Value : " << boolState[S_OF_dies_generator] << endl;
	cout << "Attribute :  boolState[relevant_evt_OF_dies_generator] | Value : " << boolState[relevant_evt_OF_dies_generator] << endl;
	cout << "Attribute :  boolState[failF_OF_dies_generator] | Value : " << boolState[failF_OF_dies_generator] << endl;
}

bool storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::figaromodelhasinstransitions()
{
	return true;
}
void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::doReinitialisations()
{
	boolState[required_OF_AND_1] = REINITIALISATION_OF_required_OF_AND_1;
	boolState[S_OF_AND_1] = REINITIALISATION_OF_S_OF_AND_1;
	boolState[relevant_evt_OF_AND_1] = REINITIALISATION_OF_relevant_evt_OF_AND_1;
	boolState[required_OF_CB_dies] = REINITIALISATION_OF_required_OF_CB_dies;
	boolState[S_OF_CB_dies] = REINITIALISATION_OF_S_OF_CB_dies;
	boolState[relevant_evt_OF_CB_dies] = REINITIALISATION_OF_relevant_evt_OF_CB_dies;
	boolState[required_OF_CB_dw_1] = REINITIALISATION_OF_required_OF_CB_dw_1;
	boolState[S_OF_CB_dw_1] = REINITIALISATION_OF_S_OF_CB_dw_1;
	boolState[relevant_evt_OF_CB_dw_1] = REINITIALISATION_OF_relevant_evt_OF_CB_dw_1;
	boolState[required_OF_CB_dw_2] = REINITIALISATION_OF_required_OF_CB_dw_2;
	boolState[S_OF_CB_dw_2] = REINITIALISATION_OF_S_OF_CB_dw_2;
	boolState[relevant_evt_OF_CB_dw_2] = REINITIALISATION_OF_relevant_evt_OF_CB_dw_2;
	boolState[required_OF_CB_up_1] = REINITIALISATION_OF_required_OF_CB_up_1;
	boolState[S_OF_CB_up_1] = REINITIALISATION_OF_S_OF_CB_up_1;
	boolState[relevant_evt_OF_CB_up_1] = REINITIALISATION_OF_relevant_evt_OF_CB_up_1;
	boolState[required_OF_CB_up_2] = REINITIALISATION_OF_required_OF_CB_up_2;
	boolState[S_OF_CB_up_2] = REINITIALISATION_OF_S_OF_CB_up_2;
	boolState[relevant_evt_OF_CB_up_2] = REINITIALISATION_OF_relevant_evt_OF_CB_up_2;
	boolState[required_OF_GRID] = REINITIALISATION_OF_required_OF_GRID;
	boolState[S_OF_GRID] = REINITIALISATION_OF_S_OF_GRID;
	boolState[relevant_evt_OF_GRID] = REINITIALISATION_OF_relevant_evt_OF_GRID;
	boolState[required_OF_LossOfAllBackups] = REINITIALISATION_OF_required_OF_LossOfAllBackups;
	boolState[S_OF_LossOfAllBackups] = REINITIALISATION_OF_S_OF_LossOfAllBackups;
	boolState[relevant_evt_OF_LossOfAllBackups] = REINITIALISATION_OF_relevant_evt_OF_LossOfAllBackups;
	boolState[required_OF_LossOfDieselLine] = REINITIALISATION_OF_required_OF_LossOfDieselLine;
	boolState[S_OF_LossOfDieselLine] = REINITIALISATION_OF_S_OF_LossOfDieselLine;
	boolState[relevant_evt_OF_LossOfDieselLine] = REINITIALISATION_OF_relevant_evt_OF_LossOfDieselLine;
	boolState[required_OF_LossOfLine2] = REINITIALISATION_OF_required_OF_LossOfLine2;
	boolState[S_OF_LossOfLine2] = REINITIALISATION_OF_S_OF_LossOfLine2;
	boolState[relevant_evt_OF_LossOfLine2] = REINITIALISATION_OF_relevant_evt_OF_LossOfLine2;
	boolState[required_OF_LossOfLine_1] = REINITIALISATION_OF_required_OF_LossOfLine_1;
	boolState[S_OF_LossOfLine_1] = REINITIALISATION_OF_S_OF_LossOfLine_1;
	boolState[relevant_evt_OF_LossOfLine_1] = REINITIALISATION_OF_relevant_evt_OF_LossOfLine_1;
	boolState[required_OF_Loss_of_GRID] = REINITIALISATION_OF_required_OF_Loss_of_GRID;
	boolState[S_OF_Loss_of_GRID] = REINITIALISATION_OF_S_OF_Loss_of_GRID;
	boolState[relevant_evt_OF_Loss_of_GRID] = REINITIALISATION_OF_relevant_evt_OF_Loss_of_GRID;
	boolState[required_OF_PropagationShortCircuitTransfo1] = REINITIALISATION_OF_required_OF_PropagationShortCircuitTransfo1;
	boolState[S_OF_PropagationShortCircuitTransfo1] = REINITIALISATION_OF_S_OF_PropagationShortCircuitTransfo1;
	boolState[relevant_evt_OF_PropagationShortCircuitTransfo1] = REINITIALISATION_OF_relevant_evt_OF_PropagationShortCircuitTransfo1;
	boolState[required_OF_PropagationShortCircuitTransfo2] = REINITIALISATION_OF_required_OF_PropagationShortCircuitTransfo2;
	boolState[S_OF_PropagationShortCircuitTransfo2] = REINITIALISATION_OF_S_OF_PropagationShortCircuitTransfo2;
	boolState[relevant_evt_OF_PropagationShortCircuitTransfo2] = REINITIALISATION_OF_relevant_evt_OF_PropagationShortCircuitTransfo2;
	boolState[required_OF_RC_CB_dies] = REINITIALISATION_OF_required_OF_RC_CB_dies;
	boolState[S_OF_RC_CB_dies] = REINITIALISATION_OF_S_OF_RC_CB_dies;
	boolState[relevant_evt_OF_RC_CB_dies] = REINITIALISATION_OF_relevant_evt_OF_RC_CB_dies;
	boolState[to_be_fired_OF_RC_CB_dies] = REINITIALISATION_OF_to_be_fired_OF_RC_CB_dies;
	boolState[required_OF_RC_CB_dw_2] = REINITIALISATION_OF_required_OF_RC_CB_dw_2;
	boolState[S_OF_RC_CB_dw_2] = REINITIALISATION_OF_S_OF_RC_CB_dw_2;
	boolState[relevant_evt_OF_RC_CB_dw_2] = REINITIALISATION_OF_relevant_evt_OF_RC_CB_dw_2;
	boolState[to_be_fired_OF_RC_CB_dw_2] = REINITIALISATION_OF_to_be_fired_OF_RC_CB_dw_2;
	boolState[required_OF_RC_CB_up_2] = REINITIALISATION_OF_required_OF_RC_CB_up_2;
	boolState[S_OF_RC_CB_up_2] = REINITIALISATION_OF_S_OF_RC_CB_up_2;
	boolState[relevant_evt_OF_RC_CB_up_2] = REINITIALISATION_OF_relevant_evt_OF_RC_CB_up_2;
	boolState[to_be_fired_OF_RC_CB_up_2] = REINITIALISATION_OF_to_be_fired_OF_RC_CB_up_2;
	boolState[required_OF_RO_CB_up_1] = REINITIALISATION_OF_required_OF_RO_CB_up_1;
	boolState[S_OF_RO_CB_up_1] = REINITIALISATION_OF_S_OF_RO_CB_up_1;
	boolState[relevant_evt_OF_RO_CB_up_1] = REINITIALISATION_OF_relevant_evt_OF_RO_CB_up_1;
	boolState[to_be_fired_OF_RO_CB_up_1] = REINITIALISATION_OF_to_be_fired_OF_RO_CB_up_1;
	boolState[required_OF_RO_CB_up_2] = REINITIALISATION_OF_required_OF_RO_CB_up_2;
	boolState[S_OF_RO_CB_up_2] = REINITIALISATION_OF_S_OF_RO_CB_up_2;
	boolState[relevant_evt_OF_RO_CB_up_2] = REINITIALISATION_OF_relevant_evt_OF_RO_CB_up_2;
	boolState[to_be_fired_OF_RO_CB_up_2] = REINITIALISATION_OF_to_be_fired_OF_RO_CB_up_2;
	boolState[required_OF_RS_dies] = REINITIALISATION_OF_required_OF_RS_dies;
	boolState[S_OF_RS_dies] = REINITIALISATION_OF_S_OF_RS_dies;
	boolState[relevant_evt_OF_RS_dies] = REINITIALISATION_OF_relevant_evt_OF_RS_dies;
	boolState[to_be_fired_OF_RS_dies] = REINITIALISATION_OF_to_be_fired_OF_RS_dies;
	boolState[required_OF_Transfo1] = REINITIALISATION_OF_required_OF_Transfo1;
	boolState[S_OF_Transfo1] = REINITIALISATION_OF_S_OF_Transfo1;
	boolState[relevant_evt_OF_Transfo1] = REINITIALISATION_OF_relevant_evt_OF_Transfo1;
	boolState[required_OF_Transfo2] = REINITIALISATION_OF_required_OF_Transfo2;
	boolState[S_OF_Transfo2] = REINITIALISATION_OF_S_OF_Transfo2;
	boolState[relevant_evt_OF_Transfo2] = REINITIALISATION_OF_relevant_evt_OF_Transfo2;
	boolState[required_OF_UE_1] = REINITIALISATION_OF_required_OF_UE_1;
	boolState[S_OF_UE_1] = REINITIALISATION_OF_S_OF_UE_1;
	boolState[relevant_evt_OF_UE_1] = REINITIALISATION_OF_relevant_evt_OF_UE_1;
	boolState[required_OF_dies_generator] = REINITIALISATION_OF_required_OF_dies_generator;
	boolState[S_OF_dies_generator] = REINITIALISATION_OF_S_OF_dies_generator;
	boolState[relevant_evt_OF_dies_generator] = REINITIALISATION_OF_relevant_evt_OF_dies_generator;
}

void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::fireOccurrence(int numFire)
{
	cout <<">>>>>>>>>>>>>>>>>>>> Fire of occurrence #" << numFire << " <<<<<<<<<<<<<<<<<<<<<<<" << endl;

	if (numFire == 0)
	{
		FIRE_xx10_OF_CB_dies = true;
	}

	if (numFire == 1)
	{
		FIRE_xx10_OF_CB_dw_1 = true;
	}

	if (numFire == 2)
	{
		FIRE_xx10_OF_CB_dw_2 = true;
	}

	if (numFire == 3)
	{
		FIRE_xx10_OF_CB_up_1 = true;
	}

	if (numFire == 4)
	{
		FIRE_xx10_OF_CB_up_2 = true;
	}

	if (numFire == 5)
	{
		FIRE_xx10_OF_GRID = true;
	}

	if (numFire == 6)
	{
		FIRE_xx23_OF_RC_CB_dies_INS_6 = true;
	}

	if (numFire == 7)
	{
		FIRE_xx23_OF_RC_CB_dies_INS_7 = true;
	}

	if (numFire == 8)
	{
		FIRE_xx23_OF_RC_CB_dw_2_INS_8 = true;
	}

	if (numFire == 9)
	{
		FIRE_xx23_OF_RC_CB_dw_2_INS_9 = true;
	}

	if (numFire == 10)
	{
		FIRE_xx23_OF_RC_CB_up_2_INS_10 = true;
	}

	if (numFire == 11)
	{
		FIRE_xx23_OF_RC_CB_up_2_INS_11 = true;
	}

	if (numFire == 12)
	{
		FIRE_xx23_OF_RO_CB_up_1_INS_12 = true;
	}

	if (numFire == 13)
	{
		FIRE_xx23_OF_RO_CB_up_1_INS_13 = true;
	}

	if (numFire == 14)
	{
		FIRE_xx23_OF_RO_CB_up_2_INS_14 = true;
	}

	if (numFire == 15)
	{
		FIRE_xx23_OF_RO_CB_up_2_INS_15 = true;
	}

	if (numFire == 16)
	{
		FIRE_xx23_OF_RS_dies_INS_16 = true;
	}

	if (numFire == 17)
	{
		FIRE_xx23_OF_RS_dies_INS_17 = true;
	}

	if (numFire == 18)
	{
		FIRE_xx10_OF_Transfo1 = true;
	}

	if (numFire == 19)
	{
		FIRE_xx10_OF_Transfo2 = true;
	}

	if (numFire == 20)
	{
		FIRE_xx10_OF_dies_generator = true;
	}

/* ---------- DECLARATION OF OCCURRENCE RULES------------ */

	// Occurrence xx10_OF_CB_dies
	if ((boolState[failF_OF_CB_dies] == false) && (boolState[required_OF_CB_dies] &&  boolState[relevant_evt_OF_CB_dies])) 
	{
		 
		if (FIRE_xx10_OF_CB_dies)
		{
			boolState[failF_OF_CB_dies]  =  true;
			FIRE_xx10_OF_CB_dies = false;
		}
	}

	// Occurrence xx10_OF_CB_dw_1
	if ((boolState[failF_OF_CB_dw_1] == false) && (boolState[required_OF_CB_dw_1] &&  boolState[relevant_evt_OF_CB_dw_1])) 
	{
		 
		if (FIRE_xx10_OF_CB_dw_1)
		{
			boolState[failF_OF_CB_dw_1]  =  true;
			FIRE_xx10_OF_CB_dw_1 = false;
		}
	}

	// Occurrence xx10_OF_CB_dw_2
	if ((boolState[failF_OF_CB_dw_2] == false) && (boolState[required_OF_CB_dw_2] &&  boolState[relevant_evt_OF_CB_dw_2])) 
	{
		 
		if (FIRE_xx10_OF_CB_dw_2)
		{
			boolState[failF_OF_CB_dw_2]  =  true;
			FIRE_xx10_OF_CB_dw_2 = false;
		}
	}

	// Occurrence xx10_OF_CB_up_1
	if ((boolState[failF_OF_CB_up_1] == false) && (boolState[required_OF_CB_up_1] &&  boolState[relevant_evt_OF_CB_up_1])) 
	{
		 
		if (FIRE_xx10_OF_CB_up_1)
		{
			boolState[failF_OF_CB_up_1]  =  true;
			FIRE_xx10_OF_CB_up_1 = false;
		}
	}

	// Occurrence xx10_OF_CB_up_2
	if ((boolState[failF_OF_CB_up_2] == false) && (boolState[required_OF_CB_up_2] &&  boolState[relevant_evt_OF_CB_up_2])) 
	{
		 
		if (FIRE_xx10_OF_CB_up_2)
		{
			boolState[failF_OF_CB_up_2]  =  true;
			FIRE_xx10_OF_CB_up_2 = false;
		}
	}

	// Occurrence xx10_OF_GRID
	if ((boolState[failF_OF_GRID] == false) && (boolState[required_OF_GRID] &&  boolState[relevant_evt_OF_GRID])) 
	{
		 
		if (FIRE_xx10_OF_GRID)
		{
			boolState[failF_OF_GRID]  =  true;
			FIRE_xx10_OF_GRID = false;
		}
	}

	// Occurrence xx23_OF_RC_CB_dies

	if ((boolState[failI_OF_RC_CB_dies] == false) && (boolState[to_be_fired_OF_RC_CB_dies]
	&& boolState[relevant_evt_OF_RC_CB_dies])) 
	{
	
		
		if (FIRE_xx23_OF_RC_CB_dies_INS_6) 
		{
			boolState[failI_OF_RC_CB_dies]  =  true;
			boolState[already_standby_OF_RC_CB_dies]  =  false;
			boolState[already_required_OF_RC_CB_dies]  =  false;
			FIRE_xx23_OF_RC_CB_dies_INS_6 = false;
		}
	
	}
	if ((boolState[failI_OF_RC_CB_dies] == false) && (boolState[to_be_fired_OF_RC_CB_dies]
	&& boolState[relevant_evt_OF_RC_CB_dies])) 
	{
	
		
		if (FIRE_xx23_OF_RC_CB_dies_INS_7) 
		{
			boolState[already_standby_OF_RC_CB_dies]  =  false;
			boolState[already_required_OF_RC_CB_dies]  =  false;
			FIRE_xx23_OF_RC_CB_dies_INS_7 = false;
		}
	
	}
	// Occurrence xx23_OF_RC_CB_dw_2

	if ((boolState[failI_OF_RC_CB_dw_2] == false) && (boolState[to_be_fired_OF_RC_CB_dw_2]
	&& boolState[relevant_evt_OF_RC_CB_dw_2])) 
	{
	
		
		if (FIRE_xx23_OF_RC_CB_dw_2_INS_8) 
		{
			boolState[failI_OF_RC_CB_dw_2]  =  true;
			boolState[already_standby_OF_RC_CB_dw_2]  =  false;
			boolState[already_required_OF_RC_CB_dw_2]  =  false;
			FIRE_xx23_OF_RC_CB_dw_2_INS_8 = false;
		}
	
	}
	if ((boolState[failI_OF_RC_CB_dw_2] == false) && (boolState[to_be_fired_OF_RC_CB_dw_2]
	&& boolState[relevant_evt_OF_RC_CB_dw_2])) 
	{
	
		
		if (FIRE_xx23_OF_RC_CB_dw_2_INS_9) 
		{
			boolState[already_standby_OF_RC_CB_dw_2]  =  false;
			boolState[already_required_OF_RC_CB_dw_2]  =  false;
			FIRE_xx23_OF_RC_CB_dw_2_INS_9 = false;
		}
	
	}
	// Occurrence xx23_OF_RC_CB_up_2

	if ((boolState[failI_OF_RC_CB_up_2] == false) && (boolState[to_be_fired_OF_RC_CB_up_2]
	&& boolState[relevant_evt_OF_RC_CB_up_2])) 
	{
	
		
		if (FIRE_xx23_OF_RC_CB_up_2_INS_10) 
		{
			boolState[failI_OF_RC_CB_up_2]  =  true;
			boolState[already_standby_OF_RC_CB_up_2]  =  false;
			boolState[already_required_OF_RC_CB_up_2]  =  false;
			FIRE_xx23_OF_RC_CB_up_2_INS_10 = false;
		}
	
	}
	if ((boolState[failI_OF_RC_CB_up_2] == false) && (boolState[to_be_fired_OF_RC_CB_up_2]
	&& boolState[relevant_evt_OF_RC_CB_up_2])) 
	{
	
		
		if (FIRE_xx23_OF_RC_CB_up_2_INS_11) 
		{
			boolState[already_standby_OF_RC_CB_up_2]  =  false;
			boolState[already_required_OF_RC_CB_up_2]  =  false;
			FIRE_xx23_OF_RC_CB_up_2_INS_11 = false;
		}
	
	}
	// Occurrence xx23_OF_RO_CB_up_1

	if ((boolState[failI_OF_RO_CB_up_1] == false) && (boolState[to_be_fired_OF_RO_CB_up_1]
	&& boolState[relevant_evt_OF_RO_CB_up_1])) 
	{
	
		
		if (FIRE_xx23_OF_RO_CB_up_1_INS_12) 
		{
			boolState[failI_OF_RO_CB_up_1]  =  true;
			boolState[already_standby_OF_RO_CB_up_1]  =  false;
			boolState[already_required_OF_RO_CB_up_1]  =  false;
			FIRE_xx23_OF_RO_CB_up_1_INS_12 = false;
		}
	
	}
	if ((boolState[failI_OF_RO_CB_up_1] == false) && (boolState[to_be_fired_OF_RO_CB_up_1]
	&& boolState[relevant_evt_OF_RO_CB_up_1])) 
	{
	
		
		if (FIRE_xx23_OF_RO_CB_up_1_INS_13) 
		{
			boolState[already_standby_OF_RO_CB_up_1]  =  false;
			boolState[already_required_OF_RO_CB_up_1]  =  false;
			FIRE_xx23_OF_RO_CB_up_1_INS_13 = false;
		}
	
	}
	// Occurrence xx23_OF_RO_CB_up_2

	if ((boolState[failI_OF_RO_CB_up_2] == false) && (boolState[to_be_fired_OF_RO_CB_up_2]
	&& boolState[relevant_evt_OF_RO_CB_up_2])) 
	{
	
		
		if (FIRE_xx23_OF_RO_CB_up_2_INS_14) 
		{
			boolState[failI_OF_RO_CB_up_2]  =  true;
			boolState[already_standby_OF_RO_CB_up_2]  =  false;
			boolState[already_required_OF_RO_CB_up_2]  =  false;
			FIRE_xx23_OF_RO_CB_up_2_INS_14 = false;
		}
	
	}
	if ((boolState[failI_OF_RO_CB_up_2] == false) && (boolState[to_be_fired_OF_RO_CB_up_2]
	&& boolState[relevant_evt_OF_RO_CB_up_2])) 
	{
	
		
		if (FIRE_xx23_OF_RO_CB_up_2_INS_15) 
		{
			boolState[already_standby_OF_RO_CB_up_2]  =  false;
			boolState[already_required_OF_RO_CB_up_2]  =  false;
			FIRE_xx23_OF_RO_CB_up_2_INS_15 = false;
		}
	
	}
	// Occurrence xx23_OF_RS_dies

	if ((boolState[failI_OF_RS_dies] == false) && (boolState[to_be_fired_OF_RS_dies] &&
	boolState[relevant_evt_OF_RS_dies])) 
	{
	
		
		if (FIRE_xx23_OF_RS_dies_INS_16) 
		{
			boolState[failI_OF_RS_dies]  =  true;
			boolState[already_standby_OF_RS_dies]  =  false;
			boolState[already_required_OF_RS_dies]  =  false;
			FIRE_xx23_OF_RS_dies_INS_16 = false;
		}
	
	}
	if ((boolState[failI_OF_RS_dies] == false) && (boolState[to_be_fired_OF_RS_dies] &&
	boolState[relevant_evt_OF_RS_dies])) 
	{
	
		
		if (FIRE_xx23_OF_RS_dies_INS_17) 
		{
			boolState[already_standby_OF_RS_dies]  =  false;
			boolState[already_required_OF_RS_dies]  =  false;
			FIRE_xx23_OF_RS_dies_INS_17 = false;
		}
	
	}
	// Occurrence xx10_OF_Transfo1
	if ((boolState[failF_OF_Transfo1] == false) && (boolState[required_OF_Transfo1] &&  boolState[relevant_evt_OF_Transfo1])) 
	{
		 
		if (FIRE_xx10_OF_Transfo1)
		{
			boolState[failF_OF_Transfo1]  =  true;
			FIRE_xx10_OF_Transfo1 = false;
		}
	}

	// Occurrence xx10_OF_Transfo2
	if ((boolState[failF_OF_Transfo2] == false) && (boolState[required_OF_Transfo2] &&  boolState[relevant_evt_OF_Transfo2])) 
	{
		 
		if (FIRE_xx10_OF_Transfo2)
		{
			boolState[failF_OF_Transfo2]  =  true;
			FIRE_xx10_OF_Transfo2 = false;
		}
	}

	// Occurrence xx10_OF_dies_generator
	if ((boolState[failF_OF_dies_generator] == false) && (boolState[required_OF_dies_generator] && boolState[relevant_evt_OF_dies_generator])) 
	{
		 
		if (FIRE_xx10_OF_dies_generator)
		{
			boolState[failF_OF_dies_generator]  =  true;
			FIRE_xx10_OF_dies_generator = false;
		}
	}

}

std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::showFireableOccurrences()
{
	std::vector<std::tuple<int, double, std::string, int>> list = {};
	cout <<"\n==================== List of fireable occurrences :  ====================" << endl;

	if ((boolState[failI_OF_RC_CB_dies] == false) && (boolState[to_be_fired_OF_RC_CB_dies] && boolState[relevant_evt_OF_RC_CB_dies]))
	{
		cout << "6 :  INS_SUB_COUNT (1) |FAULT | failI  LABEL \"instantaneous failure RC_CB_dies\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_RC_CB_dies]  =  TRUE,already_standby_OF_RC_CB_dies  =  FALSE,already_required_OF_RC_CB_dies  =  FALSE" << endl;
		list.push_back(make_tuple(6, 0.0001, "INS", 1));
	}
	if ((boolState[failI_OF_RC_CB_dies] == false) && (boolState[to_be_fired_OF_RC_CB_dies] && boolState[relevant_evt_OF_RC_CB_dies]))
	{
		cout << "7 :  INS_SUB_COUNT (1) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_RC_CB_dies]  =  FALSE,already_required_OF_RC_CB_dies  =  FALSE" << endl;
		list.push_back(make_tuple(7, 0.9999, "INS", 1));
	}
	if ((boolState[failI_OF_RC_CB_dw_2] == false) && (boolState[to_be_fired_OF_RC_CB_dw_2] && boolState[relevant_evt_OF_RC_CB_dw_2]))
	{
		cout << "8 :  INS_SUB_COUNT (2) |FAULT | failI  LABEL \"instantaneous failure RC_CB_dw_2\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_RC_CB_dw_2]  =  TRUE,already_standby_OF_RC_CB_dw_2  =  FALSE,already_required_OF_RC_CB_dw_2  =  FALSE" << endl;
		list.push_back(make_tuple(8, 0.0001, "INS", 2));
	}
	if ((boolState[failI_OF_RC_CB_dw_2] == false) && (boolState[to_be_fired_OF_RC_CB_dw_2] && boolState[relevant_evt_OF_RC_CB_dw_2]))
	{
		cout << "9 :  INS_SUB_COUNT (2) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_RC_CB_dw_2]  =  FALSE,already_required_OF_RC_CB_dw_2  =  FALSE" << endl;
		list.push_back(make_tuple(9, 0.9999, "INS", 2));
	}
	if ((boolState[failI_OF_RC_CB_up_2] == false) && (boolState[to_be_fired_OF_RC_CB_up_2] && boolState[relevant_evt_OF_RC_CB_up_2]))
	{
		cout << "10 :  INS_SUB_COUNT (3) |FAULT | failI  LABEL \"instantaneous failure RC_CB_up_2\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_RC_CB_up_2]  =  TRUE,already_standby_OF_RC_CB_up_2  =  FALSE,already_required_OF_RC_CB_up_2  =  FALSE" << endl;
		list.push_back(make_tuple(10, 0.0001, "INS", 3));
	}
	if ((boolState[failI_OF_RC_CB_up_2] == false) && (boolState[to_be_fired_OF_RC_CB_up_2] && boolState[relevant_evt_OF_RC_CB_up_2]))
	{
		cout << "11 :  INS_SUB_COUNT (3) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_RC_CB_up_2]  =  FALSE,already_required_OF_RC_CB_up_2  =  FALSE" << endl;
		list.push_back(make_tuple(11, 0.9999, "INS", 3));
	}
	if ((boolState[failI_OF_RO_CB_up_1] == false) && (boolState[to_be_fired_OF_RO_CB_up_1] && boolState[relevant_evt_OF_RO_CB_up_1]))
	{
		cout << "12 :  INS_SUB_COUNT (4) |FAULT | failI  LABEL \"instantaneous failure RO_CB_up_1\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_RO_CB_up_1]  =  TRUE,already_standby_OF_RO_CB_up_1  =  FALSE,already_required_OF_RO_CB_up_1  =  FALSE" << endl;
		list.push_back(make_tuple(12, 0.0001, "INS", 4));
	}
	if ((boolState[failI_OF_RO_CB_up_1] == false) && (boolState[to_be_fired_OF_RO_CB_up_1] && boolState[relevant_evt_OF_RO_CB_up_1]))
	{
		cout << "13 :  INS_SUB_COUNT (4) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_RO_CB_up_1]  =  FALSE,already_required_OF_RO_CB_up_1  =  FALSE" << endl;
		list.push_back(make_tuple(13, 0.9999, "INS", 4));
	}
	if ((boolState[failI_OF_RO_CB_up_2] == false) && (boolState[to_be_fired_OF_RO_CB_up_2] && boolState[relevant_evt_OF_RO_CB_up_2]))
	{
		cout << "14 :  INS_SUB_COUNT (5) |FAULT | failI  LABEL \"instantaneous failure RO_CB_up_2\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_RO_CB_up_2]  =  TRUE,already_standby_OF_RO_CB_up_2  =  FALSE,already_required_OF_RO_CB_up_2  =  FALSE" << endl;
		list.push_back(make_tuple(14, 0.0001, "INS", 5));
	}
	if ((boolState[failI_OF_RO_CB_up_2] == false) && (boolState[to_be_fired_OF_RO_CB_up_2] && boolState[relevant_evt_OF_RO_CB_up_2]))
	{
		cout << "15 :  INS_SUB_COUNT (5) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_RO_CB_up_2]  =  FALSE,already_required_OF_RO_CB_up_2  =  FALSE" << endl;
		list.push_back(make_tuple(15, 0.9999, "INS", 5));
	}
	if ((boolState[failI_OF_RS_dies] == false) && (boolState[to_be_fired_OF_RS_dies] &&	boolState[relevant_evt_OF_RS_dies]))
	{
		cout << "16 :  INS_SUB_COUNT (6) |FAULT | failI  LABEL \"instantaneous failure RS_dies\" | DIST INS (0.0001) | INDUCING boolState[failI_OF_RS_dies]  =  TRUE,already_standby_OF_RS_dies  =  FALSE,already_required_OF_RS_dies  =  FALSE" << endl;
		list.push_back(make_tuple(16, 0.0001, "INS", 6));
	}
	if ((boolState[failI_OF_RS_dies] == false) && (boolState[to_be_fired_OF_RS_dies] &&	boolState[relevant_evt_OF_RS_dies]))
	{
		cout << "17 :  INS_SUB_COUNT (6) |TRANSITION | good | DIST INS (0.9999) | INDUCING boolState[already_standby_OF_RS_dies]  =  FALSE,already_required_OF_RS_dies  =  FALSE" << endl;
		list.push_back(make_tuple(17, 0.9999, "INS", 6));
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
     
	if ((boolState[failF_OF_CB_dies] == false) && (boolState[required_OF_CB_dies] && boolState[relevant_evt_OF_CB_dies]))
	{
		cout << "0 : xx10_OF_CB_dies : FAULT failF  LABEL \"failure in operation CB_dies\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB_dies]  =  TRUE" << endl;
		list.push_back(make_tuple(0, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_CB_dw_1] == false) && (boolState[required_OF_CB_dw_1] && boolState[relevant_evt_OF_CB_dw_1]))
	{
		cout << "1 : xx10_OF_CB_dw_1 : FAULT failF  LABEL \"failure in operation CB_dw_1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB_dw_1]  =  TRUE" << endl;
		list.push_back(make_tuple(1, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_CB_dw_2] == false) && (boolState[required_OF_CB_dw_2] && boolState[relevant_evt_OF_CB_dw_2]))
	{
		cout << "2 : xx10_OF_CB_dw_2 : FAULT failF  LABEL \"failure in operation CB_dw_2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB_dw_2]  =  TRUE" << endl;
		list.push_back(make_tuple(2, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_CB_up_1] == false) && (boolState[required_OF_CB_up_1] && boolState[relevant_evt_OF_CB_up_1]))
	{
		cout << "3 : xx10_OF_CB_up_1 : FAULT failF  LABEL \"failure in operation CB_up_1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB_up_1]  =  TRUE" << endl;
		list.push_back(make_tuple(3, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_CB_up_2] == false) && (boolState[required_OF_CB_up_2] && boolState[relevant_evt_OF_CB_up_2]))
	{
		cout << "4 : xx10_OF_CB_up_2 : FAULT failF  LABEL \"failure in operation CB_up_2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_CB_up_2]  =  TRUE" << endl;
		list.push_back(make_tuple(4, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_GRID] == false) && (boolState[required_OF_GRID] && boolState[relevant_evt_OF_GRID]))
	{
		cout << "5 : xx10_OF_GRID : FAULT failF  LABEL \"failure in operation GRID\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_GRID]  =  TRUE" << endl;
		list.push_back(make_tuple(5, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Transfo1] == false) && (boolState[required_OF_Transfo1] && boolState[relevant_evt_OF_Transfo1]))
	{
		cout << "18 : xx10_OF_Transfo1 : FAULT failF  LABEL \"failure in operation Transfo1\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Transfo1]  =  TRUE" << endl;
		list.push_back(make_tuple(18, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_Transfo2] == false) && (boolState[required_OF_Transfo2] && boolState[relevant_evt_OF_Transfo2]))
	{
		cout << "19 : xx10_OF_Transfo2 : FAULT failF  LABEL \"failure in operation Transfo2\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_Transfo2]  =  TRUE" << endl;
		list.push_back(make_tuple(19, 0.0001, "EXP", 0));
	}
	if ((boolState[failF_OF_dies_generator] == false) && (boolState[required_OF_dies_generator] && boolState[relevant_evt_OF_dies_generator]))
	{
		cout << "20 : xx10_OF_dies_generator : FAULT failF  LABEL \"failure in operation dies_generator\"  DIST EXP (0.0001)  INDUCING boolState[failF_OF_dies_generator]  =  TRUE" << endl;
		list.push_back(make_tuple(20, 0.0001, "EXP", 0));
	}
	return list;
}


void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::runOnceInteractionStep_initialization()
{
	if (boolState[failF_OF_CB_dies] == true )
	{
		boolState[S_OF_CB_dies]  =  true;
	}

	if (boolState[failF_OF_CB_dw_1] == true )
	{
		boolState[S_OF_CB_dw_1]  =  true;
	}

	if (boolState[failF_OF_CB_dw_2] == true )
	{
		boolState[S_OF_CB_dw_2]  =  true;
	}

	if (boolState[failF_OF_CB_up_1] == true )
	{
		boolState[S_OF_CB_up_1]  =  true;
	}

	if (boolState[failF_OF_CB_up_2] == true )
	{
		boolState[S_OF_CB_up_2]  =  true;
	}

	if (boolState[failF_OF_GRID] == true )
	{
		boolState[S_OF_GRID]  =  true;
	}

	if (boolState[failI_OF_RC_CB_dies] == true )
	{
		boolState[S_OF_RC_CB_dies]  =  true;
	}

	if (boolState[failI_OF_RC_CB_dw_2] == true )
	{
		boolState[S_OF_RC_CB_dw_2]  =  true;
	}

	if (boolState[failI_OF_RC_CB_up_2] == true )
	{
		boolState[S_OF_RC_CB_up_2]  =  true;
	}

	if (boolState[failI_OF_RO_CB_up_1] == true )
	{
		boolState[S_OF_RO_CB_up_1]  =  true;
	}

	if (boolState[failI_OF_RO_CB_up_2] == true )
	{
		boolState[S_OF_RO_CB_up_2]  =  true;
	}

	if (boolState[failI_OF_RS_dies] == true )
	{
		boolState[S_OF_RS_dies]  =  true;
	}

	if (boolState[failF_OF_Transfo1] == true )
	{
		boolState[S_OF_Transfo1]  =  true;
	}

	if (boolState[failF_OF_Transfo2] == true )
	{
		boolState[S_OF_Transfo2]  =  true;
	}

	if (boolState[failF_OF_dies_generator] == true )
	{
		boolState[S_OF_dies_generator]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::runOnceInteractionStep_propagate_effect_S()
{
	if (boolState[S_OF_LossOfAllBackups] && boolState[S_OF_LossOfLine_1] )
	{
		boolState[S_OF_AND_1]  =  true;
	}

	if (boolState[S_OF_LossOfDieselLine] && boolState[S_OF_LossOfLine2] )
	{
		boolState[S_OF_LossOfAllBackups]  =  true;
	}

	if (((boolState[S_OF_CB_dies] || boolState[S_OF_RS_dies]) || boolState[S_OF_RC_CB_dies]) || boolState[S_OF_dies_generator] )
	{
		boolState[S_OF_LossOfDieselLine]  =  true;
	}

	if ((((((boolState[S_OF_Loss_of_GRID] || boolState[S_OF_Transfo2]) || boolState[S_OF_CB_up_2]) || boolState[S_OF_RC_CB_dw_2]) || boolState[S_OF_CB_dw_2]) || boolState[S_OF_GRID]) || boolState[S_OF_RC_CB_up_2] )
	{
		boolState[S_OF_LossOfLine2]  =  true;
	}

	if ((((boolState[S_OF_Loss_of_GRID] || boolState[S_OF_CB_up_1]) || boolState[S_OF_GRID]) || boolState[S_OF_Transfo1]) || boolState[S_OF_CB_dw_1] )
	{
		boolState[S_OF_LossOfLine_1]  =  true;
	}

	if ((boolState[S_OF_GRID] || boolState[S_OF_PropagationShortCircuitTransfo1]) || boolState[S_OF_PropagationShortCircuitTransfo2] )
	{
		boolState[S_OF_Loss_of_GRID]  =  true;
	}

	if (boolState[S_OF_RO_CB_up_1] && boolState[S_OF_Transfo1] )
	{
		boolState[S_OF_PropagationShortCircuitTransfo1]  =  true;
	}

	if (boolState[S_OF_RO_CB_up_2] && boolState[S_OF_Transfo2] )
	{
		boolState[S_OF_PropagationShortCircuitTransfo2]  =  true;
	}

	if (boolState[S_OF_AND_1] )
	{
		boolState[S_OF_UE_1]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::runOnceInteractionStep_propagate_effect_required()
{
	if ( !boolState[required_OF_UE_1] )
	{
		boolState[required_OF_AND_1]  =  false;
	}

	if (boolState[relevant_evt_OF_UE_1] && ( !boolState[S_OF_UE_1]) )
	{
		boolState[relevant_evt_OF_AND_1]  =  true;
	}

	if ( !boolState[required_OF_LossOfDieselLine] )
	{
		boolState[required_OF_CB_dies]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_CB_dies]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine_1] )
	{
		boolState[required_OF_CB_dw_1]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1]) )
	{
		boolState[relevant_evt_OF_CB_dw_1]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine2] )
	{
		boolState[required_OF_CB_dw_2]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2]) )
	{
		boolState[relevant_evt_OF_CB_dw_2]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine_1] )
	{
		boolState[required_OF_CB_up_1]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1]) )
	{
		boolState[relevant_evt_OF_CB_up_1]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine2] )
	{
		boolState[required_OF_CB_up_2]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2]) )
	{
		boolState[relevant_evt_OF_CB_up_2]  =  true;
	}

	if ((( !boolState[required_OF_Loss_of_GRID]) && ( !boolState[required_OF_LossOfLine_1])) && ( !boolState[required_OF_LossOfLine2]) )
	{
		boolState[required_OF_GRID]  =  false;
	}

	if (((boolState[relevant_evt_OF_Loss_of_GRID] && ( !boolState[S_OF_Loss_of_GRID])) || (boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1]))  ) || (boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2])) )
	{
		boolState[relevant_evt_OF_GRID]  =  true;
	}

	if (( !boolState[required_OF_AND_1]) || ( !boolState[S_OF_LossOfLine_1]) )
	{
		boolState[required_OF_LossOfAllBackups]  =  false;
	}

	if (boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1]) )
	{
		boolState[relevant_evt_OF_LossOfAllBackups]  =  true;
	}

	if (( !boolState[required_OF_LossOfAllBackups]) || ( !boolState[S_OF_LossOfLine2]) )
	{
		boolState[required_OF_LossOfDieselLine]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfAllBackups] && ( !boolState[S_OF_LossOfAllBackups]) )
	{
		boolState[relevant_evt_OF_LossOfDieselLine]  =  true;
	}

	if ( !boolState[required_OF_LossOfAllBackups] )
	{
		boolState[required_OF_LossOfLine2]  =  false;
	}

	if ((boolState[relevant_evt_OF_LossOfAllBackups] && ( !boolState[S_OF_LossOfAllBackups])) || ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_LossOfLine2]  =  true;
	}

	if ( !boolState[required_OF_AND_1] )
	{
		boolState[required_OF_LossOfLine_1]  =  false;
	}

	if ((boolState[relevant_evt_OF_AND_1] && ( !boolState[S_OF_AND_1])) || ( !boolState[S_OF_LossOfAllBackups]) )
	{
		boolState[relevant_evt_OF_LossOfLine_1]  =  true;
	}

	if (( !boolState[required_OF_LossOfLine2]) && ( !boolState[required_OF_LossOfLine_1]) )
	{
		boolState[required_OF_Loss_of_GRID]  =  false;
	}

	if ((boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2])) || (  boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1])) )
	{
		boolState[relevant_evt_OF_Loss_of_GRID]  =  true;
	}

	if ( !boolState[required_OF_Loss_of_GRID] )
	{
		boolState[required_OF_PropagationShortCircuitTransfo1]  =  false;
	}

	if (boolState[relevant_evt_OF_Loss_of_GRID] && ( !boolState[S_OF_Loss_of_GRID]) )
	{
		boolState[relevant_evt_OF_PropagationShortCircuitTransfo1]  =  true;
	}

	if ( !boolState[required_OF_Loss_of_GRID] )
	{
		boolState[required_OF_PropagationShortCircuitTransfo2]  =  false;
	}

	if (boolState[relevant_evt_OF_Loss_of_GRID] && ( !boolState[S_OF_Loss_of_GRID]) )
	{
		boolState[relevant_evt_OF_PropagationShortCircuitTransfo2]  =  true;
	}

	if ( !boolState[required_OF_LossOfDieselLine] )
	{
		boolState[required_OF_RC_CB_dies]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_RC_CB_dies]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine2] )
	{
		boolState[required_OF_RC_CB_dw_2]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2]) )
	{
		boolState[relevant_evt_OF_RC_CB_dw_2]  =  true;
	}

	if ( !boolState[required_OF_LossOfLine2] )
	{
		boolState[required_OF_RC_CB_up_2]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2]) )
	{
		boolState[relevant_evt_OF_RC_CB_up_2]  =  true;
	}

	if (( !boolState[required_OF_PropagationShortCircuitTransfo1]) || ( !boolState[S_OF_Transfo1]) )
	{
		boolState[required_OF_RO_CB_up_1]  =  false;
	}

	if (boolState[relevant_evt_OF_PropagationShortCircuitTransfo1] && ( !boolState[S_OF_PropagationShortCircuitTransfo1]) )
	{
		boolState[relevant_evt_OF_RO_CB_up_1]  =  true;
	}

	if (( !boolState[required_OF_PropagationShortCircuitTransfo2]) || ( !boolState[S_OF_Transfo2]) )
	{
		boolState[required_OF_RO_CB_up_2]  =  false;
	}

	if (boolState[relevant_evt_OF_PropagationShortCircuitTransfo2] && ( !boolState[S_OF_PropagationShortCircuitTransfo2]) )
	{
		boolState[relevant_evt_OF_RO_CB_up_2]  =  true;
	}

	if ( !boolState[required_OF_LossOfDieselLine] )
	{
		boolState[required_OF_RS_dies]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_RS_dies]  =  true;
	}

	if (( !boolState[required_OF_PropagationShortCircuitTransfo1]) && ( !  boolState[required_OF_LossOfLine_1]) )
	{
		boolState[required_OF_Transfo1]  =  false;
	}

	if (((boolState[relevant_evt_OF_PropagationShortCircuitTransfo1] && ( !boolState[S_OF_PropagationShortCircuitTransfo1])) || (boolState[relevant_evt_OF_LossOfLine_1] && ( !boolState[S_OF_LossOfLine_1]))) || ( !boolState[S_OF_RO_CB_up_1]) )
	{
		boolState[relevant_evt_OF_Transfo1]  =  true;
	}

	if (( !boolState[required_OF_LossOfLine2]) && ( !boolState[required_OF_PropagationShortCircuitTransfo2]) )
	{
		boolState[required_OF_Transfo2]  =  false;
	}

	if (((boolState[relevant_evt_OF_LossOfLine2] && ( !boolState[S_OF_LossOfLine2])) || (boolState[relevant_evt_OF_PropagationShortCircuitTransfo2] && ( !boolState[S_OF_PropagationShortCircuitTransfo2]))) || ( !boolState[S_OF_RO_CB_up_2]) )
	{
		boolState[relevant_evt_OF_Transfo2]  =  true;
	}



	boolState[relevant_evt_OF_UE_1]  =  true  ;

	if ( !boolState[required_OF_LossOfDieselLine] )
	{
		boolState[required_OF_dies_generator]  =  false;
	}

	if (boolState[relevant_evt_OF_LossOfDieselLine] && ( !boolState[S_OF_LossOfDieselLine]) )
	{
		boolState[relevant_evt_OF_dies_generator]  =  true;
	}

}


void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::runOnceInteractionStep_propagate_leaves()
{


	boolState[already_S_OF_AND_1]  =  boolState[S_OF_AND_1]  ;



	boolState[already_S_OF_CB_dies]  =  boolState[S_OF_CB_dies]  ;



	boolState[already_S_OF_CB_dw_1]  =  boolState[S_OF_CB_dw_1]  ;



	boolState[already_S_OF_CB_dw_2]  =  boolState[S_OF_CB_dw_2]  ;



	boolState[already_S_OF_CB_up_1]  =  boolState[S_OF_CB_up_1]  ;



	boolState[already_S_OF_CB_up_2]  =  boolState[S_OF_CB_up_2]  ;



	boolState[already_S_OF_GRID]  =  boolState[S_OF_GRID]  ;



	boolState[already_S_OF_LossOfAllBackups]  =  boolState[S_OF_LossOfAllBackups]  ;



	boolState[already_S_OF_LossOfDieselLine]  =  boolState[S_OF_LossOfDieselLine]  ;



	boolState[already_S_OF_LossOfLine2]  =  boolState[S_OF_LossOfLine2]  ;



	boolState[already_S_OF_LossOfLine_1]  =  boolState[S_OF_LossOfLine_1]  ;



	boolState[already_S_OF_Loss_of_GRID]  =  boolState[S_OF_Loss_of_GRID]  ;



	boolState[already_S_OF_PropagationShortCircuitTransfo1]  =  boolState[S_OF_PropagationShortCircuitTransfo1]  ;



	boolState[already_S_OF_PropagationShortCircuitTransfo2]  =  boolState[S_OF_PropagationShortCircuitTransfo2]  ;



	boolState[already_S_OF_RC_CB_dies]  =  boolState[S_OF_RC_CB_dies]  ;

	if (( !boolState[required_OF_RC_CB_dies]) && (( !boolState[already_standby_OF_RC_CB_dies]) && ( !boolState[already_required_OF_RC_CB_dies])) )
	{
		boolState[already_standby_OF_RC_CB_dies]  =  true;
	}



	boolState[already_S_OF_RC_CB_dw_2]  =  boolState[S_OF_RC_CB_dw_2]  ;

	if (( !boolState[required_OF_RC_CB_dw_2]) && (( !boolState[already_standby_OF_RC_CB_dw_2]) && ( !boolState[already_required_OF_RC_CB_dw_2])) )
	{
		boolState[already_standby_OF_RC_CB_dw_2]  =  true;
	}



	boolState[already_S_OF_RC_CB_up_2]  =  boolState[S_OF_RC_CB_up_2]  ;

	if (( !boolState[required_OF_RC_CB_up_2]) && (( !boolState[already_standby_OF_RC_CB_up_2]) && ( !boolState[already_required_OF_RC_CB_up_2])) )
	{
		boolState[already_standby_OF_RC_CB_up_2]  =  true;
	}



	boolState[already_S_OF_RO_CB_up_1]  =  boolState[S_OF_RO_CB_up_1]  ;

	if (( !boolState[required_OF_RO_CB_up_1]) && (( !boolState[already_standby_OF_RO_CB_up_1]) && ( !boolState[already_required_OF_RO_CB_up_1])) )
	{
		boolState[already_standby_OF_RO_CB_up_1]  =  true;
	}



	boolState[already_S_OF_RO_CB_up_2]  =  boolState[S_OF_RO_CB_up_2]  ;

	if (( !boolState[required_OF_RO_CB_up_2]) && (( !boolState[already_standby_OF_RO_CB_up_2]) && ( !boolState[already_required_OF_RO_CB_up_2])) )
	{
		boolState[already_standby_OF_RO_CB_up_2]  =  true;
	}



	boolState[already_S_OF_RS_dies]  =  boolState[S_OF_RS_dies]  ;

	if (( !boolState[required_OF_RS_dies]) && (( !boolState[already_standby_OF_RS_dies]) && ( !boolState[already_required_OF_RS_dies])) )
	{
		boolState[already_standby_OF_RS_dies]  =  true;
	}



	boolState[already_S_OF_Transfo1]  =  boolState[S_OF_Transfo1]  ;



	boolState[already_S_OF_Transfo2]  =  boolState[S_OF_Transfo2]  ;



	boolState[already_S_OF_UE_1]  =  boolState[S_OF_UE_1]  ;



	boolState[already_S_OF_dies_generator]  =  boolState[S_OF_dies_generator]  ;

}


void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::runOnceInteractionStep_tops()
{
	if (boolState[required_OF_RC_CB_dies] && boolState[already_standby_OF_RC_CB_dies] )
	{
		boolState[to_be_fired_OF_RC_CB_dies]  =  true;
	}

	if (boolState[required_OF_RC_CB_dw_2] && boolState[already_standby_OF_RC_CB_dw_2] )
	{
		boolState[to_be_fired_OF_RC_CB_dw_2]  =  true;
	}

	if (boolState[required_OF_RC_CB_up_2] && boolState[already_standby_OF_RC_CB_up_2] )
	{
		boolState[to_be_fired_OF_RC_CB_up_2]  =  true;
	}

	if (boolState[required_OF_RO_CB_up_1] && boolState[already_standby_OF_RO_CB_up_1] )
	{
		boolState[to_be_fired_OF_RO_CB_up_1]  =  true;
	}

	if (boolState[required_OF_RO_CB_up_2] && boolState[already_standby_OF_RO_CB_up_2] )
	{
		boolState[to_be_fired_OF_RO_CB_up_2]  =  true;
	}

	if (boolState[required_OF_RS_dies] && boolState[already_standby_OF_RS_dies] )
	{
		boolState[to_be_fired_OF_RS_dies]  =  true;
	}

}

void
storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::runInteractions() {
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
    }void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::printstatetuple(){

                std::cout<<"\n State information: (";
                for (int i=0; i<boolFailureState.size(); i++)
                    {
                    std::cout<<boolFailureState.at(i);
                    }
                std::cout<<")";
                
            }
        int_fast64_t storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::stateSize() const{
            return numBoolState;
}
        void storm::figaro::FigaroProgram_BDMP_17_Demoeng_RESS_Electrical_system_with_3_trains_Trim_Article_No_repair::fireinsttransitiongroup(std::string user_input_ins)

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
    