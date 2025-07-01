
#include "console/console.h"
#include "core/error.h"

ErrorCode Console_CmdFwVer(ConsoleCtx *ctx, const int argsLen, const char *args[], char *response)
{
    (void)argsLen;
    (void)args;

    strcpy(response, VERSION);

    return SUCCESS;
}
