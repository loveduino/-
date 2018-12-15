#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
    CMD_HEARTBEAT = 0X01,//����֡
    CMD_BINDING_PACKAGE = 0X02,//���ײ�
    CMD_CLOSE_THE_SCREEN = 0X03,//�ر���Ļ
    CMD_OPEN_THE_SCREEN = 0X04,//����Ļ
    CMD_POWER_OFF = 0X05,//�ػ�
    CMD_POWER_ON = 0X06,//����
    CMD_STRONG_PUNCH = 0X07,//ǿ��
    CMD_POSITIVE_VALUE = 0X08,//����ֵ
    
    CMD_FILTER_SEND = 0x09,//�û��豸�����ϱ���о״̬��ÿ���ϱ�һ�εķ�ʽ
    CMD_FILTER_REQUIRE = 0x19,//������ȡ��о״̬
    
    CMD_SYNCHRONIZE_WITH_WATER = 0X0A,//��ˮͬ��
    //CMD_TIME_SYNCHRONIZATION = 0X0B,//��ʱͬ��
    CMD_TIME_SYNCHRONIZATION_USED = 0XBA,//��ʱͬ�� 0xBA��������
    CMD_TIME_SYNCHRONIZATION_REMAIN = 0XBB,//��ʱͬ�� 0xBBʣ������
    CMD_WORK_STATUS_SYNCHRONIZATION = 0X0C,//����״̬ͬ��
    CMD_QUERY_DEVICE_OPERATION_INFORMATION = 0X0D,//��ѯ�豸������Ϣ
    CMD_FILTER_RESET_AND_MODIFICATION = 0X0E,//��о��λ���޸�
    CMD_MODE_SWITCHING = 0X0F,//ģʽ�л�
    CMD_RESET = 0X10,//�ָ���������
    //�޸������Ͷ˿ں�
    CMD_PARAMETER_MODIFICATION = 0X11,//�����޸�
    CMD_TIMING_FLUSH_PARAMETER_MODIFICATION = 0X12,//��ʱ��ϴ�����޸�
    CMD_MAINTENANCE_PARAMETER_MODIFICATION = 0X13,//���޲����޸�
    CMD_CONTROL_PARAMETER_MODIFICATION_1 = 0X14,//���Ʋ����޸�1
    CMD_CONTROL_PARAMETER_MODIFICATION_2 = 0X15,//���Ʋ����޸�2
    CMD_TEST_MODE_SWITCH = 0X16,//�����رղ���ģʽ
    CMD_COMPUTER_BOARD_TIME_SYNCHRONIZATION_1 = 0x17,// ���԰�ʱ��ͬ��1
    CMD_COMPUTER_BOARD_TIME_SYNCHRONIZATION_2 = 0x18,// ���԰�ʱ��ͬ��2
    CMD_SYNCHRONIZATION_OF_WATER_CONSUMPTION_USED = 0xAA,// ��ˮ��ͬ�� 0xAA��������
    CMD_SYNCHRONIZATION_OF_WATER_CONSUMPTION_REMAIN = 0xAB,// ��ˮ��ͬ�� 0xABʣ������
    CMD_REMOTE_UPGRADE_VERSION = 0x1A,//����˻�ȡ�̼���ǰ�汾��
    CMD_REMOTE_UPGRADE_DATA0 = 0x2A,//Զ�������ļ���
    CMD_REMOTE_UPGRADE_DATA1 = 0x3A,//Զ�������ļ�
    CMD_REMOTE_UPGRADE_DATA2 = 0x4A,//Զ����������
    CMD_REMOTE_UPGRADE_REBOOT = 0x7A,//Զ������
    CMD_REMOTE_UPGRADE_NEW = 0x8A,//�´������豸ʱ�Ƿ������¹̼�
    CMD_REMOTE_UPGRADE_SWITCH = 0x9A,//���й̼��л�
    CMD_SCREEN_MODE_SWITCH = 0x5A,//��Ļ��ʾģʽ�л�0x5A
    CMD_OVERHAUL_STATUS_SWITCH = 0x6A//����״̬�л�0x6A
}cmd_type_t;

typedef int (*cmd_process_cb)(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);

typedef struct{
    cmd_type_t cmd;
    cmd_process_cb cb;
}cmd_process_t;

extern int cmd_heartbeat_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x01,//����֡
extern int cmd_binding_package_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x02,//���ײ�
extern int cmd_close_the_screen_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x03,//�ر���Ļ
extern int cmd_open_the_screen_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x04,//����Ļ
extern int cmd_power_off_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x05,//�ػ�
extern int cmd_power_on_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x06,//����
extern int cmd_strong_push_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x07,//ǿ��
extern int cmd_positive_value_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x08,//����ֵ

