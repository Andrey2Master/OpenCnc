#include "global.h"
#include "core_cm3.h"
#include "stepmotor.h"
//#include "mass_mal.h"

ErrorStatus HSEStartUpStatus ;
RCC_ClocksTypeDef RCC_Clocks ;

#ifdef  __cplusplus
extern "C" {
#endif
uint32_t GetCpuClock()
  {
    return RCC_Clocks.SYSCLK_Frequency ;
  }
#ifdef  __cplusplus
  }
#endif

void SystemStartup(void) {
	 GPIO_InitTypeDef GPIO_InitStructure;
 /* Unlock the internal flash */
 FLASH_Unlock();

 /* RCC system reset(for debug purpose) */
 RCC_DeInit();

 /* Enable HSE */
 RCC_HSEConfig(RCC_HSE_ON);

 /* Wait till HSE is ready */
 HSEStartUpStatus = RCC_WaitForHSEStartUp();

 if (HSEStartUpStatus == SUCCESS) {
   FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);/* Enable Prefetch Buffer */
   FLASH_SetLatency(FLASH_Latency_2);/* Flash 2 wait state */
   RCC_HCLKConfig(RCC_SYSCLK_Div1);/* HCLK = SYSCLK */
   RCC_PCLK2Config(RCC_HCLK_Div1);/* PCLK2 = HCLK */
   RCC_PCLK1Config(RCC_HCLK_Div2); /* PCLK1 = HCLK/2 */
   RCC_ADCCLKConfig(RCC_PCLK2_Div6);     //ADCCLK = PCLK2/6 = 12MHz
   RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );/* PLLCLK = 8MHz * 9 = 72 MHz */
   RCC_PLLCmd(ENABLE); /* Enable PLL */
   /* Wait till PLL is ready */
   while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);/* Select PLL as system clock source */

   /* Wait till PLL is used as system clock source */
   while (RCC_GetSYSCLKSource() != 0x08);
 }
 RCC_GetClocksFreq( &RCC_Clocks ) ;
 SysTick_Config(SystemCoreClock / 1000);
 //-------------  GPIO  configure ----------
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);

 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 GPIO_Init(GPIOB, &GPIO_InitStructure); // LED on board
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; // LCD LIGHT
 GPIO_Init(GPIOD, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
 GPIO_Init(GPIOB, &GPIO_InitStructure); // KEY on board

 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
 RCC_ClearFlag();
 //SST25_flashInit();
 //rs232_init();
 //kbd_init();

 //extrudT_init();

 //limits_init();
 //rtc_init();
 //Lcd_Configuration(); ili9320_Initializtion(); ili9320_Clear(0); Lcd_Light_ON;

 //win_showMsgWin(); delayMs(500);
 //scr_puts("   ---- CNC MM V1.0 -----");
 //MAL_Init(0);


}


