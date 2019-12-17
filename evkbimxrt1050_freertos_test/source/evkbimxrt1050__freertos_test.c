/*
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include <stdio.h>

/* FreeRTOS kernel includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* NXP includes */
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1052.h"
#include "fsl_debug_console.h"
#include "fsl_common.h"

/* Application includes */
#include "board_extra.h"

/* FreeRTOS definitions */
#define hello_task_PRIORITY       (0)
#define gpio_task1_PRIORITY       (1)
#define user_button_task_PRIORITY (2)
#define isr_PRIORITY              (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)

extern void hello_task(void *pvParameters);
extern void gpio_task(void *pvParameters);
extern void user_button_task(void *pvParameters);

extern SemaphoreHandle_t xSemaphoreSw8;

static const char pcNameHelloTask[]      = "T1";
static const char pcNameGpioTask1[]      = "T2";
static const char pcNameUserButtonTask[] = "T3";

const int iPinGpioTask1      = BOARD_J22P7_PIN;
const int iPinUserButtonTask = BOARD_J22P8_PIN;

/* -------------------------------------------------------------- */

static void init_gpio()
{
	/* Define the init structure for the output pins pin */
	gpio_pin_config_t out_config = {kGPIO_DigitalOutput, 1, kGPIO_NoIntmode};

	/* Init output LED GPIO */
	GPIO_PinInit(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, &out_config);

	/* Init output J22-PIN7 */
	GPIO_PinInit(BOARD_J22P7_GPIO, BOARD_J22P7_PIN, &out_config);

	/* Init output J22-PIN8 */
	GPIO_PinInit(BOARD_J22P8_GPIO, BOARD_J22P8_PIN, &out_config);

	/* Define the init structure for the input pin for user button SW8 */
	gpio_pin_config_t sw_config = {kGPIO_DigitalInput, 0, kGPIO_IntFallingEdge};

	/* Init input pin for user button SW8 */
	GPIO_PinInit(BOARD_USER_BUTTON_GPIO, BOARD_USER_BUTTON_GPIO_PIN, &sw_config);
}

/* -------------------------------------------------------------- */
static void init_interrupts()
{
	/* Enable gpio pin interrupt for user button SW8 */
	xSemaphoreSw8 = xSemaphoreCreateBinary();
	NVIC_SetPriority(BOARD_USER_BUTTON_IRQ, isr_PRIORITY);
	GPIO_PortEnableInterrupts(BOARD_USER_BUTTON_GPIO, 1U << BOARD_USER_BUTTON_GPIO_PIN);
	EnableIRQ(BOARD_USER_BUTTON_IRQ);

	/* Enable interrupt for QuadTimer1 */
	QTMR_EnableInterrupts(QUADTIMER_1_PERIPHERAL, QUADTIMER_1_CHANNEL_0_CHANNEL, kQTMR_CompareInterruptEnable);
	EnableIRQ(QUADTIMER_1_IRQN);
}

/* -------------------------------------------------------------- */

int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    /* Init application specific gpio */
    init_gpio();

    /* Init application specific interrupts */
    init_interrupts();

    /* Test memory management, assumes heap size > 1MB, typical 2MB, placed in SDRAM */
    const size_t sdram_test_size = 1 * 1024 * 1024;
    uint8_t *pSdramTest = malloc(sdram_test_size);
    memset(pSdramTest, 0xab, sdram_test_size);
    uint32_t *pSdram = (uint32_t *)pSdramTest;
    for (int i=0; i < (sdram_test_size / 4); i++)
    {
    	*pSdram++ = i; // create a ramp that can be observed by the debugger
    }

    /* Turn on LED */
    GPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, 0U);

    /* Set J22-PIN7/PIN8 */
    GPIO_PinWrite(BOARD_J22P7_GPIO, BOARD_J22P7_PIN, 1U);
    GPIO_PinWrite(BOARD_J22P8_GPIO, BOARD_J22P8_PIN, 1U);

	// Macro BUILD_SDRAM shall be defined when building for SDRAM.
	// Make it easy to identify if SDRAM version is running.
#if defined(BUILD_SDRAM) && (BUILD_SDRAM == 1)
    printf("evkbimxrt1050-freertos-test-SDRAM\r\n");
#else
    printf("evkbimxrt1050-freertos-test-FLASH\r\n");
#endif

    /* Set J22-PIN7/PIN8 */
    GPIO_PinWrite(BOARD_J22P7_GPIO, BOARD_J22P7_PIN, 0U);
    GPIO_PinWrite(BOARD_J22P8_GPIO, BOARD_J22P8_PIN, 0U);

    /* Check heap before start of FreeRTOS */
    int iFreeHeap;
    iFreeHeap = xPortGetFreeHeapSize();
    printf("FreeRTOS - Heap(0) = %d\r\n", iFreeHeap);

    /* Create the tasks */
    if (xTaskCreate(hello_task,
    				pcNameHelloTask,
					configMINIMAL_STACK_SIZE + 10,
					(void *)pcNameHelloTask,
					hello_task_PRIORITY,
					NULL) != pdPASS)
    {
    	printf("Task creation failed (%s)!\r\n", pcNameHelloTask);
        while (1) ;
    }
    iFreeHeap = xPortGetFreeHeapSize();
    printf("FreeRTOS - Heap(1) = %d\r\n", iFreeHeap);

    if (xTaskCreate(gpio_task,
    				pcNameGpioTask1,
    				configMINIMAL_STACK_SIZE + 10,
    				(void *) &iPinGpioTask1,
    				gpio_task1_PRIORITY,
    				NULL) != pdPASS)
    {
    	printf("Task creation failed (%s)!\r\n", pcNameGpioTask1);
        while (1) ;
    }
    iFreeHeap = xPortGetFreeHeapSize();
    printf("FreeRTOS - Heap(2) = %d\r\n", iFreeHeap);

    if (xTaskCreate(user_button_task,
    				pcNameUserButtonTask,
					configMINIMAL_STACK_SIZE + 10,
					(void *) &iPinUserButtonTask,
					user_button_task_PRIORITY,
					NULL) != pdPASS)
    {
     	printf("Task creation failed (%s)!\r\n", pcNameUserButtonTask);
        while (1) ;
    }
    iFreeHeap = xPortGetFreeHeapSize();
    printf("FreeRTOS - Heap(3) = %d\r\n", iFreeHeap);

    vTaskStartScheduler();
    return 0;
}
