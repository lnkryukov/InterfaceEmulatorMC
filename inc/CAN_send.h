
#ifndef CAN_SEND_H
#define CAN_SEND_H

#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_can.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"

extern unsigned int id;

__IO uint32_t tx_buf = 1;
PORT_InitTypeDef PORT_InitStructure;

void init_CAN(void)
{
  RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_CAN1),ENABLE);
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC,ENABLE);
	
	CAN_InitTypeDef  sCAN;
	
	/* Set the HCLK division factor = 1 for CAN1*/
	CAN_BRGInit(MDR_CAN1,CAN_HCLKdiv1);
	
	/* CAN register init */
	CAN_DeInit(MDR_CAN1);

	/* CAN cell init */
	CAN_StructInit (&sCAN);

  sCAN.CAN_ROP  = DISABLE;         
  sCAN.CAN_SAP  = DISABLE;          
  sCAN.CAN_STM  = DISABLE;          
  sCAN.CAN_ROM  = DISABLE;       
  sCAN.CAN_PSEG = CAN_PSEG_Mul_2TQ;  
  sCAN.CAN_SEG1 = CAN_SEG1_Mul_4TQ; 
  sCAN.CAN_SEG2 = CAN_SEG2_Mul_3TQ; 
  sCAN.CAN_SJW  = CAN_SJW_Mul_2TQ;   
  sCAN.CAN_SB   = CAN_SB_1_SAMPLE;   
  sCAN.CAN_BRP  = 15;                
  sCAN.CAN_OVER_ERROR_MAX = 255;  

	CAN_Init(MDR_CAN1,&sCAN);	


	
	/* Disable all CAN1 interrupt */
	CAN_ITConfig( MDR_CAN1, CAN_IT_GLBINTEN | CAN_IT_RXINTEN | CAN_IT_TXINTEN |
							CAN_IT_ERRINTEN | CAN_IT_ERROVERINTEN, DISABLE);
			
	/* Enable CAN1 interrupt from receive buffer */
	//CAN_RxITConfig( MDR_CAN1 ,(1<<rx_buf), ENABLE);
	/* Enable CAN1 interrupt from transmit buffer */
	CAN_TxITConfig( MDR_CAN1 ,(1<<tx_buf), ENABLE);

	/* receive buffer enable */
	//CAN_Receive(MDR_CAN1, rx_buf, ENABLE);
  
  CAN_Cmd(MDR_CAN1, ENABLE);
}

void CAN_Send(uint8_t* arr, unsigned short int length)
{
  
  CAN_TxMsgTypeDef TxMsg;
  //CAN_RxMsgTypeDef RxMsg;

	TxMsg.IDE     = CAN_ID_STD;
  TxMsg.PRIOR_0 = DISABLE;
	
	for(uint8_t* ptr = arr; ptr < arr + length; ptr += 8) {
		
		TxMsg.DLC = 0;
		TxMsg.ID = CAN_STDID_TO_EXTID(id--);
		TxMsg.Data[0] = 0;
		TxMsg.Data[1] = 0;

//		RxMsg.Data[0] = 0;
//		RxMsg.Data[1] = 0;
//		RxMsg.Rx_Header.DLC = 0;
//		RxMsg.Rx_Header.ID = 0;
//		RxMsg.Rx_Header.IDE = 0;
//		RxMsg.Rx_Header.OVER_EN = 0;

		unsigned short int mod = length % 8;
		
		if((ptr + 8) > arr + length && mod != 0) {

			unsigned short int i;

			if(mod >= 4) {
				TxMsg.Data[0] = *(uint32_t*)ptr;
				TxMsg.DLC = 4;
				mod -= 4;
				ptr += 4;
				i = 1;
			} else {
				i = 0;
			}
			
			TxMsg.DLC += mod;

			for(;mod > 0; mod--) {
				TxMsg.Data[i] = TxMsg.Data[i] << 8 | *(ptr + mod - 1);
			}

		} else {
			TxMsg.DLC = 0x08;
			TxMsg.Data[0] = *(uint32_t*)ptr;
			TxMsg.Data[1] = *(uint32_t*)(ptr + 4);
		}
		
		CAN_Transmit(MDR_CAN1, tx_buf, &TxMsg);
		
		uint32_t f = 0;
		while(((CAN_GetStatus(MDR_CAN1) & CAN_STATUS_TX_READY) != RESET) && (f != 0xFFF))
		{
			f++;
		}
		CAN_ITClearRxTxPendingBit(MDR_CAN1, tx_buf, CAN_STATUS_TX_READY);
		
	}
}

#endif
/** @} */ /* End of group CAN_LoopBack_Polling_92 */

/** @} */ /* End of group __MDR32F9Q2_EVAL */

/** @} */ /* End of group __MDR32F9Qx_StdPeriph_Examples */

/******************* (C) COPYRIGHT 2011 Milandr *********/

/* END OF FILE main.c */

