#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "gprs.h"
#include "serial.h"
#include "protocol.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"


static portTASK_FUNCTION_PROTO( vEdpRecvProcessTask, pvParameters );
static TaskHandle_t xEdpRecvProcessTaskHandle;

static  void  AppTaskEdpNetCheck        (void        *p_arg);
static TaskHandle_t xedpNetCheckHandlerTask = NULL;

static  void  AppTaskEdpNetPing         (void        *p_arg);
static TaskHandle_t xedpNetPingHandlerTask = NULL;


edp_t edp = {
    .is_tcpip_connect = false,
    .is_ping_resp = false,
    .is_ping_send = false
};

int send_ping_again = 0;

/* Handle to the com port used by both tasks. */
static xComPortHandle xPort = NULL;

void GPRS_PWRKEY_Init (void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(GPRS_PWRKEY_RCC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPRS_PWRKEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPRS_PWRKEY_PORT, &GPIO_InitStructure);
}

//PWRKEY ���ſ���
//��Ƭ������ߵ�ƽ����PWRKEY���ţ�PWRKEY���ŵ͵�ƽ��Ч
//�� PWRKEY ��Ϊ�͵�ƽ����STATUS ��������� ��ƽ֮�󣬱�ʾ�����ɹ��� PWRKEY ���ſ����ͷţ�ͨ�����STATUS ���ŵĵ�ƽ���б�ģ���Ƿ񿪻���
// >1s��Ч
void GPRS_PWRKEY_On (void)
{
    GPIO_SetBits(GPRS_PWRKEY_PORT, GPRS_PWRKEY_PIN);
    wait(2);
    GPIO_ResetBits(GPRS_PWRKEY_PORT, GPRS_PWRKEY_PIN);
}

//�����ػ������� PWRKEY ���Źػ�
//�ػ�ģʽ������STATUS �ܽ���ָʾ�� �͵�ƽָʾģ���ѽ���ػ�ģʽ��
//�ػ������У�ģ����Ҫע�� GSM ���磬ע��ʱ���뵱ǰ����״̬�йأ����ⶨ
//��ʱԼ 2s~12s����˽����ӳ� 12s ���ٶ�ģ����жϵ�������Ĳ�������ȷ������
//ȫ�ػ�֮ǰ������������Ҫ���ݡ�
void GPRS_PWRKEY_Off (void)
{
    GPIO_SetBits(GPRS_PWRKEY_PORT, GPRS_PWRKEY_PIN);
    wait(12);
    GPIO_ResetBits(GPRS_PWRKEY_PORT, GPRS_PWRKEY_PIN);
}

bool Uart_SendATCmd(const char *const cmd, const char *const result, uint32_t wait_ms, uint8_t times)
{
    uint8_t _times = 1;
    while (1)
    {
        if(_times++ > times)
            return false;
        vSerialPutString( xPort, (int8_t*)cmd, strlen(cmd) );
        int sum=0;
        int len = strlen(result);
        for (;;)
        {
            int8_t c;
            if (pdTRUE == xSerialGetChar( xPort, &c, pdMS_TO_TICKS( wait_ms ) ))
            {
                sum = (c==result[sum]) ? sum+1 : 0;
                if(sum == len)
                    return true;
            }
            else
                
                break;
        }
    }
}

bool Uart_SendATCmdOut(const char *const cmd, const char *const result, const char *const out, uint32_t wait_ms, uint8_t times)
{
    uint8_t _times = 1;
    char *__out = (char *)out;
    
    while (1)
    {
        if(_times++ > times)
            return false;
        vSerialPutString( xPort, (int8_t*)cmd, strlen(cmd) );
        int sum=0;
        int len = strlen(result);
        for (;;)
        {
            int8_t c;
            if (pdTRUE == xSerialGetChar( xPort, &c, pdMS_TO_TICKS( wait_ms ) ))
            {
                *__out = c;
                __out++;
                sum = (c==result[sum]) ? sum+1 : 0;
                if(sum == len)
                    return true;
            }
            else
                break;
        }
    }
}


