  //=============================================
  // by Martin Parker 
  // Get the distnce data send over OSC.
  // useful tutorial on saving credentials to preferences file space on the m5StickC 
  // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/ 
  // useful tutorial on writing files to disk on the ESP32:
  // https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/
  //=============================================

#include <M5StickCPlus.h> // needed to bring in all the key m5StickCPlus libraries
#include "M5StickOSC.h" // custom made function by Joe Hathaway to simplfy OSC stuff
#include "M5StickWiFi.h"   // curstom made function by Joe Hathaway to simplfy WiFi stuff
#include <VL53L0X.h> // library for time of flight sensor ** DOWNLOAD VL53L0X LIBRARY IN LIBRARY MANAGER FIRST**
#include <Wire.h> // library for Arduino & Wiring
#include <Preferences.h> // needed to write numbers to flash memory so that the device opens the same each time
Preferences preferences; // create a variable space
#include "Arduino.h"
//=============================================
// CHANGE THESE VARIABLES
// this is the network you wish to connect to
const char *network = "Tong_Phone"; // network you want to connect to
const char *password = "TongTong137"; // password to connect with M3gJcGp6
// this is the IP address of the computer hosting max
const char *ip = "172.20.10.9"; // IP address of your device
// this is the default port number
int port = 9006; // port number on your device
//=============================================

// GLOBAL VARIABLES

// ToF data
float sendDistance;

// battery variables 
float localBatVolts = 0.0F;
float localBatCurrent = 0.0F;
float volts = 0.0F;
float current = 0.0F;

VL53L0X sensor;
TFT_eSprite img = TFT_eSprite(&M5.Lcd);

//=============================================
// SETUP

void setup()
{
  Wire.begin(0, 26, 100000UL);  //(0,26, 100000UL) for I2C of HAT connection
  
  M5.begin();

    img.createSprite(200, 200);
    img.fillSprite(BLACK);
    img.setTextColor(WHITE);
    img.setTextSize(2);

    sensor.setTimeout(500);
    if (!sensor.init()) {
        img.setCursor(200, 200);
        img.print("Failed");
        img.pushSprite(0, 0);
        Serial.println("Failed to detect and initialize sensor!");
        while (1) {
        }
    }
  sensor.startContinuous(15);
  
  // connect to network
  connectToWifi(network, password);
  // IP address and port to send to
  sendTo(ip, port);
  // turn on the IMU
  M5.Imu.Init();  //Init IMU.  初始化IMU
  M5.Lcd.setRotation(3);  //Rotate the screen. 将屏幕旋转
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(30, 30);
  M5.Lcd.setCursor(30, 70);
  M5.Lcd.setCursor(15, 100 );
  M5.Lcd.printf("network : %s", network);
  M5.Lcd.setCursor(15,110);
  M5.Lcd.printf("hostIP: %s:", ip);
  M5.Axp.EnableCoulombcounter(); //track the State of Charge of a battery pack.
  
}


void HandleNetwork(){
  // Only send data when connected
  if(WL_CONNECTED){
    sendOscMessage<float>("/localBatteryLevel", localBatVolts);
    sendOscMessage<float>("/externalPowerPack", volts);
    sendOscMessage<float>("/distance", sendDistance);
  }

}

void HandleBattery(){
  M5.update();
  volts = M5.Axp.GetVBusVoltage();
  current = M5.Axp.GetVBusCurrent();
  localBatVolts = M5.Axp.GetBatVoltage();
  localBatCurrent = M5.Axp.GetBatCurrent();
  M5.Axp.GetBatState();
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(15, 35);
  M5.Lcd.printf("USB VOLTAGE %.3fv", volts);
  M5.Lcd.setCursor(15, 45);
  M5.Lcd.printf("USB CURRENT %.3fv", current);
  M5.Lcd.setCursor(15, 15);
  M5.Lcd.printf("BAT VOLTAGE %.3fv", localBatVolts);
  M5.Lcd.setCursor(15, 25);
  M5.Lcd.printf("BAT CURRENT %.3fv", localBatCurrent);
}

void HandleScreen(){
  M5.update();
  M5.Lcd.setCursor(15, 90);
  M5.Lcd.printf("Broad port %i", port);
  M5.Lcd.setTextSize(10);
  M5.Lcd.setCursor(180, 15);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(15, 75);
  M5.Lcd.printf("DISTANCE: %5.2f", sendDistance);  
}

// this function use to set the default value before sending
float DistanceValidation(uint16_t distance){
  float adjustedDistance;
  if(distance > 600.0f)
  adjustedDistance = 600.0f;
  else
  adjustedDistance = distance;
  return adjustedDistance;
}

// This is what runs forever. 
void loop() {

  // get distance  
  uint16_t distance = sensor.readRangeContinuousMillimeters();

  sendDistance = DistanceValidation(distance);
  // get battery information
  HandleBattery();
  
  HandleScreen();
  // send the data from the IMU across the network
  HandleNetwork();
  // perform the loop every 15 ms this may be too fast for some routers to enjoy themselves, so set it to something less if you need to.
  delay(15);
}