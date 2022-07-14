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

#include "lis3dh-motion-detection.h"
#include "stdint.h"

#include "Wire.h"

//****************************************************************************//
//
//  Default construction is I2C mode, address 0x19.
//
//****************************************************************************//
LIS3DH::LIS3DH( uint8_t inputArg )
{
  I2CAddress = inputArg;
}

imu_status_t LIS3DH::begin( uint16_t accSample,
						uint8_t xAcc,
						uint8_t yAcc,
						uint8_t zAcc,
						uint8_t accSens )
{

	_DEBBUG("Configuring IMU");

	imu_status_t returnError = IMU_SUCCESS;

  	Wire.begin();
  
	//Spin for a few ms
	volatile uint8_t temp = 0;
	for( uint16_t i = 0; i < 10000; i++ )
	{
		temp++;
	}

	//Check the ID register to determine if the operation was a success.
	uint8_t readCheck;
	readRegister(&readCheck, LIS3DH_WHO_AM_I);
	if( readCheck != 0x33 )
	{
		returnError = IMU_HW_ERROR;
	}

	accelSampleRate = accSample;
	xAccelEnabled = xAcc;
	yAccelEnabled = yAcc;
	zAccelEnabled = zAcc;
	accelRange = accSens;

	_DEBBUG("Apply settings");
	applySettings();

	return returnError;
}

//****************************************************************************//
//
//  ReadRegisterRegion
//
//  Parameters:
//    *outputPointer -- Pass &variable (base address of) to save read data to
//    offset -- register to read
//    length -- number of bytes to read
//
//  Note:  Does not know if the target memory space is an array or not, or
//    if there is the array is big enough.  if the variable passed is only
//    two bytes long and 3 bytes are requested, this will over-write some
//    other memory!
//
//****************************************************************************//
imu_status_t LIS3DH::readRegisterRegion(uint8_t *outputPointer , uint8_t offset, uint8_t length)
{
	imu_status_t returnError = IMU_SUCCESS;

	//define pointer that will point to the external space
	uint8_t i = 0;
	uint8_t c = 0;

  Wire.beginTransmission(I2CAddress);
  offset |= 0x80; //turn auto-increment bit on, bit 7 for I2C
  Wire.write(offset);
  if( Wire.endTransmission() != 0 )
  {
    returnError = IMU_HW_ERROR;
  }
  else  //OK, all worked, keep going
  {
    // request 6 bytes from slave device
    Wire.requestFrom(I2CAddress, length);
    while ( (Wire.available()) && (i < length))  // slave may send less than requested
    {
      c = Wire.read(); // receive a byte as character
      *outputPointer = c;
      outputPointer++;
      i++;
    }
  }

	return returnError;
}

//****************************************************************************//
//
//  ReadRegister
//
//  Parameters:
//    *outputPointer -- Pass &variable (address of) to save read data to
//    offset -- register to read
//
//****************************************************************************//
imu_status_t LIS3DH::readRegister(uint8_t* outputPointer, uint8_t offset) {
	//Return value
	uint8_t result = 0;
	uint8_t numBytes = 1;
	imu_status_t returnError = IMU_SUCCESS;

	Wire.beginTransmission(I2CAddress);
	Wire.write(offset);
	if( Wire.endTransmission() != 0 )
	{
		returnError = IMU_HW_ERROR;
	}
	Wire.requestFrom(I2CAddress, numBytes);
	while ( Wire.available() ) // slave may send less than requested
	{
		result = Wire.read(); // receive a byte as a proper uint8_t
	}

		_DEBBUG("Read register 0x", offset, " = ", result);

		*outputPointer = result;
		return returnError;
}

//****************************************************************************//
//
//  readRegisterInt16
//
//  Parameters:
//    *outputPointer -- Pass &variable (base address of) to save read data to
//    offset -- register to read
//
//****************************************************************************//
imu_status_t LIS3DH::readRegisterInt16( int16_t* outputPointer, uint8_t offset )
{
	{
		//offset |= 0x80; //turn auto-increment bit on
		uint8_t myBuffer[2];
		imu_status_t returnError = readRegisterRegion(myBuffer, offset, 2);  //Does memory transfer
		int16_t output = (int16_t)myBuffer[0] | int16_t(myBuffer[1] << 8);

		_DEBBUG("12 bit from 0x", offset, " = ", output);
		*outputPointer = output;
		return returnError;
	}

}

//****************************************************************************//
//
//  writeRegister
//
//  Parameters:
//    offset -- register to write
//    dataToWrite -- 8 bit data to write to register
//
//****************************************************************************//
imu_status_t LIS3DH::writeRegister(uint8_t offset, uint8_t dataToWrite) {
	imu_status_t returnError = IMU_SUCCESS;

  //Write the byte
  Wire.beginTransmission(I2CAddress);
  Wire.write(offset);
  Wire.write(dataToWrite);
  if( Wire.endTransmission() != 0 )
  {
    returnError = IMU_HW_ERROR;
  }

	return returnError;
}

