// List of Libraries included in the skecth

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Adafruit_SSD1306.h>

// list of variable used
int upButton = 9;
int resetButton = 8;
int count = 0;
String latitude, longitude;
static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// instance for controlling Oled and GPS Module
Adafruit_SSD1306 display(128, 64);
TinyGPSPlus gps;
SoftwareSerial gpserial(RXPin, TXPin);
SoftwareSerial sim800l(3, 2);


void setup() {
  //Serial.begin(9600);
  pinMode(upButton, INPUT);
  pinMode(resetButton, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, 4);
  gpserial.begin(GPSBaud);
  sim800l.begin(9600);
}
void loop() {
  // This sketch displays information every time a new sentence is correctly encoded on the GPS Module.
  gpserial.listen();
  while (gpserial.available() > 0)
    if (gps.encode(gpserial.read()))
      get_GPS_Data();
  Operation();
}
void myMessage() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Latitude:" + String(latitude));
  display.println("Longitude:" + String(longitude));
  
  if (gps.location.isValid()) {
    SendSMS();
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.print(F("SENT"));
  } else {
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.print(F("FAIL"));
  }


  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 50);
  display.print(F("Update if Changed..."));
  display.display();
  delay(3000);
  Count();

}
void Count() {
  count = count;
  display.clearDisplay();
  display.setTextSize(5);
  display.setTextColor(WHITE);
  display.setCursor(0, 15);
  display.println(count);
  display.display();
}
void Reset() {
  count = 0;
}
void Operation() {
  if ((digitalRead(resetButton) && digitalRead(upButton)) == HIGH) {
    delay(700);
    myMessage();
    delay(500);
  } else if (digitalRead(upButton) == HIGH) {
    delay(200);
    count = count + 1;
    Count();
  } else if (digitalRead(resetButton) == HIGH) {
    Reset();
    Count();
  } else {
    count = count;
  }
}
void get_GPS_Data() {
  if (gps.location.isValid()) {
    longitude = String(gps.location.lng(), 6);
    latitude = String(gps.location.lat(), 6);
  } else {
    longitude = "xxxxxxx";
    latitude = "xxxxxxxx";
  }
}
void sms_init() {
  sim800l.listen();
  //Serial.println(F("Sending SMS..."));               //Show this message on serial monitor
  sim800l.print(F("AT+CMGF=1\r"));                   //Set the module to SMS mode
  delay(100);
  sim800l.print(F("AT+CMGS=\"Your Number\"\r")); //Your phone number don't forget to include your country code, example +212123456789"
  delay(500);
}
void SendSMS() {
  sms_init();
  sim800l.print(F("I am in danger. "));       //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
  sim800l.println(F("Pls track my location."));
  sim800l.print(F("Latitude: "));
  sim800l.println(latitude);
  sim800l.print(F("Longitude: "));
  sim800l.println(longitude);
  sim800l.print(F("Google Map: "));
  sim800l.print(F("https://maps.google.com?q="));
  sim800l.print(latitude);
  sim800l.print(',');
  sim800l.print(longitude);
  // sim800l.print(F(",14z"));
  delay(500);
  sim800l.print((char)26);// (required according to the datasheet)
  delay(500);
  //sim800l.println();
  //Serial.println(F("Text Sent."));
  delay(500);
}
