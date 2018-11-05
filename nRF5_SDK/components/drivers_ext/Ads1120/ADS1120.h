///////////////////////////////////////////////////////////////////////
// Copyright Keith Wakeham 2018, so fuck off
///////////////////////////////////////////////////////////////////////

#include "nrf_gpio.h"
#include "bsp.h"

#ifndef ADC_ADS_1120
#define ADC_ADS_1120

//Pins Temp
// #define ADS_DOUT 24
// #define ADS_DIN 23
// #define ADS_CLK 25
// #define ADS_CS 4
// #define ADS_DRDY 28


// #define ADS_START 3
// #define ADS_RESET 20


#define TX_RX_MSG_LENGTH   100

/** @def ERROR_PIN_SPI0
 * This pin is set active high when there is an error either in TX/RX for SPI0 or if the received bytes does not totally match the transmitted bytes.
 * This functionality can be tested by temporarily disconnecting the MISO pin while running this example.
 */
//#define ERROR_PIN_SPI0   8UL

/** @def ERROR_PIN_SPI1
 * This pin is set active high when there is an error either in TX/RX for SPI1 or if the received bytes does not totally match the transmitted bytes.
 * This functionality can be tested by temporarily disconnecting the MISO pin while running this example.
 */
//#define ERROR_PIN_SPI1   9UL

//void spi_master_event_handler(spi_master_evt_t spi_master_evt);

//void spi_master_init(void);

void ADS_Start(void);

void Accel_test(void);

void ADS_Sleep(void);

void ADS_Sync(void);

void ADS_soft_reset(void);

void ADS_NOP(void);

void ADS_Write_register(int ADS_register, int ADS_Payload);

int ADS_Read_register(int ADS_register);

void ADS_inputselect(int MUX, int PGA);

void ADS_SCR1 (int DOR, int omode, int cmode);

void ADS_REF (int VREFCON, int FILT, int PSW, int IDAC);

void ADS_IDAC (int IDAC1, int IDAC2, int DRDY);
// void ADS_Temp(void);

// void ADS_REF1(void);

// void ADS_REF0(void);

// void ADS_AVDD(void);

// void ADS_DVDD(void);

void ADS_init(void);

void ADS_init2(void);

void ADS_RestoreReg(void);

uint32_t ADS_EVT_Read_CFF(void);

int16_t ADS_EVT_Read(void);
//void ADS_Register_Readback(void);

int16_t ADS_EVT_Read_INSEL(int MUX_SP, int MUX_SN);

//These read inside the loop in a wait configuration, untested with interrupts, probably an issue
void ADS_test_read(void);
void ADS_test_read100(void);
void ADS_test_read100A(void);
void ADS_test_read100B(void);

#endif
