///////////////////////////////////////////////////////////////////////
// Copyright Keith Wakeham 2018, so fuck off
///////////////////////////////////////////////////////////////////////

//#define SPI_MASTER_0_ENABLE /**< Enable SPI_MASTER_0 */
#include "ADS1120.h"

#include "nrf_delay.h"
#include "nrf_drv_uart.h"
#include "nrf_drv_spi.h"
#include "app_uart.h"

static uint8_t m_tx_buf[TX_RX_MSG_LENGTH]; /*!< SPI TX buffer */
static uint8_t m_rx_buf[TX_RX_MSG_LENGTH]; /*!< SPI RX buffer */
uint16_t        buf_len = TX_RX_MSG_LENGTH; /*!< SPI Buffer Length defined in the boards */

uint32_t *spi_base_address;
//uint32_t *spi_base_address2;

bool transmission_completed = 1;

bool gagesetup = 0;

int inputselect = 0;
int SCR1 = 0;
int	REF1 = 0;

#define SPI_INSTANCE  1 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);

void ADS_Start(void)
{
		nrf_delay_ms(5);
		//nrf_gpio_cfg_input(ADS_DRDY, NRF_GPIO_PIN_PULLDOWN);
		NRF_GPIO->PIN_CNF[ADS_DRDY] =    (GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos)    |
                                    (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)  |
                                    (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)   |
                                    (GPIO_PIN_CNF_DRIVE_S0S1    << GPIO_PIN_CNF_DRIVE_Pos)  |
                                    (GPIO_PIN_CNF_SENSE_High     << GPIO_PIN_CNF_SENSE_Pos);  

}


void ADS_Sleep(void) // soft sleepy time. This should shut down the ref, conversion electronics and more, should be fine for te ads110
{
		m_tx_buf[0] = 0x03;	// Command 02h or 03h enters sleep mode
		nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, 1);
		//spi_master_tx_rx(spi_base_address, 1, tx_data, m_rx_buf);
}

void ADS_Sync(void)
{
		m_tx_buf[0] = 0x08;	// Command 08h or 09h restarts the conversion process and syncronizes at that command, updated 1120
		nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, 1);
}

void ADS_soft_reset(void)
{
		m_tx_buf[0] = 0x07;	// Command 06h or 07h does a soft reset, restores the registers to the respective power-up values, updated 1120
		nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, 1);
}
void ADS_NOP(void) //unsure if valide for the ads1120
	{
		m_tx_buf[0] = 0xFF;	// Send one "No Operation".... NOP NOP NOP... Wokka wokka wokka
		nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, 1);
}

/**
 * @brief Function for wriring registers to the ADC, can only write one register at a time however
 */
void ADS_Write_register(int ADS_register, int ADS_Payload)
{
		m_tx_buf[0] = 0x40|(ADS_register << 2);	// this is 0100 RRNN where RR = register number and nn = nmber of bytes - 1
		m_tx_buf[1] = ADS_Payload;	// 
		nrf_drv_spi_transfer(&spi, m_tx_buf, 2, m_rx_buf, 2);	
}

int ADS_Read_register(int ADS_register) //updated for 
{
		m_tx_buf[0] = 0x20|(ADS_register << 2);	// // this is 0100 RRNN where RR = register number and nn = nmber of bytes - 1
		m_tx_buf[1] = 0xFF;	// Send a NOP
		nrf_drv_spi_transfer(&spi, m_tx_buf, 2, m_rx_buf, 2);
		return m_rx_buf[1];
}

