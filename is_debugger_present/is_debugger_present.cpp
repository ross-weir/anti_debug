#include <iostream>
#include <Windows.h>
#include <winternl.h>

/*
IsDebuggerPresent is one of the simplest methods of detecting if a process is being debugged by a user-mode debugger.

IsDebuggerPresent simply reads the BeingDebugged field in the PEB (Process Environment Block) structure which will be 1 if the process is being debugged, otherwise 0.

We can defeat this debugging check by hooking the IsDebuggerPresent function and always returning 0 or simply setting the field in the structure to 0 before checking code is executed.

Setting the field to 0 even though the process is being debugged has no effect on process execution - it is a simple informative flag.

TEB: https://en.wikipedia.org/wiki/Win32_Thread_Information_Block
PEB: https://en.wikipedia.org/wiki/Process_Environment_Block
*/

PTEB get_teb()
{
#if defined(_M_X64) // x64
    return reinterpret_cast<PTEB>(__readgsqword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self)));
#else // x86
    return reinterpret_cast<PTEB>(__readfsdword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self)));
#endif
}

bool detect_debug()
{
    // Read the PEB structure directly to defeat IsDebuggerPresent hooks.
    // Note that when running 32-bit processes on 64-bit versions of windows there is a separate PEB for the 32-bit portion and the 64-bit portion.
    return IsDebuggerPresent() || get_teb()->ProcessEnvironmentBlock->BeingDebugged == 1;
}

void defeat_anti_debug()
{
    get_teb()->ProcessEnvironmentBlock->BeingDebugged = 0;
}

int main()
{
    std::cout << "Press enter to execute process\n";

    std::cin.get();
    defeat_anti_debug();

    const auto result_str = detect_debug() ? "Debugger detected!" : "No debugging detected";

    std::cout << result_str << "\n";
    std::cout << "Press enter to exit process\n";
    std::cin.get();
}
