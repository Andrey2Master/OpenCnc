/*
 * GenericUart.h
 *
 *  Created on: 15.06.2013
 *      Author: rfatkhullin
 */
#define GCS_VERSION              0
#define PLATFORM_32BIT           0x80000000

#define GCS_IDENT                100    //out message         multitype + version
#define GCS_STATUS               101    //out message         cycletime & errors_count & sensor present & box activation
#define GCS_GET_CONECT           110

#define GCS_RELOAD_CNC           120
#define GCS_CNC_GET_STP			 		 121
#define GCS_CNC_RUN							 122
#define GCS_CNC_STOP  			     123
#define GCS_CNC_HOME_X  		     124
#define GCS_CNC_HOME_Y  		     125


#define GCS_SET_RUN_SPEED        130
#define GCS_SET_JOG_SPEED        131
#define GCS_SET_CNC              132



#define GCS_PARAM   			 244
#define GCS_SET_PARAM  			 245

#define INBUF_SIZE 240
#ifndef GENERICGCS_H_
#define GENERICGCS_H_
#include "stm32f10x.h"
#include "stm32f10x_conf.h" 
//#include "mpu6050_drv.h"
//#include "GenericDCM.h"
#include "GenericUart.h"


class GenericGCS {
enum _serial_state {
        IDLE,
        HEADER_START,
        HEADER_M,
        HEADER_ARROW,
        HEADER_SIZE,
        HEADER_CMD,
    } c_state;// = IDLE;	
public:
	GenericGCS(GenericUart *);
  GenericGCS(int);
	virtual ~GenericGCS();

void update(void);
void SendDebug(void);
void SendFDebug(void);
private:
GenericUart *port;
uint8_t checksum, indRX, inBuf[INBUF_SIZE];
uint8_t cmdMSP;
bool guiConnected;
uint8_t offset;
uint8_t dataSize;
void out_float(float res);
void out32(uint32_t);
void out16(uint16_t);
void out8(uint8_t);
uint8_t read8(void);
uint16_t read16(void);
uint32_t read32(void);
float read_float(void);
void headGcsResponse (uint8_t err, uint8_t s)
{
    out8('$');
    out8('M');
    out8(err ? '!' : '>');
    checksum = 0;               // start calculating a new checksum
    out8(s);
    out8(cmdMSP);
}
void headGcsReply(uint8_t s)
{
    headGcsResponse(0, s);
}
void headGcsError(uint8_t s)
{
    headGcsResponse(1, s);
}
void tailGcsReply(void)
{
    out8(checksum);
}


void evaluateCommand(void);
void evaluateOtherData(uint8_t sr);

	float time;
};
#endif /* GENERICGCS_H_ */
