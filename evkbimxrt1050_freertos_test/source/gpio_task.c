#include <stdio.h>

/* FreeRTOS kernel includes */
#include "FreeRTOS.h"
#include "task.h"

/* NXP includes */
#include "board.h"

/* Application includes */
#include "board_extra.h"

/* -------------------------------------------------------------- */

void gpio_task(void *pvParameters)
{
	const int *piPin = (const int *) pvParameters;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	int cnt = 0;
	uint8_t output;

    for (;;)
    {
    	vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );

    	/* Toggle GPIO */
    	output = ((++cnt) % 2 == 0) ? 1 : 0;
    	if (*piPin == BOARD_J22P7_PIN)
    	{
    		GPIO_PinWrite(BOARD_J22P7_GPIO, BOARD_J22P7_PIN, output);
    	}
    }
}
