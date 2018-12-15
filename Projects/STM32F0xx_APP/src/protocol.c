#include "protocol.h"
#include "crc_soft.h"
#include "serial.h"
#include "rtc.h"
#include "e2prom.h"
#include "gprs.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "menu.h"
#include "common.h"

//˵�����̼��汾��V1_1.0.3��V0��ʾbootloader��V1Ϊ�豸A�����й̼����汾��V2Ϊ�豸B�����й̼�
#if BOOTLOADER
#define HW_VERSION         "V0_0.1.7"          //V0��ʾbootloader
#elif APP_A
#define HW_VERSION         "V1_0.1.7"          //V1Ϊ�豸A�����й̼�
#elif APP_B
#define HW_VERSION         "V2_0.1.7"          //V2Ϊ�豸B�����й̼�
#endif


#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/*
typedef struct{
    uint8_t iccid[10];
    uint8_t cmd;
    uint16_t length;
    uint8_t data[100];//�䳤�����ݾ��幦�ܾ���
    uint8_t crc[2];
}data_format_t;
*/

data_format_t g_data_format = {
    .iccid = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09}
};



//�ϵ�ĳ�ʼ��ֵ
struct water_s water = 
{
    .device_status = 0,//����
    .is_screen_status_off = 0,//��Ļ�򿪣��ϵ�Ĭ��ֵ��
    .used_days = 0,
    .remain_days = 0,
    .filter_remain_value_1 = 0,//��һ����оʣ��ֵ	HEX��2�ֽڣ�
    .filter_remain_value_2 = 0,//�ڶ�����оʣ��ֵ	HEX��2�ֽڣ�
    .filter_remain_value_3 = 0,//��������оʣ��ֵ	HEX��2�ֽڣ�
    .filter_remain_value_4 = 0,//���ļ���оʣ��ֵ	HEX��2�ֽڣ�
    .filter_remain_value_5 = 0,//���弶��оʣ��ֵ	HEX��2�ֽڣ�
    .rssi = 0,//�ź�ǿ��ֵ
    .lac_value = 0,
    .cid_value = 0,
    .date_time = 0,
    .is_make_water_status = 0,
    .is_full_water_status = 0,
    .is_overhaul_status = 0,
    .is_drop_water_status = 0,
    .is_power_on_status = 1,
    .is_shortage_water_status = 0,
    .is_bushing_now = 0,
    .is_test_mode_switch_off = 1,//�رյ���ģʽ
    .test_used_days = 0,
    .test_used_flow = 0,
    .flowmeter_count = 0,//������
    .raw_water_ppm = 0,////ʵ�ʼ�⵽��ppmֵ
    .clean_water_ppm = 0,////ʵ�ʼ�⵽��ppmֵ
    .overhaul_status_switch = 0x00,//0x00�������  //0x01�������
    .io_high_status = -1,//��¼IO1״̬,��ѹ����״̬
    .io_high_last_status = -1,
    .io_low_status = -1,//��¼IO2״̬,��ѹ����״̬
    .io_low_last_status = -1
};

//�ϵ�ĳ�ʼ��ֵ�����Ҫ�ָ���������Ҫ
struct water_s water_default = 
{
    .device_status = 0,//����
    .is_screen_status_off = 0,//��Ļ�򿪣��ϵ�Ĭ��ֵ��
    .used_days = 0,
    .remain_days = 0,
    .filter_remain_value_1 = 0,//��һ����оʣ��ֵ	HEX��2�ֽڣ�
    .filter_remain_value_2 = 0,//�ڶ�����оʣ��ֵ	HEX��2�ֽڣ�
    .filter_remain_value_3 = 0,//��������оʣ��ֵ	HEX��2�ֽڣ�
    .filter_remain_value_4 = 0,//���ļ���оʣ��ֵ	HEX��2�ֽڣ�
    .filter_remain_value_5 = 0,//���弶��оʣ��ֵ	HEX��2�ֽڣ�
    .rssi = 0,//�ź�ǿ��ֵ
    .lac_value = 0,
    .cid_value = 0,
    .date_time = 0,
    .is_make_water_status = 0,
    .is_full_water_status = 0,
    .is_overhaul_status = 0,
    .is_drop_water_status = 0,
    .is_power_on_status = 1,
    .is_shortage_water_status = 0,
    .is_bushing_now = 0,
    .is_test_mode_switch_off = 1,//�رյ���ģʽ
    .test_used_days = 0,
    .test_used_flow = 0,
    .flowmeter_count = 0,//������
    .raw_water_ppm = 0,////ʵ�ʼ�⵽��ppmֵ
    .clean_water_ppm = 0,////ʵ�ʼ�⵽��ppmֵ
    .overhaul_status_switch = 0x00,//0x00�������  //0x01�������
    .io_high_status = -1,//��¼IO1״̬,��ѹ����״̬
    .io_high_last_status = -1,
    .io_low_status = -1,//��¼IO2״̬,��ѹ����״̬
    .io_low_last_status = -1
};

//������e2prom�����Ĭ��ֵ
const struct save_data save_data_default = 
{
    .e2prom_init = 123456,
    .is_bind = 0,
    .bind_timestamp = 0,//��¼�������ʱ���
    .work_mode = 0,//0	����ģʽ    1	ʱ��ģʽ
    .used_flow = 0,
    .remain_flow = 0,
    .used_days_timestamp = 0,//��������ʱ���
    .remain_days_timestamp = 0,//ʣ������ʱ���
    .clean_water_ppm = 0,
    .clean_water_tds_switch = 1,//0��ʾĬ��ֵ��1��ʾʵʱֵ
    .raw_water_ppm = 0,
    .raw_water_tds_switch = 1,//0��ʾĬ��ֵ��1��ʾʵʱֵ
    .filter_used_timestamp_1 = 0,//��о�Ŀ�ʼʹ��ʱ���
    .filter_used_timestamp_2 = 0,//��о�Ŀ�ʼʹ��ʱ���
    .filter_used_timestamp_3 = 0,//��о�Ŀ�ʼʹ��ʱ���
    .filter_used_timestamp_4 = 0,//��о�Ŀ�ʼʹ��ʱ���
    .filter_used_timestamp_5 = 0,//��о�Ŀ�ʼʹ��ʱ���
    .filter_max_value_1 = 0,
    .filter_max_value_2 = 0,
    .filter_max_value_3 = 0,
    .filter_max_value_4 = 0,
    .filter_max_value_5 = 0,
    .force_flush_time = 18,//18���ǿ�Ƴ�ϴ
    .period_flush_time = 3600*2,//�ۼ���ˮ2Сʱ����ǿ�Ƴ�ϴ
    .maintenance_time = 8*3600,//������ˮ8Сʱ�������״̬
    .ping_time = 180,//�����������ʱ��180��
    .reconnect_time = 0,
    .screen_mode_switch.day = 0x00,//��������
    .screen_mode_switch.flow = 0x02//��������
};

//int buf_to_struct(data_format_t *data_format, uint8_t *buf, int buf_len);
//int struct_to_buf(uint8_t *buf, int *buf_len, data_format_t data_format);


