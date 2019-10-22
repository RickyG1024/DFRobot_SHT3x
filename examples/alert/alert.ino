/*!
 * @file alert.ino
 * @brief 温湿度超阈值报警
 * @n 实验现象:用户自定义设置温度和湿度的阈值，当温湿度超出了自定义的阈值时，ALERT引脚就会产生报警信号
 * @n 使用注意：在使用此功能时应当将传感器上的ALERT引脚与主控板上的中断引脚相连
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-08-26
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
//alert引脚的非报警状态为低电平
volatile  int alertState = 0;
void alert(){
  alertState = 1 - alertState;
}
void setup() {
  Serial.begin(9600);
  #ifdef ARDUINO_ARCH_MPYTHON 
  /* esp32 中断引脚与终端号码对应关系表
   * -------------------------------------------------------------------------------------
   * |                    |  DigitalPin  |        P0~P20均可作为外部中断使用             |
   * |    esp32            |--------------------------------------------------------------|
   * |                    | Interrupt No |  可用digitalPinToInterrupt(Pn) 查询中断号     |
   * |-----------------------------------------------------------------------------------|
   */
  attachInterrupt(digitalPinToInterrupt(P16)/*查询P16引脚的中断号*/,alert,CHANGE);//开启esp32的P16引脚的外部中断，双边沿触发，ALERT连接P16
  #else
  /* AVR系列Arduino 中断引脚与终端号码对应关系表
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
  /* microbit 中断引脚与终端号码对应关系表
   * ---------------------------------------------------------------------------------------------------------------
   * |                                                   |  DigitalPin  |    P0~P20均可作为外部中断使用            |
   * |                  microbit                         |---------------------------------------------------------|
   * |(作为外部中断时，无需用pinMode将其设置为输入模式)  | Interrupt No | 中断号即引脚数字值，如P0中断号为0，P1为1 |
   * |-------------------------------------------------------------------------------------------------------------|
   */
  attachInterrupt(/*中断号*/0,alert,CHANGE);//开启外部中断0,ALERT连接至主控的数字引脚上：UNO(2),Mega2560(2),Leonardo(3),microbit(P0)
  #endif
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
   * @brief All flags (Bit 15, 11, 10, 4) in the status register can be cleared (set to zero)
   * @n  把bit：15 设置为0后ALERT引脚才能正常工作，否则将一直处于高电平。
   */
  sht3x.clearStatusRegister();
  /**
   * startPeriodicMode ：进入周期测量模式，并设置可重复性、读取频率，只有在此模式下ALERT才能工作。
   * @param measureFreq   读取数据的频率，eMeasureFrequency_t类型的数据
   * @note  可选择的参数：
               eMeasureFreq_Hz5,   /**芯片每2秒采集一次数据
               eMeasureFreq_1Hz,   /**芯片每1秒采集一次数据
               eMeasureFreq_2Hz,   /**芯片每0.5秒采集一次数据
               eMeasureFreq_4Hz,   /**芯片每0.25采集一次数据
               eMeasureFreq_10Hz   /**芯片每0.1采集一次数据 
   * @param repeatability 读取温湿度数据的可重复性，默认参数为 eRepeatability_High.
   * @note  可选择的参数：
               eRepeatability_High /**高可重复性模式下，湿度的可重复性为0.10%RH，温度的可重复性为0.06°C
               eRepeatability_Medium,/**中等可重复性模式下，湿度的可重复性为0.15%RH，温度的可重复性为0.12°C
               eRepeatability_Low, /**低可重复性模式下，湿度的可重复性为0.25%RH，温度的可重复性为0.24°C
   * @return 通过读取状态寄存器来判断命令是否成功被执行，返回true则表示成功
   */
  if(!sht3x.startPeriodicMode(sht3x.eMeasureFreq_10Hz)){
    Serial.println("进入周期模式失败...");
  }
  /**
   * setTemperatureLimitC:设置温度阈值温度和警报清除温度(°C)
   * setTemperatureLimitF:设置温度阈值温度和警报清除温度(°F)
   * @param highset 高温报警点，当温度大于此值时ALERT引脚产生报警信号。
   * @param highClear 高温警报清除点，当温度大于highset产生报警信号，而温度小于此值报警信号则被清除。
   * @param lowset 低温报警点，当温度小于此值时ALERT引脚产生报警信号。
   * @param lowclear 低温警报清除点，当温度小于lowset产生报警信号，而温度大于此值时报警信号则被清除。
   * @note 填入的数值应该为整数(范围：-40 到 125(摄氏度),：-40 到 257(华氏度)highset>highClear>lowclear>lowset)。 
   */
  //sht3x.setTemperatureLimitF(/*highset=*/35,/*highClear=*/34,/*lowSet=*/18,/*lowClear=*/20)
  if(sht3x.setTemperatureLimitC(/*highset=*/35,/*highClear=*/34,/*lowSet=*/18,/*lowClear=*/20) != 0){
    Serial.println("温度限制设置失败...");
  }
  /**
   * setHumidityLimitRH: 设置相对湿度阈值温度和警报清除湿度(%RH)
   * @param highset 高湿度报警点，当相对湿度大于此值时ALERT引脚产生报警信号。
   * @param highClear 高湿度警报清除点，当相对湿度大于highset产生报警信号，而相对湿度小于此值报警信号则被清除。
   * @param lowset 低湿度报警点，当相对湿度小于此值时ALERT引脚产生报警信号。
   * @param lowclear 低湿度警报清除点，当相对湿度小于lowset产生报警信号，而相对湿度大于此值时报警信号则被清除。
   * @note 填入的数值应该为整数(范围：0 - 100 %RH,highset>highClear>lowclear>lowset)。 
   */
  if(sht3x.setHumidityLimitRH(/*highset=*/70,/*highClear=*/68,/*lowSet=*/19,/*lowClear=*/20) != 0){
    Serial.println("湿度限制设置失败...");
  }
  Serial.println("----------------------警报检测----------------------------------------");
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