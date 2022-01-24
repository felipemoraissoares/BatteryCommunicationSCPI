#include <Wire.h>

int i = 0;
//#define ADDR 0x60 //RX
//#define ADDR 0x2B //TX
#define ADDR 0x5C //BM2 
String inputString = "";         // a String to hold incoming data
bool flag_rx = false;  // whether the string is complete
uint8_t inChar;
char test[15];


void setup()
{

  Wire.begin(ADDR);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
  Serial.begin(115200);           // start serial for output
  //Serial1.begin(115200);
  Serial.println("boot: I2C sub slave");

}

void loop()
{

}

void receiveEvent(int howMany)
{

  Serial.println(" - TC Received");
  while (Wire.available()) // loop through all but the last
  {
    uint8_t c = Wire.read(); // receive byte as a character

    //Serial.print("0x");
    Serial.print(c, HEX);        // print the character
    // Serial.write(c);        // print the character
    Serial.print(" ");
    //    Serial.println((char)c);
  }
  Serial.println();

}

char teste[20] = "Return Telemetry X";

void requestEvent()
{
  Serial.println("Sending Request");

  //  while (Wire.available())
  //  {
  //    Wire.read();
  //
  //  }
  Wire.write('x');

  //teste++;
  return;

}
