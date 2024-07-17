/*
 * driver_initialization.h
 *
 * Created on: Aug 21, 2022
 * Author: Stephane Amans
 *
 */

#ifndef DRIVER_INITIALIZATION_H_
#define DRIVER_INITIALIZATION_H_

/* Include files:        */
#include "soc_configuration.h"

/** Configure elements of the SoC.
 *
 * \param: void
 *
 * \return: void
 *
 */
void soc_configuration(void);

/** Configure platform peripherals.
 *
 * \param: void
 *
 * \return: void
 *
 */
void platform_configuration(void);

#endif /* DRIVER_INITIALIZATION_H_ */
