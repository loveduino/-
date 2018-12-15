/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

/* Ensure stdint is only used by the compiler, and not the assembler. */
//��Բ�ͬ�ı��������ò�ͬ��stdint.h�ļ�
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif


/******************************************************************************/
/*                FreeRTOS������������ѡ��                                    */
/******************************************************************************/
#define configUSE_PREEMPTION			        1//Ϊ1ʱRTOSʹ����ռʽ��������Ϊ0ʱRTOSʹ��Э��ʽ��������ʱ��Ƭ��
#define configUSE_TIME_SLICING					1//1ʹ��ʱ��Ƭ����(Ĭ��ʽʹ�ܵ�)�����������Թ���һ�����ȼ����������ޡ�
#define configUSE_PORT_OPTIMISED_TASK_SELECTION	0//1�������ⷽ����ѡ����һ��Ҫ���е������������1��configMAX_PRIORITIES���ܳ���32����������£�configMAX_PRIORITIES����Ϊ����ֵ��
                                                 //һ����Ӳ������ǰ����ָ������ʹ�õ�
                                                 //MCUû����ЩӲ��ָ��Ļ��˺�Ӧ������Ϊ0��
#define configUSE_QUEUE_SETS					1//Ϊ1ʱ���ö���
#define configCPU_CLOCK_HZ				        ( SystemCoreClock )//��ʵ�ʵ�CPU�ں�ʱ��Ƶ�ʣ�Ҳ����CPUָ��ִ��Ƶ�ʣ�ͨ����ΪFcclk
#define configTICK_RATE_HZ				        ( ( TickType_t ) 1000 )//RTOS ϵͳ�����жϵ�Ƶ��
#define configMAX_PRIORITIES			        ( 32 )//��ʹ�õ�������ȼ����û�ʵ�ʿ���ʹ�õ����ȼ���Χ��0��configMAX_PRIORITIES�C 1;�û�������������ȼ���ֵԽС����ô����������ȼ�Խ�ͣ�������������ȼ���0��
#define configMINIMAL_STACK_SIZE		        ( ( unsigned short ) 128 )//��������ʹ�õĶ�ջ��С��The number of words (not bytes!) to allocate for use as the task's stack. 
#define configMAX_TASK_NAME_LEN			        ( 16 )//���������ַ�������
#define configUSE_16_BIT_TICKS					0//ϵͳ���ļ����������������ͣ�
                                                 //1��ʾΪ16λ�޷������Σ�0��ʾΪ32λ�޷�������
#define configIDLE_SHOULD_YIELD					1//Ϊ1ʱ�����������CPUʹ��Ȩ������ͬ���ȼ����û�����
#define configUSE_TASK_NOTIFICATIONS            1//Ϊ1ʱ��������֪ͨ���ܣ�Ĭ�Ͽ���
#define configUSE_MUTEXES						1//Ϊ1ʱʹ�û����ź���
#define configQUEUE_REGISTRY_SIZE				8//��Ϊ0ʱ��ʾ���ö��м�¼�������ֵ�ǿ��Լ�¼�Ķ��к��ź��������Ŀ��
#define configCHECK_FOR_STACK_OVERFLOW			2//����0ʱ���ö�ջ�����⹦�ܣ����ʹ�ô˹���
                                                 //�û������ṩһ��ջ������Ӻ��������ʹ�õĻ�
                                                 //��ֵ����Ϊ1����2����Ϊ������ջ�����ⷽ����1�����ϵļ�������ջָ���Ƿ�ָ����Ч�ռ䣬���ָ������Ч�ռ䣬�ͻ���ù��Ӻ������ŵ�죬ȱ�㲻�ܼ�����еĶ�ջ�����
                                                 //2: ���������ʱ����������ջ���һ����֪�ı��ֵ����һֱ����ջ����ļ���bytes�����ֵ���Ƿ񱻸�д���������д�͵��ù��Ӻ������ŵ�����ܼ�����еĶ�ջ�����ȱ�����1��һЩ��
                                                 //��ջ������������������л��Ŀ����������ڵ��Ե�ʱ��ʹ�ã�
