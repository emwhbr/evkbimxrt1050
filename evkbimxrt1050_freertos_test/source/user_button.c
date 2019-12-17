/*
 * user_button.c
 *
 *  Created on: Nov 17, 2019
 *      Author: emwhbr
 */

/* FreeRTOS kernel includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* NXP includes */
#include "board.h"

/* Application includes */
#include "board_extra.h"

#define SW8_IRQHandler BOARD_USER_BUTTON_IRQ_HANDLER

SemaphoreHandle_t xSemaphoreSw8;

/* -------------------------------------------------------------- */

/*
 * Interrupt service routine for user button SW8
 */
void SW8_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* clear the interrupt status */
    GPIO_PortClearInterruptFlags(BOARD_USER_BUTTON_GPIO, 1U << BOARD_USER_BUTTON_GPIO_PIN);

    /* Set gpio pin low */
    GPIO_PinWrite(BOARD_J22P8_GPIO, BOARD_J22P8_PIN, 0);

    /* release worker task */
    xSemaphoreGiveFromISR(xSemaphoreSw8, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* -------------------------------------------------------------- */

void user_button_task(void *pvParameters)
{
	const int *piPin = (const int *) pvParameters;

	/* Set gpio pin high */
	GPIO_PinWrite(BOARD_J22P8_GPIO, BOARD_J22P8_PIN, 1);

    for (;;)
    {
    	/* wait for ISR */
    	xSemaphoreTake(xSemaphoreSw8, portMAX_DELAY);

    	/* Set gpio pin high */
    	if (*piPin == BOARD_J22P8_PIN)
    	{
    		GPIO_PinWrite(BOARD_J22P8_GPIO, BOARD_J22P8_PIN, 1);
    	}
    }
}
