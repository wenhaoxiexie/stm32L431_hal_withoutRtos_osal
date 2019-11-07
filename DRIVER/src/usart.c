#include "usart.h"

static 	UART_HandleTypeDef UartHandle;

void usart1_init(void)
{

	UartHandle.Instance        = USART1;

  UartHandle.Init.BaudRate     = 115200;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;

	HAL_UART_Init(&UartHandle);

}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();			//ʹ��GPIOBʱ��
		__HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��
	
		GPIO_Initure.Pin=GPIO_PIN_6;			
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	//����ΪUSART1
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	

		GPIO_Initure.Pin=GPIO_PIN_7;			
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   
	}
}

struct __FILE 
{ 
	int handle; 
}; 

void _sys_exit(int x) 
{ 
	x = x; 
} 

FILE __stdout; 

int fputc(int ch,FILE* f)
{
	
	while(__HAL_UART_GET_FLAG(&UartHandle,UART_FLAG_TC)==RESET);
	
	
	USART1->TDR = (uint8_t)ch;
	
	return ch;
}


/* USER CODE END 1 */

