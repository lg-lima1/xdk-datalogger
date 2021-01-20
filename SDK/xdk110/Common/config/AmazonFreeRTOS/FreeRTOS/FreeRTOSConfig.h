/*
 * FreeRTOS Kernel V10.0.1
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
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @ingroup CONFIGURATION
 *
 * @defgroup FREERTOSCONFIG FreeRTOSConfig
 * @{
 *
 * @brief FreeRTOS Configuration
 *
 * @details
 *
 * @file
 *
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/********************** Configuration of FreeRTOS ****************************/

#ifndef NDEBUG
#include  <stdint.h>
#include "BCDS_Assert.h"
#define configASSERT(expression) if (!(expression)) { Assert_Dynamic((unsigned long)__LINE__,(unsigned char*)__FILE__); }
#endif

#include "BCDS_TaskConfig.h"

/* Modes of operations of operation system */
#define configUSE_PREEMPTION       ( 1 )

/* Energy saving modes */
#define configUSE_TICKLESS_IDLE    ( 0 )/*is disabled as in the low energy modes,it disables high frequency peripherals like USB*/

/* Available options when configUSE_TICKLESS_IDLE set to 1 
 * or configUSE_SLEEP_MODE_IN_IDLE set to 1 :
 * 1 - EM1, 2 - EM2, 3 - EM3 is not available on this CPU, because
 * timer doesn't work in EM3 mode */
#define configSLEEP_MODE           ( 0 )
#define BCDS_FREE_RTOS_VERSION_MAJOR (10)
#define BCDS_FREE_RTOS_VERSION_MINOR (0)
#define BCDS_FREE_RTOS_VERSION_BUILD (1)

/* Definition used only if configUSE_TICKLESS_IDLE == 0 */
#define configUSE_SLEEP_MODE_IN_IDLE       ( 0 )
#define configPRE_SLEEP_PROCESSING( param)
#define configPOST_SLEEP_PROCESSING( param )

/* EM1 use systick as system clock*/
/* EM2 use crystal 32768Hz and RTC Component as system clock
 * We use 2 times divider of this clock to reduce energy consumption
 * You can also in this mode choose crystal oscillator to get more precision in
 * time measurement or RC oscillator for more energy reduction.*/
/* EM3 use 2kHz RC and BURTC Component as system clock*/
#if ( ( configSLEEP_MODE == 2 ) && ( configUSE_TICKLESS_IDLE == 1 || configUSE_SLEEP_MODE_IN_IDLE == 1 ) )
/* Choose source of clock for RTC (system tick) 
 * if configCRYSTAL_IN_EM2 set to 1 then Crystal oscillator is used,
 * when 0 RC oscillator */
#define configCRYSTAL_IN_EM2    ( 1 )
#endif

#if (  (configSLEEP_MODE == 2 ) && ( configUSE_TICKLESS_IDLE == 1 || configUSE_SLEEP_MODE_IN_IDLE == 1 ) )
/* When we use EM2 or EM3 System clock has got low frequency,
 * so we reduce Tick rate to 1000 Hz and 40 Hz, which give more clock cycles between ticks*/
#define configTICK_RATE_HZ    ( 1000 )
#elif (  ( configSLEEP_MODE == 3 ) && ( configUSE_TICKLESS_IDLE == 1 || configUSE_SLEEP_MODE_IN_IDLE == 1 ) )
#define configTICK_RATE_HZ    ( 40 )
#else
#define configTICK_RATE_HZ    ( 1000 )
#endif

/* Definition used by Keil to replace default system clock source when we use EM2 or EM3 mode. */
#if ( ( configSLEEP_MODE == 2 || configSLEEP_MODE == 3 ) && ( configUSE_TICKLESS_IDLE == 1 || configUSE_SLEEP_MODE_IN_IDLE == 1 ) )
#define configOVERRIDE_DEFAULT_TICK_CONFIGURATION ( 1 )
#endif

