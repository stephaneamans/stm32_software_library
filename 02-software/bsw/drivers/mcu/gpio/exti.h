/*
 * lld_exti.h
 *
 * Created on: Jan 29, 2022
 * Author: Stephane Amans
 */
#ifndef EXTI_H_
#define EXTI_H_

/* Include files:        */
#include <stdint.h>

#include "regbase_exti.h"

/* EXTI initialization structure definition :        */
struct t_exti_config
{
    uintptr_t base_address_exti;
};

#endif /* EXTI_H_ */
