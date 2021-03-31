
#include "gpio.h"

typedef struct 
{
	uint8_t humi_int;
	uint8_t humi_deci;
	
	uint8_t temp_int;
	uint8_t temp_deci;
	
	uint8_t check_sum;
	
}DHT1_Data;

void delay_us(__IO uint32_t delay);

void MX_GPIO_Init_outpt(void);
void MX_GPIO_Init_input(void);

void Output_H(void);
void Output_L(void);

uint8_t request_start(void);        // 请求信号
static uint8_t Pin_readbit(void);
static uint8_t Pin_readbyte(void);
uint8_t DHT1_Orignaldata(DHT1_Data *DHT11_Data);



/* 以下是淘宝网上的示例教程

uint8_t DHT11_Init(void);//初始化DHT11
void DHT11_Rst(void);//复位DHT11  
uint8_t DHT11_Check(void);//检测是否存在DHT11
uint8_t DHT11_Read_Bit(void);//读出一个位
uint8_t DHT11_Read_Byte(void);//读出一个字节
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);//读取温湿度

*/