extern int cmd_filter_send_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//�û��豸�����ϱ���о״̬��ÿ���ϱ�һ�εķ�ʽ
extern int cmd_filter_require_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//������ȡ��о״̬

extern int cmd_synchronize_with_water_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x0a,//��ˮͬ��
//extern int cmd_time_synchronization_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x0b,//��ʱͬ��
extern int cmd_time_synchronization_used_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//��ʱͬ�� 0xBA��������
extern int cmd_time_synchronization_remain_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//��ʱͬ�� 0xBBʣ������
extern int cmd_work_status_synchronization_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x0c,//����״̬ͬ��
extern int cmd_query_device_operation_information_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x0d,//��ѯ�豸������Ϣ
extern int cmd_filter_reset_and_modification_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x0e,//��о��λ���޸�
extern int cmd_mode_switching_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x0f,//ģʽ�л�
extern int cmd_reset_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x10,//�ָ���������
//�޸������Ͷ˿ں�
extern int cmd_parameter_modification_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x11,//�����޸�
extern int cmd_timing_flush_parameter_modification_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x12,//��ʱ��ϴ�����޸�
extern int cmd_maintenance_parameter_modification_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x13,//���޲����޸�
extern int cmd_control_parameter_modification_1_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x14,//���Ʋ����޸�1
extern int cmd_control_parameter_modification_2_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x15,//���Ʋ����޸�2
extern int cmd_test_mode_switch_process(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x16,//�����رղ���ģʽ
extern int cmd_computer_board_time_synchronization_1(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x17,// ���԰�ʱ��ͬ��1
extern int cmd_computer_board_time_synchronization_2(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x18,// ���԰�ʱ��ͬ��2
extern int cmd_synchronization_of_water_consumption_used(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0xAA,// ��ˮ��ͬ�� 0xAA��������
extern int cmd_synchronization_of_water_consumption_remain(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0xAB,// ��ˮ��ͬ�� 0xABʣ������
extern int cmd_remote_upgrade_version(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x1A,//����˻�ȡ�̼���ǰ�汾��
extern int cmd_remote_upgrade_data0(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x2A,//Զ�������ļ���
extern int cmd_remote_upgrade_data1(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x3A,//Զ�������ļ�
extern int cmd_remote_upgrade_data2(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x4A,//Զ����������
extern int cmd_remote_upgrade_reboot(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x7A,//Զ������
extern int cmd_remote_upgrade_new(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x8A,//�´������豸ʱ�Ƿ������¹̼�
extern int cmd_remote_upgrade_switch(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x9A,//33�����й̼��л�
extern int cmd_screen_mode_switch(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x5A,//��Ļ��ʾģʽ�л�
extern int cmd_overhaul_status_switch(uint8_t* out_buf, int* out_len, uint8_t* in_buf, int in_len);//0x6A,//����״̬�л�0x6A

struct screen_mode_switch_t{
    uint8_t day;//0x00	��������   0x01	ʣ������
    uint8_t flow;//0x02	��������   0x03	ʣ������
};



struct save_data{
    int e2prom_init;
    
    uint8_t is_bind;
    time_t bind_timestamp;//��¼�������ʱ���
    
    //����ģʽ	        HEX��1�ֽڣ�
    /*
    0	����ģʽ
    1	ʱ��ģʽ
    */
    uint8_t work_mode;
    
    //��������	        HEX��4�ֽڣ�
    uint32_t used_flow;
    
    //ʣ������	        HEX��4�ֽڣ�
    uint32_t remain_flow;
    
    //��������ʱ���
    time_t used_days_timestamp;//���ڼ����Ѿ�ʹ�õ�����������ǰʱ���-�����¼��ʱ��꣩/ (3600*24)
    
    //ʣ������ʱ���
    time_t remain_days_timestamp;//��¼���ж���ʱ������ʹ��
    
    //��ˮTDS	            HEX��2�ֽڣ�
    uint16_t clean_water_ppm;//���������µļٵ�ppmֵ
    uint8_t clean_water_tds_switch;//0��ʾĬ��ֵ��1��ʾʵʱֵ
    
    //ԭˮTDS	            HEX��2�ֽڣ�
    uint16_t raw_water_ppm;//���������µļٵ�ppmֵ
    uint8_t raw_water_tds_switch;//0��ʾĬ��ֵ��1��ʾʵʱֵ
    
    //�� 1 ����о�Ŀ�ʼʹ��ʱ���
    time_t filter_used_timestamp_1;
    
    //�� 2 ����о�Ŀ�ʼʹ��ʱ���
    time_t filter_used_timestamp_2;
    
    //�� 3 ����о�Ŀ�ʼʹ��ʱ���
    time_t filter_used_timestamp_3;
    
    //�� 4 ����о�Ŀ�ʼʹ��ʱ���
    time_t filter_used_timestamp_4;
    
    //�� 5 ����о�Ŀ�ʼʹ��ʱ���
    time_t filter_used_timestamp_5;
    
    //��һ����о���ֵ	HEX��2�ֽڣ�
    uint16_t filter_max_value_1;
    
    //�ڶ�����о���ֵ	HEX��2�ֽڣ�
    uint16_t filter_max_value_2;
    
    //��������о���ֵ	HEX��2�ֽڣ�
    uint16_t filter_max_value_3;
    
    //���ļ���о���ֵ	HEX��2�ֽڣ�
    uint16_t filter_max_value_4;
    
    //���弶��о���ֵ	HEX��2�ֽڣ�
    uint16_t filter_max_value_5;
    
    uint8_t force_flush_time;//��λ����
    uint32_t period_flush_time;//��
    uint32_t maintenance_time;//��λ����
    
    uint16_t ping_time;//��λ����
    uint16_t reconnect_time;//��λ����
    
    struct screen_mode_switch_t screen_mode_switch;
    
};

    
//���ֽ���ǰ�����ֽ��ں�
struct water_s{
//�豸״̬	        HEX��1�ֽڣ�
/*
0	����
1	������
2	δע��
3	Ƿ��
4	��о����λ
5	Ӳ������
*/
uint8_t device_status;

//��Ļ״̬	        HEX��1�ֽڣ�
/*
0	��Ļ�򿪣��ϵ�Ĭ��ֵ��
1	��Ļ�ر�
*/
uint8_t is_screen_status_off;

//��������	        HEX��2�ֽڣ�
uint16_t used_days;

//ʣ������	        HEX��2�ֽڣ�
uint16_t remain_days;
    
//��һ����оʣ��ֵ	HEX��2�ֽڣ�
uint16_t filter_remain_value_1;

//�ڶ�����оʣ��ֵ	HEX��2�ֽڣ�
uint16_t filter_remain_value_2;

//��������оʣ��ֵ	HEX��2�ֽڣ�
uint16_t filter_remain_value_3;

//���ļ���оʣ��ֵ	HEX��2�ֽڣ�
uint16_t filter_remain_value_4;

//���弶��оʣ��ֵ	HEX��2�ֽڣ�
uint16_t filter_remain_value_5;

//�ź�ǿ��ֵ	        HEX��1�ֽڣ�
uint8_t rssi;

//LACֵ	            HEX��2�ֽڣ�
uint16_t lac_value;

//CIDֵ	            HEX��2�ֽڣ�
uint16_t cid_value;

//����ʱ��          HEX��4�ֽڣ�
uint32_t date_time;

//����һЩֵ
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
uint8_t is_make_water_status;
uint8_t is_full_water_status;
uint8_t is_overhaul_status;
uint8_t is_drop_water_status;
uint8_t is_power_on_status;
uint8_t is_shortage_water_status;

uint8_t is_bushing_now;
uint8_t is_overhaul_beep;

uint8_t is_test_mode_switch_off;//0x00  ���� 0x01  �ر�
uint16_t test_used_days;
uint16_t test_used_flow;

uint32_t flowmeter_count;//�����Ƽ�����

uint16_t raw_water_ppm;//ʵ�ʼ�⵽��ppmֵ

uint16_t clean_water_ppm;//ʵ�ʼ�⵽��ppmֵ

//����״̬�л�0x6A
uint8_t overhaul_status_switch;//0x00�������  //0x01�������
    
int io_high_status;//��¼IO1״̬,��ѹ����״̬
int io_high_last_status;
int io_low_status;//��¼IO2״̬,��ѹ����״̬
int io_low_last_status;
    
//��Ҫ�洢��ֵ
struct save_data save;
};

extern struct water_s water;
extern const struct save_data save_data_default;

#define ICCID_ADDR_START        0
#define CMD_ADDR_START          10
#define LENGTH_ADDR_START       11
#define DATA_ADDR_START         13

#define APP_O_OFFSET_ADDR       (0)
#define APP_A_OFFSET_ADDR       (10)
#define APP_B_OFFSET_ADDR       (20)

typedef struct{
    uint8_t iccid[10];
    uint8_t cmd;
    uint16_t length;
    uint8_t data[100];//�䳤�����ݾ��幦�ܾ���
    uint8_t crc[2];
}data_format_t;

extern data_format_t g_data_format;

void EDP_SendPacketPing( void );

int buf_to_struct(data_format_t *data_format, uint8_t *buf, int buf_len);
int struct_to_buf(uint8_t *buf, int *buf_len, data_format_t data_format);

int ProtocolProcess(uint8_t* buf, int len);

#endif
