#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(11, 10); //SIM800L Tx & Rx is connected to Arduino #11 & #10
SoftwareSerial SerialAT(2, 3); // RX, TX


#include "HX711.h"
#include <Arduino.h>
#include <Nextion.h>  // Include the nextion library (the official one) https://github.com/itead/ITEADLIB_Arduino_Nextion
                      // Make sure you edit the NexConfig.h file on the library folder to set the correct serial port for the display.
                      // By default it's set to Serial1, which most arduino boards don't have.
                      // Change "#define nexSerial Serial1" to "#define nexSerial Serial" if you are using arduino uno, nano, etc.
int GaugeValue;  // This variable stores the value that we are going to send to the gauge on the display.
int CurrentPage = 0;  // Create a variable to store which page is currently loaded

NexButton bt0 = NexDSButton(1, 3, "bt0");  // start button on nextion
NexPage page0 = NexPage(0, 0, "page0");  // Page added as a touch event
NexPage page1 = NexPage(1, 0, "page1");  // Page added as a touch event
NexTouch *nex_listen_list[] = 
{
  &bt0,  // Dual state button added
  &page0,  // Page added as a touch event
  &page1,  // Page added as a touch event

  NULL  // String terminated
};  // End of touch event list
void bt0PushCallback(void *ptr)  // Press event for dual state button bt0
{
  uint32_t number5 = 0;  // Create variable to store value we are going to get
  bt0.getValue(&number5);  // Read value of dual state button to know the state (0 or 1)

  if(number5 == 1){  // If dual state button is equal to 1 (meaning is ON)...
    digitalWrite(13, HIGH);  // Turn ON internal LED
  }else{  // Since the dual state button is OFF...
    digitalWrite(13, LOW);  // Turn OFF internal LED
  }
}  // End of press event

void page0PushCallback(void *ptr)  // If page 0 is loaded on the display, the following is going to execute:
{
  CurrentPage = 0;  // Set variable as 0 so from now on arduino knows page 0 is loaded on the display
}  // End of press event


// Page change event:
void page1PushCallback(void *ptr)  // If page 1 is loaded on the display, the following is going to execute:
{
  CurrentPage = 1;  // Set variable as 1 so from now on arduino knows page 1 is loaded on the display
}  // End of press event
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space

// Default heartbeat interval for GSM is 60
// If you want override this value, uncomment and set this option:
//#define BLYNK_HEARTBEAT 30
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>
int GaugeValue;  // This variable stores the value that we are going to send to the gauge on the display.
int CurrentPage = 0;  // Create a variable to store which page is currently loaded

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
char auth[] = "paste here your auth key";

// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "internet"; //APN of your service provider
char user[] = "";
char pass[] = "";
HX711 scale;

TinyGsm modem(SerialAT);

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  Serial.begin(57600);

  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);


  Serial.println("Initializing...");
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  mySerial.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  updateSerial();
  mySerial.println("AT+CREG?"); //Check whether it has registered in the network
  updateSerial();

  // Set console baud rate
  Serial.begin(19200);
  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(19200);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  modem.restart();

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");
  //nextion page press event
  bt0.attachPush(bt0PushCallback);  // Dual state button bt0 press
  page0.attachPush(page0PushCallback);  // Page press event
  page1.attachPush(page1PushCallback);  // Page press event
  //page finish
  
  //Blynk
  Blynk.begin(auth, modem, apn, user, pass);

  Serial.println("HX711 sensor");
  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
            // by the SCALE parameter (not set yet)
            
  scale.set_scale(-459.542);
  //scale.set_scale(-471.497);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
            // by the SCALE parameter set with set_scale

  Serial.println("Readings:");
}

void loop()
{
  Blynk.run();
  updateSerial();
  delay(500);
  CalibrationScale();
  delay(500);
  GaugeValue();
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 5);
  delay(5000);
  nexLoop(nex_listen_list);  // Check for any touch event

  
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

void CalibrationScale()
{
  if (scale.is_ready()) {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}

void GaugeValue()
{
  GaugeValue = map(scale.get_value(5), 0, 1023, 315, 585);  // Remaps values stored on PotentiometerValue from 0 to 1023 and converts them into a range from 315 to 585.

  if(GaugeValue > 359)  // If gauge value is above 359...
  {
    GaugeValue = GaugeValue - 360;  // Subtract 360 from value to continue from 0 degrees.
  }

  // Now that we have the value to send to the gauge, there are two ways to update the gauge.
/*
  // Option 1: Send the value directly to the gauge, updating the gauge every loop cycle.
  Serial.print("z0.val=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial.print(GaugeValue);  // This is the value you want to send to that object and atribute mention before.
  Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial.write(0xff);
  Serial.write(0xff);
*/

  // Option 2: Send the value to a variable called va0 on the display so a timer on the display (tm0) can
  // compare if the variable and the current state of the gauge are different. This way we can update the gauge only if
  // the new value for the gauge is different than the previous value.
  // This is to reduce the flickering on the gauge.
  Serial.print("va0.val=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial.print(GaugeValue);  // This is the value you want to send to that object and atribute mention before.
  Serial.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial.write(0xff);
  Serial.write(0xff);

}
