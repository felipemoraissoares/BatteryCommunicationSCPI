#include "main_bm2.h"
#include "cmd.h"
#include <Wire.h>

#define BM2_ADDR                      0x5C
#define I2C_BAUDRATE                  400000 //400 kHz 
#define PATH_BUF_SIZE                 40
#define I2C_RW_DELAY                  100    //100 ms
static char CwdBuf[PATH_BUF_SIZE] = "/";
static char TmpBuf[PATH_BUF_SIZE];
uint8_t cmd_tx[3] = {};
uint8_t rx_data[4];
uint8_t rx_tle[2];
uint8_t tm_data[15];

// prototypes
int cmd_tm(int argc, char *argv[]);
void readTM(int cmd);
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

void readTM(int cmd)
{
  Serial.print("TM = ");
  Serial.println(cmd);
 
  char aux[50];
  int byte2read;
  switch (cmd)
  {
    case 13:
    case 14:
    case 36:
    case 52:
    case 77:
      byte2read = 14;
      break;
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 51:
      byte2read = 17;
      break;
    case 58:
      byte2read = 21;
      break;
    case 53:
      byte2read = 28;
      break;
    case 0:
    case 55:
    case 56:
      byte2read = 45;
      break;
    case 78:
      byte2read = 46;
      break;
    default:
      byte2read = 15;
      break;
  }
  Serial.print("byte2read = ");
  Serial.println(byte2read);
  Wire.requestFrom(BM2_ADDR, byte2read);
  delay(I2C_RW_DELAY);
  int i = 0;
  while (Wire.available())
  {
    aux[i] = Wire.read();
    i++;
  }
  for(i = 0;i < byte2read;i++)
  Serial.write(aux[i]);
  Serial.println();
}

void sendCMD(char* CMD)
{

  Wire.beginTransmission(BM2_ADDR);
  Wire.write(CMD, strlen(CMD));
  Wire.endTransmission();
}


int cmd_tm(int argc, char *argv[])
{
  if (argc != 3)
  {
    Serial.print(F("Please send 2 args for this option! \r\n"));
    Serial.println(F("Available commands are : tm + BM2_TEL_CMD + param "));
    Serial.println(F("\n BM2_TEL_CMD: Check BM2 Telemetry Command List"));
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
    delay(I2C_RW_DELAY);
    readTM(atoi(argv[1]));
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
