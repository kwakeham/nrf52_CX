//#define SPI_MASTER_0_ENABLE /**< Enable SPI_MASTER_0 */
#include "ADS.h"

#include "nrf_delay.h"
#include "nrf_drv_uart.h"
#include "nrf_drv_spi.h"
#include "app_uart.h"
#include "boards.h"

static uint8_t m_tx_buf[TX_RX_MSG_LENGTH]; /*!< SPI TX buffer */
static uint8_t m_rx_buf[TX_RX_MSG_LENGTH]; /*!< SPI RX buffer */
uint16_t        buf_len = TX_RX_MSG_LENGTH; /*!< SPI Buffer Length defined in the boards */

uint32_t *spi_base_address;
//uint32_t *spi_base_address2;

bool transmission_completed = 1;

bool gagesetup = 0;

int inputselect = 0;
int SCR0 = 0;
int	MUX1 = 0;

#define SPI_INSTANCE  1 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);

void ADS_Start(void)
{
		nrf_gpio_pin_dir_set(ADS_RESET, NRF_GPIO_PIN_DIR_OUTPUT);
		nrf_gpio_pin_write(ADS_RESET, 1);
		nrf_delay_ms(5);
//		nrf_gpio_pin_write(ADS_RESET, 0);
		nrf_gpio_pin_dir_set(ADS_START, NRF_GPIO_PIN_DIR_OUTPUT);
		nrf_gpio_pin_write(ADS_START, 1);
	
//		nrf_gpio_pin_dir_set(ADS_CS, NRF_GPIO_PIN_DIR_OUTPUT);
//		nrf_gpio_pin_write(ADS_CS, 1);
	
		nrf_delay_ms(5);
		//spi_base_address = spi_master_init(SPI0, SPI_MODE1, (bool)0);

//		nrf_delay_ms(5);
		//nrf_gpio_cfg_input(ADS_DRDY, NRF_GPIO_PIN_PULLDOWN);
		NRF_GPIO->PIN_CNF[ADS_DRDY] =    (GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos)    |
                                    (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)  |
                                    (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)   |
                                    (GPIO_PIN_CNF_DRIVE_S0S1    << GPIO_PIN_CNF_DRIVE_Pos)  |
                                    (GPIO_PIN_CNF_SENSE_High     << GPIO_PIN_CNF_SENSE_Pos);  

}

void ADS_Reset(void) // This does a hard reset on the device
{
	
}

void ADS_Wakeup(void) // if it's in soft sleep, use this to wake it up
{
		m_tx_buf[0] = 0x01;	// Command 00h or 01h exits sleep mode
		nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, 1);
		//spi_master_tx_rx(spi_base_address, 1, tx_data, rx_data);
}

void ADS_Sleep(void) // soft sleepy time. This should shut down the ref, conversion electronics and more
{
		m_tx_buf[0] = 0x03;	// Command 02h or 03h enters sleep mode
		nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, 1);
		//spi_master_tx_rx(spi_base_address, 1, tx_data, m_rx_buf);
}

void ADS_Sync(void)
{
		m_tx_buf[0] = 0x05;	// Command 04h or 05h twice in a row restarts the conversion process and syncronizes at that command
		m_tx_buf[1] = 0x05;	//
		//spi_master_tx_rx(spi_base_address, 2, m_tx_buf, m_rx_buf);
		nrf_drv_spi_transfer(&spi, m_tx_buf, 2, m_rx_buf, 1);
}

void ADS_soft_reset(void)
{
		m_tx_buf[0] = 0xFF;	// Command 06h or 07h does a soft reset, restores the registers to the respective power-up values.
		nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, 1);
		//spi_master_tx_rx(spi_base_address, 1, m_tx_buf, m_rx_buf);
}
void ADS_NOP(void)
	{
		m_tx_buf[0] = 0xFF;	// Send one "No Operation".... NOP NOP NOP... Wokka wokka wokka
		nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, 1);
		//spi_master_tx_rx(spi_base_address, 1, m_tx_buf, m_rx_buf);
}
	
void ADS_Write_register(int ADS_register, int ADS_Payload)
{
		m_tx_buf[0] = 0x40|ADS_register;	// 
		m_tx_buf[1] = 0x00;	//  n - 1 bytes
		m_tx_buf[2] = ADS_Payload;	// 
		nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);	
}

