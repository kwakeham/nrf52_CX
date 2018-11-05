/******************************************************************************
 * Haptics.c
 *
 * Created on: April 21, 2012
 * Board: DRV2604EVM-CT, DRV2605EVM-CT
 *
 * Description: This file contains the functions for sending haptics waveforms.
 * 		Edit this file when changing actuators.
 *
 * Modified:
 *
 ******************************************************************************/

#ifndef DRV2605_H
#define DRV2605_H

// #include "msp430g2553.h"
// #include "CTS/structure.h"
//#include <stdio.h>
#include "DRV260x.h"
// #include <stdbool.h>
#include <stdint.h>


#define TICKDELAY	45714
#define LRAFREQ		LRAFREQ_160		// Set the LRA open loop frequency (see frequency constants below)

// Device Select - Write "1" next to the device that is on the EVM
#define DRV2604		0
#define DRV2605		1

// @TODO - Set control register settings
// Default Control Register Settings
#define DEFAULT_CTRL1	StartupBoost | DriveTime_3p3m // 320hz = DriveTime_1p8m, 160hz =  DriveTime_3p3m was DriveTime_2p4m, changed to DriveTime_3p1m
#define DEFAULT_CTRL2	BiDir_Input| BrakeStabilizer | SampleTime_300us | BlankingTime_Short | IDissTime_Short
#define DEFAULT_CTRL3	NGThresh_4PERCENT | ERM_ClosedLoop | DataFormat_RTP_Signed | LRADriveMode_Once | InputMode_PWM | LRA_AutoResonance

// DRV2605 Input Modes
#define MODE_RAM				0	// RAM Playback Mode
#define MODE_ROM				1	// ROM Playback Mode (Reserved for DRV2605)
#define MODE_PWM				2	// PWM Playback Mode
#define MODE_RTP				3	// RTP Real-Time Playback Mode
#define MODE_ANALOG				4	// Analog Input Mode
#define MODE_A2H				5	// Audio-to-Haptics Mode
#define MODE_DEFAULT			6   // Special RAM mode

// DRV260x Trigger Modes
#define TRIGGER_INTERNAL		0	// Internal trigger mode
#define TRIGGER_EXTERNAL_EDGE	1	// External trigger, Edge mode
#define TRIGGER_EXTERNAL_LEVEL	2	// External trigger, Level mode

// DRV260x Actuator
#define ACTUATOR_ERM			0
#define ACTUATOR_LRA			1
#define ACTUATOR_NONE			2

// DRV260x IN/TRIG Pin Coupling
#define COUPLING_DC				0
#define COUPLING_AC				1

// LRA Open-Loop Frequencies
#define LRAFREQ_240		0x104	// 240Hz
#define LRAFREQ_235		0x109	// 235Hz
#define LRAFREQ_230		0x10F	// 230Hz
#define LRAFREQ_225		0x115	// 225Hz
#define LRAFREQ_220		0x11C	// 220Hz
#define LRAFREQ_215		0x122	// 215Hz
#define LRAFREQ_210		0x129 	// 210Hz
#define LRAFREQ_205		0x130 	// 205Hz
#define LRAFREQ_200 	0x138	// 200Hz
#define LRAFREQ_195 	0x140   // 195Hz
#define LRAFREQ_190 	0x148	// 190Hz
#define LRAFREQ_185		0x151	// 185Hz
#define LRAFREQ_180		0x15B 	// 180Hz
#define LRAFREQ_175		0x165	// 175Hz
#define LRAFREQ_170		0x16F	// 170Hz
#define LRAFREQ_165 	0x17A	// 165Hz
#define LRAFREQ_160 	0x186	// 160Hz
#define LRAFREQ_155 	0x193	// 155Hz
#define LRAFREQ_150		0x1A0	// 150Hz
#define LRAFREQ_145		0x1AF	// 145Hz


/* LRA*/
#define DRV2605_ADDR 0x5A

#define DRV2605_REG_STATUS 0x00
#define DRV2605_REG_MODE 0x01
#define DRV2605_MODE_INTTRIG  0x00
#define DRV2605_MODE_EXTTRIGEDGE  0x01
#define DRV2605_MODE_EXTTRIGLVL  0x02
#define DRV2605_MODE_PWMANALOG  0x03
#define DRV2605_MODE_AUDIOVIBE  0x04
#define DRV2605_MODE_REALTIME  0x05
#define DRV2605_MODE_DIAGNOS  0x06
#define DRV2605_MODE_AUTOCAL  0x07


