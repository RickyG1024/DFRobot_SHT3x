# DFRobot_SHT3x
The SHT3x series chips are used to measure ambient temperature and relative humidity (the degree of moisture in the air, which indicates the degree to which the moisture content in the atmosphere is saturated from the atmosphere). It is a successor to the SHT2x series that contain the low-cost version of the SHT30, the standard version of SHT31, and the professional version of SHT35.<br>
The SHT3x series temperature and humidity sensors adopt IIC communication which is easy to use, with a wide operating voltage range (2.15 to 5.5 V),<br>
and a space area of the chip package is 2.5 x 2.5 mm2 and with a height of 0.9 mm, which can help SHT3x integrated into a wide range of applications for a wide range of scenarios.<br>
Based on brand new optimized CMOSens® chip, SHT3x further improved product reliability and accuracy specifications.<br>
SHT3x offers a range of new features,e.g. Enhanced signal processing, two unique user-selectable I2C addresses, an alarm mode with programmable temperature and humidity limits, and communication speeds up to 1 MHz<br>
You can choose Open or Not Open the heater
     Heater Function<br>
       ①By comparing the relative humidity and temperature values measured before and after heating, it is possible to determine whether the sensor is working properly.<br>
       ②Use of heaters in wet environments to avoid sensor condensation<br>
       ③A heater is also required to measure the dew point temperature (the temperature at which water vapor in the air turns into dewdrops).<br>
The SHT3x chip offers two modes of operation:<br>
1. Single-measure mode with an idle state current of 0.2 mA and low power consumption (measurement data is 600 mA)<br>
2. Cycle measurement mode, where the idle state current is 45 mA, and in this mode ALERT starts to work(measurement data is 600 mA)<br>
The following is the typical measurement accuracy of the chip (followed by this temperature and humidity range):<br>

Version No.|Typical Temp Precision (°C)|Typical Humidity Precision(%RH)| Range(Temp/Humidity)
-----------|:-------------------------:|:-----------------------------:|:----------------------
SHT30      |     ±0.2  @0-65 °C        |        ±2 @10-90% RH          |  -40-125 °C/0-100 %RH 
SHT31      |     ±0.2  @0-90 °C        |        ±2 @0-100% RH          |  -40-125 °C/0-100 %RH 
SHT35      |     ±0.1  @20-60°C        |        ±1.5 @0-80% RH         |  -40-125 °C/0-100 %RH 

