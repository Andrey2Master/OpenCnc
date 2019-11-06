/*
 * GenericUart.cpp
 *
 *  Created on: 15.06.2013
 *      Author: rfatkhullin
 */

#include "GenericGCS.h"
#include "parametr.h"
#include "global.h"
#include "cnc.h"
#include "stepmotor.h"
//#include "main.h"

extern struct tCNC_DATA CNC_DATA[];
unsigned int CNC_POS;

GenericGCS::GenericGCS(GenericUart *_port) 
{
port=_port;
port->Print("GrounControlSystemInit...\r");
c_state = IDLE;	
}

GenericGCS::~GenericGCS() {
	// TODO Auto-generated destructor stub
}
void GenericGCS::out_float(float res)
{
char *ptr;
static uint8_t t;
ptr=(char*)&res;
	t = *ptr++;
	port->Write(t);
	checksum ^= t;
	t = *ptr++;
	port->Write(t);
	checksum ^= t;
	t = *ptr++;
	port->Write(t);
	checksum ^= t;
	t = *ptr++;
	port->Write(t);
	checksum ^= t;
}
void GenericGCS::out32(uint32_t a)
{
    static uint8_t t;
    t = a;
    port->Write(t);
    checksum ^= t;
    t = a >> 8;
    port->Write(t);
    checksum ^= t;
    t = a >> 16;
    port->Write(t);
    checksum ^= t;
    t = a >> 24;
    port->Write(t);
    checksum ^= t;

}
void GenericGCS::out16(uint16_t a)
{
 uint8_t t;
    t = a;
    port->Write(t);
    checksum ^= t;
    t = a >> 8 & 0xff;
    port->Write(t);
    checksum ^= t;
}
void GenericGCS::out8(uint8_t a)
{
    port->Write(a);
    checksum ^= a;
}
uint8_t GenericGCS::read8(void)
{
    return inBuf[indRX++] & 0xff;
}
uint16_t GenericGCS::read16(void)
{
    uint16_t t = read8();
    t += (uint16_t) read8() << 8;
    return t;
}
uint32_t GenericGCS::read32(void)
{
    uint32_t t = read16();
    t += (uint32_t) read16() << 16;
    return t;
}
float 	GenericGCS :: read_float(void)
{
float res;
char *ptr;	
ptr=(char*)&res;
ptr[0]=inBuf[indRX++] & 0xff;
ptr[1]=inBuf[indRX++] & 0xff;
ptr[2]=inBuf[indRX++] & 0xff;
ptr[3]=inBuf[indRX++] & 0xff;
return res;
}
void GenericGCS::update(void)
{
    uint8_t c;
    while (port->Available()) {
        c =port->Read();

        if (c_state == IDLE) {
            c_state = (c == '$') ? HEADER_START : IDLE;
            if (c_state == IDLE)
                evaluateOtherData(c); // evaluate all other incoming serial data
        } else if (c_state == HEADER_START) {
            c_state = (c == 'M') ? HEADER_M : IDLE;
        } else if (c_state == HEADER_M) {
            c_state = (c == '<') ? HEADER_ARROW : IDLE;
        } else if (c_state == HEADER_ARROW) {
            if (c > INBUF_SIZE) {       // now we are expecting the payload size
                c_state = IDLE;
                continue;
            }
            dataSize = c;
            offset = 0;
            checksum = 0;
            indRX = 0;
            checksum ^= c;
            c_state = HEADER_SIZE;      // the command is to follow
            guiConnected = true;
        } else if (c_state == HEADER_SIZE) {
            cmdMSP = c;
            checksum ^= c;
            c_state = HEADER_CMD;
        } else if (c_state == HEADER_CMD && offset < dataSize) {
            checksum ^= c;
            inBuf[offset++] = c;
        } else if (c_state == HEADER_CMD && offset >= dataSize) {
            if (checksum == c) {        // compare calculated and transferred checksum
                evaluateCommand();      // we got a valid packet, evaluate it
            }
            c_state = IDLE;
        }
    }

}
void GenericGCS::evaluateCommand(void)
{
uint16_t i;
int d[3];
char *ptr;
float temp_f[3];
	long temp;
switch (cmdMSP)
	{
    case GCS_RELOAD_CNC:
        CNC_POS=0;
        headGcsReply(0);
        break;
		case GCS_SET_CNC:

				 for (i = 0; i < dataSize/8; i++)
							{
							CNC_DATA[CNC_POS].X=read32();
							CNC_DATA[CNC_POS].Y=read32();
							CNC_POS++;
							CNC_DATA[CNC_POS].X=-1;
							CNC_DATA[CNC_POS].Y=-1;
							}
	
		
		headGcsReply(0);	
		break;
		case 	GCS_CNC_GET_STP:
        headGcsReply(8);
				out32(stepm_getCurGlobalStepsNum(2));
		    out32(stepm_getCurGlobalStepsNum(3));
    break;
		
    case 	GCS_CNC_RUN:
				  runProg();		
		break;
		case GCS_GET_CONECT:
		headGcsReply(0);	
		break;
		 
	}
	
	

	 
	 tailGcsReply();
}
void GenericGCS::evaluateOtherData(uint8_t sr)
{
   
}
void GenericGCS :: SendDebug()
{
//cmdMSP=GCS_DEBUG;
evaluateCommand();
}
void GenericGCS :: SendFDebug()
{
//cmdMSP=GCS_FLOAT_DEBUG;
evaluateCommand();
}