const cmd_process_t cmd_process[] = {
    {CMD_HEARTBEAT, cmd_heartbeat_process},//0x01,//����֡
    {CMD_BINDING_PACKAGE, cmd_binding_package_process},//0x02,//���ײ�
    {CMD_CLOSE_THE_SCREEN, cmd_close_the_screen_process},//0x03,//�ر���Ļ
    {CMD_OPEN_THE_SCREEN, cmd_open_the_screen_process},//0x04,//����Ļ
    {CMD_POWER_OFF, cmd_power_off_process},//0x05,//�ػ�
    {CMD_POWER_ON, cmd_power_on_process},//0x06,//����
    {CMD_STRONG_PUNCH, cmd_strong_push_process},//0x07,//ǿ��
    {CMD_POSITIVE_VALUE, cmd_positive_value_process},//0x08,//����ֵ
    
    {CMD_FILTER_SEND, cmd_filter_send_process},//�û��豸�����ϱ���о״̬��ÿ���ϱ�һ�εķ�ʽ
    {CMD_FILTER_REQUIRE, cmd_filter_require_process},//������ȡ��о״̬
    
    {CMD_SYNCHRONIZE_WITH_WATER, cmd_synchronize_with_water_process},//0x0a,//��ˮͬ��
    //{CMD_TIME_SYNCHRONIZATION, cmd_time_synchronization_process},//0x0b,//��ʱͬ��
    {CMD_TIME_SYNCHRONIZATION_USED, cmd_time_synchronization_used_process},//��ʱͬ�� 0xBA��������
    {CMD_TIME_SYNCHRONIZATION_REMAIN, cmd_time_synchronization_remain_process},//��ʱͬ�� 0xBBʣ������
    {CMD_WORK_STATUS_SYNCHRONIZATION, cmd_work_status_synchronization_process},//0x0c,//����״̬ͬ��
    {CMD_QUERY_DEVICE_OPERATION_INFORMATION, cmd_query_device_operation_information_process},//0x0d,//��ѯ�豸������Ϣ
    {CMD_FILTER_RESET_AND_MODIFICATION, cmd_filter_reset_and_modification_process},//0x0e,//��о��λ���޸�
    {CMD_MODE_SWITCHING, cmd_mode_switching_process},//0x0f,//ģʽ�л�
    {CMD_RESET, cmd_reset_process},//0x10,//�ָ���������
    //�޸������Ͷ˿ں�
    {CMD_PARAMETER_MODIFICATION, cmd_parameter_modification_process},//0x11,//�����޸�
    {CMD_TIMING_FLUSH_PARAMETER_MODIFICATION, cmd_timing_flush_parameter_modification_process},//0x12,//��ʱ��ϴ�����޸�
    {CMD_MAINTENANCE_PARAMETER_MODIFICATION, cmd_maintenance_parameter_modification_process},//0x13,//���޲����޸�
    {CMD_CONTROL_PARAMETER_MODIFICATION_1, cmd_control_parameter_modification_1_process},//0x14,//���Ʋ����޸�1
    {CMD_CONTROL_PARAMETER_MODIFICATION_2, cmd_control_parameter_modification_2_process},//0x15,//���Ʋ����޸�2
    {CMD_TEST_MODE_SWITCH, cmd_test_mode_switch_process},//0x16,//�����رղ���ģʽ
    {CMD_COMPUTER_BOARD_TIME_SYNCHRONIZATION_1, cmd_computer_board_time_synchronization_1},//0x17,// ���԰�ʱ��ͬ��1
    {CMD_COMPUTER_BOARD_TIME_SYNCHRONIZATION_2, cmd_computer_board_time_synchronization_2},//0x18,// ���԰�ʱ��ͬ��2
    {CMD_SYNCHRONIZATION_OF_WATER_CONSUMPTION_USED, cmd_synchronization_of_water_consumption_used},//0xAA,// ��ˮ��ͬ�� 0xAA��������
    {CMD_SYNCHRONIZATION_OF_WATER_CONSUMPTION_REMAIN, cmd_synchronization_of_water_consumption_remain},//0xAB,// ��ˮ��ͬ�� 0xABʣ������
    
    {CMD_REMOTE_UPGRADE_VERSION, cmd_remote_upgrade_version},//����˻�ȡ�̼���ǰ�汾��
    {CMD_REMOTE_UPGRADE_DATA0, cmd_remote_upgrade_data0},//Զ�������ļ���
    {CMD_REMOTE_UPGRADE_DATA1, cmd_remote_upgrade_data1},//Զ�������ļ�
    {CMD_REMOTE_UPGRADE_DATA2, cmd_remote_upgrade_data2},//Զ����������
    {CMD_REMOTE_UPGRADE_REBOOT, cmd_remote_upgrade_reboot},//Զ������
    {CMD_REMOTE_UPGRADE_NEW, cmd_remote_upgrade_new},//�´������豸ʱ�Ƿ������¹̼�
    {CMD_REMOTE_UPGRADE_SWITCH, cmd_remote_upgrade_switch},//���й̼��л�
    
    {CMD_SCREEN_MODE_SWITCH, cmd_screen_mode_switch},//��Ļ��ʾģʽ�л�0x5A
    {CMD_OVERHAUL_STATUS_SWITCH, cmd_overhaul_status_switch}//����״̬�л�0x6A
};

const uint8_t cmd_process_cnt = sizeof( cmd_process ) / sizeof( cmd_process[0] );

//���԰������ϴ�����Ӧ��
int cmd_heartbeat_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x01,//����֡
{
    int ret = 0;
    
    //void AppEdpNetCheckGive(void);
    //AppEdpNetCheckGive();
    edp.is_ping_resp = true;
    
    return ret;
}

