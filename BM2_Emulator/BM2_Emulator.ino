#include <Wire.h>

int i = 0;
//#define ADDR 0x60 //RX
//#define ADDR 0x2B //TX
#define ADDR 0x5C //BM2 
String inputString = "";         // a String to hold incoming data
bool flag_rx = false;  // whether the string is complete
uint8_t inChar;
char debug_print[50];
int k = 0;

uint8_t respTM[45] = {0x01};
uint8_t receiveTC[18];
int current_len;
int tc;
bool flagRX = false;
uint32_t tempo;
uint32_t tempo_delay = 0;
char buf[4];
void setup()
{

  Wire.begin(ADDR);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
  Serial.begin(115200);           // start serial for output
  //Serial1.begin(115200);
  Serial.println("boot: I2C sub slave");
  tempo = (millis() / 1000);
}

void loop()
{
  int const *a const;
  //if (millis() >= tempo_delay + 5000)
  if (flagRX)
  {
    respTM[1] = (tempo >> 24) & 0xFF;
    respTM[2] = (tempo >> 16) & 0xFF;
    respTM[3] = (tempo >> 8) & 0xFF;
    respTM[4] = (tempo) & 0xFF;
    // sprintf(debug_print, "%d -> 0x%.8X", millis(), tempo);
    //Serial.println(debug_print);


    //ex: BM:TEL? 9,DATA
    snprintf(buf, sizeof(buf), "%.*s", current_len - 14, &receiveTC[8]);
    sprintf(debug_print, "buf =  %s", buf);
    Serial.println(debug_print);
    tc = atoi(buf);
    sprintf(debug_print, "TC =  %d", tc);
    Serial.println(debug_print);
    //  tempo_delay += millis();

       for (int i = 0 ; i < 10 ; i++)
    {
      sprintf(debug_print, "0x%.2X ", respTM[i]);
      Serial.print(debug_print);
    }
    Serial.println();
    tempo = (millis() / 1000);
    flagRX = false;
  }

}

void receiveEvent(int howMany)
{

  sprintf(debug_print, "TC Received - length = %d", howMany);
  Serial.println(debug_print);
  current_len = howMany;
  int k = 0;
  while (Wire.available()) // loop through all but the last
  {
    receiveTC[k] = Wire.read();
    k++;

    //    uint8_t c = Wire.read(); // receive byte as a character
    //
//        Serial.print("0x");
//        Serial.print(c, HEX);        // print the HEX
//        Serial.print(" ");


    //    Serial.write(c);        // print the character
    //    Serial.println();
    if (k == howMany)
      flagRX = true;
  }
  // Serial.println();

}

uint8_t tm9[20] = {0x0 , 0xB2 , 0x78 , 0x0 , 0x0 , 0xA7 , 0x1B , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0};

void requestEvent()
{
  Serial.println("Sending Request");

  //Wire.write('x');
  if(tc == 9)
    Wire.write(tm9, sizeof(tm9));
  else
    Wire.write('x');


}
