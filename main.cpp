/*
#include <iostream>
#include <windows.h>
#include <string>

int main() {

    int count = 0;

    while(true){




        if(GetKeyState('A') & 0x8000)
        {

            count++;
            std::cout << "pressed" + std::to_string(count) << std::endl;

        }

    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
*/

#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>

char keyToHook = 'Z';

// The hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        // Get the key information
        KBDLLHOOKSTRUCT* keyStruct = (KBDLLHOOKSTRUCT*)lParam;

        // Check for a specific key (e.g., 'A')
        if (keyStruct->vkCode == keyToHook) {
            if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && ((keyStruct->flags & LLKHF_UP) != 0)) {
                // The 'A' key was released, simulate an 'A' key press

                // Key down event
                INPUT input = { 0 };
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = keyToHook;
                SendInput(1, &input, sizeof(INPUT));

                // Key up event
                ZeroMemory(&input, sizeof(INPUT));
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = keyToHook;
                input.ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(1, &input, sizeof(INPUT));

                // Cancel the original key release
                return 1;
            }
            else{
                return 1;
            }
        }
    }

    // Call the next hook in the chain
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    // Set the hook

    // Read the key from the config file
    std::ifstream configFile("config.txt");
    if (configFile.is_open()) {
        std::string line;
        if (std::getline(configFile, line)) {
            size_t startPos = line.find('\'');
            size_t endPos = line.rfind('\'');
            if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
                keyToHook = line[startPos + 1];
            }
        }
        configFile.close();
    }

    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    std::cout << "Key to upstroke: " + keyToHook << std::endl;

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Remove the hook
    UnhookWindowsHookEx(keyboardHook);

    return 0;
}