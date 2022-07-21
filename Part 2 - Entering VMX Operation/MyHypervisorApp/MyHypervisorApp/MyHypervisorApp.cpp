// MyHypervisorApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <bitset>
#include <array>
#include <string>
#include <intrin.h>

using namespace std;

std::string
GetCpuID()
{
    // Initialize used variables
    char   SysType[13]; // Array consisting of 13 single bytes/characters
    string CpuID;       // The string that will be used to add all the characters to
                        // Starting coding in assembly language
    _asm
        {
            // Execute CPUID with EAX = 0 to get the CPU producer
		XOR EAX, EAX
		CPUID
                    // MOV EBX to EAX and get the characters one by one by using shift out right bitwise operation.
		MOV EAX, EBX
		MOV SysType[0], AL
		MOV SysType[1], AH
		SHR EAX, 16
		MOV SysType[2], AL
		MOV SysType[3], AH
                // Get the second part the same way but these values are stored in EDX
		MOV EAX, EDX
		MOV SysType[4], AL
		MOV SysType[5], AH
		SHR EAX, 16
		MOV SysType[6], AL
		MOV SysType[7], AH
                // Get the third part
		MOV EAX, ECX
		MOV SysType[8], AL
		MOV SysType[9], AH
		SHR EAX, 16
		MOV SysType[10], AL
		MOV SysType[11], AH
		MOV SysType[12], 00
        }
    CpuID.assign(SysType, 12);
    return CpuID;
}

bool
DetectVmxSupport()
{
    bool VMX = false;
    __asm {
		xor    eax, eax
		inc    eax
		cpuid
		bt     ecx, 0x5
		jc     VMXSupport
		VMXNotSupport :
		jmp     NopInstr
		VMXSupport :
		mov    VMX, 0x1
		NopInstr :
		nop
    }

    return VMX;
}

void
PrintAppearance()
{
    printf("\n"

           "    _   _                             _                  _____                      ____                 _       _     \n"
           "   | | | |_   _ _ __   ___ _ ____   _(_)___  ___  _ __  |  ___| __ ___  _ __ ___   / ___|  ___ _ __ __ _| |_ ___| |__  \n"
           "   | |_| | | | | '_ \\ / _ \\ '__\\ \\ / / / __|/ _ \\| '__| | |_ | '__/ _ \\| '_ ` _ \\  \\___ \\ / __| '__/ _` | __/ __| '_ \\ \n"
           "   |  _  | |_| | |_) |  __/ |   \\ V /| \\__ \\ (_) | |    |  _|| | | (_) | | | | | |  ___) | (__| | | (_| | || (__| | | |\n"
           "   |_| |_|\\__, | .__/ \\___|_|    \\_/ |_|___/\\___/|_|    |_|  |_|  \\___/|_| |_| |_| |____/ \\___|_|  \\__,_|\\__\\___|_| |_|\n"
           "          |___/|_|                                                                                                     \n"

           "\n\n");
}

int
main()
{
    std::string CpuId;

    PrintAppearance();

    CpuId = GetCpuID();

    cout << "[*] The CPU Vendor is : " << CpuId << endl;

    if (CpuId == "GenuineIntel")
    {
        cout << "[*] The Processor virtualization technology is VT-x. \n";
    }
    else
    {
        cout << "[*] This program is not designed to run in a non-VT-x environemnt !\n";
        return 1;
    }

    if (DetectVmxSupport())
    {
        cout << "[*] VMX Operation is supported by your processor .\n";
    }
    else
    {
        cout << "[*] VMX Operation is not supported by your processor .\n";
        return 1;
    }

    HANDLE hWnd = CreateFile(L"\\\\.\\MyHypervisorDevice",
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ |
                                 FILE_SHARE_WRITE,
                             NULL, /// lpSecurityAttirbutes
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL |
                                 FILE_FLAG_OVERLAPPED,
                             NULL); /// lpTemplateFile

    _getch();

    return 0;
}
