// inherit_selected_handles_child.cpp : Defines the entry point for the console application.
//
//

#include "stdafx.h"

#define BUFSIZE 4096 
HANDLE g_hInputFile = NULL;
void ErrorExit(PTSTR);

int _tmain(int argc, _TCHAR* argv[]) {
    CHAR chBuf[BUFSIZE];
    DWORD dwRead, dwWritten;
    HANDLE hStdin, hStdout;
    BOOL bSuccess;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (
        (hStdout == INVALID_HANDLE_VALUE) ||
        (hStdin == INVALID_HANDLE_VALUE)
        ) {
        ExitProcess(1);
    }

    // Send something to this process's stdout using printf.
    printf("\n ** This is a message from the child process. ** \n");

    // This simple algorithm uses the existence of the pipes to control execution.
    // It relies on the pipe buffers to ensure that no data is lost.
    // Larger applications would use more advanced process control.

    {
        // Read from standard input and stop on error or no data.
        bSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL);

        if (!bSuccess || dwRead == 0) {
            ErrorExit(TEXT("ReadFile either no data or unable to read"));
        }

        g_hInputFile = CreateFile(
            L"child_output.txt",
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (g_hInputFile == INVALID_HANDLE_VALUE)
            ErrorExit(TEXT("CreateFile"));

        // Write to standard output and stop on error.
        bSuccess = WriteFile(g_hInputFile, chBuf, dwRead, &dwWritten, NULL);

        if (!bSuccess)
            return 1;
    }
    return 0;
}

void ErrorExit(PTSTR lpszFunction)

// Format a readable error message, display a message box, 
// and exit from the application.
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                      (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40)*sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                    TEXT("%s failed with error %d: %s"),
                    lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(1);
}

