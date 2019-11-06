#include "parametr.h"
#include <string.h>
#ifndef FLASH_PAGE_COUNT
#define FLASH_PAGE_COUNT 128
#endif
static uint8_t EEPROM_CONF_VERSION = 34;
#define FLASH_PAGE_SIZE                 ((uint16_t)0x400)
#define FLASH_WRITE_ADDR                (0x08000000 + (uint32_t)FLASH_PAGE_SIZE * (FLASH_PAGE_COUNT - 1))       // use the last KB for storage
#define C_FLASH_WRITE_ADDR                (0x08000000 + (uint32_t)FLASH_PAGE_SIZE * (FLASH_PAGE_COUNT - 2))       // use the last KB for storage
param_t param;

void readEEPROM(void)
{
    uint8_t i;
	
    // check the EEPROM integrity before resetting values
    if (!validEEPROM())resetConf();
	    // Read flash
    memcpy(&param, (char *)FLASH_WRITE_ADDR, sizeof(param_t));
}
uint8_t validEEPROM(void)
{
    const param_t *temp = (const param_t *)FLASH_WRITE_ADDR;
    const uint8_t *p;
    uint8_t chk = 0;

    // check version number
    if (EEPROM_CONF_VERSION != temp->version)
        return 0;

    // check size and magic numbers
    if (temp->size != sizeof(param_t) || temp->magic_be != 0xBE || temp->magic_ef != 0xEF)
        return 0;

    // verify integrity of temporary copy
    for (p = (const uint8_t *)temp; p < ((const uint8_t *)temp + sizeof(param_t)); p++)
        chk ^= *p;

    // checksum failed
    if (chk != 0)
        return 0;

    // looks good, let's roll!
    return 1;
}
void writeParams(uint8_t b)
{
    FLASH_Status status;
    uint32_t i;
    uint8_t chk = 0;
    const uint8_t *p;

    param.version = EEPROM_CONF_VERSION;
    param.size = sizeof(param_t);
    param.magic_be = 0xBE;
    param.magic_ef = 0xEF;
    param.chk = 0;
    // recalculate checksum before writing
    for (p = (const uint8_t *)&param; p < ((const uint8_t *)&param + sizeof(param_t)); p++)
        chk ^= *p;
    param.chk = chk;

    // write it
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    if (FLASH_ErasePage(FLASH_WRITE_ADDR) == FLASH_COMPLETE) {
        for (i = 0; i < sizeof(param_t); i += 4) {
            status = FLASH_ProgramWord(FLASH_WRITE_ADDR + i, *(uint32_t *) ((char *) &param + i));

            if (status != FLASH_COMPLETE)
                break;          // TODO: fail
        }
    }
    FLASH_Lock();

    readEEPROM();
   // if (b)
    //    blinkLED(15, 20, 1);
}
// Default settings
void resetConf(void)
{
    int i;
    memset(&param, 0, sizeof(param_t));
    param.version = EEPROM_CONF_VERSION;

			
    writeParams(0);
}