/*
AT+CGSN

869286031588317

OK
*/
void get_imei_form_str(const uint8_t *const out_buf, char *in_str)
{
    uint8_t *_out_buf = (uint8_t *)out_buf;
    char *p = strstr(in_str, "\r\r\n") + strlen("\r\r\n");
    uint64_t num = atoll(p);
#if 0
    _out_buf[9] = num / 1 % 100;
    _out_buf[8] = num / 100 % 100;
    _out_buf[7] = num / 10000 % 100;
    _out_buf[6] = num / 1000000 % 100;
    _out_buf[5] = num / 100000000 % 100;
    _out_buf[4] = num / 10000000000 % 100;
    _out_buf[3] = num / 1000000000000 % 100;
    _out_buf[2] = num / 100000000000000 % 100;
    _out_buf[1] = num / 10000000000000000 % 100;
    _out_buf[0] = num / 1000000000000000000 % 100;
#else
    _out_buf[9] = (num >>  0) & 0xFF;
    _out_buf[8] = (num >>  8) & 0xFF;
    _out_buf[7] = (num >> 16) & 0xFF;
    _out_buf[6] = (num >> 24) & 0xFF;
    _out_buf[5] = (num >> 32) & 0xFF;
    _out_buf[4] = (num >> 40) & 0xFF;
    _out_buf[3] = (num >> 48) & 0xFF;
    _out_buf[2] = (num >> 56) & 0xFF;
    _out_buf[1] = (num >> 64) & 0xFF;
    _out_buf[0] = (num >> 72) & 0xFF;
    //memcpy((uint8_t *)&_out_buf[0], (uint8_t *)&num, sizeof(uint64_t));
#endif
}

/*
AT

AT+CGSN

869286031588317

OK

OK
AT+CGATT=1

OK
AT+CIPMODE=1

OK
AT+CSTT="CMNET"

OK
AT+CIICR

OK
AT+CIFSR

10.200.140.198
AT+CIPSTART="TCP","water.51204433.com",9992

OK

CONNECT
*/

