/**
 * Copyright (c) 2015 - 2018, Nordic Semiconductor ASA
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
/** @file
 * @defgroup pwm_example_main main.c
 * @{
 * @ingroup pwm_example
 *
 * @brief PWM Example Application main file.
 *
 * This file contains the source code for a sample application using PWM.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
// #include <inttypes.h>
#include <math.h>
#include "app_uart.h"
#include "app_error.h"
#include "cx.h"
#include "nrf_uart.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "drv8801.h"
#include "mpos.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}

int main(void)
{
    uint32_t err_code;
    
//     const app_uart_comm_params_t comm_params =
//     {
//         RX_PIN_NUMBER,
//         TX_PIN_NUMBER,
//         RTS_PIN_NUMBER,
//         CTS_PIN_NUMBER,
//         UART_HWFC,
//         false,
// #if defined (UART_PRESENT)
//         NRF_UART_BAUDRATE_115200
// #else
//         NRF_UARTE_BAUDRATE_115200
// #endif
//       };

    // APP_UART_FIFO_INIT(&comm_params,
    //                     UART_RX_BUF_SIZE,
    //                     UART_TX_BUF_SIZE,
    //                     uart_error_handle,
    //                     APP_IRQ_PRIORITY_LOWEST,
    //                     err_code);

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
    APP_ERROR_CHECK(err_code);

    // ret_code_t ret_code = nrf_pwr_mgmt_init();
    // APP_ERROR_CHECK(ret_code);

    printf("\r\nHellow.\r\n");
    // printf("Initialize the drv8801\r\n");
    drv8801_init();
    // printf("initialized the drv8801, sending sequence\r\n");
    drv8801_test();
    // printf("wait5\r\n");
    // nrf_delay_ms(5000);
    // printf("test2\r\n");
    // drv8801_test2();
    // printf("done\r\n");
    // nrf_delay_ms(8000);
    // printf("Drivetest!!!\r\n");
    // drv8801_drive(800);
    // nrf_delay_ms(8000);
    // printf("Drivetest!!!\r\n");
    // drv8801_drive(-200);
    // printf("Drivetest!!!\r\n");
    // nrf_delay_ms(1000);
    // drv8801_drive(-800);
    // nrf_delay_ms(1000);
    // drv8801_drive(300);
    // printf("Initializing Motorposition\r\n");
    NRF_LOG_INFO("SAADC init");
    mpos_init();
    NRF_LOG_INFO("mpos inited\r\n");
    NRF_LOG_FLUSH();
    // printf("the value is:");
    // printf ("%" PRId16, mpos_test_convert());
    // printf("\r\n");
    // printf("%f\r\n", dave);
    // nrf_delay_ms(1000);
    // printf("delay over\r\n");
    nrf_delay_ms(40);
    mpos_test_convert_event_activate();
    NRF_LOG_INFO("hotdog");
    NRF_LOG_FLUSH();
    display_value();
    // nrf_delay_ms(100);
    // mpos_test_convert_event_activate();
    NRF_LOG_INFO("hotdog\r\n");
    // nrf_delay_ms(10000);
    NRF_LOG_FLUSH();
    // int16_t sample_data;
    // while(1)
    // {
    //     // printf("the value is:");
    //     sample_data = mpos_test_convert();
    //     printf ("%" PRId16, sample_data);
    //     printf("\r\n");
    //     nrf_delay_ms(20);
    // }
    // double temp_angle  = 0;
    // int temp_count = 0;
    for (;;)
    {
        // nrf_pwr_mgmt_run();
        mpos_test_convert_event_activate();
        nrf_delay_ms(100);
        display_value();
        // for(a = 0; a < 20; a++)
        // {
        //     if (a == 0)
        //     {
        //         display_value();
        //     }
        // }
        // NRF_LOG_FLUSH();
        // Wait for an event.
        // __WFE();


        // Clear the event register.
        // __SEV();
        // __WFE();

        // NRF_LOG_FLUSH();
    }
}


/** @} */