void ADS_inputselect(int MUX, int PGA)
{
	// 0000 : AINP = AIN0, AINN = AIN1 (default)
	// 0001 : AINP = AIN0, AINN = AIN2
	// 0010 : AINP = AIN0, AINN = AIN3
	// 0011 : AINP = AIN1, AINN = AIN2
	// 0100 : AINP = AIN1, AINN = AIN3
	// 0101 : AINP = AIN2, AINN = AIN3
	// 0110 : AINP = AIN1, AINN = AIN0
	// 0111 : AINP = AIN3, AINN = AIN2
	// 1000 : AINP = AIN0, AINN = AVSS
	// 1001 : AINP = AIN1, AINN = AVSS
	// 1010 : AINP = AIN2, AINN = AVSS
	// 1011 : AINP = AIN3, AINN = AVSS
	// 1100 : (V(REFPx) – V(REFNx)) / 4 monitor (PGA bypassed)
	// 1101 : (AVDD – AVSS) / 4 monitor (PGA bypassed)
	// 1110 : AINP and AINN shorted to (AVDD + AVSS) / 2
	// 1111 : Reserved

	switch (PGA) // Programable gain, calculated as 2^n, where n is bits 6:4 in the System Control Register 0
	{
		case 1:		PGA = 0;	break;
		case 2:		PGA = 1;	break;
		case 4:		PGA = 2;	break;
		case 8:		PGA = 3;	break;
		case 16:	PGA = 4;	break;
		case 32:	PGA = 5;	break;
		case 64:	PGA = 6;	break;
		case 128:	PGA = 7;	break;
		default:	PGA = 0;  break;
	}
	inputselect = MUX << 4 | PGA << 1 | 0x00; //store the settings for the future in case it needs to resend them
//	app_uart_put(0x10);
//	app_uart_put(inputselect);
	ADS_Write_register(0x00, inputselect); // write register 0 with the combination of the two imputs
}

void ADS_SCR1 (int DOR, int omode, int cmode)
{
	switch (DOR) // Programable Data Rate, essentially a LUT that is psuedo 2^n*10 for gains of 10 to 64
    {
        case 20:		DOR = 0;	break;
        case 45:		DOR = 1;	break;
		case 90:		DOR = 2;	break;
		case 175:		DOR = 3;	break;
		case 330:		DOR = 4;	break;
		case 600:		DOR = 5;	break;
		case 1000:		DOR = 6;	break;
        default:		DOR = 0;  	break;
     }
	//omode
	//00 : Normal mode (256-kHz modulator clock, default)
	// 01 : Duty-cycle mode (internal duty cycle of 1:4)
	// 10 : Turbo mode (512-kHz modulator clock)
	// 11 : Reserved
	//cmode
	// 	0 : Single-shot mode (default)
	// 1 : Continuous conversion mode
	SCR1 = DOR << 5 | omode << 3 | cmode << 2 | 0x00; //store the settings for the future in case it needs to resend them
	ADS_Write_register(0x01, SCR1);
	//simple_uart_put(ADS_Read_register(0x03));
}
void ADS_REF (int VREFCON, int FILT, int PSW, int IDAC)
{
	//VREFCON bits 7:6
	//00 : Internal 2.048-V reference selected (default)
	//01 : External reference selected using dedicated REFP0 and REFN0 inputs
	//10 : External reference selected using AIN0/REFP1 and AIN3/REFN1 inputs
	//11 : Analog supply (AVDD – AVSS) used as reference
	//FILT 5:4
	//These bits select the 50 or 60hz filter but only for the 20sps or the 5sps setting
	//00 : No 50-Hz or 60-Hz rejection (default)
	//01 : Simultaneous 50-Hz and 60-Hz rejection
	//10 : 50-Hz rejection only
	//11 : 60-Hz rejection only
	//PSW 3
	//These bits are used to select the low side power switch. 0 open (no powers), 1 closed, but automatically opens in powerdown
	//IDAC
	//
	//000 : Off (default)
	// 001 : Reserved
	// 010 : 50 µA
	// 011 : 100 µA
	// 100 : 250 µA
	// 101 : 500 µA
	// 110 : 1000 µA
	// 111 : 1500 µA
	REF1 = VREFCON << 6 | FILT << 4 | PSW < 3 | IDAC;
	ADS_Write_register(0x02, REF1);
	//simple_uart_put(MUX1);
}

