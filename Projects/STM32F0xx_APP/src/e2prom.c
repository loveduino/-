#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "i2c_soft.h"
#include "e2prom.h"

#define I2C_PageSize           8			/* AT24C02ÿҳ��8���ֽ� */


/*
 * ��������I2C_EE_WaitEepromStandbyState
 * ����  ��Wait for EEPROM Standby state 
 * ����  ����
 * ���  ����
 * ����  ����
 * ����  �� 
 */
/*
EEPROM�ڽ��������ݺ������ڲ�����д�����ݵ�ʱ�䲻�����������������Ӧ������ԡ�
���������������ѭ��������ʼ�źţ�����⵽EEPROM��Ӧ����˵��EEPROM�Ѿ������һ��������д�룬����standby״̬�����Խ�����һ���Ĳ�����
*/
void I2C_EE_WaitEepromStandbyState(void)
{
    wait_ms(5);
}

/*
* ��������I2C_EE_BufferWrite
* ����  �����������е�����д��I2C EEPROM��
* ����  ��-pBuffer ������ָ��
*         -WriteAddr �������ݵ�EEPROM�ĵ�ַ
*         -NumByteToWrite Ҫд��EEPROM���ֽ���
* ���  ����
* ����  ����
* ����  ���ⲿ����
*/
void I2C_EE_BufferWrite(uint8_t WriteAddr, const uint8_t* const pBuffer, uint16_t NumByteToWrite)
{
    uint8_t *_pBuffer = (uint8_t *)pBuffer;
    uint8_t NumOfFirstPageAvailable = I2C_PageSize - WriteAddr % I2C_PageSize;
    //ҳ�Ǿ��Եģ�����ҳ��С���У����Ǵӿ�ʼд��ĵ�ַ��ʼ�㡣
    //ҳд���ֽ�д��������ǣ�ҳд����һ��д������ݣ����ֽ�дֻ��һ��дһ�����ݡ�
    //������AT24C02A��һҳ��8���ֽڣ�����ҳдҲ���д8�����ݣ�����ֻ���ڸ�ҳ��д�����ᷢ��һ��ҳдͬʱд��ҳ�������
    if (NumByteToWrite <= NumOfFirstPageAvailable)
    {
        I2C_WriteRegisterBytes(E2PROM_SLAVE_ADDR, WriteAddr, _pBuffer, NumByteToWrite);
        I2C_EE_WaitEepromStandbyState();
    }
    else
    {
        I2C_WriteRegisterBytes(E2PROM_SLAVE_ADDR, WriteAddr, _pBuffer, NumOfFirstPageAvailable);
        I2C_EE_WaitEepromStandbyState();
        
        uint16_t NumOfRemainPages = (NumByteToWrite - NumOfFirstPageAvailable) / I2C_PageSize;
        uint8_t NumOfRemainBytes = (NumByteToWrite - NumOfFirstPageAvailable) % I2C_PageSize;

        WriteAddr +=  NumOfFirstPageAvailable;
        _pBuffer += NumOfFirstPageAvailable;
        I2C_EE_WaitEepromStandbyState();
        while(NumOfRemainPages--)
        {
            I2C_WriteRegisterBytes(E2PROM_SLAVE_ADDR, WriteAddr, _pBuffer, I2C_PageSize);//һ�ο���д��8���ֽڵ�����
            I2C_EE_WaitEepromStandbyState();
            WriteAddr +=  I2C_PageSize;
            _pBuffer += I2C_PageSize;
        }
        if(NumOfRemainBytes != 0)
        {
            I2C_WriteRegisterBytes(E2PROM_SLAVE_ADDR, WriteAddr, _pBuffer, NumOfRemainBytes);
            I2C_EE_WaitEepromStandbyState();
        }
    }
}

/*
* ��������I2C_EE_BufferRead
* ����  ����EEPROM�����ȡһ�����ݡ� 
* ����  ��-pBuffer ��Ŵ�EEPROM��ȡ�����ݵĻ�����ָ�롣
*         -WriteAddr �������ݵ�EEPROM�ĵ�ַ�� 
*         -NumByteToWrite Ҫ��EEPROM��ȡ���ֽ�����
* ���  ����
* ����  ����
* ����  ���ⲿ����
*/
void I2C_EE_BufferRead(uint8_t ReadAddr, uint8_t* const pBuffer, uint16_t NumByteToRead)
{
    uint8_t *_pBuffer = (uint8_t *)pBuffer;
    //��ȡ���ݿ���һֱ�������һ����ַ.���ж�û��һҳ8���ֽڵ�����
    //��û��ҳ�����⣬���Դ������ַ��ʼ��ȡ�����С���ݣ�ֻ�ǳ��������洢������ʱ��ַ�Żؾ�
    I2C_ReadRegisterBytes(E2PROM_SLAVE_ADDR, ReadAddr, _pBuffer, NumByteToRead);
}

int I2C_EE_BufferWriteSafe(uint8_t WriteAddr, const uint8_t* const pBuffer, uint16_t NumByteToWrite)
{
    uint8_t* pReadBuffer = (uint8_t*)malloc(sizeof(uint8_t) * NumByteToWrite);
    if (NULL == pReadBuffer)
        return -2;
    uint8_t count = 1;
    do {
        I2C_EE_BufferWrite(WriteAddr, pBuffer, NumByteToWrite);
        I2C_EE_BufferRead(WriteAddr, pReadBuffer, NumByteToWrite);
        if (0 == memcmp(pReadBuffer, pBuffer, NumByteToWrite))
        {
            break;
        }
    }while (count++ < 3);
    free(pReadBuffer);
    if (count >= 3)
        return -1;
    else
        return count;
}

void I2C_EE_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_APB2Periph_E2WP_PORT, ENABLE);
    GPIO_InitStructure.GPIO_Pin 	= E2WP_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(E2WP_PORT, &GPIO_InitStructure);
}

