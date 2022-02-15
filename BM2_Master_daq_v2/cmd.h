
#ifndef CMD_H
#define CMD_H

#define CMDLINE_BAD_CMD         (-1)
#define CMDLINE_TOO_MANY_ARGS   (-2)
#define CMDLINE_TOO_FEW_ARGS    (-3)
#define CMDLINE_INVALID_ARG     (-4)
#define CMDLINE_NOT_IMPLEMENTED (-5)

// set to flash or const or <blank> to change mem location
#define CMD_TABLE_MEM            //sconst


// Command line function callback type.
typedef int (*pfnCmdLine)(int argc, char *argv[]);

//! Structure for an entry in the command list table.
typedef struct
{
    // name of command
    const char *pcCmd;

    // pointer to comand function
    pfnCmdLine pfnCmd;

    // help string
    const char *pcHelp;
}StCmdLineEntry;

// user defined cmd table
extern CMD_TABLE_MEM StCmdLineEntry CmdTable[];

// prototypes
void CmdShowPrompt(void);
void CmdRefresh(void);
void Uart0Handler(void);
int Cmd_none(int argc, char *argv[]);
int Cmd_help(int argc, char *argv[]);


#endif
