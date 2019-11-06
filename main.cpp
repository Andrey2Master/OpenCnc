#include "global.h"
#include "GenericUart.h"
#include "../GCS/GenericGCS.h"
#include "cnc.h"
#include "stepmotor.h"
extern struct tCNC_DATA CNC_DATA[];
GenericGCS  *GCS;

	uint32_t frq[4] = {SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ,SM_MANUAL_MODE_STEPS_PER_SEC*20,
			                 SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ,SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ};
	uint8_t dir[4] = {1,1,0,0};
	uint32_t steps[4] = {9000,9000,0,0};


int main ()
{
	_smParam.smoothStartF_from0[0] = SM_SMOOTH_START_X*K_FRQ;
	_smParam.smoothStartF_from0[1] = SM_SMOOTH_START_Y*K_FRQ;
	_smParam.smoothStartF_from0[2] = SM_SMOOTH_START_Z*K_FRQ;
	_smParam.smoothStopF_to0[0] = SM_SMOOTH_STOP_X*K_FRQ;
	_smParam.smoothStopF_to0[1] = SM_SMOOTH_STOP_Y*K_FRQ;
	_smParam.smoothStopF_to0[2] = SM_SMOOTH_STOP_Z*K_FRQ;
	_smParam.smoothAF[0] = SM_SMOOTH_DFEED_X*SM_X_STEPS_PER_MM*SM_SMOOTH_TFEED*K_FRQ / 1000;
	_smParam.smoothAF[1] = SM_SMOOTH_DFEED_Y*SM_Y_STEPS_PER_MM*SM_SMOOTH_TFEED*K_FRQ / 1000;
	_smParam.smoothAF[2] = SM_SMOOTH_DFEED_Z*SM_Z_STEPS_PER_MM*SM_SMOOTH_TFEED / 1000 * K_FRQ;
	_smParam.maxFeedRate[0] = SM_X_MAX_STEPS_PER_SEC*K_FRQ;
	_smParam.maxFeedRate[1] = SM_Y_MAX_STEPS_PER_SEC*K_FRQ;
	_smParam.maxFeedRate[2] = SM_Z_MAX_STEPS_PER_SEC*K_FRQ;
	_smParam.maxSpindleTemperature = MAX_SPINDEL_TEMPERATURE;	
	
	
	
	
	
	
	
	
	
	
	
 SystemStartup();
 stepm_init();
 GenericUart Port (USART1,115200);	
 //LED0_ON
 Port.Print("Run 1\r\n");	
 GenericGCS Tgcs(&Port);	
 GCS=&Tgcs;	
// GenericGCS  GCS(&Port);	
// stepm_addMove(steps,frq,dir);
	
	//stepm_EmergeStop(); 	
	while (1)
	{
	GCS->update();	
//	GCS.update();
	//runProg();	
	}
}