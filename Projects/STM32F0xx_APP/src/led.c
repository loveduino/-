/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "led.h"
#include "main.h"
#include "protocol.h"      

//���� 0��1��2��3��4��5��6��7��8��9��A��B��C��D��E��F����(16)��-��(17)��_ƫС(18)����ƫ��(19)
const uint8_t seg_code[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xFF,0xBF,0xF7,0xFE};
static uint8_t g_value[24];

typedef enum{
    MULTIFUNC_0 = 0xFF,
    MULTIFUNC_1 = 0xFB,
    MULTIFUNC_2 = 0xF1,
    MULTIFUNC_3 = 0xE0,
}led_multifunc_t;
static led_multifunc_t led_multifunc = MULTIFUNC_0;

typedef enum{
    DROPS_0 = 0xFF,
    DROPS_1 = 0xFE,
    DROPS_2 = 0xFC,
    DROPS_3 = 0xF8,
}led_drops_t;
static led_drops_t led_drops = DROPS_0;

typedef enum{
    SIGNAL_0 = 0xFF,
    SIGNAL_1 = 0xF7,
}led_signal_t;
static led_signal_t led_signal = SIGNAL_0;

typedef enum{
    TIME_0 = 0xFF,
    TIME_1 = 0xFE,
}led_time_t;
static led_time_t led_time = TIME_0;

typedef enum{
    DAY_0 = 0xFF,
    DAY_1 = 0xFD,
}led_day_t;
static led_day_t led_day = DAY_0;

typedef enum{
    FLOW_0 = 0xFF,
    FLOW_1 = 0xFB,
}led_flow_t;
static led_flow_t led_flow = FLOW_0;

typedef enum{
    LITER_0 = 0xFF,
    LITER_1 = 0xF7,
}led_liter_t;
static led_liter_t led_liter = LITER_0;

typedef enum{
    RAW_WATER_0 = 0xFF,
    RAW_WATER_1 = 0xCF,
}led_raw_water_t;
static led_raw_water_t led_raw_water = RAW_WATER_0;

typedef enum{
    RAW_PPM_D30_0 = 0xFF,
    RAW_PPM_D30_1 = 0xBF,
}led_raw_ppm_d30_t;
static led_raw_ppm_d30_t led_raw_ppm_d30 = RAW_PPM_D30_0;

typedef enum{
    RAW_PPM_D31_0 = 0xFF,
    RAW_PPM_D31_1 = 0xFE,
}led_raw_ppm_d31_t;
static led_raw_ppm_d31_t led_raw_ppm_d31 = RAW_PPM_D31_0;

typedef enum{
    CLEAN_WATER_0 = 0xFF,
    CLEAN_WATER_1 = 0xF9,
}led_clean_water_t;
static led_clean_water_t led_clean_water = CLEAN_WATER_0;

typedef enum{
    CLEAN_PPM_0 = 0xFF,
    CLEAN_PPM_1 = 0xE7,
}led_clean_ppm_t;
static led_clean_ppm_t led_clean_ppm = CLEAN_PPM_0;



//��������ܣ��͵�ƽѡ�����
static void LED_SegInit(uint32_t RCC_AHBPeriph, GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_SET);
}

//��������ܣ��ߵ�ƽѡ��λ
static void LED_PosInit(uint32_t RCC_AHBPeriph, GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);
}

