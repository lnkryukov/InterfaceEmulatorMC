#include "stdbool.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_rst_clk.h"
#include "../inc/MDR32F9Qx_usb_handlers.h"
#include "../inc/UART2_send.h"
#include "../inc/CAN_send.h"
#include "../inc/serialmessagemc.h"

#define BUFFER_LENGTH                        100

USB_Clock_TypeDef USB_Clock_InitStruct;
USB_DeviceBUSParam_TypeDef USB_DeviceBUSParam;

static uint8_t Buffer[BUFFER_LENGTH];

#ifdef USB_CDC_LINE_CODING_SUPPORTED
static USB_CDC_LineCoding_TypeDef LineCoding;
#endif /* USB_CDC_LINE_CODING_SUPPORTED */


static void Setup_CPU_Clock(void);
static void Setup_USB(void);
static void VCom_Configuration(void);

static volatile uint8_t isReady = 0;
unsigned int id = 0x1FFFFFFE;

int main(void)
{
	
  VCom_Configuration();

  /* CDC layer initialization */
  USB_CDC_Init(Buffer, 1, SET);
	
  Setup_CPU_Clock();
  Setup_USB();
	
	initUART2();
	init_CAN();
			
  /* Main loop */
  while (1) {
						
	}

}


/* Frequencies setup */
void Setup_CPU_Clock(void)
{
  /* Enable HSE */
  RST_CLK_HSEconfig(RST_CLK_HSE_ON);
  if (RST_CLK_HSEstatus() != SUCCESS)
  {
    /* Trap */
    while (1)
    {
    }
  }

  /* CPU_C1_SEL = HSE */
  //RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul10);
  	/* Select HSI/2 as CPU_CLK source*/
  RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);
	RST_CLK_CPU_PLLcmd(ENABLE);
  if (RST_CLK_CPU_PLLstatus() != SUCCESS)
  {
    /* Trap */
    while (1)
    {
    }
  }

  /* CPU_C3_SEL = CPU_C2_SEL */
  RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
  /* CPU_C2_SEL = PLL */
  RST_CLK_CPU_PLLuse(ENABLE);
  /* HCLK_SEL = CPU_C3_SEL */
  RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
}

/* USB Device layer setup and powering on */
void Setup_USB(void)
{
  /* Enables the CPU_CLK clock on USB */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_USB, ENABLE);

  /* Device layer initialization */
  USB_Clock_InitStruct.USB_USBC1_Source = USB_C1HSEdiv2;
  USB_Clock_InitStruct.USB_PLLUSBMUL    = USB_PLLUSBMUL12;

  USB_DeviceBUSParam.MODE  = USB_SC_SCFSP_Full;
  USB_DeviceBUSParam.SPEED = USB_SC_SCFSR_12Mb;
  USB_DeviceBUSParam.PULL  = USB_HSCR_DP_PULLUP_Set;

  USB_DeviceInit(&USB_Clock_InitStruct, &USB_DeviceBUSParam);

  /* Enable all USB interrupts */
  USB_SetSIM(USB_SIS_Msk);

  USB_DevicePowerOn();

  /* Enable interrupt on USB */
#ifdef USB_INT_HANDLE_REQUIRED
  NVIC_EnableIRQ(USB_IRQn);
#endif /* USB_INT_HANDLE_REQUIRED */

  USB_DEVICE_HANDLE_RESET;
}


/* Example-relating data initialization */
static void VCom_Configuration(void)
{
#ifdef USB_CDC_LINE_CODING_SUPPORTED
  LineCoding.dwDTERate = 115200;
  LineCoding.bCharFormat = 0;
  LineCoding.bParityType = 0;
  LineCoding.bDataBits = 8;
#endif /* USB_CDC_LINE_CODING_SUPPORTED */
}




