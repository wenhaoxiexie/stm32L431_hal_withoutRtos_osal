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
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
	
		GPIO_Initure.Pin=GPIO_PIN_6;			
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	//复用为USART1
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

