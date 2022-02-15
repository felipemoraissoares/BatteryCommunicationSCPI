#include "cmd.h"

#ifndef CMDLINE_MAX_ARGS
    #define CMDLINE_MAX_ARGS            8
#endif

#ifndef CMD_BUF_SIZE
    #define CMD_BUF_SIZE                40
#endif

//*****************************************************************************
//
// Flags
//
//*****************************************************************************
#define CMD_FLG_BUFF_READY              0x01
#define CMD_FLG_CTRL_CHAR               0x02
#define CMD_FLG_DISCARD                 0x04

//*****************************************************************************
//
// The buffer that holds the command line.
//
//*****************************************************************************
static char  CmdBuf[CMD_BUF_SIZE];
static char  CmdLst[CMD_BUF_SIZE];
static char  CmdFlags = 0;

// user defined functions
extern void CmdShowPrompt(void);

//*****************************************************************************
//
// Refresh cmd line task
//
//*****************************************************************************
void CmdRefresh(void)
{
    int Status;

    //
    // Check if serial data is ready
    //
    if((CmdFlags & CMD_FLG_BUFF_READY) == 0)
    {
        // abort
        return;
    }

    //
    // copy last cmd
    //
    strcpy(CmdLst, CmdBuf);

    //
    // Pass the line from the user to the command processor.
    // It will be parsed and valid commands executed.
    //
    Status = CmdLineProcess(CmdBuf);

    // clear buffer
    memset(CmdBuf, 0x00, CMD_BUF_SIZE);

    // show status
    CmdShowStatus(Status);

    // mark as processed
    CmdFlags &= ~CMD_FLG_BUFF_READY;

    // show command line
    CmdShowPrompt();
}


//*****************************************************************************
//
//
//
//*****************************************************************************
int CmdLineProcess(char *pcCmdLine)
{
    static char *argv[CMDLINE_MAX_ARGS + 1];
    char *pcChar;
    int argc;
    int bFindArg = 1;
    int bOpenString = 0;
    CMD_TABLE_MEM StCmdLineEntry *pCmdEntry;

    //
    // Initialize the argument counter, and point to the beginning of the
    // command line string.
    //
    argc = 0;
    pcChar = pcCmdLine;

    //
    // Advance through the command line until a zero character is found.
    //
    while(*pcChar)
    {
        //
        // Check what do we have here
        //
        switch(*pcChar)
        {
            //
            // There is an open \", the argument is a string!
            //
            case '\"':

                //
                // If its marked as open string
                //
                if(bOpenString)
                {
                    //
                    // Ignore last \" character
                    //
                    *pcChar = '\0';

                    //
                    // Close string
                    //
                    bOpenString = 0;

                    //
                    // We found an arg
                    //
                    bFindArg = 1;
                }
                else
                {
                    //
                    // Mark to find match \"
                    //
                    bOpenString = 1;
                }
            break;

            //
            // If there is a space.
            //
            case ' ':

                //
                // If open string is marked, ignore spaces
                //
                if(!bOpenString)
                {
                    //
                    // Replace it with a zero, and set the flag
                    // to search for the next argument.
                    //
                    *pcChar = 0;
                    bFindArg = 1;
                }

            break;

            //
            // Otherwise it is not a space, so it must be a character that is part
            // of an argument.
            //
            default:
                //
                // If bFindArg is set, then that means we are looking for the start
                // of the next argument.
                //
                if(bFindArg)
                {
                    //
                    // As long as the maximum number of arguments has not been
                    // reached, then save the pointer to the start of this new arg
                    // in the argv array, and increment the count of args, argc.
                    //
                    if(argc < CMDLINE_MAX_ARGS)
                    {
                        argv[argc] = pcChar;
                        argc++;
                        bFindArg = 0;
                    }

                    //
                    // The maximum number of arguments has been reached so return
                    // the error.
                    //
                    else
                    {
                        return(CMDLINE_TOO_MANY_ARGS);
                    }
                }
            break;
        }

        //
        // Advance to the next character in the command line.
        //
        pcChar++;
    }

    //
    // did we get an open string?
    //
    if(bOpenString)
    {
        //
        // Command line ends before closing \"
        //
        return(CMDLINE_INVALID_ARG);
    }

    //
    // If one or more arguments was found, then process the command.
    //
    if(argc)
    {
        //
        // Start at the beginning of the command table, to look for a matching
        // command.
        //
        pCmdEntry = CmdTable;

        //
        // Search through the command table until a null command string is
        // found, which marks the end of the table.
        //
        while(pCmdEntry->pcCmd)
        {
            //
            // If this command entry command string matches argv[0], then call
            // the function for this command, passing the command line
            // arguments.
            //
            if(!strcmp(argv[0], pCmdEntry->pcCmd))
            {
                return(pCmdEntry->pfnCmd(argc, argv));
            }

            //
            // Not found, so advance to the next entry.
            //
            pCmdEntry++;
        }
    }

    //
    // Fall through to here means that no matching command was found, so return
    // an error.
    //
    return(CMDLINE_BAD_CMD);
}



