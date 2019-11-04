/*!
 * @file periodicDataReading.ino
 * @brief Read ambient temperature (C/F) and relative humidity (%RH) in cycle read mode.
 * @n Experimental phenomenon: Before we start, we set the read frequency and repeatability of the read
 * @n (the difference between the data measured by the chip under the same measurement conditions).
 * @n and enter the periodic read mode, and then read the temperature and humidity data.
 * @n the temperature and humidity data will be printed at the serial port, after 10 seconds of operation
 * @n it will exit the cycle mode and enter 2 measurement mode: Single measurement mode and Cycle measurement mode.
 * @n Single measurement mode, reflecting the difference between the two modes of reading data
 * @n Cycle measurement mode: the chip periodically monitors temperature and humidity, only in this mode the ALERT pin will work
 * 
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-08-20
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

DFRobot_SHT3x sht3x;

void setup() {

  Serial.begin(9600);
    //Initialize the chip to detect if it can communicate properly
  while (sht3x.begin() != 0) {
    Serial.println("Failed to initialize the chip, please confirm the chip connection");
    delay(1000);
  }
  
  /**
   * readSerialNumber: Read the serial number of the chip
   * @return: Return 32-digit serial number
   */
  Serial.print("chip serial number: ");
  Serial.println(sht3x.readSerialNumber());
  /**
   * softReset：Send command resets via IIC, enter the chip's default mode single-measure mode, 
   * turn off the heater, and clear the alert of the ALERT pin.
   * @return: Read the status register to determine whether the command was executed successfully, 
   * and returning true indicates success
   */
   if(!sht3x.softReset()){
     Serial.println("Failed to reset the chip");
   }

  /**
   * pinReset: Reset through the chip's reset pin, enter the chip's default mode single-measure mode, 
   * turn off the heater, and clear the alert of the ALERT pin.
   * @return: The status register has a data bit that detects whether the chip has been reset, 
   * and returning true indicates success
   * @note When using this API, the reset pin of the chip, nRESET, should be connected to RST (default to pin4) of arduino.
   */
  //if(!sht3x.pinReset()){
    //Serial.println("Failed to reset the chip");
  //}

  /**
   * heaterEnable(): Turn on the heater inside the chip so that the sensor can have accurate humidity data even in humid environments.
   * @return: Read the status register to determine whether the command was executed successfully, and returning true indicates success.
   * @NOTE: Heaters should be used in wet environments, and other cases of use will result in incorrect readings
   */
  //if(!sht3x.heaterEnable()){
    // Serial.println("Failed to turn on the heater");
  //}
  /**
   * startPeriodicMode: Enter cycle measurement mode and set repeatability and read frequency.
   * @param measureFreq: Read the eMeasureFrequency_t data frequency
   * @note  Selectable parameters:
               eMeasureFreq_Hz5,   /**the chip collects data in every 2s
               eMeasureFreq_1Hz,   /**the chip collects data in every 1s 
               eMeasureFreq_2Hz,   /**the chip collects data in every 0.5s 
               eMeasureFreq_4Hz,   /**the chip collects data in every 0.25s 
               eMeasureFreq_10Hz   /**the chip collects data in every 0.1s 
   * @param repeatability: Reading the repeatability of temperature and humidity data, the default parameter is eRepeatability_High.
   * @note  Optional parameters:
               eRepeatability_High /**In high repeatability mode, the humidity repeatability is 0.10%RH, the temperature repeatability is 0.06°C
               eRepeatability_Medium,/**In medium repeatability mode, the humidity repeatability is 0.15%RH, the temperature repeatability is 0.12°C.
               eRepeatability_Low, /**In low repeatability mode, the humidity repeatability is0.25%RH, the temperature repeatability is 0.24°C
   * @return Read the status of the register to determine whether the command was executed successfully, and returning true indicates success
   */          
  if(!sht3x.startPeriodicMode(sht3x.eMeasureFreq_1Hz)){
    Serial.println("Failed to enter the periodic mode");
  }
  Serial.println("------------------周期测量模式下读取数据-----------------------");
}

void loop() {

  Serial.print("环境温度(°C/F):");
  /**
   * getTemperatureC:获取测量到的温度(单位：摄氏度)
   * @return 返回float类型的温度数据
   */
  Serial.print(sht3x.getTemperatureC());
  Serial.print(" C/");
  /**
   * getTemperatureF:获取测量到的温度(单位：华氏度)
   * @return 返回float类型的温度数据
   */
  Serial.print(sht3x.getTemperatureF());
  Serial.print(" F      ");
  Serial.print("相对湿度(%RH):");
  /**
   * getHumidityRH :获取测量到的湿度(单位：%RH)
   * @return 返回float类型的湿度数据
   */
  Serial.print(sht3x.getHumidityRH());
  Serial.println(" %RH");
  //应该根据芯片采集数据的频率自行调节读取的频率.
  //读取数据的频率应该大于芯片采集数据的频率，否则返回的数据就会出错。
  delay(100);
  if(millis() > 10000 && millis() < 10200){
    /**
     * stopPeriodicMode(): 从周期读取数据模式退出。
     * @return 通过读取状态寄存器来判断命令是否成功被执行，返回true则表示成功
     */
    sht3x.stopPeriodicMode();
    Serial.println("已退出周期测量模式,进入单次测量模式");
  }
  /**
   * readTemperatureAndHumidity: 在周期测量模式下获取温湿度数据,使用结构体接收数据
   * @return 返回包含摄氏温度(°C),华氏温度(°F),相对湿度(%RH),状态码的结构体
   * @n 状态为0表示返回数据正确
   *
  DFRobot_SHT3x::sRHAndTemp_t data = sht3x.readTemperatureAndHumidity();
  if(data.ERR == 0){
    Serial.print("环境温度(°C/F):");
    Serial.print(data.TemperatureC);
    Serial.print(" C/");
    Serial.print(data.TemperatureF);
    Serial.print(" F      ");
    Serial.print("相对湿度(%RH):");
    Serial.print(data.Humidity);
    Serial.println(" %RH");
  }
  */
}
