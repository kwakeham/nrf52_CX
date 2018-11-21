/**
 * Copyright (c) 2018 Keith Wakeham
 *
 * All rights reserved.
 *
 *
 */

#include "cx.h"
#include "mpos.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define SAMPLES_IN_BUFFER 1
static nrf_saadc_value_t     m_buffer_pool;
// static nrf_saadc_Valu
// static uint32_t              m_adc_evt_counter;
// nrf_saadc_value_t sample_sample;

void saadc_callback(nrfx_saadc_evt_t const * p_event)
{
    // ret_code_t err_code;
    // printf("event\r\n");
    if (p_event->type == NRFX_SAADC_EVT_DONE)                                                        //Capture offset calibration complete event
    {
        // printf("evvent done");
        ret_code_t err_code;
        err_code = nrfx_saadc_buffer_convert(p_event->data.done.p_buffer, 1);
        APP_ERROR_CHECK(err_code);
        // printf("%d\r\n", p_event->data.done.p_buffer[0]);
        // int i;
        // printf("ADC event number: %d", (int)m_adc_evt_counter);

        // for (i = 0; i < SAMPLES_IN_BUFFER; i++)
        // {
        //     printf("%d", p_event->data.done.p_buffer[i]);
        // }
        // m_adc_evt_counter++;
			
//         LEDS_INVERT(BSP_LED_1_MASK);                                                                    //Toggle LED2 to indicate SAADC buffer full		

//         if((m_adc_evt_counter % SAADC_CALIBRATION_INTERVAL) == 0)                                  //Evaluate if offset calibration should be performed. Configure the SAADC_CALIBRATION_INTERVAL constant to change the calibration frequency
//         {
//             nrf_drv_saadc_abort();                                                                      // Abort all ongoing conversions. Calibration cannot be run if SAADC is busy
//             m_saadc_calibrate = true;                                                                   // Set flag to trigger calibration in main context when SAADC is stopped
//         }
        

// #ifdef UART_PRINTING_ENABLED
//         NRF_LOG_INFO("ADC event number: %d\r\n",(int)m_adc_evt_counter);                                //Print the event number on UART

//         for (int i = 0; i < p_event->data.done.size; i++)
//         {
//             NRF_LOG_INFO("%d\r\n", p_event->data.done.p_buffer[i]);                                     //Print the SAADC result on UART
//         }
// #endif //UART_PRINTING_ENABLED      
        
//         if(m_saadc_calibrate == false)
//         {
//             err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAADC_SAMPLES_IN_BUFFER);             //Set buffer so the SAADC can write to it again. 
//             APP_ERROR_CHECK(err_code);
//         }
        
//         m_adc_evt_counter++;
  
    }
    else if (p_event->type == NRFX_SAADC_EVT_CALIBRATEDONE)
    {
//         LEDS_INVERT(BSP_LED_2_MASK);                                                                    //Toggle LED3 to indicate SAADC calibration complete
        
//         err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAADC_SAMPLES_IN_BUFFER);             //Set buffer so the SAADC can write to it again. 
//         APP_ERROR_CHECK(err_code);
//         err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAADC_SAMPLES_IN_BUFFER);             //Need to setup both buffers, as they were both removed with the call to nrf_drv_saadc_abort before calibration.
//         APP_ERROR_CHECK(err_code);
        
// #ifdef UART_PRINTING_ENABLED
//         NRF_LOG_INFO("SAADC calibration complete ! \r\n");                                              //Print on UART
// #endif //UART_PRINTING_ENABLED	
        
    }
}


void mpos_init(void)
{
    NRF_LOG_INFO("MPOS init");
    ret_code_t err_code;
    nrfx_saadc_config_t saadc_config;
    saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT; //need to manually set the resolution or else it'll default to 8 bit
    saadc_config.oversample = NRF_SAADC_OVERSAMPLE_DISABLED; // default is 4 sample over sampling so need to override that.

    err_code = nrfx_saadc_init(&saadc_config, saadc_callback);
    APP_ERROR_CHECK(err_code);

    nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN1);
    channel_config.gain = NRF_SAADC_GAIN1_5; // this is measured against either vdd/4 or vcore = 0.6v.

    nrfx_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_saadc_buffer_convert(&m_buffer_pool, 1);
    APP_ERROR_CHECK(err_code);

    // err_code = nrfx_saadc_buffer_convert(m_buffer_pool[1], 1);
    // APP_ERROR_CHECK(err_code);
    
}

int16_t mpos_test_convert(void)
{
    nrf_saadc_value_t duckman;
    ret_code_t err_code;
    err_code = nrfx_saadc_sample_convert(0, &duckman);
    
    if (err_code == NRFX_SUCCESS)
    {
        return(duckman);
    } else
    {
        return (-8008);
    }
    return (-8007);
}

void mpos_test_convert_event_activate(void)
{
    ret_code_t err_code;
    // printf("inside event activate\r\n");
    // err_code = nrfx_saadc_buffer_convert(m_buffer_pool[0], 1);
    NRF_LOG_INFO("setup sample");
    NRF_LOG_FLUSH();
    err_code = nrfx_saadc_sample();
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("setup sample done");
    NRF_LOG_FLUSH();
    
    if (err_code == NRFX_SUCCESS)
    {
        NRF_LOG_INFO("successfully setup the sampling\r\n");
    } 
    else if (err_code == NRFX_ERROR_INVALID_STATE)
    {
        NRF_LOG_ERROR("fuck sake \r\n");
    }
    
    
}

float angle(int16_t hall_0, int16_t  hall_1)
{
    return(3.2);
}

void display_value(void)
{
    NRF_LOG_INFO("%d", m_buffer_pool);
    NRF_LOG_FLUSH();
}