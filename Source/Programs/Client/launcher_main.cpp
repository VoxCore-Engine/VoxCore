#include <windows.h>
#include <cwchar>

namespace {
constexpr int kPathBufferSize = 32768;

const wchar_t* SkipExecutableInCommandLine(const wchar_t* commandLine) {
    if (commandLine == nullptr) {
        return L"";
    }

    const wchar_t* cursor = commandLine;
    if (*cursor == L'"') {
        ++cursor;
        while (*cursor != L'\0' && *cursor != L'"') {
            ++cursor;
        }
        if (*cursor == L'"') {
            ++cursor;
        }
    } else {
        while (*cursor != L'\0' && *cursor != L' ' && *cursor != L'\t') {
            ++cursor;
        }
    }

    while (*cursor == L' ' || *cursor == L'\t') {
        ++cursor;
    }

    return cursor;
}

int LaunchClientRuntime() {
    wchar_t launcherPath[kPathBufferSize] = {};
    const DWORD launcherPathLength = GetModuleFileNameW(nullptr, launcherPath, kPathBufferSize);
    if (launcherPathLength == 0 || launcherPathLength >= kPathBufferSize) {
        return 1;
    }

    wchar_t* lastSlash = wcsrchr(launcherPath, L'\\');
    if (lastSlash == nullptr) {
        return 1;
    }
    *lastSlash = L'\0';

    wchar_t runtimePath[kPathBufferSize] = {};
    if (_snwprintf_s(
            runtimePath,
            kPathBufferSize,
            _TRUNCATE,
            L"%s\\bin\\VoxClientRuntime.exe",
            launcherPath) < 0) {
        return 1;
    }

    const wchar_t* argumentTail = SkipExecutableInCommandLine(GetCommandLineW());

    wchar_t launchCommand[kPathBufferSize] = {};
    if (argumentTail[0] != L'\0') {
        if (_snwprintf_s(
                launchCommand,
                kPathBufferSize,
                _TRUNCATE,
                L"\"%s\" %s",
                runtimePath,
                argumentTail) < 0) {
            return 1;
        }
    } else if (_snwprintf_s(
                   launchCommand,
                   kPathBufferSize,
                   _TRUNCATE,
                   L"\"%s\"",
                   runtimePath) < 0) {
        return 1;
    }

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInfo{};
    if (!CreateProcessW(
            runtimePath,
            launchCommand,
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            launcherPath,
            &startupInfo,
            &processInfo)) {
        return static_cast<int>(GetLastError());
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);

    DWORD exitCode = 1;
    GetExitCodeProcess(processInfo.hProcess, &exitCode);

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return static_cast<int>(exitCode);
}
}

int main() {
    return LaunchClientRuntime();
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return LaunchClientRuntime();
}