#define DRV2605_REG_RTPIN 0x02
#define DRV2605_REG_LIBRARY 0x03
#define DRV2605_REG_WAVESEQ1 0x04
#define DRV2605_REG_WAVESEQ2 0x05
#define DRV2605_REG_WAVESEQ3 0x06
#define DRV2605_REG_WAVESEQ4 0x07
#define DRV2605_REG_WAVESEQ5 0x08
#define DRV2605_REG_WAVESEQ6 0x09
#define DRV2605_REG_WAVESEQ7 0x0A
#define DRV2605_REG_WAVESEQ8 0x0B

#define DRV2605_REG_GO 0x0C
#define DRV2605_REG_OVERDRIVE 0x0D
#define DRV2605_REG_SUSTAINPOS 0x0E
#define DRV2605_REG_SUSTAINNEG 0x0F
#define DRV2605_REG_BREAK 0x10
#define DRV2605_REG_AUDIOCTRL 0x11
#define DRV2605_REG_AUDIOLVL 0x12
#define DRV2605_REG_AUDIOMAX 0x13
#define DRV2605_REG_RATEDV 0x16
#define DRV2605_REG_CLAMPV 0x17
#define DRV2605_REG_AUTOCALCOMP 0x18
#define DRV2605_REG_AUTOCALEMP 0x19
#define DRV2605_REG_FEEDBACK 0x1A
#define DRV2605_REG_CONTROL1 0x1B
#define DRV2605_REG_CONTROL2 0x1C
#define DRV2605_REG_CONTROL3 0x1D
#define DRV2605_REG_CONTROL4 0x1E
#define DRV2605_REG_VBAT 0x21
#define DRV2605_REG_LRARESON 0x22


/***********************************************************
 *
 * 	DO NOT EDIT BELOW THIS LINE
 *
 ***********************************************************/

/* Begin setting actuator auto-calibration values  */


// Waveform Structure Type Definition
typedef struct Haptics_Waveform {
	const unsigned char 	inputMode; 		// See input mode
	const unsigned char		length;			// Size of array in bytes
	const unsigned char* 	data;			// Pointer to waveform array data (waveform array is in (amplitude, time) pairs
} Waveform;


void I2C_WriteSingleByte(uint8_t DRV260x_REG, uint8_t DRV260x_DATA);

uint8_t I2C_ReadSingleByte(uint8_t DRV260x_REG);

/**
 * Haptics_Init - initialize haptics variables and settings
 */
void Haptics_Init(void);

/**
 * Haptics_SetControlRegisters - set the control register variables
 */
void Haptics_SetControlRegisters(void);

/**
 * Haptics_RunAutoCal_LRA - run auto-calibration for an ERM actuator
 */
void Haptics_RunAutoCal_ERM(void);

/**
 * Haptics_RunAutoCal_LRA - run auto-calibration for an LRA actuator
 */
void Haptics_RunAutoCal_LRA(void);

/**
 * Haptics_RecordAutoCalValues - save auto-calibration values
 */
void Haptics_RecordAutoCalValues(uint8_t recordAutoCal);

/**
 * Haptics_Diagnostics - run actuator diagnostics
 * @return unsigned char - DRV260x status register
 */
unsigned char Haptics_Diagnostics(uint8_t actuator);

/**
 * Haptics_SendWaveformDefault- send haptic waveform using previously
 * 		selected actuator and trigger.
 * @param struct Waveform - the waveform output type, length in bytes, and data
 */
void Haptics_SendWaveformDefault(const Waveform waveform);

/**
 * Haptics_SendWaveform - send haptic waveform
 * @param struct Waveform - the waveform input type, length in bytes, and data
 * @param unsigned char actuator - actuator type
 * @param unsigned char trigger - trigger type (internal, ext. edge, ext. level)
 */
void Haptics_SendWaveform(const Waveform waveform, const uint8_t actuator, const uint8_t trigger);

/**
 * Haptics_OutputWaveform - trigger or output the waveform
 * @param struct Waveform - the waveform output type, length in bytes, and data
 */
void Haptics_OutputWaveform(const Waveform waveform);

/**
 * Haptics_HardwareMode - Set the hardware pins to the appropriate setting
 * @param unsigned char inputMode - the waveform input mode
 */
void Haptics_HardwareMode(uint8_t inputMode);

/**
 * Haptics_SendTriggerType - set the trigger settings in the DRV260x
 */
void Haptics_SendTriggerType(void);

/**
 * Haptics_SendActuatorSettings - set the actuator settings in the DRV260x
 */
void Haptics_SendActuatorSettings(void);

/**
 * Haptics_SetERMOpenLoopMode - set the device to open loop mode
 * @param openLoopMode - ERM_ClosedLoop = closed-loop mode, ERM_OpenLoop = open-loop mode
 */
void Haptics_SetERMOpenLoopMode(uint8_t openLoop);

