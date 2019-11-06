/*!
 * @file singleMeasurement.ino
 * @brief Read ambient temperature (C/F) and relative humidity (%RH) in single-read mode
 * @n Experimental phenomenon: the chip defaults in this mode, we need to send instructions to enable the chip collect data.
 * @n Which means the repeatability of the read needs to be set (the difference between the data measured by the chip under the same measurement conditions)
 * @n then read the temperature and humidity data and pintin the data in the serial port.
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
 * @param pWire IC bus pointer object and construction device, can both pass or not pass parameters, Wire in default.
 * @param address Chip IIC address, two optional addresses 0x44 and 0x45(0x45 in default).
 * @param RST Chip reset pin, 4 in default.
 * @n The IIC address is determined by the pin addr on the chip.
 * @n When the ADR is connected to VDD, the chip IIC address is 0x45.
 * @n When the ADR is connected to GND, the chip IIC address is 0x44.
 */

//DFRobot_SHT3x sht3x(&Wire,/*address=*/0x45,/*RST=*/4);

DFRobot_SHT3x   sht3x;

void setup() {
  Serial.begin(9600);
  //Initialize the chip
  while (sht3x.begin() != 0) {
    Serial.println("The initialization of the chip is failed, please confirm whether the chip connection is correct");
    delay(1000);
  }
  /**
   * readSerialNumber: Read the serial number of the chip
   * @return Return 32-digit serial number
   */
  Serial.print("The chip serial number:");
  Serial.println(sht3x.readSerialNumber());
  
  /**
   * softReset：Send command resets via iiC, enter the chip's default mode single-measure mode, turn off the heater, and clear the alert of the ALERT pin.
   * @return Read the status register to determine whether the command was executed successfully, and returning true indicates success
   */
   if(!sht3x.softReset()){
     Serial.println("Failed to reset the chip");
   }
   
  /**
   * heaterEnable(): Turn on the heater inside the chip so that the sensor can have accurate humidity data even in humid environments.
   * @return Read the status register to determine whether the command was executed successfully, and returning true indicates success.
   * @NOTE: Heaters should be used in wet environments, and other cases of use will result in incorrect readings
   */
  //if(!sht3x.heaterEnable()){
    // Serial.println("Failed to turn on the heater");
  //}
  Serial.println("------------------Read data in single measurement mode-----------------------");
}

void loop() {

  Serial.print("Ambient temperature(°C/F):");
  /**
   * getTemperatureC: Get the measured temperature (in degrees Celsius)
   * @return Return the float temperature data 
   */
  Serial.print(sht3x.getTemperatureC());
  Serial.print(" C/");
  /**
   * getTemperatureF: Get the measured temperature (in degrees Fahrenheit)
   * @return Return the float temperature data 
   */
  Serial.print(sht3x.getTemperatureF());
  Serial.print(" F      ");
  Serial.print("Relative humidity(%RH):");
  /**
   * getHumidityRH: Get measured humidity(%RH)
   * @return Return the float humidity data
   */
  Serial.print(sht3x.getHumidityRH());
  Serial.println("%RH");
  
  /**
   * @brief Get temperature and humidity data in single measurement mode.
   * @param repeatability: Set repeatability to read temperature and humidity data with the type eRepeatability_t.
   * @param repeatability: Read repeatability of the temperature and humidity data 
   * @note  Optional parameters:
               eRepeatability_High /**In high repeatability mode, the humidity repeatability is 0.10%RH, the temperature repeatability is 0.06°C
               eRepeatability_Medium,/**In medium repeatability mode, the humidity repeatability is 0.15%RH, the temperature repeatability is 0.12°C.
               eRepeatability_Low, /**In low repeatability mode, the humidity repeatability is0.25%RH, the temperature repeatability is 0.24°C
   * @return: Return a structure containing celsius temperature (°C), Fahrenheit temperature (°F), relative humidity (%RH), status code
   * @n A status of 0 indicates the right return data.
   *
  DFRobot_SHT3x::sRHAndTemp_t data = sht3x.readTemperatureAndHumidity(sht3x.eRepeatability_High);
  if(data.ERR == 0){
    Serial.print("Ambient temperature(°C/F):");
    Serial.print(data.TemperatureC);
    Serial.print(" C/");
    Serial.print(data.TemperatureF);
    Serial.print(" F      ");
    Serial.print("Relative humidity(%RH):");
    Serial.print(data.Humidity);
    Serial.println(" %RH");
  }
  */
  delay(1000);
}
