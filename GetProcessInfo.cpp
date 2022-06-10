#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

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

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            int pid = GetProcessID(argv[i]);
            if (-1 == pid) {
                std::cout << -1 << ":" << -1 << std::endl;
            }
            else {
                std::cout << pid << ":" << GetStartTime(pid) << std::endl;
            }
        }
    }
    return 0;
}
