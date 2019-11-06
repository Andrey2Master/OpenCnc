#ifndef STEPMOTOR_H_
#define STEPMOTOR_H_
#include "stm32f10x.h"
#include "stm32f10x_conf.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "global.h"
//------- stepmotor -------------------------------------
// 74hc14 - inverter on the step motors board. STEP on falling edge
//  _______        ______
//         x      |
//         |______|
#define M0_EN_PORT GPIOA
#define M0_EN_PIN GPIO_Pin_1
#define M0_DIR_PORT GPIOA
#define M0_DIR_PIN GPIO_Pin_2
#define M0_STEP_PORT GPIOA
#define M0_STEP_PIN GPIO_Pin_8

#define M1_EN_PORT GPIOE
#define M1_EN_PIN GPIO_Pin_0
#define M1_DIR_PORT GPIOB
#define M1_DIR_PIN GPIO_Pin_9
#define M1_STEP_PORT GPIOB
#define M1_STEP_PIN GPIO_Pin_1

#define M2_EN_PORT GPIOB
#define M2_EN_PIN GPIO_Pin_11
#define M2_DIR_PORT GPIOC
#define M2_DIR_PIN GPIO_Pin_7
#define M2_STEP_PORT GPIOD
#define M2_STEP_PIN GPIO_Pin_12

#define M3_EN_PORT GPIOB
#define M3_EN_PIN GPIO_Pin_11
#define M3_DIR_PORT GPIOD
#define M3_DIR_PIN GPIO_Pin_6
#define M3_STEP_PORT GPIOB
#define M3_STEP_PIN GPIO_Pin_10

void stepm_init(void);
void stepm_EmergeStop(void);
extern uint8_t stepmPause;


void stepm_addMove(uint32_t steps[], uint32_t frq[], uint8_t dir[]);

uint32_t stepm_LinesBufferIsFull(void);
int32_t stepm_getRemainLines(void);
void stepm_proc(uint8_t id);
int32_t stepm_getCurGlobalStepsNum(uint8_t id);
void stepm_ZeroGlobalCrd(void);
int32_t stepm_inProc(void);

#endif /* STEPMOTOR_H_ */
