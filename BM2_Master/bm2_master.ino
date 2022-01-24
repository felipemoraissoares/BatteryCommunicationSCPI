#include "main_bm2.h"
#include "cmd.h"
#include <Wire.h>

#define BM2_ADDR                      0x5C
#define I2C_BAUDRATE                  400000 //400 kHz 
#define PATH_BUF_SIZE                 40
#define I2C_RW_DELAY                  100    //100 ms
#define HDR_LEN                       5
static char CwdBuf[PATH_BUF_SIZE] = "/";
static char TmpBuf[PATH_BUF_SIZE];
uint8_t cmd_tx[3] = {};
uint8_t rx_data[4];
uint8_t rx_tle[2];
uint8_t tm_data[15];
char tm_cmd[20];
// prototypes
int cmd_tm(int argc, char *argv[]);
int cmd_func(int argc, char *argv[]);
int cmd_nvm(int argc, char *argv[]);
int cmd_heat(int argc, char *argv[]);
int cmd_sleep(int argc, char *argv[]);
int cmd_balance(int argc, char *argv[]);
int cmd_debug(int argc, char *argv[]);
int cmd_pf(int argc, char *argv[]);
int cmd_reset(int argc, char *argv[]);
//int cmd_flash(int argc, char *argv[]);
//int cmd_maccess(int argc, char *argv[]);
//int cmd_smb(int argc, char *argv[]);
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
  //{ "flash",   cmd_flash,      "   : Reads/Writes to the Gas Gauge Flash memory"},
  //{ "maccess",   cmd_maccess,      "   : Writes or reads to/from the ManufacturerAccess register on the Gas Gauge chip"},
  // { "smb",   cmd_smb,      "   : Writes or reads to/from the BQ34Z Gas Gauge chip registers"},
  { "func",   cmd_func,     "  : Function ID"},
  { "nvm",   cmd_nvm,       "   : Unlock/Write/Debug/Sleep Heater"},
  { "tm",      cmd_tm,      "    : Send BM2 Telemetry"},
  { "heat",   cmd_heat,     "  : On/Off/Auto Heater"},
  { "sleep",   cmd_sleep,   " : Set or Abort Sleep Cycle"},
  { "bal",   cmd_balance,   "   : On/Off/Auto Balance Circuit"},
  { "debug",   cmd_debug,   " : Enables/Disables Debug Info"},
  { "pf",      cmd_pf,      "    : Send BM2 Assert/Clear PF"},
  { "reset",      cmd_reset,      "  : Send BM2 Reset"},
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



int cmd_func(int argc, char *argv[])
{
  if (argc != 2)
  {
    Serial.print(F("Please send 1 args for this option! \r\n"));
    Serial.println(F("Available commands are : func function-ID"));
    return (0);
  }
  else
  {

    switch (atoi(argv[1]))
    {
      case 14:
      case 15:
      case 16:
      case 17:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29:
      case 30:
      case 31:
      case 32:
      case 33:
      case 34:
      case 35:
        sprintf(tm_cmd, "%s %s", BM_FUNC, argv[1]);
        Serial.print("Command to send = ");
        Serial.println(tm_cmd);
        sendCMD(tm_cmd);
        break;
      default:
        Serial.print("Invalid Function-ID!!");
        return 0;
        break;
    }
  }
}


int cmd_nvm(int argc, char *argv[])
{
  if (argc == 2)
  {
    switch (argv[1][0])
    {
      case 'u':
        sprintf(tm_cmd, "%s UNLOCK, 12345", BM_NVM);
        Serial.print("Command to send = ");
        Serial.println(tm_cmd);
        sendCMD(tm_cmd);
        break;
      case 'w':
        sprintf(tm_cmd, "%s WRITE, 1", BM_NVM);
        Serial.print("Command to send = ");
        Serial.println(tm_cmd);
        sendCMD(tm_cmd);
        break;
      default:
        Serial.println("Wrong Parameter, u to Unlock, w to Write, d to Debug and s to Sleep");
        return (0);
        break;
    }
  }
  else if (argc == 3)
  {
    switch (argv[1][0])
    {
      case 'd':
        sprintf(tm_cmd, "%s DEBUG,%s", BM_NVM, argv[2]); //Check what is scale_factor
        Serial.print("Command to send = ");
        Serial.println(tm_cmd);
        sendCMD(tm_cmd);
        break;
      case 's':
        sprintf(tm_cmd, "%s SLEEP_SF,%s", BM_NVM, argv[2]); //Check what is scale_factor
        Serial.print("Command to send = ");
        Serial.println(tm_cmd);
        sendCMD(tm_cmd);
        break;
      default:
        Serial.println("Wrong Parameter, u to Unlock, w to Write, d to Debug and s to Sleep");
        return 0;
        break;
    }
  }
  else
  {
    Serial.print(F("Please send 1 or 2 args for this option! \r\n"));
    Serial.println(F("Available commands are : nvm u to Unlock, w to Write, d to Debug and s to Sleep"));
    return (0);
  }
}

