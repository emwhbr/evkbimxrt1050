/*
 * Copyright 2017-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v1.0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/*******************************************************************************
 * Included files
 ******************************************************************************/
#include "peripherals.h"

/*******************************************************************************
 * QuadTimer_1 initialization code
 *******************************************************************************/
const qtmr_config_t QuadTimer_1_Channel_0_config = {
  .primarySource = kQTMR_ClockDivide_8,
  .secondarySource = kQTMR_Counter0InputPin,
  .enableMasterMode = false,
  .enableExternalForce = false,
  .faultFilterCount = 0,
  .faultFilterPeriod = 0,
  .debugMode = kQTMR_RunNormalInDebug
};

void QuadTimer_1_init(void)
{
  /* Quad timer channel Channel_0 peripheral initialization */
  QTMR_Init(QUADTIMER_1_PERIPHERAL, QUADTIMER_1_CHANNEL_0_CHANNEL, &QuadTimer_1_Channel_0_config);

  /* Setup the timer period of the channel */
  QTMR_SetTimerPeriod(QUADTIMER_1_PERIPHERAL, QUADTIMER_1_CHANNEL_0_CHANNEL, 1875U);
}

/*******************************************************************************
 * BOARD_InitBootPeripherals function
 ******************************************************************************/
void BOARD_InitBootPeripherals(void)
{
    BOARD_ConfigMPU();
    QuadTimer_1_init();
}