void ADS_IDAC (int IDAC1, int IDAC2, int DRDY)
{
	//IDAC1/2 routing bits 7:5, and 4:2
	// IDAC1/2 routing configuration
	// These bits select the channel where IDAC1 is routed to.
	// 000 : IDAC1 disabled (default)
	// 001 : IDAC1 connected to AIN0/REFP1
	// 010 : IDAC1 connected to AIN1
	// 011 : IDAC1 connected to AIN2
	// 100 : IDAC1 connected to AIN3/REFN1
	// 101 : IDAC1 connected to REFP0
	// 110 : IDAC1 connected to REFN0
	// 111 : Reserved
	//DRDY should 0 for dedicated, adn 1 for shared to dout and drdy
	REF1 = IDAC1 << 5 | IDAC2 << 2 | DRDY << 1 | 0x00;
	ADS_Write_register(0x03, REF1);
}

// void ADS_Temp()
// {
// 	ADS_MUX1(1,2,3); // change the reference to the internal reference
// 	while (true) //Check to see if DRDY asserted low
// 	{
// 		if(nrf_gpio_pin_read(ADS_DRDY) == 0)
// 		{
// 			m_tx_buf[0] = 0xFF;	// Write register 0
// 			m_tx_buf[1] = 0xFF;	// 1 Byte
// 			m_tx_buf[2] = 0xFF;	// Channel AIN 0 (000)and AIN 3 (011) Set next Channel to Shear Channel 0x03
// 			nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
// 			break;
// 		}
// 	}
// }

// void ADS_REF1()
// {
// 	nrf_gpio_pin_write(ADS_START, 0);
// 	ADS_MUX1(1,2,4);	// change the reference to the internal reference
// 	nrf_gpio_pin_write(ADS_START, 1);
// 	while (true) //Check to see if DRDY asserted low
// 	{
// 		if(nrf_gpio_pin_read(ADS_DRDY) == 0)
// 		{
// 			m_tx_buf[0] = 0xFF;	
// 			m_tx_buf[1] = 0xFF;	
// 			m_tx_buf[2] = 0xFF;	
// 			nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
// 			break;
// 		}
// 	}
// }

// void ADS_REF0()
// {
// 	nrf_gpio_pin_write(ADS_START, 0);
// 	ADS_MUX1(1,2,5);	// change the reference to the internal reference
// 	nrf_gpio_pin_write(ADS_START, 1);
// 	while (true) //Check to see if DRDY asserted low
// 	{
// 		if(nrf_gpio_pin_read(ADS_DRDY) == 0)
// 		{
// 			m_tx_buf[0] = 0xFF;	
// 			m_tx_buf[1] = 0xFF;	
// 			m_tx_buf[2] = 0xFF;	
// 			nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
// 			break;
// 		}
// 	}
// }

// void ADS_AVDD()
// {
// 	nrf_gpio_pin_write(ADS_START, 0);
// 	ADS_MUX1(1,2,6);	// change the reference to the internal reference
// 	nrf_gpio_pin_write(ADS_START, 1);
// 	while (true) //Check to see if DRDY asserted low
// 	{
// 		if(nrf_gpio_pin_read(ADS_DRDY) == 0)
// 		{
// 			m_tx_buf[0] = 0xFF;	
// 			m_tx_buf[1] = 0xFF;	
// 			m_tx_buf[2] = 0xFF;
// 			nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
// 			break;
// 		}
// 	}
// }

// void ADS_DVDD()
// {
// 	nrf_gpio_pin_write(ADS_START, 0);
// 	ADS_MUX1(1,2,7);	// change the reference to the internal reference
// 	nrf_gpio_pin_write(ADS_START, 1);
// 	while (true) //Check to see if DRDY asserted low
// 	{
// 		if(nrf_gpio_pin_read(ADS_DRDY) == 0)
// 		{
// 			m_tx_buf[0] = 0xFF;	
// 			m_tx_buf[1] = 0xFF;	
// 			m_tx_buf[2] = 0xFF;	
// 			nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
// 			break;
// 		}
// 	}
// }
	