static void LED_Init(void)
{
    LED_SegInit(N1_RCC_AHBPeriph_PORT, N1_PORT, N1_PIN);
    LED_SegInit(N2_RCC_AHBPeriph_PORT, N2_PORT, N2_PIN);
    LED_SegInit(N3_RCC_AHBPeriph_PORT, N3_PORT, N3_PIN);
    LED_SegInit(N4_RCC_AHBPeriph_PORT, N4_PORT, N4_PIN);
    LED_SegInit(N5_RCC_AHBPeriph_PORT, N5_PORT, N5_PIN);
    LED_SegInit(N6_RCC_AHBPeriph_PORT, N6_PORT, N6_PIN);
    LED_SegInit(N7_RCC_AHBPeriph_PORT, N7_PORT, N7_PIN);
    
    LED_PosInit(D8_RCC_AHBPeriph_PORT, D8_PORT, D8_PIN);
    LED_PosInit(D9_RCC_AHBPeriph_PORT, D9_PORT, D9_PIN);
    LED_PosInit(D10_RCC_AHBPeriph_PORT, D10_PORT, D10_PIN);
    LED_PosInit(D11_RCC_AHBPeriph_PORT, D11_PORT, D11_PIN);
    LED_PosInit(D12_RCC_AHBPeriph_PORT, D12_PORT, D12_PIN);
    LED_PosInit(D13_RCC_AHBPeriph_PORT, D13_PORT, D13_PIN);
    LED_PosInit(D14_RCC_AHBPeriph_PORT, D14_PORT, D14_PIN);
    LED_PosInit(D15_RCC_AHBPeriph_PORT, D15_PORT, D15_PIN);
    LED_PosInit(D16_RCC_AHBPeriph_PORT, D16_PORT, D16_PIN);
    LED_PosInit(D17_RCC_AHBPeriph_PORT, D17_PORT, D17_PIN);
    LED_PosInit(D18_RCC_AHBPeriph_PORT, D18_PORT, D18_PIN);
    LED_PosInit(D19_RCC_AHBPeriph_PORT, D19_PORT, D19_PIN);
    LED_PosInit(D20_RCC_AHBPeriph_PORT, D20_PORT, D20_PIN);
    LED_PosInit(D21_RCC_AHBPeriph_PORT, D21_PORT, D21_PIN);
    LED_PosInit(D22_RCC_AHBPeriph_PORT, D22_PORT, D22_PIN);
    LED_PosInit(D23_RCC_AHBPeriph_PORT, D23_PORT, D23_PIN);
    LED_PosInit(D24_RCC_AHBPeriph_PORT, D24_PORT, D24_PIN);
    LED_PosInit(D25_RCC_AHBPeriph_PORT, D25_PORT, D25_PIN);
    LED_PosInit(D26_RCC_AHBPeriph_PORT, D26_PORT, D26_PIN);
    LED_PosInit(D27_RCC_AHBPeriph_PORT, D27_PORT, D27_PIN);
    LED_PosInit(D28_RCC_AHBPeriph_PORT, D28_PORT, D28_PIN);
    LED_PosInit(D29_RCC_AHBPeriph_PORT, D29_PORT, D29_PIN);
    LED_PosInit(D30_RCC_AHBPeriph_PORT, D30_PORT, D30_PIN);
    LED_PosInit(D31_RCC_AHBPeriph_PORT, D31_PORT, D31_PIN);
}

//��������ܣ��͵�ƽѡ�����
static void seg_select(uint8_t seg, uint8_t val)
{
    switch(seg)
    {
    case 0:  if(val==0)  N1_H;  else N1_L; break;
    case 1:  if(val==0)  N2_H;  else N2_L; break;
    case 2:  if(val==0)  N3_H;  else N3_L; break;
    case 3:  if(val==0)  N4_H;  else N4_L; break;
    case 4:  if(val==0)  N5_H;  else N5_L; break;
    case 5:  if(val==0)  N6_H;  else N6_L; break;
    case 6:  if(val==0)  N7_H;  else N7_L; break;
    default:break; 
    }
}

//��������ܣ��ߵ�ƽѡ��λ
static void pos_select(uint8_t pos, uint8_t val)
{
    switch(pos)
    {
    case 0:  if(val!=0)  D8_H;  else D8_L;  break;
    case 1:  if(val!=0)  D9_H;  else D9_L;  break;
    case 2:  if(val!=0)  D10_H; else D10_L; break;
    case 3:  if(val!=0)  D11_H; else D11_L; break;
    case 4:  if(val!=0)  D12_H; else D12_L; break;
    case 5:  if(val!=0)  D13_H; else D13_L; break;
    case 6:  if(val!=0)  D14_H; else D14_L; break;
    case 7:  if(val!=0)  D15_H; else D15_L; break;
    case 8:  if(val!=0)  D16_H; else D16_L; break;
    //case 9:  if(val!=0)  D17_H; else D17_L; break;//pass
    //case 10: if(val!=0)  D18_H; else D18_L; break;//pass
    case 11: if(val!=0)  D19_H; else D19_L; break;
    case 12: if(val!=0)  D20_H; else D20_L; break;
    case 13: if(val!=0)  D21_H; else D21_L; break;
    case 14: if(val!=0)  D22_H; else D22_L; break;
    case 15: if(val!=0)  D23_H; else D23_L; break;
    case 16: if(val!=0)  D24_H; else D24_L; break;
    case 17: if(val!=0)  D25_H; else D25_L; break;
    case 18: if(val!=0)  D26_H; else D26_L; break;
    case 19: if(val!=0)  D27_H; else D27_L; break;
    case 20: if(val!=0)  D28_H; else D28_L; break;
    case 21: if(val!=0)  D29_H; else D29_L; break;
    case 22: if(val!=0)  D30_H; else D30_L; break;
    case 23: if(val!=0)  D31_H; else D31_L; break;
    default:break; 
    }
}

