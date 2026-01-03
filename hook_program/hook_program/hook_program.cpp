#include <windows.h>
#include <iostream>

int main() {
    // 1. 取得目標程序的 PID
    DWORD pid = 0;
    HWND hWnd = FindWindow(L"Notepad", NULL);
    if (!hWnd) {
        std::cout << "找不到 Notepad\n";
        return 1;
    }
    GetWindowThreadProcessId(hWnd, &pid);

    // 2. 開啟目標程序
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        std::cout << "無法開啟目標程序\n";
        return 1;
    }

    // 3. 在目標程序分配記憶體，寫入 DLL 路徑
    // dllpath為指定的dll檔位置
    const char* dllPath = "C:\\Users\\AOI\\source\\repos\\hook_program\\x64\\Debug\\hook_program.dll";
    void* pRemoteBuf = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1,
        MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, pRemoteBuf, dllPath, strlen(dllPath) + 1, NULL);

    // 4. 在目標程序建立遠端執行緒，呼叫 LoadLibraryA
    HANDLE hThread = CreateRemoteThread(
        hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)LoadLibraryA,
        pRemoteBuf, 0, NULL);

    // 5. 等待執行緒結束
    WaitForSingleObject(hThread, INFINITE);

    // 6. 清理
    VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    std::cout << "DLL 注入完成\n";
    return 0;
}