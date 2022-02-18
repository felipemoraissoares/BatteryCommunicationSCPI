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
  //if (millis() >= tempo_delay + 5000)
  if (flagRX)
  {
    /*respTM[1] = (tempo >> 24) & 0xFF;
      respTM[2] = (tempo >> 16) & 0xFF;
      respTM[3] = (tempo >> 8) & 0xFF;
      respTM[4] = (tempo) & 0xFF;
      // sprintf(debug_print, "%d -> 0x%.8X", millis(), tempo);
      //Serial.println(debug_print);
    */

    //ex: BM:TEL? 9,DATA
    snprintf(buf, sizeof(buf), "%.*s", current_len - 14, &receiveTC[8]);
    sprintf(debug_print, "\nbuf content =  %s", buf);
    Serial.println(debug_print);
    tc = atoi(buf);
    sprintf(debug_print, "TC =  %d", tc);
    Serial.println(debug_print);
    //  tempo_delay += millis();

    //    for (int i = 0 ; i < 10 ; i++)
    //    {
    //      sprintf(debug_print, "0x%.2X ", respTM[i]);
    //      Serial.print(debug_print);
    //    }
    //    Serial.println();
    tempo = (millis() / 1000);
    flagRX = false;
  }

}

void receiveEvent(int howMany)
{

  sprintf(debug_print, "TC Received -> length = %d", howMany);
  Serial.println(debug_print);
  current_len = howMany;
  int k = 0;

  while (Wire.available()) // loop through all but the last
  {
    receiveTC[k] = Wire.read();


    //    uint8_t c = Wire.read(); // receive byte as a character
    //
    //            Serial.print("0x");
    //            Serial.print(receiveTC[k], HEX);        // print the HEX
    //            Serial.print(" ");


    //    Serial.write(c);        // print the character
    //    Serial.println();
    k++;
    if (k == howMany)
      flagRX = true;
  }
  // Serial.println();

}