static void show_single_num(uint8_t num)
{
    uint8_t code = ~seg_code[num];
    for (int i = 0; i < 7; i++)
    {
        if (code & 0x01)
            seg_select(i, 1);
        else
            seg_select(i, 0);
        code >>= 1;
    }
}

static void show_single_graph(uint8_t graph)
{
    uint8_t code = ~graph;
    for (int i = 0; i < 7; i++)
    {
        if (code & 0x01)
            seg_select(i, 1);
        else
            seg_select(i, 0);
        code >>= 1;
    }
}

void show_callback(void)
{
    static uint8_t i = 0;
    
    g_value[23-8] = (uint8_t)led_multifunc;                                     //(())      ��ʾ����
    g_value[24-8] = (uint8_t)led_drops & (uint8_t)led_signal;                   //'''�ź�   ��ʾ����
    g_value[30-8] = (uint8_t)led_time & (uint8_t)led_day & (uint8_t)led_flow & (uint8_t)led_liter & (uint8_t)led_raw_water & (uint8_t)led_raw_ppm_d30;//ʱ�� DAY ���� LITER ԭˮppm    ��ʾ����
    g_value[31-8] = (uint8_t)led_raw_ppm_d31 & (uint8_t)led_clean_water & (uint8_t)led_clean_ppm;//ppm  ��ˮppm                   ��ʾ����
    
    switch(i)
    {
    case 0: pos_select( 23,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 1: pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 2: pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 3: pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 4: pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 5: pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 6: pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 7: pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 8: pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 9: pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 10:pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 11:pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 12:pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 13:pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 14:pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 15:pos_select(i-1,0);pos_select(i,1);show_single_graph(g_value[i]);i++;break;//(())      ��ʾ����
    case 16:pos_select(i-1,0);pos_select(i,1);show_single_graph(g_value[i]);i++;break;//'''�ź�   ��ʾ����
    case 17:pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 18:pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 19:pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 20:pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 21:pos_select(i-1,0);pos_select(i,1);show_single_num(g_value[i]);i++;break;
    case 22:pos_select(i-1,0);pos_select(i,1);show_single_graph(g_value[i]);i++;break;//ʱ�� DAY ���� LITER ԭˮppm    ��ʾ����
    case 23:pos_select(i-1,0);pos_select(i,1);show_single_graph(g_value[i]);i=0;break;//ppm  ��ˮppm                   ��ʾ����
    default:i=0;break;
    }
}

static void show_left_up(int num, bool is_show)
{
    if (is_show)
    {
        g_value[8-8]  = num / 1000 % 10;
        g_value[11-8] = num / 100  % 10;
        g_value[13-8] = num / 10   % 10;
        g_value[15-8] = num / 1    % 10;
    }
    else
    {
        g_value[8-8] = g_value[11-8] = g_value[13-8] = g_value[15-8] = 16;
    }
}

static void show_left_down(int num, bool is_show)
{
    if (is_show)
    {
        g_value[9-8]  = num / 10000 % 10;
        g_value[10-8] = num / 1000  % 10;
        g_value[12-8] = num / 100   % 10;
        g_value[14-8] = num / 10    % 10;
        g_value[16-8] = num / 1     % 10;
    }
    else
    {
        g_value[9-8] = g_value[10-8] = g_value[12-8] = g_value[14-8] = g_value[16-8] = 16;
    }
}

static void show_right_up(int num, bool is_show)
{
    if (is_show)
    {
        g_value[19-8] = num / 1000 % 10;
        g_value[20-8] = num / 100  % 10;
        g_value[21-8] = num / 10   % 10;
        g_value[22-8] = num / 1    % 10;
    }
    else
    {
        g_value[19-8] = g_value[20-8] = g_value[21-8] = g_value[22-8] = 16;
    }
}

static void show_right_down(int num, bool is_show)
{
    if (is_show)
    {
        g_value[25-8] = num / 10000 % 10;
        g_value[26-8] = num / 1000  % 10;
        g_value[27-8] = num / 100   % 10;
        g_value[28-8] = num / 10    % 10;
        g_value[29-8] = num / 1     % 10;
    }
    else
    {
        g_value[25-8] = g_value[26-8] = g_value[27-8] = g_value[28-8] = g_value[29-8] = 16;
    }
}

static void prvShowRefreshTask( void *pvParameters )
{
    /* Just to stop compiler warnings. */
	( void ) pvParameters;
    
	for( ;; )
	{
        vTaskDelay( pdMS_TO_TICKS(1) );
        show_callback();
    }
}


void prvShowTask( void *pvParameters )
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    LED_Init();
    
    xTaskCreate( prvShowRefreshTask, (char *) "Show Refresh", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 9, NULL );
        
    int left_up_num = 0;
    int left_down_num = 0;
    int right_up_num = 0;
    int right_down_num = 0;
#if 0
    left_up_num = g_data_format.iccid[2] + g_data_format.iccid[1]*100;
    left_down_num = g_data_format.iccid[7]/10%10 + g_data_format.iccid[6]*100 + g_data_format.iccid[5]*10000;
    right_up_num = g_data_format.iccid[4] + g_data_format.iccid[3]*100;
    right_down_num = g_data_format.iccid[9] + g_data_format.iccid[8]*100 + g_data_format.iccid[7]/1%10*10000;
#else
    uint64_t num =  ((uint64_t)g_data_format.iccid[9] << 0) + ((uint64_t)g_data_format.iccid[8] << 8) + ((uint64_t)g_data_format.iccid[7] << 16) + ((uint64_t)g_data_format.iccid[6] << 24) + ((uint64_t)g_data_format.iccid[5] << 32)
        + ((uint64_t)g_data_format.iccid[4] << 40) + ((uint64_t)g_data_format.iccid[3] << 48) + ((uint64_t)g_data_format.iccid[2] << 56) + ((uint64_t)g_data_format.iccid[1] << 64) + ((uint64_t)g_data_format.iccid[0] << 72);
    left_up_num =    num / 100000000000000   % 10000;//4
    right_up_num =   num / 10000000000       % 10000;//4
    left_down_num =  num / 100000            % 100000;//5
    right_down_num = num / 1                 % 100000;//5
#endif
    //�û������豸�ײ�֮��ÿ�ζϵ������豸ʱ����Ļ������˸��ʾ�豸�����������Ϣ����ʾʱ��Ϊ15�룬15��֮����ʾ������Ϣ��
    for (int i = 0; i < 15; i++)
    {
        show_left_up(left_up_num, true);
        show_left_down(left_down_num, true);
        show_right_up(right_up_num, true);
        show_right_down(right_down_num, true);
        
        vTaskDelay( pdMS_TO_TICKS(500) );
        
        show_left_up(0, false);
        show_left_down(0, false);
        show_right_up(0, false);
        show_right_down(0, false);
        
        vTaskDelay( pdMS_TO_TICKS(500) );
    }
    
    for( ;; )
    {
        vTaskDelay( pdMS_TO_TICKS(100) );
        
        //�豸��һ��������Ϊδע��״̬��״̬�£���ʾ���٢ڢۢ�һֱ��ʾ������豸������Ϣ����������˸��ʾ��ֱ���û����豸֮�󼤻��ײ͡�
        //����û��ײ��Ѿ�ʹ���꣬�����ѵ��ڣ����豸״̬��Ϊδ����״̬��δ����״̬��δע��״̬��Ƿ��ֻ����ʾʱ��������ĵ���˸
        if ((0 == water.save.is_bind) &&  (water.is_test_mode_switch_off == 1))
        {
            led_time = TIME_0;
            led_day = DAY_0;
            led_flow = FLOW_0;
            led_liter = LITER_0;
            led_raw_water = RAW_WATER_0;
            led_raw_ppm_d30 = RAW_PPM_D30_0;
            led_raw_ppm_d31 = RAW_PPM_D31_0;
            led_clean_water = CLEAN_WATER_0;
            led_clean_ppm = CLEAN_PPM_0;
            
            led_multifunc = MULTIFUNC_0;
            led_drops = DROPS_0;
            //led_signal = SIGNAL_0;
            
            if (0 == water.is_screen_status_off)
            {
                static uint8_t count_500ms = 0;
                if (count_500ms++ >= 5)//��˸��ʾ�豸����
                {
                    count_500ms = 0;
                    static bool is_show = false;
                    is_show = !is_show;
                    show_left_up(left_up_num, is_show);
                    show_left_down(left_down_num, is_show);
                    show_right_up(right_up_num, is_show);
                    show_right_down(right_down_num, is_show);
                }
                if (!edp.is_tcpip_connect)
                    led_signal = SIGNAL_0;
                else
                    led_signal = SIGNAL_1;
            }
            else if (1 == water.is_screen_status_off)
            {
                led_signal = SIGNAL_0;
                show_left_up(0, 0);
                show_left_down(0, 0);
                show_right_up(0, 0);
                show_right_down(0, 0);
            }
        }
        else
        {
            if (1 == water.is_screen_status_off || (water.is_power_on_status == 0))//6	�ػ�
            {
                led_time = TIME_0;
                led_day = DAY_0;
                led_flow = FLOW_0;
                led_liter = LITER_0;
                led_raw_water = RAW_WATER_0;
                led_raw_ppm_d30 = RAW_PPM_D30_0;
                led_raw_ppm_d31 = RAW_PPM_D31_0;
                led_clean_water = CLEAN_WATER_0;
                led_clean_ppm = CLEAN_PPM_0;
                
                led_multifunc = MULTIFUNC_0;
                led_drops = DROPS_0;
                led_signal = SIGNAL_0;
                
                show_left_up(0, 0);
                show_left_down(0, 0);
                show_right_up(0, 0);
                show_right_down(0, 0);
            }
            else
            {
                led_time = TIME_1;
                led_day = DAY_1;
                led_flow = FLOW_1;
                led_liter = LITER_1;
                led_raw_water = RAW_WATER_1;
                led_raw_ppm_d30 = RAW_PPM_D30_1;
                led_raw_ppm_d31 = RAW_PPM_D31_1;
                led_clean_water = CLEAN_WATER_1;
                led_clean_ppm = CLEAN_PPM_1;
                
                if (!edp.is_tcpip_connect)
                    led_signal = SIGNAL_0;
                else
                    led_signal = SIGNAL_1;
                
                static uint8_t count_200ms = 0;
                if (count_200ms++ >= 2)
                {
                    count_200ms = 0;

                    if (1 == water.is_drop_water_status)//©ˮ����������
                    {
                        //�๦�ܵ�Ϩ��ˮ�εƴ���������˸˵��©ˮ��������
                        led_multifunc = MULTIFUNC_0;
                        static uint8_t n = 0;
                        switch (n)
                        {
                        case 0: led_drops = DROPS_0; n = 1; break;
                        case 1: led_drops = DROPS_1; n = 2; break;
                        case 2: led_drops = DROPS_2; n = 3; break;
                        case 3: led_drops = DROPS_3; n = 0; break;
                        default:n = 0;break;
                        }
                    }
                    else if (1 == water.is_shortage_water_status)//ˮѹ����
                    {
                        //�๦�ܵ�Ϩ��
                        led_multifunc = MULTIFUNC_0;
                        led_drops = DROPS_0;
                    }
                    else if (1 == water.is_overhaul_status)//����9����ѹ���س���ʱ��
                    {
                        //�๦�ܵ�һֱ��˸�����豸��Ҫ����
                        static uint8_t n = 0;
                        switch (n)
                        {
                        case 0: led_multifunc = MULTIFUNC_0; n = 1; break;
                        case 1: led_multifunc = MULTIFUNC_3; n = 0; break;
                        default:n = 0;break;
                        }
                        led_drops = DROPS_0;
                    }
                    else if (1 == water.is_bushing_now)
                    {
                        //�๦�ܵƳ���������ѹ��Ͱˮ��
                        led_multifunc = MULTIFUNC_2;
                        led_drops = DROPS_0;
                    }
                    else if (1 == water.is_full_water_status)//ˮ��
                    {
                        //�๦�ܵƳ���������ѹ��Ͱˮ��
                        led_multifunc = MULTIFUNC_3;
                        led_drops = DROPS_0;
                    }
                    else if (1 == water.is_make_water_status)//������ˮ
                    {
                        //�๦�ܵ����������������豸������ˮ
                        static uint8_t n = 0;
                        switch(n)
                        {
                        case 0: led_multifunc = MULTIFUNC_0; n = 1; break;
                        case 1: led_multifunc = MULTIFUNC_1; n = 2; break;
                        case 2: led_multifunc = MULTIFUNC_2; n = 3; break;
                        case 3: led_multifunc = MULTIFUNC_3; n = 0; break;
                        default:n = 0;break;
                        }
                        led_drops = DROPS_0;
                    }
                    else
                    {
                        led_multifunc = MULTIFUNC_0;
                        led_drops = DROPS_0;
                    }
                    
                    if (water.is_test_mode_switch_off == 1)
                    {
                        if ( water.device_status == 4 )
                        {
                            static bool is_show = false;
                            is_show = !is_show;
                            if (water.save.raw_water_tds_switch == 0)//0��ʾĬ��ֵ��1��ʾʵʱֵ
                                show_left_up(water.save.raw_water_ppm, is_show);
                            else if (water.save.raw_water_tds_switch == 1)
                                show_left_up(water.raw_water_ppm, is_show);
                            
                            if (water.save.clean_water_tds_switch == 0)//0��ʾĬ��ֵ��1��ʾʵʱֵ
                                show_right_up(water.save.clean_water_ppm, is_show);
                            else if (water.save.clean_water_tds_switch == 1)
                                show_right_up(water.clean_water_ppm, is_show);
                        }
                        else
                        {
                            if (water.save.raw_water_tds_switch == 0)//0��ʾĬ��ֵ��1��ʾʵʱֵ
                                show_left_up(water.save.raw_water_ppm, 1);
                            else if (water.save.raw_water_tds_switch == 1)
                                show_left_up(water.raw_water_ppm, 1);
                            
                            if (water.save.clean_water_tds_switch == 0)//0��ʾĬ��ֵ��1��ʾʵʱֵ
                                show_right_up(water.save.clean_water_ppm, 1);
                            else if (water.save.clean_water_tds_switch == 1)
                                show_right_up(water.clean_water_ppm, 1);
                        }
                        
                        
                        if (water.save.screen_mode_switch.day == 0x00)//��������
                        {
                            if (water.device_status == 0x03)//Ƿ��
                            {
                                static bool is_show = false;
                                is_show = !is_show;
                                show_left_down(water.used_days, is_show);
                            }
                            else
                                show_left_down(water.used_days, 1);
                        }
                        else if (water.save.screen_mode_switch.day == 0x01)//ʣ������
                        {
                            if (water.device_status == 0x03)//Ƿ��
                            {
                                static bool is_show = false;
                                is_show = !is_show;
                                show_left_down(water.remain_days, is_show);
                            }
                            else
                                show_left_down(water.remain_days, 1);
                        }
                        
                        
                        if (water.save.work_mode == 0x01) //1	ʱ��ģʽ
                        {
                            show_right_down((uint32_t)(water.save.used_flow*0.917 / 1000 + 0.5), 1);//����Ǖr�Lģʽ��Ԓ����ֻ�@ʾ����������ʣ�N�������ÿ��]���K�Ҳ��ù��Ƿ�ʣ�N�������ن��}
                        }
                        else if (water.save.screen_mode_switch.flow == 0x02)//��������         
                        {
                            if (water.device_status == 0x03)//Ƿ��
                            {
                                static bool is_show = false;
                                is_show = !is_show;
                                show_right_down((uint32_t)(water.save.used_flow*0.917 / 1000 + 0.5), is_show);
                            }
                            else
                                show_right_down((uint32_t)(water.save.used_flow*0.917 / 1000 + 0.5), 1);
                        }
                        else if (water.save.screen_mode_switch.flow == 0x03)//ʣ������
                        {
                            if (water.device_status == 0x03)//Ƿ��
                            {
                                static bool is_show = false;
                                is_show = !is_show;
                                show_right_down((uint32_t)(water.save.remain_flow*0.917 / 1000 + 0.5), is_show);
                            }
                            else
                                show_right_down((uint32_t)(water.save.remain_flow*0.917 / 1000 + 0.5), 1);
                        }
                    }
                    else//����ģʽ����˸LED��
                    {
//                        static int count_500ms = 0;
//                        if (count_500ms++ >= 2)
//                        {
//                            count_500ms = 0;
                            static bool is_show = false;
                            is_show = !is_show;
                            show_left_up(water.raw_water_ppm, is_show);
                            show_right_up(water.clean_water_ppm, is_show);
                            show_left_down(water.test_used_days, is_show);
                            show_right_down((uint32_t)(water.test_used_flow*0.917 / 1000 + 0.5), is_show);
//                        }
                    }
                    
                }//if (count_200ms++ >= 2)
            }//if (1 == water.is_screen_status_off || (water.is_power_on_status == 0))
        }//if (0 == water.save.is_bind)
    }//for( ;; )
}
