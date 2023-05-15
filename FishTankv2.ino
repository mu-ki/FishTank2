  /**********************************************************************************
 *  TITLE: Blynk + IR + Manual (Switch) control 8-Channel Relays using ESP32 with Realtime Feedback & No WiFi control
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/2HxWGHXyCt4
 *  Related Blog : https://iotcircuithub.com/esp32-projects/
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP32 : https://github.com/espressif/arduino-esp32
 *  
 *  Download the libraries 
 *  Blynk Library: https://github.com/blynkkk/blynk-library/releases/download/v0.6.1/Blynk_Release_v0.6.1.zip
 *  AceButton Library: https://github.com/bxparks/AceButton
 *  IRremote Library: https://github.com/Arduino-IRremote/Arduino-IRremote
 **********************************************************************************/

#include <IRremote.h>
#include <BlynkSimpleEsp8266.h> 
#include <AceButton.h>
using namespace ace_button;

BlynkTimer timer1;

// define the GPIO connected with Relays and switches
#define RelayPin1 5  //D1
#define RelayPin2 4  //D2

#define SwitchPin1 2   //D4 
#define SwitchPin2 14  //D5

#define wifiLed   16   //D0

//Change the virtual pins according the rooms
#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2

#define VPIN_BUTTON_8    V8
#define VPIN_BUTTON_9    V9

int toggleState_1 = 1; //Define integer to remember the toggle state for relay 1
int toggleState_2 = 1; //Define integer to remember the toggle state for relay 2

int wifiFlag = 0;
#define IR_RECV_PIN 12  //D6

IRrecv irrecv(IR_RECV_PIN);
decode_results results;

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;


void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);

#define AUTH "XZK8dRNP58rsH3P1SqU8FwTZIsIhoBtx"                 // You should get Auth Token in the Blynk App.  
#define WIFI_SSID "Muki"             //Enter Wifi Name
#define WIFI_PASS "11112222"         //Enter wifi Password

void relayOnOff(int relay){

    switch(relay){
      case 1: 
             if(toggleState_1 == 1){
              digitalWrite(RelayPin1, LOW); // turn on relay 1
              toggleState_1 = 0;
              Serial.println("Device1 ON");
              }
             else{
              digitalWrite(RelayPin1, HIGH); // turn off relay 1
              toggleState_1 = 1;
              Serial.println("Device1 OFF");
              }
             delay(100);
      break;
      case 2: 
             if(toggleState_2 == 1){
              digitalWrite(RelayPin2, LOW); // turn on relay 2
              toggleState_2 = 0;
              Serial.println("Device2 ON");
              }
             else{
              digitalWrite(RelayPin2, HIGH); // turn off relay 2
              toggleState_2 = 1;
              Serial.println("Device2 OFF");
              }
             delay(100);
      break;
      default : break;      
      }  
}

void all_Switch(int mode){
  toggleState_1 = mode; digitalWrite(RelayPin1, toggleState_1); Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1); delay(100);
  toggleState_2 = mode; digitalWrite(RelayPin2, toggleState_2); Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2); delay(100);
}

void ir_remote(){
  if (irrecv.decode(&results)) {
      switch(results.value){
          case 0x80BF49B6:  relayOnOff(1); Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1); break;
          case 0x80BFC936:  relayOnOff(2); Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2); break;
          case 0x80BF3BC4:  all_Switch(1); break;
          case 0x80BF3BC5:  all_Switch(0); break;
          default : break;         
        }   
        //Serial.println(results.value, HEX);    
        irrecv.resume();   
  } 
}

// When App button is pushed - switch the state
BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  digitalWrite(RelayPin1, toggleState_1);
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, toggleState_2);
}

BLYNK_WRITE(VPIN_BUTTON_9) {
  all_Switch(1);
}
BLYNK_WRITE(VPIN_BUTTON_8) {
  all_Switch(0);
}

BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
}

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    Serial.println("Wifi Connected");
    wifiFlag = 1;
    digitalWrite(wifiLed, LOW); //Turn off WiFi LED
  }
  if (isconnected == true) {
    Serial.println("Wifi Connected");
    wifiFlag = 0;    
    digitalWrite(wifiLed, HIGH); //Turn on WiFi LED
  }
}
void setup()
{
  Serial.begin(9600);
  
  irrecv.enableIRIn(); // Start the receiver
  
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  
  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, toggleState_1);
  digitalWrite(RelayPin2, toggleState_2);

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);
  
  button1.init(SwitchPin1);
  button2.init(SwitchPin2);


  WiFi.begin(WIFI_SSID, WIFI_PASS);
  timer1.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 3 seconds
  Blynk.config(AUTH);
  delay(1000);

  Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
}

void loop()
{      
  ir_remote();
  
  button1.check();
  button2.check();

  if (WiFi.status() == WL_CONNECTED)
  {
    Blynk.run();
  }
  
  timer1.run(); // Initiates SimpleTimer
}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      toggleState_1 = 0;
      Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);   // Update Button Widget
      digitalWrite(RelayPin1, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      toggleState_1 = 1;
      Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);   // Update Button Widget
      digitalWrite(RelayPin1, HIGH);
      break;
  }
}

void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      toggleState_2 = 0;
      Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
      digitalWrite(RelayPin2, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      toggleState_2 = 1;
      Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
      digitalWrite(RelayPin2, HIGH);
      break;
  }
}