void ADS_init(void)
{
		// m_tx_buf[0] = 0xFF;	//No Operation
		// m_tx_buf[1] = 0xFF;	// No Operation. Using these to flush the data
		// m_tx_buf[2] = 0x43; //0x4x means write, the second nibble is register, write register 3
		// m_tx_buf[3] = 0x00; // Write n-1 registers so write 1 register 
		// m_tx_buf[4] = 0x60; // Bit 7 = 0, Bit 6:4 = PGA from 1 (000) to 128 (111), Bits 3:0 = SPS from 5 (0000) to 2000 (1001+)
		// m_tx_buf[5] = 0x23;
		// m_tx_buf[6] = 0x00;
		// m_tx_buf[7] = 0xFF;
		// m_tx_buf[8] = 0x42; //write register 2
		// m_tx_buf[9] = 0x00; // Write 1 register
		// m_tx_buf[10] = 0x20; // Bit 7 = CLSTAT (0 internal 1 external), Bit 6:5 = VREFCON1:0 (00 internal ref off, 01 always on, 10 or 11 on when converting, Bits 4:3 REFSELT1:0 (00 = Ref0, 01 - Ref1, 10 - onboard ref, 11 -- onboard ref interallly connected)
		// m_tx_buf[11] = 0x22; //write register 2
		// m_tx_buf[12] = 0x00; // Write 1 register
		// m_tx_buf[13] = 0xFF;
		// buf_len = 14;
		// nrf_drv_spi_transfer(&spi, m_tx_buf, 14, m_rx_buf, 14);
}

void ADS_init2(void) //does the same as init but more concisely
{
	// ADS_inputselect(0,1);
	// ADS_SCR0(128, 160); // Slow for testing
	//ADS_SCR0(64, 640);
	// ADS_MUX1(1,3,0);

	ADS_inputselect(0,128);
//	app_uart_put(0x20);
//	app_uart_put(ADS_Read_register(0x00));
	ADS_SCR1(330,0,1);
	ADS_REF(1,0,0,5);
	ADS_IDAC(5,5,0);

}
void ADS_RestoreReg() //restores the previous registers for measuring temp, references, and voltages
{
	// ADS_Write_register(0x00, inputselect);
	// ADS_Write_register(0x02, MUX1);
	// ADS_Write_register(0x03, SCR0);
	
	// if (ADS_Read_register(0x00) == inputselect)
	// {
	// 	//simple_uart_put(0xA1);
	// }
	// 	if (ADS_Read_register(0x02) == MUX1)
	// {
	// 	//simple_uart_put(0xA2);
	// }
	// 	if (ADS_Read_register(0x03) == SCR0)
	// {
	// 	//simple_uart_put(0xA3);
	// }
}
uint32_t ADS_EVT_Read_CFF(void)
{
		m_tx_buf[0] = 0xFF;	// NOP
		m_tx_buf[1] = 0xFF;	// NOP
		m_tx_buf[2] = 0xFF;	// NOP
		nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
		return (0x00 << 24 | m_rx_buf[0] << 16 | m_rx_buf[1] << 8 | m_rx_buf[2]);
}

int16_t ADS_EVT_Read(void)
{
	
	while (true) //Check to see if DRDY asserted low
		{
			if(nrf_gpio_pin_read(ADS_DRDY) == 0)
			{
				m_tx_buf[0] = 0xFF;	// NOP
				m_tx_buf[1] = 0xFF;	// NOP
				m_tx_buf[2] = 0xFF;	// NOP
				nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
				break;
			}
		}
		return (m_rx_buf[0] << 8 | m_rx_buf[1]);
}

