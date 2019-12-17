/*
 * runtime_statc.c
 *
 *  Created on: Dec 7, 2019
 *      Author: emwhbr
 */

#include <stdint.h>

#include "peripherals.h"

volatile uint32_t FreeRTOS_RunTimeCounter = 0;

/* -------------------------------------------------------------- */

void ConfigureTMR1ForRuntimeStats()
{
	/* Configuration already done by peripherals initialization, see QuadTimer_1_init() */

	/* Start the timer - select the timer counting mode */
	QTMR_StartTimer(QUADTIMER_1_PERIPHERAL, QUADTIMER_1_CHANNEL_0_CHANNEL, kQTMR_PriSrcRiseEdge);
}

/* -------------------------------------------------------------- */

uint32_t GetFreeRTOS_RunTimeCounter()
{
	return FreeRTOS_RunTimeCounter;
}

/* -------------------------------------------------------------- */

/*
 * Interrupt service routine for quad timer 1
 */
void TMR1_IRQHandler(void)
{
	/* Clear interrupt flag */
	QTMR_ClearStatusFlags(QUADTIMER_1_PERIPHERAL, QUADTIMER_1_CHANNEL_0_CHANNEL, kQTMR_CompareFlag);

	/* Update runtime counter, F=10kHz, overflow in approx. 5 days */
	FreeRTOS_RunTimeCounter++;
}