int ADS_Read_register(int ADS_register)
{
		m_tx_buf[0] = 0x20|ADS_register;	// Read register 0 - 15
		m_tx_buf[1] = 0x00;	// n - 1 bytes
		m_tx_buf[2] = 0xFF;	// Send a NOP
		nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
		return m_rx_buf[2];
}
void ADS_inputselect(int MUX_SP, int MUX_SN)
{
		// This does not activate burnout current, it leaves that as zero
		// For ADS1248 the valide inputs are 0 - 7, 47 => 0 - 3
		inputselect = MUX_SP<<3 | MUX_SN; //store the settings for the future in case it needs to resend them
		ADS_Write_register(0, inputselect); // write register 0 with the combination of the two imputs
		//simple_uart_put(ADS_Read_register(0));  // just some debug
}

void ADS_SCR0 (int PGA, int DOR)
{
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

		switch (DOR) // Programable Data Rate, essentially a LUT that is psuedo 2^n*10 for gains of 10 to 64
    {
        case 5:			DOR = 0;	break;
        case 10:		DOR = 1;	break;
        case 20:		DOR = 2;	break;
        case 40:		DOR = 3;	break;
				case 80:		DOR = 4;	break;
				case 160:		DOR = 5;	break;
				case 320:		DOR = 6;	break;
				case 640:		DOR = 7;	break;
				case 1000:	DOR = 8;	break;
				case 2000:	DOR = 9;	break; // this can also be 10 - 15 or A - F in hex
        default:	DOR = 0;  break;
     }
		SCR0 = PGA << 4 | DOR; //store the settings for the future in case it needs to resend them
		ADS_Write_register(0x03, SCR0);
		//simple_uart_put(ADS_Read_register(0x03));
}
void ADS_MUX1 (int VREFCON, int REFSELT, int MUXCAL)
{
	//VREFCON bits 6:5
	//00 = Internal reference is always off (default)
	//01 = Internal reference is always on
	//10 or 11 = Internal reference is on when a conversion is in progress and shuts down when the device receives a shutdown opcode or the START pin is taken low
	//REFSELT bits 4:3
	//These bits select the reference input for the ADC.
	//00 = REF0 input pair selected (default)
	//01 = REF1 input pair selected (ADS1248 only)
	//10 = Onboard reference selected
	//11 = Onboard reference selected and internally connected to REF0 input pairis in progress and shuts down when the device receives a shutdown opcode or the START pin is taken low
	//MUXCAL bits 2:0
	//These bits are used to select a system monitor. The MUXCAL selection supercedes selections from registers MUX0 and MUX1 (MUX_SP, MUX_SN, and VBIAS).
	//000 = Normal operation (default)
	//001 = Offset measurement
	//010 = Gain measurement
	//011 = Temperature diode
	//100 = External REF1 measurement (ADS1248 only)
	//101 = External REF0 measurement
	//110 = AVDD measurement
	//111 = DVDD measurement
	MUX1 = VREFCON << 5 | REFSELT << 3 | MUXCAL;
	ADS_Write_register(0x02, MUX1);
	//simple_uart_put(MUX1);
}



void ADS_Temp()
{
	nrf_gpio_pin_write(ADS_START, 0);
	ADS_MUX1(1,2,3); // change the reference to the internal reference
	nrf_gpio_pin_write(ADS_START, 1);	
	while (true) //Check to see if DRDY asserted low
	{
		if(nrf_gpio_pin_read(ADS_DRDY) == 0)
		{
			m_tx_buf[0] = 0xFF;	// Write register 0
			m_tx_buf[1] = 0xFF;	// 1 Byte
			m_tx_buf[2] = 0xFF;	// Channel AIN 0 (000)and AIN 3 (011) Set next Channel to Shear Channel 0x03
			nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
//			simple_uart_put(m_rx_buf[0]);
//			simple_uart_put(0x09); // tab
//			simple_uart_put(m_rx_buf[1]);
//			simple_uart_put(0x09); // tab
//			simple_uart_put(m_rx_buf[2]);
			break;
		}
	}
}

void ADS_REF1()
{
	nrf_gpio_pin_write(ADS_START, 0);
	ADS_MUX1(1,2,4);	// change the reference to the internal reference
	nrf_gpio_pin_write(ADS_START, 1);
	while (true) //Check to see if DRDY asserted low
	{
		if(nrf_gpio_pin_read(ADS_DRDY) == 0)
		{
			m_tx_buf[0] = 0xFF;	
			m_tx_buf[1] = 0xFF;	
			m_tx_buf[2] = 0xFF;	
			nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
			//spi_master_tx_rx(spi_base_address, 3, m_tx_buf, rx_data);
			
//			simple_uart_put(rx_data[0]);
//			simple_uart_put(rx_data[1]);
//			simple_uart_put(rx_data[2]);
			break;
		}
	}
}

