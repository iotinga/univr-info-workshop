#include "console/console.h"

#include "adapter/logging.h"

#include "core/error.h"

LOG_INIT(console)

struct ConsoleCmd {
    const char *name;
    int expectedArgs;
    ConsoleCommandHandler handler;
    const char *help;
};

static struct ConsoleCmd gCommands[] = {
    {"HELP", 0, Console_CmdHelp, "show this help message"},
    {"SYS_FW_VER", 0, Console_CmdFwVer, "get firmware version"},
};

ErrorCode Console_CmdHelp(ConsoleCtx *ctx, const int argc, const char *argv[], char *response)
{
    (void)argc;
    (void)argv;

    Console_Printf(ctx, "Available commands:" NEWLINE);
    for (size_t i = 0; i < ARRAY_SIZE(gCommands); i++)
        Console_Printf(ctx, "* %-15s - %s" NEWLINE, gCommands[i].name, gCommands[i].help);

    strcpy(response, CMD_RESPONSE_SUCCESS);

    return SUCCESS;
}

static int Console_ParseCommand(char *cmdLine, const char *name[], const char *args[])
{
    char *ptr = NULL;

    /* remove trailing \r\n */
    cmdLine = strtok_r(cmdLine, NEWLINE, &ptr);

    *name = strtok_r(cmdLine, "=", &ptr);
    if (*name == NULL)
    {
        return -1;
    }

    size_t i = 0;
    for (i = 0; i < CMD_ARGS_MAX_LENGTH; i++)
    {
        args[i] = strtok_r(NULL, ";", &ptr);
        if (args[i] == NULL)
            break;
    }

    return i;
}

static void Console_SerializeResponse(ConsoleCtx *ctx, const char *cmd, int argsLen, const char **args,
                                      const char *response)
{
    Console_Printf(ctx, "%s", cmd);

    for (int i = 0; i < argsLen; i++)
    {
        Console_Printf(ctx, "%c%s", i == 0 ? '_' : CMD_ARGS_SEPARATOR, args[i]);
    }

    Console_Printf(ctx, ":%s" CMD_RESPONSE_LINE_TERMINATOR, response);
}

ErrorCode Console_RunCommand(ConsoleCtx *ctx, char *cmdLine)
{
    static char responseBuffer[CMD_RESPONSE_MAX_LENGTH] = {0};

    const char *cmdName;
    const char *cmdArgs[CMD_ARGS_MAX_LENGTH];
    int numArgs = Console_ParseCommand(cmdLine, &cmdName, cmdArgs);

    if (numArgs < 0)
    {
        Console_SerializeResponse(ctx, "PARSE_ERROR", 0, NULL, CMD_RESPONSE_FAILURE);
        return FAILURE;
    }

    for (size_t i = 0; i < ARRAY_SIZE(gCommands); i++)
    {
        if (strcasecmp(gCommands[i].name, cmdName) == 0)
        {
            LOG_DEBUG("matched command: %s", gCommands[i].name);

            if (gCommands[i].expectedArgs != numArgs)
            {
                Console_SerializeResponse(ctx, "CMD_INVALID_ARGS", 0, NULL, CMD_RESPONSE_FAILURE);
                return FAILURE;
            }
            else
            {
                gCommands[i].handler(ctx, numArgs, cmdArgs, responseBuffer);
                Console_SerializeResponse(ctx, cmdName, numArgs, cmdArgs, responseBuffer);
                return SUCCESS;
            }
        }
    }

    Console_SerializeResponse(ctx, "CMD_NOT_FOUND", 0, NULL, CMD_RESPONSE_FAILURE);

    return FAILURE;
}

void Console_Printf(ConsoleCtx *ctx, const char *fmt, ...)
{
    static char buffer[256];

    va_list args;
    va_start(args, fmt);

    int length = vsnprintf(buffer, sizeof(buffer), fmt, args);
    if (length > 0)
    {
        ctx->writeFunction(ctx->writeFunctionCtx, length, buffer);
    }

    va_end(args);
}

void Console_RespondOnOff(char *response, bool value)
{
    if (value)
    {
        (void)strcpy(response, STR_ON);
    }
    else
    {
        (void)strcpy(response, STR_OFF);
    }
}

bool Console_ParseOnOff(const char *request)
{
    return (request != NULL) && !strcasecmp(request, STR_ON);
}

void Console_Respond(char *response, bool isSuccess)
{
    if (isSuccess)
    {
        (void)strcpy(response, CMD_RESPONSE_SUCCESS);
    }
    else
    {
        (void)strcpy(response, CMD_RESPONSE_FAILURE);
    }
}

void Console_RespondWithMessage(char *response, char *message)
{
    (void)strcpy(response, message);
}