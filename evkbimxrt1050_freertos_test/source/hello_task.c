#include <stdio.h>

/* FreeRTOS kernel includes */
#include "FreeRTOS.h"
#include "task.h"

/* NXP includes */
#include "board.h"

/* -------------------------------------------------------------- */

void hello_task(void *pvParameters)
{
	const char *pcTaskName = (const char *) pvParameters;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	int cnt = 0;
	uint8_t output;

    for (;;)
    {
    	// Macro BUILD_SDRAM shall be defined when building for SDRAM.
    	// LED shall blink faster to make it easy to identify if SDRAM version is running.
#if defined(BUILD_SDRAM) && (BUILD_SDRAM == 1)
    	vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 250 ) );
#else
    	vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1000 ) );
#endif

    	/* Play with FreeRTOS memory management, assumes heap is large enough and placed in SDRAM */
    	const size_t mem_test_size = 250 * 1024; // assumes enough FreeRTOS heap size, configTOTAL_HEAP_SIZE
    	uint8_t *pMemTest = pvPortMalloc(mem_test_size);
    	memset(pMemTest, 0x12, mem_test_size);
    	uint32_t *pMem = (uint32_t *)pMemTest;
    	for (int i=0; i < (mem_test_size / 4); i++)
    	{
    		*pMem++ = i; // create a ramp that can be observed by the debugger
    	}
    	vPortFree(pMemTest);
    	pMemTest = NULL;

    	/* Toggle LED */
    	output = ((++cnt) % 2 == 0) ? 1 : 0;
    	GPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, output);

    	printf("FreeRTOS - task - %s : Alive - %d\r\n", pcTaskName, cnt);
    }
}
