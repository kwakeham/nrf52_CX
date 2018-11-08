/**
 * Copyright (c) 2014 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef CX_H
#define CX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LEDs definitions for CX 1.0

#define DKTEST

#ifndef DKTEST // REAL

#define M_PHASE     2
#define M_NSLEEP    3
#define M_EN        4
#define M_MODE      17
#define M_MODE2     5
#define M_NFAULT    15
#define M_ISENSE    6

#define S_HALL_EN   19
#define S_SIN       4
#define S_COS       5

#define B_VBATT_DIV 7

#define A_INT1      6
#define A_INT2      7
#define A_CS        8
#define A_MOSI      9
#define A_MISO      10
#define A_CLK       11

#define RX_PIN_NUMBER  16 //Pin  on the BL652 Module// 6 and 8 for testing on devkit
#define TX_PIN_NUMBER  20 //Pin  on the BL652 Module//
#define CTS_PIN_NUMBER 12 //Unused
#define RTS_PIN_NUMBER 13 //Unused
#define UART_HWFC           false

#else //DUMMY outputs for if DKTEST enabled

#define M_PHASE     19 //LED3
#define M_NSLEEP    3
#define M_EN        18 //LED2
#define M_MODE      17
#define M_MODE2     5
#define M_NFAULT    15
#define M_ISENSE    6

#define S_HALL_EN   19
#define S_SIN       4
#define S_COS       5

#define B_VBATT_DIV 7

#define A_INT1      6
#define A_INT2      7
#define A_CS        8
#define A_MOSI      9
#define A_MISO      10
#define A_CLK       11

#define RX_PIN_NUMBER  8
#define TX_PIN_NUMBER  6
#define CTS_PIN_NUMBER 12 //Unused
#define RTS_PIN_NUMBER 13 //Unused
#define UART_HWFC           false

#endif // Endif for DKTEST

#ifdef __cplusplus
}
#endif

#endif // CX_H
