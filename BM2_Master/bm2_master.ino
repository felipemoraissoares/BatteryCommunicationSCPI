#include "bm2.h"
#include "cmd.h"
#include <Wire.h>

#define BM2_ADDR                      0x5C
#define I2C_BAUDRATE                  400000 //400 kHz 
#define PATH_BUF_SIZE                 40

static char CwdBuf[PATH_BUF_SIZE] = "/";
static char TmpBuf[PATH_BUF_SIZE];
uint8_t cmd_tx[3] = {};
uint8_t rx_data[4];
uint8_t rx_tle[2];
uint8_t tm_data[15];

// prototypes
int cmd_send(int argc, char *argv[]);
int cmd_tm(int argc, char *argv[]);
//void readTM(uint8_t cmd, int byte2read, int rDelay);
//void sendTC(byte CMD1, byte CMD2 , byte CMD3);
void sendCMD(char* CMD);


//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
CMD_TABLE_MEM StCmdLineEntry CmdTable[] =
{
  { "help",    Cmd_help,    "  : Display list of commands" },
  { "h",       Cmd_help,    "     : alias for help" },
  { "?",       Cmd_help,    "     : alias for help" },
  { "send",    cmd_send,    "   : Send a command to BM2" },
  { "tm",      cmd_tm,      "   : Send BM2 Telemetry"},
  {  0, 0, 0 }
};

//*****************************************************************************
//
// Start-up
//
//*****************************************************************************
void setup()
{
  // startup serial and wait for it to be opened...
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(I2C_BAUDRATE);
  // say hello
  Serial.print(F("\r\n\r\nArduino BM2 monitor v1.0\r\n"));
  Serial.print(F("Running on "));
#if defined(__AVR_ATmega2560__)
  Serial.print(F("2560 Arduino MEGA\r\n"));
#elif defined(__AVR_ATmega328P__)
  Serial.print(F("ATmega328P Arduino UNO\r\n"));
#elif defined(ARDUINO_SAM_DUE)
  Serial.print(F("Arduino Due\r\n"));
#else
  Serial.print(F("** unknown arduino board **\r\n"));
#endif
  Serial.print(F("Type '?' for help\r\n"));
  Serial.print(F("\r\n\r\n"));
  Cmd_help(0, 0);
  CmdShowPrompt();
}

//*****************************************************************************
//
// A never-ending infinite + 1 loop
//
//*****************************************************************************
void loop()
{
  // process serial events
  Uart0Handler();

  // process cmd
  CmdRefresh();


  // @@ Call other tasks here (not blocking obviously)
}

//void readTM(uint8_t cmd, int byte2read, int rDelay)
//{
//  uint8_t aux[2];
//  Wire.requestFrom(BM2_ADDR, byte2read);
//  delay(rDelay);
//  while (Wire.available())
//  {
//    switch (cmd)
//    {
//
//        aux[0] = Wire.read();
//        Serial.print("0x");
//        Serial.print(aux[0], HEX);
//        Serial.print(" ");
//        aux[1] = Wire.read();
//        Serial.print("0x");
//        Serial.println(aux[1], HEX);
//        break;
//      case ANTS_CMD_REP_COUNT_ANT1:
//      case ANTS_CMD_REP_COUNT_ANT2:
//      case ANTS_CMD_REP_COUNT_ANT3:
//      case ANTS_CMD_REP_COUNT_ANT4:
//        aux[0] = Wire.read();
//        Serial.print("0x");
//        Serial.print(aux[0], HEX);
//        Serial.println(" ");
//        break;
//      default:
//        char error_txt[40];
//        sprintf(error_txt, "cmd = 0x%.2X, Wrong command", cmd);
//        Serial.println(error_txt);
//        break;
//    }
//  }
//  switch (cmd)
//  {
//    case ANTS_CMD_REP_TIME_ANT1:
//    case ANTS_CMD_REP_TIME_ANT2:
//    case ANTS_CMD_REP_TIME_ANT3:
//    case ANTS_CMD_REP_TIME_ANT4:
//      Serial.println("TODO Main decode report antenna time");
//      //decode_time(aux);
//      break;
//    case ANTS_CMD_REP_COUNT_ANT1:
//    case ANTS_CMD_REP_COUNT_ANT2:
//    case ANTS_CMD_REP_COUNT_ANT3:
//    case ANTS_CMD_REP_COUNT_ANT4:
//    Serial.println("TODO Main decode report antenna count");
//      //decode_cont(aux[0]);
//      break;
//    case ANTS_CMD_GET_TEMP:
//      Serial.println("TODO Main decode temp");
//      //decode_temp_stat(aux);
//      break;
//    case ANTS_CMD_DEPLOY_STAT:
//    Serial.println("TODO Main decode deploy stat");
//      //decode_deploy_stat(aux);
//      break;
//  }
//}

