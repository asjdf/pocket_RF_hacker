#include "RCSwitch.h"//https://github.com/sui77/rc-switch
RCSwitch mySwitch = RCSwitch();
#include "cc1101.h"
CC1101 cc1101;

void setfreqx(unsigned long int freq) {
    unsigned long freqnum = freq / 396.734569;
    //Cube_PRINT.println(freqnum,HEX);
    byte freqx[3];
    freqx[0] = freqnum;
    freqx[1] = freqnum >> 8;
    freqx[2] = freqnum >> 16;
    //Cube_PRINT.print( freqx[2],HEX );Cube_PRINT.print(" " );Cube_PRINT.print( freqx[1],HEX);Cube_PRINT.print(" " );Cube_PRINT.print( freqx[0],HEX);Cube_PRINT.println(" " );
    cc1101.writeReg(CC1101_FREQ2, freqx[2]);
    cc1101.writeReg(CC1101_FREQ1, freqx[1]);
    cc1101.writeReg(CC1101_FREQ0, freqx[0]);
}
void setfreqx(int FREQ2, int FREQ1, int FREQ0) {
    cc1101.writeReg(CC1101_FREQ2, FREQ2);
    cc1101.writeReg(CC1101_FREQ1, FREQ1);
    cc1101.writeReg(CC1101_FREQ0, FREQ0);
}

void cc1101_config() {
    cc1101.SS_PIN = RF433_CS;
    cc1101.init();
    //cc1101.setSyncWord(syncWord);
    setfreqx(freq_cc1101);
    cc1101.disableAddressCheck();
    //cc1101.cmdStrobe(CC1101_SRX);
    cc1101.writeReg(CC1101_IOCFG2, 0x0d);

    //  cc1101.writeReg(CC1101_MDMCFG4, 0xF7);
    //cc1101.writeReg(CC1101_MDMCFG4, 0x87);//RFB 200K
    cc1101.writeReg(CC1101_MDMCFG4, 0x07);//RFB 200K
    cc1101.writeReg(CC1101_MDMCFG3, 0x93);
    //cc1101.writeReg(CC1101_AGCCTRL2, 0xB0);
    cc1101.cmdStrobe(CC1101_SRX); //F5 43
}

void Fixed_Config() {
  cc1101.SS_PIN = RF315_CS;
  cc1101.init();
  byte syncWord[2] = {0xFF, 0xFF};
  //  cc1101.setSyncWord(syncWord);
  cc1101.setSyncWord(syncWord);
  //cc1101.setCarrierFreq(CFREQ_315);
  setfreqx(freq_cc1101);
  cc1101.disableAddressCheck();
  cc1101.cmdStrobe(CC1101_STX);
  cc1101.writeReg(CC1101_IOCFG2, 0x0d);
}

void Fixed_Config_433() {
  cc1101.SS_PIN = RF315_CS;
  cc1101.init();
  byte syncWord[2] = {0xFF, 0xFF};
  cc1101.setSyncWord(syncWord);
  //cc1101.setCarrierFreq(CFREQ_433);
  setfreqx(433750000);
  cc1101.disableAddressCheck();
  cc1101.cmdStrobe(CC1101_STX);
  cc1101.writeReg(CC1101_IOCFG2, 0x0d);
  //cc1101.writeReg(CC1101_AGCCTRL2, 0xB0);
}


void cc1101_Setup() {
  //class:fixed,freq:315
  String set_class = Cube_PRINT_data.substring(Cube_PRINT_data.indexOf("class") + 6, Cube_PRINT_data.indexOf(",freq"));
  freq_cc1101 = strtoul(Cube_PRINT_data.substring(Cube_PRINT_data.indexOf("freq") + 5, Cube_PRINT_data.length()).c_str(), NULL, 10);

  if (set_class == "fixed") {
    rf_class = 1;
    detachInterrupt(1);
    cc1101.cmdStrobe(CC1101_SIDLE);
    setfreqx(freq_cc1101);
    cc1101.cmdStrobe(CC1101_SRX);
    mySwitch.enableReceive(1);//设置mySwitch中断引脚，用于接收固定码信号
    Cube_PRINT.println("*RF_sniffer_fixed");
  }
}

void SnifferFixedCode() {
    if (mySwitch.available()) {
        if (millis() - SerialLastTime < Print_Time) {
            mySwitch.resetAvailable();
            return;
        }
        Cube_PRINT.print("[RF][Sniffer]freq:");
        Cube_PRINT.print(String(freq_cc1101).substring(0, 3));
        Cube_PRINT.print(",protocol:Fixed-");
        Cube_PRINT.print(mySwitch.getReceivedProtocol());
        Cube_PRINT.print(",modulation:ask,func:");
        Cube_PRINT.print(mySwitch.getReceivedValue() & 0xf, HEX);
        Cube_PRINT.print(",data:");
        Cube_PRINT.println(mySwitch.getReceivedValue() >> 4, HEX);
        //String data1=String(mySwitch.getReceivedValue()>>4);
        //Cube_PRINT.println(strtoul(data1.c_str(),NULL,16));
        mySwitch.resetAvailable();
        SerialLastTime = millis();
    }
}

int Fixed_Transmitpin = RF315_GDO0;

void Fixed_Bit1() {
  //Cube_PRINT.print("1");
  digitalWrite(Fixed_Transmitpin, HIGH);
  delayMicroseconds(915);
  digitalWrite(Fixed_Transmitpin, LOW);
  delayMicroseconds(305);
  digitalWrite(Fixed_Transmitpin, LOW);
}
void Fixed_Bit0() {
  //Cube_PRINT.print("0");
  digitalWrite(Fixed_Transmitpin, HIGH);
  delayMicroseconds(305);
  digitalWrite(Fixed_Transmitpin, LOW);
  //delayMicroseconds(670);
  delayMicroseconds(915);
  digitalWrite(Fixed_Transmitpin, LOW);
}
void Class2_Transmit(unsigned long data, int Func) {

  for (int a = 20; a > 0; a--) {
    if (data >> a - 1 & 1 == 1) {
      Fixed_Bit1();
    } else {
      Fixed_Bit0();
    }

  }
  for (int l = 4; l > 0; l--) {
    if (Func >> l - 1 & 1 == 1) {
      Fixed_Bit1();
    } else {
      Fixed_Bit0();
    }
  }
  Fixed_Bit0();
  //digitalWrite(Fixed_Transmitpin,LOW);
  //Cube_PRINT.println("");
}

void RF_Transmit() {
  long freq = strtoul(Cube_PRINT_data.substring(14, 17).c_str(), NULL, 10);
  int func = strtoul(Cube_PRINT_data.substring(18, 19).c_str(), NULL, 16);
  long Serialnumber = strtoul(Cube_PRINT_data.substring(20, Cube_PRINT_data.length()).c_str(), NULL, 16);
  if (freq == 315) {
    Fixed_Config();
    //Serial.println("315Transmit");
  } else if (freq == 433) {
    Fixed_Config_433();
    //Serial.println("433Transmit");
  }
  for (int i = 0; i < 8; i++) {
    Class2_Transmit(Serialnumber, func);
    delay(9);
  }

  Cube_PRINT.print("*Transmit ");
  Cube_PRINT.print("Freq:");
  Cube_PRINT.print(freq);
  Cube_PRINT.print("Func:");
  Cube_PRINT.print(func, HEX);
  Cube_PRINT.print("Data:");
  Cube_PRINT.println(Serialnumber, HEX);
  cc1101_config();
}