void ADS_REF0()
{
	nrf_gpio_pin_write(ADS_START, 0);
	ADS_MUX1(1,2,5);	// change the reference to the internal reference
	nrf_gpio_pin_write(ADS_START, 1);
	while (true) //Check to see if DRDY asserted low
	{
		if(nrf_gpio_pin_read(ADS_DRDY) == 0)
		{
			m_tx_buf[0] = 0xFF;	
			m_tx_buf[1] = 0xFF;	
			m_tx_buf[2] = 0xFF;	
			nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
			//spi_master_tx_rx(spi_base_address, 3, m_tx_buf, rx_data);
//			simple_uart_put(rx_data[0]);
//			simple_uart_put(rx_data[1]);
//			simple_uart_put(rx_data[2]);
			break;
		}
	}
}

void ADS_AVDD()
{
	nrf_gpio_pin_write(ADS_START, 0);
	ADS_MUX1(1,2,6);	// change the reference to the internal reference
	nrf_gpio_pin_write(ADS_START, 1);
	while (true) //Check to see if DRDY asserted low
	{
		if(nrf_gpio_pin_read(ADS_DRDY) == 0)
		{
			m_tx_buf[0] = 0xFF;	
			m_tx_buf[1] = 0xFF;	
			m_tx_buf[2] = 0xFF;
			nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
			//spi_master_tx_rx(spi_base_address, 3, m_tx_buf, rx_data);
//			simple_uart_put(rx_data[0]);
//			simple_uart_put(rx_data[1]);
//			simple_uart_put(rx_data[2]);
			break;
		}
	}
}

void ADS_DVDD()
{
	nrf_gpio_pin_write(ADS_START, 0);
	ADS_MUX1(1,2,7);	// change the reference to the internal reference
	nrf_gpio_pin_write(ADS_START, 1);
	while (true) //Check to see if DRDY asserted low
	{
		if(nrf_gpio_pin_read(ADS_DRDY) == 0)
		{
			m_tx_buf[0] = 0xFF;	
			m_tx_buf[1] = 0xFF;	
			m_tx_buf[2] = 0xFF;	
			//spi_master_tx_rx(spi_base_address, 3, m_tx_buf, rx_data);
			nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
//			simple_uart_put(rx_data[0]);
//			simple_uart_put(rx_data[1]);
//			simple_uart_put(rx_data[2]);
			break;
		}
	}
}
	
void ADS_init(void)
{
		m_tx_buf[0] = 0xFF;	//No Operation
		m_tx_buf[1] = 0xFF;	// No Operation. Using these to flush the data
		m_tx_buf[2] = 0x43; //0x4x means write, the second nibble is register, write register 3
		m_tx_buf[3] = 0x00; // Write n-1 registers so write 1 register 
		m_tx_buf[4] = 0x60; // Bit 7 = 0, Bit 6:4 = PGA from 1 (000) to 128 (111), Bits 3:0 = SPS from 5 (0000) to 2000 (1001+)
		m_tx_buf[5] = 0x23;
		m_tx_buf[6] = 0x00;
		m_tx_buf[7] = 0xFF;
		m_tx_buf[8] = 0x42; //write register 2
		m_tx_buf[9] = 0x00; // Write 1 register
		m_tx_buf[10] = 0x20; // Bit 7 = CLSTAT (0 internal 1 external), Bit 6:5 = VREFCON1:0 (00 internal ref off, 01 always on, 10 or 11 on when converting, Bits 4:3 REFSELT1:0 (00 = Ref0, 01 - Ref1, 10 - onboard ref, 11 -- onboard ref interallly connected)
		m_tx_buf[11] = 0x22; //write register 2
		m_tx_buf[12] = 0x00; // Write 1 register
		m_tx_buf[13] = 0xFF;
		buf_len = 14;
		nrf_drv_spi_transfer(&spi, m_tx_buf, 14, m_rx_buf, 14);
		//spi_master_tx_rx(spi_base_address, 14, m_tx_buf, rx_data);
}