int cmd_binding_package_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x02,//���ײ�
{
    int ret = 0;
    
    if ((0x00 == in_buf[0]) || (0x01 == in_buf[0]))
    {
        water.save.is_bind = 1;
        water.save.work_mode = in_buf[0];//00������ģʽ   01��ʱ��ģʽ
        water.save.filter_max_value_1 = (in_buf[1] << 8) + (in_buf[2] << 0);
        water.save.filter_max_value_2 = (in_buf[3] << 8) + (in_buf[4] << 0);
        water.save.filter_max_value_3 = (in_buf[5] << 8) + (in_buf[6] << 0);
        water.save.filter_max_value_4 = (in_buf[7] << 8) + (in_buf[8] << 0);
        water.save.filter_max_value_5 = (in_buf[9] << 8) + (in_buf[10] << 0);
        
        //�Ӱ󶨿�ʼ�洢ʱ���
        time_t now = time(NULL);
        water.save.bind_timestamp = now;
        water.save.filter_used_timestamp_1 = now;
        water.save.filter_used_timestamp_2 = now;
        water.save.filter_used_timestamp_3 = now;
        water.save.filter_used_timestamp_4 = now;
        water.save.filter_used_timestamp_5 = now;
        
        water.save.used_days_timestamp = now;
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}

int cmd_close_the_screen_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x03,//�ر���Ļ
{
    int ret = 0;
    
    //if (0x00 == in_buf[0])
    {
        water.is_screen_status_off = 1;//�ر���Ļ
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}

int cmd_open_the_screen_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x04,//����Ļ
{
    int ret = 0;
    
    //if (0x00 == in_buf[0])
    {
        
        water.is_screen_status_off = 0;//����Ļ
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}

int cmd_power_off_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x05,//�ػ�
{
    int ret = 0;
    
    //if (0x00 == in_buf[0])
    {
        if (1 == water.save.is_bind)//���ڹرգ����ر���ˮ���ܡ�������ײͺ��������������Ӧ��
        {
            //6	�ػ�
            //�ر������ʱ������������еĻ�
            water.is_bushing_now = 0;
            extern TimerHandle_t xForceFlushTimerHandler;
            if( xTimerIsTimerActive( xForceFlushTimerHandler ) != pdFALSE )
            {
                xTimerStop( xForceFlushTimerHandler, 100 );
            }
            
            water.is_power_on_status = 0;
            *out_len = in_len;
            memcpy(out_buf, in_buf, *out_len);
            ret = 1;
        }
    }
    return ret;
}

int cmd_power_on_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x06,//����
{
    int ret = 0;
    
    //if (0x00 == in_buf[0])
    {
        if (1 == water.save.is_bind)//���ڿ�������������ˮ���ܡ�������ײͺ��������������Ӧ��
        {
            water.is_power_on_status = 1;
            water.io_high_status = -1;//��¼IO1״̬,��ѹ����״̬
            water.io_high_last_status = -1;
            water.io_low_status = -1;//��¼IO2״̬,��ѹ����״̬
            water.io_low_last_status = -1;
            
            *out_len = in_len;
            memcpy(out_buf, in_buf, *out_len);
            ret = 1;
        }
    }
    return ret;
}

int cmd_strong_push_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x07,//ǿ��
{
    int ret = 0;
    
    if (0x00 == in_buf[0])
    {
        if (1 == water.save.is_bind)//����ǿ�Ƴ�ϴ��������ײͺ��������������Ӧ��
        {
            //TODO
            //����ǿ�Ƴ�ϴ
            if ((water.is_overhaul_status != 1) && (water.is_drop_water_status != 1) && (water.is_power_on_status != 0) && (water.is_shortage_water_status != 1))
            {
                extern TimerHandle_t xForceFlushTimerHandler;
                if( xTimerStart( xForceFlushTimerHandler, pdMS_TO_TICKS(100) ) == pdPASS )
                {
                    water.is_bushing_now = 1;
                }
            }
            
            *out_len = in_len;
            memcpy(out_buf, in_buf, *out_len);
            ret = 1;
        }
    }
    return ret;
}

/*
����ͬ��������������������ײͺ��������������Ӧ��
1�������ʱ��ģʽ�������ʾ����������ʣ����������������
2�����������ģʽ�������ʾ����������ʣ������������������ʣ������
3������л��ײͣ�����ʱ��ģʽ�л�Ϊ����ģʽ��ƽ̨��Ҫ�����·��ײͼ���Ӧ������������ʣ������������������ʣ���������豸������л�����ģʽΪʱ��ģʽ��ƽ̨Ҳ��Ҫ�����·��ײͼ���Ӧ������������ʣ����������������
*/
int cmd_positive_value_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x08,//��ȷ��ֵ
{
    int ret = 0;
    
    if (1 == water.save.is_bind)//����ͬ��������������������ײͺ��������������Ӧ��
    {
        /*
        0	����ģʽ
        1	ʱ��ģʽ
        */
        {
            water.used_days = (in_buf[0] << 8) + in_buf[1];
            water.remain_days = (in_buf[2] << 8) + in_buf[3] + 1;
            
            water.save.used_flow = (uint32_t)((((uint32_t)in_buf[4] << 24) + ((uint32_t)in_buf[5] << 16) + ((uint32_t)in_buf[6] << 8) + ((uint32_t)in_buf[7] << 0)) / 0.917 + 0.5);
            if (water.save.work_mode == 0)//0	����ģʽ
            {
                water.save.remain_flow = (uint32_t)((((uint32_t)in_buf[8] << 24) + ((uint32_t)in_buf[9] << 16) + ((uint32_t)in_buf[10] << 8) + ((uint32_t)in_buf[11] << 0)) / 0.917 + 0.5);
            }
            else
            {
                water.save.remain_flow = 0xFFFFFFFF;
            }
            
            time_t now = time(NULL);
            water.save.used_days_timestamp = now - water.used_days * 3600 * 24;
            water.save.remain_days_timestamp = now + water.remain_days * 3600 * 24;
                        
            *out_len = in_len;
            memcpy(out_buf, in_buf, *out_len);
            ret = 1;
        }
    }
    return ret;
}

int cmd_filter_send_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //�û��豸�����ϱ���о״̬��ÿ���ϱ�һ�εķ�ʽ
{
    int ret = 0;
    return ret;
}

int cmd_filter_require_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //������ȡ��о״̬
{
    int ret = 0;
    //��о״̬�ϴ�
    void EDP_SendFilter(void);
    EDP_SendFilter();
    return ret;
}

//��ˮ���ϴ�
//ÿ��ˮ��ͷֹͣ��ˮ5����ϴ�������ˮ������λ/10ml��
//���԰������ϴ�����Ӧ��
int cmd_synchronize_with_water_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x0a,//��ˮͬ��
{
    int ret = 0;
    
    return ret;
}

/*
int cmd_time_synchronization_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x0b,//��ʱͬ��
{
    int ret = 0;
    
    if (1 == water.save.bind)//������ײͺ��������������Ӧ��
    {
        water.used_days = (in_buf[0] << 8) + in_buf[1];
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}
*/

int cmd_time_synchronization_used_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//��ʱͬ�� 0xBA��������
{
    int ret = 0;
    
    if (1 == water.save.is_bind)//������ײͺ��������������Ӧ��
    {
        uint16_t used_days = (in_buf[0] << 8) + in_buf[1];
        time_t now = time(NULL);
        //�޸��Ѿ����˶������ʱ��꣬���ȥ������
        water.save.used_days_timestamp = now - used_days * 3600 * 24;
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}

int cmd_time_synchronization_remain_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//��ʱͬ�� 0xBBʣ������
{
    int ret = 0;
    
    if (1 == water.save.is_bind)//������ײͺ��������������Ӧ��
    {
        time_t now = time(NULL);
        water.remain_days = (in_buf[0] << 8) + in_buf[1] + 1;
        water.save.remain_days_timestamp = now + water.remain_days*3600*24,
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}

/*
0x01��ˮ
0x02ˮ��
0x03ȱˮ
0x04����
*/
//�豸����״̬����ʱ�������ϴ�����Ϣ,���԰������ϴ�����Ӧ��
int cmd_work_status_synchronization_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x0c,//����״̬ͬ��
{
    int ret = 0;
    
    extern bool SendPacketStatusSuccess;
    SendPacketStatusSuccess = true;//�h�����ܵ����ص������ˣ��Ͳ���Ҫ�ٴ��ϴ��豸״̬��
    
    return ret;
}

//���̷���������Ϣ��������
int cmd_query_device_operation_information_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x0d,//��ѯ�豸������Ϣ
{
    int ret = 0;
    
    void EDP_SendPacketPing(void); //0x01,//����֡
    EDP_SendPacketPing();
    return ret;
}

int cmd_filter_reset_and_modification_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x0e,//��о��λ���޸�
{
    int ret = 0;
    
    if (1 == water.save.is_bind)//������ײͺ��������������Ӧ��
    {
        uint16_t remain_value = (in_buf[1] << 8) + in_buf[2];
        time_t now = time(NULL);
        
        switch (in_buf[0])
        {
        case 0x00: break;
        case 0x01: water.save.filter_used_timestamp_1 = now; break;//�޸��Ѿ�ʹ�õ�������¼��ʱ��죬��ô�������û�б䣬ʣ���������������������Ѿ�ʹ�õ���������0
        case 0x02: water.save.filter_used_timestamp_2 = now; break;
        case 0x03: water.save.filter_used_timestamp_3 = now; break;
        case 0x04: water.save.filter_used_timestamp_4 = now; break;
        case 0x05: water.save.filter_used_timestamp_5 = now; break;
        case 0x06: 
            water.save.filter_used_timestamp_1 = now;
            water.save.filter_used_timestamp_2 = now;
            water.save.filter_used_timestamp_3 = now;
            water.save.filter_used_timestamp_4 = now;
            water.save.filter_used_timestamp_5 = now; 
            break;
        case 0x07:
            if (remain_value > water.save.filter_max_value_1) 
                water.save.filter_max_value_1 = remain_value; 
            //�޸�����о��ʣ��ֵ��Ȼ����оʹ�õ��������� ���ֵ-ʣ��ֵ��Ȼ��������õ����������������õ�ʱ��������������ǰ�˶�ã��Ӷ������õ�һ��ʹ�ö�ʱ��ꡣ��������ʱ�򣬾�����������ʱ���-��һ��ʹ�õ�ʱ��꣬������о�Ѿ�ʹ�õ������ˡ�ʣ��ֵ�������ֵ-ʣ��ֵ
            water.save.filter_used_timestamp_1 = now - (water.save.filter_max_value_1 - remain_value)*3600*24; //��ǰ��ʱ��� - �Ѿ���ʱ��꣬Ȼ�󱣴���������൱���޸�����о��ʼʹ�õ�ʱ���
            break;
        case 0x08:
            if (remain_value > water.save.filter_max_value_2) 
                water.save.filter_max_value_2 = remain_value; 
            water.save.filter_used_timestamp_2 = now - (water.save.filter_max_value_2 - remain_value)*3600*24; //��ǰ��ʱ��� - �Ѿ���ʱ��꣬Ȼ�󱣴���������൱���޸�����о��ʼʹ�õ�ʱ���
            break;
        case 0x09:
            if (remain_value > water.save.filter_max_value_3)
                water.save.filter_max_value_3 = remain_value; 
            water.save.filter_used_timestamp_3 = now - (water.save.filter_max_value_3 - remain_value)*3600*24; //��ǰ��ʱ��� - �Ѿ���ʱ��꣬Ȼ�󱣴���������൱���޸�����о��ʼʹ�õ�ʱ���
            break;
        case 0x0a:
            if (remain_value > water.save.filter_max_value_4) 
                water.save.filter_max_value_4 = remain_value; 
            water.save.filter_used_timestamp_4 = now - (water.save.filter_max_value_4 - remain_value)*3600*24; //��ǰ��ʱ��� - �Ѿ���ʱ��꣬Ȼ�󱣴���������൱���޸�����о��ʼʹ�õ�ʱ���
            break;
        case 0x0b:
            if (remain_value > water.save.filter_max_value_5) 
                water.save.filter_max_value_5 = remain_value; 
            water.save.filter_used_timestamp_5 = now - (water.save.filter_max_value_5 - remain_value)*3600*24; //��ǰ��ʱ��� - �Ѿ���ʱ��꣬Ȼ�󱣴���������൱���޸�����о��ʼʹ�õ�ʱ���
            break;
        default:break;
        }
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}

//ģʽ�л�
int cmd_mode_switching_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x0f,//ģʽ�л�
{
    int ret = 0;
    /*
    0	����ģʽ
    1	ʱ��ģʽ
    */
    if ((0x00 == in_buf[0]) || (0x01 == in_buf[0]))
    {
        water.save.work_mode = in_buf[0];
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}

//���ڵ��԰�����ָ���������״̬
int cmd_reset_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x10,//�ָ���������
{
    int ret = 0;
    
    if (0x00 == in_buf[0])
    {
        //�ָ������ͽ���˽���
        memcpy(&water, &water_default, sizeof(water_default));
        memcpy(&water.save, &save_data_default, sizeof(save_data_default));
        //clear e2prom
        I2C_EE_BufferWriteSafe(0, (uint8_t *)&save_data_default, sizeof(save_data_default));
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}


//�޸������Ͷ˿ں�


//�޸�ǿ�Ƴ�ϴʱ��ϵͳ�����޸�
int cmd_parameter_modification_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x11,//�����޸�
{
    int ret = 0;
    
    water.save.force_flush_time = in_buf[0];
    
    extern TimerHandle_t xForceFlushTimerHandler;
    void vForceFlushTimerCallback( TimerHandle_t pxTimer );
    
    //ɾ����ʱ���������´�����ʱ��
    water.is_bushing_now = 0;
    xTimerDelete( xForceFlushTimerHandler, pdMS_TO_TICKS(100) );
    xForceFlushTimerHandler = xTimerCreate(    "Force Flush Timer",   // Just a text name, not used by the kernel.
                                 ( pdMS_TO_TICKS(water.save.force_flush_time*1000) ),   // The timer period in ticks.
                                 pdFALSE,        // The timers will auto-reload themselves when they expire.
                                 ( void * ) NULL, // Assign each timer a unique id equal to its array index.
                                 vForceFlushTimerCallback); // Each timer calls the same callback when it expires.
    
    *out_len = in_len;
    memcpy(out_buf, in_buf, *out_len);
    ret = 1;
    return ret;
}

//��ʱ��ϴ�����޸�
int cmd_timing_flush_parameter_modification_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x12,//��ʱ��ϴ�����޸�
{
    int ret = 0;
    
    water.save.period_flush_time = ((uint32_t)in_buf[0] << 16) + ((uint32_t)in_buf[1] << 8) + ((uint32_t)in_buf[2] << 0);
    
    *out_len = in_len;
    memcpy(out_buf, in_buf, *out_len);
    ret = 1;
    return ret;
}

//�޸Ľ������״̬ʱ��
int cmd_maintenance_parameter_modification_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x13,//���޲����޸�
{
    int ret = 0;
    
    water.save.maintenance_time = ((uint32_t)in_buf[0] << 16) + ((uint32_t)in_buf[1] << 8) + ((uint32_t)in_buf[2] << 0);
    *out_len = in_len;
    memcpy(out_buf, in_buf, *out_len);
    ret = 1;
    return ret;
}

//TDSֵ����
int cmd_control_parameter_modification_1_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x14,//���Ʋ����޸�1
{
    int ret = 0;
    
    if (0x01 == in_buf[0])//0x01��ˮ
    {
        if ((0x00 == in_buf[1]) || (0x01 == in_buf[1]))//TDS��⿪��00�أ�01��
        {
            water.save.clean_water_tds_switch = in_buf[1];//0��ʾĬ��ֵ��1��ʾʵʱֵ
            water.save.clean_water_ppm = (in_buf[2] << 8) + in_buf[3];
            
            *out_len = in_len;
            memcpy(out_buf, in_buf, *out_len);
            ret = 1;
        }
    }
    else if (0x02 == in_buf[0])//0x02ԭˮ
    {
        if ((0x00 == in_buf[1]) || (0x01 == in_buf[1]))//TDS��⿪��00�أ�01��
        {
            water.save.raw_water_tds_switch = in_buf[1];//0��ʾĬ��ֵ��1��ʾʵʱֵ
            water.save.raw_water_ppm = (in_buf[2] << 8) + in_buf[3];
            
            *out_len = in_len;
            memcpy(out_buf, in_buf, *out_len);
            ret = 1;
        }
    }
    return ret;
}

//���������޸�
int cmd_control_parameter_modification_2_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len) //0x15,//���Ʋ����޸�2
{
    int ret = 0;
    if (0x01 == in_buf[0])//0x01�������
    {
        water.save.ping_time = (in_buf[1] << 8) + in_buf[2];
    }
    else if (0x02 == in_buf[0])//0x02��������
    {
        water.save.reconnect_time = (in_buf[1] << 8) + in_buf[2];//δʹ��
    }
    
    *out_len = in_len;
    memcpy(out_buf, in_buf, *out_len);
    ret = 1;
    return ret;
}

int cmd_test_mode_switch_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0x16,//�����رղ���ģʽ
{
    int ret = 0;
    /*
    0x00  ����
    0x01  �ر�
    */
    if (0x00 == in_buf[0])//��������ģʽ
    {
        water.is_test_mode_switch_off = 0;
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    else if (0x01 == in_buf[0])//�رյ���ģʽ
    {
        water.is_test_mode_switch_off = 1;
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}

//�豸�����������ʱ��
//����������͵�ǰ���豸ʱ�䣬���ҷ�����Ҳ�����豸���͵�ǰ�ķ�����ʱ��
//���ڵ��԰�ͷ������˵�ʱ��ͬ�����豸�����ϴ����豸ÿ�ο��������������ڹ̶���ʱ��㣨����ÿ������һ�賿03:00:00����ʱ��ͬ�����������������ʱ��ͬ����
int cmd_computer_board_time_synchronization_1(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0x17,// ���԰�ʱ��ͬ��1
{
    int ret = 0;
    
    time_t seconds = ((uint32_t)in_buf[1] << 24) + ((uint32_t)in_buf[2] << 16) + ((uint32_t)in_buf[3] << 8) + ((uint32_t)in_buf[4] << 0);
    PCF8563_SetTime(seconds);
    //seconds = PCF8563_ReadTime();
    
    extern time_t time_dat;
    time_dat = seconds;//�޸�ϵͳʱ��
    return ret;
}


time_t seconds_aaaa;

//������ǿ�Ƹ����豸ʱ��
//���ڵ��԰�ͷ������˵�ʱ��ͬ����������·��豸ʱ��ͬ��ָ�����ʱ��ͬ����
int cmd_computer_board_time_synchronization_2(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0x18,// ���԰�ʱ��ͬ��2
{
    int ret = 0;
    
    time_t seconds = ((uint32_t)in_buf[1] << 24) + ((uint32_t)in_buf[2] << 16) + ((uint32_t)in_buf[3] << 8) + ((uint32_t)in_buf[4] << 0);
    
    PCF8563_SetTime(seconds);
    //seconds_aaaa = seconds = PCF8563_ReadTime();

    extern time_t time_dat;
    time_dat = seconds;//�޸�ϵͳʱ��
    return ret;
}

//��ˮ��ͬ��:��������
int cmd_synchronization_of_water_consumption_used(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0xAA,// ��ˮ��ͬ�� 0xAA��������
{
    int ret = 0;
    
    if (1 == water.save.is_bind)//������ײͺ��������������Ӧ��
    {
        water.save.used_flow = (uint32_t)((((uint32_t)in_buf[0] << 24) + ((uint32_t)in_buf[1] << 16) + ((uint32_t)in_buf[2] << 8) + ((uint32_t)in_buf[3] << 0)) / 0.917 + 0.5);
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}

//��ˮ��ͬ��:ʣ������
int cmd_synchronization_of_water_consumption_remain(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0xAB,// ��ˮ��ͬ�� 0xABʣ������
{
    int ret = 0;
    
    if (1 == water.save.is_bind)//������ײͺ��������������Ӧ��
    {
        water.save.remain_flow = (uint32_t)((((uint32_t)in_buf[0] << 24) + ((uint32_t)in_buf[1] << 16) + ((uint32_t)in_buf[2] << 8) + ((uint32_t)in_buf[3] << 0)) / 0.917 + 0.5);
        
        *out_len = in_len;
        memcpy(out_buf, in_buf, *out_len);
        ret = 1;
    }
    return ret;
}


bool is_iap_status = false;
int32_t packets_received = 0;
uint32_t flashdestination, ramsource;

int cmd_remote_upgrade_version(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0x1A,//����˻�ȡ�̼���ǰ�汾��
{
    int ret = 0;
    uint16_t i = 0;

    out_buf[i++] = HW_VERSION[0];
    out_buf[i++] = HW_VERSION[1];
    out_buf[i++] = HW_VERSION[2];
    out_buf[i++] = HW_VERSION[3];
    out_buf[i++] = HW_VERSION[4];
    out_buf[i++] = HW_VERSION[5];
    out_buf[i++] = HW_VERSION[6];
    out_buf[i++] = HW_VERSION[7];
    *out_len = i;
    ret = 1;
    
    return ret;
}

int cmd_remote_upgrade_data0(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0x2A,//Զ�������ļ���
{
    int32_t i, size = 0;
    uint8_t file_size[16], *file_ptr;
    uint8_t FileName[256] = {0};
    
    *out_len = 1;   
        
    if (in_buf[0] != 0x01)//128
    {
        out_buf[0] = 0;
        return 1;
    }
    
    if (in_buf[1] != ((in_buf[2] ^ 0xff) & 0xff))
    {
        out_buf[0] = 0;
        return 1;
    }
    packets_received = 0;
    /* Initialize flashdestination variable */
    flashdestination = APPLICATION_ADDRESS;
    FLASH_If_Init();
    is_iap_status = true;
    if (packets_received == 0)//��һ�����ݲ�д�뵽flash��
    {
        /* Filename packet */
        if (in_buf[3] != 0)
        {
            /* Filename packet has valid data */
            for (i = 0, file_ptr = in_buf + 3; (*file_ptr != 0) && (i < 256);)
            {
                FileName[i++] = *file_ptr++;
            }
            FileName[i++] = '\0';
            
            //�Д�file name�Ƿ����Ҫ��
#if APP_A
            if (NULL == strstr((char*)FileName, "V2_"))
#elif APP_B | BOOTLOADER
            if (NULL == strstr((char*)FileName, "V1_"))
#endif
            {
                /* End session */
                out_buf[0] = 0;
                return 1;
            }
            
            for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < (16 - 1));)
            {
                file_size[i++] = *file_ptr++;
            }
            file_size[i++] = '\0';
            Str2Int(file_size, &size);
            
            /* Test the size of the image to be sent */
            /* Image size is greater than Flash size */
            if (size > (USER_FLASH_SIZE + 1))
            {
                /* End session */
                out_buf[0] = 0;
                return 1;
            }
            packets_received  = 1;
            /* erase user application area */
            FLASH_If_Erase(APPLICATION_ADDRESS);
            // �����־λ��˵���Ѿ��޸��˹̼����ز�ȥ��
#if APP_A
            const char app_run[10] = {0};
            I2C_EE_BufferWrite(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_B_OFFSET_ADDR, (uint8_t *)app_run, 10);
#elif APP_B | BOOTLOADER
            const char app_run[10] = {0};
            I2C_EE_BufferWrite(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_A_OFFSET_ADDR, (uint8_t *)app_run, 10);
#endif
            out_buf[0] = 1;
            return 1;
        }
    }
    out_buf[0] = 0;
    return 1;
}

int cmd_remote_upgrade_data1(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0x3A,//Զ�������ļ�
{
    *out_len = 1;
    
    if (in_buf[0] != 0x02)//1024
    {
        out_buf[0] = 0;
        return 1;
    }
    
    if (in_buf[1] != ((in_buf[2] ^ 0xff) & 0xff))
    {
        out_buf[0] = 0;
        return 1;
    }
        
    if (packets_received != 0)//��һ�����ݲ�д�뵽flash��
    {
        /* Write received data in Flash */
        if (FLASH_If_Write(&flashdestination, (uint32_t*) &in_buf[3], (uint16_t) 1024/4) == 0)
        {
            packets_received++;
            out_buf[0] = 1;
            return 1;
        }
    }
    out_buf[0] = 0;
    return 1;
}

int cmd_remote_upgrade_data2(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0x4A,//Զ����������
{
    *out_len = 1;
    is_iap_status = false;
    if (in_buf[0] != 0x01)//128
    {
        out_buf[0] = 0;
        return 1;
    }
    
    if (in_buf[1] != ((in_buf[2] ^ 0xff) & 0xff))
    {
        out_buf[0] = 0;
        return 1;
    }
    
    if (packets_received != 0)
    {
        //if ()//�������Ľ�������жϣ������ȷ�����ñ�־λ
        {
#if APP_A
            const char app_run[10] = "app_b_on";
            I2C_EE_BufferWrite(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_B_OFFSET_ADDR, (uint8_t *)app_run, 10);
#elif APP_B | BOOTLOADER
            const char app_run[10] = "app_a_on";
            I2C_EE_BufferWrite(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_A_OFFSET_ADDR, (uint8_t *)app_run, 10);
#endif
            out_buf[0] = 1;
            return 1;
        }
    }
    out_buf[0] = 0;
    return 1;
}


int cmd_remote_upgrade_reboot(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0x7A,//Զ������
{
    int ret = 0;
    out_buf[0] = 0x01;
    *out_len = 1;
    ret = 1;
    
    return ret;
}


int cmd_remote_upgrade_new(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0x8A,//�´������豸ʱ�Ƿ������¹̼�
{
    *out_len = 1;
    
#if APP_A
        char buff[10] = "app_b_on";
        I2C_EE_BufferRead(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_B_OFFSET_ADDR, (uint8_t *)buff, 10);
        if (strstr(buff, "app_b_on"))
        {
            I2C_EE_BufferWrite(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_O_OFFSET_ADDR, (uint8_t *)buff, 10);
            out_buf[0] = 0x01;//�ɹ����̼��������´����¹̼�����
            return 1;
        }
#elif APP_B | BOOTLOADER
        char buff[10] = "app_a_on";
        I2C_EE_BufferRead(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_A_OFFSET_ADDR, (uint8_t *)buff, 10);
        if (strstr(buff, "app_a_on"))
        {
            I2C_EE_BufferWrite(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_O_OFFSET_ADDR, (uint8_t *)buff, 10);
            out_buf[0] = 0x01;//�ɹ����̼��������´����¹̼�����
            return 1;
        }
#endif
    out_buf[0] = 0x00;//ʧ�ܣ��̼����ܴ������⣬�������¹̼�����
    return 1;
}

int cmd_remote_upgrade_switch(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//0x9A,//33�����й̼��л�
{
    *out_len = 1;
            
    if (in_buf[0] == 0x00)//����BOOTLOADER
    {
        char buff[10] = "app_o_on";
        I2C_EE_BufferWrite(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_O_OFFSET_ADDR, (uint8_t *)buff, 10);
        out_buf[0] = 0x01;
        return 1;
    }
    else if (in_buf[0] == 0x01)//����A���̼�
    {
        char buff[10] = {0};
        I2C_EE_BufferRead(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_A_OFFSET_ADDR, (uint8_t *)buff, 10);
        if (strstr(buff, "app_a_on"))
        {
            I2C_EE_BufferWrite(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_O_OFFSET_ADDR, (uint8_t *)buff, 10);
            out_buf[0] = 0x01;
            return 1;
        }
    }
    else if (in_buf[0] == 0x02)//����B���̼�
    {
        char buff[10] = {0};
        I2C_EE_BufferRead(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_B_OFFSET_ADDR, (uint8_t *)buff, 10);
        if (strstr(buff, "app_b_on"))
        {
            I2C_EE_BufferWrite(0+sizeof(save_data_default)+sizeof(g_data_format.iccid)+APP_O_OFFSET_ADDR, (uint8_t *)buff, 10);
            out_buf[0] = 0x01;
            return 1;
        }
    }
    out_buf[0] = 0x00;
    return 1;
}

int cmd_screen_mode_switch(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//��Ļ��ʾģʽ�л�0x5A
{
    int ret = 0;
    
    if (1 == water.save.is_bind)//������ײͺ��������������Ӧ��
    {
        if ( ((in_buf[0] == 0x00) || (in_buf[0] == 0x01))  &&  ((in_buf[1] == 0x02) || (in_buf[1] == 0x03)) )
        {
            water.save.screen_mode_switch.day = in_buf[0]; //0x00	��������   0x01	ʣ������
            water.save.screen_mode_switch.flow = in_buf[1];//0x02	��������   0x03	ʣ������
            
            *out_len = in_len;
            memcpy(out_buf, in_buf, *out_len);
            ret = 1;
        }
    }
    return ret;
}

//��������ˮ����8��Сʱ˵�������˹��ϣ���Ҫͣ��������Ȼ������������û���
int cmd_overhaul_status_switch(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len)//����״̬�л�0x6A
{
    int ret = 0;
    
    if (1 == water.save.is_bind)//������ײͺ��������������Ӧ��
    {
        if ( (in_buf[0] == 0x00) || (in_buf[0] == 0x01) )
        {
            water.overhaul_status_switch = in_buf[0]; //0x00�������  //0x01�������
            
            if (water.overhaul_status_switch)//0x01�������
            {
                //�豸����
                water.is_overhaul_status = 1;
                water.is_overhaul_beep = 1;
            }
            else
            {
                water.is_overhaul_status = 0;
                water.is_overhaul_beep = 0;
                water.is_bushing_now = 0;
                water.io_high_status = -1;//��¼IO1״̬,��ѹ����״̬
                water.io_high_last_status = -1;
                water.io_low_status = -1;//��¼IO2״̬,��ѹ����״̬
                water.io_low_last_status = -1;
            }
            
            *out_len = in_len;
            memcpy(out_buf, in_buf, *out_len);
            ret = 1;
        }
    }
    
    return ret;
}

//-------------------------------------------------------------

void EDP_SendPacketPing(void) //0x01,//����֡
{
    if (edp.is_tcpip_connect)
    {
        uint8_t total_buf[100+15] = {0};
        int total_len = 0;
        
        water.date_time = time(NULL);
        
        g_data_format.cmd = 0x01;
        int i = 0;
        uint32_t remain_flow = (uint32_t)(water.save.remain_flow * 0.917 + 0.5);
        uint32_t used_flow = (uint32_t)(water.save.used_flow * 0.917 + 0.5);
        
        g_data_format.data[i++] = (uint8_t)(water.device_status);//�豸״̬
        g_data_format.data[i++] = (uint8_t)(water.is_screen_status_off);//��Ļ״̬
        g_data_format.data[i++] = (uint8_t)(water.save.work_mode);//����ģʽ
        g_data_format.data[i++] = (uint8_t)(remain_flow >> 24);//ʣ������
        g_data_format.data[i++] = (uint8_t)(remain_flow >> 16);//ʣ������
        g_data_format.data[i++] = (uint8_t)(remain_flow >> 8);//ʣ������
        g_data_format.data[i++] = (uint8_t)(remain_flow >> 0);//ʣ������
        g_data_format.data[i++] = (uint8_t)(water.remain_days >> 8);//ʣ������
        g_data_format.data[i++] = (uint8_t)(water.remain_days >> 0);//ʣ������
        g_data_format.data[i++] = (uint8_t)(used_flow >> 24);//��������
        g_data_format.data[i++] = (uint8_t)(used_flow >> 16);//��������
        g_data_format.data[i++] = (uint8_t)(used_flow >> 8);//��������
        g_data_format.data[i++] = (uint8_t)(used_flow >> 0);//��������
        g_data_format.data[i++] = (uint8_t)(water.used_days >> 8);//��������
        g_data_format.data[i++] = (uint8_t)(water.used_days >> 0);//��������
        if (water.save.clean_water_tds_switch == 0)//0��ʾĬ��ֵ��1��ʾʵʱֵ
        {
            g_data_format.data[i++] = (uint8_t)(water.save.clean_water_ppm >> 8);//��ˮTDS
            g_data_format.data[i++] = (uint8_t)(water.save.clean_water_ppm >> 0);//��ˮTDS
        }
        else
        {
            g_data_format.data[i++] = (uint8_t)(water.clean_water_ppm >> 8);//��ˮTDS
            g_data_format.data[i++] = (uint8_t)(water.clean_water_ppm >> 0);//��ˮTDS
        }
        if (water.save.raw_water_tds_switch == 0)//0��ʾĬ��ֵ��1��ʾʵʱֵ
        {
            g_data_format.data[i++] = (uint8_t)(water.save.raw_water_ppm >> 8);//ԭˮTDS
            g_data_format.data[i++] = (uint8_t)(water.save.raw_water_ppm >> 0);//ԭˮTDS
        }
        else
        {
            g_data_format.data[i++] = (uint8_t)(water.raw_water_ppm >> 8);//ԭˮTDS
            g_data_format.data[i++] = (uint8_t)(water.raw_water_ppm >> 0);//ԭˮTDS
        }
        g_data_format.data[i++] = (uint8_t)(water.rssi);//�ź�ǿ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.lac_value >> 8);//LACֵ
        g_data_format.data[i++] = (uint8_t)(water.lac_value >> 0);//LACֵ
        g_data_format.data[i++] = (uint8_t)(water.cid_value >> 8);//CIDֵ
        g_data_format.data[i++] = (uint8_t)(water.cid_value >> 0);//CIDֵ
        g_data_format.data[i++] = 0;
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 24);//����ʱ��
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 16);//����ʱ��
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 8);//����ʱ��
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 0);//����ʱ��
        g_data_format.length = i;
        struct_to_buf(total_buf, &total_len, g_data_format);
        vSerialPutBytes( NULL, total_buf, total_len );
    }
}

//��ˮ���ϴ�
//ÿ��ˮ��ͷ��ˮ10����ϴ�������ˮ������λ/10ml�����԰������ϴ�����Ӧ��
void EDP_SendPacketWaterSync(uint32_t water_value) //0x0A,//��ˮͬ��
{
    if (edp.is_tcpip_connect)
    {
        uint8_t total_buf[100+15] = {0};
        int total_len = 0;
        
        water.date_time = time(NULL);
        
        g_data_format.cmd = 0x0A;
        int i = 0;
        g_data_format.data[i++] = (uint8_t)(water_value >> 24);
        g_data_format.data[i++] = (uint8_t)(water_value >> 16);
        g_data_format.data[i++] = (uint8_t)(water_value >> 8);
        g_data_format.data[i++] = (uint8_t)(water_value >> 0);
        g_data_format.data[i++] = 0;
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 24);//����ʱ��
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 16);//����ʱ��
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 8);//����ʱ��
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 0);//����ʱ��
        g_data_format.length = i;
        struct_to_buf(total_buf, &total_len, g_data_format);
        vSerialPutBytes( NULL, total_buf, total_len );
    }
}

//����״̬ͬ��
//�豸����״̬����ʱ�������ϴ�����Ϣ�����԰������ϴ�����Ӧ��
/*
0x00	δ��ˮ
0x01	������ˮ
0x00	δˮ��
0x01	ˮ��
0x00	����
0x01	����
0x00	δ©ˮ
0x01	©ˮ
0x00	�ػ�
0x01	����
0x00	δȱˮ
0x01	ȱˮ
*/


bool SendPacketStatusSuccess = false;
void EDP_SendPacketStatus( void )//0x0C,//����״̬ͬ��
{
    if (edp.is_tcpip_connect )
    {
        uint8_t total_buf[100+15] = {0};
        int total_len = 0;
        
        g_data_format.cmd = 0x0C;
        int i = 0;
        g_data_format.data[i++] = (uint8_t)( (bool)water.is_make_water_status );//��ˮ״̬
        g_data_format.data[i++] = (uint8_t)( (bool)water.is_full_water_status );//ˮ��״̬
        g_data_format.data[i++] = (uint8_t)( (bool)water.is_overhaul_status );//����
        g_data_format.data[i++] = (uint8_t)( (bool)water.is_drop_water_status );//©ˮ
        g_data_format.data[i++] = (uint8_t)( (bool)water.is_power_on_status );//���ػ�
        g_data_format.data[i++] = (uint8_t)( (bool)water.is_shortage_water_status );//ȱˮ
        g_data_format.length = i;
        
        SendPacketStatusSuccess = false;
        struct_to_buf(total_buf, &total_len, g_data_format);
        for (int j = 0; j < 5; j++)
        {
            vSerialPutBytes( NULL, total_buf, total_len );
            vTaskDelay( pdMS_TO_TICKS(2000) );
            if (SendPacketStatusSuccess == true)
                break;
        }
    }
}

//��о״̬�ϴ�
void EDP_SendFilter(void)
{
    if (edp.is_tcpip_connect)
    {
        uint8_t total_buf[100+15] = {0};
        int total_len = 0;
        
        water.date_time = time(NULL);
        
        g_data_format.cmd = 0x09;
        int i = 0;
        g_data_format.data[i++] = (uint8_t)(water.filter_remain_value_1 >> 8);//��1����оʣ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.filter_remain_value_1 >> 0);//��1����оʣ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.filter_remain_value_2 >> 8);//��2����оʣ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.filter_remain_value_2 >> 0);//��2����оʣ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.filter_remain_value_3 >> 8);//��3����оʣ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.filter_remain_value_3 >> 0);//��3����оʣ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.filter_remain_value_4 >> 8);//��4����оʣ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.filter_remain_value_4 >> 0);//��4����оʣ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.filter_remain_value_5 >> 8);//��5����оʣ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.filter_remain_value_5 >> 0);//��5����оʣ��ֵ
        g_data_format.data[i++] = (uint8_t)(water.save.filter_max_value_1 >> 8);//��1����о���ֵ
        g_data_format.data[i++] = (uint8_t)(water.save.filter_max_value_1 >> 0);//��1����о���ֵ
        g_data_format.data[i++] = (uint8_t)(water.save.filter_max_value_2 >> 8);//��2����о���ֵ
        g_data_format.data[i++] = (uint8_t)(water.save.filter_max_value_2 >> 0);//��2����о���ֵ
        g_data_format.data[i++] = (uint8_t)(water.save.filter_max_value_3 >> 8);//��3����о���ֵ
        g_data_format.data[i++] = (uint8_t)(water.save.filter_max_value_3 >> 0);//��3����о���ֵ
        g_data_format.data[i++] = (uint8_t)(water.save.filter_max_value_4 >> 8);//��4����о���ֵ
        g_data_format.data[i++] = (uint8_t)(water.save.filter_max_value_4 >> 0);//��4����о���ֵ
        g_data_format.data[i++] = (uint8_t)(water.save.filter_max_value_5 >> 8);//��5����о���ֵ
        g_data_format.data[i++] = (uint8_t)(water.save.filter_max_value_5 >> 0);//��5����о���ֵ
        g_data_format.data[i++] = 0;
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 24);//����ʱ��
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 16);//����ʱ��
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 8);//����ʱ��
        g_data_format.data[i++] = (uint8_t)(water.date_time >> 0);//����ʱ��
        g_data_format.length = i;
        struct_to_buf(total_buf, &total_len, g_data_format);
        vSerialPutBytes( NULL, total_buf, total_len );
    }
}

int buf_to_struct(data_format_t *data_format, uint8_t *buf, int buf_len)
{
    if (buf_len <= 15)
        return 0;
    memcpy(&data_format->iccid[0], &buf[ICCID_ADDR_START], sizeof(data_format->iccid) / sizeof(data_format->iccid[0]));
    data_format->cmd = buf[CMD_ADDR_START];
    data_format->length = (buf[LENGTH_ADDR_START] << 8) + buf[LENGTH_ADDR_START+1];
    if (data_format->length > 0)
        memcpy(&data_format->data[0], &buf[DATA_ADDR_START], data_format->length);
    data_format->crc[0] = buf[DATA_ADDR_START+data_format->length];
    data_format->crc[1] = buf[DATA_ADDR_START+data_format->length+1];
    uint16_t crc16 = CRC16_MODBUS(buf, buf_len-2);
    if (crc16 != (((uint16_t)data_format->crc[0] << 8) + (uint16_t)data_format->crc[1])) 
        return 0;
    else 
        return 1;
}

int struct_to_buf(uint8_t *buf, int *buf_len, data_format_t data_format)
{
    memcpy(&buf[ICCID_ADDR_START], &data_format.iccid[0], sizeof(data_format.iccid) / sizeof(data_format.iccid[0]));
    buf[CMD_ADDR_START] = data_format.cmd;
    buf[LENGTH_ADDR_START] = (uint8_t)(data_format.length >> 8);
    buf[LENGTH_ADDR_START+1] = (uint8_t)(data_format.length >> 0);
    memcpy(&buf[DATA_ADDR_START], &data_format.data[0], data_format.length);
    *buf_len = DATA_ADDR_START + data_format.length + 2;
    uint16_t crc16 = CRC16_MODBUS(buf, *buf_len-2);
    data_format.crc[0] = (uint8_t)(crc16 >> 8);
    data_format.crc[1] = (uint8_t)(crc16 >> 0);
    buf[DATA_ADDR_START+data_format.length] = data_format.crc[0];
    buf[DATA_ADDR_START+data_format.length+1] = data_format.crc[1];
    return 1;
}

int ProtocolProcess(uint8_t* buf, int len)
{
    int out_len = 0;
    int resp = 0;
    
    uint8_t cmd = buf[CMD_ADDR_START];
    uint16_t in_len = (buf[LENGTH_ADDR_START] << 8) + buf[LENGTH_ADDR_START+1];
    uint16_t crc16_get = (buf[DATA_ADDR_START+in_len] << 8) + buf[DATA_ADDR_START+in_len+1];
    uint16_t crc16_calc = CRC16_MODBUS(buf, len-2);
    if (crc16_calc != crc16_get)
        return 0;
    
    for (int i = 0; i < cmd_process_cnt; i++)
    {
        if (cmd == cmd_process[i].cmd)
        {
            //printf("cmd:%02x\r\n", data_format.cmd);
            if ((cmd != CMD_HEARTBEAT) || (cmd != CMD_REMOTE_UPGRADE_DATA1))
                BEEP_ON;
            resp = cmd_process[i].cb(&buf[DATA_ADDR_START], &out_len, &buf[DATA_ADDR_START], in_len);
            if ((cmd != CMD_HEARTBEAT) || (cmd != CMD_REMOTE_UPGRADE_DATA1))
            {
                I2C_EE_BufferWriteSafe(0, (uint8_t *)&water.save, sizeof(save_data_default));
                BEEP_OFF;
                I2C_EE_BufferRead(0, (uint8_t *)&water.save, sizeof(save_data_default));
            }
            break;
        }
    }
    if (0 != resp)
    {
        int total_len = 0;
        total_len = DATA_ADDR_START + out_len + 2;
        buf[LENGTH_ADDR_START] = (uint8_t)(out_len >> 8);
        buf[LENGTH_ADDR_START+1] = (uint8_t)(out_len >> 0);
        uint16_t crc16 = CRC16_MODBUS(buf, total_len-2);
        buf[total_len-2] = (uint8_t)(crc16 >> 8);
        buf[total_len-1] = (uint8_t)(crc16 >> 0);
        
        vSerialPutBytes( NULL, buf, total_len );
        
        if (cmd == CMD_REMOTE_UPGRADE_REBOOT)//������������Ҫ���еĳ���
        {
            vTaskDelay( pdMS_TO_TICKS(1000) );
            NVIC_SystemReset();
        }
    }
    
    return 0;
}