//*****************************************************************************
//
// This is the uart handler for CMD line data
//
//*****************************************************************************
void Uart0Handler(void)
{
    //static unsigned long ulStatus;

    //
    // Get the interrrupt status.
    //
    //ulStatus = UARTIntStatus(UART0_BASE, 1);

    //
    // Clear the asserted interrupts.
    //
    //UARTIntClear(UART0_BASE, ulStatus);

    //
    // Loop while there are characters in the receive FIFO.
    //
    //while(UARTCharsAvail(UART0_BASE))
    while(Serial.available())
    {
        static char  Data;
        static char *p = CmdBuf;

        //
        // Read the next character from the UART0 and write it back to the UART1.
        //
        //Data = UARTCharGetNonBlocking(UART0_BASE);
        Data = Serial.read();

        //
        // check if discard was requested
        //
        if(CmdFlags & CMD_FLG_DISCARD)
        {
            // stop discarding
            CmdFlags &= ~CMD_FLG_DISCARD;

            // abort this one
            return;
        }

        //
        // check if buffer is ready for cmd process
        //
        if(CmdFlags & CMD_FLG_BUFF_READY)
        {
            // abort receiving
            return;
        }

        //
        // If we are expecting a control char
        //
        if(CmdFlags & CMD_FLG_CTRL_CHAR)
        {
            char *l;

            // done
            CmdFlags &= ~CMD_FLG_CTRL_CHAR;

            // lets see what we got
            switch(Data)
            {
                // UP arrow
                case 'A':

                    // last buffer
                    l = CmdLst;

                    // insert if available
                    while((p < (CmdBuf + CMD_BUF_SIZE - 1)) && *l)
                    {
                        // insert in current position
                        *p = *l;

                        // echo
                        //UARTCharPut(UART0_BASE, *l);
                        Serial.write(*l);

                        // next
                        p++;
                        l++;
                    }

                    // ensure line termination
                    *p = '\0';

                break;

                // LEFT
                case 'D':

                    // move cursor back
                    if(p > CmdBuf)
                    {
                        // go back
                        p--;

                        // go back
                        //UARTCharPut(UART0_BASE, '\b');
                        Serial.write('\b');
                    }

                break;

                // RIGHT
                case 'C':

                    // move cursor forward
                    if(*p)
                    {
                        // re-print the character
                        //UARTCharPut(UART0_BASE, *p);
                        Serial.write(*p);

                        // go forward
                        p++;
                    }

                break;
            }

            /* Forget everything */
            return;
        }

        //
        // check data
        switch(Data)
        {
            // bkspc code
            case '\b': // default backspace escape
            case 0x7F: // PUTTY backspace

                // remove char in buffer
                if(p > CmdBuf)
                {
                    // go back
                    p--;

                    // ensure line termination
                    *p = '\0';

                    // go back, erase char, go back again
                    //UARTCharPut(UART0_BASE, '\b');
                    //UARTCharPut(UART0_BASE, ' ');
                    //UARTCharPut(UART0_BASE, '\b');
                    Serial.print("\b \b");
                }

            break;

            // control char
            case 0x1B:

                // discard next one
                CmdFlags |= CMD_FLG_DISCARD;

                // expect control char
                CmdFlags |= CMD_FLG_CTRL_CHAR;

            break;

            // carriage return
            case '\r':
            case '\n':

                // mark as buff ready
                CmdFlags |= CMD_FLG_BUFF_READY;

                // ensure line termination
                // made with memset();
                //*p = '\0';

                // back to begining
                p = CmdBuf;

                // new line
                //UARTCharPut(UART0_BASE, '\r');
                //UARTCharPut(UART0_BASE, '\n');
                Serial.write('\r');
                Serial.write('\n');

            break;

            // any char
            default:

                // check valid ascii range
                if((Data >= ' ') && (Data <= '~'))
                {
                    // insert if available
                    if(p < (CmdBuf + CMD_BUF_SIZE - 1))
                    {
                        // insert in current position
                        *p = Data;

                        // next
                        p++;

                        // echo
                        //UARTCharPut(UART0_BASE, Data);
                        Serial.write(Data);
                    }
                }

            break;
        }

    }
}


//*****************************************************************************
//
// This is the default handler for commands
//
//*****************************************************************************
int Cmd_none(int argc, char *argv[])
{
    CmdShowStatus(CMDLINE_NOT_IMPLEMENTED);
    return(0);
}

//*****************************************************************************
//
// This is the help handler
//
//*****************************************************************************
int Cmd_help(int argc, char *argv[])
{
    CMD_TABLE_MEM StCmdLineEntry *pEntry;

    //
    // Print some header text.
    //
    //UARTprintf("\nAvailable commands\n");
    //UARTprintf("------------------\n");
    Serial.print("\nAvailable commands\r\n");
    Serial.print(  "------------------\r\n");

    //
    // Point at the beginning of the command table.
    //
    pEntry = CmdTable;

    //
    // Enter a loop to read each entry from the command table.  The
    // end of the table has been reached when the command name is NULL.
    //
    while(pEntry->pcCmd)
    {
        //
        // Print the command name and the brief description.
        //
        //UARTprintf("%s%s\n", pEntry->pcCmd, pEntry->pcHelp);
        Serial.print(pEntry->pcCmd);
        Serial.write(' ');
        Serial.print(pEntry->pcHelp);
        Serial.print("\r\n");
        
        //
        // Advance to the next entry in the table.
        //
        pEntry++;
    }

    //
    // Return success.
    //
    return(0);
}