void NET_GPRS_Config(void)
{
    char at_result[100] = {0};
netRoot:
    GPRS_PWRKEY_Init();
    GPRS_PWRKEY_On();
    wait(2);

        //printf("AT\r\n");OK
        if (!Uart_SendATCmd("AT\r\n", "OK", 1000, 5))
            goto netRoot;
            
        memset(at_result, 0x00, sizeof(at_result));
        if (!Uart_SendATCmdOut("AT+CGSN\r\n", "OK", at_result, 1000, 5))
            ;
        //printf(at_result);
        //\r\nAT+CGSN\r\r\n866050034443331\r\n\r\nOK
        if (strstr(at_result, "OK") != NULL)//��IMEI��Ū����
        {
            uint8_t iccid[10] = {0};
            get_imei_form_str(iccid, at_result);
            if (0 != memcmp(g_data_format.iccid, iccid, sizeof(g_data_format.iccid)))//��e2prom�ж�������ֵӦ���Ǻ�2gģ�鷵�ص�һ���Ŷ�
            {
                memcpy(g_data_format.iccid, iccid, sizeof(g_data_format.iccid));
                I2C_EE_BufferWriteSafe(0+sizeof(save_data_default), (uint8_t *)&g_data_format.iccid[0], sizeof(g_data_format.iccid));
                NVIC_SystemReset();//����
            }
        }
        
        //printf("AT+CPIN?\r\n");OK
        if (!Uart_SendATCmd("AT+CPIN?\r\n", "OK", 1000, 5))
            ;//continue;
        vTaskDelay(5000);
//        //������Ϣ{
//        //��������ź�ǿ��
//        printf("AT+CSQ\r\n");
//        if (!Uart_SendATCmd("AT+CSQ\r\n", "OK", 1000, 5))
//            ;//continue;
//        //��ѯ��ǰ��Ӫ�̣���ָ��ֻ������������󣬲ŷ�����Ӫ�̣����򷵻ؿ�
//        printf("AT+COPS?\r\n");
//        if (!Uart_SendATCmd("AT+COPS?\r\n", "OK", 1000, 5))
//            ;//continue;
//        //��ѯģ���ͺţ��緵�أ� SIMCOM_SIM800C
//        printf("AT+CGMM\r\n");
//        if (!Uart_SendATCmd("AT+CGMM\r\n", "OK", 1000, 5))
//            ;//continue;
//        //��ѯ�������룬������ SIM ����λ��ʱ��ſ��Բ�ѯ
//        printf("AT+CNUM\r\n");
//        if (!Uart_SendATCmd("AT+CNUM\r\n", "OK", 2000, 5))
//            ;//continue;
//        //}
        //͸��{
        //���ø��źͷ��� GPRS ҵ��
        //printf("AT+CGATT=1\r\n");
        
        if (!Uart_SendATCmd("AT+CGCLASS=\"B\"\r\n", "OK", 2000, 10));
        
        if (!Uart_SendATCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", "OK", 2000, 10));
        
        vTaskDelay(1000);  
    
        if (!Uart_SendATCmd("AT+CGATT=1\r\n", "OK", 2000, 10))//���׳�������Ͳ����ٴ����ˣ�����Ҳ��ʧ�ܵ�
            ;//continue;
        //printf("AT+CIPMODE=1\r\n");
        if (!Uart_SendATCmd("AT+CIPMODE=1\r\n", "OK", 1000, 5))
            ;//continue;
        //��ʼ��������APN
        //printf("AT+CSTT=\"CMNET\"\r\n");
        if (!Uart_SendATCmd("AT+CSTT=\"CMNET\"\r\n", "OK", 2000, 5))
            ;//continue;

        vTaskDelay(1000);    
            
/*
AT+CSQ

+CSQ: 24,0

OK
*/
        memset(at_result, 0x00, sizeof(at_result));
        if (!Uart_SendATCmdOut("AT+CSQ\r\n", "OK", at_result, 1000, 5))
            ;
        if (strstr(at_result, "+CSQ: ") != NULL)//��IMEI��Ū����
        {
            water.rssi = atoi(strstr(at_result, "+CSQ: ")+strlen("+CSQ: "));
        }
        
/*
AT+CGREG=2

OK
*/
        if (!Uart_SendATCmd("AT+CGREG=2\r\n", "OK", 1000, 5))
            ;//continue;
/*    
AT+CGREG?

+CGREG: 2,1,"3361","EB15"

OK
*/
        memset(at_result, 0x00, sizeof(at_result));
        if (!Uart_SendATCmdOut("AT+CGREG?\r\n", "OK", at_result, 1000, 5))//lac cid
            ;
        if (strstr(at_result, "+CGREG: 2") != NULL)
        {
            char *str;
            char *index = NULL;
            
            index = strstr(at_result, ",\"") + strlen(",\"");
            water.lac_value = (int)strtol(index, &str, 16);//ʮ������  
            
            index = strstr(index, ",\"") + strlen(",\"");
            water.cid_value = (int)strtol(index, &str, 16);//ʮ������  
        }
        
        //����������·
        //printf("AT+CIICR\r\n");
        if (!Uart_SendATCmd("AT+CIICR\r\n", "OK", 2000, 5))
            ;//continue;
        //��ñ���IP��ַ
        //printf("AT+CIFSR\r\n");10.177.167.229
        if (!Uart_SendATCmd("AT+CIFSR\r\n", ".", 2000, 5))
            ;//continue;
        //}
        
//        printf("AT+CIPSTART=\"TCP\",\"water.51204433.com\",9992\r\n");
        //if (!Uart_SendATCmd("AT+CIPSTART=\"TCP\",\"water.51204433.com\",\"9992\"\r\n", "CONNECT", 2000, 30))
        if (!Uart_SendATCmd("AT+CIPSTART=\"TCP\",\"lot.youheone.com\",\"9992\"\r\n", "CONNECT", 2000, 30))
        //if (!Uart_SendATCmd("AT+CIPSTART=\"TCP\",\"122.114.122.174\",\"42127\"\r\n", "CONNECT", 2000, 30))
        {
            //printf("connect failed\n");
            goto netRoot;
        }
        
        
        //�����ɹ��󣬷�����������
        for (int i = 0; i < 2; i++)
        {
            BEEP_ON;
            vTaskDelay(100);  
            BEEP_OFF;
            vTaskDelay(100);
        }
        
        uint8_t buf[100] = {0};
        int len = xSerialGetBytes( xPort, buf, ( TickType_t ) pdMS_TO_TICKS( 3000 ));
        //printf("GPRS Net Config OK!");
        
        vTaskDelay(100); 
        edp.is_tcpip_connect = true;
        
}
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vEdpRecvProcessTask, pvParameters )
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
	for ( ;; )
	{
        //��ȡ��ǰ����״̬
        if (!edp.is_tcpip_connect)
        {
//            if (NULL != xedpNetPingHandlerTask)
//                vTaskDelete( &xedpNetPingHandlerTask );
            
            NET_GPRS_Config();
            edp.is_ping_send = false;
            EDP_SendPacketPing( );
            
            
            
            extern bool is_send_device_status;
            is_send_device_status = true;
            edp.is_ping_send = true;
            edp.is_ping_resp = false;
            send_ping_again = 0;
            //xTaskCreate( AppTaskEdpNetPing, "PING", configMINIMAL_STACK_SIZE, NULL, edpNET_PING_TASK_PRIORITY, ( TaskHandle_t * ) &xedpNetPingHandlerTask );

        }
        else
        {
            //uint8_t *buf = NULL;
            //buf = (uint8_t*)pvPortMalloc(1000 * sizeof(uint8_t));
            
            
            //if (NULL == buf)
            //    for(;;);
            
            uint8_t buf[1500] = {0};
            
            int len = xSerialGetBytes( xPort, buf, ( TickType_t ) pdMS_TO_TICKS( 100 ));
            
            if (len > 0)
            {
                    
#if 0
                printf("len = %d\r\n", len);
                for (int i = 0; i < len; i++)
                {
                    printf(" %02x", buf[i]);
                }
                printf("\n");
#endif

                if (strstr((char*)buf, "CLOSED") != NULL)
                    edp.is_tcpip_connect = false;
                else
                {
                    if (0 == memcmp((uint8_t *)buf, g_data_format.iccid, 10))
                        ProtocolProcess((uint8_t *)buf, len);
                }
            }
            //vPortFree(buf);
        }
# if (INCLUDE_uxTaskGetStackHighWaterMark)
        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
        printf("vEdpRecvProcessTask space avaiable = %d\r\n", uxHighWaterMark);
#endif
	}
} /*lint !e715 !e818 pvParameters is required for a task function even if it is not referenced. */
/*-----------------------------------------------------------*/




