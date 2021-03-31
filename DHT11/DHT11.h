
#include "gpio.h"

typedef struct 
{
	uint8_t humi_int;
	uint8_t humi_deci;
	
	uint8_t temp_int;
	uint8_t temp_deci;
	
	uint8_t check_sum;
	
}DHT1_Data;                                                        /*  0011 0101       0000 0000       0001 1000       0000 0000    0100 1101
                                                                      湿度高 8 位     湿度低 8 位     温度高 8 位     温度低 8 位     校验位
                                                                   */

void delay_us(uint16_t delay);                                     // us延时函数

void MX_GPIO_Init_outpt(void);                                    // 主机GPIO口输出模式设置函数

void MX_GPIO_Init_input(void);                                    // 主机GPIO口输入模式设置函数

void Output_H(void);                                              // 主机拉高信函数

void Output_L(void);                                              // 主机拉低信函数

uint8_t request_start(void);                                      // 请求信号

static uint8_t Pin_readbit(void);                                 // 读取bit函数

static uint8_t Pin_readbyte(void);                                // 读取字节函数

uint8_t DHT1_Orignaldata(DHT1_Data *DHT11_Data);                  // 读取原始信号数据


