#ifndef Param_h
#define Param_h
#include "stm32f10x.h"
uint8_t validEEPROM(void);
void resetConf(void);
typedef struct param_t
{
	uint8_t magic_be;                       // magic number, should be 0xBE
  uint8_t version;
  uint16_t size;

	
	
	
  uint8_t magic_ef;                       // magic number, should be 0xEF
  uint8_t chk;                            // XOR checksum
}param_t;

#endif 
