#define BLINKER_WIFI
#define BLINKER_MIOT_AIRCONDITION
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "ESP8266WiFi.h"

// Blinker auth and network info
char auth[] = "3fe84ae7fe22";
char ssid[] = "ATGCUN";
char pswd[] = "CCCACCCA";

// define air conditioner with infrared sensor
#include <ir_Mitsubishi.h>
#include <Blinker.h>
const uint16_t IR = 14;
IRMitsubishiAC ac(IR);     // Set the IR port for sending messages.

// define the temperature sensor
#include <Wire.h>
#include "AM2320.h"
AM2320 th;

// define the variables of temerature and humidity
float temp_now, humi_now;
BlinkerNumber HUMI_NOW("humi_now");
BlinkerNumber TEMP_NOW("temp_now");

//wifi signal detection
BlinkerNumber TEXT("rssi");
BlinkerText MODE("mode");

//define the Sliders
BlinkerSlider SliderTem("slider_tem");
BlinkerSlider SliderWind("slider_wind");

//define the buttons
BlinkerButton PowerBtn("power_btn");
BlinkerButton WindMoveBtn("wind_btn");
BlinkerButton AutoBtn("automode");
BlinkerButton CoolBtn("coolmode");
BlinkerButton DryBtn("drymode");
BlinkerButton WindBtn("windmode");
BlinkerButton HeatBtn("heatmode");
//BlinkerButton AutosetBtn("autoset");

uint8_t setTemp;

// temperature control
void SliderTem_callback(int32_t value)
{
  BLINKER_LOG("get slider value: ", value);
  ac.setTemp(value);
  ac.send();
}

// wind control
void SliderWind_callback(int32_t value)
{
  BLINKER_LOG("get slider value: ", value);
  switch (value)
  {
    case 0:
      ac.setFan(kMitsubishiAcFanAuto);
      ac.send();
      break;
    case 1:
      ac.setFan(kMitsubishiAcVaneLow);
      ac.send();
      break;
    case 2:
      ac.setFan(kMitsubishiAcVaneMiddle);
      ac.send();
      break;
    case 3:
      ac.setFan(kMitsubishiAcVaneHigh);
      ac.send();
      break;
    case 4:
      ac.setFan(kMitsubishiAcVaneHighest);
      ac.send();
      break;
    default:
      break;
  }
}

// power control
void PowerBtn_callback(const String & state)
{

  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_ON) {
    BLINKER_LOG("Power on!");
    PowerBtn.color("#00FFE5");
    PowerBtn.text("Power On");
    PowerBtn.print("on");
    ac.on();
    ac.send();
  }
  else if (state == BLINKER_CMD_OFF) {
    BLINKER_LOG("Power off!");
    PowerBtn.color("#00FFE5");
    PowerBtn.text("Power Off");
    PowerBtn.print("off");
    ac.off();
    ac.send();
  }
  else {
    BLINKER_LOG("Get user setting: ", state);
    PowerBtn.color("#FFFFFF");
    PowerBtn.text("?");
    PowerBtn.print();
  }
}
// wind move control
void WindMoveBtn_callback(const String & state)
{

  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_ON) {
    BLINKER_LOG("Wind move on!");
    WindMoveBtn.color("#00bbff");
    WindMoveBtn.text("Auto Wind On");
    WindMoveBtn.print("on");
    ac.setWideVane(kMitsubishiAcWideVaneAuto);
    ac.send();
  }
  else if (state == BLINKER_CMD_OFF) {
    BLINKER_LOG("Wind move off!");
    WindMoveBtn.color("#00bbff");
    WindMoveBtn.text("Auto Wind Off");
    WindMoveBtn.print("off");
    ac.setWideVane(kMitsubishiAcWideVaneMiddle);
    ac.send();
  }
  else {
    BLINKER_LOG("Get user setting: ", state);
    WindMoveBtn.color("#FFFFFF");
    WindMoveBtn.text("?");
    WindMoveBtn.print();

  }
}

// Auto Mode
void AutoBtn_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_BUTTON_TAP) {
    BLINKER_LOG("Auto mode on!");
    ac.setMode(kMitsubishiAcAuto);
    ac.send();
    MODE.print("Auto");
  }
}
// Cool Mode
void CoolBtn_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_BUTTON_TAP) {
    BLINKER_LOG("Cool Mode on!");
    ac.setMode(kMitsubishiAcCool);
    ac.send();
    MODE.print("Cool");
  }
}
// Dry mode
void DryBtn_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_BUTTON_TAP) {
    BLINKER_LOG("Dry mode on!");
    ac.setMode(kMitsubishiAcDry);
    ac.send();
    MODE.print("Dry");
  }
}
// Fan mode
void WindBtn_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_BUTTON_TAP) {
    BLINKER_LOG("Fan mode on!");
    ac.setMode(kMitsubishiAcFan);
    ac.send();
    MODE.print("Fan");
  }
}
// Heat mode
void HeatBtn_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);
  if (state == BLINKER_CMD_BUTTON_TAP) {
    BLINKER_LOG("Heat mode on!");
    ac.setMode(kMitsubishiAcHeat);
    ac.send();
    MODE.print("Heat");
  }
}

//// auto set temperature mode
//void AutosetBtn_callback(const String & state)
//{
//  BLINKER_LOG("get button state: ", state);
//  if (state == BLINKER_CMD_BUTTON_TAP) {
//    BLINKER_LOG("auto set on!");
//    if(humi_now > 25){
//      ac.setTemp(24);
//      ac.send();
//    }
//    if(humi_now < 23){
//      ac.setTemp(24);
//      ac.send();
//    }
//  }
//}

void dataRead(const String & data)
{
  BLINKER_LOG("Blinker readString: ", data);
  //Vibration reminder
  Blinker.vibrate();
  uint32_t BlinkerTime = millis();
  Blinker.print("millis", BlinkerTime);
}

void heartbeat()// display real time information
{
  TEXT.print(WiFi.RSSI());
  TEMP_NOW.print(temp_now);
  HUMI_NOW.print(humi_now);
}

void setup()
{
  ac.begin();
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  //register callback functions
  PowerBtn.attach(PowerBtn_callback);
  WindMoveBtn.attach(WindMoveBtn_callback);
  AutoBtn.attach(AutoBtn_callback);
  CoolBtn.attach(CoolBtn_callback);
  DryBtn.attach(DryBtn_callback);
  WindBtn.attach(WindBtn_callback);
  HeatBtn.attach(HeatBtn_callback);
  //AutosetBtn.attach(button8_callback);
  SliderTem.attach(SliderTem_callback);
  SliderWind.attach(SliderWind_callback);

  Blinker.begin(auth, ssid, pswd);

  Blinker.attachData(dataRead);
  Blinker.attachHeartbeat(heartbeat);

  pinMode(IR, OUTPUT);
  digitalWrite(IR, HIGH);
}

void loop()
{
  Blinker.run();

  // get temperature and humidity
  humi_now = th.getHumidity();
  temp_now = th.getTemperature();
  //Serial.print(th.getHumidity());
  //Serial.print(th.getTemperature());

}
