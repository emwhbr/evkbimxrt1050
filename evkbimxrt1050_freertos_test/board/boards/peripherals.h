/*
 * Copyright 2017-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus. */

#include "fsl_common.h"
#include "fsl_qtmr.h"
#include "fsl_clock.h"

/* Definition of peripheral ID */
#define QUADTIMER_1_PERIPHERAL TMR1
/* Definition of the timer channel Channel_0. */
#define QUADTIMER_1_CHANNEL_0_CHANNEL kQTMR_Channel_0
/* Definition of the timer channel Channel_0 clock source frequency. */
#define QUADTIMER_1_CHANNEL_0_CLOCK_SOURCE 18750000UL
/* QuadTimer_1 interrupt vector ID (number). */
#define QUADTIMER_1_IRQN TMR1_IRQn

/*******************************************************************************
 * BOARD_InitBootPeripherals function
 ******************************************************************************/
void BOARD_InitBootPeripherals(void);

/*******************************************************************************
 * BOARD_InitConfigMPU function
 ******************************************************************************/
void BOARD_ConfigMPU(void);

#if defined(__cplusplus)
}
#endif /*_cplusplus. */

#endif /* _PERIPHERALS_H_ */
