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
 * @brief 构造函数
 * @param pWire IIC总线指针对象，构造设备，可传参数也可不传参数，默认Wire。
 * @param address 芯片IIC地址,共有两个可选地址0x44、0x45(默认为0x45)。
 * @param RST 芯片复位引脚，默认为4.
 * @n IIC地址是由芯片上的引脚addr决定。
 * @n 当ADR与VDD连接,芯片IIC地址为：0x45。
 * @n 当ADR与GND连接,芯片IIC地址为：0x44。
 */
//DFRobot_SHT3x sht3x(&Wire,/*address=*/0x45,/*RST=*/4);

DFRobot_SHT3x sht3x;

void setup() {

  Serial.begin(9600);
    //初始化芯片,检测是否能正常通信
  while (sht3x.begin() != 0) {
    Serial.println("初始化芯片失败，请确认芯片连线是否正确");
    delay(1000);
  }
  
  /**
   * readSerialNumber:读取芯片的序列号
   * @return 返回32位序列号
   */
  Serial.print("芯片序列号：");
  Serial.println(sht3x.readSerialNumber());
  /**
   * softReset：通过IIC发送命令复位，进入芯片的默认模式单次测量模式，关闭加热器，并清除ALERT引脚的警报。
   * @return 通过读取状态寄存器来判断命令是否成功被执行，返回true则表示成功
   */
   if(!sht3x.softReset()){
     Serial.println("芯片复位失败....");
   }

  /**
   *  pinReset：通过芯片的复位引脚进行复位，进入芯片的默认模式单次测量模式，关闭加热器，并清除ALERT引脚的警报。
   * @return 状态寄存器有一数据位能检测芯片是否进行了复位，返回true则表示成功
   * @note 在使用此API时需要将芯片的复位引脚nRESET与arduino上RST(默认为 pin4)相连.
   */
  //if(!sht3x.pinReset()){
    //Serial.println("芯片复位失败....");
  //}

  /**
   * heaterEnable()： 打开芯片里面的加热器.作用是使传感器在潮湿的环境也能有准确的湿度数据
   * @return 通过读取状态寄存器来判断命令是否成功被执行，返回true则表示成功
   * @note 加热器的使用条件，应是在潮湿环境时，若正常情况下使用则会造成读数不准.
   */
  //if(!sht3x.heaterEnable()){
    // Serial.println("加热器打开失败....");
  //}
  /**
   * startPeriodicMode ：进入周期测量模式，并设置可重复性、读取频率。
   * @param measureFreq   读取数据的频率，eMeasureFrequency_t类型的数据
   * @note  可选择的参数：
               eMeasureFreq_Hz5,   /**芯片每2秒采集一次数据
               eMeasureFreq_1Hz,   /**芯片每1秒采集一次数据
               eMeasureFreq_2Hz,   /**芯片每0.5秒采集一次数据
               eMeasureFreq_4Hz,   /**芯片每0.25采集一次数据
               eMeasureFreq_10Hz   /**芯片每0.1采集一次数据
   * @param repeatability 读取温湿度数据的可重复性，默认为eRepeatability_High.
   * @note  可选择的参数：
               eRepeatability_High /**高可重复性模式下，湿度的可重复性为0.10%RH，温度的可重复性为0.06°C
               eRepeatability_Medium,/**中等可重复性模式下，湿度的可重复性为0.15%RH，温度的可重复性为0.12°C
               eRepeatability_Low, /**低可重复性模式下，湿度的可重复性为0.25%RH，温度的可重复性为0.24°C
   * @return 通过读取状态寄存器来判断命令是否成功被执行，返回true则表示成功
   */          
  if(!sht3x.startPeriodicMode(sht3x.eMeasureFreq_1Hz)){
    Serial.println("进入周期模式失败...");
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
