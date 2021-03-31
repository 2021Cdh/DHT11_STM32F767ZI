
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
	MX_GPIO_Init_outpt();                   // 变为输出模式
	Output_L();                             // 将主机拉低
	HAL_Delay(20);                          // 配置主机拉低时间,至少拉低18ms
	Output_H();                             // 将主机拉高 
	delay_us(30);                           // 等待时间30us
	
	MX_GPIO_Init_input();                   // GPIO口设置为输入模式 
	
	uint32_t count = 1;
	while(Pin_readbit() != 0)
  {
	  delay_us(1);
		count++;
		if(count > 10000000)
		{
			printf("传感器未反应 = %d\r\n",count);
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
			printf("等待传感器拉低数据线80us时失败\r\n");
			return 0;         
		}
	};    

	while(Pin_readbit() != 0)
  {
	  delay_us(1);
		count++;
		if(count>82)
		{
			printf("等待传感器拉高数据线80us时失败\r\n");
			return 0;
		}
	};                                 //等待传感器拉高数据线80us；
	
	return 1;
}

// 读取字节函数
static uint8_t Pin_readbyte(void)
{
	uint8_t i,temp=0;         // 开始读一个字节
	for(i=0;i<8;i++)
{
    //数据准备输出  每个数据都有50US的低电平，然后判断后面高电平是时长来决定是1还是0
    //0:高电平时长26~28us
    //1：高电平时长70us   
    /*每一个bit都是以50us的低电平开始，等电平置高，则跳出循环*/
    while( Pin_readbit() != 0 );    // 等待DHT1再次变为低电平

    /*‘0’表示的置高时间过短，可直接判断‘1’表示的时间，但滞留时间不宜过长，以防跳出下一位的开始时间*/
    delay_us(60);
    
    if( Pin_readbit()  == 1 )//如果还是高电平，那就是数据1
    {
        /*等待跳出‘1’置高表示的时间*///等待剩余的高电平时间结束
        while( Pin_readbit() == 1);
        /*把第（7-i）位置 '1'*/
        temp |= (uint8_t)(0x01<<(7-i));  
    }
    else
    {
        /*把第（7-i）位置 '0'*/
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
		printf("校验和正常\r\n");
		printf("传感器数据采集完毕\r\n");
		return 0;
	}
  else
	{
		printf("校验和失败\r\n");
		return 1;
	}
}

#define CPU_FREQUENCY_MHZ    216		// STM32时钟主频
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





/*  网上示例代码及函数定义
//复位DHT11
void DHT11_Rst(void)	   
{                 
	MX_GPIO_Init_outpt(); 	//SET OUTPUT
  Output_L(); 	          //拉低DQ
  HAL_Delay(20);    	    //拉低至少18ms
  Output_H(); 	          //DQ=1 
	delay_us(30);     	    //主机拉高20~40us
}

等待DHT11的回应
返回1:未检测到DHT11的存在
返回0:存在
uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;
	MX_GPIO_Init_input();//SET INPUT	 
    while (Pin_readbit()&&retry<100)//DHT11会拉低40~80us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!Pin_readbit()&&retry<100)//DHT11拉低后会再次拉高40~80us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

从DHT11读取一个位
返回值：1/0
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while(Pin_readbit()&&retry<100)//等待变为低电平
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!Pin_readbit()&&retry<100)//等待变高电平
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//等待40us
	if(Pin_readbit())return 1;
	else return 0;		   
}

从DHT11读取一个字节
返回值：读到的数据
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

从DHT11读取一次数据
temp:温度值(范围:0~50°)
humi:湿度值(范围:20%~90%)
返回值：0,正常;1,读取失败
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)    
{        
 	uint8_t buf[5];
	uint8_t i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
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

初始化DHT11的IO口 DQ 同时检测DHT11的存在
返回1:不存在
返回0:存在    	 
uint8_t DHT11_Init(void)
{	     
	DHT11_Rst();  //复位DHT11
	return DHT11_Check();//等待DHT11的回应
}
*/
