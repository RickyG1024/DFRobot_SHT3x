/*!
 * @file alert.ino
 * @brief Temperature and humidity over-threshold alarm
 * @n Experimental phenomenon: The user customizes the temperature and humidity thresholds, 
 * @n and the ALERT pin generates an alarm signal once the values exceed the user-defined threshold
 * @n NOTE: The ALERT pin on the sensor should be connected to the interrupt pin on the main panel when using this function.
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence  The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-08-26
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
//The non-alarm status of the alert pin is low;
volatile  int alertState = 0;
void alert(){
  alertState = 1 - alertState;
}
void setup() {
  Serial.begin(9600);
  #ifdef ARDUINO_ARCH_MPYTHON 
  /*                    The Correspondence Table of ESP32 Interrupt Pins And Terminal Numbers
   * -------------------------------------------------------------------------------------------------------------
   * |                    |  DigitalPin  | P0-P20 can be used as an external interrupt                           |
   * |    esp32           |--------------------------------------------------------------------------------------|
   * |                    | Interrupt No |  DigitalPinToInterrupt (Pn) can be used to query the interrupt number |
   * |-----------------------------------------------------------------------------------------------------------|
   */
  attachInterrupt(digitalPinToInterrupt(P16)/*Query the interrupt number of the P16 pin*/,alert,CHANGE);
  //Open esp32's P16 pin for external interrupt, bilateral edge trigger, ALERT connected to P16
  #else
  /*    The Correspondence Table of AVR Series Arduino Interrupt Pins And Terminal Numbers
   * ---------------------------------------------------------------------------------------
   * |                                        |  DigitalPin  | 2  | 3  |                   |
   * |    Uno, Nano, Mini, other 328-based    |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  |                   |
   * |-------------------------------------------------------------------------------------|
   * |                                        |    Pin       | 2  | 3  | 21 | 20 | 19 | 18 |
   * |               Mega2560                 |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  | 2  | 3  | 4  | 5  |
   * |-------------------------------------------------------------------------------------|
   * |                                        |    Pin       | 3  | 2  | 0  | 1  | 7  |    |
   * |    Leonardo, other 32u4-based          |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  | 2  | 3  | 4  |    |
   * |--------------------------------------------------------------------------------------
   */
  /*                      The Correspondence Table of micro:bit Interrupt Pins And Terminal Numbers
   * -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   * |                                                                                       |  DigitalPin  |  P0-P20 can be used as an external interrupt                                    |
   * |                  micro:bit                                                            |------------------------------------------------------------------------------------------------|
   * |(When using as an external interrupt, no need to set it to input mode with pinMode)    | Interrupt No | Interrupt number is a pin digital value, such as P0 interrupt number 0, P1 is 1 |
   * |----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
   */
  attachInterrupt(/*Interrupt No*/0,alert,CHANGE);//Open the external interrupt 0, connect ALERT to the digital pin of the main control: UNO(2), Mega2560(2), Leonardo(3), microbit(P0).
  #endif
    //Initialize the chip to detect if it can communicate properly
  while (sht3x.begin() != 0) {
    Serial.println("The initialization of the chip is failed, please confirm whether the chip connection is correct");
    delay(1000);
  }
  /**
   * readSerialNumber: Read the serial number of the chip
   * @return Return 32-digit serial number
   */
  Serial.print("The chip serial number");
  Serial.println(sht3x.readSerialNumber());
  /**
   * softReset：Send command resets via iiC, enter the chip's default mode single-measure mode, turn off the heater, and clear the alert of the ALERT pin.
   * @return Read the status register to determine whether the command was executed successfully, and returning true indicates success
   */
  if(!sht3x.softReset()){
     Serial.println("Failed to reset the chip");
   }
  /**
   * @brief All flags (Bit 15, 11, 10, 4) in the status register can be cleared (set to zero)
   * @n ALERT can work properly only when the bit:15 is set to 0, otherwise it will remain high.
   */
  sht3x.clearStatusRegister();
  /**
   * startPeriodicMode: Enter cycle measurement mode and set repeatability, read frequency, and only in this mode ALERT can work.
   * @param measureFreq: Read the data frequency, data type eMeasureFrequency_t
   * @note  Selectable parameters:
               eMeasureFreq_Hz5,   /**the chip collects data in every 2s
               eMeasureFreq_1Hz,   /**the chip collects data in every 1s 
               eMeasureFreq_2Hz,   /**the chip collects data in every 0.5s 
               eMeasureFreq_4Hz,   /**the chip collects data in every 0.25s 
               eMeasureFreq_10Hz   /**the chip collects data in every 0.1s 
   * @param repeatability: reading the repeatability of temperature and humidity data, the default parameter is eRepeatability_High.
   * @note  Optional parameters:
               eRepeatability_High /**In high repeatability mode, the humidity repeatability is 0.10%RH, the temperature repeatability is 0.06°C
               eRepeatability_Medium,/**In medium repeatability mode, the humidity repeatability is 0.15%RH, the temperature repeatability is 0.12°C.
               eRepeatability_Low, /**In low repeatability mode, the humidity repeatability is0.25%RH, the temperature repeatability is 0.24°C
   * @return Read the status of the register to determine whether the command was executed successfully, and returning true indicates success
   */
  if(!sht3x.startPeriodicMode(sht3x.eMeasureFreq_10Hz)){
    Serial.println("Failed to enter the periodic mode");
  }
  /**
   * setTemperatureLimitC: Set the threshold temperature and alarm clear temperature(°C)
   * setTemperatureLimitF: Set the threshold temperature and alarm clear temperature(°F)
   * @param highset: High temperature alarm point, when the temperature is greater than this value, the ALERT pin generates an alarm signal.
   * @param highClear: High temperature alarm clear point, alarming when the temp higher than the highset, otherwise the alarm signal will be cleared.
   * @param lowset: Low temperature alarm point, when the temperature is lower than this value, the ALERT pin generates an alarm signal.
   * @param lowclear: Low temperature alarm clear point, alarming when the temp lower than the highset, otherwise the alarm signal will be cleared.
   * @note The filled value should be an integer (range: -40 to 125 degrees Celsius), -40 to 257 (Fahrenheit)highset>highClear>lowclear>lowset)
   */
  //sht3x.setTemperatureLimitF(/*highset=*/35,/*highClear=*/34,/*lowSet=*/18,/*lowClear=*/20)
  if(sht3x.setTemperatureLimitC(/*highset=*/35,/*highClear=*/34,/*lowSet=*/18,/*lowClear=*/20) != 0){
    Serial.println("Failed to set the temperature limit");
  }
  /**
   * setHumidityLimitRH: Set the relative humidity threshold temperature and the alarm clear humidity(%RH)
   * @param highset: High humidity alarm point, when the humidity is greater than this value, the ALERT pin generates an alarm signal.
   * @param highClear: High humidity alarm clear point, alarming when the humidity higher than the highset, otherwise the alarm signal will be cleared.
   * @param lowset: Low humidity alarm point, when the humidity is lower than this value, the ALERT pin generates an alarm signal.
   * @param lowclear: Low humidity alarm clear point, alarming when the humidity lower than the highset, otherwise the alarm signal will be cleared.
   * @note The filled value should be an integer (range: 0 - 100 %RH,highset>highClear>lowclear>lowset) 
   */
  if(sht3x.setHumidityLimitRH(/*highset=*/70,/*highClear=*/68,/*lowSet=*/19,/*lowClear=*/20) != 0){
    Serial.println("Failed to set the humidity limit");
  }
  Serial.println("----------------------Alarm Detection-------------------------------");
  Serial.println("当温湿度超出阈值范围就会产生警报,使用时应当将ALERT与主控板中断引脚连接");
  Serial.println("-不同的主控：UNO(2),Mega2560(2),Leonardo(3),microbit(P0),掌控(P16)----");
  Serial.println("----------------------湿度限制(%RH)-----------------------------------");
  /**
   * @brief 测量相对湿度阈值温度和警报清除湿度
   * @return 返回true表示获取数据成功
   */
  if(sht3x.measureHumidityLimitRH()){
    Serial.print("high set:");
    //getHumidityHighSetRH():获取高湿度报警点
    Serial.print(sht3x.getHumidityHighSetRH());
    Serial.print("               low clear:");
    //getHumidityHighClearRH()：获取高湿度警报清除点
    Serial.println(sht3x.getHumidityLowClearRH());
    Serial.print("high clear:");
    //getHumidityLowClearRH()：获取低湿度警报清除点
    Serial.print(sht3x.getHumidityHighClearRH());
    Serial.print("               low set:");
    //getHumidityLowSetRH()：获取低湿度报警点
    Serial.println(sht3x.getHumidityLowSetRH());
  } else {
    Serial.println("获取湿度限制失败");
  }
  /**
   * measureTemperatureLimitC： 测量温度阈值温度和警报清除温度(°C)
   * measureTemperatureLimitF： 测量温度阈值温度和警报清除温度(°F)
   * @return 返回true表示获取数据成功
   */
  Serial.println("----------------------温度限制(°C)---------------------------------");
  //Serial.println("----------------------温度限制(°F)---------------------------------");
  if(sht3x.measureTemperatureLimitC()){
    Serial.print("high set:");
    //getTemperatureHighSetC()：获取高温报警点(°C)
    //getTemperatureHighSetF()：获取高温报警点(°F)
    Serial.print(sht3x.getTemperatureHighSetC());
    Serial.print("               low clear:");
    //getTemperatureHighClearC()：获取高温警报清除点(°C)
    //getTemperatureHighClearF()：获取高温警报清除点(°F))
    Serial.println(sht3x.getTemperatureLowClearC());
    Serial.print("high clear:");
    //getTemperatureLowClearC()：获取低温警报清除点(°C)
    //getTemperatureLowClearF()：获取低温警报清除点(°F)
    Serial.print(sht3x.getTemperatureHighClearC());
    Serial.print("               low set:");
    //getTemperatureLowSetC()：获取低温报警点(°C)
    //getTemperatureLowSetF()：获取低温报警点(°F)
    Serial.println(sht3x.getTemperatureLowSetC());
    Serial.println("------------------------------------------------------------------");
  } else {
    Serial.println("获取温度限制失败");
  }
  /**
   * readAlertState: 读取ALERT引脚的状态.
   * @return 高电平则返回1，低电平则返回0.
   */
  //此判断的作用是，初始化ALERT的状态
  if(sht3x.readAlertState() == 1){
    alertState = 1;
  } else {
    alertState = 0;
  }
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
  //读取数据的频率应该大于芯片采集数据的频率，否则返回的数据就会出错。
  if(alertState == 1){
    /**
     * @brief 判断温湿度超出阈值范围的情况 
     * @return 返回状态码,状态码代表含义如下：
     * @n 01 ：表示湿度超过下阈值范围
     * @n 10 ：表示温度超过下阈值范围
     * @n 11 ：表示温湿度都超过下阈值范围
     * @n 02 ：表示湿度超过上阈值范围
     * @n 20 ：表示温度超过上阈值范围
     * @n 22 ：表示温湿度都超过上阈值范围
     * @n 12 ：表示温度超过下阈值范围,湿度超过上阈值范围
     * @n 21 ：表示温度超过上阈值范围,湿度超过下阈值范围
     * @n 0  : 恢复正常，但警报还未消除
     */
    uint8_t state = sht3x.environmentState();
    if(state == 1)  Serial.println("湿度超过下阈值范围 !!!!!!!!!!!");
    else if(state == 10)  Serial.println("温度超过下阈值范围!!!!!!!!!!!");
    else if(state == 11)  Serial.println("温湿度超过下阈值范围!!!!!!!!!!!");
    else if(state == 2)  Serial.println("湿度超过上阈值范围!!!!!!!!!!!");
    else if(state == 20)  Serial.println("温度超过上阈值范围!!!!!!!!!!!");
    else if(state == 22)  Serial.println("温湿度超过上阈值范围!!!!!!!!!!!");
    else if(state == 12)  Serial.println("温度超过下阈值范围,湿度超过上阈值范围!!!!!!!!!!!");
    else if(state == 21)  Serial.println("温度超过上阈值范围,湿度超过下阈值范围!!!!!!!!!!!");
    else Serial.println("温湿度恢复正常，但警报还未消除!!!!!!!!!!!");
  } else {
    Serial.println("温湿度处于正常范围,警报已清除");
  }
  delay(1000);
}
