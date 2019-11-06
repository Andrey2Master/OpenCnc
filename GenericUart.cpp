/*
 * GenericUart.cpp
 *
 *  Created on: 15.06.2013
 *      Author: rfatkhullin
 */

#include "GenericUart.h"
static uartHardware_t uartHardware[] = {
     //                                   TX                RX
    {GPIOA,GPIO_Pin_9,GPIO_Pin_10, DMA1_Channel4_IRQn,DMA1_Channel5,DMA1_Channel4,DMA1_IT_TC4},          // UART1
    {GPIOA,GPIO_Pin_2,GPIO_Pin_3,  DMA1_Channel7_IRQn,DMA1_Channel6,DMA1_Channel7,DMA1_IT_TC7},          // UART2
//		{GPIOC,GPIO_Pin_10,GPIO_Pin_11, DMA2_Channel4_5_IRQn,DMA2_Channel3,DMA2_Channel5,DMA2_IT_TC5},          // UART4
};
GenericUart *ptrUart1=0,*ptrUart2=0,*ptrUart3=0,*ptrUart4=0;

GenericUart::GenericUart() 
{


}
GenericUart::GenericUart(USART_TypeDef* USARTx,uint32_t speed) 
{



   	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
		rxDMAPos = 0;
		txBufferTail = 0;
		txBufferHead = 0; 
	   
	   if(USARTx==USART1)
		 {
     num=0; 
     ptrUart1=this;
  	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
     }
 	   if(USARTx==USART2)
		 {
      num=1; 
      ptrUart2=this;
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);    
     }
 	   if(USARTx==UART4)
		 {
      num=2; 
      ptrUart4=this;
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);    
     }
		GPIO_InitStructure.GPIO_Pin =uartHardware[num].tx_pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(uartHardware[num].gpio, &GPIO_InitStructure);
		 
    GPIO_InitStructure.GPIO_Pin =uartHardware[num].rx_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(uartHardware[num].gpio, &GPIO_InitStructure);
	   
	    // DMA TX Interrupt
    NVIC_InitStructure.NVIC_IRQChannel =uartHardware[num].dma_irq;// DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		 
    // Receive DMA into a circular buffer
    DMA_DeInit(uartHardware[num].rx_dma_ch);
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USARTx->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_BufferSize = UART_BUFFER_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init(uartHardware[num].rx_dma_ch, &DMA_InitStructure);

    DMA_Cmd(uartHardware[num].rx_dma_ch, ENABLE);
    USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);
    rxDMAPos = DMA_GetCurrDataCounter(uartHardware[num].rx_dma_ch);	
	 
	     // Transmit DMA
    DMA_DeInit(uartHardware[num].tx_dma_ch);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USARTx->DR;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_Init(uartHardware[num].tx_dma_ch, &DMA_InitStructure);
    DMA_ITConfig(uartHardware[num].tx_dma_ch, DMA_IT_TC, ENABLE);
    uartHardware[num].tx_dma_ch->CNDTR = 0;
    USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
		
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = speed;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USARTx, &USART_InitStructure);
    USART_Cmd(USARTx, ENABLE);

}

GenericUart::~GenericUart() {
	// TODO Auto-generated destructor stub
}
char GenericUart::Read(void) 
{
    uint8_t ch;

    ch = rxBuffer[UART_BUFFER_SIZE - rxDMAPos];
    // go back around the buffer
    if (--rxDMAPos == 0)
        rxDMAPos = UART_BUFFER_SIZE;

    return ch;
}
char GenericUart::Setup(uint32_t speed)
{
    USART_InitTypeDef USART_InitStructure;
    USART_TypeDef* USARTx;
	  if(num==0)USARTx=USART1;
	  if(num==1)USARTx=USART2;
	  if(num==2)USARTx=UART4;
	
	  USART_StructInit(&USART_InitStructure);
    USART_Cmd(USARTx, DISABLE);
    USART_InitStructure.USART_BaudRate = speed;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USARTx, &USART_InitStructure);
    USART_Cmd(USARTx, ENABLE);
	
	return 1;
}	
void GenericUart::DMA_IRQHandler(void)
{
   DMA_ClearITPendingBit(uartHardware[num].y_IT);
    DMA_Cmd(uartHardware[num].tx_dma_ch, DISABLE);

    if (txBufferHead != txBufferTail)
        uartTxDMA();


}

 void GenericUart::uartTxDMA(void)
{
    uartHardware[num].tx_dma_ch->CMAR = (uint32_t)&txBuffer[txBufferTail];
    if (txBufferHead > txBufferTail) {
        uartHardware[num].tx_dma_ch->CNDTR = txBufferHead - txBufferTail;
        txBufferTail = txBufferHead;
    } else {
        uartHardware[num].tx_dma_ch->CNDTR = UART_BUFFER_SIZE - txBufferTail;
        txBufferTail = 0;
    }

    DMA_Cmd(uartHardware[num].tx_dma_ch, ENABLE);
}
uint16_t GenericUart::Available(void)
{
    return (DMA_GetCurrDataCounter(uartHardware[num].rx_dma_ch) != rxDMAPos) ? true : false;
}
void GenericUart::Write(uint8_t ch)
{
    txBuffer[txBufferHead] = ch;
    txBufferHead = (txBufferHead + 1) % UART_BUFFER_SIZE;

    // if DMA wasn't enabled, fire it up
    if (!(uartHardware[num].tx_dma_ch->CCR & 1))
        uartTxDMA();
}

void GenericUart::Print(char *str)
{
    while (*str)
        Write(*(str++));
}

 extern "C" {
void DMA1_Channel4_IRQHandler(void)
{
if(ptrUart1)ptrUart1->DMA_IRQHandler();
}
void DMA1_Channel7_IRQHandler(void)
{
if(ptrUart2)ptrUart2->DMA_IRQHandler();
}
void DMA2_Channel4_5_IRQHandler(void)
{
if(ptrUart4)ptrUart4->DMA_IRQHandler();
}
}