![SVG Figure](https://github.com/ouki-wang/DFRobot_Sensor/raw/master/resources/images/SEN0245svg1.png)

## Product Link（链接到英文商城）
    SKU: SHT3x Temperature and Humidity Sensor(以实际名称为准)
   
## Table of Contents

* [Summary](#summary)
* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary

   1.Read repeatability of the temperature and humidity data in single measurement mode, users can select the measure repeatability(the difference between the data measured by the chip under the same measurement conditions)<br>
     The higher difference, the smaller the difference and more dependable data.<br>
   2.Read repeatability of the temperature and humidity data in cycle measurement mode, users can select the measure repeatability and measure frequency(0.5Hz,1Hz,2Hz,4Hz,10Hz)<br>
   3.The user can customize the threshold range. The ALERT pin and the Arduino's interrupt pin can achieve the effect of the temperature and humidity threshold alarm<br>
## Installation

To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

## Methods

```C++
/*!
 * @brief Construct the function
 * @param pWire IIC bus pointer object and construction device, can both pass or not pass parameters, 
 * Wire in default.
 * @param address Chip IIC address, two optional addresses 0x44 and 0x45(0x45 in default).
 * @param RST RST Chip reset pin, 4 in default.
 * @n IIC address is determined by the pin addr on the chip.
 * @n When the ADR is connected to VDD, the chip IIC address is 0x45.
 * @n When the ADR is connected to GND, the chip IIC address is 0x44.
 */
DFRobot_SHT3x(TwoWire *pWire = &Wire, uint8_t address = 0x45,uint8_t RST = 4);

/**
 * @brief Read the serial number of the chip
 * @return Return 32-digit serial number
 */
uint32_t  readSerialNumber();

/**
 * @brief Initialize the function
 * @return Return 0 indicates a successful initialization, while other values indicates failure and
 * return to error code.
 */
int begin();

/**
 * @brief Send command resets via iiC, enter the chip's default mode single-measure mode, 
 * turn off the heater, and clear the alert of the ALERT pin.
 * @return Read the status register to determine whether the command was executed successfully, 
 * and returning true indicates success
 */
bool softReset();

/**
 * @brief Reset through the chip's reset pin, enter the chip's default mode single-measure mode, and clear the alert of the ALERT pin.
 * @return The status register has a data bit that detects whether the chip has been reset, and returning true indicates success
 */
bool pinReset();
/**
 * @brief Get temperature and humidity data in single measurement mode.
 * @param repeatability Set repeatability to read temperature and humidity data with the type eRepeatability_t.
 * @return Return a structure containing celsius temperature (°C), Fahrenheit temperature (°F), relative humidity
 * (%RH), status code
 * @n A status of 0 indicates the right return data.
 */

sRHAndTemp_t readTemperatureAndHumidity(eRepeatability_t repeatability );

/**
 * @brief Get the measured temperature (in degrees Celsius)
 * @return Return the float temperature data 
 */
float getTemperatureC();

/**
 * @brief Get the measured temperature (in degrees Fahrenheit)
 * @return Return the float temperature data 
 */
float getTemperatureF();

/**
 * @brief Get measured humidity(%RH)
 * @return Return the float humidity data
 */
float getHumidityRH();

/**
 * @brief Enter cycle measurement mode and set repeatability(the difference between the data measured 
 * the difference between the data measured by the chip under the same measurement conditions)
 * @param measureFreq  Read the eMeasureFrequency_t data frequency
 * @param repeatability Set repeatability to read temperature and humidity data with the type eRepeatability_t. 
 * eRepeatability_High(high repeatability mode) in default.
 * @return Return true indicates a successful entrance to cycle measurement mode.
 */
bool startPeriodicMode(eMeasureFrequency_t measureFreq,eRepeatability_t repeatability = eRepeatability_High);

/**
 * @brief Get temperature and humidity data in cycle measurement mode.
 * @return Return a structure containing celsius temperature (°C), Fahrenheit temperature (°F), relative humidity 
 * (%RH), status code.
 * @n A status of 0 indicates the right return data.
 */
sRHAndTemp_t readTemperatureAndHumidity();

/**
 * @brief Exit from cycle measurement mode
 * @return Read the status of the register to determine whether the command was executed successfully,
 * and returning true indicates success
 */
bool stopPeriodicMode();

/**
 * @brief Turn on the heater inside the chip
 * @return Read the status of the register to determine whether the command was executed successfully,
 * and returning true indicates success
 * @note Heaters should be used in wet environments, and other cases of use will result in incorrect
 * readings
 */
bool heaterEnable();

/**
 * @brief Turn off the heater inside the chip
 * @return Read the status of the register to determine whether the command was executed successfully,
 * and returning true indicates success
 * @note Heaters should be used in wet environments, and other cases of use will result in incorrect readings
   */

bool heaterDisable();

/**
 * @brief All flags (Bit 15, 11, 10, 4) in the status register can be cleared (set to zero)
 * @n  把bit：15 设置为0后ALERT引脚才能正常工作，否则将一直处于高电平。
 */
void clearStatusRegister();

/**
 * @brief 读取ALERT引脚的状态.
 * @return 高电平则返回1，低电平则返回0.
 */
bool readAlertState();

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
 */
uint8_t environmentState();

/**
 * @brief 设置温度阈值温度和警报清除温度(°C)
 * @param highset 高温报警点，当温度大于此值时ALERT引脚产生报警信号。
 * @param highClear 高温警报清除点，当温度大于highset产生报警信号，而温度小于此值报警信号则被清除。
 * @param lowset 低温报警点，当温度小于此值时ALERT引脚产生报警信号。
 * @param lowclear 低温警报清除点，当温度小于lowset产生报警信号，而温度大于此值时报警信号则被清除
 * @note 范围：-40 到 125 ,highset>highClear>lowclear>lowset。 
 * @return 返回0则表示设置成功.
 */
uint8_t  setTemperatureLimitC(float highset,float highclear,float lowset,float lowclear);

/**
 * @brief 设置温度阈值温度和警报清除温度(°F)
 * @param highset 高温报警点，当温度大于此值时ALERT引脚产生报警信号。
 * @param highClear 高温警报清除点，当温度大于highset产生报警信号，而温度小于此值报警信号则被清除。
 * @param lowset 低温报警点，当温度小于此值时ALERT引脚产生报警信号。
 * @param lowclear 低温警报清除点，当温度小于lowset产生报警信号，而温度大于此值时报警信号则被清除。
 * @note 范围：-40 到 257 ,highset>highClear>lowclear>lowset。 
 * @return 返回0则表示设置成功.
 */
uint8_t  setTemperatureLimitF(float highset,float highclear, float lowset,float lowclear);
/**
 * @brief 设置相对湿度阈值温度和警报清除湿度(%RH)
 * @param highset 高湿度报警点，当相对湿度大于此值时ALERT引脚产生报警信号。
 * @param highClear 高湿度警报清除点，当相对湿度大于highset产生报警信号，而相对湿度小于此值报警信号则被清除。
 * @param lowset 低湿度报警点，当相对湿度小于此值时ALERT引脚产生报警信号。
 * @param lowclear 低湿度警报清除点，当相对湿度小于lowset产生报警信号，而相对湿度大于此值时报警信号则被清除。
 * @note 范围：0 - 100 %RH,highset>highClear>lowclear>lowset。
 * @return 返回0则表示设置成功.
 */
uint8_t setHumidityLimitRH(float highset,float highclear, float lowset,float lowclear);

/**
 * @brief 测量温度阈值温度和警报清除温度
 * @return 返回true 表示数据获取成功
 */
bool measureTemperatureLimitC();

/**
 * @brief 获取高温报警点温度(°C)
 * @return 返回高温报警点温度
 */
float getTemperatureHighSetC();

/**
 * @brief 获取高温警报清除点温度(°C)
 * @return 返回高温警报清除点温度
 */
float getTemperatureHighClearC();

/**
 * @brief 获取低温警报清除点温度(°C)
 * @return 返回低温警报清除点温度
 */
float getTemperatureLowClearC();

/**
 * @brief 获取低温报警点温度(°C)
 * @return 返回低温报警点温度
 */
float getTemperatureLowSetC();

/**
 * @brief 测量相对湿度阈值温度和警报清除湿度
 * @return 返回true 表示数据获取成功
 */
bool measureTemperatureLimitF();

/**
 * @brief 获取高温报警点温度(°F)
 * @return 返回高温报警点温度
 */
float getTemperatureHighSetF();

/**
 * @brief 获取高温警报清除点温度(°F)
 * @return 返回高温警报清除点温度
 */
float getTemperatureHighClearF();

/**
 * @brief 获取低温警报清除点温度(°F)
 * @return 返回低温警报清除点温度
 */
float getTemperatureLowClearF();

/**
 * @brief 获取低温报警点温度(°F)
 * @return 返回低温报警点温度
 */
float getTemperatureLowSetF();

/**
 * @brief 读取相对湿度阈值温度和警报清除湿度
 * @return 返回true 表示数据获取成功
 */
bool measureHumidityLimitRH();

/**
 * @brief 获取高湿度报警点湿度(%RH)
 * @return 返回高湿度报警点湿度
 */
float getHumidityHighSetRH();

/**
 * @brief 获取高湿度警报清除点湿度(%RH)
 * @return 返回高湿度警报清除点湿度
 */
float getHumidityHighClearRH();

/**
 * @brief 获取低湿度警报清除点湿度(%RH)
 * @return 返回低湿度警报清除点湿度
 */
float getHumidityLowClearRH();

/**
 * @brief 获取低湿度报警点湿度(v)
 * @return 返回低湿度报警点湿度
 */
float getHumidityLowSetRH();

```

## Compatibility

MCU                | Work Well    | Work Wrong   | Untested    | Remarks
------------------ | :----------: | :----------: | :---------: | -----
Arduino Uno        |      √       |              |             | 
Mega2560        |      √       |              |             | 
Leonardo        |      √       |              |             | 
ESP32        |      √       |              |             | 
micro:bit        |      √       |              |             | 


## History

- data 2019-8-25
- version V0.1


## Credits

Written by fengli(li.feng@dfrobot.com), 2019.8.25 (Welcome to our [website](https://www.dfrobot.com/))





