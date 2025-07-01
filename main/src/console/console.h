#pragma once

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core/defines.h"
#include "core/error.h"
#include "core/service.h"

#define LINE_BUFFER_SIZE 512

#define CMD_MSG_START_BYTE 0x5A

#define CMD_ARGS_MAX_LENGTH  10
#define CMD_ARGS_START_TOKEN '='
#define CMD_ARGS_SEPARATOR   ';'

#define CMD_RESPONSE_MAX_LENGTH      50
#define CMD_RESPONSE_LINE_TERMINATOR "*END\r\n"
#define CMD_RESPONSE_SUCCESS         "SUCCESS"
#define CMD_RESPONSE_FAILURE         "FAIL"

#define STR_ON  "ON"
#define STR_OFF "OFF"

#define NEWLINE "\r\n"

typedef struct {
    int (*writeFunction)(void *writeFunctionCtx, size_t length, const char *data);
    void *writeFunctionCtx;
} ConsoleCtx;

typedef ErrorCode (*ConsoleCommandHandler)(ConsoleCtx *ctx, const int argc, const char *argv[], char *response);

ErrorCode Console_CmdHelp(ConsoleCtx *ctx, const int argc, const char *argv[], char *response);
ErrorCode Console_CmdFwVer(ConsoleCtx *ctx, const int argc, const char *argv[], char *response);

void Console_Respond(char *response, bool isSuccess);
void Console_RespondOnOff(char *response, bool value);
void Console_RespondWithMessage(char *response, char *message);
bool Console_ParseOnOff(const char *request);

void Console_Printf(ConsoleCtx *ctx, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

ErrorCode Console_RunCommand(ConsoleCtx *ctx, char *cmdLine);

/**
 * @return true if need to receive more data, false if command reception is completed.
 */
bool Console_StdioConsoleReceive(char ch);

SERVICE_DECLARE(SERVICE_STDIO_CONSOLE);
SERVICE_DECLARE(SERVICE_NETWORK_CONSOLE);