int cmd_heat(int argc, char *argv[])
{
  if (argc != 2)
  {
    Serial.print(F("Please send 1 args for this option! \r\n"));
    Serial.println(F("Available commands are : heat on/off/auto"));
    return (0);
  }
  else
  {
    if (strcmp(argv[1], "on") == 0)
    {
      sprintf(tm_cmd, "%s ON", BM_HEAT);
      Serial.print("Command to send = ");
      Serial.println(tm_cmd);
      sendCMD(tm_cmd);
    }
    else if (strcmp(argv[1], "off") == 0)
    {
      sprintf(tm_cmd, "%s OFF", BM_HEAT);
      Serial.print("Command to send = ");
      Serial.println(tm_cmd);
      sendCMD(tm_cmd);
    }
    else if (strcmp(argv[1], "auto") == 0)
    {
      sprintf(tm_cmd, "%s AUTO", BM_HEAT);
      Serial.print("Command to send = ");
      Serial.println(tm_cmd);
      sendCMD(tm_cmd);
    }
    else
    {
      Serial.println("Wrong param");
      Serial.println(F("Available commands are : heat on/off/auto"));
      return (0);
    }
  }
}

int cmd_sleep(int argc, char *argv[])
{
  if (argc == 2)
  {
    if (strcmp(argv[1], "stop") == 0)
    {
      sprintf(tm_cmd, "%s STOP", BM_SLEEP);
      Serial.print("Command to send = ");
      Serial.println(tm_cmd);
      sendCMD(tm_cmd);
    }
    else if (strcmp(argv[1], "abort") == 0)
    {
      sprintf(tm_cmd, "%s ABORT", BM_SLEEP);
      Serial.print("Command to send = ");
      Serial.println(tm_cmd);
      sendCMD(tm_cmd);
    }
    else
    {
      Serial.println("Wrong param");
      Serial.println(F("Available commands are : sleep stop/abort"));
      return (0);
    }
  }
  else if (argc == 3)
  {
    if (strcmp(argv[1], "for") == 0)
    {
      if (atoi(argv[2]) < 1 ||  atoi(argv[2]) > 7200)
      {
        Serial.println("Insert valid Sleep range ->  1 to 7200 minutes");
        return (0);
      }
      else
      {
        sprintf(tm_cmd, "%s FOR, %s ", BM_SLEEP, argv[2]);
        Serial.print("Command to send = ");
        Serial.println(tm_cmd);
        sendCMD(tm_cmd);
      }
    }
  }
  else
  {
    Serial.print(F("Please send 1 or 2 args for this option! \r\n"));
    Serial.println(F("Available commands are : sleep for minutes, or stop/abort"));
    return (0);
  }
}

int cmd_balance(int argc, char *argv[])
{
  if (argc != 2)
  {
    Serial.print(F("Please send 1 args for this option! \r\n"));
    Serial.println(F("Available commands are : bal on/off/auto"));
    return (0);
  }
  else
  {
    if (strcmp(argv[1], "on") == 0)
    {
      sprintf(tm_cmd, "%s ON", BM_BALANCE);
      Serial.print("Command to send = ");
      Serial.println(tm_cmd);
      sendCMD(tm_cmd);
    }
    else if (strcmp(argv[1], "off") == 0)
    {
      sprintf(tm_cmd, "%s OFF", BM_BALANCE);
      Serial.print("Command to send = ");
      Serial.println(tm_cmd);
      sendCMD(tm_cmd);
    }
    else if (strcmp(argv[1], "auto") == 0)
    {
      sprintf(tm_cmd, "%s AUTO", BM_BALANCE);
      Serial.print("Command to send = ");
      Serial.println(tm_cmd);
      sendCMD(tm_cmd);
    }
    else
    {
      Serial.println("Wrong param");
      Serial.println(F("Available commands are : bal on/off/auto"));
      return (0);
    }
  }
}