#define configUSE_RECURSIVE_MUTEXES				1//Ϊ1ʱʹ�õݹ黥���ź���
#define configUSE_MALLOC_FAILED_HOOK			1//1ʹ���ڴ�����ʧ�ܹ��Ӻ���
#define configUSE_APPLICATION_TASK_TAG			0
#define configUSE_COUNTING_SEMAPHORES			1//Ϊ1ʱʹ�ü����ź���

/* The full demo always has tasks to run so the tick will never be turned off.
The blinky demo will use the default tickless idle implementation to turn the
tick off. */
#define configUSE_TICKLESS_IDLE					0//1���õ͹���ticklessģʽ


/******************************************************************************/
/*             FreeRTOS���ڴ������й�����ѡ��                                 */
/******************************************************************************/
#define configSUPPORT_DYNAMIC_ALLOCATION        1//֧�ֶ�̬�ڴ�����
#define configSUPPORT_STATIC_ALLOCATION			0//֧�־�̬�ڴ棬�ڴ���һЩ�ں˶����ʱ����Ҫ�û�ָ��RAM
#define configTOTAL_HEAP_SIZE					( ( size_t ) ( 20 * 1024 ) )//ϵͳ�����ܵĶѴ�С
                                                 //RTOS�ں��ܼƿ��õ���Ч��RAM��С��
                                                 //������ʹ�ùٷ����ذ��и������ڴ�������ʱ�����п����õ���ֵ��
                                                 //ÿ���������񡢶��С��������������ʱ�����ź���ʱ��RTOS�ں˻�Ϊ�˷���RAM�������RAM������configTOTAL_HEAP_SIZEָ�����ڴ�����


/******************************************************************************/
/*             FreeRTOS�빳�Ӻ����йص�����ѡ��                               */
/******************************************************************************/
#define configUSE_IDLE_HOOK						1//1��ʹ�ÿ��й��ӣ�0����ʹ��
#define configUSE_TICK_HOOK						1//1��ʹ��ʱ��Ƭ���ӣ�0����ʹ��


/* Run time stats gathering definitions. */
/******************************************************************************/
/*             FreeRTOS������ʱ�������״̬�ռ��йص�����ѡ��                 */
/******************************************************************************/
#define configGENERATE_RUN_TIME_STATS	        0//Ϊ1ʱ��������ʱ��ͳ�ƹ���
/*
If configGENERATE_RUN_TIME_STATS is defined then portCONFIGURE_TIMER_FOR_RUN_TIME_STATS must also be defined.  
portCONFIGURE_TIMER_FOR_RUN_TIME_STATS should call a port layer function to setup a peripheral timer/counter that can then be used as the run time counter time base.
*/
//��ʼ��һ�����裬��Ϊʱ��ͳ�ƵĻ�׼ʱ��
#if ( configGENERATE_RUN_TIME_STATS == 1 )
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
extern volatile unsigned long long FreeRTOSRunTimeTicks;
void ConfigureTimeForRunTimeStats(void);
#endif
    #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() ConfigureTimeForRunTimeStats()//��ʱ��3�ṩʱ��ͳ�Ƶ�ʱ����Ƶ��Ϊ10K��������Ϊ100us
#endif
/*
If configGENERATE_RUN_TIME_STATS is defined then either portGET_RUN_TIME_COUNTER_VALUE or portALT_GET_RUN_TIME_COUNTER_VALUE must also be defined.  
See the examples provided and the FreeRTOS web site for more information.
*/
//���ص�ǰ��׼ʱ�ӵ�ʱ��ֵ
#define portGET_RUN_TIME_COUNTER_VALUE()		FreeRTOSRunTimeTicks//��ȡʱ��ͳ��ʱ��ֵ

