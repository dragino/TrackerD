/******************************************************************************
lis3dh-motion-detection.h
LIS3DH Arduino
Leonardo Bispo
Mar 03, 2019
https://github.com/ldab/lis3dh-motion-detection
Resources:
Uses Wire.h for i2c operation

Inspired by https://github.com/sparkfun/SparkFun_LIS3DH_Arduino_Library

Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef __LIS3DH_IMU_H__
#define __LIS3DH_IMU_H__

#include "stdint.h"

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#if defined LOW_POWER && defined NORMAL_MODE
	#error Please choose between the 3 resolution types
#elif defined NORMAL_MODE && defined HIGH_RESOLUTION
	#error Please choose between the 3 resolution types
#elif defined LOW_POWER && defined HIGH_RESOLUTION
	#error Please choose between the 3 resolution types
#endif

#ifdef LIS3DH_DEBUG
	namespace {
  		template<typename T>
  		static void _DEBBUG(T last) {
    	Serial.println(last);
		}
		
		template<typename T, typename... Args>
		static void _DEBBUG(T head, Args... tail) {
			Serial.print(head);
			Serial.print(' ');
			_DEBBUG(tail...);
		}
	}
#else
  	#define _DEBBUG(...)
#endif

//Print variable name
#define getName(var)  #var

// Return values 
typedef enum
{
	IMU_SUCCESS,
	IMU_HW_ERROR,
	IMU_NOT_SUPPORTED,
	IMU_GENERIC_ERROR,
	IMU_OUT_OF_BOUNDS,
	IMU_ALL_ONES_WARNING,
	//...
} imu_status_t;

typedef enum
{
	DET_STOP,
	DET_MOVE,
	//...
} event_t;

typedef enum
{
	INT_1 = 1,
	INT_2,
} interrupt_t;

typedef enum
{
	X = 0,
	Y,
	Z,
} axis_t;

class LIS3DH
{
public:
	LIS3DH( uint8_t );
	
	imu_status_t begin( uint16_t accSample,
					uint8_t xAcc,
					uint8_t yAcc,
					uint8_t zAcc,
					uint8_t accSens );
	
	// The following utilities read and write to the IMU

	// readRegister reads one 8-bit register
	imu_status_t readRegister(uint8_t* outputPointer, uint8_t offset);
	
	// Reads two 8-bit regs, LSByte then MSByte order, and concatenates them.
	// Acts as a 16-bit read operation
	imu_status_t readRegisterInt16(int16_t*, uint8_t offset );
	
	// Writes an 8-bit byte;
	imu_status_t writeRegister(uint8_t, uint8_t);

	// Configure Interrupts
	// INT1 or 2, Move or Stop, Detection Sensivity and duration cycles = from 1 to 127
	imu_status_t intConf(interrupt_t interrupt,
					event_t moveType, 
					uint8_t threshold,
					uint8_t timeDur,
					bool 		polarity = 0);
	
	// Read axis acceleration as Float
	float axisAccel( axis_t _axis);

	// Set the IMU to Power-down mode ~ 0.5uA;
	imu_status_t imu_power_down( void );
	
private:
	uint8_t I2CAddress;
	uint16_t accelSampleRate; //Can be 1, 10, 25, 50, 100, 200, 400, 1600, 5000
	uint8_t xAccelEnabled;
	uint8_t yAccelEnabled;
	uint8_t zAccelEnabled;
	uint8_t accelRange; 			//Accelerometer range = 2, 4, 8, 16g

	//Apply settings at .begin()
	void applySettings( void );

	//ReadRegisterRegion takes a uint8 array address as input and reads
	//  a chunk of memory into that array.
	imu_status_t readRegisterRegion(uint8_t*, uint8_t, uint8_t );
};

//Device Registers
#define LIS3DH_STATUS_REG_AUX         0x07
#define LIS3DH_WHO_AM_I               0x0F

#define LIS3DH_CTRL_REG1              0x20
#define LIS3DH_CTRL_REG2              0x21
#define LIS3DH_CTRL_REG3              0x22
#define LIS3DH_CTRL_REG4              0x23
#define LIS3DH_CTRL_REG5              0x24 //not included
#define LIS3DH_CTRL_REG6              0x25
#define LIS3DH_REFERENCE              0x26
#define LIS3DH_STATUS_REG2            0x27
#define LIS3DH_OUT_X_L                0x28
#define LIS3DH_OUT_X_H                0x29
#define LIS3DH_OUT_Y_L                0x2A
#define LIS3DH_OUT_Y_H                0x2B
#define LIS3DH_OUT_Z_L                0x2C
#define LIS3DH_OUT_Z_H                0x2D
#define LIS3DH_FIFO_CTRL_REG          0x2E
#define LIS3DH_FIFO_SRC_REG           0x2F
#define LIS3DH_INT1_CFG               0x30
#define LIS3DH_INT1_SRC               0x31
#define LIS3DH_INT1_THS               0x32
#define LIS3DH_INT1_DURATION          0x33
#define LIS3DH_INT2_CFG               0x34
#define LIS3DH_INT2_SRC               0x35
#define LIS3DH_INT2_THS               0x36
#define LIS3DH_INT2_DURATION          0x37

#define LIS3DH_CLICK_CFG              0x38
#define LIS3DH_CLICK_SRC              0x39
#define LIS3DH_CLICK_THS              0x3A
#define LIS3DH_TIME_LIMIT             0x3B
#define LIS3DH_TIME_LATENCY           0x3C
#define LIS3DH_TIME_WINDOW            0x3D

#endif // End of __LIS3DH_IMU_H__ definition check