/*
 * test.h
 *
 * Created on: Jun 3, 2020
 * Author: Stephane Amans
 */
#ifndef TEST_H_
#define TEST_H_

/* Include files:        */
#include "soc_configuration.h"
#include "error.h"

/** Print test header welcome message.
 *
 * \param: void
 *
 * \return: Error code
 *
 */
t_error_handling print_test_header(void);


t_error_handling spi_test(void);

#endif /* TEST_H_ */
