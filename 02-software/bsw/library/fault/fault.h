/*
 * fault.h
 *
 * Created on: Jun 10, 2021
 * Author: Stephane Amans
 */
#ifndef FAULT_H_
#define FAULT_H_

/* Include files:        */
#include <stdint.h>

#include "error.h"


/* Functions prototypes:                       */

/** Log the fault parameters.
 *
 * \param error: Error triggered in the calling function.
 *
 */
void fault_log_error(t_error_handling error);

#endif /* FAULT_H_ */
