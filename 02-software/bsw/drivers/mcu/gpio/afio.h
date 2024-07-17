/*
 * lld_afio.h
 *
 * Created on: Jan 30, 2022
 * Author: Stephane Amans
 */
#ifndef AFIO_H_
#define AFIO_H_

/* Include files:        */
#include <stdint.h>

#include "regbase_afio.h"

/* EXTI initialization structure definition :        */
struct t_afio_config
{
    uintptr_t base_address_afio;
};

#endif /* AFIO_H_ */
