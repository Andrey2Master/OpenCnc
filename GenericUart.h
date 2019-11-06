/*
 * GenericUart.h
 *
 *  Created on: 15.06.2013
 *      Author: rfatkhullin
 */
#include "stm32f10x_conf.h" 
#ifndef GENERICUART_H_
#define GENERICUART_H_

class GenericUart {
public:
	GenericUart();
  GenericUart(USART_TypeDef*,uint32_t);
	virtual ~GenericUart();
uint16_t Available(void);
char Setup(uint32_t speed);
char Read(void);
void Write(uint8_t ch);
void Print(char *str);
void DMA_IRQHandler(void);

private:
#define UART_BUFFER_SIZE    255
    int num;
// Receive buffer, circular DMA
volatile uint8_t rxBuffer[UART_BUFFER_SIZE];
volatile uint8_t txBuffer[UART_BUFFER_SIZE];
uint32_t rxDMAPos;
uint32_t txBufferTail;
uint32_t txBufferHead;
void uartTxDMA(void);
	float time;

 

};
typedef struct {
    GPIO_TypeDef *gpio;
    uint32_t tx_pin;
	  uint32_t rx_pin;
    uint8_t  dma_irq;
	  DMA_Channel_TypeDef *rx_dma_ch;
    DMA_Channel_TypeDef *tx_dma_ch; 	
	  uint32_t y_IT;
} uartHardware_t;
#endif /* GENERICUART_H_ */
