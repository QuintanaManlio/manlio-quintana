/*****************************************************************************************************
    Dummy Engine V1.0 Arduino Code

    Created by: Fabián Castillo, Sergio Sanoja, Abraham Saavedra, Samir Hernández, Manlio Quintana
    ITESM CEM

    MTC GENERAC
******************************************************************************************************/

// 12C Library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// CAN Module Library
#include <mcp_can.h> // This library can be found at: https://github.com/coryjfowler/MCP_CAN_lib
#include <SPI.h>

// CAN J1939 pin buttons
//#define BREPower 0
//#define BFuelTemp 1
#define BCoolantLevel 2
#define BEOilLevel 3
#define BECTemp 4
#define BEOilTemp 5
#define BCoolantPres 6
#define BEOilPres 7
#define BESpeed 8
#define BReturn 9
#define Pot A0

// Variables
//int REPower1;
//int REPower2;
//int UnitNum;
int ESpeed;
int ECTemp;
int CoolantLevel;
int EOilPres;
int FuelTemp;
int EOilTemp;
int EOilLevel;
int CoolantPres;

unsigned long TiempoPGN_65262 = 0;
unsigned long TiempoPGN_65263 = 0;
unsigned long TiempoPGN_61444 = 0;

int ContadorESpeed = 0;

//int MarcaREPower1Basic;
//int MarcaREPower2Basic;
//int MarcaUnitNumBasic;
int ECTempBasic;
int FuelTempBasic;
int EOilTempBasic;
int CoolantLevelBasic;
int EOilPresBasic;
int EOilLevelBasic;
int CoolantPresBasic;
int ESpeedBasic;

//int MarcaREPower1;
//int MarcaREPower2;
//int MarcaUnitNum;
int MarcaECTemp;
int MarcaCoolantLevel;
int MarcaEOilPres;
int MarcaESpeed;
int MarcaFuelTemp;
int MarcaEOilTemp;
int MarcaEOilLevel;
int MarcaCoolantPres;

//unsigned char NREPower1;
//unsigned char NREPower2;
//unsigned char NUnitNum;
unsigned char NECTemp;
unsigned char NCoolantLevel;
unsigned char NEOilPres;
unsigned char NESpeed1;
unsigned char NESpeed2;
unsigned char NFuelTemp;
unsigned char NEOilTemp1;
unsigned char NEOilTemp2;
unsigned char NEOilLevel;
unsigned char NCoolantPres;