void sendCMD(char* CMD)
{

  Wire.beginTransmission(BM2_ADDR);
  Wire.write(CMD, strlen(CMD));
  Wire.endTransmission();
}

//void sendTC(uint8_t CMD1, uint8_t CMD2)
//{
//  Wire.beginTransmission(BM2_ADDR);
//  cmd_tx[0] = CMD1;
//  if (CMD2 == NULL)
//  {
//    if (CMD1 == ANTS_CMD_RESET)
//    {
//      Serial.println("Sending reset cmd!");
//      Wire.write(cmd_tx, 1);
//      Wire.endTransmission();
//      Serial.println(F("Resetting I2C"));
//      Wire.end();
//      delay(5000);
//      Serial.println(F(" Init I2C"));
//      Wire.begin();
//      Wire.setClock(I2C_BAUDRATE);
//    }
//    else
//    {
//      Wire.beginTransmission(BM2_ADDR);
//      Wire.write(cmd_tx, 1);
//      Wire.endTransmission();
//    }
//  }
//  else
//  {
//    Wire.beginTransmission(BM2_ADDR);
//    cmd_tx[1] = CMD2;
//    Wire.write(cmd_tx, 2);
//    Wire.endTransmission();
//  }
//}

int cmd_tm(int argc, char *argv[])
{
  if (argc != 3)
  {
    Serial.print(F("Please send 2 args for this option! \r\n"));
    Serial.println(F("Available commands are : tm + BM2_TEL_CMD + param "));
    Serial.println(F("\n BM2_TEL_CMD: Check BM2 Telemetry List Command"));
    Serial.println(F("\n d - DATA"));
    Serial.println(F("\n n - NAME"));
    Serial.println(F("\n l - LENGTH"));
    Serial.println(F("\n a - ASCII"));
    return (0);
  }
  else
  {
    if (atoi(argv[1]) < 0 ||  atoi(argv[1]) > 115)
    {
      Serial.println("Wrong Telemetry (argv[1]");
      return 0;
    }
    char tm_cmd[20];
    char param[6];
    switch (argv[2][0])
    {
      case 'd':
        strcpy(param, "DATA");
        break;
      case 'n':
        strcpy(param, "NAME");
        break;
      case 'l':
        strcpy(param, "LENGTH");
        break;
      case 'a':
        strcpy(param, "ASCII");
        break;
      default:
        Serial.println("Wrong param");
        return 0;
        break;
    }


    sprintf(tm_cmd, "%s %s,%s", BMTEL, argv[1], param);
    Serial.print("Command to send = ");
    Serial.println(tm_cmd);
    sendCMD(tm_cmd);
  }
}

int cmd_send(int argc, char *argv[])
{
  if (argc <= 1 || argc > 2)
  {
    Serial.print(F("Please send 1 args for this option! \r\n"));
    Serial.println(F("Available commands are : send + BM2_command "));
    return (0);
  }
  else
  {
    if (strncmp(argv[1], "flash", 5) == 0)
    {
      Serial.println("[LOG] Send Reads/Writes to the Gas Gauge Flash memory directly");
      sendCMD(BQFLASH);
    }
    else
    {
      Serial.println("Wrong Option");
      return (0);
    }
  }
}




//*****************************************************************************
//
// This function implements the Cmdline call to show prompt
//
//*****************************************************************************
void CmdShowPrompt(void)
{
  //
  // Print next prompt to the console.  Show the CWD.
  //
  //UARTprintf("\n%s> ", CwdBuf);
  Serial.print("\r\n");
  Serial.print(CwdBuf);
  Serial.write('>');
}

//*****************************************************************************
//
// This function implements the Cmdline call to show status
//
//*****************************************************************************
void CmdShowStatus(int Status)
{
  //
  // Handle the case of bad command.
  //
  if (Status == CMDLINE_BAD_CMD)
  {
    //UARTprintf("Bad command!\n");
    Serial.print("Bad command!\n");
  }

  //
  // Handle the case of too many arguments.
  //
  else if (Status == CMDLINE_TOO_MANY_ARGS)
  {
    //UARTprintf("Too many arguments for command processor!\n");
    Serial.print("Too many arguments for command processor!\n");
  }

  //
  // Handle the case of not implemented
  //
  else if (Status == CMDLINE_NOT_IMPLEMENTED)
  {
    //UARTprintf("This command is not implemented\r\n");
    Serial.print("This command is not implemented\r\n");
  }

  //
  // Otherwise the command was executed.  Print the error
  // code if one was returned.
  //
  else if (Status != 0)
  {
    // aqui devia ter o 'StringFrom Result'
    //UARTprintf("Command returned error code %s\n",
    //           StringFromFresult((FRESULT)nStatus));

    Serial.print("Command returned error code ");
    //Serial.print(StringFromFresult(Status));
    Serial.print(Status);
    Serial.print("\r\n");
  }
}
