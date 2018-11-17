/**
 * Copyright (c) 2018 Keith Wakeham
 *
 * All rights reserved.
 *
 *
 */

#include "cx.h"
#include "mpos.h"
// #include "nrfx_saadc.h"

void mpos_init(void)
{
    ret_code_t err_code;
    err_code = nrfx_saadc_init(NULL, NULL);
    APP_ERROR_CHECK(err_code);
    nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN1);
    channel_config.gain = NRF_SAADC_GAIN1;
    nrfx_saadc_channel_init(0, &channel_config);

}

int16_t mpos_test_convert(void)
{
    nrf_saadc_value_t duckman;
    nrfx_saadc_sample_convert(0, &duckman);
    return (duckman);
}