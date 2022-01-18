#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include "atlstr.h"

int GetProcessID(const TCHAR* const executableName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (!Process32First(snapshot, &entry)) {
        CloseHandle(snapshot);
        return -1;
    }

    do {
        if (!_tcsicmp(entry.szExeFile, executableName)) {
            CloseHandle(snapshot);
            return entry.th32ProcessID;
        }
    } while (Process32Next(snapshot, &entry));

    CloseHandle(snapshot);
    return -1;
}

time_t GetStartTime(int pid) {
    HANDLE hProc;
    FILETIME fProcessTime, ftExit, ftKernel, ftUser;
    hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    GetProcessTimes(hProc, &fProcessTime, &ftExit, &ftKernel, &ftUser);
    CloseHandle(hProc);
    uint64_t total_us = (((uint64_t)fProcessTime.dwHighDateTime << 32) | (uint64_t)fProcessTime.dwLowDateTime) / 10;
    total_us -= 11644473600000000ull; // UTC 0 in Windows time
    return (time_t)(total_us / 1000000);
}

int main(int argc, char* argv[])
{
    if (argc > 1) {
        USES_CONVERSION;
        TCHAR* processName = A2T(argv[1]);
        int pid = GetProcessID(processName);
        if (-1 == pid) {
            std::cout << -1 << std::endl << -1;
        }
        else {
            std::cout << pid << std::endl << GetStartTime(pid);
        }
    }
    return 0;
}
