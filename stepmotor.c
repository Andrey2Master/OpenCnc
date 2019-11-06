#include "global.h"
//#include "cnc.h"
#define STEPS_BUF_SZ 8
typedef struct {
	uint32_t steps[4];
	uint32_t pscValue[4], arrValue[4];
	uint8_t dir[4];
} LINE_DATA;

static volatile LINE_DATA steps_buf[STEPS_BUF_SZ];
static int32_t steps_buf_sz, steps_buf_p1, steps_buf_p2;

volatile struct {
	int32_t globalSteps;
	uint32_t steps;
 uint8_t clk, dir, isInProc;
} step_motors[4];

static void stepm_powerOff(uint8_t id) {
	steps_buf_sz = steps_buf_p1 = steps_buf_p2 = 0;
	switch(id) {
	 case 0: M0_EN_PORT->BRR = M0_EN_PIN; break;
	 case 1: M1_EN_PORT->BRR = M1_EN_PIN; break;
	 case 2: M2_EN_PORT->BRR = M2_EN_PIN; break;
	 case 3: M3_EN_PORT->BRR = M3_EN_PIN; break;
	}
}

void stepm_init(void) {
	for(int i = 0; i < 4; i++) {
		step_motors[i].steps = 0;
		step_motors[i].clk = TRUE; step_motors[i].isInProc = FALSE;
		step_motors[i].globalSteps = 0;
	}
	steps_buf_sz = steps_buf_p1 = steps_buf_p2 = 0;

 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Pin = M0_EN_PIN;   GPIO_Init(M0_EN_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M0_DIR_PIN;  GPIO_Init(M0_DIR_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M0_STEP_PIN; GPIO_Init(M0_STEP_PORT, &GPIO_InitStructure);
 stepm_powerOff(0);

 GPIO_InitStructure.GPIO_Pin = M1_EN_PIN;   GPIO_Init(M1_EN_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M1_DIR_PIN;  GPIO_Init(M1_DIR_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M1_STEP_PIN; GPIO_Init(M1_STEP_PORT, &GPIO_InitStructure);
 stepm_powerOff(1);

 GPIO_InitStructure.GPIO_Pin = M2_EN_PIN;   GPIO_Init(M2_EN_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M2_DIR_PIN;  GPIO_Init(M2_DIR_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M2_STEP_PIN; GPIO_Init(M2_STEP_PORT, &GPIO_InitStructure);
 stepm_powerOff(2);

 GPIO_InitStructure.GPIO_Pin = M3_EN_PIN;   GPIO_Init(M3_EN_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M3_DIR_PIN;  GPIO_Init(M3_DIR_PORT, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = M3_STEP_PIN; GPIO_Init(M3_STEP_PORT, &GPIO_InitStructure);
 stepm_powerOff(3);

 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 TIM_TimeBaseStructure.TIM_Prescaler = 1799; // any
 TIM_TimeBaseStructure.TIM_Period = 100; // any
 TIM_TimeBaseStructure.TIM_ClockDivision = 0;
 TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

 NVIC_InitTypeDef NVIC_InitStructure;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;

 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
 // TIM2 - motor 0
 NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 NVIC_Init(&NVIC_InitStructure);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
 TIM2->EGR = TIM_PSCReloadMode_Update; TIM_ARRPreloadConfig(TIM2, ENABLE);
 TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); TIM_Cmd(TIM2, ENABLE);
 // TIM3 - motor 1
 NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
 NVIC_Init(&NVIC_InitStructure);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
 TIM3->EGR = TIM_PSCReloadMode_Update; TIM_ARRPreloadConfig(TIM3, ENABLE);
 TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); TIM_Cmd(TIM3, ENABLE);
 // TIM4 - motor 2
 NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
 NVIC_Init(&NVIC_InitStructure);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
 TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
 TIM4->EGR = TIM_PSCReloadMode_Update; TIM_ARRPreloadConfig(TIM4, ENABLE);
 TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); TIM_Cmd(TIM4, ENABLE);
/*
// TIM5 - motor 3
 NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
 NVIC_Init(&NVIC_InitStructure);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
 TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
 TIM5->EGR = TIM_PSCReloadMode_Update; TIM_ARRPreloadConfig(TIM5, ENABLE);
 TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE); TIM_Cmd(TIM5, ENABLE);
*/
}

void stepm_proc(uint8_t id) {
	//if(limits_chk()) {	stepm_EmergeStop(); return;	}
	if(step_motors[id].isInProc) {
  switch(id) {
   case 0:
   	if(step_motors[id].clk) M0_STEP_PORT->BSRR = M0_STEP_PIN;
   	else M0_STEP_PORT->BRR = M0_STEP_PIN;
   	break;
   case 1:
   	if(step_motors[id].clk) M1_STEP_PORT->BSRR = M1_STEP_PIN;
   	else M1_STEP_PORT->BRR = M1_STEP_PIN;
   	break;
   case 2:
   	if(step_motors[id].clk) M2_STEP_PORT->BSRR = M2_STEP_PIN;
   	else M2_STEP_PORT->BRR = M2_STEP_PIN;
   	break;
   case 3:
   	if(step_motors[id].clk) M3_STEP_PORT->BSRR = M3_STEP_PIN;
   	else M3_STEP_PORT->BRR = M3_STEP_PIN;
   	break;
  }
  if(!step_motors[id].clk) {
 		if(step_motors[id].steps!=0) step_motors[id].steps--;
  	if(step_motors[id].dir) step_motors[id].globalSteps++;
  	else	step_motors[id].globalSteps--;
  } else {
  	if(step_motors[id].steps == 0) step_motors[id].isInProc = FALSE;
  }
  step_motors[id].clk = !step_motors[id].clk;
	} else {
		if(steps_buf_sz > 0 && !step_motors[0].isInProc && !step_motors[1].isInProc &&
				 !step_motors[2].isInProc && !step_motors[3].isInProc) {
	 	__disable_irq();
			LINE_DATA *p = (LINE_DATA *)(&steps_buf[steps_buf_p1]);
	 	for(int i = 0; i < 4; i++)	{ step_motors[i].steps = p->steps[i]; step_motors[i].dir = p->dir[i]; }
	 	if(step_motors[0].steps) {
	 		step_motors[0].isInProc = TRUE;
	 	 GPIO_WriteBit(M0_DIR_PORT, M0_DIR_PIN, p->dir[0]? Bit_SET:Bit_RESET);	M0_EN_PORT->BSRR = M0_EN_PIN;
	 	 TIM2->PSC = p->pscValue[0];	TIM_SetAutoreload(TIM2, p->arrValue[0]);
	 	}
	 	if(step_motors[1].steps) {
	 		step_motors[1].isInProc = TRUE;
 	 	GPIO_WriteBit(M1_DIR_PORT, M1_DIR_PIN, p->dir[1]? Bit_SET:Bit_RESET);	M1_EN_PORT->BSRR = M1_EN_PIN;
 	 	TIM3->PSC = p->pscValue[1];	TIM_SetAutoreload(TIM3, p->arrValue[1]);
	 	}
	 	if(step_motors[2].steps) {
	 		step_motors[2].isInProc = TRUE;
	 	 GPIO_WriteBit(M2_DIR_PORT, M2_DIR_PIN, p->dir[2]? Bit_RESET:Bit_SET);	M2_EN_PORT->BSRR = M2_EN_PIN;
	 	 TIM4->PSC = p->pscValue[2];	TIM_SetAutoreload(TIM4, p->arrValue[2]);
	 	}
	 	if(step_motors[3].steps) {
	 		step_motors[3].isInProc = TRUE;
	 	 GPIO_WriteBit(M3_DIR_PORT, M3_DIR_PIN, p->dir[3]? Bit_RESET:Bit_SET);	M3_EN_PORT->BSRR = M3_EN_PIN;
	 	 TIM5->PSC = p->pscValue[3];	TIM_SetAutoreload(TIM5, p->arrValue[3]);
	 	}
			steps_buf_p1++;
			if(steps_buf_p1 >= STEPS_BUF_SZ) steps_buf_p1 = 0;
			steps_buf_sz--;
	 	__enable_irq();
		}
	}
}

void stepm_EmergeStop(void) {
	for(int i = 0; i < 4; i++) {
		stepm_powerOff(i);
		step_motors[i].isInProc = FALSE;	step_motors[i].steps = 0;
	}
	steps_buf_sz = steps_buf_p1 = steps_buf_p2 = 0;
}

void stepm_addMove(uint32_t steps[4], uint32_t frq[4], uint8_t dir[4]) {
	if(steps[0] == 0 && steps[1] == 0 && steps[2] == 0 && steps[3] == 0) return;
	while(steps_buf_sz >= STEPS_BUF_SZ) __NOP();

	LINE_DATA *p = (LINE_DATA *)(&steps_buf[steps_buf_p2]);

	for(int i = 0; i < 4;i++) {
		uint32_t f = frq[i];
	 if(f > (15000*K_FRQ)) f = 15000*K_FRQ; //15kHz
	 if(f == 0) f = 1; //1Hz
	 // 72Mhz/(psc*arr) = frq
	 uint32_t pscValue=1;
	 uint32_t arrValue=(72000000L*K_FRQ/2)/f; // (1 falling age on 2 IRQ)
	 while(arrValue > 0x0ffff) { pscValue = pscValue << 1; arrValue = arrValue >> 1; }
	 pscValue--;
	 p->arrValue[i] = arrValue;
	 p->pscValue[i] = pscValue;
	 p->dir[i] = dir[i]; p->steps[i] = steps[i];
	}
	steps_buf_p2++;
	if(steps_buf_p2 >= STEPS_BUF_SZ) steps_buf_p2 = 0;
	__disable_irq(); 
	steps_buf_sz++; 
	__enable_irq();
}

int32_t stepm_getRemainLines(void) {
	return steps_buf_sz;
}

int32_t stepm_inProc(void) {
	if(steps_buf_sz > 0) return TRUE;
	for(int i = 0; i < 4; i++)
  if(step_motors[i].isInProc) return TRUE;
	return FALSE;
}

uint32_t stepm_LinesBufferIsFull(void) {
	return steps_buf_sz >= STEPS_BUF_SZ;
}

int32_t stepm_getCurGlobalStepsNum(uint8_t id) {
	return step_motors[id].globalSteps;
}

void stepm_ZeroGlobalCrd(void) {
	for(int i = 0; i < 4; i++) {
		step_motors[i].globalSteps = 0;
	}
}