/* Main functions*/
#define configMAX_PRIORITIES                      (5)
#define configMINIMAL_STACK_SIZE                  (( unsigned short ) 160)
#ifndef configTOTAL_HEAP_SIZE
#define configTOTAL_HEAP_SIZE                     (( size_t )(65 * 1024 ))
#endif
#define configMAX_TASK_NAME_LEN                   ( 10 )
#define configUSE_TRACE_FACILITY                  ( 0 )
#define configUSE_16_BIT_TICKS                    ( 0 )
#define configIDLE_SHOULD_YIELD                   ( 0 )
#define configUSE_MUTEXES                         ( 1 )
#define configUSE_RECURSIVE_MUTEXES               ( 1 )
#define configUSE_COUNTING_SEMAPHORES             ( 1 )
#define configUSE_ALTERNATIVE_API                 ( 0 )/* Deprecated! */
#define configQUEUE_REGISTRY_SIZE                 ( 10 )
#define configUSE_QUEUE_SETS                      ( 1 )
#if BCDS_FREERTOS_INCLUDE_AWS
#define configSUPPORT_STATIC_ALLOCATION           ( 1 )
#endif
#define configSUPPORT_DYNAMIC_ALLOCATION          ( 1 )

/* Hook function related definitions. */
#define configUSE_TICK_HOOK                       ( 0 )
#define configCHECK_FOR_STACK_OVERFLOW            ( 2 ) /* This sets stack overflow detection Method 2.
                                                        Checks whether the stack pointer remains  with valid stack area and
                                                        also verifies the limit of the valid stack region has not been overwritten.
                                                        If any one of these case occurs then stack overflow hook will be triggered */
#ifndef configUSE_MALLOC_FAILED_HOOK
#define configUSE_MALLOC_FAILED_HOOK              ( 1 )
#endif

/* Run time status gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS             ( 0 )

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                     ( 0 )
#define configMAX_CO_ROUTINE_PRIORITIES           ( 1 )

/* Software timer related definitions. */
#define CONFIGTIMER_STACKSIZE                     (configMINIMAL_STACK_SIZE+(unsigned short)1000) /* This size may need to be changed depending the no of SW timers involved in the application */
#define INCLUDE_xTimerPendFunctionCall            ( 1 )
#define configUSE_TIMERS                          ( 1 )
#define configTIMER_TASK_PRIORITY                 (TASK_PRIORITY_TIMER )
#define configTIMER_QUEUE_LENGTH                  ( 10 )
#ifndef configTIMER_TASK_STACK_DEPTH
#define configTIMER_TASK_STACK_DEPTH              CONFIGTIMER_STACKSIZE
#endif
/* Interrupt nesting behavior configuration. */
#define configKERNEL_INTERRUPT_PRIORITY           ( 255 ) 
#define configMAX_SYSCALL_INTERRUPT_PRIORITY      ( 191 ) /* equivalent to 0xa0, or priority 5. */

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                  ( 1 )
#define INCLUDE_uxTaskPriorityGet                 ( 1 )
#define INCLUDE_vTaskDelete                       ( 1 )
#define INCLUDE_vTaskSuspend                      ( 1 )
#define INCLUDE_xResumeFromISR                    ( 1 )
#define INCLUDE_vTaskDelayUntil                   ( 1 )
#define INCLUDE_vTaskDelay                        ( 1 )
#define INCLUDE_xTaskGetSchedulerState            ( 1 )
#define INCLUDE_xTaskGetCurrentTaskHandle         ( 1 )
#define INCLUDE_uxTaskGetStackHighWaterMark       ( 0 )
#define INCLUDE_xTaskGetIdleTaskHandle            ( 0 )
#define INCLUDE_xTimerGetTimerDaemonTaskHandle    ( 0 )
#define INCLUDE_pcTaskGetTaskName                 ( 0 )
#define INCLUDE_eTaskGetState                     ( 1 )
#define INCLUDE_xTaskAbortDelay					  ( 1 )

/* Default value of CPU clock (RC)*/
#define configCPU_CLOCK_HZ                        (( unsigned long ) 48000000L)

/* Defines used in energy modes */
#if ( ( configSLEEP_MODE == 2 )  && ( ( configUSE_SLEEP_MODE_IN_IDLE == 1 ) || ( configUSE_TICKLESS_IDLE == 1 ) ) )
#define configSYSTICK_CLOCK_HZ    ( 16384 )
#endif

#if ( ( configSLEEP_MODE == 3 )  && ( ( configUSE_SLEEP_MODE_IN_IDLE == 1 ) || ( configUSE_TICKLESS_IDLE == 1 ) ) )
#define configSYSTICK_CLOCK_HZ    ( 2000 )
#endif

/* Since watch dog feed is executed in vApplicationIdleHook, this macro should be set*/
#define configUSE_IDLE_HOOK  ( 1 )

/*-----------------------------------------------------------*/

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
 * standard names. */
#define vPortSVCHandler        SVC_Handler
#define xPortPendSVHandler     PendSV_Handler

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_CONFIG_H */

/**@}*/