// Read axis acceleration as Float
float LIS3DH::axisAccel( axis_t _axis)
{
	int16_t outRAW;
	uint8_t regToRead = 0;
	switch (_axis)
	{
		case 0:
			// X axis
			regToRead = LIS3DH_OUT_X_L;
			break;
		case 1:
			// Y axis
			regToRead = LIS3DH_OUT_Y_L;
			break;
		case 2:
			// Z axis
			regToRead = LIS3DH_OUT_Z_L;
			break;
	
		default:
			// Not valid axis return NAN
			return NAN;
			break;
	}

	readRegisterInt16( &outRAW, regToRead );

	float outFloat;

	switch( accelRange )
	{
		case 2:
		outFloat = (float)outRAW / 15987;
		break;
		case 4:
		outFloat = (float)outRAW / 7840;
		break;
		case 8:
		outFloat = (float)outRAW / 3883;
		break;
		case 16:
		outFloat = (float)outRAW / 1280;
		break;
		default:
		outFloat = 0;
		break;
	}

	return outFloat;

}

	// Set the IMU to Power-down mode ~ 0.5uA;
imu_status_t LIS3DH::imu_power_down( void )
{
	// ODR[3:0] -> (0000: power-down mode; others: Refer to Table 31: Data rate configuration)
	return writeRegister(LIS3DH_CTRL_REG1, 0x00);
}

//****************************************************************************//
//
//  Apply settings passed to .begin();
//
//****************************************************************************//
void LIS3DH::applySettings( void )
{
	uint8_t dataToWrite = 0;  //Temporary variable
	
	//Build CTRL_REG1

	// page 16 set CTRL_REG1[3](LPen bit)
	#ifdef LOW_POWER
		dataToWrite |= 0x08;
	#endif

	//  Convert ODR
	switch(accelSampleRate)
	{
		case 1:
		dataToWrite |= (0x01 << 4);
		break;
		case 10:
		dataToWrite |= (0x02 << 4);
		break;
		case 25:
		dataToWrite |= (0x03 << 4);
		break;
		case 50:
		dataToWrite |= (0x04 << 4);
		break;
		case 100:
		dataToWrite |= (0x05 << 4);
		break;
		case 200:
		dataToWrite |= (0x06 << 4);
		break;
		default:
		case 400:
		dataToWrite |= (0x07 << 4);
		break;
		case 1600:
		dataToWrite |= (0x08 << 4);
		break;
		case 5000:
		dataToWrite |= (0x09 << 4);
		break;
	}

	dataToWrite |= (zAccelEnabled & 0x01) << 2;
	dataToWrite |= (yAccelEnabled & 0x01) << 1;
	dataToWrite |= (xAccelEnabled & 0x01);
	//Now, write the patched together data
	_DEBBUG ("LIS3DH_CTRL_REG1: 0x", dataToWrite);

	writeRegister(LIS3DH_CTRL_REG1, dataToWrite);

	//Build CTRL_REG4
	dataToWrite = 0; //Start Fresh!
	
	// page 16 set CTRL_REG4[3](HR bit)
#ifdef HIGH_RESOLUTION
	dataToWrite |= 0x08; //set high resolution
#endif

	//  Convert scaling
	switch(accelRange)
	{	
		default:
		case 2:
		dataToWrite |= (0x00 << 4);
		break;
		case 4:
		dataToWrite |= (0x01 << 4);
		break;
		case 8:
		dataToWrite |= (0x02 << 4);
		break;
		case 16:
		dataToWrite |= (0x03 << 4);
		break;
	}

	//Now, write the patched together data
	_DEBBUG ("LIS3DH_CTRL_REG4: 0x", dataToWrite);
	writeRegister(LIS3DH_CTRL_REG4, dataToWrite);

}

//****************************************************************************//
//
//  Configure interrupts 1 or 2, stop or move, threshold and duration
//	Durationsteps and maximum values depend on the ODR chosen.
//
//****************************************************************************//
imu_status_t LIS3DH::intConf(interrupt_t interrupt,
						event_t moveType, 
						uint8_t threshold,
						uint8_t timeDur,
						bool		polarity )
{
	uint8_t dataToWrite = 0;  //Temporary variable
	imu_status_t returnError = IMU_SUCCESS;

	uint8_t regToWrite = 0;
	regToWrite = (interrupt == INT_1) ? LIS3DH_INT1_CFG : LIS3DH_INT2_CFG;

	//Build INT_CFG 0x30 or 0x34
	//Detect movement or stop
	if(moveType == 1)	dataToWrite |= 0x0A;
	else 							dataToWrite |= 0x05;

	_DEBBUG ("LIS3DH_INT_CFG: 0x", dataToWrite);
	returnError = writeRegister(regToWrite, dataToWrite);
	
	//Build INT_THS 0x32 or 0x36
	regToWrite += 2;
	returnError = writeRegister(regToWrite, threshold);

	//Build INT_DURATION 0x33 or 0x37
	regToWrite++;

	returnError = writeRegister(regToWrite, timeDur);

	dataToWrite = 0 | (polarity << 1);

	//Attach configuration to Interrupt X
	if(interrupt == 1)
	{
		returnError = writeRegister(LIS3DH_CTRL_REG3, 0x40);
	}
	else
	{
		dataToWrite |= 0x20;
	}

	returnError = writeRegister(LIS3DH_CTRL_REG6, dataToWrite);
	
	return returnError;
}
