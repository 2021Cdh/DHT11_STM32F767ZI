
#include "tim.h"
#include "DHT11.h"
#include "stdio.h"
#include "string.h"    // 为了添加串口打印功能而加的头文件

void MX_GPIO_Init_outpt(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	
  /*Configure GPIO pin : DHT1_usage_pin */
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


void MX_GPIO_Init_input(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pin : DHT1_usage_pin */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}


void Output_H(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}


void Output_L(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}


uint8_t request_start(void)
{
	uint8_t count = 0;
	
	MX_GPIO_Init_outpt();                                               // 主机GPIO口输出模式
	
	Output_L();                                                         // 主机主动拉低信号
	
	HAL_Delay(20);                                                      // 主机拉低延时20ms
	
	MX_GPIO_Init_input();                                               // 主机GPIO口输入模式，由于上拉电阻原故，此时IO口变为高电平
	
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 1)                 // 等待从机拉低信号线
	{
		count++;
		delay_us(1);
		if(count>100)
			return 0;
	};

	count=0;
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 0)                 // 等待从机再次拉高信号，此低信号保持时长80us
	{
		count++;
		delay_us(1);
		if(count>100)
			return 0;
	};
		
	count=0;
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 1)                 // 等待从机再次拉低信号，此高信号保持时长80us
	{
		count++;
		delay_us(1);
		if(count>100)
			return 0;
	};
		
	return 1;                                                           // 返回 1 ，则说明从机开始发送信号
}

static uint8_t Pin_readbit(void)
{
	uint8_t retry=0;
	
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 0);                  // 等从机拉高信号，此低信号延时时长为50us左右
			
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 1)                     // 计算高电平持续时间
	{
		retry++;
		
		delay_us(10);
		
		if(retry >= 6)
		{
			break;
		}
	}
	
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 1);
		
	if(retry > 5)
		
		return 1;      // 再次读数据位时，保证从机信号为低电平
	
	else return 0;
	
}

// 读取字节函数
static uint8_t Pin_readbyte(void)
{
	
	uint8_t i,dat = 0;

	for (i=0;i<8;i++) 
	{
		dat = dat<<1; 
		
		dat|=Pin_readbit();
  }			
  return dat;
}

uint8_t DHT1_Orignaldata(DHT1_Data *DHT111_Data)
{
	DHT111_Data->humi_int = Pin_readbyte();

	DHT111_Data->humi_deci  = Pin_readbyte();
	
	DHT111_Data->temp_int  = Pin_readbyte();
	
	DHT111_Data->temp_deci  = Pin_readbyte();
	
	DHT111_Data->check_sum = Pin_readbyte();
	
	if(DHT111_Data->check_sum  == (DHT111_Data->humi_int  + DHT111_Data->humi_deci  + DHT111_Data->temp_int  + DHT111_Data->temp_deci ))
		
		return 1;
	
  else
		
		return 0;
}


// 延时函数可用
void delay_us(uint16_t us)    
{
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	
	__HAL_TIM_ENABLE(&htim6);

	while (__HAL_TIM_GET_COUNTER(&htim6) < us){};

  __HAL_TIM_DISABLE(&htim6);
}

//// 延时函数可用
//void delay_us(uint16_t delay)
//{
//	//设置定时器预分频系数,TIM6时钟为108MHz,分频后时钟为1MHz即1us
//    //不同CPU的时钟可能不一样，PSC的值=定时器时钟/1MHz -1
//    //108M的定时器设置PSC为108-1
//	TIM6->PSC = (108-1);
//	//设置自动重装载值,定时器计数器的值自增到ARR时,会产生更新事件,ARR的值就是需要延时的时间
//	TIM6->ARR = delay;
//	
//	//重新初始化定时器计数器并生成寄存器更新事件,确保预分频值被采用，此时定时器将采用刚刚写入的预分频值，如果此处不更新，那么定时器需要等待下次更新事件的到来才会重新加载预分频值
//	TIM6->EGR |= (1<<0);
//	//清除更新标志位,该位在发生更新事件时通过硬件置 1,但需要通过软件清零
//	TIM6->SR = 0;
//	//CR1的bit3(OPM)置一,计数器在发生下一更新事件时停止计数,单脉冲模式
//	TIM6->CR1 |= (1<<3);
//	//CR1的bit0(CEN)置一,启动定时器开始计数
//	TIM6->CR1 |= (1<<0);
//	//等待更新事件到来,计数器的值自增到自动重装载寄存器的时候,会产生更新事件,此时延时时间已到
//	while((TIM6->SR & 0x01)==0);
//	
//	//清除更新标志位,该位在发生更新事件时通过硬件置 1,但需要通过软件清零
//	TIM6->SR &= ~(1<<0);
//}

//// 延时函数可用
//#define CPU_FREQUENCY_MHZ    216		//  根据STM32时钟主频设置，因为我的使用的是STM32F767ZITI单片机，所以主频最高位216MHZ
//void delay_us(uint16_t delay)
//{
//	HAL_TIM_Base_Start(&htim6);
//	
//	__HAL_TIM_SetCounter(&htim6,delay);
//	
//	while(__HAL_TIM_GetCounter(&htim6) < delay)
//	
//	HAL_TIM_Base_Stop(&htim6);
//	
//  int last, curr, val;
//	
//  int temp;

//  while (delay != 0)
//  {
//		
//		temp = delay > 900 ? 900 : delay;
//		
//		last = SysTick->VAL;
//		
//		curr = last - CPU_FREQUENCY_MHZ * temp;
//		
//		if (curr >= 0)
//       do
//          val = SysTick->VAL;
//       while ((val < last) && (val >= curr));
//    else
//      {
//        curr += CPU_FREQUENCY_MHZ * 1000;
//        do
//           val = SysTick->VAL;
//            while ((val <= last) || (val > curr));
//      }
//    delay -= temp;
//   }
//}

//// 延时函数可用
//void delay_us(uint16_t us)   
//{
//    __IO uint32_t currentTicks = SysTick->VAL;  // currentTicks = 215771
////	printf("currentTicks01 = %d\n",currentTicks);
//  /* Number of ticks per millisecond */
//  const uint32_t tickPerMs = SysTick->LOAD + 1;
////	printf("SysTick->LOAD = %d\n",tickPerMs);  // tickPerMs  = 216000Hz，systic = 1000Hz，the total frequence is 216 000 000 = 216MHz
//  /* Number of ticks to count */
//  const uint32_t nbTicks = (us * tickPerMs) / 1000;  // 216 tics
////	printf("nbTicks = %d\n",nbTicks); 
//  /* Number of elapsed ticks */
//  uint32_t elapsedTicks = 0;
//  __IO uint32_t oldTicks = currentTicks; // currentTicks = 215771
////	printf("oldTicks01 = %d\n",oldTicks); 
//  do {
//    currentTicks = SysTick->VAL;
////		printf("currentTicks01 = %d\n",currentTicks); 
//    elapsedTicks += (oldTicks < currentTicks) ? tickPerMs + oldTicks - currentTicks : oldTicks - currentTicks;
////		printf("elapsedTicks01 = %d\n",elapsedTicks); 
//    oldTicks = currentTicks;
//  } while (nbTicks > elapsedTicks);
//}