#define configUSE_TRACE_FACILITY		        1//���ó�1��ʾ�������ӻ����ٵ��ԣ��ἤ��һЩ���ӵĽṹ���Ա�ͺ�����
/* This demo makes use of one or more example stats formatting functions.  These
format the raw data provided by the uxTaskGetSystemState() function in to human
readable ASCII form.  See the notes in the implementation of vTaskList() within
FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS	1//���configUSE_TRACE_FACILITYͬʱΪ1ʱ���������3������
                                                 //prvWriteNameToBuffer(),vTaskList(),
                                                 //vTaskGetRunTimeStats()


/* Co-routine definitions. */
/******************************************************************************/
/*            FreeRTOS��Э���йص�����ѡ��                                    */
/******************************************************************************/
#define configUSE_CO_ROUTINES 			        0                       //Ϊ1ʱ����Э�̣�����Э���Ժ��������ļ�croutine.c���������ޣ�����رգ�
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )                   //Э�̵���Ч���ȼ���Ŀ


/* Software timer definitions. */
/******************************************************************************/
/*            FreeRTOS�������ʱ���йص�����ѡ��                              */
/******************************************************************************/
#define configUSE_TIMERS				        1                               //Ϊ1ʱ���������ʱ��
#define configTIMER_TASK_PRIORITY		        (configMAX_PRIORITIES-1)        //�����ʱ�����ȼ�
#define configTIMER_QUEUE_LENGTH		        10                              //�����ʱ�����г���
#define configTIMER_TASK_STACK_DEPTH	        (configMINIMAL_STACK_SIZE*2)    //�����ʱ�������ջ��С


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
/******************************************************************************/
/*           FreeRTOS��ѡ��������ѡ��                                         */
/******************************************************************************/
#define INCLUDE_xTaskGetSchedulerState          1 
#define INCLUDE_vTaskPrioritySet		        1
#define INCLUDE_uxTaskPriorityGet		        1
#define INCLUDE_vTaskDelete				        1
#define INCLUDE_vTaskCleanUpResources	        1
#define INCLUDE_vTaskSuspend			        1
#define INCLUDE_vTaskDelayUntil			        1
#define INCLUDE_vTaskDelay				        1
#define INCLUDE_eTaskGetState			        1
#define INCLUDE_xTimerPendFunctionCall	        1
#define	INCLUDE_xTaskGetHandle			        0
#define INCLUDE_uxTaskGetStackHighWaterMark     0//query how close a task has come to overflowing the stack space allocated to it.
                                                 //returns the minimum amount of remaining stack space that has been available since the task started executing.
                                                 //The closer the high water mark is to zero, the closer the task has come to overflowing its stack.
                                                 //���Խ׶�ʹ�ã�������ʱ����ò�Ҫʹ�ã���Ϊ�˺���������ʱ�䡣

/* Cortex-M0 specific definitions. */
/******************************************************************************/
/*          FreeRTOS���ж��йص�����ѡ��                                      */
/******************************************************************************/
/*!< STM32F0 uses 2 Bits for the Priority Levels    */
//STM32ֻ��4�����ȼ��������оƬ��ʱ���õ����ʽ���ȼ��ļ����Ͷ���Чλ���Լ������ȼ�����
//���ܲ��ö���λ��������ȼ�������MSB����ġ�
/*
| bit7    | bit6     | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
| <-���ڱ�����ȼ�-> | <-        û��ʵ�֣�������           -> |
*/
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		                __NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		                2   /* 4 priority levels */
#endif