/**
 * Haptics_GetERMOpenLoopMode - get the status of ERM open loop flag
 * @return unsigned char 0 = Closed Loop, 1 = Open Loop
 */
unsigned char Haptics_GetERMOpenLoopMode(void);

/**
 * Haptics_SetLRAOpenLoopMode - set the device to LRA open loop mode
 * @param openLoopMode - LRA_AutoResonance = closed-loop mode, LRA_OpenLoop = Divide by 128x
 */
void Haptics_SetLRAOpenLoopMode(uint8_t openLoop);

/**
 * Haptics_GetLRAOpenLoopMode - get the status of LRA open loop flag
 * @return unsigned char 0 = AutoResonance, 1 = Divide by 128x
 */
unsigned char Haptics_GetLRAOpenLoopMode(void);

/**
 * Haptics_EnableTrigger - turn on the trigger, uses last executed trigger type
 */
void Haptics_EnableTrigger(void);

/**
 * Haptics_DisableTriggerActive - turn off the active trigger
 */
void Haptics_DisableTriggerActive(void);

/**
 * Haptics_DisableTriggerActive - turn off the trigger specified by "trigger"
 * @param trigger - unsigned int - trigger type (internal, ext. edge, ext. level)
 */
void Haptics_DisableTrigger(uint8_t trigger);

/**
 * Haptics_SetActuator - set the actuator type
 * @param unsigned char actuator - acutator type
 */
void Haptics_SetActuator(uint8_t actuator);

/**
 * Haptics_IsActuatorLRA - determine actuator selected
 * @return unsigned char - actuator type (0 = ERM, 1 = LRA)
 */
uint8_t Haptics_IsActuatorLRA(void);

/**
 * Haptics_SetTrigger - set the trigger DRV260x trigger type
 * @param unsigned char trigger - trigger constant
 */
void Haptics_SetTrigger(uint8_t trigger);

/**
 * Haptics_GetTrigger - get the trigger type
 * @return unsigned char - trigger type (internal = 0, ext. edge = 1, ext. level = 2)
 */
uint8_t Haptics_GetTrigger(void);

/**
 * Haptics_Start - Initialize clocks and enable the haptics driver
 */
void Haptics_Start(uint8_t inputMode);

/**
 * Haptics_Start - Stop clocks and disable haptics driver
 */
void Haptics_Stop(uint8_t inputMode);

/**
 * Haptics_DefaultReset - reset control register 1-3 with default values
 */
void Haptics_DefaultReset(void);

/**
 * Haptics_OutputEnableSet - enable/disable play back of all waveforms
 */
void Haptics_OutputEnableSet(uint8_t enable);

/**
 * Haptics_IsOutputEnabled - check if waveform play back is enabled
 */
uint8_t Haptics_IsOutputEnabled(void);

/**
 * Haptics_EnableAmplifier - set enable pin high
 */
void Haptics_EnableAmplifier(void);

/**
 * Haptics_EnableAmplifier - set enable pin low
 */
void Haptics_DisableAmplifier(void);

/**
 * Haptics_SetDisabledAllowed
 * @return disabledAllowed - unsigned char - 0 = disabled not allowed, 1 = disabled allowed
 */
unsigned char Haptics_IsDisableAllowed(void);

/**
 * Haptics_SetDisabledAllowed
 * @param allowed - unsigned char - 0 = disabled not allowed, 1 = disabled allowed
 */
void Haptics_SetDisabledAllowed(uint8_t allowed);

/**
 * Haptics_LoadSwitchSelect - select the on-board actuator using the load switch
 * @param unsigned char actuator 0 = ERM, 1 = LRA
 */
void Haptics_LoadSwitchSelect(uint8_t actuator);

/**
 * Haptics_EffectActive - set if effect is active
 * @param unsigned char active - set activity status
 */
void Haptics_EffectActive(uint8_t active);

/**
 * Haptics_IsEffectActive - check if effect is active
 * @return unsigned char - 0  = inactive, 1 = active
 */
uint8_t Haptics_IsEffectActive(void);

/**
 * Haptics_SetLibrary - select ROM Library (1-6)
 * @param unsigned char Library - library number 1-6
 */
void Haptics_SetLibrary(uint8_t library);

/**
 * Haptics_SetSequenceRegister - insert a waveform into one of 8 sequence registers
 * @param unsigned char sequenceReg - register address for sequence register
 * @param unsigned char waveformIndex - index number for waveform
 */
void Haptics_SetSequenceRegister(uint8_t sequenceReg, uint8_t waveformIndex);

/**
 * Haptics_WaitWaveformComplete - wait for playback to complete for internal trigger waveforms
 */
void Haptics_WaitWaveformComplete(void);

#endif /* HAPTICS_H_ */
