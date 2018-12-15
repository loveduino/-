#ifndef __RTC_H
#define __RTC_H

#include "stm32f0xx.h"


//���Ժ꿪��
#define PCF8563_DBUG	1
#if PCF8563_DBUG
	#include "main.h"
	#define PCF8563_Debug(format,...)	printf(format,##__VA_ARGS__)
#else
	#define PCF8563_Debug(format,...)	
#endif	//PCF8563_DBUG


//PCF8563 iic���ߵ�ַ
#define PCF8563_SLAVE_ADDR			    0xA2

//PCF8563 �Ĵ���
typedef enum
{
	PCF8563_REG_CONTROL1			=	0x00,	//���ƼĴ���1
	PCF8563_REG_CONTROL2			=	0x01,	//���ƼĴ���2
	PCF8563_REG_SECONDS				=	0x02,	//�� seconds 00 to 59 coded in BCD
	PCF8563_REG_MINUTES				=	0x03,	//�� minutes 00 to 59 coded in BCD
	PCF8563_REG_HOURS				=	0x04,	//Сʱ hours 00 to 23 coded in BCD
	PCF8563_REG_DAYS				=	0x05,	//�� days 01 to 31 coded in BCD
	PCF8563_REG_WEEK				=	0x06,	//���� weekdays 0 to 6 in BCD
	PCF8563_REG_MONTHS				=	0x07,	//�·� months 01 to 12 coded in BCD
	PCF8563_REG_YEARS				=	0x08,	//��� years 00 to 99 coded in BCD
	PCF8563_REG_ALARM_MINUTE		=	0x09,	//���ӣ����� minute alarm 00 to 59 coded in BCD
	PCF8563_REG_ALARM_HOUR			=	0x0A,	//���ӣ�Сʱ hour alarm 00 to 23 coded in BCD
	PCF8563_REG_ALARM_DAY			=	0x0B,	//���ӣ��� day alarm 01 to 31 coded in BCD
	PCF8563_REG_ALARM_WEEK			=	0x0C,	//���ӣ����� weekday alarm 0 to 6 in BCD
	PCF8563_REG_CLKOUT				=	0x0D,	//ʱ���������
	PCF8563_REG_TIME_CONTROL		=	0x0E,	//��ʱ������
	PCF8563_REG_TIME				=	0x0F,	//��ʱ��������ֵ
}PCF8563_REG_TYPE;


void PCF8563_SetTime(time_t seconds);//����UTCʱ�����ñ���ʱ�䣬���Ҫ���õ��صı���ʱ�䣬��Ҫ��ȡ��ǰ����ʱ���Ӧ��UTCʱ��
//�ӹ�Ԫ1970��1��1��0ʱ0��0 �����������UTCʱ�������������������ص���UTCʱ�䣬�����Ǳ���ʱ��
time_t PCF8563_ReadTime(void);

#endif//__RTC_H