uint8_t tm8[20] = {0X1 , 0X6D , 0X24 , 0X0 , 0X0 , 0X8E , 0XB , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm9[20] = {0x01 , 0xB2 , 0x78 , 0x0 , 0x0 , 0xA7 , 0x1B , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0};
uint8_t tm10[20] = {0X1 , 0X46 , 0X6F , 0X0 , 0X0 , 0XCE , 0XFF , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm11[20] = {0X1 , 0XFF , 0X7F , 0X0 , 0X0 , 0XCD , 0XFF , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm13[19] = {0X1 , 0X71 , 0X87 , 0X0 , 0X0 , 0X16 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm14[19] = {0X1 , 0XEC , 0X98 , 0X0 , 0X0 , 0X14 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm15[20] = {0X1 , 0X95 , 0XA0 , 0X0 , 0X0 , 0X94 , 0X5 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm17[20] = {0X1 , 0X79 , 0XB4 , 0X0 , 0X0 , 0X4A , 0X9 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm18[20] = {0X1 , 0X2F , 0XBC , 0X0 , 0X0 , 0X4A , 0X9 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm19[20] = {0X1 , 0X32 , 0XCF , 0X0 , 0X0 , 0XFF , 0XFF , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm20[20] = {0X1 , 0XC7 , 0XDE , 0X0 , 0X0 , 0X88 , 0X13 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm21[20] = {0X1 , 0X7C , 0XED , 0X0 , 0X0 , 0XD0 , 0X20 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm23[20] = {0X1 , 0XD5 , 0X14 , 0X1 , 0X0 , 0X3 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm33[20] = {0X1 , 0X9F , 0X9C , 0X1 , 0X0 , 0X20 , 0X4E , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm34[20] = {0X1 , 0X72 , 0XA3 , 0X1 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm36[19] = {0X1 , 0X57 , 0XB0 , 0X1 , 0X0 , 0X2D , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm37[20] = {0X1 , 0X8 , 0XB6 , 0X1 , 0X0 , 0XB6 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm48[20] = {0X1, 0XD5, 0XCB, 0X1, 0X0, 0X8B, 0XB, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0};
uint8_t tm49[20] = {0X1 , 0X7B , 0XD1 , 0X1 , 0X0 , 0X8A , 0XB , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm50[20] = {0X1 , 0XE3 , 0XD5 , 0X1 , 0X0 , 0X96 , 0XB , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm51[20] = {0X1 , 0XD , 0XE6 , 0X1 , 0X0 , 0X92 , 0XB , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm60[20] = {0X1 , 0X8 , 0X45 , 0X2 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm61[20] = {0X1 , 0X1B , 0X4B , 0X2 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm62[20] = {0X1 , 0XEE , 0X4E , 0X2 , 0X0 , 0XEA , 0XD , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm63[20] = {0X1 , 0XD9 , 0X54 , 0X2 , 0X0 , 0XC2 , 0XD , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm71[20] = {0X1 , 0X70 , 0X5D , 0X2 , 0X0 , 0X94 , 0XB , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm72[20] = {0X1 , 0X92 , 0X63 , 0X2 , 0X0 , 0X92 , 0XB , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm73[20] = {0X1 , 0X89 , 0X6A , 0X2 , 0X0 , 0X91 , 0XB , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm74[20] = {0X1 , 0X6B , 0X70 , 0X2 , 0X0 , 0X94 , 0XB , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm77[19] = {0X1 , 0X68 , 0X75 , 0X2 , 0X0 , 0X2 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm90[20] = {0X1 , 0X20 , 0XA6 , 0X2 , 0X0 , 0X10 , 0X8 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm93[20] = {0x01, 0XA9 , 0XAC , 0X2 , 0X0 , 0XA9 , 0X1B , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};
uint8_t tm115[20] = {0X1 , 0XCA , 0XBA , 0X2 , 0X0 , 0XA , 0XB , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0 , 0X0};


void requestEvent()
{
  Serial.println("Sending Request");
  //  sprintf(debug_print, "TC =  %d", tc);
  //  Serial.println(debug_print);

  //Wire.write('x');
  switch (tc)
  {
    case 8:
      Wire.write(tm8, sizeof(tm8));
      break;
    case 9:
      Wire.write(tm9, sizeof(tm9));
      break;
    case 10:
      Wire.write(tm10, sizeof(tm10));
      break;
    case 11:
      Wire.write(tm11, sizeof(tm11));
      break;
    case 13:
      Wire.write(tm13, sizeof(tm13));
      break;
    case 14:
      Wire.write(tm14, sizeof(tm14));
      break;
    case 15:
      Wire.write(tm15, sizeof(tm15));
      break;
    case 17:
      Wire.write(tm17, sizeof(tm17));
      break;
    case 18:
      Wire.write(tm18, sizeof(tm18));
      break;
    case 19:
      Wire.write(tm19, sizeof(tm19));
      break;
    case 20:
      Wire.write(tm20, sizeof(tm20));
      break;
    case 21:
      Wire.write(tm21, sizeof(tm21));
      break;
    case 23:
      Wire.write(tm23, sizeof(tm23));
      break;
    case 33:
      Wire.write(tm33, sizeof(tm33));
      break;
    case 34:
      Wire.write(tm34, sizeof(tm34));
      break;
    case 36:
      Wire.write(tm36, sizeof(tm36));
      break;
    case 37:
      Wire.write(tm37, sizeof(tm37));
      break;
    case 48:
      Wire.write(tm48, sizeof(tm48));
      break;
    case 49:
      Wire.write(tm49, sizeof(tm49));
      break;
    case 50:
      Wire.write(tm50, sizeof(tm50));
      break;
    case 51:
      Wire.write(tm51, sizeof(tm51));
      break;
    case 60:
      Wire.write(tm60, sizeof(tm60));
      break;
    case 61:
      Wire.write(tm61, sizeof(tm61));
      break;
    case 62:
      Wire.write(tm62, sizeof(tm62));
      break;
    case 63:
      Wire.write(tm63, sizeof(tm63));
      break;
    case 71:
      Wire.write(tm71, sizeof(tm71));
      break;
    case 72:
      Wire.write(tm72, sizeof(tm72));
      break;
    case 73:
      Wire.write(tm73, sizeof(tm73));
      break;
    case 74:
      Wire.write(tm74, sizeof(tm74));
      break;
    case 77:
      Wire.write(tm77, sizeof(tm77));
      break;
    case 90:
      Wire.write(tm90, sizeof(tm90));
      break;
    case 93:
      Wire.write(tm93, sizeof(tm93));
      break;
    case 115:
      Wire.write(tm115, sizeof(tm115));
      break;
    default:
      Wire.write('X');
      break;
  }


}