// Location of each PGN and its variables
//byte PGN_65214[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 0x  // REPower (2bytes, byte )
//byte PGN_65259[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 0xCFEEB00  // UnitNum (0-200char, byte 8)
byte PGN_65262[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 0xCFEEE00  // ECTemp (1byte, byte 1), FuelTemp (1byte, byte 2), EOilTemp (2bytes, byte 3 & 4)
byte PGN_65263[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 0xCFEEF00  // CoolantLevel (1byte, byte 7), EOilPres (1byte, byte 4), EOilLevel (1byte, byte 2), CoolantPres (1byte, byte 6)
byte PGN_61444[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 0xCF00400  // ESpeed (2bytes, byte 4 & 5)

byte sndStat;

// Locates the 12C direction in 0x27 and sets de display to a 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Definition of CS pin to pin 10 of the Arduino
MCP_CAN CAN0(10);

////////////////////////////////////////////////////////////////////////////////

void setup() {
  // Definition of pin buttons and potentiometer as inputs
  //pinMode(BREPower, INPUT);
  //pinMode(BFuelTemp, INPUT);
  pinMode(BECTemp, INPUT);
  pinMode(BCoolantLevel, INPUT);
  pinMode(BEOilPres, INPUT);
  pinMode(BESpeed, INPUT);
  pinMode(BEOilTemp, INPUT);
  pinMode(BEOilLevel, INPUT);
  pinMode(BCoolantPres, INPUT);
  pinMode(BReturn, INPUT);
  pinMode(Pot, INPUT);

  // Initialize serial monitor with abaudrate of 9600
  Serial.begin(9600);

  // LCD configuration
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dummy Engine V1 ");
  lcd.setCursor(2, 1);
  lcd.print("MTC Generac");

  // Initialize MCP2515 running at 8MHz with a baudrate of 250kb/s and the masks and filters disabled
  if (CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully!");
  }
  else {
    Serial.println("Error Initializing MCP2515...");
  }

  // Change to normal mode to allow messages to be transmitted
  CAN0.setMode(MCP_NORMAL);

  delay(5000);
}

////////////////////////////////////////////////////////////////////////////////

void loop() {
  // Sends the specific data through their specific direction
  if (millis() >= TiempoPGN_65262 + 1000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Select parameter");
    lcd.setCursor(0, 1);
    lcd.print("to simulate");
    sndStat = CAN0.sendMsgBuf(0xCFEEE00, 1, 8, PGN_65262);
    TiempoPGN_65262 += 1000;
  }
  if (millis() >= TiempoPGN_65263 + 500) {
    sndStat = CAN0.sendMsgBuf(0xCFEEF00, 1, 8, PGN_65263);
    TiempoPGN_65263 += 500;
  }
  if (millis() >= TiempoPGN_61444 + 100) {
    sndStat = CAN0.sendMsgBuf(0xCF00400, 1, 8, PGN_61444);
    TiempoPGN_61444 += 100;
  }

  //////////////////////////////////////////////////////////////////
  ////////    Simulation for Engine Coolant Temperature     ////////

  if (digitalRead(BECTemp) == HIGH) {
    MarcaECTemp = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Eng. Cool. Temp.");
    while (MarcaECTemp == 1) {
      ECTempBasic = map(analogRead(Pot), 0, 1023, 0, 250);
      NECTemp = ECTempBasic;
      ECTemp = ECTempBasic - 40;
      PGN_65262[0] = NECTemp;
      if (millis() >= TiempoPGN_65262 + 1000) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Eng. Cool. Temp.");
        lcd.setCursor(0, 1);
        lcd.print(ECTemp);
        lcd.setCursor(4, 1);
        lcd.print("deg C");
        sndStat = CAN0.sendMsgBuf(0xCFEEE00, 1, 8, PGN_65262);
        TiempoPGN_65262 += 1000;
      }
      if (millis() >= TiempoPGN_65263 + 500) {
        sndStat = CAN0.sendMsgBuf(0xCFEEF00, 1, 8, PGN_65263);
        TiempoPGN_65263 += 500;
      }
      if (millis() >= TiempoPGN_61444 + 100) {
        sndStat = CAN0.sendMsgBuf(0xCF00400, 1, 8, PGN_61444);
        TiempoPGN_61444 += 100;
      }
      if (digitalRead(BReturn) == HIGH) {
        MarcaECTemp = 0;
      }
    }
  }

  //////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////
  ////////         Simulation for Fuel Temperature          ////////

  //  if (digitalRead(BFuelTemp) == HIGH) {
  //    MarcaFuelTemp = 1;
  //    lcd.clear();
  //    lcd.setCursor(0, 0);
  //    lcd.print("Fuel Temperature");
  //    while (MarcaFuelTemp == 1) {
  //      FuelTempBasic = map(analogRead(Pot), 0, 1023, 0, 250);
  //      NFuelTemp = FuelTempBasic;
  //      FuelTemp = FuelTempBasic - 40;
  //      PGN_65262[1] = NFuelTemp;
  //      if (millis() >= TiempoPGN_65262 + 1000) {
  //        lcd.clear();
  //        lcd.setCursor(0, 0);
  //        lcd.print("Fuel Temperature");
  //        lcd.setCursor(0, 1);
  //        lcd.print(FuelTemp);
  //        lcd.setCursor(4, 1);
  //        lcd.print("deg C");
  //        sndStat = CAN0.sendMsgBuf(0xCFEEE00, 1, 8, PGN_65262);
  //        TiempoPGN_65262 += 1000;
  //      }
  //      if (millis() >= TiempoPGN_65263 + 500) {
  //        sndStat = CAN0.sendMsgBuf(0xCFEEF00, 1, 8, PGN_65263);
  //        TiempoPGN_65263 += 500;
  //      }
  //      if (millis() >= TiempoPGN_61444 + 100) {
  //        sndStat = CAN0.sendMsgBuf(0xCF00400, 1, 8, PGN_61444);
  //        TiempoPGN_61444 += 100;
  //      }
  //      if (digitalRead(BReturn) == HIGH) {
  //        MarcaFuelTemp = 0;
  //      }
  //    }
  //  }

  //////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////
  ////////      Simulation for Engine Oil Temperature       ////////

  if (digitalRead(BEOilTemp) == HIGH) {
    MarcaEOilTemp = 1;
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Eng. Oil Temp.");
    while (MarcaEOilTemp == 1) {
      EOilTempBasic = map(analogRead(Pot), 0, 1023, 0, 64200);
      EOilTemp = map(analogRead(Pot), 0, 1023, -273, 1735);
      NEOilTemp1 = lowByte(EOilTempBasic);
      NEOilTemp2 = highByte(EOilTempBasic);
      PGN_65262[2] = NEOilTemp1;
      PGN_65262[3] = NEOilTemp2;
      if (millis() >= TiempoPGN_65262 + 1000) {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Eng. Oil Temp.");
        lcd.setCursor(0, 1);
        lcd.print(EOilTemp);
        lcd.setCursor(5, 1);
        lcd.print("deg C");
        sndStat = CAN0.sendMsgBuf(0xCFEEE00, 1, 8, PGN_65262);
        TiempoPGN_65262 += 1000;
      }
      if (millis() >= TiempoPGN_65263 + 500) {
        sndStat = CAN0.sendMsgBuf(0xCFEEF00, 1, 8, PGN_65263);
        TiempoPGN_65263 += 500;
      }
      if (millis() >= TiempoPGN_61444 + 100) {
        sndStat = CAN0.sendMsgBuf(0xCF00400, 1, 8, PGN_61444);
        TiempoPGN_61444 += 100;
      }
      if (digitalRead(BReturn) == HIGH) {
        MarcaEOilTemp = 0;
      }
    }
  }

  //////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////
  ////////          Simulation for Coolant Level            ////////

  if (digitalRead(BCoolantLevel) == HIGH) {
    MarcaCoolantLevel = 1;
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Coolant Level");
    while (MarcaCoolantLevel == 1) {
      CoolantLevelBasic = map(analogRead(Pot), 0, 1023, 0, 250);
      NCoolantLevel = CoolantLevelBasic;
      CoolantLevel = CoolantLevelBasic * 0.4;
      PGN_65263[6] = NCoolantLevel;
      if (millis() >= TiempoPGN_65262 + 1000) {
        sndStat = CAN0.sendMsgBuf(0xCFEEE00, 1, 8, PGN_65262);
        TiempoPGN_65262 += 1000;
      }
      if (millis() >= TiempoPGN_65263 + 500) {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Coolant Level");
        lcd.setCursor(0, 1);
        lcd.print(CoolantLevel);
        lcd.setCursor(4, 1);
        lcd.print("%");
        sndStat = CAN0.sendMsgBuf(0xCFEEF00, 1, 8, PGN_65263);
        TiempoPGN_65263 += 500;
      }
      if (millis() >= TiempoPGN_61444 + 100) {
        sndStat = CAN0.sendMsgBuf(0xCF00400, 1, 8, PGN_61444);
        TiempoPGN_61444 += 100;
      }
      if (digitalRead(BReturn) == HIGH) {
        MarcaCoolantLevel = 0;
      }
    }
  }

  //////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////
  ////////       Simulation for Engine Oil Pressure         ////////

  if (digitalRead(BEOilPres) == HIGH) {
    MarcaEOilPres = 1;
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Eng. Oil Pres.");
    while (MarcaEOilPres == 1) {
      EOilPresBasic = map(analogRead(Pot), 0, 1023, 0, 250);
      NEOilPres = EOilPresBasic;
      EOilPres = EOilPresBasic * 4;
      PGN_65263[3] = NEOilPres;
      if (millis() >= TiempoPGN_65262 + 1000) {
        sndStat = CAN0.sendMsgBuf(0xCFEEE00, 1, 8, PGN_65262);
        TiempoPGN_65262 += 1000;
      }
      if (millis() >= TiempoPGN_65263 + 500) {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Eng. Oil Pres.");
        lcd.setCursor(0, 1);
        lcd.print(EOilPres);
        lcd.setCursor(5, 1);
        lcd.print("kPa");
        sndStat = CAN0.sendMsgBuf(0xCFEEF00, 1, 8, PGN_65263);
        TiempoPGN_65263 += 500;
      }
      if (millis() >= TiempoPGN_61444 + 100) {
        sndStat = CAN0.sendMsgBuf(0xCF00400, 1, 8, PGN_61444);
        TiempoPGN_61444 += 100;
      }
      if (digitalRead(BReturn) == HIGH) {
        MarcaEOilPres = 0;
      }
    }
  }

  //////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////
  ////////         Simulation for Engine Oil Level          ////////

  if (digitalRead(BEOilLevel) == HIGH) {
    MarcaEOilLevel = 1;
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Eng. Oil Level");
    while (MarcaEOilLevel == 1) {
      EOilLevelBasic = map(analogRead(Pot), 0, 1023, 0, 250);
      NEOilLevel = EOilLevelBasic;
      EOilLevel = EOilLevelBasic * 0.4;
      PGN_65263[1] = NEOilLevel;
      if (millis() >= TiempoPGN_65262 + 1000) {
        sndStat = CAN0.sendMsgBuf(0xCFEEE00, 1, 8, PGN_65262);
        TiempoPGN_65262 += 1000;
      }
      if (millis() >= TiempoPGN_65263 + 500) {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Eng. Oil Level");
        lcd.setCursor(0, 1);
        lcd.print(EOilLevel);
        lcd.setCursor(4, 1);
        lcd.print("%");
        sndStat = CAN0.sendMsgBuf(0xCFEEF00, 1, 8, PGN_65263);
        TiempoPGN_65263 += 500;
      }
      if (millis() >= TiempoPGN_61444 + 100) {
        sndStat = CAN0.sendMsgBuf(0xCF00400, 1, 8, PGN_61444);
        TiempoPGN_61444 += 100;
      }
      if (digitalRead(BReturn) == HIGH) {
        MarcaEOilLevel = 0;
      }
    }
  }

  //////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////
  ////////         Simulation for Coolant Pressure          ////////

  if (digitalRead(BCoolantPres) == HIGH) {
    MarcaCoolantPres = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Coolant Pressure");
    while (MarcaCoolantPres == 1) {
      CoolantPresBasic = map(analogRead(Pot), 0, 1023, 0, 250);
      NCoolantPres = CoolantPresBasic;
      CoolantPres = CoolantPresBasic * 2;
      PGN_65263[6] = NCoolantPres;
      if (millis() >= TiempoPGN_65262 + 1000) {
        sndStat = CAN0.sendMsgBuf(0xCFEEE00, 1, 8, PGN_65262);
        TiempoPGN_65262 += 1000;
      }
      if (millis() >= TiempoPGN_65263 + 500) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Coolant Pressure");
        lcd.setCursor(0, 1);
        lcd.print(CoolantPres);
        lcd.setCursor(4, 1);
        lcd.print("kPa");
        sndStat = CAN0.sendMsgBuf(0xCFEEF00, 1, 8, PGN_65263);
        TiempoPGN_65263 += 500;
      }
      if (millis() >= TiempoPGN_61444 + 100) {
        sndStat = CAN0.sendMsgBuf(0xCF00400, 1, 8, PGN_61444);
        TiempoPGN_61444 += 100;
      }
      if (digitalRead(BReturn) == HIGH) {
        MarcaCoolantPres = 0;
      }
    }
  }

  //////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////
  ////////          Simulation for Engine Speed             ////////

  if (digitalRead(BESpeed) == HIGH) {
    MarcaESpeed = 1;
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Engine Speed");
    while (MarcaESpeed == 1) {
      ESpeedBasic = map(analogRead(Pot), 0, 1023, 0, 64000);
      NESpeed1 = lowByte(ESpeedBasic);
      NESpeed2 = highByte(ESpeedBasic);
      ESpeed = map(analogRead(Pot), 0, 1023, 0, 8032);
      PGN_61444[3] = NESpeed1;
      PGN_61444[4] = NESpeed2;
      if (millis() >= TiempoPGN_65262 + 1000) {
        sndStat = CAN0.sendMsgBuf(0xCFEEE00, 1, 8, PGN_65262);
        TiempoPGN_65262 += 1000;
      }
      if (millis() >= TiempoPGN_65263 + 500) {
        sndStat = CAN0.sendMsgBuf(0xCFEEF00, 1, 8, PGN_65263);
        TiempoPGN_65263 += 500;
      }
      if (millis() >= TiempoPGN_61444 + 100) {
        if (ContadorESpeed == 5) {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("Engine Speed");
          lcd.setCursor(0, 1);
          lcd.print(ESpeed);
          lcd.setCursor(6, 1);
          lcd.print("RPM");
          ContadorESpeed = 0;
        }
        sndStat = CAN0.sendMsgBuf(0xCF00400, 1, 8, PGN_61444);
        TiempoPGN_61444 += 100;
        ContadorESpeed += 1;
      }
      if (digitalRead(BReturn) == HIGH) {
        MarcaESpeed = 0;
      }
    }
  }

  //////////////////////////////////////////////////////////////////

}