void ADS_init2(void) //does the same as init but more concisely
{
	ADS_inputselect(0,1);
	ADS_SCR0(128, 160); // Slow for testing
	//ADS_SCR0(64, 640);
	ADS_MUX1(1,3,0);
	
	//simple_uart_put(ADS_Read_register(0x03));
}
void ADS_RestoreReg() //restores the previous registers for measuring temp, references, and voltages
{
	ADS_Write_register(0x00, inputselect);
	ADS_Write_register(0x02, MUX1);
	ADS_Write_register(0x03, SCR0);
	
	if (ADS_Read_register(0x00) == inputselect)
	{
		//simple_uart_put(0xA1);
	}
	
		if (ADS_Read_register(0x02) == MUX1)
	{
		//simple_uart_put(0xA2);
	}
	
		if (ADS_Read_register(0x03) == SCR0)
	{
		//simple_uart_put(0xA3);
	}
}
uint32_t ADS_EVT_Read_CFF(void)
{
		m_tx_buf[0] = 0xFF;	// NOP
		m_tx_buf[1] = 0xFF;	// NOP
		m_tx_buf[2] = 0xFF;	// NOP
		nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
		//spi_master_tx_rx(spi_base_address, 3, m_tx_buf, rx_data);
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
				//spi_master_tx_rx(spi_base_address, 3, m_tx_buf, rx_data);
//				app_uart_put(m_rx_buf[0]);
//				app_uart_put(m_rx_buf[1]);
//				app_uart_put(m_rx_buf[2]);
				break;
			}
		}
		return (m_rx_buf[0] << 8 | m_rx_buf[1]);
//		int8_t gage = 0;
//		m_tx_buf[0] = 0x40;	// Write register 0
//		m_tx_buf[1] = 0x02;	// We are going to write 3 registers, n-1
//		if (gagesetup == 1)
//		{
//			m_tx_buf[2] = 0x01;	// MUX 1, select AIN 0 and AIN 1
//			m_tx_buf[4] = 0x20; 	// Select Internal ref on, ref 0, normal operation
//			gage = 0xFF;				// If it's the second gage (shear) then throw an 0xFF in there!
//			gagesetup = 0;			// Nex read is gage 0, bending
//		} else
//		{
//			m_tx_buf[2] = 0x25;	// MUX 1, select AIN 4 and AIN 5
//			m_tx_buf[4] = 0x28; 	// Select Internal ref on, Ref 1, normal operation
//			gage = 0x00; 				// If it's the first gage (bend) then throw an 0x00 in there!
//			gagesetup = 1;			// Next read is gage 1, shear
//		}
//		m_tx_buf[3] = 0x00;	//
//		nrf_drv_spi_transfer(&spi, m_tx_buf, 5, m_rx_buf, 5);
//		//spi_master_tx_rx(spi_base_address, 5, m_tx_buf, rx_data);
	
		//return (gage << 24 | m_rx_buf[0] << 16 | m_rx_buf[1] << 8 | m_rx_buf[2]);
}

int16_t ADS_EVT_Read_INSEL(int MUX_SP, int MUX_SN)
{
		ADS_inputselect(MUX_SP,MUX_SN);
	
		while (true) //Check to see if DRDY asserted low
		{
			if(nrf_gpio_pin_read(ADS_DRDY) == 0)
			{
				m_tx_buf[0] = 0xFF;	// NOP
				m_tx_buf[1] = 0xFF;	// NOP
				m_tx_buf[2] = 0xFF;	// NOP
				nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3);
				//spi_master_tx_rx(spi_base_address, 3, m_tx_buf, rx_data);
//				app_uart_put(m_rx_buf[0]);
//				app_uart_put(m_rx_buf[1]);
//				app_uart_put(m_rx_buf[2]);
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
				//spi_master_tx_rx(spi_base_address, 3, m_tx_buf, rx_data);
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
						//spi_master_tx_rx(spi_base_address, 5, m_tx_buf, rx_data);
//						simple_uart_put(rx_data[0]);
//						simple_uart_put(rx_data[1]);
//						simple_uart_put(rx_data[2]);
						//ADS_Sync();
						//simple_uart_put(rx_data[3]);
						//simple_uart_put(rx_data[4]);
						break;
					}
				}
		}
		return;
}

void ADS_test_read100B(void)
{	
	ADS_MUX1(1,1,0);
	bool Left = 0;
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
						//spi_master_tx_rx(spi_base_address, 3, m_tx_buf, rx_data);
//						simple_uart_put(rx_data[0]);
//						simple_uart_put(rx_data[1]);
//						simple_uart_put(rx_data[2]);
						//simple_uart_put(rx_data[3]);
						//simple_uart_put(rx_data[4]);
						break;
					}
				}
		}
		return;
}

void ADS_test_read100A(void)
{	
	ADS_MUX1(1,0,0);
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
						//spi_master_tx_rx(spi_base_address, 3, m_tx_buf, rx_data);
//						simple_uart_put(rx_data[0]);
//						simple_uart_put(rx_data[1]);
//						simple_uart_put(rx_data[2]);
						//simple_uart_put(rx_data[3]);
						//simple_uart_put(rx_data[4]);
						break;
					}
				}
		}
		return;
}


