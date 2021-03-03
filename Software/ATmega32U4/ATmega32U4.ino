long freq_cc1101 = 315000000;  //设置CC1101默认监听频率
// long freq_cc1101 = 434000000; //设置CC1101默认监听频率
int rf_class = 1;  //设置射频监听模式为固定码
#define Cube_PRINT Serial
#define Print_Time 350
unsigned long SerialLastTime;
String Cube_PRINT_data;
#include "Pin.h"
#include "rf.h"
#include "SerialCommunication.h"
void pinSetup(){
	pinMode(nRF_CS_1, OUTPUT);
	pinMode(nRF_CE,OUTPUT);

	pinMode(RF433_CS, OUTPUT);
	pinMode(RF433_GDO0,OUTPUT);
	pinMode(RF433_GDO2, INPUT);
	
	digitalWrite(RF433_CS, HIGH);
	digitalWrite(RF433_GDO0, LOW);
	digitalWrite(RF315_CS, HIGH);
	digitalWrite(RF315_GDO0, LOW);

	pinMode(RF315_CS, OUTPUT);
	pinMode(RF315_GDO0,OUTPUT);
	pinMode(RF315_GDO2, INPUT);

	pinMode(RF_EN, OUTPUT);
	digitalWrite(RF_EN, HIGH);

	pinMode(ID_OUT, INPUT);
	pinMode(MOD, OUTPUT);
	pinMode(SHD, OUTPUT);

	digitalWrite(MOD,LOW);
	digitalWrite(SHD,LOW);
}
void setup() {
    Cube_PRINT.begin(9600);  //设置与ESP8266通信引脚
    delay(2000);
    pinSetup();  //初始化模块引脚
    delay(100);
    Cube_PRINT.println("Cubemini Runing...");
    cc1101_config();  //配置CC1101 模块，默认监听freq_cc1101 变量中的频率
    delay(4);
    cc1101.PrintConfig();  //输出CC 1101关键几个寄存器
    mySwitch.enableReceive(1);  //设置mySwitch中断引脚，用于接收固定码信号
                                // rf_class=2;
    // RF433_Setup_rollcode();//滚动码CC1101 寄存器配置
    // cc1101_config();
    // Hackradio.enableReceive(RF315_att);//设置中断引脚，接收滚动码信号
}
void loop() {
    SerialCommunication();
    SnifferFixedCode();  //嗅探固定码数据
}
