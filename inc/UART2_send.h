#ifndef UART2_SEND_H
#define UART2_SEND_H

#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

static PORT_InitTypeDef PortInit;
static UART_InitTypeDef UART_InitStructure;

static void UART2SendData(uint8_t* data, unsigned short int length) {

	uint16_t dataByte;
  for(unsigned short int i = 0; i < length; i += 2) {
		
		dataByte = *(data + i);

    /* Check TXFE flag */
    while (UART_GetFlagStatus (MDR_UART2, UART_FLAG_TXFE)!= SET)
    {
    }

    /* Send Data from UART2 */
    UART_SendData (MDR_UART2, dataByte);
  }

}

void initUART2 (void)
{

  /* Enables the HSI clock on PORTB,PORTD */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD,ENABLE);

  /* Fill PortInit structure*/
  PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
  PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
  PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
  PortInit.PORT_PD = PORT_PD_DRIVER;
  PortInit.PORT_GFEN = PORT_GFEN_OFF;
  PortInit.PORT_FUNC = PORT_FUNC_ALTER;
  PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;

  /* Configure PORTD pins 1 (UART2_TX) as output */
  PortInit.PORT_OE = PORT_OE_OUT;
  PortInit.PORT_Pin = PORT_Pin_1;
  PORT_Init(MDR_PORTD, &PortInit);

  /* Configure PORTD pins 0 (UART1_RX) as input */
  PortInit.PORT_OE = PORT_OE_IN;
  PortInit.PORT_Pin = PORT_Pin_0;
  PORT_Init(MDR_PORTD, &PortInit);

  RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);

  /* Set the HCLK division factor = 1 for UART1,UART2*/
  UART_BRGInit(MDR_UART2, UART_HCLKdiv1);

  /* Initialize UART_InitStructure */
  UART_InitStructure.UART_BaudRate                = 9600;
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;
  UART_InitStructure.UART_StopBits                = UART_StopBits2;
  UART_InitStructure.UART_Parity                  = UART_Parity_Even;
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

  /* Configure UART2 parameters*/
  UART_Init (MDR_UART2,&UART_InitStructure);

  /* Enables UART2 peripheral */
  UART_Cmd(MDR_UART2,ENABLE);
}

#endif

/** @} */ /* End of group UART_WordSend_92 */

/** @} */ /* End of group __MDR32F9Q2_EVAL */

/** @} */ /* End of group __MDR32F9Qx_StdPeriph_Examples */

/******************* (C) COPYRIGHT 2011 Milandr *********/

/* END OF FILE main.c */

