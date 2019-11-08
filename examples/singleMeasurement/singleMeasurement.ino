/*!
 * @file singleMeasurement.ino
 * @brief Read ambient temperature (C/F) and relative humidity (%RH) in single-read mode.
 * @n Experimental phenomenon the chip defaults in this mode, we need to send instructions to enable the chip collect data,
 * which means the repeatability of the read needs to be set (the difference between the data measured by the chip under the same measurement conditions)
 * then read the temperature and humidity data and pintin the data in the serial port.
 * @n Single measure mode: read data as needed, power consumption is relatively low, the chip IDE state only costs 0.5mA. 
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-08-21
 * @get from https://www.dfrobot.com
 * @url https://github.com/DFRobot/DFRobot_SHT3x
*/

#include <DFRobot_SHT3x.h>

/*!
 * @brief Construct the function
 * @param pWire IIC bus pointer object and construction device, can both pass or not pass parameters, Wire in default.
 * @param address Chip IIC address, two optional addresse.