int cmd_debug(int argc, char *argv[])
{
  if (argc != 3)
  {
    Serial.print(F("Please send 2 args for this option! \r\n"));
    Serial.println(F("Available commands are : debug e/d  mask "));
    return (0);
  }
  else
  {
    switch (argv[1][0])
    {
      case 'd':
        if ( (strcmp(argv[2], "0x0001") == 0) || (strcmp(argv[2], "0x0002") == 0) || (strcmp(argv[2], "0x0004") == 0) || (strcmp(argv[2], "0x0008") == 0) || (strcmp(argv[2], "0x0010") == 0) )
        {
          sprintf(tm_cmd, "%s DISable, %s", BM_DEUBG, argv[2]);
          Serial.print("Command to send = ");
          Serial.println(tm_cmd);
          sendCMD(tm_cmd);
        }
        else
        {
          Serial.println("Invalid Mask, please type a valid mask:");
          Serial.println("debug, e or d, 0x0001");
          Serial.println("debug, e or d, 0x0002");
          Serial.println("debug, e or d, 0x0004");
          Serial.println("debug, e or d, 0x0008");
          Serial.println("debug, e or d, 0x0010");
        }
        break;
      case 'n':
        if ( (strcmp(argv[2], "0x0001") == 0) || (strcmp(argv[2], "0x0002") == 0) || (strcmp(argv[2], "0x0004") == 0) || (strcmp(argv[2], "0x0008") == 0) || (strcmp(argv[2], "0x0010") == 0) )
        {
          sprintf(tm_cmd, "%s ENable, %s", BM_DEUBG, argv[2]);
          Serial.print("Command to send = ");
          Serial.println(tm_cmd);
          sendCMD(tm_cmd);
        }
        else
        {
          Serial.println("Invalid Mask, please type a valid mask:");
          Serial.println("debug, e or d, 0x0001");
          Serial.println("debug, e or d, 0x0002");
          Serial.println("debug, e or d, 0x0004");
          Serial.println("debug, e or d, 0x0008");
          Serial.println("debug, e or d, 0x0010");
        }
        break;
      default:
        Serial.println("Wrong param, e to Enable or d to Disable");
        return (0);
        break;
    }
  }
}

int cmd_pf(int argc, char *argv[])
{
  if (argc != 2)
  {
    Serial.print(F("Please send 1 args for this option! \r\n"));
    Serial.println(F("Available commands are : pf on/off"));
    return (0);
  }
  else
  {
    if (strcmp(argv[1], "on") == 0)
    {
      sprintf(tm_cmd, "%s ON", BM_PFIN);
      Serial.print("Command to send = ");
      Serial.println(tm_cmd);
      sendCMD(tm_cmd);
    }
    else if (strcmp(argv[1], "off") == 0)
    {
      sprintf(tm_cmd, "%s OFF", BM_PFIN);
      Serial.print("Command to send = ");
      Serial.println(tm_cmd);
      sendCMD(tm_cmd);
    }
    else
    {
      Serial.println("Wrong param");
      Serial.println(F("Available commands are : pf on/off"));
      return (0);
    }
  }
}

int cmd_reset(int argc, char *argv[])
{
  if (argc > 1)
  {
    Serial.print(F("Please send 0 args for this option! \r\n"));
    Serial.println(F("Available commands are : reset"));
    return (0);
  }
  else
  {
    sprintf(tm_cmd, "%s BQ", BM_RESET);
    Serial.print("Command to send = ");
    Serial.println(tm_cmd);
    sendCMD(tm_cmd);
    return (0);
  }
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
      byte2read = HDR_LEN + 14;
      break;
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
      byte2read = HDR_LEN + 17;
      break;
    case 58:
      byte2read = HDR_LEN + 21;
      break;
    case 53:
      byte2read = HDR_LEN + 28;
      break;
    case 0:
    case 55:
    case 56:
      byte2read = HDR_LEN + 45;
      break;
    case 78:
      byte2read = HDR_LEN + 46;
      break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 33:
    case 34:
    case 37:
    case 38:
    case 48:
    case 49:
    case 50:
    case 51:
    case 57:
    case 60:
    case 61:
    case 62:
    case 63:
    case 71:
    case 72:
    case 73:
    case 74:
    case 84:
    case 90:
    case 93:
    case 114:
    case 115:
      byte2read = HDR_LEN + 15;
      break;
    default:
      Serial.print("Invalid TEL Param!!");
      return ;
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
  for (i = 0; i < byte2read; i++)
  {
    Serial.print(aux[i]);
  }
  Serial.println();
  for (i = 0; i < byte2read; i++)
  {
    Serial.print(aux[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  memset(aux,0,sizeof(aux));
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
    switch (atoi(argv[1]))
    {
      case 0:
      case 8:
      case 9:
      case 10:
      case 11:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 28:
      case 29:
      case 30:
      case 31:
      case 32:
      case 33:
      case 34:
      case 36:
      case 37:
      case 38:
      case 48:
      case 49:
      case 50:
      case 51:
      case 52:
      case 53:
      case 55:
      case 56:
      case 57:
      case 58:
      case 60:
      case 61:
      case 62:
      case 63:
      case 71:
      case 72:
      case 73:
      case 74:
      case 77:
      case 78:
      case 84:
      case 90:
      case 93:
      case 114:
      case 115:
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

        sprintf(tm_cmd, "%s %s,%s\n", BM_TEL, argv[1], param);
        Serial.print("Command to send = ");
        Serial.println(tm_cmd);
        for (int i = 0; i <= 20; i++)
        {
          Serial.print(tm_cmd[i],HEX);
          Serial.print(" ");
        }
        Serial.println();
        sendCMD(tm_cmd);
        delay(I2C_RW_DELAY);
        readTM(atoi(argv[1]));
        return 0;
        break;
      default:
        Serial.println("Wrong Telemetry (argv[1])");
        return 0;
        break;
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