int16_t ADS_EVT_Read_INSEL(int MUX, int PGA)
{
		ADS_inputselect(MUX,PGA);
	
		while (true) //Check to see if DRDY asserted low
		{
			if(nrf_gpio_pin_read(ADS_DRDY) == 0)
			{
				m_tx_buf[0] = 0x00;	//
				m_tx_buf[1] = 0x00;	//
				nrf_drv_spi_transfer(&spi, m_tx_buf, 2, m_rx_buf, 2);
				break;
			}
		}
		return (m_rx_buf[0] << 8 | m_rx_buf[1]);
}
	

void ADS_test_read(void)
{	
		//nrf_delay_ms(200);
		while (true) //Check to see if DRDY asserted low
		{
			if(nrf_gpio_pin_read(ADS_DRDY) == 0)
			{
				m_tx_buf[0] = 0xFF;	// NOP
				m_tx_buf[1] = 0xFF;	// NOP
				m_tx_buf[2] = 0xFF;	// NOP
				nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
				app_uart_put(m_rx_buf[0]);
				app_uart_put(m_rx_buf[1]);
				app_uart_put(m_rx_buf[2]);
				break;
			}
		}
		return;
}


void ADS_test_read100(void)
{	
	
	bool Left = 1;
		for ( int x = 0; x < 96; x++ )
		{
			while (true) //Check to see if DRDY asserted low
				{
					if(nrf_gpio_pin_read(ADS_DRDY) == 0)
					{
						m_tx_buf[0] = 0x40;	// Write register 0
						m_tx_buf[1] = 0x02;	// 1 Byte
						if (Left ==1)
						{
							m_tx_buf[2] = 0x01;	// 
							m_tx_buf[4] = 0x20; //
							Left = 0;
						} else
						{
							m_tx_buf[2] = 0x25;	//
							m_tx_buf[4] = 0x28; //
							Left = 1;
						}
						m_tx_buf[3] = 0x00;	//
						nrf_drv_spi_transfer(&spi, m_tx_buf, 5, m_rx_buf, 5);
						break;
					}
				}
		}
		return;
}

void ADS_test_read100B(void)
{	
	// ADS_MUX1(1,1,0);
	// bool Left = 0;
	// 	for ( int x = 0; x < 96; x++ )
	// 	{
	// 		while (true) //Check to see if DRDY asserted low
	// 			{
	// 				if(nrf_gpio_pin_read(ADS_DRDY) == 0)
	// 				{
	// 					m_tx_buf[0] = 0x40;	// Write register 0
	// 					m_tx_buf[1] = 0x00;	// 1 Byte
	// 					if (Left ==1)
	// 					{
	// 						m_tx_buf[2] = 0x01;	// Channel AIN 0 (000)and AIN 3 (011) Set next Channel to Shear Channel 0x03
	// 					} else
	// 					{
	// 						m_tx_buf[2] = 0x25;	// Channel AIN 0 (000)and AIN 3 (011) Set next Channel to Shear Channel 0x03
	// 					}
	// 					m_tx_buf[3] = 0x00;	//
	// 					nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
	// 					break;
	// 				}
	// 			}
	// 	}
	// 	return;
}

void ADS_test_read100A(void)
{	
	// ADS_MUX1(1,0,0);
	bool Left = 1;
		for ( int x = 0; x < 96; x++ )
		{
			while (true) //Check to see if DRDY asserted low
				{
					if(nrf_gpio_pin_read(ADS_DRDY) == 0)
					{
						m_tx_buf[0] = 0x40;	// Write register 0
						m_tx_buf[1] = 0x00;	// 1 Byte
						if (Left ==1)
						{
							m_tx_buf[2] = 0x01;	// Channel AIN 0 (000)and AIN 3 (011) Set next Channel to Shear Channel 0x03
						} else
						{
							m_tx_buf[2] = 0x25;	// Channel AIN 0 (000)and AIN 3 (011) Set next Channel to Shear Channel 0x03
						}
						m_tx_buf[3] = 0x00;	//
						nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
						break;
					}
				}
		}
		return;
}


