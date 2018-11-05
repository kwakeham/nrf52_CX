/******************************************************************************
 * Actuator_Waveforms.h
 *
 * Created on: April 23, 2012
 * Board: DRV2605EVK-CT RevA
 *
 ******************************************************************************/

#include "Actuator_Waveforms.h"
#include "DRV2605.h"



//--------------------------------------------------------//
// ROM Structure
//--------------------------------------------------------//
static unsigned char ERM_ROM_data[] = {
		DRV260x_CONTROL3, ERM_OpenLoop, 0x20,
		DRV260x_LIBRARY, ROM_A, 0xFF
};
const Waveform ERM_ROM = {MODE_ROM,6,ERM_ROM_data};

static unsigned char LRA_ROM_data[] = {
		DRV260x_LIBRARY, ROM_LRA, 0xFF
};
const Waveform LRA_ROM = {MODE_ROM,0,LRA_ROM_data};

static unsigned char ROM_data[] = {
		0x00, 0x00, 0xFF
};
const Waveform Generic_ROM = {MODE_ROM,0,ROM_data};

//--------------------------------------------------------//
// PWM Mode Examples
//--------------------------------------------------------//
static const unsigned char PWM_Click_data[] = {
		0xFE, 0x04,
		0x01, 0x03
};
const Waveform PWM_Click = {MODE_PWM,4,PWM_Click_data};

static const unsigned char PWM_Alert_data[] = {
		0xFE, 0x03,
		0xB8, 0x96,
		0x00, 0x05
};
const Waveform PWM_Alert = {MODE_PWM,6,PWM_Alert_data};

static const unsigned char PWM_Alert_NoBrake_data[] = {
		0xFE, 0x96
};
const Waveform PWM_Alert_NoBrake = {MODE_PWM,2,PWM_Alert_NoBrake_data};

//--------------------------------------------------------//
// RTP Mode Examples
//--------------------------------------------------------//
static const unsigned char RTP_Click_data[] = {
		0x7F, 0x06,
		0x00, 0x08
};
const Waveform RTP_Click = {MODE_RTP,4,RTP_Click_data};

static const unsigned char RTP_Alert_data[] = {
		0x7F, 0x96,
		0x00, 0x08
};
const Waveform RTP_Alert = {MODE_RTP,4,RTP_Alert_data};

static const unsigned char RTP_Alert_NoBrake_data[] = {
		0x7F, 0x96
};
const Waveform RTP_Alert_NoBrake = {MODE_RTP,2,RTP_Alert_NoBrake_data};

//--------------------------------------------------------//
// Analog Mode
//--------------------------------------------------------//
static const unsigned char Analog_AC_data[] = {
		DRV260x_CONTROL1, AC_Couple, 0x20,
		DRV260x_CONTROL3, InputMode_ANALOG, 0x02,
		DRV260x_MODE, PWM_Analog, 0xFF
};
const Waveform Analog_AC = {MODE_ANALOG,9,Analog_AC_data};

static const unsigned char Analog_DC_data[] = {
		DRV260x_CONTROL1, DC_Couple, 0x20,
		DRV260x_CONTROL3, InputMode_ANALOG, 0x02,
		DRV260x_MODE, PWM_Analog, 0xFF
};
const Waveform Analog_DC = {MODE_ANALOG,9,Analog_DC_data};

//--------------------------------------------------------//
// Audio2Haptics Mode
//--------------------------------------------------------//
static const unsigned char A2H_AC_data[] = {
		DRV260x_CONTROL1, AC_Couple, 0x20,
		DRV260x_CONTROL3, (InputMode_ANALOG), 0x22,
		DRV260x_MODE, Audio2Haptics, 0xFF
};
const Waveform A2H_AC = {MODE_A2H,9,A2H_AC_data};

static const unsigned char A2H_DC_data[] = {
		DRV260x_CONTROL1, DC_Couple, 0x20,
		DRV260x_CONTROL3, (InputMode_ANALOG), 0x22,
		DRV260x_MODE, Audio2Haptics, 0xFF
};
const Waveform A2H_DC = {MODE_A2H,9,A2H_DC_data};