static portTASK_FUNCTION( AppTaskEdpNetPing, pvParameters )
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    for (;;)
    {
        if (edp.is_tcpip_connect)
        {
            vTaskDelay( pdMS_TO_TICKS(2 * 1000) );
            //printf("����ping\r\n");
            if (send_ping_again == 0)
            {
                edp.is_ping_send = false;
                EDP_SendPacketPing( );
                
                edp.is_ping_send = true;
                edp.is_ping_resp = false;
                send_ping_again = 0;
            }
            
            /* We want this task to execute exactly every 3 minutes. */
            vTaskDelay( pdMS_TO_TICKS(abs(water.save.ping_time-2) * 1000) );
        }
        else
        {
            vTaskDelay( pdMS_TO_TICKS(1000) );
        }
    }
}

int count__ = 0;
static portTASK_FUNCTION( AppTaskEdpNetCheck, pvParameters )
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    //printf("����EDP ping�źż������\r\n");
    
    
    for (;;)
    {
        
        if (edp.is_ping_send == true)
        {
            if (edp.is_ping_resp == true)//�������������з��أ�֤����������
            {
                edp.is_ping_send = false;
                edp.is_ping_resp = false;
                send_ping_again = 0;
            }
            if (count__++ >= 10)//�ȴ�30s����û�����緵�أ�˵���������
            {
                count__ = 0;
                extern bool is_iap_status;
                if (!is_iap_status)//�]��������ʱ���жϳ�ʱû�л�Ӧʱ����Ϊ������ϡ�
                {
                    if (send_ping_again < 2)
                    {
                        send_ping_again++;
                        EDP_SendPacketPing( );
                        edp.is_ping_send = true;
                        edp.is_ping_resp = false;
                    }
                    else
                    {
                        edp.is_tcpip_connect = false;//�����쳣����������
                        edp.is_ping_send = false;
                        edp.is_ping_resp = false;
                    }
                }
            }
        }
        else
            count__ = 0;
        
        vTaskDelay ( pdMS_TO_TICKS(1000) );
        
    }
}

void AppEdpNetCheckGive(void)
{
    xTaskNotifyGive( xedpNetCheckHandlerTask );
}


void prvGprsTask( void *pvParameters )
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    GPRS_PWRKEY_Init();
    
	xTaskCreate( vEdpRecvProcessTask, "EDP RECV PROCESS", 1000, NULL, tskIDLE_PRIORITY + 3, ( TaskHandle_t * ) &xEdpRecvProcessTaskHandle );
    //����Edp Net Check���񣬼��edp�������Ƿ�����
    xTaskCreate( AppTaskEdpNetCheck, "EDP NET CHECK", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, &xedpNetCheckHandlerTask );
    
    xTaskCreate( AppTaskEdpNetPing, "PING", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, ( TaskHandle_t * ) &xedpNetPingHandlerTask );
    
    vTaskDelete( NULL );
}


