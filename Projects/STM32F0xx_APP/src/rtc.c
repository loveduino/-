#include <time.h>
#include "rtc.h"
#include "i2c_soft.h"


unsigned char BCD2HEX(unsigned char bcd_data)    //BCDתΪHEX(orʮ����)�ӳ���  ,0x12(BCD) -> 12(DEC)  תС
{   
    return (((bcd_data>>4)*10) + (bcd_data & 0x0F));
}

unsigned char HEX2BCD(unsigned char hex_data)    //HEX(orʮ����)תΪBCD�ӳ��� ,12(DEC) -> 0x12(BCD)   ת��
{   
    return (((hex_data/10)<<4) + (hex_data%10));   
}

void PCF8563_BufferWrite(uint8_t WriteAddr, const uint8_t* const pBuffer, int NumByteToWrite)
{
    I2C_WriteCommand8(PCF8563_SLAVE_ADDR, WriteAddr);
    I2C_WriteBytesAfterCommand(PCF8563_SLAVE_ADDR, pBuffer, NumByteToWrite);
}

void PCF8563_BufferRead(uint8_t ReadAddr, uint8_t* const pBuffer, uint16_t NumByteToRead)
{
    I2C_WriteCommand8(PCF8563_SLAVE_ADDR, ReadAddr);
    I2C_ReadBytesAfterCommand(PCF8563_SLAVE_ADDR, pBuffer, NumByteToRead);
}

/*
struct tm {
  int tm_sec;   // �룬������Χ�� 0 �� 59���������� 61
  int tm_min;   // �֣���Χ�� 0 �� 59
  int tm_hour;  // Сʱ����Χ�� 0 �� 23
  int tm_mday;  // һ���еĵڼ��죬��Χ�� 1 �� 31
  int tm_mon;   // �£���Χ�� 0 �� 11
  int tm_year;  // �� 1900 ���������
  int tm_wday;  // һ���еĵڼ��죬��Χ�� 0 �� 6�������������������ձ�ʾΪ 0
  int tm_yday;  // һ���еĵڼ��죬��Χ�� 0 �� 365���� 1 �� 1 ������
  int tm_isdst; // ����ʱ
}

UTC��ʱ���Ǵӹ�Ԫ1970��1��1��0ʱ0��0�뿪ʼ�ġ�
time���ʱ���Ǵӹ�Ԫ1900��1��1��0ʱ0��0�뿪ʼ�ġ�
�������2208988800s

GMT 0:00 �������α�׼ʱ��
UT +00:00 ȫ��ʱ��
UTC +00:00 У׼��ȫ��ʱ��
*/
//�ӹ�Ԫ1970��1��1��0ʱ0��0�����������UTCʱ����������������ת��Ϊ����ʱ��д�뵽RTC��
void PCF8563_SetTime(time_t seconds)//����UTCʱ�����ñ���ʱ�䣬���Ҫ���õ��صı���ʱ�䣬��Ҫ��ȡ��ǰ����ʱ���Ӧ��UTCʱ��
{
    uint8_t time_value[7];
    time_t _seconds = seconds;
    struct tm *set_time = gmtime(&_seconds);
    
    time_value[6]/*00�C99 */ = HEX2BCD((unsigned char)(set_time->tm_year + 1900 - 2000)) & 0xFF;
    time_value[5]/*01�C12 */ = HEX2BCD((unsigned char)(set_time->tm_mon + 1)) & 0x1F;   
    time_value[4]/*00�C06 */ = HEX2BCD((unsigned char)(set_time->tm_wday)) & 0x07; // һ���еĵڼ��죬��Χ�� 0 �� 6�������������������ձ�ʾΪ 0
    time_value[3]/*01�C31 */ = HEX2BCD((unsigned char)(set_time->tm_mday)) & 0x3F;
    time_value[2]/*00�C23 */ = HEX2BCD((unsigned char)(set_time->tm_hour)) & 0x3F;  
    time_value[1]/*00�C59 */ = HEX2BCD((unsigned char)(set_time->tm_min)) & 0x7F;   
    time_value[0]/*00�C59 */ = HEX2BCD((unsigned char)(set_time->tm_sec)) & 0x7F;   
    PCF8563_BufferWrite(PCF8563_REG_SECONDS, time_value, 7);
}

//int year;
//�ӹ�Ԫ1970��1��1��0ʱ0��0 �����������UTCʱ�������������������ص���UTCʱ�䣬�����Ǳ���ʱ��
time_t PCF8563_ReadTime(void)
{
    uint8_t time_value[7];
    struct tm read_time;
    
    PCF8563_BufferRead(PCF8563_REG_SECONDS, time_value, 7);
    //year = BCD2HEX(time_value[6]);
    read_time.tm_year = BCD2HEX(time_value[6]&0xFF) + 2000 - 1900;
    read_time.tm_mon =  BCD2HEX(time_value[5]&0x1F) - 1;
    read_time.tm_wday = BCD2HEX(time_value[4]&0x07);
    read_time.tm_mday = BCD2HEX(time_value[3]&0x3F);
    read_time.tm_hour = BCD2HEX(time_value[2]&0x3F);
    read_time.tm_min =  BCD2HEX(time_value[1]&0x7F);
    read_time.tm_sec =  BCD2HEX(time_value[0]&0x7F);
    
//    for (int i = 0; i < 7; i++)
//    {
//        printf("time_value[%d] = %02x\r\n", i, time_value[i]);
//    }
//    printf("\r\n\r\n");
//    
//    printf("%d %d %d  %d:%d:%d\r\n", read_time.tm_year, read_time.tm_mon, read_time.tm_mday, read_time.tm_hour, read_time.tm_min, read_time.tm_sec);
//    
    read_time.tm_isdst = -1;
    //mktime()����������timeptr��ָ��tm�ṹ����ת���ɴӹ�Ԫ1970��1��1��0ʱ0��0 �����������UTCʱ����������������
    return mktime(&read_time);//����ʱ���ʼ����ϵͳtime��ʱ��
}


