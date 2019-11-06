#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#define TRUE 1
#define FALSE 0


#define K_FRQ 10




#define digitalHi(p, i)     { p->BSRR = i; }
#define digitalLo(p, i)     { p->BRR = i; }
#define digitalToggle(p, i) { p->ODR ^= i; }

#define LED0_GPIO   GPIOB
#define LED0_PIN    GPIO_Pin_8

// Helpful macros
#define LED0_TOGGLE              digitalToggle(LED0_GPIO, LED0_PIN);
#define LED0_OFF                 digitalHi(LED0_GPIO, LED0_PIN);
#define LED0_ON                  digitalLo(LED0_GPIO, LED0_PIN);

/*----- keyboard ----------------------------------------
*  col: PE2, PE3, PE4, PE5
*  row: PE6, PC1, PC2, PC3
* ------- stepmotor -------------------------------------
*    EN    DIR   STEP
* 0: PA1   PA2   PA3
* 1: PE0   PB9   PB1
* 2: PB11  PC7   PD12
* 3: PB11  PD6   PB10
* ----- extruder ---------------------------------------
* SPI2: PB13-SCK, PB14-MISO, PB15-MOSI  - thermometer
* TIM8_CH1: PC6 - heater PWM,  PB12 - FUN ON/OFF
* ----- position switch ----------------------------------
*  X,Y,Z: PA0,PB8,PD3
* ---- sensor ------------------------
* PA8
*/

//---- sensor ------------
#define SENSOR_PORT GPIOA
#define SENSOR_PIN  GPIO_Pin_8

//----- limit switch ----------------------------------
#define XPORT GPIOA
#define XPIN GPIO_Pin_0
#define ZPORT GPIOB
#define ZPIN GPIO_Pin_8
#define YPORT GPIOD
#define YPIN GPIO_Pin_3



//----- keyboard ----------------------------------------
///   0: 1: 2: 3:
//0:  1  2  3  A
//1:  4  5  6  B
//2:  7  8  9  C
//3:  *  0  #  D
#define COL0_PORT GPIOE
#define COL0_PIN GPIO_Pin_2
#define COL1_PORT GPIOE
#define COL1_PIN GPIO_Pin_3
#define COL2_PORT GPIOE
#define COL2_PIN GPIO_Pin_4
#define COL3_PORT GPIOE
#define COL3_PIN GPIO_Pin_5

#define ROW0_PORT GPIOE
#define ROW0_PIN GPIO_Pin_6
#define ROW1_PORT GPIOC
#define ROW1_PIN GPIO_Pin_1
#define ROW2_PORT GPIOC
#define ROW2_PIN GPIO_Pin_2
#define ROW3_PORT GPIOC
#define ROW3_PIN GPIO_Pin_3

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

//#include "stm32f10x_rtc.h"
//#include "stm32f10x_spi.h"





#ifdef __cplusplus
 extern "C" {
#endif
#include "hw_config.h"
//#include "scr_io.h"
//#include "rtc.h"
//#include "ff.h"
//#include "sdcard.h"
//#include "flash_spi.h"
//#include "rs232_interface.h"
//#include "keyboard.h"

//#include "stepmotor.h"
//#include "cnc.h"
//#include "extruder_t.h"
//#include "limits.h"
//#include "scan.h"
//#include "adjust.h"


#ifdef __cplusplus
}
#endif











void delayMs(uint32_t msec);
char *str_trim(char *str);
uint8_t questionYesNo(char *msg, char *param);

void manualMode(void);

/*
typedef struct {
	uint16_t magicValue;
	double startX;
	double startY;
	double startZ;
	double extruder_k;
} FLASH_VALUES;

extern FLASH_VALUES commonValues;
*/

void showCriticalStatus(char *msg, int st);
uint16_t calcColor(uint8_t val);

//#define DEBUG_MODE
#ifdef DEBUG_MODE
 #define DBG(...) { rf_printf(__VA_ARGS__); }
#else
 #define DBG(...) {}
#endif

#endif /* GLOBAL_H_ */