/*
������û��жϺ������õ�freertos�ṩ��ϵͳ������һ��Ҫʹ�ô�FromISR��freertos�ṩ��ϵͳ������
��������û��жϵ����ȼ�һ��Ҫ��configKERNEL_INTERRUPT_PRIORITY��configMAX_SYSCALL_INTERRUPT_PRIORITY�����ȼ�֮�䣬
һ��configKERNEL_INTERRUPT_PRIORITY����Ϊ��Ƭ��������ȼ���configMAX_SYSCALL_INTERRUPT_PRIORITY��FreeRTOSϵͳ�ܹ����ε�������ȼ���
�涨�ж����ȼ���configMAX_SYSCALL_INTERRUPT_PRIORITY�߲��ܵ���FreeRTOS API����˸ߵ��ж����ȼ����Ѿ�����FreeRTOSϵͳ���Ʒ�Χ�ڣ���
*/

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
//����������ȼ���STM32F0���ȼ���ʹ����2λ��configPRIO_BITS��������STM32����ʹ����4��Ҳ����4λ������ռʽ���ȼ���
//������ȼ�������4����������ȼ����Ǿ���3.CORTEX�ں˵����ȼ�����ֵԽС���ȼ�Խ�ߣ���0��������ȼ���
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			3//�ж�������ȼ�

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
//����ϵͳ�ɹ��������ж����ȼ�������5�����ȼ������ȼ���С��2��0~1������FreeRTOS����
//�������κ�FreeRTOS API���жϿ���ʹ�����е��ж����ȼ����������ǿ����ж�Ƕ��
//���ȼ�0~1�жϲ��ᱻFreeRTOS��ֹ��������Ϊִ��FreeRTOS�ں˶���ʱ���жϲ��ɵ���FreeRTOS��API����
//���ȼ�2~3�жϿ��Ե����ԡ�FromISR����β��API�������������ǿ����ж�Ƕ��
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	2//FreeRTOSΪ������ĳЩӦ�ö��ж�ʵʱ��Ҫ��ߵ�����ʹ���ж����ȼ�����ĳ��ֵ֮�󣬾Ͳ��ܵ��ò���ϵͳ���ں˺��������ʵʱ�ԡ�

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
//�����ں��ж����ȼ���STM32ʹ����4λ��Ϊ���ȼ�������4λ�Ǹ�4λ�����Ҫ����4λ�������������ȼ���
//��freertosϵͳ�ں��жϣ�configKERNEL_INTERRUPT_PRIORITY�������ȼ����ó����
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )//0x0f -> 0xF0
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
//���ڴ����ȼ����жϿ��԰�ȫ�ĵ���FreeRTOS��API���������ڴ����ȼ����жϣ�FreeRTOS�ǲ��ܽ�ֹ�ģ��жϷ�����Ҳ���ܵ���FreeRTOS��API������
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )//0x50

/* This is the value being used as per the ST library which permits 4
priority values, 0 to 3.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY	3


/******************************************************************************/
/*                 FreeRTOS������йص�����ѡ��                               */
/******************************************************************************/
//STM32��ʹ�� USE_FULL_ASSERT ������Լ��
/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#ifdef USE_RTOS_ASSERT//���Ե�ʱ�����ʹ�ö��ԣ���������ɺ���ȥ�����ԣ���ֹ��������!
/* Ensure stdio is only used by the compiler, and not the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdio.h>
#endif
#define vAssertCalled( char, int ) printf( "Error:%s, %d\r\n", char, int )//vAssertCalled������Ҫ�û�����ȥ����
#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled(__FILE__,__LINE__)
//#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }
#else
#define configASSERT( x )   ((void)0)
#endif /* USE_RTOS_ASSERT */


/******************************************************************************/
/*                FreeRTOS���жϷ������йص�����ѡ��                        */
/******************************************************************************/
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names - or at least those used in the unmodified vector table. */
/* ��ֲҪʵ�ֵ������ж� , Ϊ�˾����ٸĶ������ļ���������жϺ������Ʋ��ı䣬���ε�ԭʼ���е��жϺ�������*/
#define vPortSVCHandler                         SVC_Handler
#define xPortPendSVHandler                      PendSV_Handler
#define xPortSysTickHandler                     SysTick_Handler
 
#endif /* FREERTOS_CONFIG_H */
