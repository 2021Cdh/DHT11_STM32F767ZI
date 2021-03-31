
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
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


void MX_GPIO_Init_input(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pin : DHT1_usage_pin */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
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


static uint8_t Pin_readbit(void)
{
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);
}

uint8_t request_start(void)
{
	MX_GPIO_Init_outpt();                   // ��Ϊ���ģʽ
	Output_L();                             // ����������
	HAL_Delay(20);                          // ������������ʱ��,��������18ms
	Output_H();                             // ���������� 
	delay_us(30);                           // �ȴ�ʱ��30us
	
	MX_GPIO_Init_input();                   // GPIO������Ϊ����ģʽ 
	
	uint32_t count = 1;
	while(Pin_readbit() != 0)
  {
	  delay_us(1);
		count++;
		if(count > 10000000)
		{
			printf("������δ��Ӧ = %d\r\n",count);
			return 0;
		}
	};   

  count = 0;	
	
	while(Pin_readbit() == 0)
  {
	  delay_us(1);
		count++;
		if(count>100)
		{
			printf("�ȴ�����������������80usʱʧ��\r\n");
			return 0;         
		}
	};    

	while(Pin_readbit() != 0)
  {
	  delay_us(1);
		count++;
		if(count>82)
		{
			printf("�ȴ�����������������80usʱʧ��\r\n");
			return 0;
		}
	};                                 //�ȴ�����������������80us��
	
	return 1;
}

// ��ȡ�ֽں���
static uint8_t Pin_readbyte(void)
{
	uint8_t i,temp=0;         // ��ʼ��һ���ֽ�
	for(i=0;i<8;i++)
{
    //����׼�����  ÿ�����ݶ���50US�ĵ͵�ƽ��Ȼ���жϺ���ߵ�ƽ��ʱ����������1����0
    //0:�ߵ�ƽʱ��26~28us
    //1���ߵ�ƽʱ��70us   
    /*ÿһ��bit������50us�ĵ͵�ƽ��ʼ���ȵ�ƽ�øߣ�������ѭ��*/
    while( Pin_readbit() != 0 );    // �ȴ�DHT1�ٴα�Ϊ�͵�ƽ

    /*��0����ʾ���ø�ʱ����̣���ֱ���жϡ�1����ʾ��ʱ�䣬������ʱ�䲻�˹������Է�������һλ�Ŀ�ʼʱ��*/
    delay_us(60);
    
    if( Pin_readbit()  == 1 )//������Ǹߵ�ƽ���Ǿ�������1
    {
        /*�ȴ�������1���ø߱�ʾ��ʱ��*///�ȴ�ʣ��ĸߵ�ƽʱ�����
        while( Pin_readbit() == 1);
        /*�ѵڣ�7-i��λ�� '1'*/
        temp |= (uint8_t)(0x01<<(7-i));  
    }
    else
    {
        /*�ѵڣ�7-i��λ�� '0'*/
        temp &= (uint8_t)~(0x01<<(7-i));
    }
}
   return temp;
}
	
uint8_t DHT11_Orignaldata(DHT1_Data *DHT11_Data)
{
	if(request_start())
	{
		DHT11_Data->humi_int = Pin_readbyte();
		DHT11_Data->humi_deci = Pin_readbyte();
		DHT11_Data->temp_int = Pin_readbyte();
		DHT11_Data->temp_deci = Pin_readbyte();
		DHT11_Data->check_sum = Pin_readbyte();
		MX_GPIO_Init_outpt();
		Output_H();
	}
	if(DHT11_Data->check_sum == (DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int + DHT11_Data->temp_deci))
	{
		printf("У�������\r\n");
		printf("���������ݲɼ����\r\n");
		return 0;
	}
  else
	{
		printf("У���ʧ��\r\n");
		return 1;
	}
}

#define CPU_FREQUENCY_MHZ    216		// STM32ʱ����Ƶ
void delay_us(__IO uint32_t delay)
{
    int last, curr, val;
    int temp;

    while (delay != 0)
    {
        temp = delay > 900 ? 900 : delay;
        last = SysTick->VAL;
        curr = last - CPU_FREQUENCY_MHZ * temp;
        if (curr >= 0)
        {
            do
            {
                val = SysTick->VAL;
            }
            while ((val < last) && (val >= curr));
        }
        else
        {
            curr += CPU_FREQUENCY_MHZ * 1000;
            do
            {
                val = SysTick->VAL;
            }
            while ((val <= last) || (val > curr));
        }
        delay -= temp;
    }
}

//void delay_us(uint16_t us)
//{
//	__HAL_TIM_SET_COUNTER(&htim7, 0);
//	
//	__HAL_TIM_ENABLE(&htim7);

//	while (__HAL_TIM_GET_COUNTER(&htim7) < us){};
//		
////	printf("%d\n",__HAL_TIM_GET_COUNTER(&htim7));

//  __HAL_TIM_DISABLE(&htim7);

//}

//void delay_us(uint32_t us)
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





/*  ����ʾ�����뼰��������
//��λDHT11
void DHT11_Rst(void)	   
{                 
	MX_GPIO_Init_outpt(); 	//SET OUTPUT
  Output_L(); 	          //����DQ
  HAL_Delay(20);    	    //��������18ms
  Output_H(); 	          //DQ=1 
	delay_us(30);     	    //��������20~40us
}

�ȴ�DHT11�Ļ�Ӧ
����1:δ��⵽DHT11�Ĵ���
����0:����
uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;
	MX_GPIO_Init_input();//SET INPUT	 
    while (Pin_readbit()&&retry<100)//DHT11������40~80us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!Pin_readbit()&&retry<100)//DHT11���ͺ���ٴ�����40~80us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

��DHT11��ȡһ��λ
����ֵ��1/0
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while(Pin_readbit()&&retry<100)//�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!Pin_readbit()&&retry<100)//�ȴ���ߵ�ƽ
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//�ȴ�40us
	if(Pin_readbit())return 1;
	else return 0;		   
}

��DHT11��ȡһ���ֽ�
����ֵ������������
uint8_t DHT11_Read_Byte(void)    
{        
	uint8_t i,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
		dat<<=1; 
		dat|=DHT11_Read_Bit();
  }						    
  return dat;
}

��DHT11��ȡһ������
temp:�¶�ֵ(��Χ:0~50��)
humi:ʪ��ֵ(��Χ:20%~90%)
����ֵ��0,����;1,��ȡʧ��
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)    
{        
 	uint8_t buf[5];
	uint8_t i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[1];
		}
	}else return 1;
	return 0;	    
}

��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
����1:������
����0:����    	 
uint8_t DHT11_Init(void)
{	     
	DHT11_Rst();  //��λDHT11
	return DHT11_Check();//�ȴ�DHT11�Ļ�Ӧ
}
*/
