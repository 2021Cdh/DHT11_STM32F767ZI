
#include "gpio.h"

typedef struct 
{
	uint8_t humi_int;
	uint8_t humi_deci;
	
	uint8_t temp_int;
	uint8_t temp_deci;
	
	uint8_t check_sum;
	
}DHT1_Data;                                                        /*  0011 0101       0000 0000       0001 1000       0000 0000    0100 1101
                                                                      ʪ�ȸ� 8 λ     ʪ�ȵ� 8 λ     �¶ȸ� 8 λ     �¶ȵ� 8 λ     У��λ
                                                                   */

void delay_us(uint16_t delay);                                     // us��ʱ����

void MX_GPIO_Init_outpt(void);                                    // ����GPIO�����ģʽ���ú���

void MX_GPIO_Init_input(void);                                    // ����GPIO������ģʽ���ú���

void Output_H(void);                                              // ���������ź���

void Output_L(void);                                              // ���������ź���

uint8_t request_start(void);                                      // �����ź�

static uint8_t Pin_readbit(void);                                 // ��ȡbit����

static uint8_t Pin_readbyte(void);                                // ��ȡ�ֽں���

uint8_t DHT1_Orignaldata(DHT1_Data *DHT11_Data);                  // ��ȡԭʼ�ź�����


