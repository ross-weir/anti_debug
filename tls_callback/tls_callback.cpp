#include <iostream>
#include <Windows.h>

/*
Resources:
- 
*/

void NTAPI tls_callback_dynamic(PVOID hModule, DWORD dwReason, PVOID pContext)
{

}

void NTAPI tls_callback1(PVOID hModule, DWORD dwReason, PVOID pContext)
{
    MessageBox(0, L"TLS callback", L"TLS", MB_OK);

    // Add tls_callback_dynmaic as a TLS callback at runtime.
    // Because the callback is added at runtime it won't be visible when inspecting the program image header on disk. Sneaky.
}

/*
From Microsoft:

The program can provide one or more TLS callback functions to support additional initialization and termination for TLS data objects. A typical use for such a callback function would be to call constructors and destructors for objects.

Although there is typically no more than one callback function, a callback is implemented as an array to make it possible to add additional callback functions if desired. If there is more than one callback function, each function is called in the order in which its address appears in the array. A null pointer terminates the array. It is perfectly valid to have an empty list (no callback supported), in which case the callback array has exactly one member-a null pointer.

The prototype for a callback function (pointed to by a pointer of type PIMAGE_TLS_CALLBACK) has the same parameters as a DLL entry-point function:
*/
#ifdef _WIN64
#pragma const_seg(".CRT$XLB")
#pragma comment (linker, "/INCLUDE:_tls_used")
EXTERN_C const
#else
#pragma data_seg(".CRT$XLB")
#pragma comment (linker, "/INCLUDE:_tls_used")
EXTERN_C
#endif

// Must be assigned within the two sets of proprocessor definitions.
PIMAGE_TLS_CALLBACK tls_callback = reinterpret_cast<PIMAGE_TLS_CALLBACK>(tls_callback1);

// End section decleration
#ifdef _WIN64
#pragma const_seg()
#else
#pragma data_seg()
#endif //_WIN64 End section decleration

int main()
{
    std::cout << "Press enter to execute process\n";

    std::cin.get();
}