/* USB_CDC_HANDLE_DATA_RECEIVE implementation - data echoing */
USB_Result USB_CDC_RecieveData(uint8_t* Buffer, uint32_t Length)
{
 	
	uint8_t crcCheck = checkCrc(Buffer);
	
	if(crcCheck) {
		crcCheck = 0;
		
		Header* header = getHeader(Buffer);
		
		uint8_t* data = getData(Buffer);		
				
		crcCheck = 0;
		
		switch(header->type) {
			
			case RS485: 
				
				USB_CDC_SendData(&crcCheck, 1);
			
				isReady = 1;
				UART2SendData(data, header->length - 4);
				isReady = 0;

				break;

			case CAN:
				
				USB_CDC_SendData(&crcCheck, 1);
				
				isReady = 1;
				CAN_Send(data, header->length - 4);
				isReady = 0;

				break;
			
			case UPDATE:
				USB_CDC_SendData((uint8_t*) &isReady, 1);
				break;
		}
						
	} else {
		crcCheck = 255;
		USB_CDC_SendData(&crcCheck, 1);
	}

  return USB_SUCCESS;
}


/* USB_CDC_HANDLE_DATA_SENT implementation - sending of pending data */

USB_Result USB_CDC_DataSent()
{
	
	USB_CDC_ReceiveStart();
	
  return USB_SUCCESS;
}


#ifdef USB_CDC_LINE_CODING_SUPPORTED

/* USB_CDC_HANDLE_GET_LINE_CODING implementation example */
USB_Result USB_CDC_GetLineCoding(uint16_t wINDEX, USB_CDC_LineCoding_TypeDef* DATA)
{
  assert_param(DATA);
  if (wINDEX != 0)
  {
    /* Invalid interface */
    return USB_ERR_INV_REQ;
  }

  /* Just store received settings */
  *DATA = LineCoding;
  return USB_SUCCESS;
}

/* USB_CDC_HANDLE_SET_LINE_CODING implementation example */
USB_Result USB_CDC_SetLineCoding(uint16_t wINDEX, const USB_CDC_LineCoding_TypeDef* DATA)
{
  assert_param(DATA);
  if (wINDEX != 0)
  {
    /* Invalid interface */
    return USB_ERR_INV_REQ;
  }

  /* Just send back settings stored earlier */
  LineCoding = *DATA;
  return USB_SUCCESS;
}

#endif /* USB_CDC_LINE_CODING_SUPPORTED */

#ifdef USB_DEBUG_PROTO

/* Overwritten USB_DEVICE_HANDLE_SETUP default handler - to dump received setup packets */
USB_Result USB_DeviceSetupPacket_Debug(USB_EP_TypeDef EPx, const USB_SetupPacket_TypeDef* USB_SetupPacket)
{
  return USB_DeviceSetupPacket(EPx, USB_SetupPacket);
}

#endif /* USB_DEBUG_PROTO */



/**
  * @brief  Reports the source file ID, the source line number
  *         and expression text (if USE_ASSERT_INFO == 2) where
  *         the assert_param error has occurred.
  * @param  file_id: pointer to the source file name
  * @param  line: assert_param error line source number
  * @param  expr:
  * @retval None
  */
#if (USE_ASSERT_INFO == 1)
void assert_failed(uint32_t file_id, uint32_t line)
{
  /* User can add his own implementation to report the source file ID and line number.
     Ex: printf("Wrong parameters value: file Id %d on line %d\r\n", file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#elif (USE_ASSERT_INFO == 2)
void assert_failed(uint32_t file_id, uint32_t line, const uint8_t* expr);
{
  /* User can add his own implementation to report the source file ID, line number and
     expression text.
     Ex: printf("Wrong parameters value (%s): file Id %d on line %d\r\n", expr, file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_ASSERT_INFO */

/** @} */ /* End of group USB_Virtual_COM_Port_Echo_92 */

/** @} */ /* End of group __MDR32F9Q2_EVAL */

/** @} */ /* End of group __MDR32F9Qx_StdPeriph_Examples */

/******************* (C) COPYRIGHT 2011 Milandr *******************
*
* END OF FILE main.c */

