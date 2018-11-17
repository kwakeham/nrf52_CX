/**
 * Copyright (c) 2018 Keith Wakeham
 *
 * All rights reserved.
 *
 *
 */

#ifndef MPOS_H
#define MPOS_H

#include <stdint.h>
#include "nrfx_saadc.h"
#include "app_error.h"

#ifdef __cplusplus
extern "C" {
#endif

void mpos_init(void);

int16_t mpos_test_convert(void);

#ifdef __cplusplus
}
#endif

#endif // MPOS_H