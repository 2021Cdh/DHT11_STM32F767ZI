
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

uint8_t request_start(void);        // �����ź�
static uint8_t Pin_readbit(void);
static uint8_t Pin_readbyte(void);
uint8_t DHT1_Orignaldata(DHT1_Data *DHT11_Data);



/* �������Ա����ϵ�ʾ���̳�

uint8_t DHT11_Init(void);//��ʼ��DHT11
void DHT11_Rst(void);//��λDHT11  
uint8_t DHT11_Check(void);//����Ƿ����DHT11
uint8_t DHT11_Read_Bit(void);//����һ��λ
uint8_t DHT11_Read_Byte(void);//����һ���ֽ�
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);//��ȡ��ʪ��

*/

