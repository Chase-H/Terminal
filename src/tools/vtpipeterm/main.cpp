//
//    Copyright (C) Microsoft.  All rights reserved.
//
#include <windows.h>
#include <wil\result.h>
#include <wil\resource.h>
#include <wil\wistd_functional.h>
#include <wil\wistd_memory.h>

int __cdecl wmain(int /*argc*/, WCHAR* /*argv[]*/)
{
    HANDLE const hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    THROW_LAST_ERROR_IF_FALSE(GetConsoleMode(hOut, &dwMode));
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    dwMode |= DISABLE_NEWLINE_AUTO_RETURN;
    THROW_LAST_ERROR_IF_FALSE(SetConsoleMode(hOut, dwMode));

    wil::unique_handle pipe;
    pipe.reset(CreateNamedPipeW(L"\\\\.\\pipe\\convtpipe", PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 0, 0, 0, nullptr));
    THROW_IF_HANDLE_INVALID(pipe.get());

    THROW_LAST_ERROR_IF_FALSE(ConnectNamedPipe(pipe.get(), nullptr));

    byte buffer[256];
    DWORD dwRead;
    while (true)
    {
        dwRead = 0;
        THROW_LAST_ERROR_IF_FALSE(ReadFile(pipe.get(), buffer, ARRAYSIZE(buffer), &dwRead, nullptr));
        THROW_LAST_ERROR_IF_FALSE(WriteFile(hOut, buffer, dwRead, nullptr, nullptr));
    }

    return 0;
}
