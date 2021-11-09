#include <Wire.h>
int i = 0;
//#define ADDR 0x60 //RX
//#define ADDR 0x2B //TX
#define ADDR 0x5C //BM2 
String inputString = "";         // a String to hold incoming data
bool flag_rx = false;  // whether the string is complete
uint8_t inChar;
char test[15];
uint8_t uart_frame[250];

void setup()
{
  Wire1.begin(ADDR);                // join i2c bus with address #4
  Wire1.onReceive(receiveEvent); // register event
  Wire1.onRequest(requestEvent);
  Serial.begin(115200);           // start serial for output
  //Serial1.begin(115200);
  Serial.println("boot: I2C sub slave");
}

void loop()
{
  int i = 0;
  while (Serial.available())
  {
    // get the new byte:
    inChar = (uint8_t)Serial.read();
    // add it to the inputString:
    //inputString += inChar;
    uart_frame[i] = inChar;
    i++;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar != '\n')
      flag_rx = true;
    else
    {
      uart_frame[i] = inChar;
      i++;
    }
  }
  if (flag_rx)
  {
//    Serial.print("i = ");
//    Serial.println(i);
//    Serial.print("uart_frame = ");
//    for (int j = 0; j < i; j++)
//    {
//      Serial.print(uart_frame[j], HEX);
//      Serial.print(" ");
//    }
//    Serial.println();
//    
    for (int j = 0; j < 4; j++)
    {
      Serial.print(test[j], HEX);
      //Serial.print(" ");
    }
    Serial.println();
//    for (int k = 0 ; k < 4; k++)
//      Serial1.print(test[k]);
    Serial1.write(test,4);
    flag_rx = false;
    i = 0;
  }
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  // Serial.println("I2C ISR\r\n");
  i++;
  int k = 1;
  uint8_t rx[2];
  Serial.print(i);
  Serial.println(" - TC Received");
  while (Wire1.available()) // loop through all but the last
  {
    uint8_t c = Wire1.read(); // receive byte as a character
    //    rx[k] = c;
    //    k--;
    //Serial.print("0x");
    //Serial.print(c, HEX);        // print the character
    Serial.write(c);        // print the character
    //Serial.print(" ");
    //    Serial.println((char)c);
  }
  Serial.println();
  //  char temp[20];
  //  uint16_t data =   ((uint16_t) rx[1] << 8) | rx[0];
  //  sprintf(temp,"uint16_t = 0x%.4X",data);
  //  Serial.println(temp);
}

uint8_t teste [4]= {0x00,0x55,0xFF,0x33};
void requestEvent()
{
  Serial.println("Sending Request");

  while (Wire1.available())
  {
    Wire1.read();

  }
  Wire1.write(teste,4);
  //teste++;
  return;

}
