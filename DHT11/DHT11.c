
#include "tim.h"
#include "DHT11.h"
#include "stdio.h"
#include "string.h"    // Ϊ����Ӵ��ڴ�ӡ���ܶ��ӵ�ͷ�ļ�

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
	
	MX_GPIO_Init_outpt();                                               // ����GPIO�����ģʽ
	
	Output_L();                                                         // �������������ź�
	
	HAL_Delay(20);                                                      // ����������ʱ20ms
	
	MX_GPIO_Init_input();                                               // ����GPIO������ģʽ��������������ԭ�ʣ���ʱIO�ڱ�Ϊ�ߵ�ƽ
	
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 1)                 // �ȴ��ӻ������ź���
	{
		count++;
		delay_us(1);
		if(count>100)
			return 0;
	};

	count=0;
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 0)                 // �ȴ��ӻ��ٴ������źţ��˵��źű���ʱ��80us
	{
		count++;
		delay_us(1);
		if(count>100)
			return 0;
	};
		
	count=0;
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 1)                 // �ȴ��ӻ��ٴ������źţ��˸��źű���ʱ��80us
	{
		count++;
		delay_us(1);
		if(count>100)
			return 0;
	};
		
	return 1;                                                           // ���� 1 ����˵���ӻ���ʼ�����ź�
}

static uint8_t Pin_readbit(void)
{
	uint8_t retry=0;
	
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 0);                  // �ȴӻ������źţ��˵��ź���ʱʱ��Ϊ50us����
			
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 1)                     // ����ߵ�ƽ����ʱ��
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
		
		return 1;      // �ٴζ�����λʱ����֤�ӻ��ź�Ϊ�͵�ƽ
	
	else return 0;
	
}

// ��ȡ�ֽں���
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


// ��ʱ��������
void delay_us(uint16_t us)    
{
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	
	__HAL_TIM_ENABLE(&htim6);

	while (__HAL_TIM_GET_COUNTER(&htim6) < us){};

  __HAL_TIM_DISABLE(&htim6);
}

//// ��ʱ��������
//void delay_us(uint16_t delay)
//{
//	//���ö�ʱ��Ԥ��Ƶϵ��,TIM6ʱ��Ϊ108MHz,��Ƶ��ʱ��Ϊ1MHz��1us
//    //��ͬCPU��ʱ�ӿ��ܲ�һ����PSC��ֵ=��ʱ��ʱ��/1MHz -1
//    //108M�Ķ�ʱ������PSCΪ108-1
//	TIM6->PSC = (108-1);
//	//�����Զ���װ��ֵ,��ʱ����������ֵ������ARRʱ,����������¼�,ARR��ֵ������Ҫ��ʱ��ʱ��
//	TIM6->ARR = delay;
//	
//	//���³�ʼ����ʱ�������������ɼĴ��������¼�,ȷ��Ԥ��Ƶֵ�����ã���ʱ��ʱ�������øո�д���Ԥ��Ƶֵ������˴������£���ô��ʱ����Ҫ�ȴ��´θ����¼��ĵ����Ż����¼���Ԥ��Ƶֵ
//	TIM6->EGR |= (1<<0);
//	//������±�־λ,��λ�ڷ��������¼�ʱͨ��Ӳ���� 1,����Ҫͨ���������
//	TIM6->SR = 0;
//	//CR1��bit3(OPM)��һ,�������ڷ�����һ�����¼�ʱֹͣ����,������ģʽ
//	TIM6->CR1 |= (1<<3);
//	//CR1��bit0(CEN)��һ,������ʱ����ʼ����
//	TIM6->CR1 |= (1<<0);
//	//�ȴ������¼�����,��������ֵ�������Զ���װ�ؼĴ�����ʱ��,����������¼�,��ʱ��ʱʱ���ѵ�
//	while((TIM6->SR & 0x01)==0);
//	
//	//������±�־λ,��λ�ڷ��������¼�ʱͨ��Ӳ���� 1,����Ҫͨ���������
//	TIM6->SR &= ~(1<<0);
//}

//// ��ʱ��������
//#define CPU_FREQUENCY_MHZ    216		//  ����STM32ʱ����Ƶ���ã���Ϊ�ҵ�ʹ�õ���STM32F767ZITI��Ƭ����������Ƶ���λ216MHZ
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

//// ��ʱ��������
//void delay_us(uint16_t us)   
//{
//    __IO uint32_t currentTicks = SysTick->VAL;  // currentTicks = 215771
////	printf("currentTicks01 = %d\n",currentTicks);
//  /* Number of ticks per millisecond */
//  const uint32_t tickPerMs = SysTick->LOAD + 1;
////	printf("SysTick->LOAD = %d\n",tickPerMs);  // tickPerMs  = 216000Hz��systic = 1000Hz��the total frequence is 216 000 000 = 216MHz
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

